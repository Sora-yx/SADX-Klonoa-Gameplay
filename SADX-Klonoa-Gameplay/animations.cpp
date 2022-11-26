#include "pch.h"

AnimationFile* KlonoaANM[60] = { 0 };
AnimationFile* KlonoaEvANM[50] = { 0 };
#define AnimCount 160
#define boneCount 48

AnimData_t KlonoaAnimList[AnimCount] = { 0 };

static FunctionHook<void, taskwk*, int> DrawEventActionPL_t((intptr_t)0x417FB0);
static FunctionHook<void, task*, NJS_ACTION*, NJS_TEXLIST*, float, char, char> EV_SetAction_t((intptr_t)EV_SetAction);
static FunctionHook<void, task*, NJS_OBJECT*, NJS_MOTION*, NJS_TEXLIST*, float, int, int> EV_SetMotion_t((intptr_t)EV_SetMotion);
static FunctionHook<void> LoadPlayerMotionDataAll_t((intptr_t)0x5034A0);
static FunctionHook<void, task*, char*> EV_SetFace_t((intptr_t)0x4310D0);

static bool setAnim = false;

AnimData_t* GetKlonoaAnimList()
{
	return KlonoaAnimList;
}

static const std::unordered_map<uint16_t, uint16_t> AnimMotion_ids_map = {
	{ 1, anm_std },
	{ 3, anm_walk},
	{ 4, anm_walk2 },
	{ 5, anm_walk3 },
	{ 6, anm_run },
	{ 7, anm_brake },
	{ 10, anm_fall},
	{ 14, anm_jump},
	{ 18, anm_run2},
	{ 20, anm_turnAround},
	{ 123, anm_bStance1 },
};

void KlonoaManageVictoryMotion(playerwk* co2)
{
	if (!co2)
		return;

	if (co2->mj.reqaction == 75 || co2->mj.reqaction == 143)
	{
		if (co2->mj.nframe >= 39.99f)
		{
			co2->mj.nframe = 24.0f;
		}
		else
		{
			co2->mj.nframe += 0.4f;
		}
	}
}

//Series of hack to make all the rendering events anim function working with Chunk Model

void DrawKlonoa_Event(NJS_ACTION* anim, float frame, QueuedModelFlagsB flg)
{
	NJS_ACTION act2 = *anim;

	if (QueueCharacterAnimations)
	{
		DrawQueueDepthBias = -5952.0;
		njCnkAction_Queue(&act2, frame, QueuedModelFlagsB_EnableZWrite);
		DrawQueueDepthBias = 0.0;
	}
	else
	{
		njCnkAction(&act2, frame);
	}
}

//used for cutscene and charSel
void __cdecl Klonoa_linkEx(NJS_ACTION_LINK* action, float frame, int flag)
{
	if (action)
	{
		njSetTexture(&KlonoaTexList);
		return njCnkActionLink(action, frame, (QueuedModelFlagsB)flag);
	}
}

void __cdecl late_actionClipEx_r(NJS_ACTION* anim, float a2, QueuedModelFlagsB a3)
{
	if (anim && anim->object == KlonoaMDL->getmodel())
	{
		njSetTexture(&KlonoaTexList);
		return DrawKlonoa_Event(anim, a2, a3);
	}

	late_ActionEx(anim, a2, (LATE)a3);
}

//event head are not compatible obviously, we force standing animation when they happen
void __cdecl late_ActionLinkEx_r(NJS_ACTION_LINK* action, float frame, int flag)
{
	if (action && action->object == KlonoaMDL->getmodel())
	{
		njSetTexture(&KlonoaTexList);
		auto anim = GetKlonoaAnimList();

		if (anim && anim[0].Animation)
		{
			return DrawKlonoa_Event(anim[0].Animation, frame, (QueuedModelFlagsB)flag);
		}

		return;
	}

	late_ActionLinkEx(action, frame, (LATE)flag);
}

void late_ActionLinkMesh_r(NJS_ACTION_LINK* actionLink, float frame, LATE flgs)
{
	if (actionLink)
	{
		if (actionLink->object && actionLink->object == KlonoaMDL->getmodel())
		{
			njSetTexture(&KlonoaTexList);
			return Klonoa_linkEx(actionLink, frame, flgs);
		}
	}

	return late_ActionLinkMesh(actionLink, frame, flgs);
}

void late_ActionLink_r(NJS_ACTION_LINK* actionLink, float frame, LATE flgs)
{
	if (actionLink)
	{
		if (actionLink->object == KlonoaMDL->getmodel())
		{
			njSetTexture(&KlonoaTexList);
			return Klonoa_linkEx(actionLink, frame, flgs);
		}
	}

	return late_ActionLink(actionLink, frame, flgs);
}

void late_ActionMesh_r(NJS_ACTION* act, float frame, LATE flgs)
{
	if (act)
	{
		if (act->object == KlonoaMDL->getmodel())
		{
			njSetTexture(&KlonoaTexList);
			return DrawKlonoa_Event(act, frame, (QueuedModelFlagsB)flgs);
		}
	}

	return late_ActionMesh(act, frame, flgs);
}

void __cdecl late_Action_r(NJS_ACTION* anim, float a2, QueuedModelFlagsB a3)
{
	if (anim)
	{
		if (anim->object == KlonoaMDL->getmodel())
		{
			njSetTexture(&KlonoaTexList);
			return DrawKlonoa_Event(anim, a2, a3);
		}
	}

	return late_Action(anim, a2, (LATE)a3);
}

bool isSonicNextAnim(NJS_TEXLIST* tex)
{
	int pnum = getKlonoaPlayer();

	if (pnum >= 0)
	{
		if (tex == &SONIC_TEXLIST || tex == &SUPERSONIC_TEXLIST)
		{
			return true;
		}
	}

	return false;
}

//convert all Sonic event anims with Klonoa anim counterpart on run time
void __cdecl EV_SetMotion_r(task* a1, NJS_OBJECT* a2, NJS_MOTION* a3, NJS_TEXLIST* a4, float a5, int a6, int a7)
{
	if (a1 && a2 && a2 != KlonoaMDL->getmodel())
	{
		if (isSonicNextAnim(a4)) {
			a2 = KlonoaMDL->getmodel();
			a3 = KlonoaEvANM[1]->getmotion();
			a4 = &KlonoaTexList;
		}
	}

	EV_SetMotion_t.Original(a1, a2, a3, a4, a5, a6, a7);
}

