#pragma once

struct ModelAnimationThing_r
{
	NJS_VECTOR* Vectors;
	NJS_VECTOR* VectorsB;
	Rotation3* Rotations;
	void(__cdecl* Function)(NJS_CNK_MODEL*);
};

struct EVBOSS_WORK
{
	char mode;
	char smode;
	char type;
	char attackmode;
	__int16 timer;
	__int16 count;
	char* attackmodetbl;
	int flag;
	void(__cdecl* exec)(task*);
	void(__cdecl* exit)(task*);
	NJS_POINT3 targetpos;
	unsigned int old_ang;
	float homing_dist;
	int ana_ang;
	float ana_str;
	float dist;
	float distxz;
	unsigned int btn_on;
	unsigned int btn_press;
	task* stp;
	taskwk* stwp;
	motionwk2* smwp;
	CharObj2* spwp;
	task* dtp;
	taskwk* dtwp;
	motionwk2* dmwp;
	CharObj2* dpwp;
	task* ltp;
	task* ring_tp;
	task* wall_tp;
	float fpara[4];
	__int16 spara[8];
	NJS_POINT3 init_pos;
};



ThiscallFunctionPointer(void*, njStartMotionObj, (NJS_MOTION* obj, float frame), 0x780500);
DataPointer(ModelAnimationThing_r*, nj_objmotinfo_ptr, 0x389DCCC);

DataArray(colaround, HomingAttackTarget_Sonic_, 0x3B259C0, 657);
DataArray(colaround, HomingAttackTarget_NonSonic_, 0x3B242F8, 657);
FunctionPointer(void, PlayIdleVoice, (taskwk* a1), 0x442360);
FunctionPointer(void, DrawCharacterShadow, (EntityData1* a1, struct_a3* a2), 0x49F0B0);


DataArray(NJS_ACTION*, SonicCharSelAnim, 0x3C5FF94, 4);
FunctionPointer(void, DrawEventAction, (taskwk* data), 0x4187D0);

TaskFunc(dispEffectKnuxHadoken, 0x4C0B60);

FunctionPointer(void, StartPanelJump, (taskwk* data), 0x4B8470);
TaskFunc(UpdateSetDataAndDelete, 0x46C150);

FunctionPointer(void, sub_49ED70, (taskwk* a1, float a2), 0x49ED70);
TaskFunc(SetFlagNoRespawn, 0x46C100);
FunctionPointer(void*, AllocateCPData, (), 0x44EFC0);
TaskFunc(Bubble_Child, 0x7A8260);
DataArray(player_parameter, PhysicsArray_, 0x9154E8, 8);

ObjectFunc(UnindusA_Exec, 0x4AF030);
ObjectFunc(UnindusB_Exec, 0x4AF3D0);
ObjectFunc(UnindusC_Exec, 0x4AF770);
DataPointer(float, Life_Max, 0x03C58158);

FunctionPointer(void, CL_ColPolCFPolygon, (zxsdwstr* a1, float a2), 0x454F60);

TaskFunc(ESman_Exec, 0x4C8DD0);

DataPointer(NJS_SPRITE, AncientLightSprite, 0x981A28);
DataPointer(NJS_SPRITE, AncientLightSprite2, 0x981A08);
DataPointer(NJS_TEXLIST, textlist_ancient_light, 0x981998);

DataPointer(NJS_SPRITE, sprite_score, 0x913078);

//cutscene anims
//EV0002
DataPointer(NJS_ACTION, action_s_s0029_sonic, 0x3C84998);

//03

DataPointer(NJS_ACTION, action_s_s0031_sonic, 0x3C851F8);
DataPointer(NJS_ACTION, action_s_s0032_sonic, 0x3C851F0);
DataPointer(NJS_ACTION, action_s_s0033_sonic, 0x3C85200);
DataPointer(NJS_ACTION, action_s_s0013_sonic, 0x3C85208);

//E07
DataPointer(NJS_ACTION, action_s_s0006_sonic, 0x3C84878);

//E08
DataPointer(NJS_ACTION, action_s_s0038_sonic, 0x3C84888);
DataPointer(NJS_ACTION, action_s_s0044_sonic, 0x3C84890);
DataPointer(NJS_ACTION, action_s_s0009_sonic, 0x3C84898);
DataPointer(NJS_ACTION, action_s_s0042_sonic, 0x3C848A8);
DataPointer(NJS_ACTION, action_s_s0045_sonic, 0x3C848C8);

//E0B
DataPointer(NJS_ACTION, action_s_s0046_sonic, 0x3C84948);
DataPointer(NJS_ACTION, action_s_s9001_sonic, 0x3C84958);
DataPointer(NJS_ACTION, action_s_s9000_sonic, 0x3C84960);
DataPointer(NJS_ACTION, action_s_s0002_sonic, 0x3C84968);

//E0D
DataPointer(NJS_ACTION, action_s_s0028_sonic, 0x3C853B8);

