#include "pch.h"

void SuperJump_Physics(taskwk* data, motionwk2* data2, playerwk* co2)
{
	PResetAngle(data, data2, co2);
	if (PResetAccelerationAir(data, data2, co2))
	{
		PGetSpeed(data, data2, co2);
		PSetPosition(data, data2, co2);
		PResetPosition(data, data2, co2);
	}

	PGetAccelerationAir(data, data2, co2);
}

signed int KlonoaSJump_CheckInput(taskwk* data, playerwk* co2)
{
	if ((JumpButtons & Controllers[data->charIndex].PressedButtons) == 0)
	{
		return 0;
	}

	data->mode = act_super_jump;
	co2->mj.reqaction = anm_hover;
	co2->spd.y += 7.0f;
	co2->free.sw[2] = 90;
	PlayCustomSound(kl_SuperJump0);
	return 1;
}


signed int KlonoaSJump2_CheckInput(taskwk* data, playerwk* co2)
{
	if ((JumpButtons & Controllers[data->charIndex].PressedButtons) == 0)
	{
		return 0;
	}

	data->mode = act_super_jump2;
	co2->mj.reqaction = anm_hover;
	co2->spd.y += 8.0f;
	co2->free.sw[2] = 90;
	PlayCustomSound(kl_SuperJump1);
	return 1;
}

