#include "../tomb3/pch.h"
#include "box.h"
#include "objects.h"
#include "../specific/game.h"
#include "lot.h"
#include "../3dsystem/phd_math.h"
#include "lara.h"
#include "draw.h"
#include "control.h"
#include "missile.h"
#include "items.h"
#include "sphere.h"
#include "camera.h"

BOX_INFO* boxes;
long number_boxes;
short* overlap;

short* ground_zone[4][2];
short* fly_zone[2];

void AlertNearbyGuards(ITEM_INFO* item)
{
	ITEM_INFO* target;
	CREATURE_INFO* creature;
	long dx, dy, dz, dist;

	for (int i = 0; i < MAX_LOT; i++)
	{
		creature = &baddie_slots[i];

		if (creature->item_num == NO_ITEM)
			continue;

		target = &items[creature->item_num];

		if (target->room_number == item->room_number)
		{
			creature->alerted = 1;
			continue;
		}

		dx = (target->pos.x_pos - item->pos.x_pos) >> 6;
		dy = (target->pos.y_pos - item->pos.y_pos) >> 6;
		dz = (target->pos.z_pos - item->pos.z_pos) >> 6;
		dist = SQUARE(dx) + SQUARE(dy) + SQUARE(dz);

		if (dist < 8000)
			creature->alerted = 1;
	}
}

void InitialiseCreature(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (item->object_number != ELECTRIC_CLEANER && item->object_number != SHIVA && item->object_number != TARGETS)
		item->pos.y_rot += short((GetRandomControl() - 0x4000) >> 1);

	item->collidable = 1;
	item->data = 0;
}

long CreatureActive(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (item->status == ITEM_INVISIBLE)
	{
		if (!EnableBaddieAI(item_number, 0))
			return 0;

		item->status = ITEM_ACTIVE;
	}

	if (item->data)
		return 1;

	return 0;
}

void CreatureAIInfo(ITEM_INFO* item, AI_INFO* info)
{
	CREATURE_INFO* creature;
	OBJECT_INFO* obj;
	ITEM_INFO* enemy;
	ROOM_INFO* r;
	FLOOR_INFO* floor;
	short* zone;
	long x, y, z;
	short pivot, ang, state;

	creature = (CREATURE_INFO*)item->data;

	if (!creature)
		return;

	obj = &objects[item->object_number];
	enemy = creature->enemy;

	if (!enemy)
	{
		enemy = lara_item;
		creature->enemy = lara_item;
	}

	if (creature->LOT.fly)
		zone = ground_zone[-1][0];
	else
		zone = ground_zone[(creature->LOT.step >> 8) - 1][flip_status];

	r = &room[item->room_number];
	floor = &r->floor[((item->pos.z_pos - r->z) >> WALL_SHIFT) + r->x_size * ((item->pos.x_pos - r->x) >> WALL_SHIFT)];
	item->box_number = floor->box;

	if (creature->LOT.fly)
		info->zone_number = 0x2000;
	else
		info->zone_number = zone[item->box_number];

	r = &room[enemy->room_number];
	floor = &r->floor[((enemy->pos.z_pos - r->z) >> WALL_SHIFT) + r->x_size * ((enemy->pos.x_pos - r->x) >> WALL_SHIFT)];
	enemy->box_number = floor->box;

	if (creature->LOT.fly)
		info->enemy_zone = 0x2000;
	else
		info->enemy_zone = zone[enemy->box_number];

	if (!obj->non_lot && (boxes[enemy->box_number].overlap_index & creature->LOT.block_mask ||
		creature->LOT.node[item->box_number].search_number == (creature->LOT.search_number | 0x8000)))
		info->enemy_zone |= 0x4000;

	pivot = obj->pivot_length;

	if (enemy == lara_item)
		ang = lara.move_angle;
	else
		ang = enemy->pos.y_rot;

	x = enemy->pos.x_pos + (14 * enemy->speed * phd_sin(ang) >> W2V_SHIFT) - (pivot * phd_sin(item->pos.y_rot) >> W2V_SHIFT) - item->pos.x_pos;
	y = item->pos.y_pos - enemy->pos.y_pos;
	z = enemy->pos.z_pos + (14 * enemy->speed * phd_cos(ang) >> W2V_SHIFT) - (pivot * phd_cos(item->pos.y_rot) >> W2V_SHIFT) - item->pos.z_pos;

	ang = (short)phd_atan(z, x);

	if (z > 32000 || z < -32000 || x > 32000 || x < -32000)
		info->distance = 0x7FFFFFFF;
	else if (creature->enemy)
		info->distance = SQUARE(x) + SQUARE(z);
	else
		info->distance = 0x7FFFFFFF;

	info->angle = ang - item->pos.y_rot;
	info->enemy_facing = ang - enemy->pos.y_rot + 0x8000;

	x = abs(x);
	z = abs(z);

	if (enemy == lara_item)
	{
		state = lara_item->current_anim_state;

		if (state == AS_DUCK || state == AS_DUCKROLL || state == AS_ALL4S || state == AS_CRAWL || state == AS_ALL4TURNL || state == AS_ALL4TURNR)
			y -= 384;
	}

	if (x > z)
		info->x_angle = (short)phd_atan(x + (z >> 1), y);
	else
		info->x_angle = (short)phd_atan(z + (x >> 1), y);

	info->ahead = info->angle > -0x4000 && info->angle < 0x4000;
	info->bite = info->ahead && enemy->hit_points > 0 && abs(enemy->pos.y_pos - item->pos.y_pos) <= 512;
}