bool ConvertSonicActionToKloAction(NJS_ACTION* a2)
{
	for (uint8_t i = 1; i < 124; i++)
	{
		if (SONIC_ACTIONS[i] == a2)
		{
			if ((AnimMotion_ids_map.find(i)->first == i))
			{
				uint16_t id = AnimMotion_ids_map.find(i)->second;

				if (KlonoaAnimList[id].Animation)
				{
					a2->motion = KlonoaAnimList[id].Animation->motion;
					return true;
				}
			}
		}
	}

	return false;
}

void __cdecl EV_SetAction_r(task* obj, NJS_ACTION* anim, NJS_TEXLIST* tex, float speed, char mode, char linkframe)
{
	if (obj && anim && anim->object != KlonoaMDL->getmodel())
	{
		if (isSonicNextAnim(tex))
		{
			anim->object = KlonoaMDL->getmodel();
			tex = &KlonoaTexList;

			if (!ConvertSonicActionToKloAction(anim)) //if the animation didn't get replaced, apply a placeholder as a failafe.
				anim->motion = KlonoaEvANM[1]->getmotion();

			if (speed > 1.0f)
				speed = 1.0f;
		}
	}

	if (anim == KlonoaAnimList[11].Animation && !IsIngame())
	{
		speed /= 1.7f;
	}

	EV_SetAction_t.Original(obj, anim, tex, speed, mode, linkframe);
}

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
		mov dword ptr[ebp + 140h], offset SonicAnimData
			jmp loc_49AB51
	}
}

