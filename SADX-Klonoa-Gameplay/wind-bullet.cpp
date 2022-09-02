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
	PlayCustomSound(shot);
	return 1;
}


void KlonoaBulletAction(taskwk* data, playerwk* co2, klonoawk* klwk)
{
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

		auto bullet = CreateElementalTask(LoadObj_Data1, 6, EffectKnuxHadoken);

		if (bullet)
		{
			KLRingPosNew = { 2.0f, 0.0f, 0.0f };
			PConvertVector_P2G(data, &KLRingPosNew);
			bullet->twp->pos = a3;
			bullet->twp->scl = KLRingPosNew;
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