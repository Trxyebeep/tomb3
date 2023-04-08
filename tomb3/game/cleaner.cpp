#include "../tomb3/pch.h"
#include "cleaner.h"
#include "effect2.h"
#include "../specific/game.h"
#include "box.h"
#include "sound.h"
#include "control.h"
#include "items.h"
#include "sphere.h"
#include "lara.h"
#include "../3dsystem/phd_math.h"

static void TriggerElectricSparks(PHD_VECTOR* pos, short item_number, short Node)
{
	SPARKS* sptr;
	long dx, dz;

	dx = lara_item->pos.x_pos - pos->x;
	dz = lara_item->pos.z_pos - pos->z;

	if (dx < -0x5000 || dx > 0x5000 || dz < -0x5000 || dz > 0x5000)
		return;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = (GetRandomControl() & 0x3F) + 192;
	sptr->sG = sptr->sR;
	sptr->sB = sptr->sR;
	sptr->dR = sptr->sB >> 2;
	sptr->dG = sptr->sB >> 1;
	sptr->dB = (GetRandomControl() & 0x3F) + 192;
	sptr->ColFadeSpeed = 8;
	sptr->FadeToBlack = 8;
	sptr->TransType = 2;
	sptr->Dynamic = -1;
	sptr->Life = (GetRandomControl() & 7) + 20;
	sptr->sLife = sptr->Life;
	sptr->x = (GetRandomControl() & 0x1F) - 16;
	sptr->y = (GetRandomControl() & 0x1F) - 16;
	sptr->z = (GetRandomControl() & 0x1F) - 16;
	sptr->Xvel = ((GetRandomControl() & 0xFF) << 2) - 512;
	sptr->Yvel = (GetRandomControl() & 7) - 4;
	sptr->Zvel = ((GetRandomControl() & 0xFF) << 2) - 512;
	sptr->Friction = 4;
	sptr->Flags = SF_ATTACHEDNODE | SF_ITEM | SF_SCALE;
	sptr->FxObj = (uchar)item_number;
	sptr->NodeNumber = (uchar)Node;
	sptr->Scalar = 1;
	sptr->Width = (GetRandomControl() & 3) + 4;
	sptr->sWidth = sptr->Width;
	sptr->dWidth = sptr->Width >> 1;
	sptr->Height = sptr->Width;
	sptr->sHeight = sptr->Height;
	sptr->dHeight = sptr->Height >> 1;
	sptr->MaxYvel = 0;
	sptr->Gravity = (GetRandomControl() & 3) + 4;
}

void InitialiseCleaner(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	item->pos.x_pos = (item->pos.x_pos & ~WALL_MASK) | 512;
	item->pos.z_pos = (item->pos.z_pos & ~WALL_MASK) | 512;
	item->item_flags[0] = 1024;
	item->item_flags[1] = 0;
	item->item_flags[2] = 64;
	item->data = 0;
	item->collidable = 1;
}

