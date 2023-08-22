#pragma once

enum HudE
{
	dream,
	heartEmpty,
	heart,
	bgHeart,
	bgLifeStone,
};

void DrawKlonoaHUD();
void loadKLHudTex();
void DrawKlonoaHP(char pnum, const Float x, const Float y, const Float sclX, const Float sclY);

extern NJS_SPRITE Heart_SPRITE;