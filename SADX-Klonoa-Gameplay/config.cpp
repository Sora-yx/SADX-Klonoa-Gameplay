#include "pch.h"

bool allowSpinDash = false;
bool allowHomingAttack = false;
bool nerfPhysics = true;
bool hud = true;
bool obj = true;
bool useHP = true;

static UsercallFunc(signed int, Sonic_ChargeSpinDash_t, (playerwk* a1, taskwk* a2), (a1, a2), 0x496EE0, rEAX, rEAX, rEDI);


BOOL Sonic_ChargeSpinDash_r(playerwk* co2, taskwk* data)
{
	if (isKlonoa(data->charIndex) && !EV_MainThread_ptr)
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
	useHP = config->getBool("gameplay", "useHP", true);

	hud = config->getBool("visual", "hud", true);
	obj = config->getBool("visual", "obj", true);

	delete config;

	if (nerfPhysics)
	{
		PhysicsArray_[Characters_Sonic] = PhysicsArray_[Characters_Tails];
		PhysicsArray_[Characters_Sonic].pos_error = 2.0f;
		PhysicsArray_[Characters_Sonic].height = 10.0f;
		PhysicsArray_[Characters_Sonic].eyes_height = 7.0f;
		PhysicsArray_[Characters_Sonic].center_height = 5.4f;
	}

	if (!allowSpinDash)
	{
		Sonic_ChargeSpinDash_t.Hook(Sonic_ChargeSpinDash_r);
	}

	if (useHP)
		obj = true;
}
