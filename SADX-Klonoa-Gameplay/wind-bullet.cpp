#include "pch.h"

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

HomingAttackTarget GetEnemies(NJS_VECTOR* pos) {

	HomingAttackTarget target = { 0, 1000000.0f };

	for (int i = 0; i < HomingAttackTarget_Sonic_Index; ++i) {

		HomingAttackTarget* target_ = &HomingAttackTarget_Sonic[i];

		float dist = GetDistance(pos, &target_->entity->Position);

		if (dist < target.distance && target_->entity &&
			target_->entity->CollisionInfo->id == 3 &&
			target_->entity->CollisionInfo->Object->Data2 != nullptr) {
			target.distance = dist;
			target.entity = target_->entity;
			break;
		}
	}

	return target;
}

signed int EnemyGrab(taskwk* data)
{
	if (!data || !data->cwp)
		return 0;

	auto cwp = data->cwp;

	for (int i = 0; i < 16; i++)
	{
		auto cwk = cwp->hit_info[i].hit_twp;

		if (cwk != nullptr)
		{
			if (cwk->cwp->mytask && cwk->cwp->mytask->twp)
			{
				if (cwk->cwp->id == 3)
				{
					return 1;
				}
			}
		}
	}

	return 0;
}

void KlonoaBulletAction(taskwk* data, playerwk* co2, klonoawk* klwk)
{

	if (klwk->currentBulletPtr) {
		EnemyGrab(klwk->currentBulletPtr->twp);
	}

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

		klwk->currentBulletPtr = CreateElementalTask(LoadObj_Data1, 6, EffectKnuxHadoken);

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