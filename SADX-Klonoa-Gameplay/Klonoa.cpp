#include "pch.h"
#include "abilities.h"

bool klonoa = false;
uint8_t klonoaPnum = 0;

NJS_VECTOR KLScaleDiff = { 0.2f, 0.2f, 0.2f };

ModelInfo* KlonoaMDL = nullptr;
AnimationFile* KlonoaANM[20] = { 0 };
#define AnimCount 160
#define boneCount 48
AnimData_t KlonoaAnimList[AnimCount] = { 0 };

static NJS_TEXNAME KlonoaTex[2] = { 0 };
static NJS_TEXLIST KlonoaTexList = { arrayptrandlength(KlonoaTex) };

static FunctionHook<void, task*> Sonic_Main_t((intptr_t)Sonic_Main);
static FunctionHook<void, task*> Sonic_Display_t((intptr_t)Sonic_Display);
static FunctionHook<void, taskwk*, motionwk2*, playerwk*> Sonic_RunsActions_t((intptr_t)Sonic_Act1);

bool LoadKlonoa_Worker(task* obj) {

	if (obj->twp->mode == 0) {
		void* mem = AllocateMemory(sizeof(klonoawk));
		memset(mem, 0, sizeof(klonoawk));
		obj->awp = (anywk*)mem;
		return true;
	}

	return false;
}

int getKlonoaPlayer()
{
	for (uint8_t i = 0; i < 8; i++)
	{
		if (klonoa && playertwp[i] && playertwp[i]->charIndex == klonoaPnum)
		{
			return i;
		}
	}

	return -1;
}

bool isKlonoa(char pnum)
{
	if (klonoa && playertwp[pnum] && playertwp[pnum]->charIndex == klonoaPnum)
	{
		return true;
	}

	return false;
}

bool isKlonoaHold(char pnum)
{
	if (!playertwp[pnum])
		return false;

	return playertwp[pnum]->mode >= act_holdStd && playertwp[pnum]->mode <= act_holdJump;
}


void DrawKlonoa_Event(NJS_ACTION* anim, float frame, QueuedModelFlagsB flg)
{
	SetupWorldMatrix();
	Direct3D_SetChunkModelRenderState();

	NJS_ACTION act2 = *anim;

	if (*(int*)0x3ABD9CC)
	{
		DrawQueueDepthBias = -5952.0;
		njCnkAction_Queue(&act2, frame, QueuedModelFlagsB_EnableZWrite);
		DrawQueueDepthBias = 0.0;
	}
	else
	{
		njCnkAction(&act2, frame);
	}

	Direct3D_UnsetChunkModelRenderState();
}

