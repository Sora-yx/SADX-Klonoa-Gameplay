#include "pch.h"

bool klonoa = false;
uint8_t klonoaPnum = 0;

ModelInfo* KlonoaMDL = nullptr;
AnimationFile* KlonoaANM[20] = { 0 };
#define AnimCount 147
#define boneCount 48
AnimData_t KlonoaAnimList[AnimCount];
static Trampoline* LoadLevelObject_t = nullptr;

NJS_TEXNAME KlonoaTex[2];
NJS_TEXLIST KlonoaTexList = { arrayptrandlength(KlonoaTex) };

static Trampoline* Sonic_Main_t = nullptr;
static Trampoline* Sonic_Display_t = nullptr;
static Trampoline* DrawSonicMotion_t = nullptr;

extern NJS_TEXLIST KlonoaTexList;

int getKlonoaPlayer()
{
	for (uint8_t i = 0; i < 8; i++)
	{
		if (klonoa && EntityData1Ptrs[i] && EntityData1Ptrs[i]->CharIndex == klonoaPnum)
		{
			return i;
		}
	}

	return -1;
}

bool isKlonoa(char pnum)
{
	if (klonoa && EntityData1Ptrs[pnum] && EntityData1Ptrs[pnum]->CharIndex == klonoaPnum)
	{
		return true;
	}

	return false;
}


void FreeObjModels()
{
	FreeMDL(KlonoaMDL);
}


void DrawKlonoa(CharObj2* a2, int animNum, NJS_ACTION* action)
{

	SetupWorldMatrix();
	Direct3D_SetChunkModelRenderState();

	NJS_ACTION act2 = *action;

	if (*(int*)0x3ABD9CC)
	{
		DrawQueueDepthBias = -5952.0;
		njCnkAction_Queue(&act2, a2->AnimationThing.Frame, QueuedModelFlagsB_EnableZWrite);
		DrawQueueDepthBias = 0.0;
	}
	else
	{
		njCnkAction(&act2, a2->AnimationThing.Frame);
	}


	Direct3D_UnsetChunkModelRenderState();
	njPopMatrix(1);
}


void SpinDash_RotateModel(int curAnim, taskwk* data)
{
	if (curAnim == 14)
	{
		if (data->flag & (Status_OnColli | Status_Ground))
		{
			NJS_VECTOR scale = { 0.0f, -1.0f, 0.0f };
			njTranslateV(0, &scale);
			njRotateZ(0, 0x2000);
			scale.x = 0.69999999f;
			scale.y = 1.1f;
			scale.z = 0.80000001f;
			njScaleV(0, &scale);
		}
	}
}

void __cdecl Klonoa_Display_r(ObjectMaster* obj)
{
	EntityData1* data = obj->Data1;
	auto tp = (task*)obj;

	if (MissedFrames || !klonoa || (!IsVisible(&data->Position, 15.0)))
		return;

	auto co2 = CharObj2Ptrs[klonoaPnum];
	int curAnim = (unsigned __int16)co2->AnimationThing.Index;
	auto data2_pp = (EntityData2*)obj->Data2;

	Direct3D_SetZFunc(1u);
	BackupConstantAttr();
	AddConstantAttr(0, NJD_FLAG_IGNORE_SPECULAR);

	if (SuperSonicFlag)
		Direct3D_PerformLighting(4);
	else
		Direct3D_PerformLighting(2);

	if (!MetalSonicFlag && SONIC_OBJECTS[6]->sibling != *(NJS_OBJECT**)0x3C55D40)
	{
		co2->AnimationThing.field_2 = 2;
		ProcessVertexWelds(data, data2_pp, co2);
		co2->AnimationThing.WeldInfo[0xB].ModelB = SONIC_OBJECTS[6]->sibling;
		co2->AnimationThing.field_2 = 0;
		ProcessVertexWelds(data, data2_pp, co2);
		*(NJS_OBJECT**)0x3C55D40 = SONIC_OBJECTS[6]->sibling;
	}


	if (co2->AnimationThing.State == 2)
		curAnim = (unsigned __int16)co2->AnimationThing.LastIndex;

	if (!(data->InvulnerableTime & 2))
	{
		njSetTexture(&KlonoaTexList);
		NJS_VECTOR scale = { 0.2f, 0.2f, 0.2f };
		njPushMatrix(nullptr);
		NJS_VECTOR pos = data->CollisionInfo->CollisionArray->center;
		pos.y -= 5.0f;
		njTranslateV(0, &pos);
		njScaleV(0, &scale);

		njRotateZ_(data->Rotation.z);
		njRotateX_(data->Rotation.x);
		njRotateY_(0x8000 - data->Rotation.y);

		SpinDash_RotateModel(curAnim, (taskwk*)data);

		NJS_ACTION* action;
		if (co2->AnimationThing.State == 2) {
			action = co2->AnimationThing.action;
		}
		else
		{
			action = co2->AnimationThing.AnimData[curAnim].Animation;
		}

		DrawKlonoa(co2, curAnim, action);

		njPopMatrix(1u);
	}



	Direct3D_PerformLighting(0);
	ClampGlobalColorThing_Thing();
	RestoreConstantAttr();
	Direct3D_ResetZFunc();

	if (IsGamePaused())
		DrawCharacterShadow(data, &co2->_struct_a3);
}


