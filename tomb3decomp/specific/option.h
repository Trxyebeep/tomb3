#pragma once
#include "../global/vars.h"

void inject_option(bool replace);

long GetRenderWidth();
long GetRenderHeight();
#ifdef TROYESTUFF
long GetRenderWidthDownscaled();
long GetRenderHeightDownscaled();
#endif
void do_detail_option(INVENTORY_ITEM* item);
void do_levelselect_option(INVENTORY_ITEM* item);
void do_pickup_option(INVENTORY_ITEM* item);
void do_sound_option(INVENTORY_ITEM* item);
void DefaultConflict();
void do_control_option(INVENTORY_ITEM* item);
void do_compass_option(INVENTORY_ITEM* item);
void do_passport_option(INVENTORY_ITEM* item);
void do_inventory_options(INVENTORY_ITEM* item);

enum d_txt
{
	DT_GAMMA,
	DT_TRUEALPHA,
	DT_DITHER,
	DT_FILTER,
	DT_ZBUFFER,
	DT_RESOLUTION,
	DT_EMPTY,
	DT_VIDEOTITLE,
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
	DOP_GAMMA,
	DOP_TRUEALPHA,
	DOP_DITHER,
	DOP_FILTER,
	DOP_ZBUFFER,
	DOP_RESOLUTION,
	DOP_NOPTS
};
