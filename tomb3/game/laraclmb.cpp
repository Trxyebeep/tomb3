#include "../tomb3/pch.h"
#include "laraclmb.h"
#include "lara.h"
#include "control.h"
#include "laramisc.h"
#include "../specific/input.h"
#include "camera.h"

long LaraCheckForLetGo(ITEM_INFO* item, COLL_INFO* coll)
{
	FLOOR_INFO* floor;
	short room_number;

	item->gravity_status = 0;
	item->fallspeed = 0;
	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	coll->trigger = trigger_index;

	if (input & IN_ACTION && item->hit_points > 0)
		return 0;

	lara.torso_x_rot = 0;
	lara.torso_y_rot = 0;
	lara.head_x_rot = 0;
	lara.head_y_rot = 0;
	item->anim_number = ANIM_FALLDOWN;
	item->frame_number = anims[ANIM_FALLDOWN].frame_base;
	item->current_anim_state = AS_FORWARDJUMP;
	item->goal_anim_state = AS_FORWARDJUMP;
	item->gravity_status = 1;
	item->speed = 2;
	item->fallspeed = 1;
	lara.gun_status = LG_ARMLESS;
	return 1;
}

void LaraDoClimbLeftRight(ITEM_INFO* item, COLL_INFO* coll, long result, long shift)
{
	if (result == 1)
	{
		if (input & IN_LEFT)
			item->goal_anim_state = AS_CLIMBLEFT;
		else if (input & IN_RIGHT)
			item->goal_anim_state = AS_CLIMBRIGHT;
		else
			item->goal_anim_state = AS_CLIMBSTNC;

		item->pos.y_pos += shift;
	}
	else if (!result)
	{
		item->pos.x_pos = coll->old.x;
		item->pos.z_pos = coll->old.z;
		item->current_anim_state = AS_CLIMBSTNC;
		item->goal_anim_state = AS_CLIMBSTNC;

		if (coll->old_anim_state == AS_CLIMBSTNC)
		{
			item->frame_number = coll->old_frame_number;
			item->anim_number = coll->old_anim_number;
			AnimateLara(item);
		}
		else
		{
			item->anim_number = ANIM_CLIMBSTNC;
			item->frame_number = anims[ANIM_CLIMBSTNC].frame_base;
		}
	}
	else
	{
		item->goal_anim_state = AS_HANG;

		do AnimateItem(item); while (item->current_anim_state != AS_HANG);

		item->pos.x_pos = coll->old.x;
		item->pos.z_pos = coll->old.z;
	}
}

void lara_as_climbleft(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->enable_spaz = 0;
	coll->enable_baddie_push = 0;
	camera.target_angle = -5460;
	camera.target_elevation = -2730;

	if (!(input & IN_LEFT) && !(input & IN_LSTEP))
		item->goal_anim_state = AS_CLIMBSTNC;
}

void lara_as_climbright(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->enable_spaz = 0;
	coll->enable_baddie_push = 0;
	camera.target_angle = 5460;
	camera.target_elevation = -2730;

	if (!(input & IN_RIGHT) && !(input & IN_RSTEP))
		item->goal_anim_state = AS_CLIMBSTNC;
}

void lara_as_climbstnc(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->enable_spaz = 0;
	coll->enable_baddie_push = 0;
	camera.target_elevation = -3640;

	if (input & IN_LOOK)
		LookUpDown();

	if (input & IN_LEFT || input & IN_LSTEP)
		item->goal_anim_state = AS_CLIMBLEFT;
	else if (input & IN_RIGHT || input & IN_RSTEP)
		item->goal_anim_state = AS_CLIMBRIGHT;
	else if (input & IN_JUMP)
	{
		item->goal_anim_state = AS_BACKJUMP;
		lara.gun_status = LG_ARMLESS;
		lara.move_angle = item->pos.y_rot + 0x8000;
	}
}

void lara_as_climbing(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->enable_spaz = 0;
	coll->enable_baddie_push = 0;
	camera.target_elevation = 5460;
}

void lara_as_climbend(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->enable_spaz = 0;
	coll->enable_baddie_push = 0;
	camera.flags = 1;
	camera.target_angle = -8190;
}

