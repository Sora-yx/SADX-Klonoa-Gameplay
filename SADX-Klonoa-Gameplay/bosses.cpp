#include "pch.h"
#include "abilities.h"

//series of hacks to make bosses spawn enemy so we can defeat them Klonoa style! 

TaskHook Chaos0_t((intptr_t)Chaos0_Main);
TaskHook Chaos2_t((intptr_t)Chaos2_Main);
TaskHook Chaos4_t((intptr_t)Chaos4_Main);
TaskHook Chaos6_t((intptr_t)Chaos6_Main);
TaskHook EggHornet_t((intptr_t)EggHornet_Main);
TaskHook EggViper_t((intptr_t)0x581E10);

TaskHook Knux_Main_t((intptr_t)Knuckles_Main);
TaskHook Gamma_Main_t((intptr_t)Gamma_Main);
TaskHook KnuxBossAI_t((intptr_t)0x4D5DE0);

NJS_SPRITE portalEffSprite = { 0 };
NJS_SPRITE starEffSprite = { 0 };

static FunctionHook<void, int> RunLevelDestructor_t(RunLevelDestructor);
static FunctionHook<void> sub_4C27B0_t((intptr_t)0x4C27B0);

const static int TimerPortalCD = 100;
const static int TimerMobCD = 200;
static int timer = 0;
char enemyCount = 0;

static task* enemyBossTask[6] = { nullptr };

struct EnemyBossSpawnS
{
	int currentLevel;
	NJS_POINT3 pos;
	char id;
	ObjectFuncPtr enemy;
};

const EnemyBossSpawnS EnemyBossSpawnPos[] = {
	{ LevelIDs_Chaos0, { 258.0f, 17.0f, 280.0f}, 0, SpinnerB_Main },
	{ LevelIDs_Chaos2, { 26, 19, 43}, 0, SpinnerB_Main },
	{ LevelIDs_Chaos4, { 34, 38, 166 }, 0, SpinnerB_Main },
	{ LevelIDs_Chaos4, { 36, 38, -109}, 1, SpinnerA_Main },
	{ LevelIDs_Chaos4, { 113, 38, -11}, 2, SpinnerA_Main },
	{ LevelIDs_Chaos6, { -107, 754, -504 }, 0, SpinnerB_Main },
	{ LevelIDs_Chaos6, { 151, 760, -414}, 1, SpinnerC_Main },
	{ LevelIDs_Chaos6, { -113, 754, -285 }, 2,  SpinnerA_Main },
	{ LevelIDs_EggHornet, { 1060, 135.5, 980 }, 0, SpinnerA_Main},
	{ LevelIDs_EggHornet, { 840, 135.5, 1070}, 1, SpinnerB_Main },
	{ LevelIDs_EggViper, { 219, 27, 65 }, 0, SpinnerB_Main },
	{ LevelIDs_EggViper, { 214, 27, -196}, 1, SpinnerB_Main },
	{ LevelIDs_EggViper, { -200, 33, -157}, 2, SpinnerB_Main },
	{ LevelIDs_EggViper, { -200, 33, 68}, 3, SpinnerB_Main },
};

void resetEnemyBossSpawn()
{
	timer = 0;
	enemyCount = 0;
}

char getNumberOfEnemyToSpawn()
{
	switch (CurrentLevel)
	{
	case LevelIDs_Chaos0:
		return 1;
	case LevelIDs_Chaos2:
		return 1;
	case LevelIDs_Chaos4:
		return 3;
	case LevelIDs_Chaos6:
		return 3;
	case LevelIDs_EggHornet:
		return 2;
	case LevelIDs_EggViper:
		if (EggViper_Health > 3)
			return 4;
		else
			return 0;
	}

	return 0;
}

void EnemyBoss_Delete_r(task* obj)
{
	auto task = obj;

	if (task)
	{
		char id = obj->twp->id;
		FreeTask(enemyBossTask[id]);
		timer = 0; //reset respawn timer
		enemyBossTask[id] = nullptr;
	}
}

void putTubu_r(NJS_VECTOR* pos)
{
	NJS_VECTOR dest = { 0 };

	dest.z = 8.0f;
	njPushMatrix(nj_unit_matrix_);
	njCalcVector(0, &dest, &dest);
	njPopMatrix(1u);

	portalEffSprite.p.x = dest.x + pos->x;
	portalEffSprite.p.y = dest.y + pos->y;
	portalEffSprite.p.z = dest.z + pos->z;

	njDrawSprite3D_Queue(
		&portalEffSprite,
		(unsigned __int64)0,
		NJD_SPRITE_ALPHA | NJD_SPRITE_COLOR | NJD_SPRITE_SCALE,
		QueuedModelFlagsB_SomeTextureThing);
}

void DrawAncienLight_Sprite(NJS_VECTOR* pos)
{
	starEffSprite.p = *pos;
	char frame = (char)GetFrameCounter();
	njDrawSprite3D_Queue(&starEffSprite, frame & 7, NJD_SPRITE_ALPHA | NJD_SPRITE_SCALE | NJD_SPRITE_COLOR, QueuedModelFlagsB_SomeTextureThing);
}