long SearchLOT(LOT_INFO* LOT, long expansion)
{
	BOX_NODE* node;
	BOX_NODE* expand;
	BOX_INFO* box;
	short* zone;
	long index, done, box_number, overlap_flags, change;
	short search_zone;

	if (LOT->fly)
	{
		zone = ground_zone[-1][0];
		search_zone = 0x2000;
		
	}
	else
	{
		zone = ground_zone[(LOT->step >> 8) - 1][flip_status];
		search_zone = zone[LOT->head];
	}

	for (int i = 0; i < expansion; i++)
	{
		if (LOT->head == 2047)
		{
			LOT->tail = 2047;
			return 0;
		}

		box = &boxes[LOT->head];
		node = &LOT->node[LOT->head];
		index = box->overlap_index & 0x3FFF;
		done = 0;

		do
		{
			box_number = overlap[index++];
			overlap_flags = box_number & ~2047;

			if (box_number & 0x8000)
			{
				done = 1;
				box_number &= 0x7FF;
			}

			if (!LOT->fly && search_zone != zone[box_number])
				continue;

			change = boxes[box_number].height - box->height;

			if (change > LOT->step || change < LOT->drop)
				continue;

			expand = &LOT->node[box_number];

			if ((node->search_number & 0x7FFF) < (expand->search_number & 0x7FFF))
				continue;

			if (node->search_number & 0x8000)
			{
				if ((node->search_number & 0x7FFF) == (expand->search_number & 0x7FFF))
					continue;

				expand->search_number = node->search_number;
			}
			else
			{
				if ((node->search_number & 0x7FFF) == (expand->search_number & 0x7FFF) && !(expand->search_number & 0x8000))
					continue;

				if (boxes[box_number].overlap_index & LOT->block_mask)
					expand->search_number = node->search_number | 0x8000;
				else
				{
					expand->search_number = node->search_number;
					expand->exit_box = LOT->head;
				}
			}

			if (expand->next_expansion == 2047 && box_number != LOT->tail)
			{
				LOT->node[LOT->tail].next_expansion = (short)box_number;
				LOT->tail = (short)box_number;
			}

		} while (!done);

		LOT->head = node->next_expansion;
		node->next_expansion = 2047;
	}

	return 1;
}

long UpdateLOT(LOT_INFO* LOT, long expansion)
{
	BOX_NODE* expand;

	if (LOT->required_box != 2047 && LOT->required_box != LOT->target_box)
	{
		LOT->target_box = LOT->required_box;
		expand = &LOT->node[LOT->required_box];

		if (expand->next_expansion == 2047 && LOT->tail != LOT->required_box)
		{
			expand->next_expansion = LOT->head;

			if (LOT->head == 2047)
				LOT->tail = LOT->target_box;

			LOT->head = LOT->target_box;
		}

		LOT->search_number++;
		expand->search_number = LOT->search_number;
		expand->exit_box = 2047;
	}

	return SearchLOT(LOT, expansion);
}

void TargetBox(LOT_INFO* LOT, short box_number)
{
	BOX_INFO* box;

	box = &boxes[box_number & 0x7FF];
	LOT->target.x = (((ulong)box->bottom - (ulong)box->top - 1) >> 5) * GetRandomControl() + ((ulong)box->top << WALL_SHIFT) + 512;
	LOT->target.z = (((ulong)box->right - (ulong)box->left - 1) >> 5) * GetRandomControl() + ((ulong)box->left << WALL_SHIFT) + 512;
	LOT->required_box = box_number & 0x7FF;

	if (LOT->fly)
		LOT->target.y = box->height - 384;
	else
		LOT->target.y = box->height;
}

long EscapeBox(ITEM_INFO* item, ITEM_INFO* enemy, short box_number)
{
	BOX_INFO* box;
	long x, z;

	box = &boxes[box_number];
	x = (((ulong)box->bottom + (ulong)box->top) << 9) - enemy->pos.x_pos;
	z = (((ulong)box->left + (ulong)box->right) << 9) - enemy->pos.z_pos;

	if (x > -5120 && x < 5120 && z > -5120 && z < 5120)
		return 0;

	return z > 0 == item->pos.z_pos > enemy->pos.z_pos || x > 0 == item->pos.x_pos > enemy->pos.x_pos;
}

long ValidBox(ITEM_INFO* item, short zone_number, short box_number)
{
	CREATURE_INFO* creature;
	BOX_INFO* box;
	short* zone;

	creature = (CREATURE_INFO*)item->data;

	if (creature->LOT.fly)
		zone = ground_zone[-1][0];
	else
		zone = ground_zone[(creature->LOT.step >> 8) - 1][flip_status];

	if (!creature->LOT.fly && zone[box_number] != zone_number)
		return 0;

	box = &boxes[box_number];

	if (box->overlap_index & creature->LOT.block_mask)
		return 0;

	if (item->pos.z_pos > box->left << WALL_SHIFT && item->pos.z_pos < box->right << WALL_SHIFT &&
		item->pos.x_pos > box->top << WALL_SHIFT && item->pos.x_pos < box->bottom << WALL_SHIFT)
		return 0;

	return 1;
}

long StalkBox(ITEM_INFO* item, ITEM_INFO* enemy, short box_number)
{
	BOX_INFO* box;
	long x, z, xrange, zrange, enemy_quad, box_quad, baddie_quad;

	if (!enemy)
		return 0;

	box = &boxes[box_number];
	x = (((ulong)box->bottom + (ulong)box->top) << 9) - enemy->pos.x_pos;
	z = (((ulong)box->left + (ulong)box->right) << 9) - enemy->pos.z_pos;
	xrange = ((ulong)box->bottom - (ulong)box->top + 3) << WALL_SHIFT;	//3 is the # of blocks
	zrange = ((ulong)box->right - (ulong)box->left + 3) << WALL_SHIFT;

	if (x > xrange || x < -xrange || z > zrange || z < -zrange)
		return 0;

	enemy_quad = (enemy->pos.y_rot >> 14) + 2;
	box_quad = z <= 0 ? (x <= 0 ? 0 : 3) : (x > 0) + 1;

	if (enemy_quad == box_quad)
		return 0;

	baddie_quad = item->pos.z_pos <= enemy->pos.z_pos ? (item->pos.x_pos <= enemy->pos.x_pos ? 0 : 3) : (item->pos.x_pos > enemy->pos.x_pos) + 1;
	return enemy_quad != baddie_quad || abs(enemy_quad - box_quad) != 2;
}

