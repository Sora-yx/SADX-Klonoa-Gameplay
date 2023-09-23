#include "pch.h"

std::shared_ptr<AnimationFile> KlonoaANM[60];
std::shared_ptr<AnimationFile> KlonoaEvANM[50];
#define AnimCount 170
#define boneCount 48

AnimData_t KlonoaAnimList[AnimCount] = { 0 };

static FunctionHook<void, taskwk*, int> DrawEventActionPL_t((intptr_t)0x417FB0);
//static FunctionHook<void, task*, NJS_ACTION*, NJS_TEXLIST*, float, char, char> EV_SetAction_t((intptr_t)EV_SetAction);
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
	{ 122, anm_bStance0},
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



//used for cutscene and charSel
void __cdecl Klonoa_linkEx(NJS_ACTION_LINK* action, float frame, int flag)
{
	if (action)
	{
		njSetTexture(&KlonoaTexList);
	}
}

void __cdecl late_actionClipEx_r(NJS_ACTION* anim, float a2, QueuedModelFlagsB a3)
{
	if (anim && anim->object == KlonoaMDL.get()->getmodel())
	{
		njSetTexture(&KlonoaTexList);
	}

	late_ActionClipEx(anim, a2, (LATE)a3, 0.0f);
}

//event head are not compatible obviously, we force standing animation when they happen
void __cdecl late_ActionLinkEx_r(NJS_ACTION_LINK* action, float frame, int flag)
{
	if (action && action->object == KlonoaMDL.get()->getmodel())
	{
		njSetTexture(&KlonoaTexList);
	}

	late_ActionLinkEx(action, frame, (LATE)flag);
}

void late_ActionLinkMesh_r(NJS_ACTION_LINK* actionLink, float frame, LATE flgs)
{
	if (actionLink)
	{
		if (actionLink->object && actionLink->object == KlonoaMDL.get()->getmodel())
		{
			njSetTexture(&KlonoaTexList);
		}
	}

	return late_ActionLinkMesh(actionLink, frame, flgs);
}

void late_ActionLink_r(NJS_ACTION_LINK* actionLink, float frame, LATE flgs)
{
	if (actionLink)
	{
		if (actionLink->object == KlonoaMDL.get()->getmodel())
		{
			njSetTexture(&KlonoaTexList);
		}
	}

	return late_ActionLink(actionLink, frame, flgs);
}

void late_ActionMesh_r(NJS_ACTION* act, float frame, LATE flgs)
{
	if (act)
	{
		if (act->object == KlonoaMDL.get()->getmodel())
		{
			njSetTexture(&KlonoaTexList);
		}
	}

	return late_ActionMesh(act, frame, flgs);
}

