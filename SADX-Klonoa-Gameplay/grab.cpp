#include "pch.h"

void ResetKlonoaGrab(klonoawk* klwk)
{
	if (!klwk)
		return;

	klwk->enemyGrabPtr = nullptr;
	klwk->charBossGrabbed = false;
	return;

}

void CheckKlonoaEnemyPtr(klonoawk* klwk, taskwk* data, playerwk* co2)
{
	if (!data || !klwk)
		return;

	if (klwk->enemyGrabPtr || klwk->charBossGrabbed)
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