target_type CalculateTarget(PHD_VECTOR* target, ITEM_INFO* item, LOT_INFO* LOT)
{
	BOX_INFO* box;
	long box_number, box_left, box_right, box_top, box_bottom;
	long left, right, top, bottom, prime_free;

	UpdateLOT(LOT, 5);
	target->x = item->pos.x_pos;
	target->y = item->pos.y_pos;
	target->z = item->pos.z_pos;
	box_number = item->box_number;

	if (box_number == 2047)
		return NO_TARGET;

	box = &boxes[box_number];
	left = box->left << WALL_SHIFT;
	right = (box->right << WALL_SHIFT) - 1;
	top = box->top << WALL_SHIFT;
	bottom = (box->bottom << WALL_SHIFT) - 1;
	prime_free = 15;

	do
	{
		box = &boxes[box_number];

		if (LOT->fly)
		{
			if (target->y > box->height - WALL_SIZE)
				target->y = box->height - WALL_SIZE;
		}
		else if (target->y > box->height)
			target->y = box->height;

		box_left = box->left << WALL_SHIFT;
		box_right = (box->right << WALL_SHIFT) - 1;
		box_top = box->top << WALL_SHIFT;
		box_bottom = (box->bottom << WALL_SHIFT) - 1;

		if (item->pos.z_pos >= box_left && item->pos.z_pos <= box_right && item->pos.x_pos >= box_top && item->pos.x_pos <= box_bottom)
		{
			left = box_left;
			right = box_right;
			top = box_top;
			bottom = box_bottom;
		}
		else
		{
			if (item->pos.z_pos < box_left)
			{
				if (prime_free & 1 && item->pos.x_pos >= box_top && item->pos.x_pos <= box_bottom)
				{
					if (target->z < box_left + 512)
						target->z = box_left + 512;

					if (prime_free & 16)
						return SECONDARY_TARGET;

					if (box_top > top)
						top = box_top;

					if (box_bottom < bottom)
						bottom = box_bottom;

					prime_free = 1;
				}
				else if (prime_free != 1)
				{
					target->z = right - 512;

					if (prime_free != 15)
						return SECONDARY_TARGET;

					prime_free = 31;
				}
			}
			else if (item->pos.z_pos > box_right)
			{
				if (prime_free & 2 && item->pos.x_pos >= box_top && item->pos.x_pos <= box_bottom)
				{
					if (target->z > box_right - 512)
						target->z = box_right - 512;

					if (prime_free & 16)
						return SECONDARY_TARGET;

					if (box_top > top)
						top = box_top;

					if (box_bottom < bottom)
						bottom = box_bottom;

					prime_free = 2;
				}
				else if (prime_free != 2)
				{
					target->z = left + 512;

					if (prime_free != 15)
						return SECONDARY_TARGET;

					prime_free = 31;
				}
			}

			if (item->pos.x_pos < box_top)
			{
				if (prime_free & 4 && item->pos.z_pos >= box_left && item->pos.z_pos <= box_right)
				{
					if (target->x < box_top + 512)
						target->x = box_top + 512;

					if (prime_free & 16)
						return SECONDARY_TARGET;

					if (box_left > left)
						left = box_left;

					if (box_right < right)
						right = box_right;

					prime_free = 4;
				}
				else if (prime_free != 4)
				{
					target->x = bottom - 512;

					if (prime_free != 15)
						return SECONDARY_TARGET;

					prime_free = 31;
				}
			}
			else if (item->pos.x_pos > box_bottom)
			{
				if (prime_free & 8 && item->pos.z_pos >= box_left && item->pos.z_pos <= box_right)
				{
					if (target->x > box_bottom - 512)
						target->x = box_bottom - 512;

					if (prime_free & 16)
						return SECONDARY_TARGET;

					if (box_left > left)
						left = box_left;

					if (box_right < right)
						right = box_right;

					prime_free = 8;
				}
				else if (prime_free != 8)
				{
					target->x = top + 512;

					if (prime_free != 15)
						return SECONDARY_TARGET;

					prime_free = 31;
				}
			}
		}

		if (box_number == LOT->target_box)
		{
			if (prime_free & 3)
				target->z = LOT->target.z;
			else if (!(prime_free & 16))
			{
				if (target->z < box_left + 512)
					target->z = box_left + 512;
				else if (target->z > box_right - 512)
					target->z = box_right - 512;
			}

			if (prime_free & 12)
				target->x = LOT->target.x;
			else if (!(prime_free & 16))
			{
				if (target->x < box_top + 512)
					target->x = box_top + 512;
				else if (target->x > box_bottom - 512)
					target->x = box_bottom - 512;
			}

			target->y = LOT->target.y;
			return PRIME_TARGET;
		}

		box_number = LOT->node[box_number].exit_box;

		if (box_number != 2047 && boxes[box_number].overlap_index & LOT->block_mask)
			break;

	} while (box_number != 2047);

	if (prime_free & 3)
		target->z = (((box_right - box_left - WALL_SIZE) * GetRandomControl()) >> 15) + box_left + 512;
	else if (!(prime_free & 16))
	{
		if (target->z < box_left + 512)
			target->z = box_left + 512;
		else if (target->z > box_right - 512)
			target->z = box_right - 521;
	}

	if (prime_free & 12)
		target->x = (((box_bottom - box_top - WALL_SIZE) * GetRandomControl()) >> 15) + box_top + 512;
	else if (!(prime_free & 16))
	{
		if (target->x < box_top + 512)
			target->x = box_top + 512;
		else if (target->x > box_bottom - 512)
			target->x = box_bottom - 512;
	}

	if (LOT->fly)
		target->y = box->height - 384;
	else
		target->y = box->height;

	return NO_TARGET;
}

void CreatureMood(ITEM_INFO* item, AI_INFO* info, long violent)
{
	CREATURE_INFO* creature;
	ITEM_INFO* enemy;
	LOT_INFO* LOT;
	short box_number;

	creature = (CREATURE_INFO*)item->data;

	if (!creature)
		return;

	enemy = creature->enemy;
	LOT = &creature->LOT;

	switch (creature->mood)
	{
	case BORED_MOOD:
		box_number = LOT->node[(LOT->zone_count * GetRandomControl()) >> 15].box_number;

		if (ValidBox(item, info->zone_number, box_number))
		{
			if (StalkBox(item, enemy, box_number) && enemy->hit_points > 0 && creature->enemy)
			{
				TargetBox(LOT, box_number);
				creature->mood = BORED_MOOD;
			}
			else if (LOT->required_box == 2047)
				TargetBox(LOT, box_number);
		}

		break;

	case ATTACK_MOOD:
		LOT->target.x = enemy->pos.x_pos;
		LOT->target.y = enemy->pos.y_pos;
		LOT->target.z = enemy->pos.z_pos;
		LOT->required_box = enemy->box_number;

		if (LOT->fly && lara.water_status == LARA_ABOVEWATER)
			LOT->target.y += GetBestFrame(enemy)[2];

		break;

	case ESCAPE_MOOD:
		box_number = LOT->node[(LOT->zone_count * GetRandomControl()) >> 15].box_number;

		if (ValidBox(item, info->zone_number, box_number) && LOT->required_box == 2047)
		{
			if (EscapeBox(item, enemy, box_number))
				TargetBox(LOT, box_number);
			else if (info->zone_number == info->enemy_zone && StalkBox(item, enemy, box_number) && !violent)
			{
				TargetBox(LOT, box_number);
				creature->mood = STALK_MOOD;
			}
		}

		break;

	case STALK_MOOD:

		if (LOT->required_box == 2047 || !StalkBox(item, enemy, LOT->required_box))
		{
			box_number = LOT->node[(LOT->zone_count * GetRandomControl()) >> 15].box_number;

			if (ValidBox(item, info->zone_number, box_number))
			{
				if (StalkBox(item, enemy, box_number))
					TargetBox(LOT, box_number);
				else if (LOT->required_box == 2047)
				{
					TargetBox(LOT, box_number);

					if (info->zone_number != info->enemy_zone)
						creature->mood = BORED_MOOD;
				}
			}
		}

		break;
	}

	if (LOT->target_box == 2047)
		TargetBox(LOT, item->box_number);

	CalculateTarget(&creature->target, item, LOT);
}

