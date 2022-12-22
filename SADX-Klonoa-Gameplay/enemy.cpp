#include "pch.h"
#include "abilities.h"

ObjectFuncPtr enemyList[] = { Kiki_Main, RhinoTank_Main, Sweep_Main, SpinnerA_Main, SpinnerB_Main, SpinnerC_Main, EPolice_Main, EBuyon,
ESman, UnindusA_Exec, UnindusB_Exec, UnindusC_Exec, (ObjectFuncPtr)0x5B03B0, (ObjectFuncPtr)0x4A6420, OMonkeyCage, (ObjectFuncPtr)ESman_Exec };

const char enemyArraySize = LengthOfArray(enemyList);
static const float ColCrashThrowTimer = 0.083333336f * 7; //small timer to make the enemy die almost instantly when they hit each other

TaskHook Kiki_Main_t((intptr_t)Kiki_Main);
TaskHook RhinoTank_t((intptr_t)RhinoTank_Main);
TaskHook Sweep_t((intptr_t)Sweep_Main);
TaskHook SpinnerA_t((intptr_t)SpinnerA_Main);
TaskHook SpinnerB_t((intptr_t)SpinnerB_Main);
TaskHook SpinnerC_t((intptr_t)SpinnerC_Main);
TaskHook EPolice_t((intptr_t)EPolice_Main);
TaskHook EBuyon_t((intptr_t)EBuyon);
TaskHook ESman_t((intptr_t)ESman_Exec);
TaskHook UnidusA_t((intptr_t)UnindusA_Exec);
TaskHook UnidusB_t((intptr_t)UnindusB_Exec);
TaskHook UnidusC_t((intptr_t)UnindusC_Exec); //lava mob RM
TaskHook EGacha_t((intptr_t)0x5B03B0);
TaskHook ERobo_t((intptr_t)0x4A6420);
TaskHook ELeon_t((intptr_t)Leon_Main);
TaskHook OMonkeyCage_t((intptr_t)OMonkeyCage);

static FunctionHook<void, int> IncrementAct_t((intptr_t)IncrementAct);

TaskHook RingLineV_t((intptr_t)0x7AC180);
TaskHook RingLine_t((intptr_t)0x7ABC30);
TaskHook UpdateSetAndDelete_t(UpdateSetDataAndDelete);

task* EnemyLineV = nullptr;
task* EnemyLine = nullptr;

DataPointer(CCL_INFO, stru_981D10, 0x981D10);

//a collision can only hit another one depending on the list they are using, so we change the one used when we throw an enemy so it can hit another one.
char GetColListIDForThrowEnemy()
{
	if (isBossLevel())
		return 0;

	return 4;
}

static void SetDamageCol(taskwk* twp)
{
	if (CurrentLevel != LevelIDs_Chaos4)
		twp->cwp->info->a = 13.0f;
	else
		twp->cwp->info->a = 35.0f;

	twp->cwp->info->damage |= 3u;
	twp->cwp->info->damage |= 0xCu;
}

static void ChildDamageCol(task* obj);
static bool SetCrashCol(taskwk* data)
{
	if (data->cwp && data->cwp->hit_cwp && data->cwp->hit_cwp->mytask)
	{
		auto task = data->cwp->hit_cwp->mytask;

		if (task->exec == ChildDamageCol || task->ctp && task->ctp->exec == ChildDamageCol)
			return false;

		if (task->twp && task->twp->cwp->id != 0) //if the enemy hit another col type that isn't a player
		{
			return true;
		}
	}

	return false;
}

//used to add an extra col to damage other enemies with throw
static void ChildDamageCol(task* obj)
{
	auto twp = obj->twp;
	twp->pos = obj->ptp->twp->pos;

	if (!twp->mode)
	{
		CCL_Init(obj, &stru_981D10, 1, 1u);
		twp->mode++;
	}

	if (twp->cwp)
	{
		SetDamageCol(twp);

		if (SetCrashCol(twp))
		{
			auto timer = obj->ptp->twp->counter.f;

			if (timer > ColCrashThrowTimer)
				obj->ptp->twp->counter.f = ColCrashThrowTimer;
		}

		EntryColliList(twp);
	}
}

