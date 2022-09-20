#include "pch.h"
#include <fstream>
#include "hud.h"

time_t t = NULL;

HelperFunctions HelperFunctionsGlobal;
std::string modpath;

#define DataArray_(type, name, address, length) \
    static type *const name = (type *)address; static const int name##_Length = length

DataArray_(playerwk*, playerpwp_, 0x3B3CDF0, 8);

extern "C" {

	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions& helperFunctions)
	{
		if (helperFunctions.Version < 13)
		{
			MessageBox(WindowHandle,
				L"Error, your version of the mod loader does not support API version 13. Some functionality will not be available.\nPlease exit the game and update the mod loader for the best experience.",
				L"Mod Name Error: Mod Loader out of date", MB_OK | MB_ICONERROR);
		}


		HelperFunctionsGlobal = helperFunctions; // Save the helper pointer for external use
		modpath = path;
		ReadConfig(path, helperFunctions);

		initKlonoa();
		init_Audio();
		init_EnemiesHack();
		init_BossesHacks();
		initKlonoaHP();
		init_Hud();
		srand((unsigned)time(&t));
	}

	__declspec(dllexport) void __cdecl OnFrame()
	{
		if (GameState >= 15)
			RunCustomSounds();

		DrawKlonoaHUD();
	}


	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };
}