void GetCreatureMood(ITEM_INFO* item, AI_INFO* info, long violent)
{
	CREATURE_INFO* creature;
	ITEM_INFO* enemy;
	LOT_INFO* LOT;
	mood_type mood;

	creature = (CREATURE_INFO*)item->data;

	if (!creature)
		return;

	enemy = creature->enemy;
	LOT = &creature->LOT;

	if (LOT->node[item->box_number].search_number == (LOT->search_number | 0x8000))
		LOT->required_box = 2047;

	if (creature->mood != ATTACK_MOOD && LOT->required_box != 2047 && !ValidBox(item, info->zone_number, LOT->target_box))
	{
		if (info->zone_number == info->enemy_zone)
			creature->mood = BORED_MOOD;

		LOT->required_box = 2047;
	}

	mood = creature->mood;

	if (enemy)
	{
		if (enemy->hit_points <= 0 && enemy == lara_item)
			creature->mood = BORED_MOOD;
		else if (violent)
		{
			switch (creature->mood)
			{
			case BORED_MOOD:
			case STALK_MOOD:

				if (info->zone_number == info->enemy_zone)
					creature->mood = ATTACK_MOOD;
				else if (item->hit_status)
					creature->mood = ESCAPE_MOOD;

				break;

			case ATTACK_MOOD:

				if (info->zone_number != info->enemy_zone)
					creature->mood = BORED_MOOD;

				break;

			case ESCAPE_MOOD:

				if (info->zone_number == info->enemy_zone)
					creature->mood = ATTACK_MOOD;

				break;
			}
		}
		else
		{
			switch (creature->mood)
			{
			case BORED_MOOD:
			case STALK_MOOD:

				if (creature->alerted && info->zone_number != info->enemy_zone)
					creature->mood = info->distance > 3072 ? STALK_MOOD : BORED_MOOD;
				else if (info->zone_number == info->enemy_zone)
					creature->mood = (info->distance < 0x900000 || (creature->mood == STALK_MOOD && LOT->required_box == 2047)) ? ATTACK_MOOD : STALK_MOOD;

				break;

			case ATTACK_MOOD:

				if (item->hit_status && (GetRandomControl() < 2048 || info->zone_number != info->enemy_zone))
					creature->mood = STALK_MOOD;
				else if (info->zone_number != info->enemy_zone && info->distance > 6144)
					creature->mood = BORED_MOOD;

				break;

			case ESCAPE_MOOD:

				if (info->zone_number == info->enemy_zone && GetRandomControl() < 256)
					creature->mood = STALK_MOOD;

				break;
			}
		}
	}
	else
		creature->mood = BORED_MOOD;

	if (mood != creature->mood)
	{
		if (mood == ATTACK_MOOD)
			TargetBox(LOT, LOT->target_box);

		LOT->required_box = 2047;
	}
}

long BadFloor(long x, long y, long z, long box_height, long next_height, short room_number, LOT_INFO* LOT)
{
	FLOOR_INFO* floor;
	BOX_INFO* box;

	floor = GetFloor(x, y, z, &room_number);

	if (floor->box == 2047)
		return 1;

	box = &boxes[floor->box];

	if (LOT->block_mask & box->overlap_index)
		return 1;

	if (box_height - box->height > LOT->step || box_height - box->height < LOT->drop)
		return 1;

	if (box_height - box->height < -LOT->step && box->height > next_height)
		return 1;

	if (LOT->fly && y > LOT->fly + box->height)
		return 1;

	return 0;
}

long CreatureCreature(short item_number)
{
	ITEM_INFO* item;
	long x, z, dx, dz, dist;
	short yrot, rad, item_num;

	item = &items[item_number];
	x = item->pos.x_pos;
	z = item->pos.z_pos;
	yrot = item->pos.y_rot;
	rad = objects[item->object_number].radius;

	for (item_num = room[item->room_number].item_number; item_num != NO_ITEM; item_num = item->next_item)
	{
		item = &items[item_num];

		if (item_num == item_number)
			break;

		if (item != lara_item && item->status == ITEM_ACTIVE && item->hit_points > 0)
		{
			dx = abs(item->pos.x_pos - x);
			dz = abs(item->pos.z_pos - z);
			dist = dx > dz ? dx + (dz >> 1) : dz + (dx >> 1);

			if (dist < rad + objects[item->object_number].radius)
				return short(phd_atan(item->pos.z_pos - z, item->pos.x_pos - x) - yrot);
		}
	}

	return 0;
}

void CreatureDie(short item_number, long explode)
{
	ITEM_INFO* item;
	ITEM_INFO* pickup;
	FLOOR_INFO* floor;
	short pickup_number, room_number;

	item = &items[item_number];
	item->hit_points = DONT_TARGET;
	item->collidable = 0;

	if (explode)
	{
		ExplodingDeath(item_number, -1, 0);
		KillItem(item_number);
	}
	else
		RemoveActiveItem(item_number);

	DisableBaddieAI(item_number);
	item->flags |= IFL_INVISIBLE;

	if (item->clear_body)
	{
		item->next_active = body_bag;
		body_bag = item_number;
	}

	pickup_number = item->carried_item;

	while (pickup_number != NO_ITEM)
	{
		pickup = &items[pickup_number];
		pickup->pos.x_pos = (item->pos.x_pos & -512) | 512;
		pickup->pos.z_pos = (item->pos.z_pos & -512) | 512;
		room_number = item->room_number;
		floor = GetFloor(pickup->pos.x_pos, item->pos.y_pos, pickup->pos.z_pos, &room_number);
		pickup->pos.y_pos = GetHeight(floor, pickup->pos.x_pos, item->pos.y_pos, pickup->pos.z_pos);
		ItemNewRoom(pickup_number, item->room_number);
		pickup_number = pickup->carried_item;
	}
}