//E011
DataPointer(NJS_ACTION, action_s_s0025_sonic, 0x3C84EC4);
DataPointer(NJS_ACTION, action_s_s0026_sonic, 0x3C84ECC);
DataPointer(NJS_ACTION, action_s_s0024_sonic, 0x3C84ED4);
DataPointer(NJS_ACTION, action_s_s0027_sonic, 0x3C84EDC);

//E012
DataPointer(NJS_ACTION, action_s_s0037_sonic, 0x3C84C78);

//E013
DataPointer(NJS_ACTION, action_s_s0004_sonic, 0x3C848A0);
DataPointer(NJS_ACTION, action_s_s0017_sonic, 0x3C84C80);
DataPointer(NJS_ACTION, action_s_s0001_sonic, 0x3C84C90);

//E017
DataPointer(NJS_ACTION, action_s_s0058_sonic, 0x3C84C98);

//E01B
DataPointer(NJS_ACTION, action_s_s0007_sonic, 0x3C84A78);
DataPointer(NJS_ACTION, action_s_s0005_sonic, 0x3C84A90);
DataPointer(NJS_ACTION, action_s_s0012_sonic, 0x3C84AA0);

//E01C
DataPointer(NJS_ACTION, action_s_s0067_sonic, 0x3C84A38);

//E01E
DataPointer(NJS_ACTION, action_s_s0021_sonic, 0x3C853C0);
DataPointer(NJS_ACTION, action_s_s0019_sonic, 0x3C853C8);
DataPointer(NJS_ACTION, action_s_s0030_sonic, 0x3C853D0);
DataPointer(NJS_ACTION, action_s_s0020_sonic, 0x3C853D8);

//E023
DataPointer(NJS_ACTION, action_s_s0018_sonic, 0x3C85074);

//E026
DataPointer(NJS_ACTION, action_s_s0071_sonic, 0x3C85218);
DataPointer(NJS_ACTION, action_s_s0070_sonic, 0x3C85220);
DataPointer(NJS_ACTION, action_s_s0068_sonic, 0x3C85228);

//E08
DataPointer(NJS_ACTION, action_s_s0036_sonic, 0x3C85190);
DataPointer(NJS_ACTION, action_s_s0035_sonic, 0x3C851A0);

//E02A
DataPointer(NJS_ACTION, action_s_s0053_sonic, 0x3C827C0);
DataPointer(NJS_ACTION, action_s_s0057_sonic, 0x3C84E34);
DataPointer(NJS_ACTION, action_s_s0054_sonic, 0x3C84E3C);

//E031
DataPointer(NJS_ACTION, action_s_s0055_sonic, 0x3C84D94);
DataPointer(NJS_ACTION, action_s_s0056_sonic, 0x3C84DAC);

//E052
DataPointer(NJS_ACTION, action_s_s0069_sonic, 0x3C8544C);

//E060
DataPointer(NJS_ACTION, action_s_s0076_sonic, 0x3C853F0);

//E08A
DataPointer(NJS_ACTION, action_s_s0034_sonic, 0x3C84938);

//E00F4
DataPointer(NJS_ACTION, action_s_s0059_sonic, 0x3C84EF4);
DataPointer(NJS_ACTION, action_s_s0060_sonic, 0x3C84EFC);
DataPointer(NJS_ACTION, action_s_s0061_sonic, 0x3C84EE4);
DataPointer(NJS_ACTION, action_s_s0062_sonic, 0x3C84EEC);

DataPointer(NJS_ACTION, action_s_s0089_sonic, 0x03C84A08);
DataPointer(NJS_ACTION, action_s_s0064_sonic, 0x03C84A10);
DataPointer(NJS_ACTION, action_s_s0065_sonic, 0x03C84A18);
DataPointer(NJS_ACTION, action_s_s0066_sonic, 0x03C84A20);

//upgrades item
DataPointer(NJS_ACTION, action_s_item_r0, 0x3C847E8);
DataPointer(NJS_ACTION, action_s_item_r2, 0x3C847E0);
DataPointer(NJS_ACTION, action_s_item_r1, 0x3C847D8);
DataPointer(NJS_ACTION, action_s_item_s1, 0x3C847C0);
DataPointer(NJS_ACTION, action_s_item_s0, 0x3C847C8);
DataPointer(NJS_ACTION, action_s_item_s2, 0x3C847D0);
DataPointer(NJS_ACTION, action_s_item_l0, 0x3C84758);
DataPointer(NJS_ACTION, action_s_item_l1, 0x3C84760);
DataPointer(NJS_ACTION, action_s_item_l2, 0x3C84768);

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

// void __usercall SonicHoldObject(playerwk *a1@<eax>, taskwk *a2@<ecx>)
static const void* const sHoldObjPtr = (void*)0x4955E0;
static inline void SonicHoldObject(playerwk* a1, taskwk* a2)
{
	__asm
	{
		mov ecx, a2
		mov eax, a1
		call sHoldObjPtr
	}
}


static void(__cdecl** NodeCallbackFuncPtr)(NJS_OBJECT* obj) = (decltype(NodeCallbackFuncPtr))0x3AB9908;
DataPointer(task*, TRB2S, 0x3C8449C);