void SetKlonoaAnims()
{
	//Init new anim list, put falling animation as a placeholder / failsafe for animation that don't have a SADX counterpart
	for (int i = 0; i < LengthOfArray(KlonoaAnimList); i++)
	{
		KlonoaAnimList[i].Animation = new NJS_ACTION;
		KlonoaAnimList[i].Animation->object = KlonoaMDL->getmodel();
		KlonoaAnimList[i].Animation->motion = KlonoaANM[anmID_fall]->getmotion();
		KlonoaAnimList[i].Instance = KlonoaANM[anmID_fall]->getmodelcount();
		KlonoaAnimList[i].Property = 4;
		KlonoaAnimList[i].NextAnim = 18;
		KlonoaAnimList[i].TransitionSpeed = 0.25f;
		KlonoaAnimList[i].AnimationSpeed = 0.5f;
	}

	KlonoaAnimList[0].Animation->motion = KlonoaANM[anmID_stand]->getmotion();
	KlonoaAnimList[0].AnimationSpeed = 0.5f;
	KlonoaAnimList[0].NextAnim = 0;
	KlonoaAnimList[0].Property = 3;
	KlonoaAnimList[0].TransitionSpeed = 0.0625f;

	//surf stance
	KlonoaAnimList[1].Animation->motion = KlonoaANM[anmID_battleStd]->getmotion();
	KlonoaAnimList[1].TransitionSpeed = 0.25f;
	KlonoaAnimList[1].AnimationSpeed = 0.50f;
	KlonoaAnimList[1].NextAnim = 2;

	KlonoaAnimList[2].Animation->motion = KlonoaANM[anmID_battleStd]->getmotion();
	KlonoaAnimList[2].TransitionSpeed = 0.25f;
	KlonoaAnimList[2].AnimationSpeed = 0.50f;
	KlonoaAnimList[2].NextAnim = 2;

	KlonoaAnimList[7] = KlonoaAnimList[0];
	KlonoaAnimList[8] = KlonoaAnimList[0];

	//walk
	KlonoaAnimList[9].Animation->motion = KlonoaANM[anmID_walk]->getmotion();
	KlonoaAnimList[9].AnimationSpeed = 1.5f;
	KlonoaAnimList[9].NextAnim = 9;
	KlonoaAnimList[9].Property = 10;
	KlonoaAnimList[9].TransitionSpeed = 0.15f;

	//speed walk
	KlonoaAnimList[10].Animation->motion = KlonoaANM[anmID_run]->getmotion();
	KlonoaAnimList[10].AnimationSpeed = 0.3f;
	KlonoaAnimList[10].NextAnim = 10;
	KlonoaAnimList[10].Property = 10;
	KlonoaAnimList[10].TransitionSpeed = 0.20f;

	//jogging
	KlonoaAnimList[11].Animation->motion = KlonoaANM[anmID_run]->getmotion();
	KlonoaAnimList[11].Property = 10;
	KlonoaAnimList[11].NextAnim = 11;
	KlonoaAnimList[11].TransitionSpeed = 0.25f;
	KlonoaAnimList[11].AnimationSpeed = 0.2f;

	//run
	KlonoaAnimList[12].Animation->motion = KlonoaANM[anmID_run]->getmotion();
	KlonoaAnimList[12].Property = 10;
	KlonoaAnimList[12].NextAnim = 12;
	KlonoaAnimList[12].TransitionSpeed = 0.25f;
	KlonoaAnimList[12].AnimationSpeed = 0.2f;

	//sprint
	KlonoaAnimList[13].Animation->motion = KlonoaANM[anmID_run]->getmotion();
	KlonoaAnimList[13].Property = 9;
	KlonoaAnimList[13].NextAnim = 13;
	KlonoaAnimList[13].TransitionSpeed = 0.5f;
	KlonoaAnimList[13].AnimationSpeed = 0.2f;

	//falling
	KlonoaAnimList[18].Animation->motion = KlonoaANM[anmID_fall]->getmotion();
	KlonoaAnimList[18].Property = 3;
	KlonoaAnimList[18].NextAnim = 18;
	KlonoaAnimList[18].TransitionSpeed = 0.25f;
	KlonoaAnimList[18].AnimationSpeed = 0.5f;

	//idle
	KlonoaAnimList[4].Animation->motion = KlonoaANM[anmID_idle]->getmotion();
	KlonoaAnimList[4].Property = 4;
	KlonoaAnimList[4].NextAnim = 5;
	KlonoaAnimList[4].TransitionSpeed = 0.25f;
	KlonoaAnimList[4].AnimationSpeed = 0.5f;

	KlonoaAnimList[5].Animation->motion = KlonoaANM[anmID_idle]->getmotion();
	KlonoaAnimList[5].NextAnim = 6;
	KlonoaAnimList[5].TransitionSpeed = 0.25f;
	KlonoaAnimList[5].AnimationSpeed = 0.5f;

	KlonoaAnimList[6].Animation->motion = KlonoaANM[anmID_idle]->getmotion();
	KlonoaAnimList[6].NextAnim = 0;
	KlonoaAnimList[6].TransitionSpeed = 0.25f;
	KlonoaAnimList[6].AnimationSpeed = 1.0f;

	//jump
	KlonoaAnimList[14].Animation->motion = KlonoaANM[anmID_jump]->getmotion();
	KlonoaAnimList[14].Property = 10;
	KlonoaAnimList[14].NextAnim = 14;
	KlonoaAnimList[14].TransitionSpeed = 0.25f;
	KlonoaAnimList[14].AnimationSpeed = 0.07f;

	//spring
	KlonoaAnimList[16].Animation->motion = KlonoaANM[anmID_jump]->getmotion();
	KlonoaAnimList[16].Property = 3;
	KlonoaAnimList[16].NextAnim = 16;;
	KlonoaAnimList[16].TransitionSpeed = 0.25f;
	KlonoaAnimList[16].AnimationSpeed = 0.5f;

	//landing
	KlonoaAnimList[19].Animation->motion = KlonoaANM[animID_Landing]->getmotion();
	KlonoaAnimList[19].NextAnim = 0;
	KlonoaAnimList[19].TransitionSpeed = 0.25f;
	KlonoaAnimList[19].AnimationSpeed = 0.5f;

	//brake
	KlonoaAnimList[20].Animation->motion = KlonoaANM[animID_Brake]->getmotion();
	KlonoaAnimList[20].NextAnim = 0;
	KlonoaAnimList[20].TransitionSpeed = 0.25f;
	KlonoaAnimList[20].AnimationSpeed = 0.8f;

	//push start
	KlonoaAnimList[21].Animation->motion = KlonoaANM[anmID_push]->getmotion();
	KlonoaAnimList[21].NextAnim = 22;
	KlonoaAnimList[21].TransitionSpeed = 0.25f;
	KlonoaAnimList[21].AnimationSpeed = 0.5f;

	//push
	KlonoaAnimList[22].Animation->motion = KlonoaANM[anmID_push]->getmotion();
	KlonoaAnimList[22].NextAnim = 22;
	KlonoaAnimList[22].Property = 3;
	KlonoaAnimList[22].TransitionSpeed = 0.25f;
	KlonoaAnimList[22].AnimationSpeed = 0.5f;

	//hurt
	KlonoaAnimList[23].Animation->motion = KlonoaANM[anmID_hurt]->getmotion();
	KlonoaAnimList[23].NextAnim = 23;
	KlonoaAnimList[23].Property = 5;
	KlonoaAnimList[23].TransitionSpeed = 0.25f;
	KlonoaAnimList[23].AnimationSpeed = 0.4f;

	//turn around
	KlonoaAnimList[30] = KlonoaAnimList[20];

	//pick obj (throw anim reversed for now)
	KlonoaAnimList[33].Animation->motion = KlonoaANM[anmID_throw]->getmotion();
	KlonoaAnimList[33].NextAnim = 37;
	KlonoaAnimList[33].TransitionSpeed = 0.25f;
	KlonoaAnimList[33].AnimationSpeed = 0.4f;
	KlonoaAnimList[33].Property = 6; //reverse

	//obj run
	KlonoaAnimList[34].Animation->motion = KlonoaANM[anmID_holdRun]->getmotion();
	KlonoaAnimList[34].NextAnim = 34;
	KlonoaAnimList[34].AnimationSpeed = 0.5f;
	KlonoaAnimList[34].Property = 3;

	//obj stand
	KlonoaAnimList[37].Animation->motion = KlonoaANM[animID_holdStd]->getmotion();
	KlonoaAnimList[37].Property = 3;
	KlonoaAnimList[37].NextAnim = 37;
	KlonoaAnimList[37].TransitionSpeed = 0.25f;
	KlonoaAnimList[37].AnimationSpeed = 0.5f;

	//obj walk
	KlonoaAnimList[38].Animation->motion = KlonoaANM[anmID_holdRun]->getmotion();
	KlonoaAnimList[38].NextAnim = 38;
	KlonoaAnimList[38].AnimationSpeed = 0.3f;
	KlonoaAnimList[38].Property = 3;

	//trampoline
	KlonoaAnimList[39].Animation->motion = KlonoaANM[anmID_jump]->getmotion();
	KlonoaAnimList[39].Property = 3;
	KlonoaAnimList[39].NextAnim = 16;
	KlonoaAnimList[39].TransitionSpeed = 0.25f;
	KlonoaAnimList[39].AnimationSpeed = 0.5f;

	KlonoaAnimList[40].Animation->motion = KlonoaANM[anmID_spinning]->getmotion();
	KlonoaAnimList[40].NextAnim = 18;
	KlonoaAnimList[40].TransitionSpeed = 0.25f;
	KlonoaAnimList[40].AnimationSpeed = 0.5f;

	KlonoaAnimList[41].Animation->motion = KlonoaANM[anmID_jump]->getmotion();
	KlonoaAnimList[41].Property = 3;
	KlonoaAnimList[41].NextAnim = 16;
	KlonoaAnimList[41].TransitionSpeed = 0.25f;
	KlonoaAnimList[41].AnimationSpeed = 0.5f;

	KlonoaAnimList[42].Animation->motion = KlonoaANM[anmID_spinning]->getmotion();
	KlonoaAnimList[42].NextAnim = 18;
	KlonoaAnimList[42].TransitionSpeed = 0.25f;
	KlonoaAnimList[42].AnimationSpeed = 0.7f;

	KlonoaAnimList[43].Animation->motion = KlonoaANM[anmID_spinning]->getmotion();
	KlonoaAnimList[43].Property = 3;
	KlonoaAnimList[43].NextAnim = 16;
	KlonoaAnimList[43].TransitionSpeed = 0.25f;
	KlonoaAnimList[43].AnimationSpeed = 1.0f;

	//hang pulley / heli
	KlonoaAnimList[47].Animation->motion = KlonoaANM[anmID_holdOne]->getmotion();
	KlonoaAnimList[47].Property = 3;
	KlonoaAnimList[47].NextAnim = 47;
	KlonoaAnimList[47].TransitionSpeed = 0.5f;
	KlonoaAnimList[47].AnimationSpeed = 0.5f;

	//shake tree
	KlonoaAnimList[49].Animation->motion = KlonoaANM[anmID_shake]->getmotion();
	KlonoaAnimList[49].NextAnim = 49;
	KlonoaAnimList[49].TransitionSpeed = 0.5f;
	KlonoaAnimList[49].Property = 3;
	KlonoaAnimList[49].AnimationSpeed = 0.2f;

	//set shake (throw anim for now)
	KlonoaAnimList[50].Animation->motion = KlonoaANM[anmID_throw]->getmotion();
	KlonoaAnimList[50].NextAnim = 51;
	KlonoaAnimList[50].TransitionSpeed = 0.25f;
	KlonoaAnimList[50].AnimationSpeed = 0.3f;

	//shaking
	KlonoaAnimList[51].Animation->motion = KlonoaANM[anmID_shake]->getmotion();
	KlonoaAnimList[51].NextAnim = 51;
	KlonoaAnimList[51].Property = 3;
	KlonoaAnimList[51].TransitionSpeed = 0.25f;
	KlonoaAnimList[51].AnimationSpeed = 0.2f;

	//stop shaking
	KlonoaAnimList[52].Animation->motion = KlonoaANM[anmID_holdObj]->getmotion();
	KlonoaAnimList[52].NextAnim = 37;
	KlonoaAnimList[52].TransitionSpeed = 0.25f;
	KlonoaAnimList[52].AnimationSpeed = 0.4f;

	//idle hold tree
	KlonoaAnimList[53].Animation->motion = KlonoaANM[anmID_holdObj]->getmotion();
	KlonoaAnimList[53].NextAnim = 54;
	KlonoaAnimList[53].TransitionSpeed = 0.5f;
	KlonoaAnimList[53].AnimationSpeed = 0.5f;

	KlonoaAnimList[54].Animation->motion = KlonoaANM[anmID_holdObj]->getmotion();
	KlonoaAnimList[54].Property = 3;
	KlonoaAnimList[54].NextAnim = 54;
	KlonoaAnimList[54].TransitionSpeed = 0.5f;
	KlonoaAnimList[54].AnimationSpeed = 0.5f;

	//object jump
	KlonoaAnimList[55].Animation->motion = KlonoaANM[anmID_holdJump]->getmotion();
	KlonoaAnimList[55].NextAnim = 55;
	KlonoaAnimList[55].AnimationSpeed = 0.5f;
	KlonoaAnimList[55].Property = 3;

	//object fall
	KlonoaAnimList[56].Animation->motion = KlonoaANM[anmID_holdFall]->getmotion();
	KlonoaAnimList[56].NextAnim = 57;
	KlonoaAnimList[56].AnimationSpeed = 0.5f;

	KlonoaAnimList[57].Animation->motion = KlonoaANM[anmID_holdFall]->getmotion();
	KlonoaAnimList[57].NextAnim = 57;
	KlonoaAnimList[57].AnimationSpeed = 0.5f;
	KlonoaAnimList[57].Property = 3;

	//Throw object
	KlonoaAnimList[59].Animation->motion = KlonoaANM[anmID_throw]->getmotion();
	KlonoaAnimList[59].NextAnim = anm_std;
	KlonoaAnimList[59].AnimationSpeed = 0.2f;

	KlonoaAnimList[60] = KlonoaAnimList[59];

	KlonoaAnimList[61] = KlonoaAnimList[59];
	KlonoaAnimList[61].NextAnim = 18;

	//hold obj
	KlonoaAnimList[62].Animation->motion = KlonoaANM[anmID_holdObj]->getmotion();
	KlonoaAnimList[62].Property = 4;
	KlonoaAnimList[62].NextAnim = 18;
	KlonoaAnimList[62].TransitionSpeed = 0.5;
	KlonoaAnimList[62].AnimationSpeed = 0.5f;

	KlonoaAnimList[63] = KlonoaAnimList[59];

	//Light Dash
	KlonoaAnimList[64].Animation->motion = KlonoaANM[anmID_fly]->getmotion();
	KlonoaAnimList[64].NextAnim = 31;
	KlonoaAnimList[64].TransitionSpeed = 0.125;
	KlonoaAnimList[64].AnimationSpeed = 1.0f;
	KlonoaAnimList[64].Property = 3;

	//cart
	KlonoaAnimList[65].Animation->motion = KlonoaANM[anmID_holdObj]->getmotion();
	KlonoaAnimList[65].NextAnim = 65;
	KlonoaAnimList[65].TransitionSpeed = 0.125f;
	KlonoaAnimList[65].AnimationSpeed = 1.0f;
	KlonoaAnimList[65].Property = 5;

	//climbing ladder
	KlonoaAnimList[69].Animation->motion = KlonoaANM[anmID_Climbing]->getmotion();
	KlonoaAnimList[69].AnimationSpeed = 0.1f;
	KlonoaAnimList[69].NextAnim = 69;
	KlonoaAnimList[69].Property = 12;
	KlonoaAnimList[69].TransitionSpeed = 1.0f;

	//shot from cannon
	KlonoaAnimList[70].Animation->motion = KlonoaANM[anmID_fly]->getmotion();
	KlonoaAnimList[70].NextAnim = 70;
	KlonoaAnimList[70].TransitionSpeed = 0.125;
	KlonoaAnimList[70].AnimationSpeed = 1.0f;
	KlonoaAnimList[70].Property = 3;

	KlonoaAnimList[74].Animation->motion = KlonoaANM[anmID_jmpPanel]->getmotion();
	KlonoaAnimList[74].NextAnim = 74;
	KlonoaAnimList[74].Property = 3;
	KlonoaAnimList[74].TransitionSpeed = 0.25f;
	KlonoaAnimList[74].AnimationSpeed = 0.5f;

	//victory.
	KlonoaAnimList[75].Animation->motion = KlonoaANM[anmID_victory]->getmotion();
	KlonoaAnimList[75].NextAnim = 75;
	KlonoaAnimList[75].TransitionSpeed = 0.5f;
	KlonoaAnimList[75].AnimationSpeed = 0.5f;
	KlonoaAnimList[75].Property = 12;

	//victory standing.
	KlonoaAnimList[76].Animation->motion = KlonoaANM[anmID_victoryStd]->getmotion();
	KlonoaAnimList[76].NextAnim = 0;
	KlonoaAnimList[76].Property = 3;
	KlonoaAnimList[76].TransitionSpeed = 0.5f;
	KlonoaAnimList[76].AnimationSpeed = 0.5f;

	KlonoaAnimList[77].Animation->motion = KlonoaANM[anmID_holdObj]->getmotion();
	KlonoaAnimList[77].NextAnim = 78;
	KlonoaAnimList[77].TransitionSpeed = 0.125;
	KlonoaAnimList[77].AnimationSpeed = 1.0f;

	KlonoaAnimList[78] = KlonoaAnimList[77];
	KlonoaAnimList[78].NextAnim = 78;
	KlonoaAnimList[78].Property = 3;

	//hold rocket h
	KlonoaAnimList[79].Animation->motion = KlonoaANM[anmID_holdRocketV]->getmotion();
	KlonoaAnimList[79].NextAnim = 79;
	KlonoaAnimList[79].TransitionSpeed = 0.125;
	KlonoaAnimList[79].AnimationSpeed = 1.0f;
	KlonoaAnimList[79].Property = 3;

	//hold rocket vertical
	KlonoaAnimList[80] = KlonoaAnimList[79];
	KlonoaAnimList[80].NextAnim = 80;

	//begin death
	KlonoaAnimList[86].Animation->motion = KlonoaANM[anmID_death]->getmotion();
	KlonoaAnimList[86].TransitionSpeed = 0.125;
	KlonoaAnimList[86].AnimationSpeed = 1.0f;

	//death
	KlonoaAnimList[87].Animation->motion = KlonoaANM[anmID_death]->getmotion();
	KlonoaAnimList[87].NextAnim = 87;
	KlonoaAnimList[87].TransitionSpeed = 0.125;
	KlonoaAnimList[87].AnimationSpeed = 1.0f;
	KlonoaAnimList[87].Property = 5;

	//Holding onto icicle
	KlonoaAnimList[94].Animation->motion = KlonoaANM[anmID_holdIcicle]->getmotion();
	KlonoaAnimList[94].NextAnim = 94;
	KlonoaAnimList[94].TransitionSpeed = 0.125;
	KlonoaAnimList[94].AnimationSpeed = 0.5f;
	KlonoaAnimList[94].Property = 3;

	//board
	KlonoaAnimList[102].Animation->motion = KlonoaANM[animID_Board]->getmotion();
	KlonoaAnimList[102].NextAnim = 102;
	KlonoaAnimList[102].TransitionSpeed = 0.125;
	KlonoaAnimList[102].AnimationSpeed = 1.0f;
	KlonoaAnimList[102].Property = 3;

	//board brake right
	KlonoaAnimList[103].Animation->motion = KlonoaANM[animID_BoardRight]->getmotion();
	KlonoaAnimList[103].NextAnim = 103;
	KlonoaAnimList[103].TransitionSpeed = 0.125;
	KlonoaAnimList[103].AnimationSpeed = 0.5f;
	KlonoaAnimList[103].Property = 5;

	//board brake left
	KlonoaAnimList[104].Animation->motion = KlonoaANM[animID_BoardLeft]->getmotion();
	KlonoaAnimList[104].NextAnim = 104;
	KlonoaAnimList[104].TransitionSpeed = 0.125;
	KlonoaAnimList[104].AnimationSpeed = 0.5f;
	KlonoaAnimList[104].Property = 5;

	//falling board
	KlonoaAnimList[105].Animation->motion = KlonoaANM[animID_FallBoard]->getmotion();
	KlonoaAnimList[105].NextAnim = 111;
	KlonoaAnimList[105].TransitionSpeed = 0.125;
	KlonoaAnimList[105].AnimationSpeed = 0.2f;
	KlonoaAnimList[105].Property = 4;

	//jump board
	KlonoaAnimList[106].Animation->motion = KlonoaANM[animID_BoardJump]->getmotion();
	KlonoaAnimList[106].NextAnim = 105;
	KlonoaAnimList[106].TransitionSpeed = 0.125;
	KlonoaAnimList[106].AnimationSpeed = 0.5f;
	KlonoaAnimList[106].Property = 4;

	//board right side
	KlonoaAnimList[107].Animation->motion = KlonoaANM[animID_BoardRight]->getmotion();
	KlonoaAnimList[107].NextAnim = 107;
	KlonoaAnimList[107].TransitionSpeed = 0.125;
	KlonoaAnimList[107].AnimationSpeed = 0.3f;
	KlonoaAnimList[107].Property = 12;

	//board left side
	KlonoaAnimList[108].Animation->motion = KlonoaANM[animID_BoardLeft]->getmotion();
	KlonoaAnimList[108].NextAnim = 108;
	KlonoaAnimList[108].TransitionSpeed = 0.125;
	KlonoaAnimList[108].AnimationSpeed = 0.3f;
	KlonoaAnimList[108].Property = 12;

	KlonoaAnimList[109] = KlonoaAnimList[107];
	KlonoaAnimList[109].NextAnim = 114;

	KlonoaAnimList[110] = KlonoaAnimList[108];
	KlonoaAnimList[110].NextAnim = 114;

	KlonoaAnimList[111] = KlonoaAnimList[105];
	KlonoaAnimList[111].NextAnim = 112;
	KlonoaAnimList[112] = KlonoaAnimList[105];
	KlonoaAnimList[112].NextAnim = 112;

	KlonoaAnimList[113] = KlonoaAnimList[105];
	KlonoaAnimList[113].NextAnim = 102;

	KlonoaAnimList[114] = KlonoaAnimList[102];
	KlonoaAnimList[114].NextAnim = 114;
	KlonoaAnimList[114].Property = 3;

	//losing balance, don't have a counterpart so stick to neutral
	KlonoaAnimList[115] = KlonoaAnimList[102];

	//trick 1
	KlonoaAnimList[116].Animation->motion = KlonoaANM[animID_trick1]->getmotion();
	KlonoaAnimList[116].NextAnim = 112;
	KlonoaAnimList[116].TransitionSpeed = 0.125;
	KlonoaAnimList[116].AnimationSpeed = 0.5f;

	//trick 2
	KlonoaAnimList[117].Animation->motion = KlonoaANM[animID_trick2]->getmotion();
	KlonoaAnimList[117].NextAnim = 112;
	KlonoaAnimList[117].TransitionSpeed = 0.125;
	KlonoaAnimList[117].AnimationSpeed = 0.5f;

	//trick 3
	KlonoaAnimList[118].Animation->motion = KlonoaANM[animID_trick3]->getmotion();
	KlonoaAnimList[118].NextAnim = 112;
	KlonoaAnimList[118].TransitionSpeed = 0.125;
	KlonoaAnimList[118].AnimationSpeed = 0.5f;

	KlonoaAnimList[119] = KlonoaAnimList[118];
	KlonoaAnimList[119].Property = 6;
	KlonoaAnimList[119].NextAnim = 112;

	KlonoaAnimList[120] = KlonoaAnimList[118];
	KlonoaAnimList[120].NextAnim = 121;
	KlonoaAnimList[121] = KlonoaAnimList[117];
	KlonoaAnimList[121].NextAnim = 122;
	KlonoaAnimList[122] = KlonoaAnimList[116];
	KlonoaAnimList[122].NextAnim = 112;
	KlonoaAnimList[123] = KlonoaAnimList[117];

	//crouch
	KlonoaAnimList[124].Animation->motion = KlonoaANM[animID_Crouch]->getmotion();
	KlonoaAnimList[124].NextAnim = 124;
	KlonoaAnimList[124].TransitionSpeed = 0.125;
	KlonoaAnimList[124].AnimationSpeed = 0.3f;
	KlonoaAnimList[124].Property = 3;

	//showers
	KlonoaAnimList[126].Animation->motion = KlonoaANM[anmID_shower]->getmotion();
	KlonoaAnimList[126].NextAnim = 127;
	KlonoaAnimList[126].TransitionSpeed = 0.125;
	KlonoaAnimList[126].AnimationSpeed = 0.25f;

	KlonoaAnimList[127].Animation->motion = KlonoaANM[anmID_shower2]->getmotion();
	KlonoaAnimList[127].NextAnim = 128;
	KlonoaAnimList[127].TransitionSpeed = 0.125;
	KlonoaAnimList[127].AnimationSpeed = 0.25f;

	KlonoaAnimList[128] = KlonoaAnimList[127];
	KlonoaAnimList[128].NextAnim = 0;

	//Chao Pet
	KlonoaAnimList[130].Animation->motion = KlonoaANM[anmID_PetSet]->getmotion();
	KlonoaAnimList[130].NextAnim = 131;
	KlonoaAnimList[130].TransitionSpeed = 1.0f;
	KlonoaAnimList[130].AnimationSpeed = 1.0f;

	KlonoaAnimList[131].Animation->motion = KlonoaANM[anmID_PetLoop]->getmotion();
	KlonoaAnimList[131].NextAnim = 131;
	KlonoaAnimList[131].TransitionSpeed = 1.0f;
	KlonoaAnimList[131].AnimationSpeed = 0.5f;
	KlonoaAnimList[131].Property = 3;

	KlonoaAnimList[132].Animation->motion = KlonoaANM[anmID_PetEnd]->getmotion();
	KlonoaAnimList[132].NextAnim = 0;
	KlonoaAnimList[132].TransitionSpeed = 1.0f;
	KlonoaAnimList[132].AnimationSpeed = 0.8f;


	KlonoaAnimList[133].Animation->motion = KlonoaANM[anmID_whistle]->getmotion();
	KlonoaAnimList[133].NextAnim = 130;
	KlonoaAnimList[133].TransitionSpeed = 1.0f;
	KlonoaAnimList[133].AnimationSpeed = 1.0f;

	//Super Klonoa anims (to do: use SS anim here maybe?)

	//standing SK
	KlonoaAnimList[134].Animation->motion = KlonoaAnimList[0].Animation->motion;
	KlonoaAnimList[134].AnimationSpeed = KlonoaAnimList[0].AnimationSpeed;
	KlonoaAnimList[134].Property = KlonoaAnimList[0].Property;
	KlonoaAnimList[134].TransitionSpeed = KlonoaAnimList[0].TransitionSpeed;
	KlonoaAnimList[134].NextAnim = 134;

	//standing -> moving SK
	KlonoaAnimList[135].Animation->motion = KlonoaAnimList[9].Animation->motion;
	KlonoaAnimList[135].AnimationSpeed = KlonoaAnimList[9].AnimationSpeed;
	KlonoaAnimList[135].Property = KlonoaAnimList[9].Property;
	KlonoaAnimList[135].TransitionSpeed = KlonoaAnimList[9].TransitionSpeed;
	KlonoaAnimList[135].NextAnim = 135;

	//moving 1
	KlonoaAnimList[136].Animation->motion = KlonoaAnimList[11].Animation->motion;
	KlonoaAnimList[136].AnimationSpeed = KlonoaAnimList[11].AnimationSpeed;
	KlonoaAnimList[136].Property = KlonoaAnimList[11].Property;
	KlonoaAnimList[136].TransitionSpeed = KlonoaAnimList[11].TransitionSpeed;
	KlonoaAnimList[136].NextAnim = 136;

	//moving 2
	KlonoaAnimList[137].Animation->motion = KlonoaAnimList[12].Animation->motion;
	KlonoaAnimList[137].AnimationSpeed = KlonoaAnimList[12].AnimationSpeed;
	KlonoaAnimList[137].Property = KlonoaAnimList[12].Property;
	KlonoaAnimList[137].TransitionSpeed = KlonoaAnimList[12].TransitionSpeed;
	KlonoaAnimList[137].NextAnim = 137;

	//moving 3
	KlonoaAnimList[138].Animation->motion = KlonoaAnimList[13].Animation->motion;
	KlonoaAnimList[138].AnimationSpeed = KlonoaAnimList[13].AnimationSpeed;
	KlonoaAnimList[138].Property = KlonoaAnimList[13].Property;
	KlonoaAnimList[138].TransitionSpeed = KlonoaAnimList[13].TransitionSpeed;
	KlonoaAnimList[138].NextAnim = 138;

	//landing SK
	KlonoaAnimList[142].Animation->motion = KlonoaAnimList[19].Animation->motion;
	KlonoaAnimList[142].AnimationSpeed = KlonoaAnimList[19].AnimationSpeed;
	KlonoaAnimList[142].Property = KlonoaAnimList[19].Property;
	KlonoaAnimList[142].TransitionSpeed = KlonoaAnimList[19].TransitionSpeed;
	KlonoaAnimList[142].NextAnim = 134;

	//victory SK
	KlonoaAnimList[143].Animation->motion = KlonoaAnimList[75].Animation->motion;
	KlonoaAnimList[143].AnimationSpeed = KlonoaAnimList[75].AnimationSpeed;
	KlonoaAnimList[143].Property = KlonoaAnimList[75].Property;
	KlonoaAnimList[143].TransitionSpeed = KlonoaAnimList[75].TransitionSpeed;
	KlonoaAnimList[143].NextAnim = 143;

	KlonoaAnimList[144].Animation->motion = KlonoaAnimList[76].Animation->motion;
	KlonoaAnimList[144].AnimationSpeed = KlonoaAnimList[76].AnimationSpeed;
	KlonoaAnimList[144].Property = KlonoaAnimList[76].Property;
	KlonoaAnimList[144].TransitionSpeed = KlonoaAnimList[76].TransitionSpeed;
	KlonoaAnimList[144].NextAnim = 144;

	KlonoaAnimList[145].Animation->motion = KlonoaAnimList[14].Animation->motion;
	KlonoaAnimList[145].AnimationSpeed = KlonoaAnimList[14].AnimationSpeed;
	KlonoaAnimList[145].Property = KlonoaAnimList[14].Property;
	KlonoaAnimList[145].TransitionSpeed = KlonoaAnimList[14].TransitionSpeed;
	KlonoaAnimList[145].NextAnim = 145;

	//hover
	KlonoaAnimList[anm_hover].Animation->motion = KlonoaANM[anmID_hover]->getmotion();
	KlonoaAnimList[anm_hover].Property = 3;
	KlonoaAnimList[anm_hover].NextAnim = anm_hover;
	KlonoaAnimList[anm_hover].TransitionSpeed = 0.25f;
	KlonoaAnimList[anm_hover].AnimationSpeed = 0.5f;

	//bullet start
	KlonoaAnimList[anm_windBullet].Animation->motion = KlonoaANM[anmID_bulletStart]->getmotion();
	KlonoaAnimList[anm_windBullet].Property = 4;
	KlonoaAnimList[anm_windBullet].NextAnim = anm_windBulletEnd;
	KlonoaAnimList[anm_windBullet].TransitionSpeed = 0.25f;
	KlonoaAnimList[anm_windBullet].AnimationSpeed = 0.5f;

	//bullet mid air
	KlonoaAnimList[anm_windBulletAir].Animation->motion = KlonoaANM[animID_bulletMidAir]->getmotion();
	KlonoaAnimList[anm_windBulletAir].Property = 4;
	KlonoaAnimList[anm_windBulletAir].NextAnim = anm_fall;
	KlonoaAnimList[anm_windBulletAir].TransitionSpeed = 0.25f;
	KlonoaAnimList[anm_windBulletAir].AnimationSpeed = 0.5f;

	//bullet end
	KlonoaAnimList[anm_windBulletEnd].Animation->motion = KlonoaANM[animID_bulletEnd]->getmotion();
	KlonoaAnimList[anm_windBulletEnd].Property = 4;
	KlonoaAnimList[anm_windBulletEnd].NextAnim = 0;
	KlonoaAnimList[anm_windBulletEnd].TransitionSpeed = 0.25f;
	KlonoaAnimList[anm_windBulletEnd].AnimationSpeed = 0.5f;

	//hold enemy std
	KlonoaAnimList[anm_holdStd].Animation->motion = KlonoaANM[animID_holdStd]->getmotion();
	KlonoaAnimList[anm_holdStd].Property = 3;
	KlonoaAnimList[anm_holdStd].NextAnim = anm_holdStd;
	KlonoaAnimList[anm_holdStd].TransitionSpeed = 0.25f;
	KlonoaAnimList[anm_holdStd].AnimationSpeed = 0.5f;

	//hold enemy wlk
	KlonoaAnimList[anm_holdWalk].Animation->motion = KlonoaANM[anmID_holdRun]->getmotion();
	KlonoaAnimList[anm_holdWalk].NextAnim = anm_holdWalk;
	KlonoaAnimList[anm_holdWalk].AnimationSpeed = 0.3f;
	KlonoaAnimList[anm_holdWalk].Property = 3;

	//hold enemy run
	KlonoaAnimList[anm_holdRun].Animation->motion = KlonoaANM[anmID_holdRun]->getmotion();
	KlonoaAnimList[anm_holdRun].NextAnim = anm_holdRun;
	KlonoaAnimList[anm_holdRun].AnimationSpeed = 0.5f;
	KlonoaAnimList[anm_holdRun].Property = 3;

	//hold enemy jump
	KlonoaAnimList[anm_holdJump].Animation->motion = KlonoaANM[anmID_holdJump]->getmotion();
	KlonoaAnimList[anm_holdJump].NextAnim = anm_holdJump;
	KlonoaAnimList[anm_holdJump].AnimationSpeed = 0.5f;
	KlonoaAnimList[anm_holdJump].Property = 3;

	//hold enemy fall
	KlonoaAnimList[anm_holdFall].Animation->motion = KlonoaANM[anmID_holdFall]->getmotion();
	KlonoaAnimList[anm_holdFall].NextAnim = anm_holdFall;
	KlonoaAnimList[anm_holdFall].AnimationSpeed = 0.5f;
	KlonoaAnimList[anm_holdFall].Property = 3;

	//throw enemy
	KlonoaAnimList[anm_throwStd].Animation->motion = KlonoaANM[anmID_throw]->getmotion();
	KlonoaAnimList[anm_throwStd].NextAnim = anm_std;
	KlonoaAnimList[anm_throwStd].AnimationSpeed = 0.5f;
	KlonoaAnimList[anm_throwStd].Property = 4;

	//fly standing
	KlonoaAnimList[anm_flyStd].Animation->motion = KlonoaANM[anmID_flyStd]->getmotion();
	KlonoaAnimList[anm_flyStd].NextAnim = anm_flyStd;
	KlonoaAnimList[anm_flyStd].AnimationSpeed = 0.7f;
	KlonoaAnimList[anm_flyStd].Property = 3;

	for (int j = 134; j < 146; j++)
	{
		if (KlonoaAnimList[j].Animation->motion)
		{
			KlonoaAnimList[j].Animation->object = SuperKlonoaMDL->getmodel();
		}
	}
}