void __cdecl late_Action_r(NJS_ACTION* anim, float a2, QueuedModelFlagsB a3)
{
	if (anim)
	{
		if (anim->object == KlonoaMDL.get()->getmodel())
		{
			njSetTexture(&KlonoaTexList);
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
	if (a1 && a2 && a2 != KlonoaMDL.get()->getmodel())
	{
		if (isSonicNextAnim(a4)) {
			a2 = KlonoaMDL.get()->getmodel();
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

static Trampoline* EV_SetAction_t = nullptr; //use trampoline for compatibility with DC Conversion
void __cdecl EV_SetAction_r(task* obj, NJS_ACTION* anim, NJS_TEXLIST* tex, float speed, char mode, char linkframe)
{
	if (obj && anim && anim->object != KlonoaMDL.get()->getmodel())
	{
		if (isSonicNextAnim(tex))
		{
			anim->object = KlonoaMDL.get()->getmodel();
			tex = &KlonoaTexList;

			if (!ConvertSonicActionToKloAction(anim)) //if the animation didn't get replaced, apply a placeholder as a failafe.
			{
				anim->motion = KlonoaAnimList[anm_bStance1].Animation->motion;
			}

			if (speed > 1.0f)
				speed = 1.0f;

			if (anim->motion == KlonoaAnimList[9].Animation->motion || anim->motion == KlonoaAnimList[10].Animation->motion || anim->motion == KlonoaAnimList[11].Animation->motion)
			{
				speed = 0.6f;
			}
		}
	}

	if (!IsIngame()) //charsel walk anim spd fix
	{
		if (anim == KlonoaAnimList[11].Animation)
		{
			speed /= 1.7f;
		}
	}

	FunctionPointer(void, origin, (task * obj, NJS_ACTION * anim, NJS_TEXLIST * tex, float speed, char mode, char linkframe), EV_SetAction_t->Target());
	origin(obj, anim, tex, speed, mode, linkframe);
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

void FreeKlonoaAnims()
{
	for (int i = 0; i < LengthOfArray(KlonoaAnimList); i++)
	{
		auto &anim = KlonoaAnimList[i].Animation;
		if (anim && anim->motion && anim->object && anim->object != (NJS_OBJECT*)0xdddddddd) //garbage memory check
		{		
			PrintDebug("Free Klonoa Anim: %d\n", i);	
			delete anim;
			anim = nullptr;
		}
	}

	return;
}

void SetKlonoaAnims(AnimData_t animList[], const int size)
{
	//Init new anim list, put falling animation as a placeholder / failsafe for animation that don't have a SADX counterpart
	for (uint16_t i = 0; i < size; i++)
	{
		animList[i].Animation = new NJS_ACTION;

		if (i >= 134 && i <= 145)
		{
			animList[i].Animation->object = SuperKlonoaMDL.get()->getmodel();
			animList[i].NextAnim = 141;
		}
		else
		{
			animList[i].Animation->object = KlonoaMDL.get()->getmodel();
			animList[i].NextAnim = 18;
		}

		animList[i].Animation->motion = KlonoaANM[anmID_fall]->getmotion();
		animList[i].Instance = KlonoaANM[anmID_fall]->getmodelcount();
		animList[i].Property = 4;
		animList[i].TransitionSpeed = 0.25f;
		animList[i].AnimationSpeed = 0.5f;
	}


	animList[0].Animation->motion = KlonoaANM[anmID_stand]->getmotion();
	animList[0].AnimationSpeed = 0.7f;
	animList[0].NextAnim = 0;
	animList[0].Property = 3;
	animList[0].TransitionSpeed = 0.0625f;

	//surf stance
	animList[1].Animation->motion = KlonoaANM[anmID_battleStd]->getmotion();
	animList[1].TransitionSpeed = 0.25f;
	animList[1].AnimationSpeed = 0.70f;
	animList[1].NextAnim = 2;

	animList[2].Animation->motion = KlonoaANM[anmID_battleStd]->getmotion();
	animList[2].TransitionSpeed = 0.25f;
	animList[2].AnimationSpeed = 0.70f;
	animList[2].NextAnim = 2;

	animList[7] = animList[0];
	animList[8] = animList[0];

	//walk
	animList[9].Animation->motion = KlonoaANM[anmID_walk]->getmotion();
	animList[9].AnimationSpeed = 1.5f;
	animList[9].NextAnim = 9;
	animList[9].Property = 10;
	animList[9].TransitionSpeed = 0.15f;

	//speed walk
	animList[10].Animation->motion = KlonoaANM[anmID_run]->getmotion();
	animList[10].AnimationSpeed = 0.3f;
	animList[10].NextAnim = 10;
	animList[10].Property = 10;
	animList[10].TransitionSpeed = 0.20f;

	//jogging
	animList[11].Animation->motion = KlonoaANM[anmID_run]->getmotion();
	animList[11].Property = 10;
	animList[11].NextAnim = 11;
	animList[11].TransitionSpeed = 0.25f;
	animList[11].AnimationSpeed = 0.2f;

	//run
	animList[12].Animation->motion = KlonoaANM[anmID_run]->getmotion();
	animList[12].Property = 10;
	animList[12].NextAnim = 12;
	animList[12].TransitionSpeed = 0.25f;
	animList[12].AnimationSpeed = 0.2f;

	//sprint
	animList[13].Animation->motion = KlonoaANM[anmID_run]->getmotion();
	animList[13].Property = 9;
	animList[13].NextAnim = 13;
	animList[13].TransitionSpeed = 0.5f;
	animList[13].AnimationSpeed = 0.2f;

	//falling
	animList[18].Animation->motion = KlonoaANM[anmID_fall]->getmotion();
	animList[18].Property = 3;
	animList[18].NextAnim = 18;
	animList[18].TransitionSpeed = 0.25f;
	animList[18].AnimationSpeed = 0.5f;

	//idle
	animList[4].Animation->motion = KlonoaANM[anmID_idle]->getmotion();
	animList[4].Property = 4;
	animList[4].NextAnim = 5;
	animList[4].TransitionSpeed = 0.25f;
	animList[4].AnimationSpeed = 0.7f;

	animList[5].Animation->motion = KlonoaANM[anmID_idle]->getmotion();
	animList[5].NextAnim = 6;
	animList[5].TransitionSpeed = 0.25f;
	animList[5].AnimationSpeed = 0.7f;

	animList[6].Animation->motion = KlonoaANM[anmID_idle]->getmotion();
	animList[6].NextAnim = 0;
	animList[6].TransitionSpeed = 0.25f;
	animList[6].AnimationSpeed = 1.0f;

	//jump
	animList[14].Animation->motion = KlonoaANM[anmID_jump]->getmotion();
	animList[14].Property = 10;
	animList[14].NextAnim = 14;
	animList[14].TransitionSpeed = 0.25f;
	animList[14].AnimationSpeed = 0.06f;

	//spring
	animList[16].Animation->motion = KlonoaANM[anmID_jump]->getmotion();
	animList[16].Property = 3;
	animList[16].NextAnim = 16;;
	animList[16].TransitionSpeed = 0.25f;
	animList[16].AnimationSpeed = 0.5f;

	//landing
	animList[19].Animation->motion = KlonoaANM[animID_Landing]->getmotion();
	animList[19].NextAnim = 0;
	animList[19].TransitionSpeed = 0.25f;
	animList[19].AnimationSpeed = 0.5f;

	//brake
	animList[20].Animation->motion = KlonoaANM[animID_Brake]->getmotion();
	animList[20].NextAnim = 0;
	animList[20].TransitionSpeed = 0.25f;
	animList[20].AnimationSpeed = 0.8f;

	//push start
	animList[21].Animation->motion = KlonoaANM[anmID_push]->getmotion();
	animList[21].NextAnim = 22;
	animList[21].TransitionSpeed = 0.25f;
	animList[21].AnimationSpeed = 0.5f;

	//push
	animList[22].Animation->motion = KlonoaANM[anmID_push]->getmotion();
	animList[22].NextAnim = 22;
	animList[22].Property = 3;
	animList[22].TransitionSpeed = 0.25f;
	animList[22].AnimationSpeed = 0.5f;

	//hurt
	animList[23].Animation->motion = KlonoaANM[anmID_hurt]->getmotion();
	animList[23].NextAnim = 23;
	animList[23].Property = 5;
	animList[23].TransitionSpeed = 0.25f;
	animList[23].AnimationSpeed = 0.4f;

	//turn around
	animList[30] = animList[20];

	//pick obj (throw anim reversed for now)
	animList[33].Animation->motion = KlonoaANM[anmID_throw]->getmotion();
	animList[33].NextAnim = 37;
	animList[33].TransitionSpeed = 0.25f;
	animList[33].AnimationSpeed = 0.4f;
	animList[33].Property = 6; //reverse

	//obj run
	animList[34].Animation->motion = KlonoaANM[anmID_holdRun]->getmotion();
	animList[34].NextAnim = 34;
	animList[34].AnimationSpeed = 0.5f;
	animList[34].Property = 3;

	//obj stand
	animList[37].Animation->motion = KlonoaANM[animID_holdStd]->getmotion();
	animList[37].Property = 3;
	animList[37].NextAnim = 37;
	animList[37].TransitionSpeed = 0.25f;
	animList[37].AnimationSpeed = 0.5f;

	//obj walk
	animList[38].Animation->motion = KlonoaANM[anmID_holdRun]->getmotion();
	animList[38].NextAnim = 38;
	animList[38].AnimationSpeed = 0.3f;
	animList[38].Property = 3;

	//trampoline
	animList[39].Animation->motion = KlonoaANM[anmID_jump]->getmotion();
	animList[39].Property = 3;
	animList[39].NextAnim = 16;
	animList[39].TransitionSpeed = 0.25f;
	animList[39].AnimationSpeed = 0.5f;

	animList[40].Animation->motion = KlonoaANM[anmID_spinning]->getmotion();
	animList[40].NextAnim = 18;
	animList[40].TransitionSpeed = 0.25f;
	animList[40].AnimationSpeed = 0.5f;

	animList[41].Animation->motion = KlonoaANM[anmID_jump]->getmotion();
	animList[41].Property = 3;
	animList[41].NextAnim = 16;
	animList[41].TransitionSpeed = 0.25f;
	animList[41].AnimationSpeed = 0.5f;

	animList[42].Animation->motion = KlonoaANM[anmID_spinning]->getmotion();
	animList[42].NextAnim = 18;
	animList[42].TransitionSpeed = 0.25f;
	animList[42].AnimationSpeed = 0.7f;

	animList[43].Animation->motion = KlonoaANM[anmID_spinning]->getmotion();
	animList[43].Property = 3;
	animList[43].NextAnim = 16;
	animList[43].TransitionSpeed = 0.25f;
	animList[43].AnimationSpeed = 1.0f;

	//hang pulley / heli
	animList[47].Animation->motion = KlonoaANM[anmID_holdOne]->getmotion();
	animList[47].Property = 3;
	animList[47].NextAnim = 47;
	animList[47].TransitionSpeed = 0.5f;
	animList[47].AnimationSpeed = 0.5f;

	//shake tree
	animList[49].Animation->motion = KlonoaANM[anmID_shake]->getmotion();
	animList[49].NextAnim = 49;
	animList[49].TransitionSpeed = 0.5f;
	animList[49].Property = 3;
	animList[49].AnimationSpeed = 0.2f;

	//set shake (throw anim for now)
	animList[50].Animation->motion = KlonoaANM[anmID_throw]->getmotion();
	animList[50].NextAnim = 51;
	animList[50].TransitionSpeed = 0.25f;
	animList[50].AnimationSpeed = 0.3f;

	//shaking
	animList[51].Animation->motion = KlonoaANM[anmID_shake]->getmotion();
	animList[51].NextAnim = 51;
	animList[51].Property = 3;
	animList[51].TransitionSpeed = 0.25f;
	animList[51].AnimationSpeed = 0.2f;

	//stop shaking
	animList[52].Animation->motion = KlonoaANM[anmID_holdObj]->getmotion();
	animList[52].NextAnim = 37;
	animList[52].TransitionSpeed = 0.25f;
	animList[52].AnimationSpeed = 0.4f;

	//idle hold tree
	animList[53].Animation->motion = KlonoaANM[anmID_holdObj]->getmotion();
	animList[53].NextAnim = 54;
	animList[53].TransitionSpeed = 0.5f;
	animList[53].AnimationSpeed = 0.5f;

	animList[54].Animation->motion = KlonoaANM[anmID_holdObj]->getmotion();
	animList[54].Property = 3;
	animList[54].NextAnim = 54;
	animList[54].TransitionSpeed = 0.5f;
	animList[54].AnimationSpeed = 0.5f;

	//object jump
	animList[55].Animation->motion = KlonoaANM[anmID_holdJump]->getmotion();
	animList[55].NextAnim = 55;
	animList[55].AnimationSpeed = 0.5f;
	animList[55].Property = 3;

	//object fall
	animList[56].Animation->motion = KlonoaANM[anmID_holdFall]->getmotion();
	animList[56].NextAnim = 57;
	animList[56].AnimationSpeed = 0.5f;

	animList[57].Animation->motion = KlonoaANM[anmID_holdFall]->getmotion();
	animList[57].NextAnim = 57;
	animList[57].AnimationSpeed = 0.5f;
	animList[57].Property = 3;

	//Throw object
	animList[59].Animation->motion = KlonoaANM[anmID_throw]->getmotion();
	animList[59].NextAnim = anm_std;
	animList[59].AnimationSpeed = 0.2f;

	animList[60] = animList[59];

	animList[61] = animList[59];
	animList[61].NextAnim = 18;

	//hold obj
	animList[62].Animation->motion = KlonoaANM[anmID_holdObj]->getmotion();
	animList[62].Property = 4;
	animList[62].NextAnim = 18;
	animList[62].TransitionSpeed = 0.5;
	animList[62].AnimationSpeed = 0.5f;

	animList[63] = animList[59];

	//Light Dash
	animList[64].Animation->motion = KlonoaANM[anmID_fly]->getmotion();
	animList[64].NextAnim = 31;
	animList[64].TransitionSpeed = 0.125;
	animList[64].AnimationSpeed = 1.0f;
	animList[64].Property = 3;

	//cart
	animList[65].Animation->motion = KlonoaANM[anmID_holdObj]->getmotion();
	animList[65].NextAnim = 65;
	animList[65].TransitionSpeed = 0.125f;
	animList[65].AnimationSpeed = 1.0f;
	animList[65].Property = 5;

	//climbing ladder
	animList[69].Animation->motion = KlonoaANM[anmID_Climbing]->getmotion();
	animList[69].AnimationSpeed = 0.1f;
	animList[69].NextAnim = 69;
	animList[69].Property = 12;
	animList[69].TransitionSpeed = 1.0f;

	//shot from cannon
	animList[70].Animation->motion = KlonoaANM[anmID_fly]->getmotion();
	animList[70].NextAnim = 70;
	animList[70].TransitionSpeed = 0.125;
	animList[70].AnimationSpeed = 1.0f;
	animList[70].Property = 3;

	animList[74].Animation->motion = KlonoaANM[anmID_jmpPanel]->getmotion();
	animList[74].NextAnim = 74;
	animList[74].Property = 3;
	animList[74].TransitionSpeed = 0.25f;
	animList[74].AnimationSpeed = 0.5f;

	//victory.
	animList[75].Animation->motion = KlonoaANM[anmID_victory]->getmotion();
	animList[75].NextAnim = 75;
	animList[75].TransitionSpeed = 0.5f;
	animList[75].AnimationSpeed = 0.5f;
	animList[75].Property = 12;

	//victory standing.
	animList[76].Animation->motion = KlonoaANM[anmID_victoryStd]->getmotion();
	animList[76].NextAnim = 0;
	animList[76].Property = 3;
	animList[76].TransitionSpeed = 0.5f;
	animList[76].AnimationSpeed = 0.5f;

	animList[77].Animation->motion = KlonoaANM[anmID_holdObj]->getmotion();
	animList[77].NextAnim = 78;
	animList[77].TransitionSpeed = 0.125;
	animList[77].AnimationSpeed = 1.0f;

	animList[78] = animList[77];
	animList[78].NextAnim = 78;
	animList[78].Property = 3;

	//hold rocket h
	animList[79].Animation->motion = KlonoaANM[anmID_holdRocketV]->getmotion();
	animList[79].NextAnim = 79;
	animList[79].TransitionSpeed = 0.125;
	animList[79].AnimationSpeed = 1.0f;
	animList[79].Property = 3;

	//hold rocket vertical
	animList[80] = animList[79];
	animList[80].NextAnim = 80;

	//begin death
	animList[86].Animation->motion = KlonoaANM[anmID_death]->getmotion();
	animList[86].TransitionSpeed = 0.125;
	animList[86].AnimationSpeed = 1.0f;

	//death
	animList[87].Animation->motion = KlonoaANM[anmID_death]->getmotion();
	animList[87].NextAnim = 87;
	animList[87].TransitionSpeed = 0.125;
	animList[87].AnimationSpeed = 1.0f;
	animList[87].Property = 5;

	//Holding onto icicle
	animList[94].Animation->motion = KlonoaANM[anmID_holdIcicle]->getmotion();
	animList[94].NextAnim = 94;
	animList[94].TransitionSpeed = 0.125;
	animList[94].AnimationSpeed = 0.5f;
	animList[94].Property = 3;

	//board
	animList[102].Animation->motion = KlonoaANM[animID_Board]->getmotion();
	animList[102].NextAnim = 102;
	animList[102].TransitionSpeed = 0.125;
	animList[102].AnimationSpeed = 0.5f;
	animList[102].Property = 3;

	//board brake right
	animList[103].Animation->motion = KlonoaANM[animID_BoardRight]->getmotion();
	animList[103].NextAnim = 103;
	animList[103].TransitionSpeed = 0.125;
	animList[103].AnimationSpeed = 0.5f;
	animList[103].Property = 5;

	//board brake left
	animList[104].Animation->motion = KlonoaANM[animID_BoardLeft]->getmotion();
	animList[104].NextAnim = 104;
	animList[104].TransitionSpeed = 0.125;
	animList[104].AnimationSpeed = 0.5f;
	animList[104].Property = 5;

	//falling board
	animList[105].Animation->motion = KlonoaANM[animID_FallBoard]->getmotion();
	animList[105].NextAnim = 111;
	animList[105].TransitionSpeed = 0.125;
	animList[105].AnimationSpeed = 0.4f;
	animList[105].Property = 4;

	//jump board
	animList[106].Animation->motion = KlonoaANM[animID_BoardJump]->getmotion();
	animList[106].NextAnim = 105;
	animList[106].TransitionSpeed = 0.125;
	animList[106].AnimationSpeed = 0.5f;
	animList[106].Property = 4;

	//board right side
	animList[107].Animation->motion = KlonoaANM[animID_BoardRight]->getmotion();
	animList[107].NextAnim = 107;
	animList[107].TransitionSpeed = 0.125;
	animList[107].AnimationSpeed = 0.3f;
	animList[107].Property = 12;

	//board left side
	animList[108].Animation->motion = KlonoaANM[animID_BoardLeft]->getmotion();
	animList[108].NextAnim = 108;
	animList[108].TransitionSpeed = 0.125;
	animList[108].AnimationSpeed = 0.3f;
	animList[108].Property = 12;

	animList[109] = animList[107];
	animList[109].NextAnim = 114;

	animList[110] = animList[108];
	animList[110].NextAnim = 114;

	animList[111] = animList[105];
	animList[111].NextAnim = 112;
	animList[112] = animList[105];
	animList[112].NextAnim = 112;

	animList[113] = animList[105];
	animList[113].NextAnim = 102;

	animList[114] = animList[102];
	animList[114].NextAnim = 114;
	animList[114].Property = 3;

	//losing balance, don't have a counterpart so stick to neutral
	animList[115] = animList[102];

	//trick 1
	animList[116].Animation->motion = KlonoaANM[animID_trick1]->getmotion();
	animList[116].NextAnim = 112;
	animList[116].TransitionSpeed = 0.125;
	animList[116].AnimationSpeed = 0.5f;

	//trick 2
	animList[117].Animation->motion = KlonoaANM[animID_trick2]->getmotion();
	animList[117].NextAnim = 112;
	animList[117].TransitionSpeed = 0.125;
	animList[117].AnimationSpeed = 0.5f;

	//trick 3
	animList[118].Animation->motion = KlonoaANM[animID_trick3]->getmotion();
	animList[118].NextAnim = 112;
	animList[118].TransitionSpeed = 0.125;
	animList[118].AnimationSpeed = 0.5f;

	animList[119] = animList[118];
	animList[119].Property = 6;
	animList[119].NextAnim = 112;

	animList[120] = animList[118];
	animList[120].NextAnim = 121;
	animList[121] = animList[117];
	animList[121].NextAnim = 122;
	animList[122] = animList[116];
	animList[122].NextAnim = 112;
	animList[123] = animList[117];

	//crouch
	animList[124].Animation->motion = KlonoaANM[animID_Crouch]->getmotion();
	animList[124].NextAnim = 124;
	animList[124].TransitionSpeed = 0.125;
	animList[124].AnimationSpeed = 0.6f;
	animList[124].Property = 3;

	//showers
	animList[126].Animation->motion = KlonoaANM[anmID_shower]->getmotion();
	animList[126].NextAnim = 127;
	animList[126].TransitionSpeed = 0.125;
	animList[126].AnimationSpeed = 0.25f;

	animList[127].Animation->motion = KlonoaANM[anmID_shower2]->getmotion();
	animList[127].NextAnim = 128;
	animList[127].TransitionSpeed = 0.125;
	animList[127].AnimationSpeed = 0.25f;

	animList[128] = animList[127];
	animList[128].NextAnim = 0;

	//Chao Pet
	animList[130].Animation->motion = KlonoaANM[anmID_PetSet]->getmotion();
	animList[130].NextAnim = 131;
	animList[130].TransitionSpeed = 1.0f;
	animList[130].AnimationSpeed = 1.0f;

	animList[131].Animation->motion = KlonoaANM[anmID_PetLoop]->getmotion();
	animList[131].NextAnim = 131;
	animList[131].TransitionSpeed = 1.0f;
	animList[131].AnimationSpeed = 0.5f;
	animList[131].Property = 3;

	animList[132].Animation->motion = KlonoaANM[anmID_PetEnd]->getmotion();
	animList[132].NextAnim = 0;
	animList[132].TransitionSpeed = 1.0f;
	animList[132].AnimationSpeed = 0.8f;


	animList[133].Animation->motion = KlonoaANM[anmID_whistle]->getmotion();
	animList[133].NextAnim = 130;
	animList[133].TransitionSpeed = 1.0f;
	animList[133].AnimationSpeed = 1.0f;

	//Super Klonoa anims (to do: use SS anim here maybe?)

	//standing SK
	animList[134].Animation->motion = animList[0].Animation->motion;
	animList[134].Animation->object = SuperKlonoaMDL.get()->getmodel();
	animList[134].AnimationSpeed = animList[0].AnimationSpeed;
	animList[134].Property = animList[0].Property;
	animList[134].TransitionSpeed = animList[0].TransitionSpeed;
	animList[134].NextAnim = 134;

	//standing -> moving SK
	animList[135].Animation->motion = animList[9].Animation->motion;
	animList[135].AnimationSpeed = animList[9].AnimationSpeed;
	animList[135].Property = animList[9].Property;
	animList[135].TransitionSpeed = animList[9].TransitionSpeed;
	animList[135].NextAnim = 135;

	//moving 1
	animList[136].Animation->motion = animList[11].Animation->motion;
	animList[136].AnimationSpeed = animList[11].AnimationSpeed;
	animList[136].Property = animList[11].Property;
	animList[136].TransitionSpeed = animList[11].TransitionSpeed;
	animList[136].NextAnim = 136;

	//moving 2
	animList[137].Animation->motion = animList[12].Animation->motion;
	animList[137].AnimationSpeed = animList[12].AnimationSpeed;
	animList[137].Property = animList[12].Property;
	animList[137].TransitionSpeed = animList[12].TransitionSpeed;
	animList[137].NextAnim = 137;

	//moving 3
	animList[138].Animation->motion = animList[13].Animation->motion;
	animList[138].AnimationSpeed = animList[13].AnimationSpeed;
	animList[138].Property = animList[13].Property;
	animList[138].TransitionSpeed = animList[13].TransitionSpeed;
	animList[138].NextAnim = 138;

	//falling
	animList[139].Animation->motion = animList[13].Animation->motion;
	animList[139].AnimationSpeed = animList[13].AnimationSpeed;
	animList[139].Property = animList[13].Property;
	animList[139].TransitionSpeed = animList[13].TransitionSpeed;
	animList[139].NextAnim = 138;

	//landing SK
	animList[142].Animation->motion = animList[19].Animation->motion;
	animList[142].AnimationSpeed = animList[19].AnimationSpeed;
	animList[142].Property = animList[19].Property;
	animList[142].TransitionSpeed = animList[19].TransitionSpeed;
	animList[142].NextAnim = 134;

	//victory SK
	animList[143].Animation->motion = animList[75].Animation->motion;
	animList[143].AnimationSpeed = animList[75].AnimationSpeed;
	animList[143].Property = animList[75].Property;
	animList[143].TransitionSpeed = animList[75].TransitionSpeed;
	animList[143].NextAnim = 143;

	animList[144].Animation->motion = animList[76].Animation->motion;
	animList[144].AnimationSpeed = animList[76].AnimationSpeed;
	animList[144].Property = animList[76].Property;
	animList[144].TransitionSpeed = animList[76].TransitionSpeed;
	animList[144].NextAnim = 144;

	animList[145].Animation->motion = animList[14].Animation->motion;
	animList[145].AnimationSpeed = animList[14].AnimationSpeed;
	animList[145].Property = animList[14].Property;
	animList[145].TransitionSpeed = animList[14].TransitionSpeed;
	animList[145].NextAnim = 145;

	//hover
	animList[anm_hover].Animation->motion = KlonoaANM[anmID_hover]->getmotion();
	animList[anm_hover].Property = 3;
	animList[anm_hover].NextAnim = anm_hover;
	animList[anm_hover].TransitionSpeed = 0.25f;
	animList[anm_hover].AnimationSpeed = 0.5f;

	//bullet start
	animList[anm_windBullet].Animation->motion = KlonoaANM[anmID_bulletStart]->getmotion();
	animList[anm_windBullet].Property = 4;
	animList[anm_windBullet].NextAnim = anm_windBulletEnd;
	animList[anm_windBullet].TransitionSpeed = 0.25f;
	animList[anm_windBullet].AnimationSpeed = 0.5f;

	//bullet mid air
	animList[anm_windBulletAir].Animation->motion = KlonoaANM[animID_bulletMidAir]->getmotion();
	animList[anm_windBulletAir].Property = 4;
	animList[anm_windBulletAir].NextAnim = anm_fall;
	animList[anm_windBulletAir].TransitionSpeed = 0.25f;
	animList[anm_windBulletAir].AnimationSpeed = 0.5f;

	//bullet end
	animList[anm_windBulletEnd].Animation->motion = KlonoaANM[animID_bulletEnd]->getmotion();
	animList[anm_windBulletEnd].Property = 4;
	animList[anm_windBulletEnd].NextAnim = 0;
	animList[anm_windBulletEnd].TransitionSpeed = 0.25f;
	animList[anm_windBulletEnd].AnimationSpeed = 0.5f;

	//hold enemy std
	animList[anm_holdStd].Animation->motion = KlonoaANM[animID_holdStd]->getmotion();
	animList[anm_holdStd].Property = 3;
	animList[anm_holdStd].NextAnim = anm_holdStd;
	animList[anm_holdStd].TransitionSpeed = 0.25f;
	animList[anm_holdStd].AnimationSpeed = 0.5f;

	//hold enemy wlk
	animList[anm_holdWalk].Animation->motion = KlonoaANM[anmID_holdRun]->getmotion();
	animList[anm_holdWalk].NextAnim = anm_holdWalk;
	animList[anm_holdWalk].AnimationSpeed = 0.3f;
	animList[anm_holdWalk].Property = 3;

	//hold enemy run
	animList[anm_holdRun].Animation->motion = KlonoaANM[anmID_holdRun]->getmotion();
	animList[anm_holdRun].NextAnim = anm_holdRun;
	animList[anm_holdRun].AnimationSpeed = 0.5f;
	animList[anm_holdRun].Property = 3;

	//hold enemy jump
	animList[anm_holdJump].Animation->motion = KlonoaANM[anmID_holdJump]->getmotion();
	animList[anm_holdJump].NextAnim = anm_holdJump;
	animList[anm_holdJump].AnimationSpeed = 0.5f;
	animList[anm_holdJump].Property = 3;

	//hold enemy fall
	animList[anm_holdFall].Animation->motion = KlonoaANM[anmID_holdFall]->getmotion();
	animList[anm_holdFall].NextAnim = anm_holdFall;
	animList[anm_holdFall].AnimationSpeed = 0.5f;
	animList[anm_holdFall].Property = 3;

	//throw enemy
	animList[anm_throwStd].Animation->motion = KlonoaANM[anmID_throw]->getmotion();
	animList[anm_throwStd].NextAnim = anm_std;
	animList[anm_throwStd].AnimationSpeed = 0.5f;
	animList[anm_throwStd].Property = 4;

	//throw enemy air
	animList[anm_throwAir].Animation->motion = KlonoaANM[anmID_throwAir]->getmotion();
	animList[anm_throwAir].NextAnim = anm_fall;
	animList[anm_throwAir].AnimationSpeed = 0.5f;
	animList[anm_throwAir].Property = 4;


	auto super = SuperKlonoaMDL.get()->getmodel();
	//Super Klonoa next anims
	//fly standing
	animList[anm_flyStd].Animation->motion = KlonoaANM[anmID_flyStd]->getmotion();
	animList[anm_flyStd].NextAnim = anm_flyStd;
	animList[anm_flyStd].AnimationSpeed = 0.7f;
	animList[anm_flyStd].Property = 3;
	animList[anm_flyStd].Animation->object = super;

	//fly Up
	animList[anm_flyUp].Animation->motion = KlonoaANM[anmID_jump]->getmotion();
	animList[anm_flyUp].NextAnim = anm_flyUp;
	animList[anm_flyUp].AnimationSpeed = 0.5f;
	animList[anm_flyUp].Property = 3;
	animList[anm_flyUp].Animation->object = super;

	//fly down
	animList[anm_flyDown].Animation->motion = KlonoaANM[anmID_fall]->getmotion();
	animList[anm_flyDown].NextAnim = anm_flyDown;
	animList[anm_flyDown].AnimationSpeed = 0.5f;
	animList[anm_flyDown].Property = 3;
	animList[anm_flyDown].Animation->object = super;

	//fly moving
	animList[anm_flyMoving].Animation->motion = KlonoaANM[anmID_fly]->getmotion();
	animList[anm_flyMoving].NextAnim = anm_flyMoving;
	animList[anm_flyMoving].AnimationSpeed = 0.5f;
	animList[anm_flyMoving].Property = 3;
	animList[anm_flyMoving].Animation->object = super;

	for (int j = 134; j < 146; j++)
	{
		if (animList[j].Animation->motion)
		{
			animList[j].Animation->object = super;
		}
	}
}

void LoadKlonoa_AnimFiles()
{
	KlonoaANM[0] =  LoadAnimSmartPtr("stand");
	KlonoaANM[1] =  LoadAnimSmartPtr("death");
	KlonoaANM[2] =  LoadAnimSmartPtr("fall");
	KlonoaANM[3] =  LoadAnimSmartPtr("walk");
	KlonoaANM[4] =  LoadAnimSmartPtr("fly");
	KlonoaANM[5] =  LoadAnimSmartPtr("hover");
	KlonoaANM[6] =  LoadAnimSmartPtr("run");
	KlonoaANM[7] =  LoadAnimSmartPtr("idle");
	KlonoaANM[8] =  LoadAnimSmartPtr("jump");
	KlonoaANM[9] =  LoadAnimSmartPtr("holdOneHand");
	KlonoaANM[10] =  LoadAnimSmartPtr("holdObj");
	KlonoaANM[11] =  LoadAnimSmartPtr("Victory");
	KlonoaANM[12] =  LoadAnimSmartPtr("Victory2");
	KlonoaANM[13] =  LoadAnimSmartPtr("standVictory");
	KlonoaANM[14] =  LoadAnimSmartPtr("standBattle");
	KlonoaANM[15] =  LoadAnimSmartPtr("bulletStart");
	KlonoaANM[16] =  LoadAnimSmartPtr("bulletMidAir");
	KlonoaANM[17] =  LoadAnimSmartPtr("bulletEnd");
	KlonoaANM[18] =  LoadAnimSmartPtr("holdStand");
	KlonoaANM[19] =  LoadAnimSmartPtr("holdWalk");
	KlonoaANM[20] =  LoadAnimSmartPtr("holdRun");
	KlonoaANM[21] =  LoadAnimSmartPtr("holdJump");
	KlonoaANM[22] =  LoadAnimSmartPtr("holdFall");
	KlonoaANM[23] =  LoadAnimSmartPtr("throw");
	KlonoaANM[24] =  LoadAnimSmartPtr("airThrow"); 
	KlonoaANM[25] =  LoadAnimSmartPtr("Brake");
	KlonoaANM[26] =  LoadAnimSmartPtr("Landing");
	KlonoaANM[27] =  LoadAnimSmartPtr("Board");
	KlonoaANM[28] =  LoadAnimSmartPtr("Crouch");
	KlonoaANM[29] =  LoadAnimSmartPtr("FallBoard");
	KlonoaANM[30] =  LoadAnimSmartPtr("trick1");
	KlonoaANM[31] =  LoadAnimSmartPtr("trick2");
	KlonoaANM[32] =  LoadAnimSmartPtr("trick3");
	KlonoaANM[33] =  LoadAnimSmartPtr("BoardLeft");
	KlonoaANM[34] =  LoadAnimSmartPtr("BoardRight");
	KlonoaANM[35] =  LoadAnimSmartPtr("BoardJump");
	KlonoaANM[36] =  LoadAnimSmartPtr("flyStd");
	//	KlonoaANM[37] = LoadObjectAnim("cart"); //MEH
	KlonoaANM[38] =  LoadAnimSmartPtr("icicleHold");
	KlonoaANM[39] =  LoadAnimSmartPtr("climb");
	KlonoaANM[40] =  LoadAnimSmartPtr("spin");
	KlonoaANM[41] =  LoadAnimSmartPtr("jmpPanel");
	KlonoaANM[42] =  LoadAnimSmartPtr("push");
	KlonoaANM[43] =  LoadAnimSmartPtr("hurt");
	KlonoaANM[44] =  LoadAnimSmartPtr("holdRocketV");
	KlonoaANM[45] =  LoadAnimSmartPtr("shower");
	KlonoaANM[46] =  LoadAnimSmartPtr("shower2");
	KlonoaANM[47] =  LoadAnimSmartPtr("Whistle");
	KlonoaANM[48] =  LoadAnimSmartPtr("PetSet");
	KlonoaANM[49] =  LoadAnimSmartPtr("shake");
	KlonoaANM[50] =  LoadAnimSmartPtr("PetLoop");
	KlonoaANM[51] =  LoadAnimSmartPtr("PetEnd");

}

void InitKlonoaCharSelAnim()
{
	SonicCharSelAnim[0] = KlonoaAnimList[11].Animation;
	SonicCharSelAnim[1] = KlonoaAnimList[75].Animation;
	SonicCharSelAnim[2] = KlonoaAnimList[76].Animation;
	SonicCharSelAnim[3] = 0;
}

void LoadKlonoaEventAnims()
{
	KlonoaEvANM[0] =  LoadEventAnimSmartPtr("Upgrade0");
	KlonoaEvANM[1] =  LoadEventAnimSmartPtr("battlePos");
	KlonoaEvANM[2] =  LoadEventAnimSmartPtr("Victory");
	KlonoaEvANM[3] =  LoadEventAnimSmartPtr("sleep");
	KlonoaEvANM[4] =  LoadEventAnimSmartPtr("wakeUp");
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

	auto mdl = KlonoaMDL.get()->getmodel();
	action_s_item_r0.object = mdl;
	action_s_item_r2.object = mdl;
	action_s_item_r1.object = mdl;
	action_s_item_s1.object = mdl;
	action_s_item_s0.object = mdl;
	action_s_item_s2.object = mdl;
	action_s_item_l0.object = mdl;
	action_s_item_l1.object = mdl;
	action_s_item_l2.object = mdl;


	//dodge knux punch
	action_s_s0035_sonic.object = mdl;
	action_s_s0035_sonic.motion = KlonoaANM[5]->getmotion();
	action_s_s0036_sonic.object = mdl;
	action_s_s0036_sonic.motion = KlonoaEvANM[1]->getmotion();

	//sleep
	action_s_s0031_sonic.object = mdl;
	action_s_s0031_sonic.motion = KlonoaEvANM[3]->getmotion();
	action_s_s0053_sonic.object = mdl;
	action_s_s0053_sonic.motion = KlonoaEvANM[3]->getmotion();

	//wake up
	action_s_s0032_sonic.object = mdl;
	action_s_s0032_sonic.motion = KlonoaEvANM[4]->getmotion();
	action_s_s0054_sonic.object = mdl;
	action_s_s0054_sonic.motion = KlonoaEvANM[4]->getmotion();


	action_s_s0071_sonic.object = mdl;
	action_s_s0071_sonic.motion = KlonoaEvANM[2]->getmotion();

	setAnim = true;
}

void LoadPlayerMotionDataAll_r()
{
	LoadPlayerMotionDataAll_t.Original();
	SetKlonoaEventAnims();
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

DataPointer(PADREC_DATA_TAG, EV0052T, 0x8694FC);
DataPointer(EPATHTAG, epathtag_cube0052_t3, 0x2BE82B0);

void PlayVictoryAnim()
{
	EV_SetAction(playertp[0], &action_s_s0071_sonic, &KlonoaTexList, 0.5f, 1, 12);
}

void SetEndingAnimationOutro(task* player)
{
	EV_ClrAction(player);
	EV_PlayPad(0, &EV0052T);
	EV_Wait(5);
	EV_CameraPos(1, 85, 2171.3999f, 118.0f, 1630.995f);
	EV_CameraAng(1, 85, 0x1B6D, 0x200E, 0xFD00);
	EV_Wait(93);
	EV_CameraAng(0, 15, 0x66D, 0x200E, 0xFD00);
	EV_CameraPos(0, 15, 2171.3f, 125.0f, 1630.665f);
	EV_SetAction(player, &action_s_s0071_sonic, &KlonoaTexList, 0.5f, 1, 12);
	PlayCustomSoundVolume(kl_SuperJump0, 2.0f);
	EV_SetMode(player, 0);
	EV_SetPath(player, &epathtag_cube0052_t3, 0.0f, 2);
	EV_Wait(50);
	auto BLACK = COverlayCreate(0.033333335f, 0.1f, 0.0f, 0.0f, 0.0f);
	EV_Wait(30);
}

void Init_KlonoaAnim()
{
	LoadKlonoa_AnimFiles();
	SetKlonoaAnims(KlonoaAnimList, LengthOfArray(KlonoaAnimList));
	LoadKlonoaEventAnims();

	EV_SetAction_t = new Trampoline(0x42FE00, 0x42FE06, EV_SetAction_r);
	EV_SetMotion_t.Hook(EV_SetMotion_r);
	LoadPlayerMotionDataAll_t.Hook(LoadPlayerMotionDataAll_r);

	WriteJump(InitSonicCharSelAnims, InitKlonoaCharSelAnim);

	//hack all rendering events actions (cutscene anim support)
	WriteCall((void*)0x418214, late_actionClipEx_r);
	WriteCall((void*)0x41815E, late_ActionLinkEx_r);
	WriteCall((void*)0x41812E, late_ActionLinkMesh_r);
	WriteCall((void*)0x41814B, late_ActionLink_r);
	WriteCall((void*)0x4181FD, late_ActionMesh_r);
	WriteCall((void*)0x41820D, late_Action_r);

	WriteCall((void*)0x6D00E7, SetEndingAnimationOutro);
	WriteData<5>((int*)0x006D013D, 0x90);
	WriteData<5>((int*)0x006D0119, 0x90);
}