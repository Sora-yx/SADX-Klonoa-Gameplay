#include "pch.h"
#include "hud.h"

static NJS_TEXNAME KHud[4] = { 0 };
static NJS_TEXLIST KHudTexlist = { arrayptrandlength(KHud) };
static const float ringPos = 52.0f;

const char diff = 9;
static NJS_TEXANIM Hud_TEXANIM[] = {
	{ 0x20, 0x27, 0, 0, 0, 0, 0x100, 0x100, dream, 0x20 },
	{ 0x32, 0x27, 0, 0, 0, 0, 0x100, 0x100, heartEmpty, 0x20 },
	{ 0x32, 0x27, 0, 0, 0, 0, 0x100, 0x100, heart, 0x20 },
	{ 230, 96, 0, 0, 0, 0, 0x100, 0x100, bgHeart, 0x20 },
	{ 0x52, 0x30, 0, 0, 0, 0, 0x100, 0x100, bgLifeStone, 0x20 },
};

NJS_SPRITE Heart_SPRITE = { {0}, 1.0f, 1.0f, {-1000}, &KHudTexlist, Hud_TEXANIM };
NJS_SPRITE Stone_SPRITE = { {0}, 1.0f, 1.0f, {-1000}, &KHudTexlist, Hud_TEXANIM };
static NJS_SPRITE Hud_BGSprite = { {0}, 1.0f, 1.0f, {0}, &KHudTexlist, Hud_TEXANIM };

bool isHudAllowed()
{
	if (!hud || !IsIngame() && !IsGamePaused() || MissedFrames || HideHud < 0 ||
		CurrentLevel >= LevelIDs_TwinkleCircuit || GameMode == GameModes_Adventure_Field)
	{
		return false;
	}

	return true;
}

void DrawDreamStoneHUD()
{
	float posX = HorizontalStretch * 520.0;
	Hud_BGSprite.p.x = posX;
	Hud_BGSprite.p.y = ringPos - 10.0f;
	Stone_SPRITE.p = Hud_BGSprite.p;

	SetMaterialAndSpriteColor_Float(1.0f, 0.086, 0.890, 0.129); //set green color to dream stone icon
	late_DrawSprite2D(&Stone_SPRITE, dream, 22045.498f, NJD_SPRITE_ANGLE | NJD_SPRITE_ALPHA | NJD_SPRITE_COLOR, LATE_LIG);

	SetMaterialAndSpriteColor_Float(1.0f, 0.129, 0.576, 0.709); //set blue color
	late_DrawSprite2D(&Hud_BGSprite, bgLifeStone, 22044.498f, NJD_SPRITE_ALPHA | NJD_SPRITE_COLOR, LATE_LIG);
	ResetMaterial();
}

void DrawHeartHUD()
{
	Heart_SPRITE.p.x = 10;
	Heart_SPRITE.p.y = ringPos + 50.0f;
	Hud_BGSprite.p.x = -5;
	Hud_BGSprite.p.y = ringPos + 13.0f;

	late_DrawSprite2D(&Hud_BGSprite, bgHeart, 22045.498f, NJD_SPRITE_ALPHA | NJD_SPRITE_COLOR, LATE_LIG);
}


void DrawKlonoaHUD()
{
	if (!isKlonoa(klonoaPnum) || !isHudAllowed())
		return;

	HelperFunctionsGlobal.PushScaleUI(uiscale::Align_Automatic, false, 1.0f, 1.0f);
	njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
	njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);

	SetMaterialAndSpriteColor_Float(1.0f, 1.0f, 1.0f, 1.0f);
	njSetTexture(&KHudTexlist);

	if (useHP)
	{
		DrawHeartHUD();
		DrawKlonoaHP();
	}

	DrawDreamStoneHUD();

	ResetMaterial();
	HelperFunctionsGlobal.PopScaleUI();
}



static void __declspec(naked) HudDisplayRingsASM()
{
	__asm
	{
		push esi 
		push bl 
		push eax 
		//call HudDisplayRings_r
		pop eax 
		pop bl 
		pop esi 
		retn
	}
}


void loadKLHudTex()
{
    if (!hud)
        return;

    LoadPVM("hud", &KHudTexlist);
}

void init_Hud()
{
   
}