void LoadKlonoa_AnimFiles()
{
	KlonoaANM[0] = LoadObjectAnim("stand");
	KlonoaANM[1] = LoadObjectAnim("death");
	KlonoaANM[2] = LoadObjectAnim("fall");
	KlonoaANM[3] = LoadObjectAnim("walk");
	KlonoaANM[4] = LoadObjectAnim("fly");
	KlonoaANM[5] = LoadObjectAnim("hover");
	KlonoaANM[6] = LoadObjectAnim("run");
	KlonoaANM[7] = LoadObjectAnim("idle");
	KlonoaANM[8] = LoadObjectAnim("jump");
	KlonoaANM[9] = LoadObjectAnim("holdOneHand");
	KlonoaANM[10] = LoadObjectAnim("holdObj");
	KlonoaANM[11] = LoadObjectAnim("Victory");
	KlonoaANM[12] = LoadObjectAnim("Victory2");
	KlonoaANM[13] = LoadObjectAnim("standVictory");
	KlonoaANM[14] = LoadObjectAnim("standBattle");
	KlonoaANM[15] = LoadObjectAnim("bulletStart");
	KlonoaANM[16] = LoadObjectAnim("bulletMidAir");
	KlonoaANM[17] = LoadObjectAnim("bulletEnd");
	KlonoaANM[18] = LoadObjectAnim("holdStand");
	KlonoaANM[19] = LoadObjectAnim("holdWalk");
	KlonoaANM[20] = LoadObjectAnim("holdRun");
	KlonoaANM[21] = LoadObjectAnim("holdJump");
	KlonoaANM[22] = LoadObjectAnim("holdFall");
	KlonoaANM[23] = LoadObjectAnim("throw");
	KlonoaANM[24] = LoadObjectAnim("airThrow"); //TO DO ADD THROW AIR
	KlonoaANM[25] = LoadObjectAnim("Brake");
	KlonoaANM[26] = LoadObjectAnim("Landing");
	KlonoaANM[27] = LoadObjectAnim("Board");
	KlonoaANM[28] = LoadObjectAnim("Crouch");
	KlonoaANM[29] = LoadObjectAnim("FallBoard");
	KlonoaANM[30] = LoadObjectAnim("trick1");
	KlonoaANM[31] = LoadObjectAnim("trick2");
	KlonoaANM[32] = LoadObjectAnim("trick3");
	KlonoaANM[33] = LoadObjectAnim("BoardLeft");
	KlonoaANM[34] = LoadObjectAnim("BoardRight");
	KlonoaANM[35] = LoadObjectAnim("BoardJump");
	KlonoaANM[36] = LoadObjectAnim("flyStd");
	//	KlonoaANM[37] = LoadObjectAnim("cart"); //MEH
	KlonoaANM[38] = LoadObjectAnim("icicleHold");
	KlonoaANM[39] = LoadObjectAnim("climb");
	KlonoaANM[40] = LoadObjectAnim("spin");
	KlonoaANM[41] = LoadObjectAnim("jmpPanel");
	KlonoaANM[42] = LoadObjectAnim("push");
	KlonoaANM[43] = LoadObjectAnim("hurt");
	KlonoaANM[44] = LoadObjectAnim("holdRocketV");
	KlonoaANM[45] = LoadObjectAnim("shower");
	KlonoaANM[46] = LoadObjectAnim("shower2");
	KlonoaANM[47] = LoadObjectAnim("Whistle");
	KlonoaANM[48] = LoadObjectAnim("PetSet");
	KlonoaANM[49] = LoadObjectAnim("shake");
	KlonoaANM[50] = LoadObjectAnim("PetLoop");
	KlonoaANM[51] = LoadObjectAnim("PetEnd");

}