void DrawKlonoa(playerwk* co2, int animNum, NJS_ACTION* action)
{
	SetupWorldMatrix();
	Direct3D_SetChunkModelRenderState();

	NJS_ACTION act2 = *action;

	if (*(int*)0x3ABD9CC)
	{
		DrawQueueDepthBias = -5952.0f;
		njCnkAction_Queue(&act2, co2->mj.nframe, QueuedModelFlagsB_EnableZWrite);
		DrawQueueDepthBias = 0.0;
	}
	else
	{
		njCnkAction(&act2, co2->mj.nframe);
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

void Klonoa_Fall(taskwk* data, playerwk* co2)
{
	data->mode = 12;
	co2->mj.reqaction = 18;
}

//used for cutscene and charSel
void __cdecl Klonoa_late_ActionEx(NJS_ACTION* anim, float a2, QueuedModelFlagsB a3)
{
	NJS_VECTOR scale = { 1.0f, 1.0f, 1.0f };

	for (int i = 0; i < 4; i++) {

		if (anim && anim->motion && SonicCharSelAnim[i] && SonicCharSelAnim[i]->motion)
		{
			if (anim->motion == SonicCharSelAnim[i]->motion) {
				njSetTexture(&KlonoaTexList);
				scale = { 0.9f, 0.9f, 0.9f };
				njScaleV(0, &scale);
				DrawKlonoa_Event(anim, a2, a3);
				return;
			}
		}
	}

	njScaleV(0, &scale);
	late_ActionEx(anim, a2, (LATE)a3);
}

//used for cutscene and charSel
void __cdecl  Klonoa_linkEx(NJS_ACTION_LINK* action, float frame, int flag)
{
	NJS_VECTOR scale = { 1.0f, 1.0f, 1.0f };

	if (!IsIngame()) {

		for (uint8_t i = 0; i < 4; i++) {

			if (action && action->motionlink->motion[0] && SonicCharSelAnim[i] && SonicCharSelAnim[i]->motion)
			{
				if (action->motionlink->motion[0] == SonicCharSelAnim[i]->motion) {
					njSetTexture(&KlonoaTexList);
					scale = { 0.9f, 0.9f, 0.9f };
					njScaleV(0, &scale);
					njCnkActionLink(action, frame, flag);
					return;
				}
			}
		}
	}

	njScaleV(0, &scale);
	late_ActionLinkEx(action, frame, (LATE)flag);
}
short twistamount[8] = {};
void(__cdecl** NodeCallbackFuncPtr)(NJS_OBJECT* obj) = (decltype(NodeCallbackFuncPtr))0x3AB9908;


//Calc Klonoa hand and root Pos for objects position
void NodeCallback2(NJS_OBJECT* obj)
{
	if (!playertwp[klonoaPnum])
		return;

	char pnum = klonoaPnum;
	auto co2 = playerpwp[pnum];
	auto kl = (klonoawk*)playertp[pnum]->awp;
	auto klMDL = KlonoaMDL->getmodel();

	float* v1 = _nj_current_matrix_ptr_;

	if (obj == klMDL->child->child) //root 2
	{
		if (playertwp[klonoaPnum]->mode == 2)
			njRotateX(v1, twistamount[pnum]);
	}
	else if (obj == klMDL->child->child->child->sibling->sibling->sibling->sibling->sibling->child->sibling->sibling->child->child->child)
	{
		NJS_VECTOR v = { 0.0f, -1.5f, 0.0f };

		njCalcPoint(v1, &v, &co2->righthand_pos); //right hand pos
		v.z = 1;
		njCalcVector(v1, &v, &co2->righthand_vec);  //right hand vec
		SetVectorDiff(&co2->righthand_pos);
		SetVectorDiff(&co2->righthand_vec);

		//CharObj2Ptrs[currentplayer]->SoManyVectors[0].y = CharObj2Ptrs[currentplayer]->SoManyVectors[0].y - 8;
	}
	else if (obj == klMDL->child->child->child->sibling->sibling->sibling->sibling->sibling->child->sibling->sibling->sibling->child->child->child)
	{
		NJS_VECTOR v = { 0, -1.5, 0 };
		njCalcPoint(v1, &v, &co2->lefthand_pos); //left hand pos
		v.z = 1;
		njCalcVector(v1, &v, &co2->lefthand_vec); //left hand vec
		SetVectorDiff(&co2->lefthand_pos);
		SetVectorDiff(&co2->lefthand_vec);
		//CharObj2Ptrs[currentplayer]->SoManyVectors[7].y = CharObj2Ptrs[currentplayer]->SoManyVectors[7].y - 8;
	}
	else if (obj == klMDL->child->child->child->sibling->sibling->sibling->sibling->sibling->child->sibling->sibling->child->child->child->child)
	{
		NJS_VECTOR v = { 0, 1.0f, 0 };
		njCalcPoint(v1, &v, &kl->ringPos); //ring pos
		v.z = 1;
		njCalcVector(v1, &v, &kl->ringVec); // ring vec
		SetVectorDiff(&kl->ringPos);
		SetVectorDiff(&kl->ringVec);
	}
}

void __cdecl Klonoa_Display_r(task* obj)
{
	auto data = obj->twp;

	if (MissedFrames || (!IsVisible(&data->pos, 15.0)))
		return;

	char pnum = data->charIndex;

	if (!isKlonoa(pnum))
	{
		return Sonic_Display_t.Original(obj);
	}

	auto co2 = playerpwp[pnum];
	int curAnim = (unsigned __int16)co2->mj.reqaction;
	auto data2_pp = (motionwk2*)obj->mwp;

	Direct3D_SetZFunc(1u);
	BackupConstantAttr();
	AddConstantAttr(0, NJD_FLAG_IGNORE_SPECULAR);

	if (SuperSonicFlag)
		Direct3D_PerformLighting(4);
	else
		Direct3D_PerformLighting(2);

	if (!MetalSonicFlag && SONIC_OBJECTS[6]->sibling != *(NJS_OBJECT**)0x3C55D40)
	{
		co2->mj.jvmode = 2;
		PJoinVertexes(data, data2_pp, co2);
		co2->mj.pljvptr[0xB].dstobj = SONIC_OBJECTS[6]->sibling;
		co2->mj.jvmode = 0;
		PJoinVertexes(data, data2_pp, co2);
		*(NJS_OBJECT**)0x3C55D40 = SONIC_OBJECTS[6]->sibling;
	}

	if (co2->mj.mtnmode == 2)
		curAnim = (unsigned __int16)co2->mj.action;

	if (!(data->wtimer & 2))
	{
		njSetTexture(&KlonoaTexList);

		njPushMatrix(0);
		NJS_VECTOR pos = data->cwp->info->center;
		pos.y -= 4.3f;
		njTranslateV(0, &pos);
		njScaleV(0, &KLScaleDiff);

		njRotateZ_(data->ang.z);
		njRotateX_(data->ang.x);
		njRotateY_(0x8000 - data->ang.y);

		SpinDash_RotateModel(curAnim, (taskwk*)data);

		NJS_ACTION* action = co2->mj.plactptr[curAnim].actptr;

		if (data->ewp->action.list)
		{
			DrawEventAction(data);
		}
		else 
		{
			if (co2->mj.mtnmode == 2) {
				action = co2->mj.actwkptr;
			}

			DrawKlonoa(co2, curAnim, action);
			*NodeCallbackFuncPtr = NodeCallback2;
			njPushMatrix(_nj_unit_matrix_);
			njNullAction(action, co2->mj.nframe);
			njPopMatrix(1);
			*NodeCallbackFuncPtr = nullptr;
		}

		njPopMatrix(1u);
	}

	Direct3D_PerformLighting(0);
	ClampGlobalColorThing_Thing();
	RestoreConstantAttr();
	Direct3D_ResetZFunc();

	if (IsGamePaused())
		DrawCharacterShadow((EntityData1*)data, (struct_a3*)&co2->shadow);
}

void __cdecl Klonoa_runsActions_r(taskwk* data, motionwk2* data2, playerwk* co2)
{
	if (!co2)
	{
		return;
	}

	if (!isKlonoa(data->charIndex))
	{
		return Sonic_RunsActions_t.Original(data, data2, co2);
	}

	auto data1 = (EntityData1*)data;
	char pnum = data->charIndex;
	auto klwk = (klonoawk*)playertp[pnum]->awp;

	switch (data->mode)
	{
	case act_stnd:
	case 2:
		if ((Sonic_NAct((CharObj2*)co2, data1, (EntityData2*)data2) || KlonoaWBullet_CheckInput(data, co2, klwk)))
		{
			break;
		}

		break;
	case 8:
	case 12:
		if (Sonic_NAct((CharObj2*)co2, data1, (EntityData2*)data2))
		{
			break;
		}

		if (co2->spd.y < 0.0f)
		{
			co2->mj.reqaction = 18;
		}

		if ((KlonoaWBullet_CheckInput(data, co2, klwk)) || hover_CheckInput(data, co2, klwk))
		{
			break;
		}
		break;
	case act_hover:
		if (Sonic_NAct((CharObj2*)co2, data1, (EntityData2*)data2))
		{
			break;
		}

		if ((JumpButtons & Controllers[data->charIndex].HeldButtons) == 0)
		{
			Klonoa_Fall(data, co2);
			break;
		}

		if (klwk->hoverTimer > 0)
		{
			klwk->hoverTimer--;
		}
		else
		{
			Klonoa_Fall(data, co2);
		}
		break;
	case act_windBullet:
	case act_windBulletAir:
		if (Sonic_NAct((CharObj2*)co2, data1, (EntityData2*)data2))
		{
			break;
		}

		KlonoaBulletEnd(data, co2, klwk);
		break;
	case act_super_jump:
		if (Sonic_NAct((CharObj2*)co2, data1, (EntityData2*)data2))
		{
			klwk->superJumpCount = 0;
			break;
		}

		if (data->flag & 3)
		{
			data->mode = act_stnd;
			klwk->superJumpCount = 0;
			co2->mj.reqaction = 0;
			return;
		}

		break;
	}

	Sonic_RunsActions_t.Original(data, data2, co2);
}

void __cdecl Klonoa_Main_r(task* obj)
{
	auto data = obj->twp;
	motionwk2* data2 = (motionwk2*)obj->mwp;
	playerwk* co2 = (playerwk*)obj->mwp->work.l;
	chaoswk* cwk = (chaoswk*)obj->awp;

	char pnum = data->counter.b[0];

	if (!data->mode)
	{
		if (LoadKlonoa_Worker(obj)) {
			LoadPVM("KlonoaTex", &KlonoaTexList);
			LoadPVM("KNU_EFF", &KNU_EFF_TEXLIST);
			klonoa = true;
		}
		else
		{
			PrintDebug("Klonoa Mod: Failed to load Klonoa worker mod won't work...\n");
		}
	}

	Sonic_Main_t.Original(obj);

	if (!klonoa)
		return;

	switch (data->mode)
	{
	case act_hover:
		hover_Physics(data, data2, co2);
		break;
	case act_super_jump:
		SuperJump_Physics(data, data2, co2);
		break;
	}

	klonoaPnum = pnum;
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
	KlonoaANM[9] = LoadObjectAnim("holdOneHand");
	KlonoaANM[10] = LoadObjectAnim("holdObj");
	KlonoaANM[11] = LoadObjectAnim("Victory");
	KlonoaANM[12] = LoadObjectAnim("Victory2");
	KlonoaANM[13] = LoadObjectAnim("standVictory");
	KlonoaANM[14] = LoadObjectAnim("standBattle");
	KlonoaANM[15] = LoadObjectAnim("bulletStart");
	KlonoaANM[16] = LoadObjectAnim("bulletMidAir");
	KlonoaANM[17] = LoadObjectAnim("bulletEnd");


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
	KlonoaAnimList[1].Property = 4;
	KlonoaAnimList[1].NextAnim = 2;

	KlonoaAnimList[2].Animation->motion = KlonoaANM[anmID_battleStd]->getmotion();
	KlonoaAnimList[2].TransitionSpeed = 0.25f;
	KlonoaAnimList[2].AnimationSpeed = 0.50f;
	KlonoaAnimList[2].Property = 4;
	KlonoaAnimList[2].NextAnim = 2;

	//walk
	KlonoaAnimList[9].Animation->motion = KlonoaANM[anmID_walk]->getmotion();
	KlonoaAnimList[9].AnimationSpeed = 1.0f;
	KlonoaAnimList[9].NextAnim = 9;
	KlonoaAnimList[9].Property = 10;
	KlonoaAnimList[9].TransitionSpeed = 0.15f;

	//speed walk
	KlonoaAnimList[10].Animation->motion = KlonoaANM[anmID_run]->getmotion();
	KlonoaAnimList[10].AnimationSpeed = 0.2f;
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
	KlonoaAnimList[5].Property = 4;
	KlonoaAnimList[5].NextAnim = 6;
	KlonoaAnimList[5].TransitionSpeed = 0.25f;
	KlonoaAnimList[5].AnimationSpeed = 0.5f;

	KlonoaAnimList[6].Animation->motion = KlonoaANM[anmID_idle]->getmotion();
	KlonoaAnimList[6].Property = 4;
	KlonoaAnimList[6].NextAnim = 0;
	KlonoaAnimList[6].TransitionSpeed = 0.25f;
	KlonoaAnimList[6].AnimationSpeed = 1.0f;

	//jump
	KlonoaAnimList[14].Animation->motion = KlonoaANM[anmID_jump]->getmotion();
	KlonoaAnimList[14].Property = 3;
	KlonoaAnimList[14].NextAnim = 14;
	KlonoaAnimList[14].TransitionSpeed = 0.5f;
	KlonoaAnimList[14].AnimationSpeed = 0.5f;

	//hang pulley / heli
	KlonoaAnimList[47].Animation->motion = KlonoaANM[anmID_holdOne]->getmotion();
	KlonoaAnimList[47].Property = 3;
	KlonoaAnimList[47].NextAnim = 47;
	KlonoaAnimList[47].TransitionSpeed = 0.5f;
	KlonoaAnimList[47].AnimationSpeed = 0.5f;

	//idle hold tree
	KlonoaAnimList[54].Animation->motion = KlonoaANM[anmID_holdObj]->getmotion();
	KlonoaAnimList[54].Property = 3;
	KlonoaAnimList[54].NextAnim = 54;
	KlonoaAnimList[54].TransitionSpeed = 0.5f;
	KlonoaAnimList[54].AnimationSpeed = 0.5f;

	//hold obj
	KlonoaAnimList[62].Animation->motion = KlonoaANM[anmID_holdObj]->getmotion();
	KlonoaAnimList[62].Property = 4;
	KlonoaAnimList[62].NextAnim = 18;
	KlonoaAnimList[62].TransitionSpeed = 0.5;
	KlonoaAnimList[62].AnimationSpeed = 1.0f;

	//hold rocket
	KlonoaAnimList[80].Animation->motion = KlonoaANM[anmID_holdObj]->getmotion();
	KlonoaAnimList[80].NextAnim = 80;
	KlonoaAnimList[80].TransitionSpeed = 0.125;
	KlonoaAnimList[80].AnimationSpeed = 1.0f;

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

	//victory.
	KlonoaAnimList[75].Animation->motion = KlonoaANM[anmID_victory]->getmotion();
	KlonoaAnimList[75].Property = 4;
	KlonoaAnimList[75].NextAnim = 76;
	KlonoaAnimList[75].TransitionSpeed = 0.5f;
	KlonoaAnimList[75].AnimationSpeed = 0.5f;

	//victory standing.
	KlonoaAnimList[76].Animation->motion = KlonoaANM[anmID_victoryStd]->getmotion();
	KlonoaAnimList[76].NextAnim = 0;
	KlonoaAnimList[76].Property = 3;
	KlonoaAnimList[76].TransitionSpeed = 0.5f;
	KlonoaAnimList[76].AnimationSpeed = 0.5f;
}

void InitKlonoaCharSelAnim()
{
	SonicCharSelAnim[0] = KlonoaAnimList[11].Animation;
	SonicCharSelAnim[1] = KlonoaAnimList[75].Animation;
	SonicCharSelAnim[2] = KlonoaAnimList[76].Animation;
	SonicCharSelAnim[3] = 0;
}

void init_KlonoaModelsAnim()
{
	LoadKlonoa_Files();
}


void initKlonoa()
{	
	init_KlonoaModelsAnim();

	Sonic_Main_t.Hook(Klonoa_Main_r);
	Sonic_RunsActions_t.Hook(Klonoa_runsActions_r);
	Sonic_Display_t.Hook(Klonoa_Display_r);

	WriteJump((void*)0x49AB47, SetAnimList);
	WriteData((short*)0x49ACD8, (short)0x9090);
	WriteData<2>((void*)0x4916A5, 0x90u); // disable metal's weird tilting thing
	WriteData((char*)0x49BE22, (char)0xEB);

	WriteData<1>((int*)0x493500, 0xC3); //disable sonic morph
	WriteData<1>((int*)0x4937B0, 0xC3); //disable morph head

	WriteCall((void*)0x418214, Klonoa_late_ActionEx);
	WriteCall((void*)0x41815E, Klonoa_linkEx);
	WriteJump(InitSonicCharSelAnims, InitKlonoaCharSelAnim);
}