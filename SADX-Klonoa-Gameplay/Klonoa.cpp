#include "pch.h"
#include "abilities.h"
#include "objects.h"
#include "hp.h"

bool klonoa = false;
uint8_t klonoaPnum = 0;

const NJS_VECTOR KLScaleDiff = { 0.2f, 0.2f, 0.2f };
const NJS_VECTOR orgScale = { 1.0f, 1.0f, 1.0f };

ModelInfo* KlonoaMDL = nullptr;
ModelInfo* SuperKlonoaMDL = nullptr;

static NJS_TEXNAME KlonoaTex[2] = { 0 };
NJS_TEXLIST KlonoaTexList = { arrayptrandlength(KlonoaTex) };

static NJS_TEXNAME SuperKlonoaTex[3] = { 0 };
NJS_TEXLIST SuperKlonoaTexList = { arrayptrandlength(SuperKlonoaTex) };

PVMEntry klonoaTex_Entry[] = {
	{"KlonoaTex", &KlonoaTexList},
	{"KNU_EFF", &KNU_EFF_TEXLIST},
	{"SuperKlonoaTex", &SuperKlonoaTexList},
};

TaskHook Sonic_Main_t(SonicTheHedgehog);
TaskHook Sonic_Display_t(SonicDisplay);
TaskHook Sonic_Delete_t(SonicDestruct);
FunctionHook<void, taskwk*, motionwk2*, playerwk*> Sonic_RunsActions_t((intptr_t)Sonic_Act1);