void UpdateSetAndDelete_r(task* obj)
{
	if (!obj)
		return;

	if (obj->ocp && obj->ocp->ssCondition)
	{
		return UpdateSetAndDelete_t.Original(obj);
	}

	obj->exec = DestroyTask;
}

void Enemy_DeleteLineV_r(task* obj)
{
	auto task = EnemyLineV;
	if (task)
	{
		FreeTask(task);
	}

	EnemyLineV = nullptr;
	Enemy_Delete((ObjectMaster*)obj);
}

void CreateEnemyLineVSpawn(task* obj)
{
	if (!EnemyLineV)
	{
		EnemyLineV = CreateElementalTask(2, 3, (TaskFuncPtr)SpinnerB_Main);
		EnemyLineV->dest = Enemy_DeleteLineV_r;
		EnemyLineV->twp->pos = obj->twp->pos;
		EnemyLineV->twp->pos.y += 7.0f;
	}
}

void Enemy_DeleteLine_r(task* obj)
{
	auto task = EnemyLineV;
	if (task)
	{
		FreeTask(task);
	}

	EnemyLine = nullptr;
	Enemy_Delete((ObjectMaster*)obj);
}

void CreateEnemyLineSpawn(task* obj)
{
	if (!EnemyLine)
	{
		EnemyLine = CreateElementalTask(2, 3, (TaskFuncPtr)SpinnerB_Main);
		EnemyLine->dest = Enemy_DeleteLine_r;
		EnemyLine->twp->pos = obj->twp->pos;
		EnemyLine->twp->pos.y += 7.0f;
	}
}

void RingLine_r(task* obj)
{
	auto data = obj->twp;
	auto child = obj->ctp;

	if (hasLightShoes(getKlonoaPlayer()) && child && data->mode == 1)
	{
		if (child->next && child->next->next && child->next->next->next)
		{
			auto task = child->next->next->next;

			if (task && task->twp) {
				CreateEnemyLineSpawn(obj);
			}
		}
	}

	RingLine_t.Original(obj);
}

void RingLineV_r(task* obj)
{
	auto data = obj->twp;
	auto child = obj->ctp;

	if (hasLightShoes(getKlonoaPlayer()) && child)
	{
		if (child->next && child->next->next && child->next->next->next)
		{
			auto task = child->next->next->next;

			if (task && task->twp && data->mode == 3) {
				CreateEnemyLineVSpawn(obj);
			}
		}
	}

	RingLineV_t.Original(obj);
}

//reset grab pointer when changing act
void IncrementAct_r(int amount)
{
	for (uint8_t i = 0; i < PMax; i++)
	{
		if (isKlonoa(i))
		{
			auto player = playertwp[i];
			auto klwk = (klonoawk*)playertp[i]->awp;
			ResetKlonoaGrab(klwk);
		}
	}

	FreeAllCustomSounds();

	return IncrementAct_t.Original(amount);
}

static float dropSpd = 5.0f;

static bool TimingEnemyHurt(taskwk* data)
{
	if (data && --data->wtimer <= 0)
	{
		data->flag |= Status_Hurt;
		data->mode = dead;
		return true;
	}

	return false;
}

void KillLeon(taskwk* data1)
{
	if (data1->mode > 5)
	{
		data1->flag &= 0xFBu;
		data1->mode = 5;
		data1->counter.b[1] = 0;
		data1->counter.b[2] = 1;
		data1->scl.z = 0.34999999f;
	}
}

void KillKiki(taskwk* data1)
{
	if (data1->mode > 12)
	{
		data1->mode = 12;
		data1->counter.b[1] = 0;
		data1->counter.b[2] = 1;
	}
}

void KillSpinner(taskwk* data1)
{
	if (data1->mode > 5)
	{
		auto a2 = data1->pos.y + 5.0f;
		CreateFlash2(data1->pos.x, a2, data1->pos.z, 1.4);
		CreateExpSpring(data1, 8u);
		data1->mode = 5;
	}
}

void KillEPolice(taskwk* data1)
{
	if (data1->mode > 6)
	{
		data1->mode = 6;
	}
}

