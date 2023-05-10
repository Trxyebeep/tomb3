#include "../tomb3/pch.h"
#include "diver.h"
#include "control.h"
#include "effect2.h"
#include "../specific/game.h"
#include "objects.h"
#include "items.h"
#include "missile.h"
#include "box.h"
#include "lara.h"
#include "effects.h"

static BITE_INFO diver_poon = { 17, 164, 44, 18 };

long GetWaterSurface(long x, long y, long z, short room_number)
{
	ROOM_INFO* r;
	FLOOR_INFO* floor;

	r = &room[room_number];
	floor = &r->floor[((z - r->z) >> WALL_SHIFT) + r->x_size * ((x - r->x) >> WALL_SHIFT)];

	if (r->flags & ROOM_UNDERWATER)
	{
		while (floor->sky_room != NO_ROOM)
		{
			r = &room[floor->sky_room];

			if (!(r->flags & ROOM_UNDERWATER))
				return floor->ceiling << 8;

			floor = &r->floor[((z - r->z) >> WALL_SHIFT) + r->x_size * ((x - r->x) >> WALL_SHIFT)];
		}
	}
	else
	{
		while (floor->pit_room != NO_ROOM)
		{
			r = &room[floor->pit_room];

			if (r->flags & ROOM_UNDERWATER)
				return floor->floor << 8;

			floor = &r->floor[((z - r->z) >> WALL_SHIFT) + r->x_size * ((x - r->x) >> WALL_SHIFT)];
		}
	}

	return NO_HEIGHT;
}

void ControlGhostGasEmitter(short item_number)
{
	ITEM_INFO* item;
	SPARKS* sptr;
	long dx, dz, size;

	item = &items[item_number];

	if (!TriggerActive(item) || wibble & 0xF)
		return;

	dx = lara_item->pos.x_pos - item->pos.x_pos;
	dz = lara_item->pos.z_pos - item->pos.z_pos;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 0;
	sptr->sG = 0;
	sptr->sB = 0;
	sptr->dR = 12;
	sptr->dG = 32;
	sptr->dB = 0;
	sptr->FadeToBlack = 32;
	sptr->ColFadeSpeed = (GetRandomControl() & 7) + 24;
	sptr->TransType = 2;
	sptr->extras = 0;
	sptr->Life = (GetRandomControl() & 7) + 64;
	sptr->sLife = sptr->Life;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomControl() & 0x1FF) + item->pos.x_pos - 256;
	sptr->y = item->pos.y_pos - (GetRandomControl() & 0xF) - 264;
	sptr->z = (GetRandomControl() & 0x3FF) + item->pos.z_pos - 512;
	sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Yvel = -1 - (GetRandomControl() & 1);
	sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Friction = 4;

	if (GetRandomControl() & 1)
	{
		sptr->Flags = SF_ALTDEF | SF_ROTATE | SF_DEF | SF_SCALE;
		sptr->RotAng = GetRandomControl() & 0xFFF;

		if (GetRandomControl() & 1)
			sptr->RotAdd = -4 - (GetRandomControl() & 7);
		else
			sptr->RotAdd = (GetRandomControl() & 7) + 4;
	}
	else
		sptr->Flags = SF_ALTDEF | SF_DEF | SF_SCALE;

	sptr->Scalar = 3;
	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->MaxYvel = 0;
	sptr->Gravity = 0;
	size = (GetRandomControl() & 0x1F) + 96;
	sptr->Width = uchar(size >> 1);
	sptr->sWidth = sptr->Width;
	sptr->dWidth = sptr->Width;
	sptr->Height = uchar((size + (GetRandomControl() & 0x1F) + 32) >> 1);
	sptr->sHeight = sptr->Height;
	sptr->dHeight = sptr->Height;
}

