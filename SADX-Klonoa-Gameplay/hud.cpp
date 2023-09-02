#include "pch.h"
#include "hud.h"
#include "multiapi.h"

#define ReplaceTex 	HelperFunctionsGlobal.ReplaceTexture
#define replaceFile HelperFunctionsGlobal.ReplaceFile

static NJS_TEXNAME KHud[4] = { 0 };
static NJS_TEXLIST KHudTexlist = { arrayptrandlength(KHud) };
static const float ringPos = 52.0f;
bool HudPlus = false;

static UsercallFuncVoid(HudDisplayRings_t, (signed int a1, unsigned __int8 a2, NJS_SPRITE* a3), (a1, a2, a3), 0x425960, rEAX, rBL, rESI);

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
		CurrentLevel >= LevelIDs_TwinkleCircuit || GameMode == GameModes_Adventure_Field && !CharacterBossActive || (*(uint8_t*)0x425F90 == 0xc3)) //is code hide hud
	{
		return false;
	}
	return true;
}

static Uint32 StoneTimer = 0;

void DrawDreamStoneCounter(float posY, bool boss)
{
	if (isMultiActive())
		return;

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

void DrawBGHeartHUD(uint32_t screen, const Float resX, const Float resY, const Float sclX, const Float sclY)
{
	static const float yDiff = HudPlus ? 12.0f : 32.0f;
	static float yExtra = 13.0f;
	const float scaling = min(sclX, sclY);

	Hud_BGSprite.p.x = resX + -5 * scaling;
	Hud_BGSprite.p.y = resY + ringPos * sclY + yExtra * sclY - yDiff * scaling;

	auto saveScl = Hud_BGSprite.sx;
	auto saveAng = Hud_BGSprite.ang;

	Hud_BGSprite.sx = Hud_BGSprite.sy = scaling;

	//adjust hud size and pos depending on the number of the difficulty
	if (hpMaxConfig == 1)
	{
		Hud_BGSprite.sx /= 3.0f;
		Hud_BGSprite.p.x += 15.0f * sclX;
	}

	if (hpMaxConfig == 6)
	{
		Hud_BGSprite.p.y += 15.0f * sclY;
		Hud_BGSprite.sx *= 2.0f;
		Hud_BGSprite.p.x -= 15.0f * sclX;
		Hud_BGSprite.ang = -1000;
	}

	late_DrawSprite2D(&Hud_BGSprite, bgHeart, 22045.498f, NJD_SPRITE_ALPHA | NJD_SPRITE_COLOR | NJD_SPRITE_ANGLE, LATE_LIG);

	Hud_BGSprite.sx = saveScl;
	Hud_BGSprite.ang = saveAng;
}

void DrawBGHeartHUD()
{
	static const float y = HudPlus ? 12.0f : 32.0f;
	Hud_BGSprite.p.x = -5;
	Hud_BGSprite.p.y = ringPos + 13.0f - y;

	auto saveScl = Hud_BGSprite.sx;
	auto saveAng = Hud_BGSprite.ang;

	//adjust hud size and pos depending on the number of the difficulty
	if (hpMaxConfig == 1)
	{
		Hud_BGSprite.sx /= 3.0f;
		Hud_BGSprite.p.x += 15.0f;
	}

	if (hpMaxConfig == 6)
	{
		Hud_BGSprite.p.y += 15.0f;
		Hud_BGSprite.sx *= 2.0f;
		Hud_BGSprite.p.x -= 15.0f;
		Hud_BGSprite.ang = -1000;
	}

	late_DrawSprite2D(&Hud_BGSprite, bgHeart, 22045.498f, NJD_SPRITE_ALPHA | NJD_SPRITE_COLOR | NJD_SPRITE_ANGLE, LATE_LIG);

	Hud_BGSprite.sx = saveScl;
	Hud_BGSprite.ang = saveAng;
}

void DrawKlonoaHUD(uint32_t screen, const Float x, const Float y, const Float sclX, const Float sclY)
{
	const bool isMP = isMultiActive();

	if (!isMP)
		HelperFunctionsGlobal.PushScaleUI(uiscale::Align_Automatic, false, 1.0f, 1.0f);

	njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
	njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);

	SetMaterialAndSpriteColor_Float(1.0f, 1.0f, 1.0f, 1.0f);
	njSetTexture(&KHudTexlist);

	if (useHP)
	{
		if (isMP)
		{
			DrawBGHeartHUD(screen, x, y, sclX, sclY);
		}
		else
		{
			DrawBGHeartHUD();
		}

		DrawKlonoaHP(screen, x, y, sclX, sclY);
	}

	if (!isMP)
	{
		static bool boss = isBossLevel();
		DrawDreamStoneHUD(boss);
		static float posY = HudPlus ? 20.0f : 0.0f;
		DrawDreamStoneCounter(posY, boss);
	}

	njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
	njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
	ResetMaterial();

	if (!isMP)
		HelperFunctionsGlobal.PopScaleUI();
}

