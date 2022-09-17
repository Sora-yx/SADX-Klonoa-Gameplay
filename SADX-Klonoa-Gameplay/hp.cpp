#include "pch.h"
#include "hud.h"

const char HpMax = 3;
char HP = 3;

static FunctionHook <void, char> HurtCharacter_t(HurtCharacter);

void DrawKlonoaHP()
{
	for (char i = 0; i < HpMax; i++) {

		char hit = HpMax - HP;

		if (hit > 0 && i >= HP)
			late_DrawSprite2D(&Heart_SPRITE, heartEmpty, 22046.498f, NJD_SPRITE_ANGLE | NJD_SPRITE_ALPHA | NJD_SPRITE_COLOR, LATE_LIG);
		else
			late_DrawSprite2D(&Heart_SPRITE, heart, 22046.498f, NJD_SPRITE_ANGLE | NJD_SPRITE_ALPHA | NJD_SPRITE_COLOR, LATE_LIG);

		Heart_SPRITE.p.x += 40.0f;
		Heart_SPRITE.p.y -= 6.0f;
	}
}

void DamageKlonoa(char dmg)
{
	if (HP > dmg)
		HP -= dmg;
	else
		HP = 0;
}

void AddKlonoaHP(char hp)
{
	if (HP < HpMax)
		HP += hp;
}

void ResetKlonoaHP()
{
	HP = HpMax;
}

void HurtCharacter_r(char a1)
{
	if (isKlonoa(a1))
	{
		if (HP <= 0)
		{
			HP = 0;
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
		HurtCharacter_t.Hook(HurtCharacter_r);
}