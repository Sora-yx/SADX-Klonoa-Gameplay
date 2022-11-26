#include "pch.h"

static UsercallFunc(signed int, Sonic_CheckPickUpObject_t, (playerwk* a1, taskwk* a2, motionwk2* a3), (a1, a2, a3), 0x495310, rEAX, rEAX, rECX, rEDI);

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


float pickAnimFrame = 6.0f;
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

void __fastcall FixChaoPosHold(NJS_VECTOR* vd, NJS_VECTOR *vs)
{

	if (vs && isKlonoa(0) && (playerpwp[0]->mj.reqaction == 51 || playerpwp[0]->mj.reqaction == 52))
		vs->y -= 4.0f;

	return njAddVector(vd, vs);
}


void __cdecl Sonic_Snowboard_Main_r(task* a1)
{
	auto data = a1->twp;
	int pNum = (unsigned __int8)data->counter.b[0];
	auto data2 = (motionwk2*)EntityData2Ptrs[pNum];
	auto pData = playertwp[pNum];
	auto objMtn = a1->mwp;
	auto curAnim = playerpwp[pNum]->mj.action;

	if (curAnim < 102 || curAnim > 124)
	{
		if ((data->flag & 8) == 0)
		{
			data->pos.y = data->pos.y - 5.2199998f;
			ObjectMovableSRegularExecute(a1);
			data->pos.y = data->pos.y - -5.2199998f;
		}
		Sonic_Snowboard_Display((ObjectMaster*)a1);
	}
	else
	{
		data->pos = pData->cwp->info->center;
		data->ang = pData->ang;
		data->ang.y = 0x8000 - pData->ang.y;
		objMtn->spd = data2->spd;
		data->flag &= 0xF7u;
		Sonic_Snowboard_Display((ObjectMaster*)a1);
	}
}

void init_Patches()
{
	PickDrop_Patches();
	WriteCall((void*)0x49582A, FixChaoPosHold);
	WriteJump(Sonic_Snowboard_Main, Sonic_Snowboard_Main_r);
}