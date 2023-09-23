#include "pch.h"
#include "FunctionHook.h"
#include "ModelInfo.h"
#include "AnimationFile.h"
#include "mod.h"

extern NJS_TEXLIST KlonoaTexList;

void TornadoExec_r(task* tp);
void TornadoDisp_r(task* tp);
void Tornado2TransfoDisp_r(task* tp);
void TornadoEventDisp_r(task* tp);
void Tornado2EventDisp_r(task* tp);
void TornadoShotDisp_r(task* tp);
static bool isTransfoDone = false;

TaskHook TornadoDisp_t((intptr_t)Tornado_Display, TornadoDisp_r);
TaskHook Tornado2TransfoDisp_t((intptr_t)0x62AB90, Tornado2TransfoDisp_r);
TaskHook TornadoExec_t((intptr_t)Tornado_Main, TornadoExec_r);
TaskHook TornadoEventDisp_t(0x6F8800, TornadoEventDisp_r);
TaskHook Tornado2EventDisp_t(0x4188D0, Tornado2EventDisp_r);
TaskHook TornadoShotDisp_t(0x62A9A0, TornadoShotDisp_r);

static NJS_MATRIX TornadoMatrixPtr;

static const NJS_POINT3 offsetIdle = { 0.0f, -4.5f, -1.2f };
static const NJS_POINT3 offsetShot = { 0.0f, -4.5f, -1.2f };

struct nodeModel
{
	NJS_OBJECT* model;
	uint8_t node;
};

//array that store tornado models and the node we will use for convenience
nodeModel TornadoMdlsNode[]
{
	{ &Tornado1_Object, 27},
	{ &Tornado1_EventObj, 27},
	{ &Tornado1_EventShot, 9},
	{ &Tornado2Before_Object, 26},
	{ &Tornado2Transfo_Object, 97},
	{ &Tornado2Change_Object, 28},
	{ &Tornado2_EventObj, 28},
	{ &Tornado2TakeOff_EventObj, 26},
};

void HideSonicTornado()
{
	for (uint8_t j = 0; j < LengthOfArray(TornadoMdlsNode); j++)
	{
		if (TornadoMdlsNode[j].model == nullptr)
			continue;

		auto obj = TornadoMdlsNode[j].model->getnode(TornadoMdlsNode[j].node + 1);
		
		for (uint8_t i = 0; i < 3; i++)
		{
			if (obj && obj->scl[i] != 0.0f) //crash otherwise, because hu, idk, lol
				obj->scl[i] = 0.0f;
		}
	}
}

enum
{
	stand,
	hover,
	idle,
};



static Float KlonoaAnmSpd[3] = { 0.5f, 0.4f, 1.0f }; //Klonoa Animations speed variables
static float MotionFrames = 0.0f;

void TornadoCallBack(NJS_OBJECT* obj)
{
	//browse the tornado model array above
	for (uint8_t i = 0; i < LengthOfArray(TornadoMdlsNode); i++)
	{
		if (!TornadoMdlsNode[i].model)
			continue;

		uint8_t curNode = TornadoMdlsNode[i].node;

		if (obj == TornadoMdlsNode[i].model->getnode(curNode)) //if the game is about to draw Tails in the tornado
		{
			njSetMatrix(TornadoMatrixPtr, _nj_current_matrix_ptr_); //copy matrix position for use later
			break;
		}
	}

	if (backupCallback)
		backupCallback(obj);
}


static void DrawKlonoaTornado(NJS_POINT3 Offset, const uint8_t index = stand)
{
	if (MissedFrames)
		return;

	auto& curFrame = MotionFrames;
	auto animList = GetKlonoaAnimList();
	auto anim = animList[anm_board].Animation;

	if (!IsGamePaused())
	{
		curFrame = fmodf(curFrame + KlonoaAnmSpd[index], anim->motion->nbFrame); //current frame of Klonoa animation (Also Helps us increase animation speed)
	}

	njSetTexture(&KlonoaTexList);

	NJS_VECTOR posOffset = Offset; // x, y, z || in/out of the page, vertical, Horizontal (all relative to camera)

	njPushMatrix(TornadoMatrixPtr); //push the tornado matrix (which did copy the tornado position earlier)
	njTranslateV(0, &posOffset); //since the tornado doesn't have a node for wing, we adjust the position to reach the wing.
	njScaleV(0, &KLScaleDiff); //scale order fix lighting 
	njRotateZ(0, -0x800);
	dsScaleLight(0.2f);

	late_DrawMotionClipMesh(anim->object, anim->motion, curFrame, LATE_LIG, 0.0f);

	njPopMatrix(1); //pop matrix

	dsReScaleLight();
}

