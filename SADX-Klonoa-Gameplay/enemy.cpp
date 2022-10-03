#include "pch.h"
#include "abilities.h"


ObjectFuncPtr enemyList[] = { Kiki_Main, RhinoTank_Main, Sweep_Main, SpinnerA_Main, SpinnerB_Main, SpinnerC_Main, EPolice_Main, EBuyon,
ESman, UnindusA_Exec, UnindusB_Exec, UnindusC_Exec, (ObjectFuncPtr)0x5B03B0, (ObjectFuncPtr)0x4A6420, Leon_Main, OMonkeyCage };

const char enemyArraySize = LengthOfArray(enemyList);

TaskHook Kiki_Main_t((intptr_t)Kiki_Main);
TaskHook RhinoTank_t((intptr_t)RhinoTank_Main);
TaskHook Sweep_t((intptr_t)Sweep_Main);
TaskHook SpinnerA_t((intptr_t)SpinnerA_Main);
TaskHook SpinnerB_t((intptr_t)SpinnerB_Main);
TaskHook SpinnerC_t((intptr_t)SpinnerC_Main);
TaskHook EPolice_t((intptr_t)EPolice_Main);
TaskHook EBuyon_t((intptr_t)EBuyon);
TaskHook ESman_t((intptr_t)0x4C8DD0); //ESman
TaskHook UnidusA_t((intptr_t)UnindusA_Exec);
TaskHook UnidusB_t((intptr_t)UnindusB_Exec);
TaskHook UnidusC_t((intptr_t)UnindusC_Exec); //lava mob RM
TaskHook EGacha_t((intptr_t)0x5B03B0);
TaskHook ERobo_t((intptr_t)0x4A6420);
TaskHook Leon_t((intptr_t)Leon_Main);
TaskHook OMonkeyCage_t((intptr_t)OMonkeyCage);

static FunctionHook<void, int> IncrementAct_t((intptr_t)IncrementAct);

TaskHook RingLineV_t((intptr_t)0x7AC180);
TaskHook RingLine_t((intptr_t)0x7ABC30);
TaskHook UpdateSetAndDelete_t(UpdateSetDataAndDelete);

task* EnemyLineV = nullptr;
task* EnemyLine = nullptr;

//a collision can only hit another one depending on the list they are using, so we change the one used when we throw an enemy so it can hit another one.
char GetColListIDForThrowEnemy()
{
	if (isBossLevel())
		return 0;

	return 4;
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

	if (hasLightShoes(klonoaPnum) && child && data->mode == 1)
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

	if (hasLightShoes(klonoaPnum) && child)
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
	auto pnum = getKlonoaPlayer();
	if (pnum > -1) {
		auto player = playertwp[pnum];
		auto klwk = (klonoawk*)playertp[pnum]->awp;
		ResetKlonoaGrab(klwk);
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

void DestroyEnemy(taskwk* a1)
{
	auto a2 = a1->pos.y + 5.0f;
	CreateFlash2(a1->pos.x, a2, a1->pos.z, 1.4);
	CreateExpSpring(a1, 8u);
	auto a3 = a1->pos.y + 14.0f;

	if (CurrentLevel < LevelIDs_Chaos0)
		CreateAnimal(rand() % 15, a1->pos.x, a3, a1->pos.z);
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

	if (CurrentLevel != LevelIDs_Chaos4)
		data->cwp->info->a = 13.0f;
	else
		data->cwp->info->a = 30.0f;

	data->cwp->info->damage |= 3u;
	data->cwp->info->damage |= 0xCu;
	njAddVector(&data->pos, &des);
	data->ang.x += 2048;

	if (data->cwp && data->cwp->hit_cwp && data->cwp->hit_cwp->mytask)
	{
		auto task = data->cwp->hit_cwp->mytask;

		if (task->twp && task->twp->cwp->id != 0)
		{
			data->counter.f = 0.083333336f * 7;
		}

	}

	EntryColliList(data);
}

static bool EnemyCapturedHandle(task* obj)
{
	auto data = obj->twp;

	if (data)
	{
		bool Enabled = data->mode >= captured;

		if (Enabled)
		{
			auto pnum = getKlonoaPlayer();

			if (pnum < 0)
				return false;

			auto player = playertwp[pnum];
			auto klwk = (klonoawk*)playertp[pnum]->awp;

			if (!player || !klwk)
				return false;

			data->ang.y += 1024;


			switch (data->mode)
			{
			case captured:
				if (player && isKlonoa(pnum))
				{
					data->pos = player->pos;
					data->pos.y += 16.0f;
					data->ang.x = player->ang.x;
					data->ang.z = player->ang.z;
				}
				break;
			case drop:
				if (!TimingEnemyHurt(data))
				{
					data->pos.y -= dropSpd;
				}
				break;
			case throwSetup:
				data->counter.f = 5.0f; //timer
				CCL_Init(obj, (CCL_INFO*)0x981D10, 1, GetColListIDForThrowEnemy());
				data->mode++;
				break;
			case threw:
				ThrowEnemy_Action(obj);
				break;
			case dead:
				ResetKlonoaGrab(klwk);
				DestroyEnemy(data);
				UpdateSetDataAndDelete(obj);
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


void OMonkeyCage_r(task* obj)
{
	auto data = obj->twp;
	auto pnum = getKlonoaPlayer();
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
	if (isKlonoa(klonoaPnum))
	{
		return Collision_Init(obj, collisionArray, count, 4u);
	}

	return Collision_Init(obj, collisionArray, count, list);
}

void init_EnemiesHack()
{
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

	IncrementAct_t.Hook(IncrementAct_r);

	RingLineV_t.Hook(RingLineV_r);
	RingLine_t.Hook(RingLine_r);
	OMonkeyCage_t.Hook(OMonkeyCage_r);
	UpdateSetAndDelete_t.Hook(UpdateSetAndDelete_r);

	//change spinner col list so they can be grabbed
	WriteCall((void*)0x4B0D17, EnemyCol_Fix);
	WriteCall((void*)0x540684, EnemyCol_Fix);
}