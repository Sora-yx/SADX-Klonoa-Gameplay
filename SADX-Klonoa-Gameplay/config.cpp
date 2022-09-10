#include "pch.h"

bool allowSpinDash = false;
bool allowHomingAttack = false;
bool nerfPhysics = true;
bool hud = true;
bool obj = true;

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
	allowHomingAttack = config->getBool("gameplay", "allowHomingAttack", false);
	allowSpinDash = config->getBool("gameplay", "allowSpinDash", false);
	nerfPhysics = config->getBool("gameplay", "nerfPhysics", true);

	hud = config->getBool("visual", "hud", true);
	obj = config->getBool("visual", "obj", true);


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