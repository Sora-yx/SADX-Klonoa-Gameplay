#include "pch.h"

#define AddVoices(A) HelperFunctionsGlobal.ReplaceFile("system\\sounddata\\voice_jp\\wma\\" A ".wma", "system\\sounddata\\voice_us\\wma\\" A ".wav")

static FunctionHook<void, int> PlayVoice_t((intptr_t)PlayVoice);
static FunctionHook<void, unsigned __int8, float, NJS_VECTOR*> DoExplosionRockThing_t((intptr_t)0x446D90);
static FunctionHook<void, int> KillPlayer_t(KillHimP);
static FunctionHook<void, int> KillP1ByFallingDown_t(KillHimByFallingDownP); //gotta love symbol name

//we identify Sonic voice clip and replace them with some random Klonoa speech voice, all done in code to avoid duplicate voices files. :D

//The game often has many Sonic voices file in a row, so we can just identify a range to replace them (ex: the voices from 36 to 53 are all Sonic clips)

static const std::unordered_map<int16_t, int16_t> Sonicvoice_ids_map = {
	{ 36, 53 },
	{ 167, 168},
	{ 205, 215},
	{ 400, 403 },
	{ 410, 411},
	{ 414, 420},
	{ 446, 447 },
	{ 460, 462 },
	{ 468, 469},
	{ 472, 473},
	{ 481, 482},
	{ 492, 496},
	{ 507, 508},
	{ 511, 516},
	{ 520, 523},
	{ 526, 527},
	{ 553, 554},
	{ 567, 568},
	{ 579, 580},
	{ 582, 588},
	{ 593, 600},
	{ 603, 605},
	{ 625, 627},
	{ 675, 676},
	{ 710, 711},
	{ 762, 763},
	{ 873, 874},
	{ 877, 878},
	{ 939, 940},
	{ 945, 946},
	{ 1079, 1080},
	{ 1096, 1098},
	{ 1285, 1286},
	{ 1373, 1474},
	{ 1380, 1381},
	{ 1406, 1407},
	{ 1412, 1413},
	{ 1424, 1425},
	{ 1427, 1429},
	{ 1460, 1462},
	{ 1476, 1478},
	{ 1483, 1490},
	{ 1526, 1528},
	{ 1531, 1533},
	{ 1682, 1686 },
};

//other sonic voices that aren't followed by each other
int SonicVoiceIDs2[] = { 406, 422, 425,432, 435, 440, 442, 444, 457, 464, 466, 476, 484, 499, 502, 505, 518, 531,
534, 549, 556, 558, 561, 565, 573, 600, 608, 610, 613, 615, 629, 632, 642, 654, 664, 694, 696, 707, 717, 766, 789, 791, 827, 830,
832, 861, 864, 866, 871, 948, 950, 1074, 1082, 1093, 1148, 1150, 1282, 1288, 1290, 1385, 1409, 1416, 1419, 1452, 1457, 1464,
1469, 1492, 1494, 1513, 1515, 1520, 1540, 1546, 1548 };

void PlayRandomKlonoaVoice()
{
	int rng = kl_speech1 + rand() % 5;

	if (EV_MainThread_ptr)
	{
		int total = kl_EvVoiceEnd - kl_EvVoice01;
		rng = kl_EvVoice01 + rand() % total;
	}

	return PlayCustomSoundVolume(rng, 0.8f);
}

static void __cdecl PlayVoice_r(int a1)
{	
	std::unordered_map<int16_t, int16_t>::const_iterator it = Sonicvoice_ids_map.begin();

	while (it != Sonicvoice_ids_map.end())
	{
		// Accessing KEY from element pointed by it.
		int val1 = it->first;
		// Accessing VALUE from element pointed by it.
		int val2 = it->second;

		if (a1 >= val1 && a1 <= val2)
		{
			return PlayRandomKlonoaVoice();
		}
		// Increment the Iterator to point to next entry
		it++;
	}

	for (int i = 0; i < LengthOfArray(SonicVoiceIDs2); i++)
	{
		if (a1 == SonicVoiceIDs2[i])
		{
			return PlayRandomKlonoaVoice();
		}
	}

	return PlayVoice_t.Original(a1);
}

void PlayIdleVoice_r(taskwk* data)
{
	if (data && isKlonoa(data->charIndex))
	{
		return PlayRandomKlonoaVoice();
	}

	PlayIdleVoice(data);
}

int PlayDrownVoice(int ID, void* a2, int a3, void* a4)
{
	if (ID == 1506)
	{
		PlayCustomSound(kl_drown);
		return 1;
	}

	return PlaySound(ID, a2, a3, a4);
}

int PlayInvncibMagneticVoice(int ID, void* a2, int a3, void* a4)
{
	int player = getKlonoaPlayer();

	if (player >= 0)
	{
		PlayCustomSoundVolume(kl_bounce02, 0.7f);
	}

	return PlaySound(ID, a2, a3, a4);
}

int PlaySpeedBarrierVoice(int ID, void* a2, int a3, void* a4)
{
	int player = getKlonoaPlayer();

	if (player >= 0)
	{
		PlayCustomSoundVolume(kl_bounce01, 0.7f);
	}

	return PlaySound(ID, a2, a3, a4);
}

void PlayBlowPlayerVoice(unsigned __int8 playerID, float spd, NJS_VECTOR* a3)
{
	if (isKlonoa(playerID))
	{
		PlayCustomSoundVolume(kl_cannonshot, 0.7f);
	}

	DoExplosionRockThing_t.Original(playerID, spd, a3);
}

void KillPlayer_r(int pno)
{
	if (isKlonoa(pno))
	{
		PlayCustomSoundVolume(se_death, 0.5f);
	}

	KillPlayer_t.Original(pno);
}

void KillPlayerFall_r(int pno)
{
	if (isKlonoa(pno))
	{
		PlayCustomSoundVolume(se_death, 0.5f);
	}

	KillP1ByFallingDown_t.Original(pno);
}

char reboundCount = 0;
void PlayReboundVoice(unsigned __int8 playerID, float speedX, float speedY, float speedZ)
{
	if (isKlonoa(playerID))
	{
		switch (reboundCount)
		{
		case 0:
			PlayCustomSoundVolume(kl_bounce01, 1.0f);
			reboundCount++;
			break;
		case 1:
			PlayCustomSoundVolume(kl_bounce02, 1.0f);
			reboundCount++;
			break;
		case 2:
		default:
			PlayCustomSoundVolume(kl_bounce03, 1.0f);
			reboundCount = 0;
			break;
		}
	}

	return EnemyBounceThing(playerID, speedX, speedY, speedZ);
}

void init_Audio()
{
	WriteCall((void*)0x491701, PlayIdleVoice_r);
	WriteCall((void*)0x446BDA, PlayDrownVoice);
	WriteCall((void*)0x4D6DAF, PlayInvncibMagneticVoice);		
	WriteCall((void*)0x4D6E6F, PlayInvncibMagneticVoice);
	WriteCall((void*)0x4D6C39, PlaySpeedBarrierVoice);	
	WriteCall((void*)0x4D6DEF, PlaySpeedBarrierVoice);
	PlayVoice_t.Hook(PlayVoice_r);
	DoExplosionRockThing_t.Hook(PlayBlowPlayerVoice);
	KillPlayer_t.Hook(KillPlayer_r);
	KillP1ByFallingDown_t.Hook(KillPlayerFall_r);
	WriteCall((void*)0x4413CD, PlayReboundVoice);

	Sounds_Init();
}