void lara_as_climbdown(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->enable_spaz = 0;
	coll->enable_baddie_push = 0;
	camera.target_elevation = -8190;
}

void lara_col_climbleft(ITEM_INFO* item, COLL_INFO* coll)
{
	long result, shift;

	if (!LaraCheckForLetGo(item, coll))
	{
		lara.move_angle = item->pos.y_rot - 0x4000;
		result = LaraTestClimbPos(item, coll->radius, -120 - coll->radius, -512, 512, &shift);
		LaraDoClimbLeftRight(item, coll, result, shift);
	}
}

void lara_col_climbright(ITEM_INFO* item, COLL_INFO* coll)
{
	long result, shift;

	if (!LaraCheckForLetGo(item, coll))
	{
		lara.move_angle = item->pos.y_rot + 0x4000;
		result = LaraTestClimbPos(item, coll->radius, coll->radius + 120, -512, 512, &shift);
		LaraDoClimbLeftRight(item, coll, result, shift);
	}
}

void lara_col_climbstnc(ITEM_INFO* item, COLL_INFO* coll)
{
	long result_r, result_l, shift_r, shift_l, ledge_r, ledge_l;

	if (LaraCheckForLetGo(item, coll) || item->anim_number != ANIM_CLIMBSTNC)
		return;

	if (input & IN_FORWARD)
	{
		if (item->goal_anim_state == AS_NULL)
			return;

		item->goal_anim_state = AS_CLIMBSTNC;
		result_r = LaraTestClimbUpPos(item, coll->radius, coll->radius + 120, &shift_r, &ledge_r);
		result_l = LaraTestClimbUpPos(item, coll->radius, -120 - coll->radius, &shift_l, &ledge_l);

		if (result_r && result_l)
		{
			if (result_r >= 0 && result_l >= 0)
			{
				if (shift_r)
				{
					if (shift_l)
					{
						if (shift_l >= 0 == shift_r >= 0)
						{
							if (shift_r < 0 && shift_r < shift_l)
								shift_l = shift_r;
							else if (shift_r > 0 && shift_r > shift_l)
								shift_l = shift_r;
						}
					}
					else
						shift_l = shift_r;
				}

				item->goal_anim_state = AS_CLIMBING;
				item->pos.y_pos += shift_l;
			}
			else if (abs(ledge_l - ledge_r) <= 120)
			{
				item->goal_anim_state = AS_NULL;
				item->pos.y_pos += (ledge_l + ledge_r) / 2 - 256;
			}
		}
	}
	else if (input & IN_BACK)
	{
		if (item->goal_anim_state == AS_HANG)
			return;

		item->goal_anim_state = AS_CLIMBSTNC;
		item->pos.y_pos += 256;
		result_r = LaraTestClimbPos(item, coll->radius, coll->radius + 120, -512, 512, &shift_r);
		result_l = LaraTestClimbPos(item, coll->radius, -120 - coll->radius, -512, 512, &shift_l);
		item->pos.y_pos -= 256;

		if (result_r && result_l)
		{
			if (shift_r && shift_l)
			{
				if (shift_l >= 0 == shift_r >= 0)
				{
					if (shift_r < 0 && shift_r < shift_l)
						shift_l = shift_r;
					else if (shift_r > 0 && shift_r > shift_l)
						shift_l = shift_r;
				}
			}

			if (result_r == 1 && result_l == 1)
			{
				item->goal_anim_state = AS_CLIMBDOWN;
				item->pos.y_pos += shift_l;
			}
			else
				item->goal_anim_state = AS_HANG;
		}
	}
}