void CleanerControl(short item_number)
{
	ITEM_INFO* item;
	FLOOR_INFO* floor;
	ROOM_INFO* r;
	PHD_VECTOR pos;
	long c, x, z, h, left, ahead;
	short room_number;
	static char elecspark[3];

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];

	if (!item->item_flags[2])
		return;

	if (item->pos.y_rot & 0x3FFF)	//turn
		item->pos.y_rot += item->item_flags[0];
	else if (((item->pos.z_pos & WALL_MASK) == 512 && (!item->pos.y_rot || item->pos.y_rot == -0x8000)) ||		//middle of a block, decide where to go next
		((item->pos.x_pos & WALL_MASK) == 512 && (item->pos.y_rot == 0x4000 || item->pos.y_rot == -0x4000)))
	{
		if (item->item_flags[1] == 1)
		{
			x = item->pos.x_pos + (WALL_SIZE * phd_sin(item->pos.y_rot + 0x8000) >> W2V_SHIFT);
			z = item->pos.z_pos + (WALL_SIZE * phd_cos(item->pos.y_rot + 0x8000) >> W2V_SHIFT);
			room_number = item->room_number;
			GetFloor(x, item->pos.y_pos, z, &room_number);
			r = &room[room_number];
			r->floor[((z - r->z) >> WALL_SHIFT) + r->x_size * ((x - r->x) >> WALL_SHIFT)].stopper = 0;
			item->item_flags[1] = 0;
		}

		switch (item->pos.y_rot)
		{
		case 0:
			room_number = item->room_number;
			floor = GetFloor(item->pos.x_pos - WALL_SIZE, item->pos.y_pos, item->pos.z_pos, &room_number);
			h = GetHeight(floor, item->pos.x_pos - WALL_SIZE, item->pos.y_pos, item->pos.z_pos);
			r = &room[room_number];
			floor = &r->floor[((item->pos.z_pos - r->z) >> WALL_SHIFT) + r->x_size * ((item->pos.x_pos - WALL_SIZE - r->x) >> WALL_SHIFT)];
			left = h == item->pos.y_pos && !floor->stopper;

			room_number = item->room_number;
			floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos + WALL_SIZE, &room_number);
			h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos + WALL_SIZE);
			r = &room[room_number];
			floor = &r->floor[((item->pos.z_pos + WALL_SIZE - r->z) >> WALL_SHIFT) + r->x_size * ((item->pos.x_pos - r->x) >> WALL_SHIFT)];
			ahead = h == item->pos.y_pos && !floor->stopper;

			if (!ahead && !left && item->item_flags[0] > 0)
			{
				item->pos.y_rot += 1024;
				item->item_flags[0] = 1024;
			}
			else if (!ahead && !left && item->item_flags[0] < 0)
			{
				item->pos.y_rot -= 1024;
				item->item_flags[0] = -1024;
			}
			else if (left && item->item_flags[0] > 0)
			{
				item->pos.y_rot -= 1024;
				item->item_flags[0] = -1024;
			}
			else
			{
				item->item_flags[0] = 1024;
				item->item_flags[1] = 1;
				item->pos.z_pos += item->item_flags[2];
				r->floor[((item->pos.z_pos + WALL_SIZE - r->z) >> WALL_SHIFT) + r->x_size * ((item->pos.x_pos - r->x) >> WALL_SHIFT)].stopper = 1;
			}

			break;

		case 0x4000:
			room_number = item->room_number;
			floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos + WALL_SIZE, &room_number);
			h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos + WALL_SIZE);
			r = &room[room_number];
			floor = &r->floor[((item->pos.z_pos + WALL_SIZE - r->z) >> WALL_SHIFT) + r->x_size * ((item->pos.x_pos - r->x) >> WALL_SHIFT)];
			left = h == item->pos.y_pos && !floor->stopper;

			room_number = item->room_number;
			floor = GetFloor(item->pos.x_pos + WALL_SIZE, item->pos.y_pos, item->pos.z_pos, &room_number);
			h = GetHeight(floor, item->pos.x_pos + WALL_SIZE, item->pos.y_pos, item->pos.z_pos);
			r = &room[room_number];
			floor = &r->floor[((item->pos.z_pos - r->z) >> WALL_SHIFT) + r->x_size * ((item->pos.x_pos + WALL_SIZE - r->x) >> WALL_SHIFT)];
			ahead = h == item->pos.y_pos && !floor->stopper;

			if (!ahead && !left && item->item_flags[0] > 0)
			{
				item->pos.y_rot += 1024;
				item->item_flags[0] = 1024;
			}
			else if (!ahead && !left && item->item_flags[0] < 0)
			{
				item->pos.y_rot -= 1024;
				item->item_flags[0] = -1024;
			}
			else if (left && item->item_flags[0] > 0)
			{
				item->pos.y_rot -= 1024;
				item->item_flags[0] = -1024;
			}
			else
			{
				item->item_flags[0] = 1024;
				item->item_flags[1] = 1;
				item->pos.x_pos += item->item_flags[2];
				r->floor[((item->pos.z_pos - r->z) >> WALL_SHIFT) + r->x_size * ((item->pos.x_pos + WALL_SIZE - r->x) >> WALL_SHIFT)].stopper = 1;
			}

			break;

		case -0x4000:
			room_number = item->room_number;
			floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos - WALL_SIZE, &room_number);
			h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos - WALL_SIZE);
			r = &room[room_number];
			floor = &r->floor[((item->pos.z_pos - WALL_SIZE - r->z) >> WALL_SHIFT) + r->x_size * ((item->pos.x_pos - r->x) >> WALL_SHIFT)];
			left = h == item->pos.y_pos && !floor->stopper;

			room_number = item->room_number;
			floor = GetFloor(item->pos.x_pos - WALL_SIZE, item->pos.y_pos, item->pos.z_pos, &room_number);
			h = GetHeight(floor, item->pos.x_pos - WALL_SIZE, item->pos.y_pos, item->pos.z_pos);
			r = &room[room_number];
			floor = &r->floor[((item->pos.z_pos - r->z) >> WALL_SHIFT) + r->x_size * ((item->pos.x_pos - WALL_SIZE - r->x) >> WALL_SHIFT)];
			ahead = h == item->pos.y_pos && !floor->stopper;

			if (!ahead && !left && item->item_flags[0] > 0)
			{
				item->pos.y_rot += 1024;
				item->item_flags[0] = 1024;
			}
			else if (!ahead && !left && item->item_flags[0] < 0)
			{
				item->pos.y_rot -= 1024;
				item->item_flags[0] = -1024;
			}
			else if (left && item->item_flags[0] > 0)
			{
				item->pos.y_rot -= 1024;
				item->item_flags[0] = -1024;
			}
			else
			{
				item->item_flags[0] = 1024;
				item->item_flags[1] = 1;
				item->pos.x_pos -= item->item_flags[2];
				r->floor[((item->pos.z_pos - r->z) >> WALL_SHIFT) + r->x_size * ((item->pos.x_pos - WALL_SIZE - r->x) >> WALL_SHIFT)].stopper = 1;
			}

			break;

		case -0x8000:
			room_number = item->room_number;
			floor = GetFloor(item->pos.x_pos + WALL_SIZE, item->pos.y_pos, item->pos.z_pos, &room_number);
			h = GetHeight(floor, item->pos.x_pos + WALL_SIZE, item->pos.y_pos, item->pos.z_pos);
			r = &room[room_number];
			floor = &r->floor[((item->pos.z_pos - r->z) >> WALL_SHIFT) + r->x_size * ((item->pos.x_pos + WALL_SIZE - r->x) >> WALL_SHIFT)];
			left = h == item->pos.y_pos && !floor->stopper;

			room_number = item->room_number;
			floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos - WALL_SIZE, &room_number);
			h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos - WALL_SIZE);
			r = &room[room_number];
			floor = &r->floor[((item->pos.z_pos - WALL_SIZE - r->z) >> WALL_SHIFT) + r->x_size * ((item->pos.x_pos - r->x) >> WALL_SHIFT)];
			ahead = h == item->pos.y_pos && !floor->stopper;

			if (!ahead && !left && item->item_flags[0] > 0)
			{
				item->pos.y_rot += 1024;
				item->item_flags[0] = 1024;
			}
			else if (!ahead && !left && item->item_flags[0] < 0)
			{
				item->pos.y_rot -= 1024;
				item->item_flags[0] = -1024;
			}
			else if (left && item->item_flags[0] > 0)
			{
				item->pos.y_rot -= 1024;
				item->item_flags[0] = -1024;
			}
			else
			{
				item->item_flags[0] = 1024;
				item->item_flags[1] = 1;
				item->pos.z_pos -= item->item_flags[2];
				r->floor[((item->pos.z_pos - WALL_SIZE - r->z) >> WALL_SHIFT) + r->x_size * ((item->pos.x_pos - r->x) >> WALL_SHIFT)].stopper = 1;
			}

			break;
		}

		floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
		GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
		TestTriggers(trigger_index, 1);

		if (HeavyTriggered)
		{
			item->item_flags[2] = 0;
			SoundEffect(SFX_CLEANER_FUSEBOX, &item->pos, SFX_DEFAULT);
		}
	}
	else
	{
		//move
		switch (item->pos.y_rot)
		{
		case 0:
			item->pos.z_pos += item->item_flags[2];
			break;

		case 0x4000:
			item->pos.x_pos += item->item_flags[2];
			break;

		case -0x4000:
			item->pos.x_pos -= item->item_flags[2];
			break;

		case -0x8000:
			item->pos.z_pos -= item->item_flags[2];
			break;
		}
	}

	if (item->touch_bits & 0xFFFC && !lara.electric)
	{
		lara.electric = 1;
		lara_item->hit_points = 0;
		item->item_flags[2] = 0;
		SoundEffect(SFX_CLEANER_FUSEBOX, &item->pos, SFX_DEFAULT);
	}

	AnimateItem(item);
	room_number = item->room_number;
	GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);

	if (item->room_number != room_number)
		ItemNewRoom(item_number, room_number);

	SoundEffect(SFX_CLEANER_LOOP, &item->pos, SFX_DEFAULT);

	if ((!(GetRandomControl() & 7) && !elecspark[0]) || elecspark[0])
	{
		if (!elecspark[0])
			elecspark[0] = (GetRandomControl() & 7) + 4;
		else
			elecspark[0]--;

		pos.x = -160;
		pos.y = -8;
		pos.z = 16;
		GetJointAbsPosition(item, &pos, 5);
		TriggerElectricSparks(&pos, item_number, 9);
		pos.x += (GetRandomControl() & 0x1F) - 16;
		pos.y += (GetRandomControl() & 0x1F) - 16;
		pos.z += (GetRandomControl() & 0x1F) - 16;
		c = (GetRandomControl() & 0x7F) + 128;
		TriggerDynamic(pos.x, pos.y, pos.z, 10, c >> 2, c >> 1, c);
	}

	if ((!(GetRandomControl() & 7) && !elecspark[1]) || elecspark[1])
	{
		if (!elecspark[1])
			elecspark[1] = (GetRandomControl() & 7) + 4;
		else
			elecspark[1]--;

		pos.x = -160;
		pos.y = -8;
		pos.z = 16;
		GetJointAbsPosition(item, &pos, 9);
		TriggerElectricSparks(&pos, item_number, 10);
		pos.x += (GetRandomControl() & 0x1F) - 16;
		pos.y += (GetRandomControl() & 0x1F) - 16;
		pos.z += (GetRandomControl() & 0x1F) - 16;
		c = (GetRandomControl() & 0x7F) + 128;
		TriggerDynamic(pos.x, pos.y, pos.z, 10, c >> 2, c >> 1, c);
	}

	if ((!(GetRandomControl() & 7) && !elecspark[2]) || elecspark[2])
	{
		if (!elecspark[2])
			elecspark[2] = (GetRandomControl() & 7) + 4;
		else
			elecspark[2]--;

		pos.x = -160;
		pos.y = -8;
		pos.z = 16;
		GetJointAbsPosition(item, &pos, 13);
		TriggerElectricSparks(&pos, item_number, 11);
		pos.x += (GetRandomControl() & 0x1F) - 16;
		pos.y += (GetRandomControl() & 0x1F) - 16;
		pos.z += (GetRandomControl() & 0x1F) - 16;
		c = (GetRandomControl() & 0x7F) + 128;
		TriggerDynamic(pos.x, pos.y, pos.z, 10, c >> 2, c >> 1, c);
	}
}
