#include "pch.h"

bool allowKlonoaMoves = false;
bool nerfPhysics = true;
bool hud = true;
bool obj = true;
bool useHP = true;
bool infiniteHover = false;
bool music = true;

uint8_t hpMaxConfig = 1;

static const uint8_t hpList[] =
{
	6,
	3,
	1
};

static UsercallFunc(signed int, Sonic_ChargeSpinDash_t, (playerwk* a1, taskwk* a2), (a1, a2), 0x496EE0, rEAX, rEAX, rEDI);
static UsercallFuncVoid(DoHomingAttack_t, (playerwk* a1, taskwk* a2, motionwk2* a3), (a1, a2, a3), 0x494B80, rEAX, rECX, stack4);
static UsercallFunc(Bool, Sonic_JumpCancel_t, (taskwk* a1, playerwk* a2), (a1, a2), 0x492F50, rEAX, rESI, rEDI);

Bool Sonic_JumpCancel_r(taskwk* data, playerwk* a2)
{
	if (isKlonoa(data->charIndex) && !EV_MainThread_ptr && !isTailsRace(data->charIndex))
	{
		return false;
	}

	return Sonic_JumpCancel_t.Original(data, a2);
}

BOOL Sonic_ChargeSpinDash_r(playerwk* co2, taskwk* data)
{
	if (isKlonoa(data->charIndex) && !EV_MainThread_ptr && !isTailsRace(data->charIndex))
	{
		return false;
	}

	return Sonic_ChargeSpinDash_t.Original(co2, data);
}

void DoHomingAttack_r(playerwk* a1, taskwk* data, motionwk2* a3)
{
	if (isKlonoa(data->charIndex) && !EV_MainThread_ptr && !isTailsRace(data->charIndex))
	{
		data->mode = act_fall;
		a1->mj.reqaction = anm_fall;
		return;
	}

	return DoHomingAttack_t.Original(a1, data, a3);
}

void ReadConfig(const char* path, const HelperFunctions& helperFunctions) {
	//Ini file Configuration
	const IniFile* config = new IniFile(std::string(path) + "\\config.ini");
	allowKlonoaMoves = config->getBool("gameplay", "allowKlonoaMoves", true);
	nerfPhysics = config->getBool("gameplay", "nerfPhysics", true);
	useHP = config->getBool("gameplay", "useHP", true);
	hpMaxConfig = hpList[config->getInt("gameplay", "hpMax", 1)];
	infiniteHover = config->getBool("gameplay", "infiniteHover", false);

	hud = config->getBool("visual", "hud", true);
	obj = config->getBool("visual", "obj", true);

	music = config->getBool("audio", "music", true);

	delete config;

	if (nerfPhysics)
	{
		PhysicsArray_[Characters_Sonic] = PhysicsArray_[Characters_Tails];
		PhysicsArray_[Characters_Sonic].pos_error = 2.0f;
		PhysicsArray_[Characters_Sonic].height = 10.0f;
		PhysicsArray_[Characters_Sonic].eyes_height = 7.0f;
		PhysicsArray_[Characters_Sonic].center_height = 5.4f;
	}

	if (allowKlonoaMoves)
	{
		Sonic_ChargeSpinDash_t.Hook(Sonic_ChargeSpinDash_r);
		DoHomingAttack_t.Hook(DoHomingAttack_r);
		Sonic_JumpCancel_t.Hook(Sonic_JumpCancel_r);

		WriteJump((void*)0x49937e, (void*)0x49a848); //remove SH spin dash on building
		WriteData<1>((int*)0x499383, 0x90);
	}

	if (useHP)
		obj = true;

	if (music)
	{
		helperFunctions.ReplaceFile("system\\sounddata\\bgm\\wma\\icecap3.wma", "system\\music\\icecap3.mp3");
		helperFunctions.ReplaceFile("system\\sounddata\\bgm\\wma\\option.wma", "system\\music\\option.adx");
	}
}