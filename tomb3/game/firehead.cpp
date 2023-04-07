#include "../tomb3/pch.h"
#include "firehead.h"
#include "effect2.h"
#include "../specific/game.h"
#include "objects.h"
#include "control.h"
#include "sphere.h"
#include "sound.h"
#include "traps.h"
#include "items.h"
#include "lara.h"

static void TriggerFireHeadFlame(long x, long y, long z, long angle, long speed)
{
	SPARKS* sptr;
	long dx, dz;

	dx = lara_item->pos.x_pos - x;
	dz = lara_item->pos.z_pos - z;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz>0x4000)
		return;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = (GetRandomControl() & 0x1F) + 48;
	sptr->sG = sptr->sR;
	sptr->sB = (GetRandomControl() & 0x3F) + 192;
	sptr->dR = (GetRandomControl() & 0x3F) + 192;
	sptr->dG = (GetRandomControl() & 0x3F) + 128;
	sptr->dB = 32;
	sptr->FadeToBlack = 8;
	sptr->ColFadeSpeed = (GetRandomControl() & 3) + 12;
	sptr->TransType = 2;
	sptr->extras = 0;
	sptr->Life = (GetRandomControl() & 7) + 28;
	sptr->sLife = sptr->Life;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomControl() & 0x1F) + x - 16;
	sptr->y = (GetRandomControl() & 0x1F) + y - 16;
	sptr->z = (GetRandomControl() & 0x1F) + z - 16;
	speed -= GetRandomControl() % ((speed >> 3) + 1);
	sptr->Xvel = short((GetRandomControl() & 0x7F) + ((speed * rcossin_tbl[angle]) >> 11) - 64);
	sptr->Yvel = (GetRandomControl() & 7) + 6;
	sptr->Zvel = short((GetRandomControl() & 0x7F) + ((speed * rcossin_tbl[angle + 1]) >> 11) - 64);
	sptr->Friction = 4;
	sptr->Flags = SF_ALTDEF | SF_DEF | SF_SCALE;
	
	if (GetRandomControl() & 1)
	{
		sptr->Flags |= SF_ROTATE;
		sptr->RotAng = GetRandomControl() & 0xFFF;
		sptr->RotAdd = (GetRandomControl() & 0x3F) - 32;
	}

	sptr->Gravity = -8 - (GetRandomControl() & 0xF);
	sptr->MaxYvel = -8 - (GetRandomControl() & 7);
	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Scalar = 3;
	sptr->dWidth = uchar((speed >> 4) + (GetRandomControl() & 0xF));
	sptr->Width = sptr->dWidth >> 2;
	sptr->sWidth = sptr->Width;
	sptr->dHeight = sptr->dWidth;
	sptr->Height = sptr->Width;
	sptr->sHeight = sptr->Height;
}

void InitialiseFireHead(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	item->anim_number = objects[item->object_number].anim_index + 1;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = FIREHEAD_REAR;
	item->goal_anim_state = FIREHEAD_REAR;
}

void ControlFireHead(short item_number)
{
	ITEM_INFO* item;
	PHD_VECTOR pos;
	long angle, r, g;
	long xmin, xmax, ymin, ymax, zmin, zmax, lx, ly, lz;

	item = &items[item_number];

	if (!TriggerActive(item))
		return;

	if (item->current_anim_state == FIREHEAD_STILL)
	{
		item->goal_anim_state = FIREHEAD_REAR;
		AnimateItem(item);
		return;
	}

	angle = (((item->pos.y_rot >> 3) & 0xFFFE) + 0x800) & 0x1FFE;
	r = (GetRandomControl() & 0x3F) + 192;
	g = (GetRandomControl() & 0x1F) + 96;

	if (item->current_anim_state == FIREHEAD_REAR)
	{
		pos.x = 0;
		pos.y = 128;
		pos.z = 0;
		GetJointAbsPosition(item, &pos, 7);

		item->item_flags[FH_BLOWLOOPS] = item->hit_points;
		item->item_flags[FH_SPEED] = 0;
		item->item_flags[FH_STOP] = 0;
		item->item_flags[FH_DEADLYRANGE] = 0;
		TriggerDynamic(pos.x, pos.y, pos.z, 8, r, g, 0);
	}
	else
	{
		//FIREHEAD_BLOW
		pos.x = 0;
		pos.y = 0;
		pos.z = 0;
		GetJointAbsPosition(item, &pos, 7);

		if (item->item_flags[FH_STOP])
		{
			if (item->item_flags[FH_SPEED])
				item->item_flags[FH_SPEED] -= 64;

			if (item->item_flags[FH_DEADLYRANGE])
			{
				item->item_flags[FH_DEADLYRANGE] -= 32;

				if (item->item_flags[FH_DEADLYRANGE] < 0)
					item->item_flags[FH_DEADLYRANGE] = 0;
			}
		}
		else
		{
			SoundEffect(SFX_FLAME_THROWER_LOOP, &item->pos, SFX_DEFAULT);

			if (item->item_flags[FH_SPEED] < 2048)
				item->item_flags[FH_SPEED] += 64;

			if (item->item_flags[FH_DEADLYRANGE] < 2048)
				item->item_flags[FH_DEADLYRANGE] += 32;
		}

		if (item->item_flags[FH_BLOWLOOPS])
			item->item_flags[FH_BLOWLOOPS]--;

		if (wibble & 4)
			TriggerFireHeadFlame(pos.x, pos.y, pos.z, angle, item->item_flags[FH_SPEED]);

		if (!item->item_flags[FH_BLOWLOOPS] && !item->item_flags[FH_STOP] && item->frame_number == anims[item->anim_number].frame_base)
		{
			item->item_flags[FH_STOP] = 1;
			item->goal_anim_state = FIREHEAD_REAR;
		}

		TriggerDynamic(pos.x, pos.y, pos.z, (item->item_flags[1] >> 7) + 8, r, g, 0);

		if (!lara.burn)
		{
			ymin = item->pos.y_pos - 1024;
			ymax = item->pos.y_pos + 256;
			
			if (!item->pos.y_rot)
			{
				xmin = pos.x;
				xmax = pos.x + item->item_flags[FH_DEADLYRANGE];
				zmin = pos.z - 512;
				zmax = pos.z + 512;
			}
			else if (item->pos.y_rot == 0x4000)
			{
				xmin = pos.x - 512;
				xmax = pos.x + 512;
				zmin = pos.z - item->item_flags[FH_DEADLYRANGE];
				zmax = pos.z;
			}
			else if (item->pos.y_rot == -0x8000)
			{
				xmin = pos.x - item->item_flags[FH_DEADLYRANGE];
				xmax = pos.x;
				zmin = pos.z - 512;
				zmax = pos.z + 512;
			}
			else
			{
				xmin = pos.x - 512;
				xmax = pos.x + 512;
				zmin = pos.z;
				zmax = pos.z + item->item_flags[FH_DEADLYRANGE];
			}

			lx = lara_item->pos.x_pos;
			ly = lara_item->pos.y_pos;
			lz = lara_item->pos.z_pos;

			if (lx > xmin && lx < xmax && ly > ymin && ly < ymax && lz > zmin && lz < zmax)
				LaraBurn();
		}
	}

	AnimateItem(item);
}

void ControlRotateyThing(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (TriggerActive(item))
		AnimateItem(item);
	else
		KillItem(item_number);
}