static void DrawPortalEffect(NJS_VECTOR* pos)
{
	SetMaterialAndSpriteColor_Float(1.0f, 0.925f, 0.180f, 1.0f);
	njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
	njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);

	DrawAncienLight_Sprite(pos); //portal effect

	for (int i = 0; i < 4; ++i)
	{
		putTubu_r(pos); //stars
	}

	njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
	njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
	ResetMaterial();
}

static void CreateEnemyPortal(char id)
{
	for (int i = 0; i < LengthOfArray(EnemyBossSpawnPos); i++)
	{
		if (CurrentLevel == EnemyBossSpawnPos[i].currentLevel)
		{
			if (EnemyBossSpawnPos[i].id == id && !enemyBossTask[id]) {

				auto pos = EnemyBossSpawnPos[i].pos;
				pos.y += 3.0f;
				DrawPortalEffect(&pos);
			}
		}
	}
}

static void CreateEnemy(char id)
{
	for (int i = 0; i < LengthOfArray(EnemyBossSpawnPos); i++)
	{
		if (CurrentLevel == EnemyBossSpawnPos[i].currentLevel)
		{
			if (EnemyBossSpawnPos[i].id == id && !enemyBossTask[id]) {

				enemyBossTask[id] = CreateElementalTask(2, 3, (TaskFuncPtr)EnemyBossSpawnPos[i].enemy);

				if (enemyBossTask[id]) {
					enemyBossTask[id]->dest = EnemyBoss_Delete_r;
					enemyBossTask[id]->twp->pos = EnemyBossSpawnPos[i].pos;
					enemyBossTask[id]->twp->id = id;
				}
			}
		}
	}
}

static void SpawnEnemyCheck()
{
	if (!IsIngame() || getKlonoaPlayer() < 0 || EV_MainThread_ptr)
		return;

	if (Life_Max <= 0.0f)
	{
		for (int i = 0; i < LengthOfArray(enemyBossTask); i++)
		{
			if (enemyBossTask[i])
			{
				FreeTask(enemyBossTask[i]);
			}
		}

		return;
	}

	char nbEnemy = getNumberOfEnemyToSpawn();

	if (enemyCount >= nbEnemy || !ControlEnabled)
		return;

	if (timer >= TimerPortalCD && timer < TimerMobCD)
	{
		for (char i = 0; i < nbEnemy; i++)
		{
			CreateEnemyPortal(i);
		}
	}

	if (timer == TimerMobCD)
	{
		timer = 0;

		for (char i = 0; i < nbEnemy; i++)
		{
			CreateEnemy(i);
		}

		return;
	}

	timer++;
}

void chaos0_r(task* obj)
{
	SpawnEnemyCheck();
	Chaos0_t.Original(obj);
}

void chaos2_r(task* obj)
{
	SpawnEnemyCheck();
	Chaos2_t.Original(obj);
}

static float one = 1.0f;
static float two = 2.0f;
static intptr_t Chaos4Damage = 0x55560f;

void chaos4_r(task* obj)
{
	auto data = obj->twp;

	if (!data->mode)
	{
		for (int i = 0; i < PMax; i++) {

			if (isKlonoa(i))
			{
				WriteData((float**)Chaos4Damage, &two);
				break;
			}
			else
			{
				WriteData((float**)Chaos4Damage, &one);
			}
		}
	}

	SpawnEnemyCheck();
	Chaos4_t.Original(obj);
}

void chaos6_r(task* obj)
{
	SpawnEnemyCheck();
	Chaos6_t.Original(obj);
}

void eggHornet_r(task* obj)
{
	SpawnEnemyCheck();
	EggHornet_t.Original(obj);
}

void eggViper_r(task* obj)
{
	SpawnEnemyCheck();
	EggViper_t.Original(obj);
}


static float dropSpd = 5.0f;

static bool BTimingEnemyHurt(taskwk* data)
{
	if (data && --data->timer.b[0] <= 0)
	{
		data->mode = bDone;
		return true;
	}

	return false;
}

void BThrowEnemy_Action(task* tp)
{
	auto data = tp->twp;
	int timer = data->timer.b[0]--;

	if (timer <= 0.0f)
	{
		data->mode = bDone;
		return;
	}

	auto wk = (playerwk*)tp->mwp->work.l;
	if (wk)
	{
		njAddVector(&data->pos, &wk->acc);
	}
}

