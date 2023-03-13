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
#include "../game/kayak.h"
#include "../game/sub.h"
#include "../game/flamer.h"
#include "../game/objlight.h"
#include "../game/setup.h"
#include "../game/pickup.h"
#include "../game/punk.h"
#include "../game/lara1gun.h"
#include "../game/people.h"
#include "../game/51civvy.h"
#include "../game/invfunc.h"
#include "../game/triboss.h"
#include "../game/inventry.h"
#include "../game/londboss.h"
#include "../game/gameflow.h"
#include "../game/sound.h"
#include "../game/savegame.h"
#include "../game/boat.h"
#include "../game/quadbike.h"
#include "../game/minecart.h"
#include "../game/sphere.h"
#include "../game/train.h"
#include "../game/diver.h"
#include "../game/missile.h"
#include "../game/objects.h"
#include "../game/items.h"
#include "../game/lot.h"
#include "../game/cobra.h"
#include "../game/51laser.h"
#include "../game/winston.h"
#include "../game/mpgun.h"
#include "../game/51baton.h"
#include "../game/flymaker.h"
#include "../game/moveblok.h"
#include "../game/cleaner.h"
#include "../game/orca.h"
#include "../game/rapmaker.h"
#include "../game/wingmute.h"
#include "../game/willboss.h"
#include "../game/tribeaxe.h"
#include "../game/trex.h"
#include "../game/tonyboss.h"
#include "../game/compy.h"
#include "../game/tiger.h"
#include "../game/shiva.h"
#include "../game/target.h"
#include "../game/swat.h"
#include "../game/monkey.h"
#include "../game/sealmute.h"
#include "../game/rat.h"
#include "../game/raptor.h"
#include "../game/firehead.h"
#include "../game/prisoner.h"
#include "../game/lasers.h"

#include "../3dsystem/3d_gen.h"
#include "../3dsystem/hwinsert.h"
#include "../3dsystem/scalespr.h"
#include "../3dsystem/phd_math.h"

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
#include "../specific/display.h"
#include "../specific/specific.h"
#include "../specific/frontend.h"
#include "../specific/dd.h"
#include "../specific/dxdialog.h"
#include "../specific/dxshell.h"
#include "../specific/winmain.h"
#include "../specific/utils.h"
#include "../specific/init.h"
#include "../specific/time.h"
#include "../specific/di.h"
#include "../specific/audio.h"
#include "../specific/ds.h"
#include "../specific/texture.h"
#include "../specific/mmx.h"
#include "../specific/fmv.h"
#include "../specific/drawprimitive.h"

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
#ifdef RANDO_STUFF
	replace = 0;
#endif

	//3dsystem
	inject_3dgen(replace);
	inject_hwinsert(replace);
	inject_scalespr(replace);
	inject_phdmath(replace);

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
	inject_kayak(replace);
	inject_sub(replace);
	inject_flamer(replace);
	inject_objlight(replace);
	inject_setup(replace);
	inject_pickup(replace);
	inject_punk(replace);
	inject_lara1gun(replace);
	inject_people(replace);
	inject_civvy(replace);
	inject_invfunc(replace);
	inject_triboss(replace);
	inject_inventry(replace);
	inject_londboss(replace);
	inject_gameflow(replace);
	inject_sound(replace);
	inject_savegame(replace);
	inject_boat(replace);
	inject_quadbike(replace);
	inject_minecart(replace);
	inject_sphere(replace);
	inject_train(replace);
	inject_diver(replace);
	inject_missile(replace);
	inject_objects(replace);
	inject_items(replace);
	inject_lot(replace);
	inject_cobra(replace);
	inject_51laser(replace);
	inject_winston(replace);
	inject_mpgun(replace);
	inject_51baton(replace);
	inject_flymaker(replace);
	inject_moveblok(replace);
	inject_cleaner(replace);
	inject_orca(replace);
	inject_rapmaker(replace);
	inject_wingmute(replace);
	inject_willboss(replace);
	inject_tribeaxe(replace);
	inject_trex(replace);
	inject_tonyboss(replace);
	inject_compy(replace);
	inject_tiger(replace);
	inject_shiva(replace);
	inject_target(replace);
	inject_swat(replace);
	inject_monkey(replace);
	inject_sealmute(replace);
	inject_rat(replace);
	inject_raptor(replace);
	inject_firehead(replace);
	inject_prisoner(replace);
	inject_lasers(replace);

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
	inject_display(replace);
	inject_specific(replace);
	inject_frontend(replace);
	inject_dd(replace);
	inject_dxdialog(replace);
	inject_dxshell(replace);
	inject_winmain(replace);
	inject_utils(replace);
	inject_init(replace);
	inject_time(replace);
	inject_di(replace);
	inject_audio(replace);
	inject_ds(replace);
	inject_texture(replace);
	inject_mmx(replace);
	inject_fmv(replace);
	inject_drawprim(replace);
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

	return 1;
}

