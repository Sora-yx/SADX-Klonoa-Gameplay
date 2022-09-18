#include "pch.h"

static const char hoverTimer = 70;

signed int hover_CheckInput(taskwk* data, playerwk* co2, klonoawk* klwk)
{
	if ((JumpButtons & Controllers[data->charIndex].PressedButtons) == 0 || !allowKlonoaMoves || klwk->hoverUsed)
	{
		return 0;
	}

	data->mode = act_hover;
	co2->mj.reqaction = anm_hover;
	co2->spd.y = 0.0f;
	klwk->hoverTimer = hoverTimer;
	klwk->hoverUsed = true;
	PlayCustomSound(se_hover);
	return 1;
}


void hover_Physics(taskwk* data, motionwk2* data2, playerwk* co2)
{
	PResetAngle(data, data2, co2);
	if (co2->spd.y <= 0.0)
	{
		PGetAccelerationGlik(data, data2, co2);
		PGetSpeedGlik(data, data2, co2);
	}
	else
	{
		PGetAccelerationAir(data, data2, co2);
		PGetSpeed(data, data2, co2);
	}
	PSetPosition(data, data2, co2);
	PResetPosition(data, data2, co2);
}