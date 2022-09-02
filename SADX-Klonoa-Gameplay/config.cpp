#include "pch.h"

bool allowSpinDash = false;
bool allowHomingAttack = false;
bool nerfPhysics = true;

static UsercallFunc(signed int, Sonic_ChargeSpinDash_t, (playerwk* a1, taskwk* a2), (a1, a2), 0x496EE0, rEAX, rEAX, rEDI);

BOOL Sonic_ChargeSpinDash_r(playerwk* co2, taskwk* data)
{
	if (isKlonoa(data->charIndex))
	{
		return false;
	}

	return Sonic_ChargeSpinDash_t.Original(co2, data);
}

void ReadConfig(const char* path, const HelperFunctions& helperFunctions) {
	//Ini file Configuration
	const IniFile* config = new IniFile(std::string(path) + "\\config.ini");
	allowHomingAttack = config->getBool("move", "allowHomingAttack", false);
	allowSpinDash = config->getBool("move", "allowSpinDash", false);
	nerfPhysics = config->getBool("physics", "nerfPhysics", true);
	delete config;

	if (nerfPhysics)
	{
		PhysicsArray[Characters_Sonic] = PhysicsArray[Characters_Tails];
	}

	if (!allowSpinDash)
	{
		Sonic_ChargeSpinDash_t.Hook(Sonic_ChargeSpinDash_r);
	}

}