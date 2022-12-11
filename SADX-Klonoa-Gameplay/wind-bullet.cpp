#include "pch.h"


CCL_INFO bullet_col = { 0, 0, 0x70, 0x40, 0x400, { 0 }, 12.0f, 0.0, 0.0, 0.0, 0, 0, 0 };

extern ObjectFuncPtr enemyList[];

signed int KlonoaWBullet_CheckInput(taskwk* data, playerwk* co2, klonoawk* klwk)
{
	if ((AttackButtons & Controllers[data->charIndex].PressedButtons) == 0 || !allowKlonoaMoves || klwk->currentBulletPtr)
	{
		return 0;
	}


	bool isOnGround = (data->flag & 3);
	data->mode = isOnGround ? act_windBullet : act_windBulletAir;
	co2->mj.reqaction = isOnGround ? anm_windBullet : anm_windBulletAir;

	if (co2->spd.x > 0.5f && isOnGround)
		co2->spd.x = 0.5f;

	co2->spd.y = 0.0f;

	PlayCustomSound(se_shot);
	return 1;
}

bool isTargetAnEnemy(task* enemy) {

	for (int i = 0; i < enemyArraySize; ++i) {
		if (enemyList[i] == (ObjectFuncPtr)enemy->exec) {
			return true;
		}
	}

	return false;
}

signed int WindBullet_CheckHitEnemy(taskwk* bulletData, klonoawk* klwk, playerwk* co2)
{
	if (!bulletData)
		return 0;

	auto data = GetClosestEnemyCol(bulletData);

	if (data && data->cwp)
	{
		if (isTargetAnEnemy(data->cwp->mytask)) //check if it's an enemy...
		{
			if (data->cwp->mytask->exec != (TaskFuncPtr)OMonkeyCage)
			{
				data->mode = captured; //set the enemy to a new custom state, see "enemy.cpp"
				data->smode = bulletData->smode; //store pNum
				klwk->enemyGrabPtr = data->cwp->mytask; //we copy the task of the enemy for external use with Klonoa.
				return 1;
			}
			else
			{
				if ((co2->equipment & Upgrades_AncientLight)) //can only destroy monkey cage with the ancien light
				{
					data->mode = captured;
					data->smode = bulletData->smode; //store pNum
					return 1;
				}
			}
		}
	}

	return 0;
}

bool isTargetCharBoss(task* enemy) {

	if (CharacterBossActive) {
		if ((ObjectFuncPtr)enemy->exec == Knuckles_Main || (ObjectFuncPtr)enemy->exec == Gamma_Main) {
			return true;
		}
	}

	return false;
}

signed int WindBullet_CheckHitCharBoss(taskwk* bulletData, klonoawk* klwk)
{
	if (!bulletData || !CharacterBossActive)
		return 0;

	auto data = GetClosestEnemyCol(bulletData);

	if (data && data->cwp)
	{
		if (isTargetCharBoss(data->cwp->mytask)) //check if it's a char fight...
		{
			if (!data->wtimer) {
				data->mode = Bcaptured; //set the enemy to a new custom state, see "bosses.cpp"
				data->btimer = bulletData->smode; //store pnum
				klwk->enemyGrabPtr = data->cwp->mytask; //we copy the task of the boss for external use with Klonoa.
				return 1;
			}
		}
	}

	return 0;
}

void deleteBullet(task* tp)
{
	char pnum = tp->twp->smode;
	auto player = playertp[pnum];

	if (player) {
		auto klwk = (klonoawk*)player->awp;

		if (klwk) {
			auto task = klwk->currentBulletPtr;
			if (task) {
				FreeTask(task);
			}

			klwk->bulletShot = false;;
			klwk->currentBulletPtr = nullptr;
		}
	}
}

