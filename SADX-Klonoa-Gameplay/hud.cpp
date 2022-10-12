#include "pch.h"
#include "hud.h"

static NJS_TEXNAME KHud[4] = { 0 };
static NJS_TEXLIST KHudTexlist = { arrayptrandlength(KHud) };
static const float ringPos = 52.0f;
static bool HudPlus = false;

static NJS_TEXANIM Hud_TEXANIM[] = {
	{ 0x20, 0x27, 0, 0, 0, 0, 0x100, 0x100, dream, 0x20 },
	{ 0x32, 0x27, 0, 0, 0, 0, 0x100, 0x100, heartEmpty, 0x20 },
	{ 0x32, 0x27, 0, 0, 0, 0, 0x100, 0x100, heart, 0x20 },
	{ 230, 96, 0, 0, 0, 0, 0x100, 0x100, bgHeart, 0x20 },
	{ 0x68, 0x3B, 0, 0, 0, 0, 0x100, 0x100, bgLifeStone, 0x20 },
};

NJS_SPRITE Heart_SPRITE = { {0}, 1.0f, 1.0f, {-1000}, &KHudTexlist, Hud_TEXANIM };
NJS_SPRITE Stone_SPRITE = { {0}, 1.0f, 1.0f, {-1000}, &KHudTexlist, Hud_TEXANIM };
static NJS_SPRITE Hud_BGSprite = { {0}, 1.0f, 1.0f, {0}, &KHudTexlist, Hud_TEXANIM };

bool isHudAllowed()
{
	if (!hud || !IsIngame() && !IsGamePaused() || MissedFrames || HideHud < 0 ||
		CurrentLevel >= LevelIDs_TwinkleCircuit || GameMode == GameModes_Adventure_Field && !CharacterBossActive)
	{
		return false;
	}

	return true;
}

static Uint32 StoneTimer = 0;

void DrawDreamStoneCounter(float posY, bool boss) {

	static const auto backup = Hud_RingTimeLife;
	Hud_RingTimeLife.ang = -1000;

	Hud_RingTimeLife.p.x = HorizontalStretch * 583.0f;
	Hud_RingTimeLife.p.y = ringPos - 12.0f + posY;
	Hud_RingTimeLife.p.y += boss ? 39.5f : 0.0f;

	int16_t rings = Rings;

	//Draw the ring counter if rings > 0, or draw blinking red zeros
	if (rings == 0) {
		if (!IsGamePaused())
		{
			StoneTimer += 1024;
		}

		float color = 1.0f - powf(njSin(StoneTimer), 2); //makes the color go from 0 to 1 to 0 etc

		SetMaterialAndSpriteColor_Float(1, 1, color, color);

		for (uint8_t i = 0; i < 3; ++i) {
			njDrawSprite2D_ForcePriority(&Hud_RingTimeLife, dream, -1.501f, NJD_SPRITE_ALPHA | NJD_SPRITE_COLOR | NJD_SPRITE_ANGLE);
			Hud_RingTimeLife.p.x -= 16;
			Hud_RingTimeLife.p.y += 2.0f;
		}
	}
	else {
		if (rings > 999) {
			rings = 999;
		}

		for (uint8_t i = 0; i < 3; ++i) {
			if (rings <= 0) {
				njDrawSprite2D_ForcePriority(&Hud_RingTimeLife, 0, -1.501f, NJD_SPRITE_ALPHA | NJD_SPRITE_ANGLE);
			}
			else {
				njDrawSprite2D_ForcePriority(&Hud_RingTimeLife, rings % 10, -1.501f, NJD_SPRITE_ALPHA | NJD_SPRITE_ANGLE);
				rings /= 10;

			}

			Hud_RingTimeLife.p.x -= 16;
			Hud_RingTimeLife.p.y += 2.0f;
		}
	}

	Hud_RingTimeLife = backup;
}

void DrawDreamStoneHUD(bool bosslevel)
{
	//isBossLevel() ? 40.0f : 0.0f;
	static const float posX = HorizontalStretch * 520.0f;
	static const float posY = HudPlus ? 0.0f : 20.0f;
	static const float bossPosY = bosslevel ? 39.0f : 0.0f;
	Hud_BGSprite.p.x = posX;
	Hud_BGSprite.p.y = ringPos - posY + bossPosY;

	Stone_SPRITE.p = Hud_BGSprite.p;

	SetMaterialAndSpriteColor_Float(1.0f, 0.086f, 0.890f, 0.129f); //set green color to dream stone icon
	late_DrawSprite2D(&Stone_SPRITE, dream, 22045.498f, NJD_SPRITE_ANGLE | NJD_SPRITE_ALPHA | NJD_SPRITE_COLOR, LATE_LIG);

	Hud_BGSprite.p.y -= 10.0f;
	SetMaterialAndSpriteColor_Float(1.0f, 0.129f, 0.576f, 0.709f); //set blue color
	late_DrawSprite2D(&Hud_BGSprite, bgLifeStone, 22044.498f, NJD_SPRITE_ALPHA | NJD_SPRITE_COLOR, LATE_LIG);
	ResetMaterial();
}

void DrawHeartHUD()
{
	static const float y = HudPlus ? 0.0f : 12.0f;
	Heart_SPRITE.p.x = 10;
	Heart_SPRITE.p.y = ringPos + 50.0f -y;
	Hud_BGSprite.p.x = -5;
	Hud_BGSprite.p.y = ringPos + 13.0f -y;

	late_DrawSprite2D(&Hud_BGSprite, bgHeart, 22045.498f, NJD_SPRITE_ALPHA | NJD_SPRITE_COLOR, LATE_LIG);
}

void DrawKlonoaHUD()
{
	if (getKlonoaPlayer() < 0 || !isHudAllowed())
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

	static bool boss = isBossLevel();
	DrawDreamStoneHUD(boss);
	static float posY = HudPlus ? 20.0f : 0.0f;
	DrawDreamStoneCounter(posY, boss);

	njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
	njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
	ResetMaterial();
	HelperFunctionsGlobal.PopScaleUI();
}


void loadKLHudTex()
{
	if (!hud)
		return;

	LoadPVM("hud", &KHudTexlist);
}

void init_Hud()
{
	if (hud) {
		HMODULE hud = GetModuleHandle(L"sadx-hud-plus");
		HudPlus = hud != NULL;


	}
}