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
void DrawKlonoaHP(char pnum);

extern NJS_SPRITE Heart_SPRITE;