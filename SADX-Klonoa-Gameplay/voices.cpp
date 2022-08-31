#include "pch.h"

static FunctionHook<void, int> PlayVoice_t((intptr_t)PlayVoice);

//we identify Sonic voice clip and replace them with some random Klonoa speech voice, all done in code to avoid duplicate voices files. :D

//The game often has many Sonic voices file in a row, so we can just identify a range to replace them (ex: the voices from 36 to 53 are all Sonic clips)

static const std::unordered_map<int16_t, int16_t> Sonicvoice_ids_map = {
	{ 36, 53 },
	{ 167, 168},
	{ 205, 215},
	{ 400, 403 },
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

void PlayRandomKlonoaVoice()
{
	return PlayVoice(kl_speech1 + rand() % 5);
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

void init_Audio()
{
	WriteCall((void*)0x491701, PlayIdleVoice_r);
	PlayVoice_t.Hook(PlayVoice_r);
}