void DestroyEnemy(task* tp, taskwk* data)
{
	auto target = (intptr_t)tp->exec;
	if (CurrentLevel < LevelIDs_Chaos0)
	{
		if (target == (intptr_t)Leon_Main)
		{
			KillLeon(data);
			return;
		}

		if (target == (intptr_t)Kiki_Main)
		{
			KillKiki(data);
			return;
		}

		if (target == (intptr_t)SpinnerA_Main || target == (intptr_t)SpinnerB_Main || target == (intptr_t)SpinnerC_Main)
		{
			KillSpinner(data);
			return;
		}

		if (target == (intptr_t)&EPolice_Main)
		{
			KillEPolice(data);
			return;
		}
	}

	auto a2 = data->pos.y + 5.0f;
	CreateFlash2(data->pos.x, a2, data->pos.z, 1.4);
	CreateExpSpring(data, 8u);
	auto a3 = data->pos.y + 14.0f;

	if (CurrentLevel < LevelIDs_Chaos0)
		CreateAnimal(rand() % 15, data->pos.x, a3, data->pos.z);
}


void ThrowEnemyToBoss_Action(task* tp)
{
	float timer = 0.0f;
	auto data = tp->twp;
	auto wk = (enemywk*)tp->mwp;
	auto des = wk->home;
	timer = (data->counter.f - 0.083333336f);
	data->counter.f = data->counter.f - 0.083333336f;

	if (timer <= 0.0f)
	{
		data->mode = dead;
		return;
	}

	SetDamageCol(data);
	njAddVector(&data->pos, &des);
	data->ang.x += 2048;


	EntryColliList(data);
}


void ThrowEnemy_Action(task* tp)
{
	float timer = 0.0f;
	auto data = tp->twp;
	auto wk = (enemywk*)tp->mwp;
	auto des = wk->home;
	timer = (data->counter.f - 0.083333336f);
	data->counter.f = data->counter.f - 0.083333336f;

	if (timer <= 0.0f)
	{
		data->mode = dead;
		return;
	}

	njAddVector(&data->pos, &des);
	data->ang.x += 2048;


	if (data->cwp && data->cwp->hit_cwp && data->cwp->hit_cwp->mytask)
	{
		if (data->counter.f > ColCrashThrowTimer)
			data->counter.f = ColCrashThrowTimer;

		if (data->cwp->hit_cwp->mytask->twp)
			data->cwp->hit_cwp->mytask->twp->flag |= Status_Hurt;
	}
}

static bool EnemyCapturedHandle(task* obj)
{
	auto data = obj->twp;

	if (data)
	{
		bool Enabled = data->mode >= captured;

		if (Enabled)
		{
			auto pnum = data->smode;

			if (!isKlonoa(pnum))
			{
				data->mode = 1;
				return false;
			}

			auto player = playertwp[pnum];
			auto klwk = (klonoawk*)playertp[pnum]->awp;

			if (!klwk)
			{
				return false;
			}

			data->ang.y += 1024;

			switch (data->mode)
			{
			case captured:
				if (player)
				{
					if (player->mode <= 2)
					{
						player->mode = act_holdStd;
					}

					data->pos = player->pos;
					data->pos.y += 16.0f;
					data->ang.x = player->ang.x;
					data->ang.z = player->ang.z;
				}
				break;
			case dropSetup:
				data->counter.f = 5.0f; //timer
				CCL_Init(obj, &stru_981D10, 1, GetColListIDForThrowEnemy());
				data->mode = drop;
				break;
			case drop:
				if (!TimingEnemyHurt(data))
				{
					data->pos.y -= dropSpd;
				}
				break;
			case throwSetup:
				data->counter.f = 5.0f; //timer
				CCL_Init(obj, &stru_981D10, 1, GetColListIDForThrowEnemy());
				data->mode++;
				break;
			case threw:
				if (isBossLevel())
					ThrowEnemyToBoss_Action(obj);
				else
					ThrowEnemy_Action(obj);

				break;
			case dead:
				AddEnemyScore(10);
				DestroyEnemy(obj, data);
				UpdateSetDataAndDelete(obj);
				ResetKlonoaGrab(klwk);
				break;
			}

			if (obj->disp)
			{
				obj->disp(obj);
			}

			return true;
		}
	}

	return false;
}

void ERobo_r(task* obj)
{
	if (!EnemyCapturedHandle(obj))
	{
		ERobo_t.Original(obj);
	}
}

