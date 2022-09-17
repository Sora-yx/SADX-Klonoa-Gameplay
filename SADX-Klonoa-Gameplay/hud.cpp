#include "pch.h"
#include "hud.h"

static NJS_TEXNAME KHud[4] = { 0 };
static NJS_TEXLIST KHudTexlist = { arrayptrandlength(KHud) };
static const float ringPos = 52.0f;

const char diff = 9;
static NJS_TEXANIM Hud_TEXANIM[] = {
	{ 0x100, 0x30, 0, 0, 0, 0, 0x100, 0x100, dream, 0x20 },
	{ 0x32, 0x27, 0, 0, 0, 0, 0x100, 0x100, heartEmpty, 0x20 },
	{ 0x32, 0x27, 0, 0, 0, 0, 0x100, 0x100, heart, 0x20 },
	{ 230, 96, 0, 0, 0, 0, 0x100, 0x100, bgHeart, 0x20 },
	{ 0x100, 0x50, 0, 0, 0, 0, 0x100, 0x100, bgLife, 0x20 },
};

NJS_SPRITE Heart_SPRITE = { {0}, 1.0f, 1.0f, {-1000}, &KHudTexlist, Hud_TEXANIM };
static NJS_SPRITE Hud_BGSprite = { {0}, 1.0f, 1.0f, {0}, &KHudTexlist, Hud_TEXANIM };

void heartHud_Render()
{
	if (!isKlonoa(klonoaPnum) || !hud || !useHP || !IsIngame() && !IsGamePaused() || MissedFrames || HideHud < 0 || GameMode == GameModes_Adventure_Field)
		return;

	HelperFunctionsGlobal.PushScaleUI(uiscale::Align_Automatic, false, 1.0f, 1.0f);
	njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
	njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);

	SetMaterialAndSpriteColor_Float(1.0f, 1.0f, 1.0f, 1.0f);
	njSetTexture(&KHudTexlist);
	Heart_SPRITE.p.x = 10;
	Heart_SPRITE.p.y = ringPos + 50.0f;
	Hud_BGSprite.p.x = -5;
	Hud_BGSprite.p.y = ringPos + 13.0f;

	late_DrawSprite2D(&Hud_BGSprite, bgHeart, 22045.498f, NJD_SPRITE_ALPHA | NJD_SPRITE_COLOR, LATE_LIG);

	DrawKlonoaHP();

	ResetMaterial();
	HelperFunctionsGlobal.PopScaleUI();
}


void loadKLHud()
{
	if (!hud || !useHP)
		return;

	LoadPVM("hud", &KHudTexlist);
}