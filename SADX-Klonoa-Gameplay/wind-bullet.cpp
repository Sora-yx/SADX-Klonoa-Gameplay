#include "pch.h"

CCL_INFO bullet_col = { 0, 0, 0x70, 0x40, 0x400, { 0 }, 12.0f, 0.0, 0.0, 0.0, 0, 0, 0 };
extern ObjectFuncPtr enemyList[14];

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

bool isTargetAnEnemy(task* enemy) {

	for (int i = 0; i < LengthOfArray(enemyList); ++i) {

		if (enemyList[i] == (ObjectFuncPtr)enemy->exec) {
			return true;
		}
	}

	return false;
}

signed int WindBullet_CheckHitEnemy(taskwk* bulletData, klonoawk* klwk)
{
	if (!bulletData || !bulletData->cwp)
		return 0;

	auto cwp = bulletData->cwp;

	//Loop the collision array of the bullet...
	for (int i = 0; i < 16; i++)
	{
		auto target = cwp->hit_info[i].hit_twp;

		if (target != nullptr) //if the collision hit something, start looking for more data.
		{
			if (target->cwp->mytask && target->cwp->mytask->twp) // if the target has an exec and twp data
			{
				if (isTargetAnEnemy(target->cwp->mytask)) //check if it's an enemy...
				{
					target->mode = captured; //set the enemy to a new custom state, see "enemy.cpp"
					klwk->enemyGrabPtr = target->cwp->mytask; //we copy the task of the enemy for external use with Klonoa.
					return 1;
				}
			}
		}
	}

	return 0;
}

void bulletTask(task* tp)
{
	float timer = 0.0f;

	auto data = tp->twp;
	if (data->mode)
	{
		if (data->mode == 1)
		{
			timer = (data->counter.f - 0.083333336f);
			data->counter.f = data->counter.f - 0.083333336f;
			if (timer <= 0.0f)
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
		CCL_Init(tp, &bullet_col, 1, 4u);
		data->mode = 1;
		tp->disp = dispEffectKnuxHadoken;
	}

	tp->disp(tp);
	CheckRangeOut(tp);
}


void BulletLookForTarget(klonoawk* klwk, taskwk* data)
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
}

void BulletEnd(taskwk* data, playerwk* co2, klonoawk* klwk)
{
	if (co2->mj.reqaction < anm_windBullet || co2->mj.reqaction > anm_windBulletEnd)
	{
		bool isOnGround = (data->flag & 3);
		data->mode = isOnGround ? act_stnd : act_fall;
	}
}

void BulletAction(taskwk* data, playerwk* co2, klonoawk* klwk)
{
	//create and move bullet
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

	klwk->currentBulletPtr = CreateElementalTask(LoadObj_Data1, 6, bulletTask);

	if (klwk->currentBulletPtr)
	{
		dest = { 3.0f, 0.0f, 0.0f };
		PConvertVector_P2G(data, &dest);
		klwk->currentBulletPtr->twp->pos = startPos;
		klwk->currentBulletPtr->twp->scl = dest;
		klwk->bulletShot = true;
	}
}

void Klonoa_ManageBullet(taskwk* data, playerwk* co2, klonoawk* klwk)
{
	BulletAction(data, co2, klwk);
	BulletLookForTarget(klwk, data);
	BulletEnd(data, co2, klwk);
}