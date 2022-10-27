#include "../tomb3/pch.h"
#include "boat.h"
#include "../specific/init.h"

void InitialiseBoat(short item_number)
{
	ITEM_INFO* item;
	BOAT_INFO* boat;

	item = &items[item_number];
	boat = (BOAT_INFO*)game_malloc(sizeof(BOAT_INFO), 0);
	item->data = boat;
	boat->boat_turn = 0;
	boat->right_fallspeed = 0;
	boat->left_fallspeed = 0;
	boat->tilt_angle = 0;
	boat->extra_rotation = 0;
	boat->water = 0;
	boat->pitch = 0;

	for (int i = 0; i < 32; i++)
	{
		WakePts[i][0].life = 0;
		WakePts[i][1].life = 0;
	}
}

void inject_boat(bool replace)
{
	INJECT(0x00411FE0, InitialiseBoat, replace);
}