//Hack tornado display to make it draws Klonoa
void TornadoDisp_r(task* tp)
{
	SetNodeCallBack(TornadoCallBack);
	TornadoDisp_t.Original(tp);


	DrawKlonoaTornado(offsetIdle);

	RestoreOriginalCallBack();
}

static void DrawKlonoaDuringTransform(task* tp)
{
	isTransfoDone = true;

	if (MissedFrames)
		return;

	uint8_t index = hover;
	auto animList = GetKlonoaAnimList();
	auto anim = animList[anm_board].Animation;
	auto& curFrame = MotionFrames;
	auto tornadoAnimTransoFrame = tp->twp->counter.f;
	NJS_POINT3 pos = offsetIdle;

	if (tornadoAnimTransoFrame >= 220)
	{
		pos.y += 2.0f;
		anim = animList[anm_hover].Animation;
	}
	
	if (tornadoAnimTransoFrame >= 576)
	{
		pos.y -= 1.5f;
		index = idle;
		anim = animList[4].Animation;
	}

	if (!IsGamePaused())
	{
		curFrame = fmodf(curFrame + KlonoaAnmSpd[index], anim->motion->nbFrame); //current frame of Klonoa animation (Also Helps us increase animation speed)
	}

	njSetTexture(&KlonoaTexList);

	njPushMatrix(TornadoMatrixPtr); //push the tornado matrix (which did copy the tornado position earlier)
	njTranslateV(0, &pos); 
	njScaleV(0, &KLScaleDiff); //scale order fix lighting 
	njRotateZ(0, -0x800);
	dsScaleLight(0.2f);

	late_DrawMotionClipMesh(anim->object, anim->motion, curFrame, LATE_LIG, 0.0f);
	njPopMatrix(1); //pop matrix

	dsReScaleLight();
	___dsSetPalette(0);
}

//Hack tornado transfo display to make it draws Klnoa
void Tornado2TransfoDisp_r(task* tp)
{
	SetNodeCallBack(TornadoCallBack);
	Tornado2TransfoDisp_t.Original(tp);
	DrawKlonoaDuringTransform(tp);
	RestoreOriginalCallBack();
}


void TornadoEventDisp_r(task* tp)
{
	SetNodeCallBack(TornadoCallBack);
	TornadoEventDisp_t.Original(tp);

	HideSonicTornado();
	auto twp = tp->twp;

	if (twp && dsCheckViewV(&twp->pos, 50.0f))
		DrawKlonoaTornado(offsetIdle);

	RestoreOriginalCallBack();
}

void Tornado2EventDisp_r(task* tp)
{
	bool isTR2 = (TR2TakeOffEVPtr && TR2TakeOffEVPtr == tp || TR2TailsRM && TR2TailsRM == tp || TR2SonkRM && TR2SonkRM == tp);
	bool isTRB2 = (TRB2 && TRB2 == tp || TRB2S && TRB2S == tp);
	auto twp = tp->twp;

	if (isTR2 || isTRB2)
	{
		HideSonicTornado();
		SetNodeCallBack(TornadoCallBack);
	}

	Tornado2EventDisp_t.Original(tp);

	if (isTR2 || isTRB2)
	{
		if (twp && dsCheckViewV(&twp->pos, 50.0f))
		{
			DrawKlonoaTornado(offsetIdle);
		}

		RestoreOriginalCallBack();
	}
}

void TornadoShotDisp_r(task* tp)
{
	SetNodeCallBack(TornadoCallBack);
	TornadoShotDisp_t.Original(tp);

	auto twp = tp->twp;
	if (twp && dsCheckViewV(&twp->pos, 150.0f))
		DrawKlonoaTornado(offsetShot);

	RestoreOriginalCallBack();
}

//Load Klonoa textures when the Tornado is init
void TornadoExec_r(task* tp)
{
	auto twp = tp->twp;

	if (tp->twp->mode == 0)
	{
		isTransfoDone = false;
		HideSonicTornado();
		LoadPVM("KlonoaTex", &KlonoaTexList);
	}

	TornadoExec_t.Original(tp); //run original Tornado code
}
