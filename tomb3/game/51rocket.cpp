#include "../tomb3/pch.h"
#include "51rocket.h"
#include "effect2.h"
#include "../specific/game.h"
#include "objects.h"
#include "control.h"
#include "sound.h"
#include "items.h"
#include "traps.h"
#include "camera.h"
#include "lara.h"

static char strut_fall;

static void Trigger51BlastFire(long x, long y, long z, long smoke, long end)
{
	SPARKS* sptr;
	long size;

	if (end < 0)
		sptr = &sparks[GetFreeSpark()];
	else
		sptr = &sparks[end];

	sptr->On = 1;

	if (smoke)
	{
		sptr->sR = 0;
		sptr->sG = 0;
		sptr->sB = 0;
		sptr->dR = 64;
		sptr->dG = 64;
		sptr->dB = 64;
	}
	else
	{
		sptr->sR = (GetRandomControl() & 0x1F) + 128;
		sptr->sG = (GetRandomControl() & 0x1F) + 64;
		sptr->sB = 32;
		sptr->dR = (GetRandomControl() & 0x1F) + 224;
		sptr->dG = (GetRandomControl() & 0x1F) + 160;
		sptr->dB = 32;
	}

	sptr->ColFadeSpeed = 16;

	if (end)
	{
		sptr->FadeToBlack = (GetRandomControl() & 0x1F) + 32;
		sptr->Life = uchar((end >> 1) + 72);
	}
	else
	{
		sptr->FadeToBlack = smoke != 0 ? 32 : 8;
		sptr->Life = (!smoke ? 0 : 32) + (GetRandomControl() & 7) + 32;
	}
	
	sptr->sLife = sptr->Life;
	sptr->TransType = 2;
	sptr->extras = 0;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomControl() & 0xF) + x - 8;
	sptr->y = y;
	sptr->z = (GetRandomControl() & 0xF) + z - 8;
	sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Yvel = -(GetRandomControl() & 7);
	sptr->Zvel = (GetRandomControl() & 0xFF) - 128;

	sptr->Friction = 4;
	sptr->Flags = SF_ALTDEF | SF_ROTATE | SF_DEF | SF_SCALE;
	sptr->RotAng = GetRandomControl() & 0xFFF;

	if (GetRandomControl() & 1)
		sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
	else
		sptr->RotAdd = (GetRandomControl() & 0xF) + 16;

	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Scalar = 4;
	sptr->MaxYvel = 0;
	sptr->Gravity = 0;

	size = (GetRandomControl() & 0x3F) + 64;

	if (end)
	{
		sptr->Width = (uchar)size;
		sptr->sWidth = sptr->Width;
		sptr->dWidth = sptr->Width;
		
		sptr->Height = sptr->Width;
		sptr->sHeight = sptr->Height;
		sptr->dHeight = sptr->Height;
	}
	else
	{
		sptr->dWidth = (uchar)size;
		sptr->Width = sptr->dWidth >> 1;
		sptr->sWidth = sptr->Width;
		
		sptr->dHeight = sptr->dWidth;
		sptr->Height = sptr->dHeight >> 1;
		sptr->sHeight = sptr->Height;
	}
}

