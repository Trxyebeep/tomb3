// dllmain.cpp : Defines the entry point for the DLL application.
#include "../tomb3/pch.h"
#include "../game/lara.h"
#include "../game/collide.h"
#include "../game/laraclmb.h"
#include "../game/larasurf.h"
#include "../game/laraswim.h"
#include "../game/laramisc.h"

HINSTANCE hinstance = 0;

#ifndef BUILD_DLL
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif

DLL_EXPORT long dummyz();

long dummyz()
{
	return 0;
}

void inject_all(bool replace)
{
	//3dsystem
	

	//game
	inject_lara(replace);
	inject_collide(replace);
	inject_laraclmb(replace);
	inject_larasurf(replace);
	inject_laraswim(replace);
	inject_laramisc(replace);

	//specific
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		hinstance = hModule;
		inject_all(1);

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

