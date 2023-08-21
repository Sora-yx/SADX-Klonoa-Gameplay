#include "pch.h"
#include "hud.h"
#include "hp.h"

ModelInfo* dreamStoneMDL = nullptr;
ModelInfo* alarmClockMDL = nullptr;
ModelInfo* boardMDL = nullptr;
ModelInfo* heartMDL = nullptr;
ModelInfo* largeHeartMDL = nullptr;

static NJS_TEXNAME KObjCommonTex[4] = { 0 };
static NJS_TEXLIST KObjComTexlist = { arrayptrandlength(KObjCommonTex) };

TaskHook CheckPoint_t((intptr_t)CheckPoint_Main);
TaskHook Ring_Display_t((intptr_t)Ring_Display);
static FunctionHook<void> LoadLevelObjTextures_t((intptr_t)LoadLevelObjTextures);

bool isKlonoaAndObj(ModelInfo* obj)
{
	int player = getKlonoaPlayer();

	if (player < 0 || !obj)
	{
		return false;
	}

	return true;
}

void __cdecl DreamStone_Display(task* obj)
{
	if (!obj)
	{
		return Ring_Display_t.Original(obj);
	}

	auto data = obj->twp;
	NJS_VECTOR scl = { 0.5f, 0.5f, 0.5f };
	if (!MissedFrames && IsVisible(&data->pos, 20.0))
	{
		if (data->btimer < 0x32u && (data->scl.y) != 0xC9742400)
		{
			sub_49ED70(data, data->scl.y);
		}

		njSetTexture(&KObjComTexlist);
		njPushMatrix(0);
		njTranslate(0, data->pos.x, data->pos.y, data->pos.z);

		njRotateY_((unsigned __int16)data->ang.y);
		njScaleV(0, &scl);

		if (QueueCharacterAnimations)
		{
			DrawQueueDepthBias = 32.0f;
			dsDrawObject(dreamStoneMDL->getmodel());
			DrawQueueDepthBias = 0.0f;
		}
		else
		{
			dsDrawObject(dreamStoneMDL->getmodel());
		}

		njPopMatrix(1u);
	}
}

void __cdecl DrawDreamStone(NJS_MODEL_SADX* a1)
{
	if (!dreamStoneMDL)
	{
		return DrawModel(a1);
	}

	NJS_VECTOR scl = { 0.5f, 0.5f, 0.5f };
	njSetTexture(&KObjComTexlist);
	njScaleV(0, &scl);
	dsDrawObject(dreamStoneMDL->getmodel());
}

void __cdecl DrawDreamStoneClip(NJS_MODEL_SADX* model, float scale)
{
	if (!dreamStoneMDL)
	{
		return ds_DrawModelClip(model, scale);
	}

	float radius = 0.0f;
	NJS_VECTOR scl = { 0.5f, 0.5f, 0.5f };
	njSetTexture(&KObjComTexlist);
	njScaleV(0, &scl);

	if ((fogemulation & 2) != 0 && (fogemulation & 1) != 0 && !FogEnabled)
	{
		fogemulation &= ~2u;
		DrawQueueDepthBias = -47952.0f;

		dsDrawObject(dreamStoneMDL->getmodel());
		fogemulation |= 2u;
	}

	if (!MissedFrames && !VerifyTexList(CurrentTexList))
	{
		if (model)
		{
			if (scale == 0.0f || model->r == 0.0f || (radius = scale * model->r, IsVisible(&model->center, radius)))
			{
				dsDrawObject(dreamStoneMDL->getmodel());
			}
		}
	}
}

int PlayDreamStoneSound(int ID, void* a2, int a3, void* a4)
{
	if (!dreamStoneMDL)
	{
		return PlaySound(ID, a2, a3, a4);
	}

	PlayCustomSoundVolume(se_dreamStn, 0.2f);
	return 1;
}

void Heart_Disp(task* obj)
{
	if (MissedFrames)
		return;

	auto data = obj->twp;
	njSetTexture(&KObjComTexlist);
	njPushMatrix(0);
	njTranslateV(0, &data->pos);
	njRotateY_(data->ang.y);

	dsDrawObject((NJS_OBJECT*)data->timer.l);
	njPopMatrix(1u);
}

void Heart_Exec(task* obj)
{
	auto data = obj->twp;

	switch (data->mode)
	{
	case 0:
		obj->disp = Heart_Disp;
		data->timer.l = data->scl.z == 2.0f ? (int)largeHeartMDL->getmodel() : (int)heartMDL->getmodel();
		CCL_Init(obj, (CCL_INFO*)&CSphere_Collision, 1, 4u);
		data->mode++;
		break;
	case 1:
	{
		data->ang.y += 400;
		auto player = (taskwk*)GetCollidingEntityA((EntityData1*)data);
		if (player)
		{
			if (!isKlonoa(player->counter.b[0])) //if a non klonoa player get a heart, give it to klonoa player
			{
				int id = getKlonoaPlayer();

				if (id > -1) 
					player = playertwp[id];
				else
				{
					data->mode++;
					return;
				}

			}
				
			if (data->scl.z == 2.0f) {
				PlayCustomSoundVolume(se_heartLarge, 0.3f);
				ResetKlonoaHP(player->counter.b[1]);
			}
			else
			{
				PlayCustomSoundVolume(se_heart, 0.3f);
				AddKlonoaHP(player->counter.b[1], 1);
			}

			data->mode++;
		}
	}
	break;
	default:
		FreeTask(obj);
		return;
	}

	AddToCollisionList((EntityData1*)data);
	obj->disp(obj);
}