short CreatureTurn(ITEM_INFO* item, short maximum_turn)
{
	CREATURE_INFO* creature;
	long x, z;
	short angle;

	creature = (CREATURE_INFO*)item->data;

	if (!creature || !maximum_turn)
		return 0;

	x = creature->target.x - item->pos.x_pos;
	z = creature->target.z - item->pos.z_pos;
	angle = short(phd_atan(z, x) - item->pos.y_rot);

	if (angle > 0x4000 || angle < -0x4000)
	{
		if (SQUARE(x) + SQUARE(z) < SQUARE((item->speed << 14) / maximum_turn))
			maximum_turn >>= 1;
	}

	if (angle > maximum_turn)
		angle = maximum_turn;
	else if (angle < -maximum_turn)
		angle = -maximum_turn;

	item->pos.y_rot += angle;
	return angle;
}

long CreatureAnimation(short item_number, short angle, short tilt)
{
	ITEM_INFO* item;
	CREATURE_INFO* creature;
	LOT_INFO* LOT;
	FLOOR_INFO* floor;
	PHD_VECTOR oldPos;
	short* zone;
	short* bounds;
	long box_height, y, height, next_box, next_height, x, z, wx, wz, xShift, zShift, dy, top;
	short room_number, rad;

	item = &items[item_number];
	creature = (CREATURE_INFO*)item->data;

	if (!creature)
		return 0;

	LOT = &creature->LOT;
	oldPos.x = item->pos.x_pos;
	oldPos.y = item->pos.y_pos;
	oldPos.z = item->pos.z_pos;
	height = boxes[item->box_number].height;

	if (LOT->fly)
		zone = ground_zone[-1][0];
	else
		zone = ground_zone[(LOT->step >> 8) - 1][flip_status];

	if (!objects[item->object_number].water_creature)
	{
		room_number = item->room_number;
		GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);

		if (item->room_number != room_number)
			ItemNewRoom(item_number, room_number);
	}

	AnimateItem(item);

	if (item->status == ITEM_DEACTIVATED)
	{
		CreatureDie(item_number, 0);
		return 0;
	}

	bounds = GetBoundsAccurate(item);
	y = item->pos.y_pos + bounds[2];
	room_number = item->room_number;
	GetFloor(oldPos.x, y, oldPos.z, &room_number);
	floor = GetFloor(item->pos.x_pos, y, item->pos.z_pos, &room_number);
	box_height = boxes[floor->box].height;

	if (objects[item->object_number].non_lot)
	{
		floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
		box_height = boxes[floor->box].height;
		next_box = floor->box;
	}
	else
		next_box = LOT->node[floor->box].exit_box;

	if (next_box == 2047)
		next_height = box_height;
	else
		next_height = boxes[next_box].height;

	if (floor->box == 2047 || (!LOT->fly && zone[item->box_number] != zone[floor->box]) || height - box_height > LOT->step || height - box_height < LOT->drop)
	{
		wx = item->pos.x_pos >> WALL_SHIFT;
		wz = item->pos.z_pos >> WALL_SHIFT;
		xShift = oldPos.x >> WALL_SHIFT;
		zShift = oldPos.z >> WALL_SHIFT;

		if (wx < xShift)
			item->pos.x_pos = oldPos.x & ~WALL_MASK;
		else if (wx > xShift)
			item->pos.x_pos = oldPos.x | WALL_MASK;

		if (wz < zShift)
			item->pos.z_pos = oldPos.z & ~WALL_MASK;
		else if (wz > zShift)
			item->pos.z_pos = oldPos.z | WALL_MASK;

		floor = GetFloor(item->pos.x_pos, y, item->pos.z_pos, &room_number);
		box_height = boxes[floor->box].height;

		if (objects[item->object_number].non_lot)
		{
			floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
			box_height = boxes[floor->box].height;
			next_box = floor->box;
		}
		else
			next_box = LOT->node[floor->box].exit_box;

		if (next_box == 2047)
			next_height = box_height;
		else
			next_height = boxes[next_box].height;
	}

	x = item->pos.x_pos;
	z = item->pos.z_pos;
	wx = x & WALL_MASK;
	wz = z & WALL_MASK;
	rad = objects[item->object_number].radius;
	xShift = 0;
	zShift = 0;

	if (wz < rad)
	{
		if (BadFloor(x, y, z - rad, box_height, next_height, room_number, LOT))
			zShift = rad - wz;

		if (wx < rad)
		{
			if (!BadFloor(x - rad, y, z, box_height, next_height, room_number, LOT))
			{
				if (!zShift && BadFloor(x - rad, y, z - rad, box_height, next_height, room_number, LOT))
				{
					if (item->pos.y_rot > -0x6000 && item->pos.y_rot < 0x2000)
						zShift = rad - wz;
					else
						xShift = rad - wx;
				}
			}
			else
				xShift = rad - wx;
		}
		else if (wx > WALL_SIZE - rad)
		{
			if (!BadFloor(x + rad, y, z, box_height, next_height, room_number, LOT))
			{
				if (!zShift && BadFloor(x + rad, y, z - rad, box_height, next_height, room_number, LOT))
				{
					if (item->pos.y_rot > -0x2000 && item->pos.y_rot < 0x6000)
						zShift = rad - wz;
					else
						xShift = WALL_SIZE - rad - wx;
				}
			}
			else
				xShift = WALL_SIZE - rad - wx;
		}
	}
	else if (wz > WALL_SIZE - rad)
	{
		if (BadFloor(x, y, z + rad, box_height, next_height, room_number, LOT))
			zShift = WALL_SIZE - rad - wz;

		if (wx < rad)
		{
			if (!BadFloor(x - rad, y, z, box_height, next_height, room_number, LOT))
			{
				if (!zShift && BadFloor(x - rad, y, z + rad, box_height, next_height, room_number, LOT))
				{
					if (item->pos.y_rot > -0x2000 && item->pos.y_rot < 0x6000)
						xShift = rad - wx;
					else
						zShift = WALL_SIZE - rad - wz;
				}
			}
			else
				xShift = rad - wx;
		}
		else if (wx > WALL_SIZE - rad)
		{
			if (!BadFloor(x + rad, y, z, box_height, next_height, room_number, LOT))
			{
				if (!zShift && BadFloor(x + rad, y, z + rad, box_height, next_height, room_number, LOT))
				{
					if (item->pos.y_rot > -0x6000 && item->pos.y_rot < 0x2000)
						xShift = WALL_SIZE - rad - wx;
					else
						zShift = WALL_SIZE - rad - wz;
				}
			}
			else
				xShift = WALL_SIZE - rad - wx;
		}
	}
	else if (wx < rad)
	{
		if (BadFloor(x - rad, y, z, box_height, next_height, room_number, LOT))
			xShift = rad - wx;
	}
	else if (wx > WALL_SIZE - rad)
	{
		if (BadFloor(x + rad, y, z, box_height, next_height, room_number, LOT))
			xShift = WALL_SIZE - rad - wx;
	}

	item->pos.x_pos += xShift;
	item->pos.z_pos += zShift;

	if (xShift || zShift)
	{
		floor = GetFloor(item->pos.x_pos, y, item->pos.z_pos, &room_number);
		item->pos.y_rot += angle;

		if (tilt)
			CreatureTilt(item, 2 * tilt);
	}

	if (item->object_number != TREX && item->speed && item->hit_points > 0)
		angle = (short)CreatureCreature(item_number);
	else
		angle = 0;

	if (angle)
	{
		if (abs(angle) < 2048)
			item->pos.y_rot -= angle;
		else if (angle > 0)
			item->pos.y_rot -= 2048;
		else
			item->pos.y_rot += 2048;

		return 1;
	}

	if (LOT->fly)
	{
		dy = creature->target.y - item->pos.y_pos;

		if (dy > LOT->fly)
			dy = LOT->fly;
		else if (dy < -LOT->fly)
			dy = -LOT->fly;

		height = GetHeight(floor, item->pos.x_pos, y, item->pos.z_pos);

		if (item->pos.y_pos + dy > height)
		{
			if (item->pos.y_pos > height)
			{
				dy = -LOT->fly;
				item->pos.x_pos = oldPos.x;
				item->pos.z_pos = oldPos.z;
			}
			else
			{
				dy = 0;
				item->pos.y_pos = height;
			}
		}
		else if (objects[item->object_number].water_creature)
		{
			height = GetCeiling(floor, item->pos.x_pos, y, item->pos.z_pos);

			if (item->object_number == WHALE)
				top = 128;
			else
				top = bounds[2];

			if (item->pos.y_pos + top + dy < height)
			{
				if (item->pos.y_pos + top < height)
				{
					dy = LOT->fly;
					item->pos.x_pos = oldPos.x;
					item->pos.z_pos = oldPos.z;
				}
				else
					dy = 0;
			}
		}
		else
		{
			GetFloor(item->pos.x_pos, y + 256, item->pos.z_pos, &room_number);

			if (room[room_number].flags & (ROOM_UNDERWATER | ROOM_SWAMP))
				dy = -LOT->fly;
		}

		item->pos.y_pos += dy;
		floor = GetFloor(item->pos.x_pos, y, item->pos.z_pos, &room_number);
		item->floor = GetHeight(floor, item->pos.x_pos, y, item->pos.z_pos);

		if (item->speed)
			angle = (short)phd_atan(item->speed, -dy);
		else
			angle = 0;

		if (angle < -3640)
			angle = -3640;
		else if (angle > 3640)
			angle = 3640;

		if (angle < item->pos.x_rot - 182)
			item->pos.x_rot -= 182;
		else  if (angle > item->pos.x_rot + 182)
			item->pos.x_rot += 182;
		else
			item->pos.x_rot = angle;
	}
	else
	{
		floor = GetFloor(item->pos.x_pos, y, item->pos.z_pos, &room_number);
		height = GetCeiling(floor, item->pos.x_pos, y, item->pos.z_pos);

		if (item->object_number == TREX || item->object_number == SHIVA || item->object_number == MUTANT2)
			top = 768;
		else
			top = bounds[2];

		if (item->pos.y_pos + top < height)
		{
			item->pos.x_pos = oldPos.x;
			item->pos.z_pos = oldPos.z;
			item->pos.y_pos = oldPos.y;
		}

		floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
		item->floor = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

		if (item->pos.y_pos > item->floor)
			item->pos.y_pos = item->floor;
		else if (item->floor - item->pos.y_pos > 64)
			item->pos.y_pos += 64;
		else if (item->pos.y_pos < item->floor)
			item->pos.y_pos = item->floor;

		item->pos.x_rot = 0;
	}

	if (!objects[item->object_number].water_creature)
	{
		GetFloor(item->pos.x_pos, item->pos.y_pos - 512, item->pos.z_pos, &room_number);

		if (room[room_number].flags & ROOM_UNDERWATER)
			item->hit_points = 0;
	}

	if (item->room_number != room_number)
		ItemNewRoom(item_number, room_number);

	return 1;
}

