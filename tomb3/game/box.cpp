#include "../tomb3/pch.h"
#include "box.h"
#include "objects.h"
#include "../specific/game.h"
#include "lot.h"
#include "../3dsystem/phd_math.h"
#include "lara.h"
#include "draw.h"

void AlertNearbyGuards(ITEM_INFO* item)
{
	ITEM_INFO* target;
	CREATURE_INFO* creature;
	long dx, dy, dz, dist;

	for (int i = 0; i < 5; i++)
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

void inject_box(bool replace)
{
	INJECT(0x00416A30, AlertNearbyGuards, replace);
	INJECT(0x004142E0, InitialiseCreature, replace);
	INJECT(0x00414330, CreatureActive, replace);
	INJECT(0x00414390, CreatureAIInfo, replace);
	INJECT(0x00414800, SearchLOT, replace);
	INJECT(0x00414780, UpdateLOT, replace);
	INJECT(0x00414A10, TargetBox, replace);
	INJECT(0x00414AB0, EscapeBox, replace);
	INJECT(0x00414B60, ValidBox, replace);
	INJECT(0x004150C0, StalkBox, replace);
	INJECT(0x004151C0, CalculateTarget, replace);
	INJECT(0x00414E50, CreatureMood, replace);
}
