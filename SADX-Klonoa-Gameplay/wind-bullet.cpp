#include "pch.h"

signed int KlonoaWBullet_CheckInput(taskwk* data, playerwk* co2)
{
	if ((AttackButtons & Controllers[data->charIndex].PressedButtons) == 0 || allowSpinDash)
	{
		return 0;
	}

	bool isOnGround = (data->flag & 3);
	data->mode = isOnGround ? act_windBullet : act_windBulletAir;
	co2->mj.reqaction = isOnGround ? anm_windBullet : anm_windBulletAir;
	co2->spd.y = 0.0f;
	PlayCustomSound(shot);
	return 1;
}