void CreatureTilt(ITEM_INFO* item, short angle)
{
	angle = (angle << 2) - item->pos.z_rot;

	if (angle < -546)
		item->pos.z_rot -= 546;
	else if (angle > 546)
		item->pos.z_rot += 546;
}

void CreatureJoint(ITEM_INFO* item, short joint, short required)
{
	CREATURE_INFO* creature;
	short change;

	creature = (CREATURE_INFO*)item->data;

	if (!creature)
		return;

	change = required - creature->joint_rotation[joint];

	if (change > 910)
		change = 910;
	else if (change < -910)
		change = -910;

	creature->joint_rotation[joint] += change;

	if (creature->joint_rotation[joint] > 0x3000)
		creature->joint_rotation[joint] = 0x3000;
	else if (creature->joint_rotation[joint] < -0x3000)
		creature->joint_rotation[joint] = -0x3000;
}

void CreatureFloat(short item_number)
{
	ITEM_INFO* item;
	FLOOR_INFO* floor;
	long water_level;
	short room_number;

	item = &items[item_number];
	item->hit_points = DONT_TARGET;
	item->pos.x_rot = 0;
	water_level = GetWaterHeight(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number);

	if (item->pos.y_pos > water_level)
		item->pos.y_pos -= 32;

	if (item->pos.y_pos < water_level)
		item->pos.y_pos = water_level;

	AnimateItem(item);
	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	item->floor = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

	if (item->room_number != room_number)
		ItemNewRoom(item_number, room_number);
}