void __cdecl Sonic_Main_r(task* obj)
{
	auto tp = obj;
	auto data = obj->twp;
	motionwk2* data2 = (motionwk2*)obj->mwp;
	playerwk* co2 = (playerwk*)obj->mwp->work.l;
	chaoswk* cwk = (chaoswk*)obj->awp;

	char pnum = data->counter.b[0];

	if (!data->mode)
	{
		LoadPVM("KlonoaTex", &KlonoaTexList);
	}

	TARGET_DYNAMIC(Sonic_Main)(obj);


	klonoa = true;
	klonoaPnum = data->counter.b[0];
}

AnimData_t MetalAnimList[AnimCount];

int loc_49AB51 = 0x49AB51;
__declspec(naked) void SetAnimList()
{
	__asm
	{
		mov eax, [MetalSonicFlag]
		mov al, [eax]
		test al, al
		jnz metal
		mov dword ptr[ebp + 140h], offset KlonoaAnimList
		jmp loc_49AB51
		metal :
		mov dword ptr[ebp + 140h], offset MetalAnimList
			jmp loc_49AB51
	}
}


void LoadKlonoa_Files()
{
	KlonoaMDL = LoadChunkModel("Klonoa");
	KlonoaANM[0] = LoadObjectAnim("stand");
	KlonoaANM[1] = LoadObjectAnim("death");
	KlonoaANM[2] = LoadObjectAnim("fall");
	KlonoaANM[3] = LoadObjectAnim("walk");
	KlonoaANM[4] = LoadObjectAnim("fly");
	KlonoaANM[5] = LoadObjectAnim("hover");
	KlonoaANM[6] = LoadObjectAnim("run");
	KlonoaANM[7] = LoadObjectAnim("idle");
	KlonoaANM[8] = LoadObjectAnim("jump");

	//Init new anim list, put falling animation as a placeholder / failsafe for animation that don't have a SADX counterpart
	for (int i = 0; i < LengthOfArray(KlonoaAnimList); i++)
	{
		KlonoaAnimList[i].Animation = new NJS_ACTION;
		KlonoaAnimList[i].Animation->object = KlonoaMDL->getmodel();
		KlonoaAnimList[i].Animation->motion = KlonoaANM[anm_fall]->getmotion();
		KlonoaAnimList[i].Instance = KlonoaANM[anm_fall]->getmodelcount();
		KlonoaAnimList[i].Property = 3;
		KlonoaAnimList[i].NextAnim = 18;
		KlonoaAnimList[i].TransitionSpeed = 0.25f;
		KlonoaAnimList[i].AnimationSpeed = 1.0f;
	}


	KlonoaAnimList[0].Animation->motion = KlonoaANM[anm_stand]->getmotion();
	KlonoaAnimList[0].AnimationSpeed = 1.0f;
	KlonoaAnimList[0].NextAnim = 0;
	KlonoaAnimList[0].Property = 3;
	KlonoaAnimList[0].TransitionSpeed = 0.0625f;

	//walk
	KlonoaAnimList[9].Animation->motion = KlonoaANM[anm_walk]->getmotion();
	KlonoaAnimList[9].AnimationSpeed = 3.4f;
	KlonoaAnimList[9].NextAnim = 9;
	KlonoaAnimList[9].Property = 10;
	KlonoaAnimList[9].TransitionSpeed = 0.25f;

	//speed walk
	KlonoaAnimList[10].Animation->motion = KlonoaANM[anm_walk]->getmotion();
	KlonoaAnimList[10].AnimationSpeed = 1.4f;
	KlonoaAnimList[10].NextAnim = 10;
	KlonoaAnimList[10].Property = 10;
	KlonoaAnimList[10].TransitionSpeed = 0.25f;

	//jogging
	KlonoaAnimList[11].Animation->motion = KlonoaANM[anm_run]->getmotion();
	KlonoaAnimList[11].Property = 10;
	KlonoaAnimList[11].NextAnim = 11;
	KlonoaAnimList[11].TransitionSpeed = 0.25f;
	KlonoaAnimList[11].AnimationSpeed = 0.60000001f;

	//run
	KlonoaAnimList[12].Animation->motion = KlonoaANM[anm_run]->getmotion();
	KlonoaAnimList[12].Property = 10;
	KlonoaAnimList[12].NextAnim = 12;
	KlonoaAnimList[12].TransitionSpeed = 0.25f;
	KlonoaAnimList[12].AnimationSpeed = 0.3f;

	//sprint
	KlonoaAnimList[13].Animation->motion = KlonoaANM[anm_run]->getmotion();
	KlonoaAnimList[13].Property = 9;
	KlonoaAnimList[13].NextAnim = 13;
	KlonoaAnimList[13].TransitionSpeed = 0.5f;
	KlonoaAnimList[13].AnimationSpeed = 0.35555f;

	//falling
	KlonoaAnimList[18].Animation->motion = KlonoaANM[anm_fall]->getmotion();
	KlonoaAnimList[18].Property = 3;
	KlonoaAnimList[18].NextAnim = 18;
	KlonoaAnimList[18].TransitionSpeed = 0.25f;
	KlonoaAnimList[18].AnimationSpeed = 1.0f;

	//idle
	KlonoaAnimList[4].Animation->motion = KlonoaANM[anm_idle]->getmotion();
	KlonoaAnimList[4].Property = 4;
	KlonoaAnimList[4].NextAnim = 5;
	KlonoaAnimList[4].TransitionSpeed = 0.25;
	KlonoaAnimList[4].AnimationSpeed = 1.0f;

	KlonoaAnimList[5].Animation->motion = KlonoaANM[anm_idle]->getmotion();
	KlonoaAnimList[5].Property = 4;
	KlonoaAnimList[5].NextAnim = 6;
	KlonoaAnimList[5].TransitionSpeed = 0.25;
	KlonoaAnimList[5].AnimationSpeed = 1.0f;

	KlonoaAnimList[6].Animation->motion = KlonoaANM[anm_idle]->getmotion();
	KlonoaAnimList[6].Property = 4;
	KlonoaAnimList[6].NextAnim = 5;
	KlonoaAnimList[6].TransitionSpeed = 0.25;
	KlonoaAnimList[6].AnimationSpeed = 1.0f;

	//jump
	KlonoaAnimList[32].Animation->motion = KlonoaANM[anm_jump]->getmotion();
	KlonoaAnimList[32].Property = 3;
	KlonoaAnimList[32].NextAnim = 32;
	KlonoaAnimList[32].TransitionSpeed = 0.5;
	KlonoaAnimList[32].AnimationSpeed = 1.0;

}

void init_KlonoaModelsAnim()
{
	LoadKlonoa_Files();
}


void initKlonoa()
{
	init_KlonoaModelsAnim();

	Sonic_Main_t = new Trampoline((int)Sonic_Main, (int)Sonic_Main + 0x7, Sonic_Main_r);
	WriteJump(Sonic_Display, Klonoa_Display_r);
	//DrawSonicMotion_t = new Trampoline((intptr_t)0x494400, (intptr_t)0x494406, DrawSonicMotionASM);
	WriteJump((void*)0x49AB47, SetAnimList);
	WriteData((short*)0x49ACD8, (short)0x9090);
	WriteData<2>((void*)0x4916A5, 0x90u); // disable metal's weird tilting thing
	WriteData((char*)0x49BE22, (char)0xEB);

	WriteData<1>((int*)0x493500, 0xC3); //disable sonic morph
	WriteData<1>((int*)0x4937B0, 0xC3); //disable morph head

}