bool isKlonoa(char pnum)
{
	if (klonoa && playertwp[pnum] && playertwp[pnum]->charIndex == klonoaPnum)
	{
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

bool LoadKlonoa_Worker(task* obj) {

	if (obj->twp->mode == 0 && !obj->awp) {
		void* mem = AllocateMemory(sizeof(klonoawk));
		memset(mem, 0, sizeof(klonoawk));
		obj->awp = (anywk*)mem;
		return true;
	}

	return false;
}

bool isKlonoaHold(char pnum)
{
	if (!playertwp[pnum])
		return false;

	return playertwp[pnum]->mode >= act_super_jump && playertwp[pnum]->mode <= act_throwAir;
}

//regular draw during gameplay
void DrawKlonoa(playerwk* co2, int animNum, NJS_ACTION* action)
{
	NJS_ACTION act2 = *action;

	if (QueueCharacterAnimations)
	{
		DrawQueueDepthBias = -5952.0f;
		njCnkAction_Queue(&act2, co2->mj.nframe, QueuedModelFlagsB_EnableZWrite);
		DrawQueueDepthBias = 0.0;
	}
	else
	{
		njCnkAction(&act2, co2->mj.nframe);
	}
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
	data->mode = act_fall;
	co2->mj.reqaction = anm_fall;
}

signed int KlonoaCheckDamage(taskwk* data, playerwk* mwp)
{
	if (data->id == 2 || (data->flag & 4) == 0 && (!mwp || (mwp->attr & 0x10000) == 0))
		return 0;

	char charid = data->counter.b[0];
	data->wtimer = 120 * 2;
	VibConvergence(charid, 7, 59, 6);

	auto curAction = data->mode;

	if (curAction >= 46 && curAction <= 50)
	{
		dsPlay_oneshot(1222, 0, 0, 0);
		data->mode = 50;
		mwp->mj.reqaction = 46;
		PSetCrashEffect(data);

		if (GetNumRing() > 0)
		{
			data->pos.x += (float)(mwp->cstsp->tnorm.x * 5.0f);
			data->pos.y += (float)(mwp->cstsp->tnorm.y * 5.0f);
			data->pos.z += (float)(mwp->cstsp->tnorm.z * 5.0f);
			mwp->spd.x = mwp->spd.x * (float)0.2;
			mwp->spd.y = mwp->p.jmp_y_spd;
			data->ang.x = 0;
			data->ang.z = 49152;
		}

		return 1;
	}

	if (curAction >= 62 && curAction <= 68)
	{
		dsPlay_oneshot(1233, 0, 0, 0);
		data->mode = 68;
		mwp->mj.reqaction = 110;
		data->flag &= 0xFFFBu;
		return 0;
	}
	if ((mwp->equipment & 0x8000) != 0)
	{
		mwp->spd.x = mwp->spd.x * 0.5f;
		return 0;
	}

	if (curAction == 45)
		return 0;

	if (curAction == 44 && (ssStageNumber < 15 || ssStageNumber > 25))
		CameraReleaseEventCamera();

	dsPlay_oneshot(1233, 0, 0, 0);

	data->mode = act_hurt;
	mwp->mj.reqaction = 23;
	data->flag &= 0xFFFBu;
	mwp->spd.x = -mwp->p.run_speed;
	mwp->spd.y = mwp->p.jmp_y_spd;
	data->flag &= 0xFAFFu;
	char rng = rand() % 2;


	if (useHP || Rings > 0)
		PlayCustomSoundVolume(rng ? kl_pain : kl_pain2, 1.0f);

	DamageKlonoa(1);
	return 1;
}

signed int KlonoaCheckBeInTheAir(playerwk* co2, taskwk* data, klonoawk* klwk)
{
	__int16 flag = data->flag;

	if ((flag & (Status_OnColli | Status_Ground)) != 0)
	{
		return 0;
	}

	if (klwk->enemyGrabPtr)
	{
		data->mode = act_holdFall;
		co2->mj.reqaction = anm_holdFall;
		return 1;
	}

	if ((flag & Status_Ball) != 0)
	{
		data->mode = act_jump;
		return 1;
	}

	if ((flag & Status_HoldObject) != 0)
	{
		if ((PCheckHoldObject(data) & 0x200) != 0)
		{
			data->mode = 28;
			co2->mj.reqaction = 57;
			return 1;
		}

		Character_Grab((EntityData1*)data, (CharObj2*)co2);
	}

	data->mode = 12;
	co2->mj.reqaction = anm_fall;
	return 1;
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
	auto klMDL = isSuper(pnum) ? SuperKlonoaMDL->getmodel() : KlonoaMDL->getmodel();

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
	}
	else if (obj == klMDL->child->child->child->sibling->sibling->sibling->sibling->sibling->child->sibling->sibling->sibling->child->child->child)
	{
		NJS_VECTOR v = { 0, -1.5, 0 };
		njCalcPoint(v1, &v, &co2->lefthand_pos); //left hand pos
		v.z = 1;
		njCalcVector(v1, &v, &co2->lefthand_vec); //left hand vec
		SetVectorDiff(&co2->lefthand_pos);
		SetVectorDiff(&co2->lefthand_vec);
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

void Klonoa_Delete_r(task* obj)
{
	klonoa = false;
	FreeAllCustomSounds();
	Sonic_Delete_t.Original(obj);
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

	Direct3D_PerformLighting(2);
	CnkDisableSpecular();
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
		NJS_TEXLIST* texture = isSuper(pnum) ? &SuperKlonoaTexList : &KlonoaTexList;
		njSetTexture(texture);

		njPushMatrix(0);

		NJS_VECTOR pos = data->cwp->info->center;
		pos.y -= kloGetPosYDiff(curAnim);
		njTranslateV(0, &pos);

		njScaleV(0, &KLScaleDiff);

		njRotateZ_(data->ang.z);
		njRotateX_(data->ang.x);
		njRotateY_(-0x8000 - LOWORD(data->ang.y));

		SpinDash_RotateModel(curAnim, (taskwk*)data);

		NJS_ACTION* action = co2->mj.plactptr[curAnim].actptr;

		SetupChunkModelRender();

		if (data->ewp->action.list) //cutscene / charsel
		{
			DrawEventAction(data);
		}
		else //regular gameplay
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

		ResetChunkModelRender();

		njPopMatrix(1u);
	}

	Direct3D_PerformLighting(0);
	ClampGlobalColorThing_Thing();
	RestoreConstantAttr();
	Direct3D_ResetZFunc();
	CnkRestoreSpecular();

	if (IsGamePaused())
		DrawCharacterShadow((EntityData1*)data, (struct_a3*)&co2->shadow);
}

void __cdecl Klonoa_runsActions_r(taskwk* data, motionwk2* data2, playerwk* co2)
{
	if (!co2)
	{
		return;
	}

	if (!isKlonoa(data->charIndex) || EV_MainThread_ptr || !IsIngame() || MetalSonicFlag)
	{
		return Sonic_RunsActions_t.Original(data, data2, co2);
	}

	auto data1 = (EntityData1*)data;
	char pnum = data->charIndex;
	auto klwk = (klonoawk*)playertp[pnum]->awp;

	if (co2->item < 0)
	{
		data->flag |= 0x8000u;
		data->wtimer = 0;
	}
	else
	{
		int timer = data->wtimer;

		if (timer)
		{
			if (timer != 32752)
			{
				data->flag &= 0xFBu;
				data->wtimer = timer - 1;
			}
		}
		else if (KlonoaCheckDamage(data, co2))
		{
			data->flag |= 0x8000;

			if ((data->flag & Status_HoldObject) != 0)
			{
				Character_Grab((EntityData1*)data1, (CharObj2*)co2);
			}

			data->flag &= 0xC6FFu;

			if ((co2->item & (Powerups_MagneticBarrier | Powerups_Barrier)) != 0)
			{
				co2->item = co2->item & 0xFFFC;
			}
			else
			{
				HurtCharacter(pnum);
			}
		}
		else
		{
			data->flag &= 0x7FFFu;
		}
	}

	CheckKlonoaEnemyPtr(klwk, data, co2);

	switch (data->mode)
	{
	case act_stnd:
	case 2:
		if ((Sonic_NAct((CharObj2*)co2, data1, (EntityData2*)data2) || Sonic_CheckPickUpObject((CharObj2*)co2, data1)))
		{
			break;
		}

		if (KlonoaWBullet_CheckInput(data, co2, klwk))
			return;

		break;
	case act_jump:
	case act_fall:
	case 78:
	case 82:

		if (Sonic_NAct((CharObj2*)co2, data1, (EntityData2*)data2) || (data->flag & 3))
		{
			klwk->hoverUsed = false;
			break;
		}
		else
		{
			if (co2->spd.y < 0.0f)
			{
				co2->mj.reqaction = anm_fall;
			}

			if (Fly_CheckInput(data, co2))
			{
				return;
			}

			if ( hover_CheckInput(data, co2, klwk) || KlonoaWBullet_CheckInput(data, co2, klwk))
			{
				data->flag &= ~Status_Attack;
				data->flag &= ~Status_Ball;
				return;
			}
		}
		break;
	case act_hurt:

		if (Sonic_NAct((CharObj2*)co2, data1, (EntityData2*)data2) || data->wtimer == 120)
		{
			return;
		}

		if ((data->flag & 3) != 0)
		{
			data->ang.x = data2->ang_aim.x;
			data->ang.z = data2->ang_aim.z;

			if (SonicCheckStop(co2, data))
			{
				if (klwk->enemyGrabPtr)
				{
					data->mode = act_holdStd;
					co2->mj.reqaction = anm_holdStd;
				}
				else
				{
					data->mode = 1;
					co2->mj.reqaction = 2;
				}

			}
			else
			{
				if (klwk->enemyGrabPtr)
				{
					data->mode = act_holdRun;
					co2->mj.reqaction = anm_holdRun;
				}
				else
				{
					data->mode = 2;
					co2->mj.reqaction = 2;
				}
			}
		}

		data->flag &= 0xFAu;
		return;
	case act_hover:
		if (Sonic_NAct((CharObj2*)co2, data1, (EntityData2*)data2))
		{
			break;
		}

		if ((JumpButtons & Controllers[data->charIndex].HeldButtons) == 0 || data->flag & 3)
		{
			Klonoa_Fall(data, co2);
			break;
		}

		if (klwk->hoverTimer > 0)
		{
			klwk->hoverTimer--;
		}
		else if (!infiniteHover)
		{
			Klonoa_Fall(data, co2);
			break;
		}
		return;
	case act_windBullet:
	case act_windBulletAir:
		if (Sonic_NAct((CharObj2*)co2, data1, (EntityData2*)data2))
		{
			break;
		}

		Klonoa_ManageBullet(data, co2, klwk);
		return;
	case act_super_jump:
		if (Sonic_NAct((CharObj2*)co2, data1, (EntityData2*)data2))
		{
			klwk->superJumpCount = 0;
			break;
		}

		if (data->flag & 3)
		{
			if (PCheckBreak(data) && co2->spd.x > 0.0f)
			{
				data->mode = act_brake;
				co2->mj.reqaction = 0;
			}
			else
			{
				if (SonicCheckStop(co2, data))
				{
					data->ang.x = data2->ang_aim.x;
					data->ang.z = data2->ang_aim.z;
					co2->mj.reqaction = anm_std;
				}
				else
				{
					data->ang.x = data2->ang_aim.x;
					data->ang.z = data2->ang_aim.z;
					data->mode = act_walk;
					co2->mj.reqaction = 2;
				}
				data->flag &= 0xFAu;
			}

			break;
		}
		else
		{
			if (DoJumpThing((EntityData1*)data1, (CharObj2*)co2))
			{
				co2->spd.y = co2->p.jmp_addit * 0.80000001f + co2->spd.y;
			}

			if (KlonoaWBullet_CheckInput(data, co2, klwk) || hover_CheckInput(data, co2, klwk))
			{
				data->flag &= ~Status_Attack;
				data->flag &= ~Status_Ball;
				return;
			}
			else if (data2->spd.y <= 0.0f)
			{
				data->mode = act_fall;
				co2->mj.reqaction = anm_fall;
			}
		}

		break;
	case act_holdStd:
		if (Sonic_NAct((CharObj2*)co2, data1, (EntityData2*)data2) || !klwk->enemyGrabPtr)
		{
			DropEnemy(klwk);
			return;
		}

		if (HoldEnemy_CheckJump(data, co2) || ThrowEnemy_CheckInput(data, co2, klwk) || KlonoaCheckBeInTheAir(co2, data, klwk))
		{
			return;
		}

		if (GetAnalog((EntityData1*)data1, 0, 0) || co2->spd.x != 0.0f)
		{
			data->mode++;
			co2->mj.reqaction = anm_holdWalk;
			return;
		}

		break;
	case act_holdRun:
		if (Sonic_NAct((CharObj2*)co2, data1, (EntityData2*)data2) || !klwk->enemyGrabPtr)
		{
			DropEnemy(klwk);
			return;
		}

		if (HoldEnemy_CheckJump(data, co2) || ThrowEnemy_CheckInput(data, co2, klwk) || KlonoaCheckBeInTheAir(co2, data, klwk))
		{
			return;
		}

		if (co2->spd.x <= co2->p.run_speed - 0.5f)
		{
			co2->mj.reqaction = anm_holdWalk;
		}
		else
		{
			co2->mj.reqaction = anm_holdRun;
		}

		if (PCheckBreak(data) && co2->spd.x >= (double)co2->p.jog_speed)
		{
			data->mode = act_holdStd;
			co2->mj.reqaction = anm_holdStd;
			PlaySound(18, 0, 0, 0);
			return;
		}
		else if (co2->spd.x == 0.0f)
		{
			data->mode = act_holdStd;
			co2->mj.reqaction = anm_holdStd;
			return;
		}

		break;
	case act_holdJump:
	case act_holdFall:
		if (Sonic_NAct((CharObj2*)co2, data1, (EntityData2*)data2) || !klwk->enemyGrabPtr)
		{
			DropEnemy(klwk);
			klwk->superJumpCount = 0;
			break;
		}

		if (ThrowEnemy_CheckInput(data, co2, klwk))
		{
			return;
		}

		if (data->flag & 3)
		{
			data->ang.x = data2->ang_aim.x;
			data->ang.z = data2->ang_aim.z;

			if (co2->spd.x > 0.0f)
			{
				data->mode = act_holdRun;
				co2->mj.reqaction = anm_holdRun;
			}
			else
			{
				data->mode = act_holdStd;
				co2->mj.reqaction = anm_holdStd;
			}

			data->flag &= 0xFAu;
			return;
		}
		else
		{
			if (DoJumpThing((EntityData1*)data1, (CharObj2*)co2))
			{
				co2->spd.y = co2->p.jmp_addit * 0.80000001f + co2->spd.y;
			}

			if (KlonoaSJump_CheckInput(data, co2, klwk))
			{
				DropEnemy(klwk);
				return;
			}
		}

		return;
	case act_throwStd:
	case act_throwAir:

		if (Sonic_NAct((CharObj2*)co2, data1, (EntityData2*)data2))
		{
			break;
		}

		if (co2->mj.reqaction != anm_throwAir && co2->mj.reqaction != anm_throwStd)
		{
			data->mode = 1;
			co2->mj.reqaction = 0;
			return;
		}
		return;
	case act_flyStd:
	case act_fly:

		if (Sonic_NAct((CharObj2*)co2, data1, (EntityData2*)data2))
		{
			break;
		}

		if (!PauseEnabled && !TimeThing) //if level is about to finish, force klonoa to fall to avoid softlock
		{
			Klonoa_Fall(data, co2);
			break;
		}

		if (data->flag & 3)
		{
			data->ang.x = data2->ang_aim.x;
			data->ang.z = data2->ang_aim.z;

			data->mode = 1;
			co2->mj.reqaction = 0;
			data->flag &= 0xFAu;
			return;
		}

		Fly_ManageMotion(data, co2);

		return;
	}

	Sonic_RunsActions_t.Original(data, data2, co2);
}

void __cdecl Klonoa_Main_r(task* obj)
{
	auto data = obj->twp;
	motionwk2* data2 = (motionwk2*)obj->mwp;
	playerwk* co2 = (playerwk*)obj->mwp->work.l;
	char pnum = data->counter.b[0];

	switch (data->mode)
	{
	case 0:

		ReplaceSonicAnimPtr();

		if (MetalSonicFlag)
		{
			klonoa = false;
			break;
		}	

		if (LoadKlonoa_Worker(obj)) {

			for (int i = 0; i < LengthOfArray(klonoaTex_Entry); i++)
			{
				LoadPVM(klonoaTex_Entry[i].Name, klonoaTex_Entry[i].TexList);
			}

			ResetKlonoaHP();
			klonoa = true;
		}
		else
		{
			PrintDebug("Klonoa Mod: Failed to load Klonoa worker; mod won't work...\n");
			klonoa = false;
			break;
		}


		break;
	case act_hover:
		hover_Physics(data, data2, co2);
		break;

	case act_super_jump:
	case act_windBulletAir:
	case act_holdJump:
	case act_holdFall:
		PResetAngle(data, data2, co2);

		if (!PResetAccelerationAir(data, data2, co2))
		{
			PGetAccelerationAir(data, data2, co2);
		}

		RegularPhysicsFunctions(data, data2, co2);

		break;
	case act_windBullet:
	case act_holdStd:
		PGetRotation(data, data2, co2);
		PGetAcceleration(data, data2, co2);
		RegularPhysicsFunctions(data, data2, co2);
		break;
	case act_holdRun:
		PGetAcceleration(data, data2, co2);
		PGetSpeed(data, data2, co2);
		if (PSetPosition(data, data2, co2) == 2)
		{
			co2->spd.x = -co2->spd.x;
			co2->spd.z = -co2->spd.z;
		}
		else
		{
			PResetPosition(data, data2, co2);
		}
		break;
	case act_flyStd:
		PResetAngle(data, data2, co2);
		RegularPhysicsFunctions(data, data2, co2);
		co2->spd.y = 0.0f;
		break;
	case act_fly:
		PResetAngle(data, data2, co2);
		SuperKlonoa_Fly(co2, data, data2);
		RegularPhysicsFunctions(data, data2, co2);
		break;
	}

	Sonic_Main_t.Original(obj);

	klonoaPnum = pnum;
}

void __cdecl Sonic_Snowboard_Main_r(task* a1)
{
	auto data = a1->twp;
	int pNum = (unsigned __int8)data->counter.b[0];
	auto data2 = (motionwk2*)EntityData2Ptrs[pNum];
	auto pData = playertwp[pNum];
	auto objMtn = a1->mwp;
	auto curAnim = playerpwp[pNum]->mj.action;
	auto klwk = (klonoawk*)playertp[pNum]->awp;

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

void initKlonoa()
{
	KlonoaMDL = LoadChunkModel("Klonoa");
	SuperKlonoaMDL = LoadChunkModel("SuperKlonoa");
	Init_KlonoaAnim();

	Sonic_Main_t.Hook(Klonoa_Main_r);
	Sonic_RunsActions_t.Hook(Klonoa_runsActions_r);
	Sonic_Display_t.Hook(Klonoa_Display_r);
	Sonic_Delete_t.Hook(Klonoa_Delete_r);

	init_Objects();

	WriteJump(Sonic_Snowboard_Main, Sonic_Snowboard_Main_r);
	PickDrop_Patches();

	for (int i = 0; i < LengthOfArray(klonoaTex_Entry); i++) {
		HelperFunctionsGlobal.RegisterCharacterPVM(Characters_Sonic, klonoaTex_Entry[i]);
	}
}