// dllmain.cpp : Defines the entry point for the DLL application.
#include "../tomb3/pch.h"
#include "../game/lara.h"
#include "../game/collide.h"
#include "../game/laraclmb.h"
#include "../game/larasurf.h"
#include "../game/laraswim.h"
#include "../game/laramisc.h"
#include "../game/health.h"
#include "../game/camera.h"
#include "../game/hair.h"
#include "../game/larafire.h"
#include "../game/lara2gun.h"
#include "../game/fish.h"
#include "../game/fusebox.h"
#include "../game/traps.h"
#include "../game/footprnt.h"
#include "../game/laraflar.h"
#include "../game/draw.h"
#include "../game/laraelec.h"
#include "../game/effect2.h"
#include "../game/biggun.h"
#include "../game/demo.h"
#include "../game/box.h"
#include "../game/effects.h"
#include "../game/control.h"
#include "../game/cinema.h"
#include "../game/text.h"

#include "../3dsystem/3d_gen.h"
#include "../3dsystem/hwinsert.h"
#include "../3dsystem/scalespr.h"

#include "../specific/drawbars.h"
#include "../specific/transform.h"
#include "../specific/litesrc.h"
#include "../specific/output.h"
#include "../specific/smain.h"
#include "../specific/hwrender.h"
#include "../specific/game.h"
#include "../specific/file.h"
#include "../specific/draweffects.h"
#include "../specific/picture.h"
#include "../specific/option.h"
#include "../specific/input.h"

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
	inject_3dgen(replace);
	inject_hwinsert(replace);
	inject_scalespr(replace);

	//game
	inject_lara(replace);
	inject_collide(replace);
	inject_laraclmb(replace);
	inject_larasurf(replace);
	inject_laraswim(replace);
	inject_laramisc(replace);
	inject_health(replace);
	inject_camera(replace);
	inject_hair(replace);
	inject_larafire(replace);
	inject_lara2gun(replace);
	inject_fish(replace);
	inject_fusebox(replace);
	inject_traps(replace);
	inject_footprnt(replace);
	inject_laraflar(replace);
	inject_draw(replace);
	inject_laraelec(replace);
	inject_effect2(replace);
	inject_biggun(replace);
	inject_demo(replace);
	inject_box(replace);
	inject_effects(replace);
	inject_control(replace);
	inject_cinema(replace);
	inject_text(replace);

	//specific
	inject_drawbars(replace);
	inject_transform(replace);
	inject_litesrc(replace);
	inject_output(replace);
	inject_smain(replace);
	inject_hwrender(replace);
	inject_sgame(replace);
	inject_file(replace);
	inject_draweffects(replace);
	inject_picture(replace);
	inject_option(replace);
	inject_input(replace);
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

