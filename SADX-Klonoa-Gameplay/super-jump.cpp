#include "pch.h"


signed int KlonoaSJump_CheckInput(taskwk* data, playerwk* co2, klonoawk* klwk)
{
	if ((JumpButtons & Controllers[data->charIndex].PressedButtons) == 0 || klwk->enemyGrabPtr == nullptr)
	{
		return 0;
	}

	data->mode = act_super_jump;
	co2->mj.reqaction = anm_jump;

	char count = klwk->superJumpCount;	
	co2->spd.y = 5.0f + count;

	if (!count)
		PlayCustomSoundVolume(kl_SuperJump0, 0.7f);
	else
		PlayCustomSoundVolume(kl_SuperJump1, 0.7f);


	return 1;
}


signed int HoldEnemy_CheckJump(taskwk* data, playerwk* co2)
{
    if (JumpAllowed((EntityData1*)data) == 2)
    {
        StartPanelJump(data);
        data->mode = 40;
        Sonic_JumpPadAni((CharObj2*)co2);
        data->flag &= ~2u;
        co2->jumptimer = 0;
        PlaySound(17, 0, 0, 0);
        return 1;
    }

    if (!JumpAllowed((EntityData1*)data))
    {
        return 0;
    }

    data->mode = act_holdJump;
    co2->spd.y = co2->p.jmp_y_spd;
    co2->mj.reqaction = anm_holdJump;
    data->flag &= 0xFD;
    co2->jumptimer = 0;
    PlaySound(17, 0, 0, 0); 
    return 1;
}