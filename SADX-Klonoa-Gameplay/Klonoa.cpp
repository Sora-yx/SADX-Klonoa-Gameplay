#include "pch.h"
#include "abilities.h"
#include "objects.h"
#include "hp.h"

bool klonoa = false; //check if at least one player is Klonoa
bool klonoaPnum[PMax] = { false }; //used to get pNum of a klonoa player

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
	if (klonoa && klonoaPnum[pnum])
	{
		return true;
	}

	return false;
}

int getKlonoaPlayer()
{
	for (uint8_t i = 0; i < PMax; i++)
	{
		if (klonoa && playertwp[i] && klonoaPnum[i])
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
	mwp->mj.reqaction = anm_hurt;
	data->flag &= 0xFFFBu;
	mwp->spd.x = -mwp->p.run_speed;
	mwp->spd.y = mwp->p.jmp_y_spd;
	data->flag &= 0xFAFFu;
	char rng = rand() % 2;

	if (useHP || Rings > 0)
		PlayCustomSoundVolume(rng ? kl_pain : kl_pain2, 1.0f);

	if ( (mwp->item & 3) == 0)
		DamageKlonoa(charid, 1);

	return 1;
}

signed int KlonoaCheckBeInTheAir(playerwk* co2, taskwk* data, klonoawk* klwk)
{
	__int16 flag = data->flag;

	if ((flag & (Status_OnColli | Status_Ground)) != 0)
	{
		return 0;
	}

	if (klwk->enemyGrabPtr || klwk->charBossGrabbed)
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

short twistamount[PMax] = {};
void(__cdecl** NodeCallbackFuncPtr)(NJS_OBJECT* obj) = (decltype(NodeCallbackFuncPtr))0x3AB9908;

//Calc Klonoa hand and root Pos for objects position
void NodeCallback2(NJS_OBJECT* obj)
{
	for (int i = 0; i < PMax; i++) {
		if (!isKlonoa(i))
			continue;

		char pnum = i;
		auto co2 = playerpwp[pnum];

		if (!co2)
			continue;

		auto kl = (klonoawk*)playertp[pnum]->awp;
		auto klMDL = isSuper(pnum) ? SuperKlonoaMDL->getmodel() : KlonoaMDL->getmodel();

		float* v1 = _nj_current_matrix_ptr_;

		if (obj == klMDL->child->child) //root 2
		{
			if (playertwp[pnum]->mode == 2)
				njRotateX(v1, twistamount[pnum]);
		}                 
		else if (obj == klMDL->getnode(37)) 
		{
			NJS_VECTOR v = { 0.0f, -1.5f, 0.0f };

			njCalcPoint(v1, &v, &co2->righthand_pos); //right hand pos
			v.z = 1;
			njCalcVector(v1, &v, &co2->righthand_vec);  //right hand vec
			SetVectorDiff(&co2->righthand_pos);
			SetVectorDiff(&co2->righthand_vec);
		}             
		else if (obj == klMDL->getnode(32))
		{
			NJS_VECTOR v = { 0, -1.5, 0 };
			njCalcPoint(v1, &v, &co2->lefthand_pos); //left hand pos
			v.z = 1;
			njCalcVector(v1, &v, &co2->lefthand_vec); //left hand vec
			SetVectorDiff(&co2->lefthand_pos);
			SetVectorDiff(&co2->lefthand_vec);
		}
		else if (obj == klMDL->getnode(33))
		{
			NJS_VECTOR v = { 0, 1.0f, 0 };
			njCalcPoint(v1, &v, &kl->ringPos); //ring pos
			v.z = 1;
			njCalcVector(v1, &v, &kl->ringVec); // ring vec
			SetVectorDiff(&kl->ringPos);
			SetVectorDiff(&kl->ringVec);
		}
	}
}

void Klonoa_Delete_r(task* obj)
{
	klonoa = false;

	if (obj->twp)
		klonoaPnum[obj->twp->counter.b[0]] = false;

	FreeAllCustomSounds();
	Sonic_Delete_t.Original(obj);
}

void __cdecl Klonoa_Display_r(task* obj)
{
	auto data = obj->twp;

	if (MissedFrames || (!IsVisible(&data->pos, 15.0f)))
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

	if (co2->mj.mtnmode == 2)
		curAnim = (unsigned __int16)co2->mj.action;

	if (!(data->wtimer & 2))
	{
		bool super = isSuper(pnum);
		NJS_TEXLIST* texture = super ? &SuperKlonoaTexList : &KlonoaTexList;
		njSetTexture(texture);

		njPushMatrix(0);

		NJS_VECTOR pos = data->cwp->info->center;
		pos.y -= kloGetPosYDiff(curAnim);
		njTranslateV(0, &pos);

		njRotateZ_(data->ang.z);
		njRotateX_(data->ang.x);
		njRotateY_(-0x8000 - LOWORD(data->ang.y));

		SpinDash_RotateModel(curAnim, (taskwk*)data);

		if (!super)
		{
			njScaleV(0, &KLScaleDiff); //scale order fix lighting 
			dsScaleLight(0.2f);
		}
			
		SetupChunkModelRender();

		if (super)
		{
			njScaleV(0, &KLScaleDiff);
			dsScaleLight(0.2f);
		}		

		NJS_MATRIX m = { 0 };
		njSetMatrix(m, (NJS_MATRIX_PTR)&EnvironmentMapMatrix);
		njScale((NJS_MATRIX_PTR)&EnvironmentMapMatrix, 0.25f, 0.25f, 0.25f);

		if (data->ewp->action.list) //cutscene / charsel
		{
			DrawEventAction(data);
		}
		else //regular gameplay
		{
			NJS_ACTION* action = co2->mj.plactptr[curAnim].actptr;
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
		njSetMatrix((NJS_MATRIX_PTR)&EnvironmentMapMatrix, m);
		njPopMatrix(1u);
	}

	dsReScaleLight();
	Direct3D_PerformLighting(0);
	ClampGlobalColorThing_Thing();
	RestoreConstantAttr();
	Direct3D_ResetZFunc();

	if (IsGamePaused())
		DrawCharacterShadow((EntityData1*)data, (struct_a3*)&co2->shadow);
}

void __cdecl Klonoa_runsActions_r(taskwk* data, motionwk2* data2, playerwk* co2)
{
	if (!co2 || !data)
	{
		return;
	}

	auto data1 = (EntityData1*)data;
	char pnum = data->charIndex;
	auto klwk = (klonoawk*)playertp[pnum]->awp;
	char action = data->mode;

	if (!isKlonoa(pnum) || EV_MainThread_ptr || MetalSonicFlag || isTailsRace(pnum))
	{
		return Sonic_RunsActions_t.Original(data, data2, co2);
	}

	if (co2 && (co2->item & 0x8000) != 0)
	{
		data->wtimer = 0;
		data->flag |= 0x8000u;
	}
	else if (data->wtimer)
	{
		if (data->wtimer != 32752)
		{
			auto v13 = data->flag & 0xFFFB;
			data->wtimer--;
			data->flag = v13;
		}

		if (data->wtimer <= 1 && useHP && isMultiActive() && HP[pnum] <= 0)
		{
			ResetKlonoaHP(pnum);
		}
	}
	else if (KlonoaCheckDamage(data, co2))
	{
		data->flag |= 0x8000;

		if ((data->flag & 0x800) != 0)
			Character_Grab(data1, (CharObj2*)co2); //throw object out

		data->flag &= 0xC6FFu;
		auto v10 = co2->item;

		if ((v10 & 3) != 0)
			co2->item = v10 & 0xFFFC;
		else
			DamegeRingScatter(data->counter.b[0]);
	}
	else
	{
		data->flag &= 0x7FFFu;
	}

	CheckKlonoaEnemyPtr(klwk, data, co2);
	KlonoaManageVictoryMotion(co2);

	if (!IsIngame())
	{
		return Sonic_RunsActions_t.Original(data, data2, co2);
	}

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
				co2->mj.reqaction = isSuper(pnum) ? anm_SSFall : anm_fall;
			}

			if (Fly_CheckInput(data, co2))
			{
				return;
			}

			if (hover_CheckInput(data, co2, klwk) || KlonoaWBullet_CheckInput(data, co2, klwk))
			{
				data->flag &= ~Status_Attack;
				data->flag &= ~Status_Ball;
				return;
			}
		}
		break;
	case act_hurt:

		if (Sonic_NAct((CharObj2*)co2, data1, (EntityData2*)data2) || (data->wtimer == 120 * 2))
		{
			return;
		}

		if ((data->flag & 3) != 0)
		{
			data->ang.x = data2->ang_aim.x;
			data->ang.z = data2->ang_aim.z;

			if (SonicCheckStop(co2, data))
			{
				if (klwk->enemyGrabPtr || klwk->charBossGrabbed)
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
				if (klwk->enemyGrabPtr || klwk->charBossGrabbed)
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

		if ((JumpButtons & Controllers[data->charIndex].HeldButtons) == 0 || (data->flag & 3))
		{
			Klonoa_Fall(data, co2);
			return;
		}

		if (klwk->hoverTimer > 0)
		{
			klwk->hoverTimer--;
		}
		else if (!infiniteHover)
		{
			Klonoa_Fall(data, co2);
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
		if (Sonic_NAct((CharObj2*)co2, data1, (EntityData2*)data2) || !klwk->enemyGrabPtr && !klwk->charBossGrabbed)
		{
			DropEnemy(klwk);
			break;
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

		return;
	case act_holdRun:
		if (Sonic_NAct((CharObj2*)co2, data1, (EntityData2*)data2) || !klwk->enemyGrabPtr && !klwk->charBossGrabbed)
		{
			DropEnemy(klwk);
			break;
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

		return;
	case act_holdJump:
	case act_holdFall:
		if (Sonic_NAct((CharObj2*)co2, data1, (EntityData2*)data2) || !klwk->enemyGrabPtr && !klwk->charBossGrabbed)
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

			data->mode = isSuper(pnum) ? 75 : 1;
			co2->mj.reqaction = isSuper(pnum) ? 134 : 0;
			data->flag &= 0xFAu;
			return;
		}

		Fly_ManageMotion(data, co2);
		break;
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
			klonoaPnum[pnum] = false;
			break;
		}

		if (LoadKlonoa_Worker(obj)) {
			for (int i = 0; i < LengthOfArray(klonoaTex_Entry); i++)
			{
				LoadPVM(klonoaTex_Entry[i].Name, klonoaTex_Entry[i].TexList);
			}

			ResetKlonoaHP(pnum);
			klonoa = true;
			klonoaPnum[pnum] = true;
		}
		else
		{
			PrintDebug("Klonoa Mod: Failed to load Klonoa worker; mod won't work...\n");
			klonoa = false;
			klonoaPnum[pnum] = false;
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

	for (uint8_t i = 0; i < LengthOfArray(klonoaTex_Entry); i++) {
		HelperFunctionsGlobal.RegisterCharacterPVM(Characters_Sonic, klonoaTex_Entry[i]);
	}

	//fix Klonoa stuck on jump panel
	JumpPanel_Collision_[3].center.y = 2.0f;
	JumpPanel_Collision_[3].a = 7.0f;

	WriteData<1>((int*)0x493730, 0xC3); //disable event head
	WriteData<5>((int*)0x46AD09, 0x90); //The game locks player control on the capsule BEFORE you even open it, which softlock Klonoa in flying mode, we disable that.
}