#include "pch.h"

void ResetKlonoaGrab(klonoawk* klwk)
{
	if (CharacterBossActive)
		return;

	auto task = klwk->enemyGrabPtr;

	if (task)
	{
		FreeTask(task);
		klwk->enemyGrabPtr = nullptr;
		return;
	}
}

void CheckKlonoaEnemyPtr(klonoawk* klwk, taskwk* data, playerwk* co2)
{
	if (!data || !klwk)
		return;
		
	if (klwk->enemyGrabPtr)
	{
		if (!isKlonoaHold(data->counter.b[0]) && data->mode > act_fall && data->mode != act_hurt)
		{
			ResetKlonoaGrab(klwk);
		}
	}
	else
	{
		if (data->mode >= act_holdStd && data->mode <= act_holdFall)
		{
			data->mode = 1;
			co2->mj.reqaction = 0;
		}
	}
}

void ThrowEnemyCalcDirection(taskwk* data, klonoawk* klwk)
{
	if (!klwk->enemyGrabPtr || !klwk->enemyGrabPtr->twp)
	{
		data->mode = 1;
		return;
	}

	auto enemyData = klwk->enemyGrabPtr->twp;

	NJS_VECTOR KLRingPos = klwk->ringPos;
	NJS_VECTOR dirspd = KLRingPos;
	SetVectorDiff(&dirspd);
	NJS_VECTOR startPos = { 0 };

	njPushMatrix(_nj_unit_matrix_);
	njRotateZ_(data->ang.z);
	njRotateX_(data->ang.x);
	njRotateY_(0x8000 - (data->ang.y));
	njCalcVector(_nj_current_matrix_ptr_, &dirspd, &startPos);
	njPopMatrix(1u);
	njAddVector(&startPos, &data->cwp->info->center);

	dirspd = { 5.0f, 0.0f, 0.0f };

	PConvertVector_P2G(data, &dirspd);

	enemyData->pos = startPos;

	if (CurrentLevel != LevelIDs_Chaos4)
		enemyData->pos.y += 6.0f;
	else
		enemyData->pos.y -= 2.0f;

	if (!CharacterBossActive) {
		auto wk = (enemywk*)klwk->enemyGrabPtr->mwp;
		wk->home = dirspd;
	}
	else
	{
		auto wk = (playerwk*)klwk->enemyGrabPtr->mwp->work.l;
		if (wk)
		{
			wk->acc = dirspd;
		}
	}
}

void DropEnemy(klonoawk* klwk)
{
	if (klwk && klwk->enemyGrabPtr && klwk->enemyGrabPtr->twp)
	{
		auto data = klwk->enemyGrabPtr->twp;
		if (!CharacterBossActive) {
			FreeColliWork(data);
			data->wtimer = 40;
			data->mode = drop;
		}
		else
		{
			data->timer.b[0] = 20;
			data->mode = Bdrop;
		}
	}
}

signed int ThrowEnemy_CheckInput(taskwk* data, playerwk* co2, klonoawk* klwk)
{
	if ((AttackButtons & Controllers[data->charIndex].PressedButtons) == 0 || !klwk->enemyGrabPtr)
	{
		return 0;
	}

	auto enemyData = klwk->enemyGrabPtr->twp;

	if (enemyData) {
		if (!CharacterBossActive) {
			FreeColliWork(enemyData);
			enemyData->mode = throwSetup;
		}
		else
		{
			enemyData->mode = BthrowSetup;
		}
		bool isOnGround = (data->flag & 3);
		data->mode = isOnGround ? act_throwStd : act_throwAir;
		co2->mj.reqaction =  isOnGround ? anm_throwStd : anm_throwAir; 
		ThrowEnemyCalcDirection(data, klwk);
		PlayCustomSoundVolume(kl_throw, 1);
		return 1;
	}

	return 0;
}


//patch regular obj pick /drop anim frames check BS

void FixSonicDropThrowObject()
{
	//change condition from "==" to ">=" or "<=" depending on the situation
	WriteData<1>((int*)0x493401, 0x77);	//0x76
	WriteData<1>((int*)0x493233, 0x77);
	WriteData<1>((int*)0x49324A, 0x87);

	WriteData<1>((int*)0x493366, 0x7D);
	WriteData<1>((int*)0x49337c, 0x77);
}

float pickAnimFrame = 5.0f;
//fix animation frame check for object throw / drop etc.
void PickDrop_Patches()
{
	FixSonicDropThrowObject();
	//pick obj
	WriteData((float**)0x495327, &pickAnimFrame);
	//throw obj on ground (Klo animation is way too short so we reduce the check to 3 frames)
	WriteData<1>((int*)0x493365, 0x40);
	WriteData<1>((int*)0x493364, 0x40);
}