void lara_col_climbing(ITEM_INFO* item, COLL_INFO* coll)
{
	long frame, yshift, result_r, result_l, shift_r, shift_l, ledge_r, ledge_l;

	if (LaraCheckForLetGo(item, coll) || item->anim_number != ANIM_CLIMBING)
		return;

	frame = item->frame_number - anims[ANIM_CLIMBING].frame_base;

	if (!frame)
		yshift = 0;
	else if (frame == 28 || frame == 29)
		yshift = -256;
	else if (frame == 57)
		yshift = -512;
	else
		return;

	item->pos.y_pos += yshift - 256;
	result_r = LaraTestClimbUpPos(item, coll->radius, coll->radius + 120, &shift_r, &ledge_r);
	result_l = LaraTestClimbUpPos(item, coll->radius, -120 - coll->radius, &shift_l, &ledge_l);
	item->pos.y_pos += 256;

	if (!result_r || !result_l || !(input & IN_FORWARD))
	{
		item->goal_anim_state = AS_CLIMBSTNC;

		if (yshift)
			AnimateLara(item);
	}
	else
	{
		if (result_r >= 0 && result_l >= 0)
		{
			item->goal_anim_state = AS_CLIMBING;
			item->pos.y_pos -= yshift;
		}
		else
		{
			item->goal_anim_state = AS_CLIMBSTNC;
			AnimateLara(item);

			if (abs(ledge_l - ledge_r) <= 120)
			{
				item->goal_anim_state = AS_NULL;
				item->pos.y_pos += (ledge_l + ledge_r) / 2 - 256;
			}
		}
	}
}

void lara_col_climbdown(ITEM_INFO* item, COLL_INFO* coll)
{
	long frame, yshift, result_r, result_l, shift_r, shift_l;

	if (LaraCheckForLetGo(item, coll) || item->anim_number != ANIM_CLIMBDOWN)
		return;

	frame = item->frame_number - anims[ANIM_CLIMBDOWN].frame_base;

	if (!frame)
		yshift = 0;
	else if (frame == 28 || frame == 29)
		yshift = 256;
	else if (frame == 57)
		yshift = 512;
	else
		return;

	item->pos.y_pos += yshift + 256;
	result_r = LaraTestClimbPos(item, coll->radius, coll->radius + 120, -512, 512, &shift_r);
	result_l = LaraTestClimbPos(item, coll->radius, -120 - coll->radius, -512, 512, &shift_l);
	item->pos.y_pos -= 256;

	if (!result_r || !result_l || !(input & IN_BACK))
	{
		item->goal_anim_state = AS_CLIMBSTNC;

		if (yshift)
			AnimateLara(item);
	}
	else
	{
		if (shift_r && shift_l)
		{
			if (shift_l < 0 != shift_r < 0)
			{
				item->goal_anim_state = AS_CLIMBSTNC;
				AnimateLara(item);
				return;
			}

			if (shift_r < 0 && shift_r < shift_l)
				shift_l = shift_r;
			else if (shift_r > 0 && shift_r > shift_l)
				shift_l = shift_r;
		}

		if (result_r == -1 || result_l == -1)
		{
			item->anim_number = ANIM_CLIMBSTNC;
			item->frame_number = anims[ANIM_CLIMBSTNC].frame_base;
			item->current_anim_state = AS_CLIMBSTNC;
			item->goal_anim_state = AS_HANG;
			AnimateLara(item);
		}
		else
		{
			item->goal_anim_state = AS_CLIMBDOWN;
			item->pos.y_pos -= yshift;
		}
	}
}

long LaraTestClimb(long x, long y, long z, long xfront, long zfront, long item_height, short item_room, long* shift)
{
	FLOOR_INFO* floor;
	long hang, h, c;
	short room_number;

	*shift = 0;
	hang = 1;

	if (!lara.climb_status)
		return 0;

	room_number = item_room;
	floor = GetFloor(x, y - 128, z, &room_number);
	h = GetHeight(floor, x, y, z);

	if (h == NO_HEIGHT)
		return 0;

	h -= 128 + y + item_height;

	if (h < -70)
		return 0;

	if (h < 0)
		*shift = h;

	c = GetCeiling(floor, x, y, z) - y;

	if (c > 70)
		return 0;

	if (c > 0)
	{
		if (*shift)
			return 0;

		*shift = c;
	}

	if (item_height + h < 900)
		hang = 0;

	floor = GetFloor(x + xfront, y, z + zfront, &room_number);
	h = GetHeight(floor, x + xfront, y, z + zfront);

	if (h != NO_HEIGHT)
		h -= y;

	if (h <= 70)
	{
		if (h > 0)
		{
			if (*shift < 0)
				return 0;

			if (h > *shift)
				*shift = h;
		}

		room_number = item_room;
		GetFloor(x, y + item_height, z, &room_number);
		floor = GetFloor(x + xfront, y + item_height, z + zfront, &room_number);
		c = GetCeiling(floor, x + xfront, y + item_height, z + zfront);

		if (c == NO_HEIGHT)
			return 1;

		c -= y;

		if (c <= h)
			return 1;

		if (c >= 512)
			return 1;

		if (c > 442 && *shift <= 0)
		{
			*shift = c - 512;
			return 1;
		}

		return -(hang != 0);
	}

	c = GetCeiling(floor, x + xfront, y, z + zfront) - y;

	if (c >= 512)
		return 1;

	if (c > 442)
	{
		if (*shift <= 0)
		{
			*shift = c - 512;
			return 1;
		}

		return -(hang != 0);
	}

	if (c > 0)
		return -(hang != 0);

	if (c <= -70 || !hang || *shift > 0)
		return 0;

	if (*shift > c)
		*shift = c;

	return -1;
}

