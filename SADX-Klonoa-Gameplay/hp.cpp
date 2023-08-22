#include "pch.h"
#include "hud.h"

static const float ringPos = 52.0f;
char HpMax = 3;
char HP[PMax] = { HpMax };
extern bool HudPlus;

static FunctionHook <void, char> HurtCharacter_t(HurtCharacter);

void DrawKlonoaHP(char pnum, const Float x, const Float y, const Float sclX, const Float sclY)
{
	static const float yDiff = HudPlus ? 12.0f : 32.0f;
	Float heartDiff = HpMax == 6 ? 10.0f : 0.0f;
	const bool isMP = isMultiActive();

	if (isMP)
	{
		Heart_SPRITE.sx = Heart_SPRITE.sy = min(sclX, sclY);
		Heart_SPRITE.p.x = x + 10.0f * sclX;
		Heart_SPRITE.p.y = y + ringPos * sclY + 50.0f * sclY - yDiff * sclY + heartDiff * sclY;
	}
	else
	{
		Heart_SPRITE.p.x = 10.0f;
		Heart_SPRITE.p.y = ringPos + 50.0f - yDiff + heartDiff;
	}

	for (uint8_t i = 0; i < HpMax; i++) 
	{
		char hit = HpMax - HP[pnum];

		if (hit > 0 && i >= HP[pnum])
			late_DrawSprite2D(&Heart_SPRITE, heartEmpty, 22046.498f, NJD_SPRITE_ANGLE | NJD_SPRITE_ALPHA | NJD_SPRITE_COLOR, LATE_LIG);
		else
			late_DrawSprite2D(&Heart_SPRITE, heart, 22046.498f, NJD_SPRITE_ANGLE | NJD_SPRITE_ALPHA | NJD_SPRITE_COLOR, LATE_LIG);

		if (isMP)
		{
			Heart_SPRITE.p.x += 40.0f * min(sclX, sclY);
			Heart_SPRITE.p.y -= 6.0f * sclY;
		}
		else
		{
			Heart_SPRITE.p.x += 40.0f;
			Heart_SPRITE.p.y -= 6.0f;
		}
	}
}

void DamageKlonoa(char pnum, char dmg)
{
	if (HP[pnum] > dmg)
		HP[pnum] -= dmg;
	else
		HP[pnum] = 0;
}

void AddKlonoaHP(char pnum, char hp)
{
	if (HP[pnum] < HpMax)
		HP[pnum] += hp;
}

void ResetKlonoaHP(char pnum)
{
	HP[pnum] = HpMax;
}

void HurtCharacter_r(char a1)
{
	if (isKlonoa(a1))
	{
		if (HP[a1] <= 0)
		{
			HP[a1] = 0;
			KillHimP(a1);
			return;
		}

		return;
	}

	HurtCharacter_t.Original(a1);
}

void initKlonoaHP()
{
	if (useHP)
	{
		HurtCharacter_t.Hook(HurtCharacter_r);

		HpMax = hpMaxConfig;
	}
}