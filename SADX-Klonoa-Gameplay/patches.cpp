#include "pch.h"

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


void init_Patches()
{
	PickDrop_Patches();
	WriteCall((void*)0x49582A, FixChaoPosHold);
}