long LaraTestClimbPos(ITEM_INFO* item, long front, long right, long origin, long height, long* shift)
{
	long quad, x, z, xfront, zfront;

	xfront = 0;
	zfront = 0;
	quad = ushort(item->pos.y_rot + 0x2000) >> 14;

	switch (quad)
	{
	case NORTH:
		x = item->pos.x_pos + right;
		z = item->pos.z_pos + front;
		zfront = 4;
		break;

	case EAST:
		x = item->pos.x_pos + front;
		z = item->pos.z_pos - right;
		xfront = 4;
		break;

	case SOUTH:
		x = item->pos.x_pos - right;
		z = item->pos.z_pos - front;
		zfront = -4;
		break;

	default:
		x = item->pos.x_pos - front;
		z = item->pos.z_pos + right;
		xfront = -4;
		break;
	}

	return LaraTestClimb(x, item->pos.y_pos + origin, z, xfront, zfront, height, item->room_number, shift);
}

long LaraTestClimbUpPos(ITEM_INFO* item, long front, long right, long* shift, long* ledge)
{
	FLOOR_INFO* floor;
	long quad, x, y, z, xfront, zfront, h, c;
	short room_number;

	y = item->pos.y_pos - 768;
	xfront = 0;
	zfront = 0;
	quad = ushort(item->pos.y_rot + 0x2000) >> 14;

	switch (quad)
	{
	case NORTH:
		x = item->pos.x_pos + right;
		z = item->pos.z_pos + front;
		zfront = 4;
		break;

	case EAST:
		x = item->pos.x_pos + front;
		z = item->pos.z_pos - right;
		xfront = 4;
		break;

	case SOUTH:
		x = item->pos.x_pos - right;
		z = item->pos.z_pos - front;
		zfront = -4;
		break;

	default:
		x = item->pos.x_pos - front;
		z = item->pos.z_pos + right;
		xfront = -4;
		break;
	}

	*shift = 0;
	room_number = item->room_number;
	floor = GetFloor(x, y, z, &room_number);
	c = 256 - y + GetCeiling(floor, x, y, z);

	if (c > 70)
		return 0;

	if (c > 0)
		*shift = c;

	floor = GetFloor(x + xfront, y, z + zfront, &room_number);
	h = GetHeight(floor, x + xfront, y, z + zfront);

	if (h == NO_HEIGHT)
	{
		*ledge = NO_HEIGHT;
		return 1;
	}

	h -= y;
	*ledge = h;

	if (h <= 128)
	{
		if (h > 0 && h > *shift)
			*shift = h;

		room_number = item->room_number;
		GetFloor(x, y + 512, z, &room_number);
		floor = GetFloor(x + xfront, y + 512, z + zfront, &room_number);
		c = GetCeiling(floor, x + xfront, y + 512, z + zfront) - y;
		return c <= h || c >= 512;
	}

	c = GetCeiling(floor, x + xfront, y, z + zfront) - y;

	if (c >= 512)
		return 1;

	if (h - c > 762)
	{
		*shift = h;
		return -1;
	}

	return 0;
}
