#include "pch.h"

bool allowSpinDash = false;
bool nerfPhysics = true;

void ReadConfig(const char* path, const HelperFunctions& helperFunctions) {
	//Ini file Configuration
	const IniFile* config = new IniFile(std::string(path) + "\\config.ini");

	delete config;

}