static void Trigger51RocketSmoke(long x, long y, long z, long yv, long fire)
{
	SPARKS* sptr;
	long size;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;

	if (fire)
	{
		sptr->sR = (GetRandomControl() & 0x1F) + 48;
		sptr->sG = sptr->sR;
		sptr->sB = (GetRandomControl() & 0x3F) + 192;
		sptr->dR = (GetRandomControl() & 0x3F) + 192;
		sptr->dG = (GetRandomControl() & 0x3F) + 128;
		sptr->dB = 32;
	}
	else
	{
		sptr->sR = 0;
		sptr->sG = 0;
		sptr->sB = 0;
		sptr->dR = uchar((yv >> 5) + 32);
		sptr->dG = sptr->dR;
		sptr->dB = sptr->dR;
	}

	sptr->ColFadeSpeed = 16 - uchar(!fire ? 0 : yv >> 9);
	sptr->FadeToBlack = !fire ? 16 : 0;
	sptr->Life = (GetRandomControl() & 3) - uchar(!fire ? 0 : yv >> 8) + 60;
	sptr->sLife = sptr->Life;
	sptr->TransType = 2;
	sptr->extras = 0;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomControl() & 0xF) + x - 8;
	sptr->y = y;
	sptr->z = (GetRandomControl() & 0xF) + z - 8;
	sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Yvel = short(yv + (GetRandomControl() & 0xF));
	sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Friction = 4;
	
	if (GetRandomControl() & 1)
	{
		sptr->Flags = SF_ALTDEF | SF_ROTATE | SF_DEF | SF_SCALE;
		sptr->RotAng = GetRandomControl() & 0xFFF;

		if (GetRandomControl() & 1)
			sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
		else
			sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
	}
	else
		sptr->Flags = SF_ALTDEF | SF_DEF | SF_SCALE;

	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Scalar = 3;
	sptr->MaxYvel = 0;
	sptr->Gravity = 0;

	size = (GetRandomControl() & 0x3F) + (yv >> 5) + 64;

	if (size > 255)
		size = 255;

	sptr->dWidth = (uchar)size;
	sptr->Width = sptr->dWidth >> 2;
	sptr->sWidth = sptr->Width;
	
	sptr->dHeight = sptr->dWidth;
	sptr->Height = sptr->dHeight >> 2;
	sptr->sHeight = sptr->Height;
}

