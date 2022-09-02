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

signed int KlonoaSJump_CheckInput(taskwk* data, playerwk* co2, klonoawk* klwk)
{
	if ((JumpButtons & Controllers[data->charIndex].PressedButtons) == 0)
	{
		return 0;
	}

	data->mode = act_super_jump;
	co2->mj.reqaction = anm_hover;

	char count = klwk->superJumpCount;	
	co2->spd.y += 7.0f + count;

	if (!count)
		PlayCustomSound(kl_SuperJump0);
	else
		PlayCustomSound(kl_SuperJump1);

	return 1;
}