void CreatureUnderwater(ITEM_INFO* item, long depth)
{
	depth += GetWaterHeight(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number);

	if (item->pos.y_pos < depth)
	{
		item->pos.y_pos = depth;

		if (item->pos.x_rot > 364)
			item->pos.x_rot -= 364;
		else if (item->pos.x_rot > 0)
			item->pos.x_rot = 0;
	}
}

short CreatureEffect(ITEM_INFO* item, BITE_INFO* bite, short(*generate)(long x, long y, long z, short speed, short yrot, short room_number))
{
	PHD_VECTOR pos;

	pos.x = bite->x;
	pos.y = bite->y;
	pos.z = bite->z;
	GetJointAbsPosition(item, &pos, bite->mesh_num);
	return generate(pos.x, pos.y, pos.z, item->speed, item->pos.y_rot, item->room_number);
}

long CreatureVault(short item_number, short angle, long vault, long shift)
{
	ITEM_INFO* item;
	long x, y, z, xFloor, zFloor;
	short room_number;

	item = &items[item_number];
	x = item->pos.x_pos >> WALL_SHIFT;
	y = item->pos.y_pos;
	z = item->pos.z_pos >> WALL_SHIFT;
	room_number = item->room_number;
	CreatureAnimation(item_number, angle, 0);

	if (item->floor > y + 896)
		vault = -4;
	else if (item->floor > y + 640 && item->object_number == MONKEY)
		vault = -3;
	else if (item->floor > y + 384 && item->object_number == MONKEY)
		vault = -2;
	else
	{
		if (item->pos.y_pos > y - 384)
			return 0;

		if (item->pos.y_pos > y - 640)
			vault = 2;
		else if (item->pos.y_pos > y - 896)
			vault = 3;
		else
			vault = 4;
	}

	xFloor = item->pos.x_pos >> WALL_SHIFT;
	zFloor = item->pos.z_pos >> WALL_SHIFT;

	if (z == zFloor)
	{
		if (x == xFloor)
			return 0;

		if (x >= xFloor)
		{
			item->pos.y_rot = -0x4000;
			item->pos.x_pos = (x << WALL_SHIFT) + shift;
		}
		else
		{
			item->pos.y_rot = 0x4000;
			item->pos.x_pos = (xFloor << WALL_SHIFT) - shift;
		}
	}
	else if (x == xFloor)
	{
		if (z < zFloor)
		{
			item->pos.y_rot = 0;
			item->pos.z_pos = (zFloor << WALL_SHIFT) - shift;
		}
		else
		{
			item->pos.y_rot = -0x8000;
			item->pos.z_pos = (z << WALL_SHIFT) + shift;
		}
	}

	item->floor = y;
	item->pos.y_pos = y;

	if (item->room_number != room_number)
		ItemNewRoom(item_number, room_number);

	return vault;
}

void CreatureKill(ITEM_INFO* item, long kill_anim, long kill_state, long lara_kill_state)
{
	item->anim_number = objects[item->object_number].anim_index + (short)kill_anim;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = (short)kill_state;
	lara_item->anim_number = objects[LARA_EXTRA].anim_index;
	lara_item->frame_number = anims[lara_item->anim_number].frame_base;
	lara_item->current_anim_state = 8;
	lara_item->goal_anim_state = (short)lara_kill_state;
	lara_item->pos.x_pos = item->pos.x_pos;
	lara_item->pos.y_pos = item->pos.y_pos;
	lara_item->pos.z_pos = item->pos.z_pos;
	lara_item->pos.x_rot = item->pos.x_rot;
	lara_item->pos.y_rot = item->pos.y_rot;
	lara_item->pos.z_rot = item->pos.z_rot;
	lara_item->fallspeed = 0;
	lara_item->gravity_status = 0;
	lara_item->speed = 0;

	if (lara_item->room_number != item->room_number)
		ItemNewRoom(lara.item_number, item->room_number);

	AnimateItem(lara_item);
	lara.extra_anim = 1;
	lara.gun_status = LG_HANDSBUSY;
	lara.gun_type = LG_UNARMED;
	lara.hit_direction = -1;
	lara.air = -1;
	camera.type = CHASE_CAMERA;
	camera.pos.room_number = lara_item->room_number;
	camera.flags = 1;
	camera.target_angle = 0x78DC;
	camera.target_elevation = -4550;
}

short AIGuard(CREATURE_INFO* creature)
{
	long rnd;

	if (items[creature->item_num].ai_bits & MODIFY)
		return 0;

	rnd = GetRandomControl();

	if (rnd < 256)
	{
		creature->head_left = 1;
		creature->head_right = 1;
	}
	else if (rnd < 384)
	{
		creature->head_left = 1;
		creature->head_right = 0;
	}
	else if (rnd < 512)
	{
		creature->head_left = 0;
		creature->head_right = 1;
	}

	if (creature->head_left && creature->head_right)
		return 0;

	if (creature->head_left)
		return -0x4000;

	if (creature->head_right)
		return 0x4000;

	return 0;
}

void AlertAllGuards(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* target;
	CREATURE_INFO* creature;

	item = &items[item_number];

	for (int i = 0; i < MAX_LOT; i++)
	{
		creature = &baddie_slots[i];

		if (creature->item_num != NO_ITEM)
		{
			target = &items[creature->item_num];

			if (target->object_number == item->object_number && target->status == ITEM_ACTIVE)
				creature->alerted = 1;
		}
	}
}

short SameZone(CREATURE_INFO* creature, ITEM_INFO* target_item)
{
	ITEM_INFO* item;
	ROOM_INFO* r;
	short* zone;

	if (creature->LOT.fly)
		return 1;

	zone = ground_zone[(creature->LOT.step >> 8) - 1][flip_status];
	item = &items[creature->item_num];

	r = &room[item->room_number];
	item->box_number = r->floor[((item->pos.z_pos - r->z) >> WALL_SHIFT) + r->x_size * ((item->pos.x_pos - r->x) >> WALL_SHIFT)].box;

	r = &room[target_item->room_number];
	target_item->box_number = r->floor[((target_item->pos.z_pos - r->z) >> WALL_SHIFT) + r->x_size * ((target_item->pos.x_pos - r->x) >> WALL_SHIFT)].box;

	return zone[item->box_number] == zone[target_item->box_number];
}