void InitKlonoaCharSelAnim()
{
	SonicCharSelAnim[0] = KlonoaAnimList[11].Animation;
	SonicCharSelAnim[1] = KlonoaAnimList[75].Animation;
	SonicCharSelAnim[2] = 0;
	SonicCharSelAnim[3] = 0;
}

void LoadKlonoaEventAnims()
{
	KlonoaEvANM[0] = LoadEventAnim("Upgrade0");
	KlonoaEvANM[1] = LoadEventAnim("battlePos");
	KlonoaEvANM[2] = LoadEventAnim("Victory");
}

void SetKlonoaEventAnims()
{
	if (setAnim)
		return;

	action_s_item_r0.motion = KlonoaEvANM[0]->getmotion();
	action_s_item_r2.motion = KlonoaEvANM[0]->getmotion();
	action_s_item_r1.motion = KlonoaEvANM[0]->getmotion();
	action_s_item_s1.motion = KlonoaEvANM[0]->getmotion();
	action_s_item_s0.motion = KlonoaEvANM[0]->getmotion();
	action_s_item_s2.motion = KlonoaEvANM[0]->getmotion();
	action_s_item_l0.motion = KlonoaEvANM[0]->getmotion();
	action_s_item_l1.motion = KlonoaEvANM[0]->getmotion();
	action_s_item_l2.motion = KlonoaEvANM[0]->getmotion();

	action_s_item_r0.object = KlonoaMDL->getmodel();
	action_s_item_r2.object = KlonoaMDL->getmodel();
	action_s_item_r1.object = KlonoaMDL->getmodel();
	action_s_item_s1.object = KlonoaMDL->getmodel();
	action_s_item_s0.object = KlonoaMDL->getmodel();
	action_s_item_s2.object = KlonoaMDL->getmodel();
	action_s_item_l0.object = KlonoaMDL->getmodel();
	action_s_item_l1.object = KlonoaMDL->getmodel();
	action_s_item_l2.object = KlonoaMDL->getmodel();

	action_s_s0071_sonic.object = KlonoaMDL->getmodel();
	action_s_s0071_sonic.motion = KlonoaEvANM[2]->getmotion();

	setAnim = true;
}

