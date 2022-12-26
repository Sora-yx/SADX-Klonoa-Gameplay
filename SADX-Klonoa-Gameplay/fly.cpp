#include "pch.h"

signed int Fly_CheckInput(taskwk* data, playerwk* co2)
{
	if ((JumpButtons & Controllers[data->charIndex].PressedButtons) == 0 || !allowKlonoaMoves || !isSuper(data->charIndex))
	{
		co2->equipment &= ~Upgrades_JetAnklet;
		return 0;
	}

	data->mode = act_flyStd;
	co2->mj.reqaction = 64;
	co2->spd.y += 2.0f;
	PlayCustomSoundVolume(kl_SuperJump1, 1);
	co2->equipment |= Upgrades_JetAnklet; //give the Jet Anklet upgrade to fly faster
	return 1;
}

//inspired from tails fly code

void SuperKlonoa_Fly(playerwk* co2, taskwk* data, motionwk2* data2)
{
	float v5 = 0.0f;
	int pID = (unsigned __int8)data->counter.b[0];
	float limVSpd = 0.0f;
	float dif = 0.0f;
	float posResult = 0.0f;
	zxsdwstr result = { 0 };

	result.pos = data->pos;

	CL_ColPolCFPolygon(&result, 20.0);

	if (result.lower.findflag == 1)
	{
		posResult = data->pos.y - result.lower.onpos;
		if (co2->spd.y < -3.0f && posResult >= 25.0f && posResult < 50.0)
		{
			v5 = -3.0f;
		LABEL_10:
			data2->acc.y = (v5 - co2->spd.y) * 0.25f + data2->acc.y;
			goto LABEL_11;
		}
		if (co2->spd.y < -0.30000001f && posResult > 0.0f && posResult < 25.0f)
		{
			v5 = -0.30000001f;
			goto LABEL_10;
		}
	}
LABEL_11:

	auto heldButton = Controllers[pID].HeldButtons;
	static const char max = 5.0f;

	if ((heldButton & JumpButtons) != 0)
	{
		if (co2->spd.y < max) {
			data2->acc.y += 0.15555f;
		}
	}
	else
	{
		if ((heldButton & AttackButtons) == 0)
		{
			co2->spd.y = 0.0f;
			data2->acc.y = 0.0f;
		}
		else
		{
			if (co2->spd.y < -max) {
				limVSpd = -co2->p.lim_v_spd;
				if (limVSpd >= co2->spd.y)
				{
					dif = limVSpd - 0.1f - co2->spd.y + data2->acc.y;
				}
				else
				{
					dif = data2->acc.y - 0.1f;
				}

				data2->acc.y = dif;
			}
		}
	}

	if (!GetAnalog((EntityData1*)data, 0, nullptr) && co2->spd.x < 4.0f)
	{
		co2->free.sw[3] = 0;
		co2->free.sw[4] = 0;
	}

	if (co2->spd.x >= 4.0f)
	{
		if (!co2->free.sw[3])
		{
			PlayCustomSoundVolume(kl_SuperJump0, 1.0f);
			co2->spd.x += 1.0f;
			co2->free.sw[3] = 1;
		}
	}

	if (co2->spd.x >= 7.0f)
	{
		if (!co2->free.sw[4])
		{
			co2->spd.x += 2.0f;
			PlayCustomSoundVolume(kl_bounce03, 1.0f);
			co2->free.sw[4] = 1;
		}
	}

	PGetAccelerationFly(data, data2, co2);
}

void Fly_ChangeMotion(Uint16 act, Uint16 anm, char pnum)
{
	playertwp[pnum]->mode = act;
	playerpwp[pnum]->mj.reqaction = anm;
}

void Fly_ManageMotion(taskwk* data, playerwk* co2)
{
	auto pnum = data->charIndex;

	if (!GetAnalog((EntityData1*)data, 0, nullptr)) {
		if (Controllers[pnum].HeldButtons & AttackButtons)
		{
			Fly_ChangeMotion(act_fly, anm_flyDown, pnum);
			return;
		}
		else if (Controllers[pnum].HeldButtons & JumpButtons)
		{
			Fly_ChangeMotion(act_fly, anm_flyUp, pnum);
			return;
		}

		if (co2->spd.x < 1.0f)
			Fly_ChangeMotion(act_flyStd, anm_flyStd, pnum);
	}
	else
	{
		Fly_ChangeMotion(act_fly, anm_flyMoving, pnum);
	}
}