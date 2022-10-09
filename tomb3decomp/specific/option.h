#pragma once
#include "../global/vars.h"

void inject_option(bool replace);

long GetRenderWidth();
long GetRenderHeight();
void do_detail_option(INVENTORY_ITEM* item);
void do_levelselect_option(INVENTORY_ITEM* item);

#define do_inventory_options	( (void(__cdecl*)(INVENTORY_ITEM*)) 0x00487750 )

enum d_txt
{
#ifdef TROYESTUFF
	DT_PICKUP,
	DT_BARS,
	DT_SHADOW,
	DT_FOOTPRINTS,
#endif
	DT_GAMMA,
	DT_TRUEALPHA,
	DT_DITHER,
	DT_FILTER,
	DT_ZBUFFER,
	DT_RESOLUTION,
	DT_EMPTY,
	DT_VIDEOTITLE,
#ifdef TROYESTUFF
	DT_OP_PICKUP,
	DT_OP_BARS,
	DT_OP_SHADOW,
	DT_OP_FOOTPRINTS,
#endif
	DT_OP_GAMMA,
	DT_OP_TRUEALPHA,
	DT_OP_DITHER,
	DT_OP_FILTER,
	DT_OP_ZBUFFER,
	DT_OP_RESOLUTION,
	DT_NUMT
};

enum d_options
{
#ifdef TROYESTUFF
	DOP_PICKUP,
	DOP_BARS,
	DOP_SHADOW,
	DOP_FOOTPRINTS,
#endif
	DOP_GAMMA,
	DOP_TRUEALPHA,
	DOP_DITHER,
	DOP_FILTER,
	DOP_ZBUFFER,
	DOP_RESOLUTION,
	DOP_NOPTS
};
