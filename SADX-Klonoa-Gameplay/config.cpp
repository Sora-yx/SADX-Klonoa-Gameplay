#include "pch.h"

bool allowSpinDash = false;
bool allowHomingAttack = false;
bool nerfPhysics = true;


void ReadConfig(const char* path, const HelperFunctions& helperFunctions) {
	//Ini file Configuration
	const IniFile* config = new IniFile(std::string(path) + "\\config.ini");

	delete config;

	if (nerfPhysics)
	{
		PhysicsArray[Characters_Sonic] = PhysicsArray[Characters_Tails];
	}
}