void ControlArea51Rocket(short item_number)
{
	ITEM_INFO* item;
	long r, g, b, x, y, z, rad, lp;
	short room_number;

	item = &items[item_number];

	if (!TriggerActive(item))
		return;

	if (item->object_number == SPECIAL_FX1)
	{
		if (item->required_anim_state < 512)
		{
			item->required_anim_state += 8;

			if (item->required_anim_state < 512)
			{
				SoundEffect(SFX_LARA_FLARE_BURN, 0, SFX_DEFAULT);
				item->current_anim_state = 0;
				item->goal_anim_state = 0;
			}
			else
			{
				item->required_anim_state += 2048;
				item->goal_anim_state = 64;
				SoundEffect(SFX_EXPLOSION2, 0, SFX_DEFAULT);
			}
		}
		else
		{
			if (item->current_anim_state)
			{
				SoundEffect(SFX_HUGE_ROCKET_LOOP, 0, SFX_DEFAULT);
				item->required_anim_state += 32;

				if (item->required_anim_state > 16000)
					KillItem(item_number);
				else
				{
					b = 0x4000 - item->required_anim_state;
					r = (b * ((GetRandomControl() & 0x1F) + 224)) >> 12;
					g = (b * ((GetRandomControl() & 0x3F) + 96)) >> 12;
					b = (b * (GetRandomControl() & 0x1F)) >> 12;
					TriggerDynamic(item->pos.x_pos - 7680, room[52].minfloor - (GetRandomControl() & 0x1FF) - 256, item->pos.z_pos - 1024, 24, r, g, b);
					camera.bounce = -((0x4000 - item->required_anim_state) >> 6);
				}

				return;
			}

			if (!lara.burn)
			{
				rad = item->goal_anim_state;

				if (rad >= 8192)
					rad = 8192;

				if (lara_item->pos.x_pos > item->pos.x_pos - rad - 1536)
				{
					lara_item->hit_points = 0;
					LaraBurn();
				}

				item->goal_anim_state += 80;
			}

			item->required_anim_state += 32;
		}

		if (item->required_anim_state < 4608)
		{
			if (item->goal_anim_state > 768)
				SoundEffect(SFX_HUGE_ROCKET_LOOP, 0, SFX_DEFAULT);

			if (item->goal_anim_state > 1024)
				item->goal_anim_state = 1024;
		}
		else
		{
			SoundEffect(SFX_HUGE_ROCKET_LOOP, 0, SFX_DEFAULT);

			if (item->required_anim_state > 12288)
				item->required_anim_state = 12288;

			if (item->goal_anim_state > 22528)
			{
				for (lp = 0; lp < 64; lp++)
				{
					x = item->pos.x_pos - (GetRandomControl() & 0xFFF) - 5632;
					z = item->pos.z_pos + (GetRandomControl() & 0x7FF) - 2048;
					Trigger51BlastFire(x, room[52].minfloor - (GetRandomControl() & 0x7FF), z, 0, lp);
				}

				for (lp = 64; lp < 96; lp++)
				{
					x = item->pos.x_pos - (GetRandomControl() & 0xFFF) - 5632;
					z = item->pos.z_pos + (GetRandomControl() & 0x7FF) - 2048;
					Trigger51BlastFire(x, room[52].minfloor - (GetRandomControl() & 0x7FF), z, 1, lp);
				}

				camera.bounce = -((0x4000 - item->required_anim_state) >> 6);
				item->current_anim_state = 1;
				return;
			}

			item->fallspeed--;

			if (item->fallspeed < -1024)
				item->fallspeed = -1024;

			if (item->fallspeed < -72)
				strut_fall = 1;

			item->pos.y_pos += item->fallspeed >> 2;
			room_number = item->room_number;
			GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);

			if (item->room_number != room_number)
				ItemNewRoom(item_number, room_number);
		}

		if (item->required_anim_state >= 0x2000)
			camera.bounce = -((0x4000 - item->required_anim_state) >> 6);
		else if (item->required_anim_state <= 64)
			camera.bounce = -1;
		else
			camera.bounce = -(item->required_anim_state >> 6);
	}

	if (!(wibble & 0xC))
	{
		if (item->required_anim_state >= 128)
			r = 4 * (item->required_anim_state + (GetRandomControl() & 0x1F)) - 512;
		else
			r = GetRandomControl() & 0x1F;

		if (r > 6144)
			r = 6144;

		b = item->required_anim_state >= 512;
		Trigger51RocketSmoke(item->pos.x_pos - 896, item->pos.y_pos - 64, item->pos.z_pos - 512, r, b);
		Trigger51RocketSmoke(item->pos.x_pos - 128, item->pos.y_pos - 64, item->pos.z_pos - 512, r, b);
		Trigger51RocketSmoke(item->pos.x_pos - 512, item->pos.y_pos - 64, item->pos.z_pos - 896, r, b);
		Trigger51RocketSmoke(item->pos.x_pos - 512, item->pos.y_pos - 64, item->pos.z_pos - 128, r, b);
	}

	if (item->goal_anim_state)
	{
		r = (GetRandomControl() & 0x1F) + 224;
		g = (GetRandomControl() & 0x3F) + 96;
		b = GetRandomControl() & 0x1F;
		TriggerDynamic(item->pos.x_pos - 512, item->pos.y_pos, item->pos.z_pos - 512, 31, r, g, b);

		rad = item->goal_anim_state;

		if (rad >= 8192)
			rad = 8192;

		b = item->goal_anim_state >= 1024 ? 2047 : 255;
		x = item->pos.x_pos - (GetRandomControl() & 0x7FF) - rad + 512;
		y = room[52].minfloor - (b & GetRandomControl());
		z = item->pos.z_pos + (GetRandomControl() & 0x7FF) - 2048;

		if (wibble & 4)
			Trigger51BlastFire(x, y, z, 0, -1);

		x = item->pos.x_pos - rad + 512;
		z = item->pos.z_pos - 1024;
		r = (GetRandomControl() & 0x1F) + 224;
		g = (GetRandomControl() & 0x3F) + 96;
		b = GetRandomControl() & 0x1F;
		TriggerDynamic(x, y, z, 24, r, g, b);

		if (wibble & 4)
		{
			b = item->goal_anim_state >= 1024 ? 2047 : 255;
			x = item->pos.x_pos - (GetRandomControl() & 0x3FF) - rad - 512;
			y = room[52].minfloor - (b & GetRandomControl());
			z = item->pos.z_pos + (GetRandomControl() & 0x7FF) - 2048;
			Trigger51BlastFire(x, y, z, 1, -1);
		}
	}
}

void InitialiseArea51Struts(short item_number)
{
	strut_fall = 0;
}

void ControlArea51Struts(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (!strut_fall)
		item->current_anim_state = 1;
	else if (item->goal_anim_state != 2 && item->current_anim_state != 2)
		item->goal_anim_state = 2;

	AnimateItem(item);
}
