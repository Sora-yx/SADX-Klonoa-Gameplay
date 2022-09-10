#pragma once

FunctionPointer(void, PlayIdleVoice, (taskwk* a1), 0x442360);
FunctionPointer(void, DrawCharacterShadow, (EntityData1* a1, struct_a3* a2), 0x49F0B0);
FunctionPointer(void, PGetAccelerationGlik, (taskwk* a1, motionwk2* a2, playerwk* a3), 0x448000);
FunctionPointer(void, PGetSpeedGlik, (taskwk* a1, motionwk2* a2, playerwk* a3), 0x0444580);
FunctionPointer(void, PResetPosition, (taskwk* a1, motionwk2* a2, playerwk* a3), 0x43EE70);


DataArray(NJS_ACTION*, SonicCharSelAnim, 0x3C5FF94, 4);
FunctionPointer(void, DrawEventAction, (taskwk* data), 0x4187D0);

TaskFunc(dispEffectKnuxHadoken, 0x4C0B60);

FunctionPointer(void, StartPanelJump, (taskwk* data), 0x4B8470);
TaskFunc(UpdateSetDataAndDelete, 0x46C150);

FunctionPointer(void, sub_49ED70, (taskwk* a1, float a2), 0x49ED70);
TaskFunc(SetFlagNoRespawn, 0x46C100);


// BOOL __usercall@<eax>(CharObj2 *Data2@<eax>, EntityData1 *Data1@<edi>)
static const void* const SonicCheckStopPtr = (void*)0x494FF0;
static inline signed int SonicCheckStop(playerwk* a1, taskwk* a2)
{
	int result;
	__asm
	{
		mov ecx, [a2]
		mov eax, [a1]
		call SonicCheckStopPtr
		mov result, eax
	}
	return result;
}

// void __usercall initCollidata(taskwk *data@<esi>)
static const void* const initCPDataPtr = (void*)0x44F070;
static inline void initCollidata(taskwk* a1)
{
	__asm
	{
		mov esi, a1
		call initCPDataPtr

	}
}