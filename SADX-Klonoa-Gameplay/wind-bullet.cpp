#include "pch.h"

CCL_INFO bullet_col = { 0, 0, 0x70, 0x40, 0x400, { 0 }, 12.0f, 0.0, 0.0, 0.0, 0, 0, 0 };

signed int KlonoaWBullet_CheckInput(taskwk* data, playerwk* co2, klonoawk* klwk)
{
	if ((AttackButtons & Controllers[data->charIndex].PressedButtons) == 0 || allowSpinDash)
	{
		return 0;
	}

	klwk->bulletShot = false;
	bool isOnGround = (data->flag & 3);
	data->mode = isOnGround ? act_windBullet : act_windBulletAir;
	co2->mj.reqaction = isOnGround ? anm_windBullet : anm_windBulletAir;
	co2->spd.y = 0.0f;
	auto task = klwk->currentBulletPtr;
	if (task)
		FreeTask(task);

	klwk->currentBulletPtr = nullptr;
	PlayCustomSound(shot);
	return 1;
}

bool isTargetAnEnemy(taskwk* enemy) {

	HomingAttackTarget target = { 0, 1000000.0f };

	for (int i = 0; i < HomingAttackTarget_Sonic_Index; ++i) {

		HomingAttackTarget* target_ = &HomingAttackTarget_Sonic[i];

		if (target_->entity &&
			target_->entity->CollisionInfo->Object->Data2 != nullptr && target_->entity == (EntityData1*)enemy) {
			return true;
		}
	}

	return false;
}

signed int WindBullet_CheckHitEnemy(taskwk* data, klonoawk* klwk)
{
	if (!data || !data->cwp)
		return 0;

	auto cwp = data->cwp;

	//Loop the collision array of the bullet...
	for (int i = 0; i < 16; i++)
	{
		auto cwk = cwp->hit_info[i].hit_twp;

		if (cwk != nullptr) //if the collision hit something, start looking for more data.
		{
			if (cwk->cwp->mytask && cwk->cwp->mytask->twp) // if the target has an exec and twp data
			{
				if (cwk->cwp->id == 3) //if it's potentially an enemy
				{
					if (isTargetAnEnemy(cwk->cwp->mytask->twp)) //actually check if it's an enemy because you know...
					{
						cwk->mode = captured; //set the enemy to a new custom state, see "enemy.cpp"
						klwk->enemyGrabPtr = cwk->cwp->mytask; //we copy the task of the enemy for external use with Klonoa.
						return 1;
					}

				}
			}
		}
	}

	return 0;
}

void BulletTask(task* tp)
{
	float v3 = 0.0f;
	colliwk* col = nullptr;
	float v5 = 0.0f;
	float v6 = 0.0f;

	auto data = tp->twp;
	if (data->mode)
	{
		if (data->mode == 1)
		{
			v3 = (data->counter.f - 0.083333336f);
			data->counter.f = data->counter.f - 0.083333336f;
			if (v3 <= 0.0f)
			{
				FreeTask(tp);
				return;
			}

			col = data->cwp;
			v5 = (float)(data->scl.y + data->pos.y);
			v6 = (float)(data->scl.z + data->pos.z);
			data->pos.x = data->scl.x + data->pos.x;
			data->pos.y = v5;
			data->pos.z = v6;
			//			col->info->a = v3 * 10.0f;
			EntryColliList(data);
		}
	}
	else
	{
		data->counter.f = 1.0f;
		CCL_Init(tp, &bullet_col, 1, 4u);
		data->mode = 1;
		tp->disp = dispEffectKnuxHadoken;
	}

	tp->disp(tp);
	CheckRangeOut(tp);
}

void KlonoaBulletAction(taskwk* data, playerwk* co2, klonoawk* klwk)
{
	//if bullet exist, look for an enemy
	if (klwk->currentBulletPtr) {

		//if bullet hit an enemy
		if (WindBullet_CheckHitEnemy(klwk->currentBulletPtr->twp, klwk))
		{
			data->mode = act_holdStd;
			PlayCustomSoundVolume(pickEnemy, 1);
			return;
		}
	}

	//create and move bullet
	if (co2->mj.reqaction != anm_windBullet && co2->mj.reqaction != anm_windBulletAir || klwk->bulletShot)
		return;

	NJS_VECTOR KLRingPos = klwk->ringPos;
	NJS_VECTOR KLRingPosNew = KLRingPos;
	SetVectorDiff(&KLRingPosNew);
	NJS_VECTOR a3 = { 0 };

	njPushMatrix(_nj_unit_matrix_);
	njRotateZ_(data->ang.z);
	njRotateX_(data->ang.x);
	njRotateY_(0x8000 - (data->ang.y));
	njCalcVector(_nj_current_matrix_ptr_, &KLRingPosNew, &a3);
	njPopMatrix(1u);
	njAddVector(&a3, &data->cwp->info->center);

	if (klwk->bulletShot == false) {

		klwk->currentBulletPtr = CreateElementalTask(LoadObj_Data1, 6, BulletTask);

		if (klwk->currentBulletPtr)
		{
			KLRingPosNew = { 2.0f, 0.0f, 0.0f };
			PConvertVector_P2G(data, &KLRingPosNew);
			klwk->currentBulletPtr->twp->pos = a3;
			klwk->currentBulletPtr->twp->scl = KLRingPosNew;
		}
		klwk->bulletShot = true;
	}

}


void KlonoaBulletEnd(taskwk* data, playerwk* co2, klonoawk* klwk)
{
	KlonoaBulletAction(data, co2, klwk);

	if (co2->mj.reqaction < anm_windBullet || co2->mj.reqaction > anm_windBulletEnd)
	{
		bool isOnGround = (data->flag & 3);
		data->mode = isOnGround ? act_stnd : act_fall;
	}
}