#include "pch.h"
#include <fstream>

time_t t = NULL;

HelperFunctions HelperFunctionsGlobal;
std::string modpath;

extern "C" {
	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions& helperFunctions)
	{
		if (helperFunctions.Version < 11)
		{
			MessageBox(WindowHandle,
				L"Error, your version of the mod loader does not support API version 11. Some functionality will not be available.\nPlease exit the game and update the mod loader for the best experience.",
				L"Mod Name Error: Mod Loader out of date", MB_OK | MB_ICONERROR);
		}



		HelperFunctionsGlobal = helperFunctions; // Save the helper pointer for external use
		modpath = path;

		ReadConfig(path, helperFunctions);

		initKlonoa();
		init_Audio();
		srand((unsigned)time(&t));
	}


	__declspec(dllexport) void __cdecl OnFrame()
	{

	}

	__declspec(dllexport) void __cdecl OnExit()
	{
		FreeObjModels();
	}

	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };
}