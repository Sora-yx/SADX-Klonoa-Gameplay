#include "pch.h"

//series of hacks to make bosses spawn enemy so we can defeat them Klonoa style! 

TaskHook Chaos0_t((intptr_t)Chaos0_Main);
TaskHook Chaos2_t((intptr_t)Chaos2_Main);
TaskHook Chaos4_t((intptr_t)Chaos4_Main);
TaskHook Chaos6_t((intptr_t)Chaos6_Main);
TaskHook EggHornet_t((intptr_t)EggHornet_Main);
TaskHook EggViper_t((intptr_t)0x581E10);

static FunctionHook<void, int> RunLevelDestructor_t(RunLevelDestructor);

const static int TimerCD = 130;
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
	{ LevelIDs_Chaos0, { 258.0f, 17.0f, 264.0f}, 0, SpinnerB_Main },
	{ LevelIDs_Chaos2, { 26, 19, 43}, 0, SpinnerB_Main },
	{ LevelIDs_Chaos4, { 34, 38, 166 }, 0, SpinnerB_Main },
	{ LevelIDs_Chaos4, { 36, 38, -109}, 1, SpinnerA_Main },
	{ LevelIDs_Chaos6, { -107, 754, -504 }, 0, SpinnerB_Main },
	{ LevelIDs_Chaos6, { 151, 760, -414}, 1, SpinnerC_Main },
	{ LevelIDs_Chaos6, { -113, 754, -285 }, 2,  SpinnerA_Main },
	{ LevelIDs_EggHornet, { 1060, 129.5, 980 }, 0, SpinnerA_Main},
	{ LevelIDs_EggHornet, { 840, 129.5, 1070}, 1, SpinnerB_Main },
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
		return 2;
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
	if (!IsIngame() || !isKlonoa(klonoaPnum))
		return;

	if (Life_Max <= 0.0f && !TimeThing)
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

	if (enemyCount >= nbEnemy)
		return;

	if (++timer == TimerCD)
	{
		timer = 0;

		for (char i = 0; i < nbEnemy; i++)
		{
			CreateEnemy(i);
		}

		return;
	}
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

void chaos4_r(task* obj)
{
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

	Chaos0_t.Hook(chaos0_r);
	Chaos2_t.Hook(chaos2_r);
	Chaos4_t.Hook(chaos4_r);
	Chaos6_t.Hook(chaos6_r);
	EggHornet_t.Hook(eggHornet_r);
	EggViper_t.Hook(eggViper_r);
	RunLevelDestructor_t.Hook(RunLevelDestructor_r);

	//remove bounce after hitting a boss
	WriteCall((void*)0x571D36, RemoveEnemyBounceThing);	
	WriteCall((void*)0x54C98E, RemoveEnemyBounceThing);	
	WriteCall((void*)0x799C4E, RemoveEnemyBounceThing);	
	WriteCall((void*)0x551FD2, RemoveEnemyBounceThing); //chaos 4	
	WriteCall((void*)0x559399, RemoveEnemyBounceThing); //chaos 6	
	WriteCall((void*)0x580C7E, RemoveEnemyBounceThing); //egg viper
	WriteCall((void*)0x580BED, RemoveEnemyBounceThing); //egg viper	
	WriteCall((void*)0x580D2C, RemoveEnemyBounceThing); //egg viper
}