void EGacha_r(task* obj)
{
	if (!EnemyCapturedHandle(obj))
	{
		EGacha_t.Original(obj);
	}
}

void UnidusA_r(task* obj)
{
	if (!EnemyCapturedHandle(obj))
	{
		UnidusA_t.Original(obj);
	}
}

void UnidusB_r(task* obj)
{
	if (!EnemyCapturedHandle(obj))
	{
		UnidusB_t.Original(obj);
	}
}

void UnidusC_r(task* obj)
{
	if (!EnemyCapturedHandle(obj))
	{
		UnidusC_t.Original(obj);
	}
}

void EBuyon_r(task* obj)
{
	if (!EnemyCapturedHandle(obj))
	{
		EBuyon_t.Original(obj);
	}
}

void EPolice_r(task* obj)
{
	if (!EnemyCapturedHandle(obj))
	{
		EPolice_t.Original(obj);
	}
}

void SpinnerA_r(task* obj)
{
	if (!EnemyCapturedHandle(obj))
	{
		SpinnerA_t.Original(obj);
	}
}

void SpinnerB_r(task* obj)
{
	if (!EnemyCapturedHandle(obj))
	{
		SpinnerB_t.Original(obj);
	}
}

void SpinnerC_r(task* obj)
{
	if (!EnemyCapturedHandle(obj))
	{
		SpinnerC_t.Original(obj);
	}
}

void kiki_r(task* obj)
{
	if (!EnemyCapturedHandle(obj))
	{
		Kiki_Main_t.Original(obj);
	}
}

void rhino_r(task* obj)
{
	if (!EnemyCapturedHandle(obj))
	{
		RhinoTank_t.Original(obj);
	}
}

void Sweep_r(task* obj)
{
	if (!EnemyCapturedHandle(obj))
	{
		Sweep_t.Original(obj);
	}
}

void ESman_r(task* obj)
{
	if (!EnemyCapturedHandle(obj))
	{
		ESman_t.Original(obj);
	}
}

void ELeon_r(task* obj)
{
	if (!EnemyCapturedHandle(obj))
	{
		ELeon_t.Original(obj);
	}
}

void OMonkeyCage_r(task* obj)
{
	auto data = obj->twp;
	auto pnum = data->smode;
	bool Enabled = data->mode >= captured;

	if (data->mode == captured) {
		data->flag |= Status_Hurt;
		data->mode = 1;
		obj->disp(obj);
		return;
	}

	if (pnum < 0 || !Enabled)
	{
		return OMonkeyCage_t.Original(obj);
	}
}

void EnemyCol_Fix(ObjectMaster* obj, CollisionData* collisionArray, int count, unsigned __int8 list)
{
	for (int i = 0; i < PMax; i++)
	{
		if (isKlonoa(i))
		{
			return Collision_Init(obj, collisionArray, count, 4u);
		}
	}

	return Collision_Init(obj, collisionArray, count, list);
}

void init_EnemiesHack()
{
	IncrementAct_t.Hook(IncrementAct_r);

	if (!allowKlonoaMoves)
		return;

	Kiki_Main_t.Hook(kiki_r);
	RhinoTank_t.Hook(rhino_r);
	Sweep_t.Hook(Sweep_r);
	SpinnerA_t.Hook(SpinnerA_r);
	SpinnerB_t.Hook(SpinnerB_r);
	SpinnerC_t.Hook(SpinnerC_r);
	EPolice_t.Hook(EPolice_r);
	EBuyon_t.Hook(EBuyon_r);
	ESman_t.Hook(ESman_r);
	UnidusA_t.Hook(UnidusA_r);
	UnidusB_t.Hook(UnidusB_r);
	UnidusC_t.Hook(UnidusC_r);
	EGacha_t.Hook(EGacha_r);
	ERobo_t.Hook(ERobo_r);
	ELeon_t.Hook(ELeon_r);

	RingLineV_t.Hook(RingLineV_r);
	RingLine_t.Hook(RingLine_r);
	OMonkeyCage_t.Hook(OMonkeyCage_r);
	UpdateSetAndDelete_t.Hook(UpdateSetAndDelete_r);

	//change spinner col list so they can be grabbed
	WriteCall((void*)0x4B0D17, EnemyCol_Fix);
	WriteCall((void*)0x540684, EnemyCol_Fix);
}