void bulletTask(task* tp)
{
	float timer = 0.0f;

	auto data = tp->twp;
	if (data->mode)
	{
		if (data->mode == 1)
		{
			char pnum = tp->twp->smode;
			auto player = playertp[pnum];
			klonoawk* klwk = nullptr;

			if (player)
				klwk = (klonoawk*)player->awp;

			timer = (data->counter.f - 0.083333336f);
			data->counter.f = data->counter.f - 0.083333336f;

			if (timer <= 0.0f || klwk && klwk->enemyGrabPtr != nullptr)
			{
				FreeTask(tp);
				return;
			}

			data->pos.x += data->scl.x;
			data->pos.y += data->scl.y;
			data->pos.z += data->scl.z;

			EntryColliList(data);
		}
	}
	else
	{
		data->counter.f = 1.5f;
		CCL_Init(tp, &bullet_col, 1, 3u);
		tp->disp = dispEffectKnuxHadoken;
		tp->dest = deleteBullet;

		data->mode = 1;
	}

	tp->disp(tp);
	CheckRangeOut(tp);
}

void BulletLookForTarget(klonoawk* klwk, taskwk* data)
{
	auto pnum = data->counter.b[0];
	//if bullet exists, look for an enemy

	if (klwk->currentBulletPtr) {

		auto co2 = playerpwp[pnum];

		//if bullet hit an enemy
		if (WindBullet_CheckHitEnemy(klwk->currentBulletPtr->twp, klwk, co2) || WindBullet_CheckHitCharBoss(klwk->currentBulletPtr->twp, klwk))
		{
			//if target is monkey in cage, don't grab it, destroy instead.
			if (klwk->currentBulletPtr->exec == (TaskFuncPtr)OMonkeyCage)
			{
				data->mode = act_stnd;
				co2->mj.reqaction = 0;
				PlayCustomSoundVolume(se_pickEnemy, 1);
				return;
			}

			//grab enemy
			data->mode = act_holdStd;
			co2->mj.reqaction = anm_holdStd;
			PlayCustomSoundVolume(se_pickEnemy, 1);
			return;
		}
	}
}

//reset klonoa action when the bullet despawn
void BulletEnd(taskwk* data, playerwk* co2, klonoawk* klwk)
{
	if (!klwk->enemyGrabPtr)
	{
		if (co2->mj.reqaction < anm_windBullet || co2->mj.reqaction > anm_windBulletEnd)
		{
			bool isOnGround = (data->flag & 3);
			data->mode = isOnGround ? act_stnd : act_fall;
			klwk->bulletShot = false;
		}
	}
}

//create and move bullet
void BulletAction(taskwk* data, playerwk* co2, klonoawk* klwk)
{
	if (co2->mj.reqaction != anm_windBullet && co2->mj.reqaction != anm_windBulletAir || klwk->bulletShot)
		return;

	NJS_VECTOR KLRingPos = klwk->ringPos;
	NJS_VECTOR dest = KLRingPos;
	SetVectorDiff(&dest);
	NJS_VECTOR startPos = { 0 };

	njPushMatrix(_nj_unit_matrix_);
	njRotateZ_(data->ang.z);
	njRotateX_(data->ang.x);
	njRotateY_(0x8000 - (data->ang.y));
	njCalcVector(_nj_current_matrix_ptr_, &dest, &startPos);
	njPopMatrix(1u);
	njAddVector(&startPos, &data->cwp->info->center);

	auto task = CreateElementalTask(LoadObj_Data1, 6, bulletTask);

	if (task)
	{
		task->twp->smode = data->charIndex;
		klwk->currentBulletPtr = task;
		dest = { 3.0f, 0.0f, 0.0f };
		PConvertVector_P2G(data, &dest);
		klwk->currentBulletPtr->twp->pos = startPos;
		klwk->currentBulletPtr->twp->scl = dest;
		klwk->bulletShot = true;
	}
}

void Klonoa_ManageBullet(taskwk* data, playerwk* co2, klonoawk* klwk)
{
	if (!klwk->enemyGrabPtr)
	{
		BulletAction(data, co2, klwk);
		BulletLookForTarget(klwk, data);
		BulletEnd(data, co2, klwk);
	}
}