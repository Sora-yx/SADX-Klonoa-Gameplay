#include "pch.h"

ModelInfo* dreamStoneMDL = nullptr;
ModelInfo* alarmClockMDL = nullptr;

static NJS_TEXNAME KObjCommonTex[2] = { 0 };
static NJS_TEXLIST KObjComTexlist = { arrayptrandlength(KObjCommonTex) };

TaskHook Ring_Display_t((intptr_t)Ring_Display);
static FunctionHook<void> LoadLevelObjTextures_t((intptr_t)LoadLevelObjTextures);


void __cdecl DreamStone_Display(task* obj)
{
	int player = getKlonoaPlayer();

	if (player < 0 || !dreamStoneMDL)
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

		SetupWorldMatrix();
		Direct3D_SetChunkModelRenderState();
		njRotateY_((unsigned __int16)data->ang.y);
		njScaleV(0, &scl);
		if (QueueCharacterAnimations)
		{
			DrawQueueDepthBias = 32.0f;
			DrawChunkObject(dreamStoneMDL->getmodel());
			DrawQueueDepthBias = 0.0f;
		}
		else
		{
			DrawChunkObject(dreamStoneMDL->getmodel());
		}
		Direct3D_UnsetChunkModelRenderState();
		njPopMatrix(1u);

	}
}

void LoadObjTextures()
{
	LoadPVM("KObjCommon", &KObjComTexlist);
}

void __cdecl LoadLevelObjTextures_r()
{
	LoadObjTextures();
	LoadLevelObjTextures_t.Original();
}

void init_Objects()
{
	if (!obj)
		return;

	dreamStoneMDL = LoadChunkModel("DreamStone");
	alarmClockMDL = LoadChunkModel("Alarm");
	Ring_Display_t.Hook(DreamStone_Display);
	LoadLevelObjTextures_t.Hook(LoadLevelObjTextures_r);
}