void DrawKlonoaHUD()
{
	if (!isHudAllowed())
		return;

	const bool isMP = isMultiActive();

	if (isMP && !IsGamePaused())
	{
		int viewport_backup = viewport_get_num();
		viewport_set_num(-1);

		for (int i = 0; i < 4; ++i)
		{
			if (playertwp[i] && isKlonoa(i) && viewport_is_enabled(i))
			{
				float x, y, w, h;

				if (!viewport_get_info(i, &x, &y, &w, &h))
				{
					continue;
				}

				float screenX = HorizontalResolution * x; // X position of the splitted screen
				float screenY = VerticalResolution * y; // Y position of the splitted screen
				float scaleX = HorizontalStretch * w + 0.5f; // X stretch of the splitted screen
				float scaleY = VerticalStretch * h + 0.5f; // Y stretch of the splitted screen

				DrawKlonoaHUD(i, screenX, screenY, scaleX, scaleY);
			}
		}

		viewport_set_num(viewport_backup);
	}
	else
	{
		DrawKlonoaHUD(0, 0, 0, 0 ,0);
	}

}

void loadKLHudTex()
{
	if (!hud)
		return;

	LoadPVM("hud", &KHudTexlist);
}

//hide ring (also used for compatibility hud plus)
void HudDisplayRings_r(signed int ringCount, unsigned __int8 digits, NJS_SPRITE* hud)
{
	if (ringCount == Rings && digits == 3 && hud == &Hud_RingTimeLife)
	{
		return;
	}

	HudDisplayRings_t.Original(ringCount, digits, hud);
}

void init_Hud()
{
	std::string texHudPath = modpath + "\\replacetex\\OBJ_REGULAR2\\";
	std::string path0 = texHudPath + "item_1up.png";
	std::string path1 = texHudPath + "item_ring10.png";
	std::string path2 = texHudPath + "item_ring5.png";
	std::string path3 = texHudPath + "item_ringq.png";
	std::string path4 = texHudPath + "item_ringnum.png";

	if (hud)
	{

		ReplaceTex("OBJ_REGULAR", "item_1up", path0.c_str(), 4031, 64, 64);
		ReplaceTex("OBJ_REGULAR", "item_ring10", path1.c_str(), 4032, 64, 64);
		ReplaceTex("OBJ_REGULAR", "item_ring5", path2.c_str(), 4033, 64, 64);
		ReplaceTex("OBJ_REGULAR", "item_ringq", path3.c_str(), 4034, 64, 64);
		ReplaceTex("OBJ_REGULAR", "item_ringnum", path4.c_str(), 3250000, 64, 64);

		replaceFile("system\\CON_REGULAR.PVM", "system\\CON_REGULAR_Klonoa.pvmx");
		replaceFile("system\\CON_REGULAR_E.PVM", "system\\CON_REGULAR_Klonoa_E.pvmx");

		HMODULE hudP = GetModuleHandle(L"sadx-hud-plus");
		HudPlus = hudP != NULL;

		HudDisplayRings_t.Hook(HudDisplayRings_r);
	}
}