void LoadPlayerMotionDataAll_r()
{
	LoadPlayerMotionDataAll_t.Original();
	SetKlonoaEventAnims();
}

TaskFunc(EV_ClrFace, 0x4310F0);
void EV_Wait_r(task* tp)
{
	EV_SetAng(playertp[0], 0, 57000, 0);
	EV_ClrFace(tp);
}

void InitSuperKlonoa(HMODULE h)
{
	unsigned int* pcount = reinterpret_cast<unsigned int*>(GetProcAddress(h, "SSAnimCount"));
	*pcount = LengthOfArray<unsigned int>(KlonoaAnimList);

	AnimData** pdata = reinterpret_cast<AnimData**>(GetProcAddress(h, "SSAnimData"));
	*pdata = KlonoaAnimList;
}

void ReplaceSonicAnimPtr()
{
	if (MetalSonicFlag)
	{
		WriteData((AnimData**)0x49AB4D, &SonicAnimData);
	}
	else
	{
		WriteData((AnimData**)0x49AB4D, KlonoaAnimList);
	}
}

void Init_KlonoaAnim()
{
	LoadKlonoa_AnimFiles();
	SetKlonoaAnims();
	LoadKlonoaEventAnims();

	EV_SetAction_t.Hook(EV_SetAction_r);
	EV_SetMotion_t.Hook(EV_SetMotion_r);
	LoadPlayerMotionDataAll_t.Hook(LoadPlayerMotionDataAll_r);

	WriteJump(InitSonicCharSelAnims, InitKlonoaCharSelAnim);

	//hack all rendering events actions to use Chunk model draw function instead (cutscene anim support)
	WriteCall((void*)0x418214, late_actionClipEx_r);
	WriteCall((void*)0x41815E, late_ActionLinkEx_r);
	WriteCall((void*)0x41812E, late_ActionLinkMesh_r);
	WriteCall((void*)0x41814B, late_ActionLink_r);
	WriteCall((void*)0x4181FD, late_ActionMesh_r);
	WriteCall((void*)0x41820D, late_Action_r);

	WriteCall((void*)0x6E7C5E, EV_Wait_r);
}