static bool CharacterCapturedHandle(task* obj)
{
	auto data = obj->twp;

	if (data)
	{
		bool Enabled = data->mode >= Bcaptured && data->mode <= bDone;

		if (Enabled)
		{
			auto pnum = 0;
			auto player = playertwp[pnum];
			auto klwk = (klonoawk*)playertp[pnum]->awp;

			if (!player || !klwk)
				return false;

			data->ang.x -= 1524;

			switch (data->mode)
			{
			case Bcaptured:
				pnum = data->btimer;

				if (obj->exec == (TaskFuncPtr)Knuckles_Main)
				{
					PlayVoice(1011);
					EV_SetAction(obj, KNUCKLES_ACTIONS[6], &KNUCKLES_TEXLIST, 0.80000001f, 3, 0);
				}

				if (obj->exec == (TaskFuncPtr)Gamma_Main)
				{
					PlayVoice(1212);
					EV_SetAction(obj, E102_ACTIONS[16], &E102_TEXLIST, 0.80000001f, 3, 0);
				}

				data->mode++;
				break;
			case Bcaptured2:
				data->ang.y = player->ang.y;
				if (player && isKlonoa(pnum))
				{
					data->pos = { player->pos.x, player->pos.y + 16.0f, player->pos.z };
				}
				break;
			case Bdrop:
				if (!BTimingEnemyHurt(data))
				{
					data->pos.y -= dropSpd;
				}
				break;
			case BthrowSetup:
				data->timer.b[0] = 20;
				data->mode++;
				break;
			case Bthrew:
				BThrowEnemy_Action(obj);
				break;
			case bDone:
			default:
				EV_ClrAction(obj);
				player->mode = 1;

				data->pos = player->pos;
				if ((player->flag & 3) == 0)
				{
					data->pos.y -= 2.0f;
				}
				data->pos.x += 15.0f;
				data->pos.z += 10.0f;

				data->mode = 1;
				data->flag |= Status_Hurt;
				break;
			}

			return true;
		}
	}

	return false;
}

void Knuckles_Main_r(task* obj)
{
	if (CharacterBossActive)
	{
		int isKlo = getKlonoaPlayer();
		if (isKlo >= 0)
		{
			if (CharacterCapturedHandle(obj))
			{
				obj->disp(obj);
				return;
			}

		}
	}

	Knux_Main_t.Original(obj);
}

void Gamma_Main_r(task* obj)
{
	if (CharacterBossActive)
	{
		if (getKlonoaPlayer() >= 0)
		{
			CharacterCapturedHandle(obj);
		}
	}

	Gamma_Main_t.Original(obj);
}

void sub_4C27B0_r()
{
	int pnum = getKlonoaPlayer();
	if (pnum >= 0)
	{
		if (playertwp[1])
		{
			if (playertwp[1]->mode >= Bcaptured)
			{
				CameraReleaseEventCamera();
				return;
			}
		}
	}

	sub_4C27B0_t.Original();
}

void RunLevelDestructor_r(int a1)
{
	if (!a1)
		resetEnemyBossSpawn();

	RunLevelDestructor_t.Original(a1);
}

void RemoveEnemyBounceThing(unsigned __int8 playerID, float speedX, float speedY, float speedZ)
{
	if (isKlonoa(playerID))
	{
		return;
	}

	return EnemyBounceThing(playerID, speedX, speedY, speedZ);
}

void init_BossesHacks()
{
	if (!allowKlonoaMoves)
		return;

	//copy ancient light sprites then edit that copy to create custom portal effects used for enemy spawn
	memcpy(&portalEffSprite, &AncientLightSprite, sizeof(NJS_SPRITE));
	memcpy(&starEffSprite, &AncientLightSprite2, sizeof(NJS_SPRITE));

	portalEffSprite.sx += 1.0f;
	portalEffSprite.sy += 1.0f;
	starEffSprite.sx += 1.0f;
	starEffSprite.sy += 1.0f;

	Chaos0_t.Hook(chaos0_r);
	Chaos2_t.Hook(chaos2_r);
	Chaos4_t.Hook(chaos4_r);
	Chaos6_t.Hook(chaos6_r);
	EggHornet_t.Hook(eggHornet_r);
	EggViper_t.Hook(eggViper_r);

	Knux_Main_t.Hook(Knuckles_Main_r);
	Gamma_Main_t.Hook(Gamma_Main_r);
	sub_4C27B0_t.Hook(sub_4C27B0_r);

	RunLevelDestructor_t.Hook(RunLevelDestructor_r);

	//remove bounce after hitting a boss
	WriteCall((void*)0x571D36, RemoveEnemyBounceThing);	 //chaos 0
	WriteCall((void*)0x54A401, RemoveEnemyBounceThing); //egg hornet
	WriteCall((void*)0x54C98E, RemoveEnemyBounceThing); //chaos 2
	WriteCall((void*)0x551FD2, RemoveEnemyBounceThing); //chaos 4	
	WriteCall((void*)0x559399, RemoveEnemyBounceThing); //chaos 6	
	WriteCall((void*)0x580C7E, RemoveEnemyBounceThing); //egg viper
	WriteCall((void*)0x580BED, RemoveEnemyBounceThing); //egg viper	
	WriteCall((void*)0x580D2C, RemoveEnemyBounceThing); //egg viper


}