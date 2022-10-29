#include "pch.h"
#include "hud.h"

char HpMax = 3;
char HP[PMax] = { HpMax };

static FunctionHook <void, char> HurtCharacter_t(HurtCharacter);

void DrawKlonoaHP(char pnum)
{
	for (char i = 0; i < HpMax; i++) {
		char hit = HpMax - HP[pnum];

		if (hit > 0 && i >= HP[pnum])
			late_DrawSprite2D(&Heart_SPRITE, heartEmpty, 22046.498f, NJD_SPRITE_ANGLE | NJD_SPRITE_ALPHA | NJD_SPRITE_COLOR, LATE_LIG);
		else
			late_DrawSprite2D(&Heart_SPRITE, heart, 22046.498f, NJD_SPRITE_ANGLE | NJD_SPRITE_ALPHA | NJD_SPRITE_COLOR, LATE_LIG);

		Heart_SPRITE.p.x += 40.0f;
		Heart_SPRITE.p.y -= 6.0f;
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

		if (difficulty == easy)
		{
			HpMax = 6;
		}

		if (difficulty == hard)
		{
			HpMax = 1;
		}
	}
}