void GetAITarget(CREATURE_INFO* creature)
{
	ITEM_INFO* item;
	ITEM_INFO* enemy;
	ITEM_INFO* target;
	FLOOR_INFO* floor;
	long lp;
	short enemy_object;
	char ai_bits;

	enemy = creature->enemy;

	if (enemy)
		enemy_object = enemy->object_number;
	else
		enemy_object = NO_ITEM;

	item = &items[creature->item_num];
	ai_bits = item->ai_bits;

	if (ai_bits & GUARD)
	{
		creature->enemy = lara_item;

		if (creature->alerted)
		{
			item->ai_bits &= ~GUARD;

			if (ai_bits & AMBUSH)
				item->ai_bits |= MODIFY;
		}
	}
	else if (ai_bits & PATROL1)
	{
		if (creature->alerted || creature->hurt_by_lara)
		{
			item->ai_bits &= ~PATROL1;

			if (ai_bits & AMBUSH)
				item->ai_bits |= MODIFY;
		}
		else if (!creature->patrol2 && enemy_object != AI_PATROL1)
		{
			for (lp = 0; lp < level_items; lp++)
			{
				target = &items[lp];

				if (target->object_number == AI_PATROL1 && target->room_number != NO_ROOM &&
					SameZone(creature, target) && target->pos.y_rot == item->item_flags[3])
				{
					creature->enemy = target;
					return;
				}
			}
		}
		else if (creature->patrol2 && enemy_object != AI_PATROL2)
		{
			for (lp = 0; lp < level_items; lp++)
			{
				target = &items[lp];

				if (target->object_number == AI_PATROL2 && target->room_number != NO_ROOM &&
					SameZone(creature, target) && target->pos.y_rot == item->item_flags[3])
				{
					creature->enemy = target;
					return;
				}
			}
		}
		else if (abs(enemy->pos.x_pos - item->pos.x_pos) < 768 && abs(enemy->pos.y_pos - item->pos.y_pos) < 768 && abs(enemy->pos.z_pos - item->pos.z_pos) < 768)
		{
			floor = GetFloor(enemy->pos.x_pos, enemy->pos.y_pos, enemy->pos.z_pos, &enemy->room_number);
			GetHeight(floor, enemy->pos.x_pos, enemy->pos.y_pos, enemy->pos.z_pos);
			TestTriggers(trigger_index, 1);
			creature->patrol2 = ~creature->patrol2;
		}
	}
	else if (ai_bits & AMBUSH)
	{
		if (ai_bits & MODIFY || creature->hurt_by_lara)
		{
			if (enemy_object != AI_AMBUSH)
			{
				for (lp = 0; lp < level_items; lp++)
				{
					target = &items[lp];

					if (target->object_number == AI_AMBUSH && target->room_number != NO_ROOM && SameZone(creature, target) &&
						(target->pos.y_rot == item->item_flags[3] || item->object_number == MONKEY))
					{
						creature->enemy = target;
						return;
					}
				}
			}
			else if (item->object_number != MONKEY)
			{
				if (abs(enemy->pos.x_pos - item->pos.x_pos) < 768 && abs(enemy->pos.y_pos - item->pos.y_pos) < 768 && abs(enemy->pos.z_pos - item->pos.z_pos) < 768)
				{
					floor = GetFloor(enemy->pos.x_pos, enemy->pos.y_pos, enemy->pos.z_pos, &enemy->room_number);
					GetHeight(floor, enemy->pos.x_pos, enemy->pos.y_pos, enemy->pos.z_pos);
					TestTriggers(trigger_index, 1);
					creature->reached_goal = 1;
					creature->enemy = lara_item;
					item->ai_bits &= ~(AMBUSH | MODIFY);
					item->ai_bits |= GUARD;
					creature->alerted = 0;
				}
			}
		}
		else
			creature->enemy = lara_item;
	}
	else if (ai_bits & FOLLOW)
	{
		if (creature->hurt_by_lara)
		{
			creature->enemy = lara_item;
			creature->alerted = 1;
			item->ai_bits &= ~FOLLOW;
		}
		else if (item->hit_status)
			item->ai_bits &= ~FOLLOW;
		else if (enemy_object != AI_FOLLOW)
		{
			for (lp = 0; lp < level_items; lp++)
			{
				target = &items[lp];

				if (target->object_number == AI_FOLLOW && target->room_number != NO_ROOM &&
					SameZone(creature, target) && target->pos.y_rot == item->item_flags[3])
				{
					creature->enemy = target;
					return;
				}
			}
		}
		else if (abs(enemy->pos.x_pos - item->pos.x_pos) < 768 && abs(enemy->pos.y_pos - item->pos.y_pos) < 768 && abs(enemy->pos.z_pos - item->pos.z_pos) < 768)
		{
			creature->reached_goal = 1;
			item->ai_bits &= ~FOLLOW;
		}
	}
	else if (item->object_number == MONKEY && item->carried_item == NO_ITEM)
	{
		if (item->ai_bits == MODIFY)
		{
			if (enemy_object != KEY_ITEM4)
			{
				for (lp = 0; lp < level_items; lp++)
				{
					target = &items[lp];

					if (target->object_number == KEY_ITEM4 && target->room_number != NO_ROOM && !target->ai_bits &&
						target->status != ITEM_INVISIBLE && !(target->flags & IFL_CLEARBODY) && SameZone(creature, target))
					{
						creature->enemy = target;
						return;
					}
				}
			}
		}
		else if (enemy_object != MEDI_ITEM)
		{
			for (lp = 0; lp < level_items; lp++)
			{
				target = &items[lp];

				if (target->object_number == MEDI_ITEM && target->room_number != NO_ROOM && !target->ai_bits &&
					target->status != ITEM_INVISIBLE && !(target->flags & IFL_CLEARBODY) && SameZone(creature, target))
				{
					creature->enemy = target;
					return;
				}
			}
		}
	}
}

void AdjustStopperFlag(ITEM_INFO* item, long dir, long set)
{
	ROOM_INFO* r;
	long x, z;
	short room_number;

	x = item->pos.x_pos;
	z = item->pos.z_pos;
	r = &room[item->room_number];
	r->floor[((z - r->z) >> WALL_SHIFT) + r->x_size * ((x - r->x) >> WALL_SHIFT)].stopper = set;

	x = item->pos.x_pos + (WALL_SIZE * phd_sin(dir) >> W2V_SHIFT);
	z = item->pos.z_pos + (WALL_SIZE * phd_cos(dir) >> W2V_SHIFT);
	room_number = item->room_number;
	GetFloor(x, item->pos.y_pos, z, &room_number);
	r = &room[room_number];
	r->floor[((z - r->z) >> WALL_SHIFT) + r->x_size * ((x - r->x) >> WALL_SHIFT)].stopper = set;
}