static short Harpoon(long x, long y, long z, short speed, short ang, short room_number)
{
	FX_INFO* fx;
	short fx_num;

	fx_num = CreateEffect(room_number);

	if (fx_num != NO_ITEM)
	{
		fx = &effects[fx_num];
		fx->pos.x_pos = x;
		fx->pos.y_pos = y;
		fx->pos.z_pos = z;
		fx->room_number = room_number;
		fx->pos.x_rot = 0;
		fx->pos.y_rot = ang;
		fx->pos.z_rot = 0;
		fx->speed = 150;
		fx->fallspeed = 0;
		fx->frame_number = 0;
		fx->object_number = DIVER_HARPOON;
		fx->shade = 0xE00;
		ShootAtLara(fx);
	}

	return fx_num;
}

void DiverControl(short item_number)
{
	ITEM_INFO* item;
	CREATURE_INFO* diver;
	AI_INFO info;
	GAME_VECTOR start;
	GAME_VECTOR target;
	long surface, shoot;
	short angle;

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	diver = (CREATURE_INFO*)item->data;

	if (item->hit_points <= 0)
	{
		if (item->current_anim_state != 9)
		{
			item->anim_number = objects[DIVER].anim_index + 16;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 9;
		}

		CreatureFloat(item_number);
		return;
	}

	CreatureAIInfo(item, &info);
	GetCreatureMood(item, &info, 0);
	CreatureMood(item, &info, 0);

	if (lara.water_status == LARA_ABOVEWATER)
	{
		start.x = item->pos.x_pos;
		start.y = item->pos.y_pos - 256;
		start.z = item->pos.z_pos;
		start.room_number = item->room_number;
		target.x = lara_item->pos.x_pos;
		target.y = lara_item->pos.y_pos - 612;
		target.z = lara_item->pos.z_pos;
		shoot = LOS(&start, &target);

		if (shoot)
		{
			diver->target.x = lara_item->pos.x_pos;
			diver->target.y = lara_item->pos.y_pos;
			diver->target.z = lara_item->pos.z_pos;
		}

		if (info.angle < -0x2000 || info.angle > 0x2000)
			shoot = 0;
	}
	else if (info.angle > -0x2000 && info.angle < 0x2000)
	{
		start.x = item->pos.x_pos;
		start.y = item->pos.y_pos;
		start.z = item->pos.z_pos;
		start.room_number = item->room_number;
		target.x = lara_item->pos.x_pos;
		target.y = lara_item->pos.y_pos;
		target.z = lara_item->pos.z_pos;
		shoot = LOS(&start, &target);
	}
	else
		shoot = 0;

	angle = CreatureTurn(item, diver->maximum_turn);
	surface = GetWaterSurface(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number) + 512;

	switch (item->current_anim_state)
	{
	case 1:
		diver->maximum_turn = 546;

		if (diver->target.y < surface && item->pos.y_pos < surface + diver->LOT.fly)
			item->goal_anim_state = 2;
		else if (diver->mood != ESCAPE_MOOD && shoot)
			item->goal_anim_state = 4;

		break;

	case 2:
		diver->maximum_turn = 546;

		if (diver->target.y > surface)
			item->goal_anim_state = 1;
		else if (diver->mood != ESCAPE_MOOD && shoot)
			item->goal_anim_state = 6;

		break;

	case 3:
	case 7:

		if (!diver->flags)
		{
			CreatureEffect(item, &diver_poon, Harpoon);
			diver->flags = 1;
		}

		break;

	case 4:
		diver->flags = 0;

		if (!shoot || diver->mood == ESCAPE_MOOD || diver->target.y < surface && item->pos.y_pos < surface + diver->LOT.fly)
			item->goal_anim_state = 1;
		else
			item->goal_anim_state = 3;

		break;

	case 6:
		diver->flags = 0;

		if (!shoot || diver->mood == ESCAPE_MOOD || diver->target.y > surface)
			item->goal_anim_state = 2;
		else
			item->goal_anim_state = 7;
		break;
	}

	CreatureAnimation(item_number, angle, 0);

	if (item->current_anim_state == 1 || item->current_anim_state > 2 && item->current_anim_state <= 4)
		CreatureUnderwater(item, 512);
	else
		item->pos.y_pos = surface - 512;
}