void savepointCollision_r(task* tsk, taskwk* data)
{
	int pNum = getKlonoaPlayer();

	if (pNum < 0)
		return;

	auto cpData = savepoint_data;
	savepoint_data->tp[0]->twp->ang.x += data->ang.x;
	cpData->tp[0]->twp->ang.y = data->ang.y - cpData->ang.y;
	cpData->tp[0]->twp->ang.z = cpData->ang.z + data->ang.z;
	cpData->tp[1]->twp->ang.x = cpData->ang.x + data->ang.x;
	cpData->tp[1]->twp->ang.y = data->ang.y + cpData->ang.y;
	cpData->tp[1]->twp->ang.z = cpData->ang.z + data->ang.z;

	auto klwk = (klonoawk*)playertp[pNum]->awp;

	if (!data->cwp || data->mode >= 2)
		return;

	//todo make bullet able to hit it, (maybe add config to avoid that?)
	if (data->cwp->flag & 1 && data->cwp->hit_cwp && data->cwp->hit_cwp->mytask == playertp[pNum]) {
		data->mode = 2;
		savepoint_data->ang_spd.y = 0.0f;
		cpData->write_timer = 300;
		if (playertwp[pNum])
		{
			SetRestartData(&playertwp[pNum]->pos, (Rotation3*)&playertwp[pNum]->ang);
		}
		SetBroken(tsk);
		PlayCustomSoundVolume(se_alarmClock, 0.7f);
	}

	if (GetCollidingEntityA((EntityData1*)data))
	{
		if (!data->wtimer) {
			PlayCustomSound(se_bubbleHit);
			data->wtimer = 40;
		}
	}

	if (data->wtimer > 0)
		data->wtimer--;

	AddToCollisionList((EntityData1*)data);
}

void Bubble_ChildMain(task* obj)
{
	auto data = obj->twp;

	if (!data->mode)
	{
		data->scl = { 10.0f, 1.0f, 4.0f };
		data->pos.y += 1.5f;
		obj->disp = Bubble_Child;
		data->mode++;
	}
	else
	{
		if (obj->ptp->twp->mode == 2)
		{
			FreeTask(obj);
			return;
		}
	}

	obj->disp(obj);
}

void AlarmClock_Display(task* tsk)
{
	if (MissedFrames)
		return;

	auto data = tsk->twp;

	NJS_VECTOR scl = { 0.5f, 0.5f, 0.5f };

	njPushMatrixEx();
	njTranslateEx(&data->pos);

	njRotateZ_(data->ang.z);
	njRotateX_(data->ang.x);
	njRotateY_(data->ang.y);
	njScaleV(0, &scl);
	njSetTexture(&KObjComTexlist);

	if (data->mode < 2)
	{
		dsDrawObject(alarmClockMDL->getmodel());
	}

	njPopMatrixEx();
}

void AlarmClock_Main(task* tsk)
{
	if (CheckRangeOut(tsk))
	{
		return;
	}

	if (!isKlonoaAndObj(alarmClockMDL) || CurrentCharacter != Characters_Sonic)
	{
		return CheckPoint_t.Original(tsk);
	}

	savepoint_data = (OBJECT_SAVEPOINT_DATA*)tsk->awp;
	auto data = tsk->twp;
	data->ang.y += 400;

	switch (data->mode)
	{
	case 0:
		data->pos.y += 15.0f;

		data->mode++;
		tsk->awp = (anywk*)AllocateCPData();
		savepoint_data = (OBJECT_SAVEPOINT_DATA*)tsk->awp;
		tsk->dest = (TaskFuncPtr)LevelItem_Delete;
		tsk->disp = AlarmClock_Display;
		CCL_Init(tsk, (CCL_INFO*)&CSphere_Collision, 1, 4u);

		savepoint_data->tp[0] = CreateChildTask(
			LoadObj_Data1, (TaskFuncPtr)nullsub, tsk);
		savepoint_data->tp[1] = CreateChildTask(
			LoadObj_Data1, (TaskFuncPtr)nullsub, tsk);

		initCollidata(data);

		savepoint_data->ang.y = 0xFFFFC000;
		if (tsk->ocp)
		{
			SetFlagNoRespawn(tsk);
		}
		if (CheckBroken(tsk))
		{
			savepoint_data->ang.x = 0;
			data->mode = 3;
		}
		else
		{
			CreateChildTask((LoadObj)(LoadObj_Data1), Bubble_ChildMain, tsk);
		}
		if (useHP && hpMaxConfig != 1)
		{
			task* heart = CreateElementalTask(2, 2, Heart_Exec);
			if (heart)
			{
				heart->twp->pos = data->pos;
				heart->twp->pos.x += 20.0f;
				heart->twp->pos.z += 20.0f;
			}
		}
		break;
	case 1:
		savepointCollision_r(tsk, data);
		break;
	case 2:
		savepointCollision_r(tsk, data);
		DisplayCheckpointTime_B();
		break;
	case 3:
		savepointCollision_r(tsk, data);
		DisplayCheckpointTime_B();
		break;
	default:
		UpdateSetDataAndDelete(tsk);
		break;
	}

	if (tsk->disp && data->mode < 3)
		tsk->disp(tsk);

	LoopTaskC(tsk);
}

