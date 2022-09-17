#pragma once

enum HudE
{
	dream,
	heartEmpty,
	heart,
	bgHeart,
	bgLife,
};

void heartHud_Render();
void loadKLHud();
void DrawKlonoaHP();

extern NJS_SPRITE Heart_SPRITE;