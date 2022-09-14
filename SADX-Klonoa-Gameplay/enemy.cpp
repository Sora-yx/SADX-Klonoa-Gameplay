#include "pch.h"
#include "abilities.h"

ObjectFuncPtr enemyList[14] = { Kiki_Main, RhinoTank_Main, Sweep_Main, SpinnerA_Main, SpinnerB_Main, SpinnerC_Main, EPolice_Main, EBuyon,
ESman, UnidusA_Main, UnidusB_Main, UnidusC_Main, (ObjectFuncPtr)0x5B03B0, ERobo_0 };

TaskHook Kiki_Main_t((intptr_t)Kiki_Main);
TaskHook RhinoTank_t((intptr_t)RhinoTank_Main);
TaskHook Sweep_t((intptr_t)Sweep_Main);
TaskHook SpinnerA_t((intptr_t)SpinnerA_Main);
TaskHook SpinnerB_t((intptr_t)SpinnerB_Main);
TaskHook SpinnerC_t((intptr_t)SpinnerC_Main);
TaskHook EPolice_t((intptr_t)EPolice_Main);
TaskHook EBuyon_t((intptr_t)EBuyon);
TaskHook ESman_t((intptr_t)ESman);
TaskHook UnidusA_t((intptr_t)UnidusA_Main);
TaskHook UnidusB_t((intptr_t)UnidusB_Main);
TaskHook UnidusC_t((intptr_t)UnidusC_Main);
TaskHook EGacha_t((intptr_t)0x5B03B0);
TaskHook ERobo_t((intptr_t)ERobo_0);

static FunctionHook<void, int> IncrasementAct_t((intptr_t)IncrementAct);



//reset grab pointer when changing act
void IncrementAct_r(int amount)
{
	auto pnum = getKlonoaPlayer();
	if (pnum > -1) {
		auto player = playertwp[pnum];
		auto klwk = (klonoawk*)playertp[pnum]->awp;
		ResetKlonoaGrab(klwk);
	}

	return IncrasementAct_t.Original(amount);
}

static float throwSpd = 5.0f;

static bool TimingEnemyHurt(taskwk* data, klonoawk* klwk)
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
	auto a3 = a1->pos.y + 15.0f;
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

	data->cwp->info->a = 12.0f;
	data->pos.x += des.x;
	data->pos.y += des.y;
	data->pos.z += des.z;
	data->ang.x += 2048;
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
				}
				break;
			case drop:
				if (!TimingEnemyHurt(data, klwk))
				{
					data->pos.y -= throwSpd;
				}
				break;
			case throwSetup:
				data->counter.f = 4.0f; //timer
				CCL_Init(obj, (CCL_INFO*)0x981D10, 1, 4u);
				data->mode++;
				break;
			case threw:
				ThrowEnemy_Action(obj);
				break;
			case dead:
				ResetKlonoaGrab(klwk);
				DestroyEnemy(data);
				if (obj->ocp && obj->ocp->ssCondition)
					UpdateSetDataAndDelete(obj);
				else
					FreeTask(obj);
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

void EnemyCol_Fix(ObjectMaster* obj, CollisionData* collisionArray, int count, unsigned __int8 list)
{
	if (isKlonoa(klonoaPnum))
	{
		return Collision_Init(obj, collisionArray, count, 3u);
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

	IncrasementAct_t.Hook(IncrementAct_r);

	WriteCall((void*)0x4B0D17, EnemyCol_Fix);

}