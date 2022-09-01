#include "pch.h"

signed int KlonoaWBullet_CheckInput(taskwk* data, playerwk* co2)
{
	if ((AttackButtons & Controllers[data->charIndex].PressedButtons) == 0)
	{
		return 0;
	}

	bool isOnGround = (data->flag & 3);
	data->mode = isOnGround ? act_windBullet : act_windBulletAir;
	co2->mj.reqaction = 147;
	co2->spd.y = 0.0f;
	PlayCustomSound(shot);
	return 1;
}
