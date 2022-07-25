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