PVMEntry MinimalPVMs[]{
	{ "GOMA", &GOMA_TEXLIST },
	{ "PEN", &PEN_TEXLIST },
	{ "RAKO", &RAKO_TEXLIST },
	{ "KUJA", &KUJA_TEXLIST },
	{ "TUBA", &TUBA_TEXLIST },
	{ "OUM", &OUM_TEXLIST },
	{ "BANB", &BANB_TEXLIST },
	{ "USA", &USA_TEXLIST },
	{ "WARA", &WARA_TEXLIST },
	{ "GORI", &GORI_TEXLIST },
	{ "LION", &LION_TEXLIST },
	{ "ZOU", &ZOU_TEXLIST },
	{ "MOGU", &MOGU_TEXLIST },
	{ "KOAR", &KOAR_TEXLIST },
	{ "SUKA", &SUKA_TEXLIST },
};

void LoadObjTextures()
{
	LoadPVM("KObjCommon", &KObjComTexlist);
	LoadPVM("SUPI_SUPI", &SUPI_SUPI_TEXLIST);
	LoadPVM("ANCIENT_LIGHT", &textlist_ancient_light);
	loadKLHudTex();

	// Load every Minimal texs because most of them will be loaded anyway
	for (int j = 0; j < LengthOfArray(MinimalPVMs); ++j)
	{
		LoadPVM(MinimalPVMs[j].Name, MinimalPVMs[j].TexList);
	}
}

void __cdecl LoadLevelObjTextures_r()
{
	LoadObjTextures();
	LoadLevelObjTextures_t.Original();
}

extern NJS_TEXLIST SuperKlonoaTexList;

void klonoaDisp(task* obj)
{
	auto data = obj->twp;

	NJS_TEXLIST* texture = &SuperKlonoaTexList;
	njSetTexture(texture);
	njPushMatrix(0);
	NJS_VECTOR pos = data->pos;
	njTranslateV(0, &pos);

	njScaleV(0, &KLScaleDiff);

	njRotateZ_(data->ang.z);
	njRotateX_(data->ang.x);
	njRotateY_(-0x8000 - LOWORD(data->ang.y));

	SetupChunkModelRender();

	DrawChunkObject(KlonoaMDL->getmodel());

	njPopMatrix(1u);

	ResetChunkModelRender();
	ClampGlobalColorThing_Thing();
	RestoreConstantAttr();
}

void KlonoaMdlTsk(task* obj)
{
	auto data = obj->twp;
	if (!data->mode)
	{
		obj->disp = klonoaDisp;
		data->mode++;
	}

	obj->disp(obj);
}

void LoadKlonoaTask()
{
	if (Controllers[0].PressedButtons & Buttons_Y)
	{
		auto tsk = CreateElementalTask(2, 1, KlonoaMdlTsk);
		tsk->twp->pos = playertwp[0]->pos;
	}
}


Sint32 __cdecl njSetTexture_Snowboard(NJS_TEXLIST* texlist)
{
	return njSetTexture(&KObjComTexlist);
}


void init_Objects()
{
	LoadLevelObjTextures_t.Hook(LoadLevelObjTextures_r);

	if (!obj)
		return;

	//load models
	dreamStoneMDL = LoadBasicModel("DreamStone");
	alarmClockMDL = LoadBasicModel("Alarm");
	boardMDL = LoadBasicModel("board");
	heartMDL = LoadBasicModel("heart");
	largeHeartMDL = LoadBasicModel("largeHeart");

	if (boardMDL)
	{
		SONIC_OBJECTS[71] = boardMDL->getmodel();
		WriteCall((void*)0x494182, njSetTexture_Snowboard);
	}

	//obj display hack
	Ring_Display_t.Hook(DreamStone_Display);
	CheckPoint_t.Hook(AlarmClock_Main);

	WriteCall((void*)0x44FA79, DrawDreamStone);
	WriteCall((void*)0x614D61, DrawDreamStone);
	WriteCall((void*)0x61F302, DrawDreamStoneClip);

	//obj sound hack
	//ring
	WriteCall((void*)0x4504DA, PlayDreamStoneSound);
	WriteCall((void*)0x450557, PlayDreamStoneSound);
	WriteCall((void*)0x45051F, PlayDreamStoneSound);
	WriteCall((void*)0x614DE1, PlayDreamStoneSound);
	WriteCall((void*)0x61F598, PlayDreamStoneSound);
	WriteCall((void*)0x44FE95, PlayDreamStoneSound);
}