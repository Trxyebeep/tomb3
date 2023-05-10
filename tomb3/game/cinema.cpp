#include "../tomb3/pch.h"
#include "cinema.h"
#include "draw.h"
#include "../specific/draweffects.h"
#include "../specific/picture.h"
#include "../specific/output.h"
#include "items.h"
#include "sphere.h"
#include "laramisc.h"
#include "objects.h"
#include "../specific/game.h"
#include "effect2.h"
#include "../3dsystem/phd_math.h"
#include "../3dsystem/3d_gen.h"
#include "control.h"
#include "../specific/input.h"
#include "hair.h"
#include "../specific/specific.h"
#include "gameflow.h"
#include "../specific/smain.h"
#include "sound.h"
#include "camera.h"
#include "lara.h"
#include "effects.h"
#include "inventry.h"
#include "../newstuff/LaraDraw.h"
#if (DIRECT3D_VERSION >= 0x900)
#include "../newstuff/Picture2.h"
#endif

short* cine;
PHD_3DPOS cinematic_pos;
long cutscene_track = 1;
short cine_loaded;
short cine_frame;
short num_cine_frames;
static long actual_current_frame;
static short cinematic_level;

long DrawPhaseCinematic()
{
	CalcLaraMatrices(0);
	phd_PushUnitMatrix();
	phd_SetTrans(0, 0, 0);
	CalcLaraMatrices(1);
	phd_PopMatrix();

	ResetLaraUnderwaterNodes();

	camera_underwater = 0;
	DrawRooms(camera.pos.room_number);
	S_DrawSparks();
	S_FadePicture();
	S_OutputPolyList();
	camera.number_frames = S_DumpScreen();

	if (S_DumpCine())
		camera.number_frames = TICKS_PER_FRAME;

	S_AnimateTextures(camera.number_frames);
	return camera.number_frames;
}

void InitialiseGenPlayer(short item_number)
{
	ITEM_INFO* item;

	AddActiveItem(item_number);
	item = &items[item_number];
	item->pos.y_rot = 0;
	item->dynamic_light = 0;
}

void InitCinematicRooms()
{
	ROOM_INFO* r;

	for (int i = 0; i < number_rooms; i++)
	{
		r = &room[i];

		if (r->flipped_room >= 0)
			room[r->flipped_room].bound_active = 1;

		r->flags |= ROOM_OUTSIDE;
	}

	number_draw_rooms = 0;

	for (int i = 0; i < number_rooms; i++)
	{
		r = &room[i];

		if (!r->bound_active)
		{
			draw_rooms[number_draw_rooms] = i;
			number_draw_rooms++;
		}
	}
}

long GetCinematicRoom(long x, long y, long z)
{
	ROOM_INFO* r;

	for (int i = 0; i < number_rooms; i++)
	{
		r = &room[i];

		if (x >= r->x + WALL_SIZE && x < (r->y_size << WALL_SHIFT) + r->x - WALL_SIZE &&
			y >= r->maxceiling && y <= r->minfloor &&
			z >= r->z + WALL_SIZE && z < (r->x_size << WALL_SHIFT) + r->z - WALL_SIZE)
			return i;
	}

	return -1;
}

void LaraControlCinematic(short item_number)
{
	ITEM_INFO* item;
	PHD_VECTOR pos;
	short room_number;

	item = &items[item_number];
	item->pos.y_rot = camera.target_angle;
	item->pos.x_pos = camera.pos.x;
	item->pos.y_pos = camera.pos.y;
	item->pos.z_pos = camera.pos.z;

	pos.x = 0;
	pos.y = 0;
	pos.z = 0;
	GetJointAbsPosition(item, &pos, 0);
	room_number = (short)GetCinematicRoom(pos.x, pos.y, pos.z);

	if (room_number != -1 && item->room_number != room_number)
		ItemNewRoom(item_number, room_number);

	AnimateLara(item);
}

void InitialisePlayer1(short item_number)
{
	objects[LARA].draw_routine = DrawDummyItem;
	objects[LARA].control = LaraControlCinematic;
	objects[LARA].shadow_size = 0;
	AddActiveItem(item_number);
	lara_item = &items[item_number];

	camera.pos.x = lara_item->pos.x_pos;
	camera.pos.y = lara_item->pos.y_pos;
	camera.pos.z = lara_item->pos.z_pos;
	lara_item->pos.y_rot = 0;
	camera.target_angle = 0;
	camera.pos.room_number = lara_item->room_number;
	lara_item->dynamic_light = 0;
	lara_item->anim_number = 0;
	lara_item->frame_number = 0;
	lara_item->current_anim_state = 0;
	lara_item->goal_anim_state = 0;
	lara.hit_direction = -1;
}

void UpdateLaraGuns()
{
	FX_INFO* fx;
	PHD_VECTOR* pos;
	PHD_VECTOR lpos;
	PHD_VECTOR rpos;
	long flash, r;
	short room_number, fx_num;

	if (!lara.left_arm.flash_gun && !lara.right_arm.flash_gun && !SmokeCountL && !SmokeCountR)
		return;

	flash = 0;
	r = 0;

	lpos.x = -12;
	lpos.y = 48;
	lpos.z = 40;
	GetJointAbsPosition(lara_item, &lpos, HAND_L);

	rpos.x = 8;
	rpos.y = 48;
	rpos.z = 40;
	GetJointAbsPosition(lara_item, &rpos, HAND_R);

	pos = &lpos;

	if (lara.left_arm.flash_gun)
	{
		flash = lara.left_arm.flash_gun;
		lara.left_arm.flash_gun--;
	}
	else if (lara.right_arm.flash_gun)
	{
		flash = lara.right_arm.flash_gun;
		pos = &rpos;
		r = 1;
		lara.right_arm.flash_gun--;
	}

	if (flash)
	{
		if (flash == 3)
		{
			room_number = (short)GetCinematicRoom(pos->x, pos->y, pos->z);
			fx_num = CreateEffect(room_number);

			if (fx_num != NO_ITEM)
			{
				fx = &effects[fx_num];
				fx->pos.x_pos = pos->x;
				fx->pos.y_pos = pos->y;
				fx->pos.z_pos = pos->z;
				fx->room_number = room_number;
				fx->pos.x_rot = 0;
				fx->pos.y_rot = 0;
				fx->pos.z_rot = (short)GetRandomControl();
				fx->speed = (GetRandomControl() & 0x1F) + 16;
				fx->object_number = GUNSHELL;
				fx->fallspeed = -48 - (GetRandomControl() & 7);
				fx->frame_number = objects[GUNSHELL].mesh_index;
				fx->shade = 0x4210;
				fx->counter = (GetRandomControl() & 1) + 1;
			}

			if (r)
				SmokeCountR = 28;
			else
				SmokeCountL = 28;
		}

		TriggerDynamic(pos->x + (GetRandomControl() & 0xFF) - 128,
			pos->y - (GetRandomControl() & 0x7F) + 63,
			pos->z + (GetRandomControl() & 0xFF) - 128,
			10, (GetRandomControl() & 0x3F) + 192, (GetRandomControl() & 0x1F) + 128, GetRandomControl() & 0x3F);
	}

	if (SmokeCountL)
	{
		TriggerGunSmoke(lpos.x, lpos.y, lpos.z, 0, 0, 0, 0, LG_PISTOLS, SmokeCountL);
		SmokeCountL--;
	}

	if (SmokeCountR)
	{
		TriggerGunSmoke(rpos.x, rpos.y, rpos.z, 0, 0, 0, 0, LG_PISTOLS, SmokeCountR);
		SmokeCountR--;
	}
}

void CalculateCinematicCamera()
{
	PHD_VECTOR pos;
	PHD_VECTOR tar;
	short* ptr;
	long s, c;
	short tx, ty, tz, cx, cy, cz, roll, fov, room_number;

	ptr = &cine[8 * cine_frame];
	tx = ptr[0];
	ty = ptr[1];
	tz = ptr[2];
	cx = ptr[3];
	cy = ptr[4];
	cz = ptr[5];
	fov = ptr[6];
	roll = ptr[7];
	s = phd_sin(camera.target_angle);
	c = phd_cos(camera.target_angle);

	pos.x = lara_item->pos.x_pos + ((cz * s + cx * c) >> W2V_SHIFT);
	pos.y = lara_item->pos.y_pos + cy;
	pos.z = lara_item->pos.z_pos + ((cz * c - cx * s) >> W2V_SHIFT);

	tar.x = lara_item->pos.x_pos + ((tx * c + tz * s) >> W2V_SHIFT);
	tar.y = lara_item->pos.y_pos + ty;
	tar.z = lara_item->pos.z_pos + ((tz * c - tx * s) >> W2V_SHIFT);

	room_number = (short)GetCinematicRoom(pos.x, pos.y, pos.z);

	if (room_number >= 0)
		camera.pos.room_number = room_number;

	AlterFOV(fov);
	phd_LookAt(pos.x, pos.y, pos.z, tar.x, tar.y, tar.z, roll);
}

void InGameCinematicCamera()
{
	short* ptr;
	long s, c, dx, dz;
	short tx, ty, tz, cx, cy, cz, roll, fov;

	cine_frame++;

	if (cine_frame > num_cine_frames)
		cine_frame = num_cine_frames - 1;

	ptr = &cine[8 * cine_frame];
	tx = ptr[0];
	ty = ptr[1];
	tz = ptr[2];
	cx = ptr[3];
	cy = ptr[4];
	cz = ptr[5];
	fov = ptr[6];
	roll = ptr[7];
	s = phd_sin(camera.target_angle);
	c = phd_cos(camera.target_angle);

	camera.pos.x = cinematic_pos.x_pos + ((c * cx + s * cz) >> W2V_SHIFT);
	camera.pos.y = cinematic_pos.y_pos + cy;
	camera.pos.z = cinematic_pos.z_pos + ((c * cz - s * cx) >> W2V_SHIFT);

	camera.target.x = cinematic_pos.x_pos + ((c * tx + s * tz) >> W2V_SHIFT);
	camera.target.y = cinematic_pos.y_pos + ty;
	camera.target.z = cinematic_pos.z_pos + ((c * tz - s * tx) >> W2V_SHIFT);

	AlterFOV(fov);
	phd_LookAt(camera.pos.x, camera.pos.y, camera.pos.z, camera.target.x, camera.target.y, camera.target.z, roll);
	GetFloor(camera.pos.x, camera.pos.y, camera.pos.z, &camera.pos.room_number);

	if (camera.mike_at_lara)
	{
		camera.actual_angle = lara.torso_y_rot + lara.head_y_rot + lara_item->pos.y_rot;
		camera.mike_pos.x = lara_item->pos.x_pos;
		camera.mike_pos.y = lara_item->pos.y_pos;
		camera.mike_pos.z = lara_item->pos.z_pos;
	}
	else
	{
		dx = camera.target.x - camera.pos.x;
		dz = camera.target.z - camera.pos.z;
		camera.actual_angle = (short)phd_atan(dz, dx);
		camera.mike_pos.x = camera.pos.x + ((phd_persp * phd_sin(camera.actual_angle)) >> W2V_SHIFT);
		camera.mike_pos.y = camera.pos.y;
		camera.mike_pos.z = camera.pos.z + ((phd_persp * phd_cos(camera.actual_angle)) >> W2V_SHIFT);
	}
}

void ControlCinematicPlayer(short item_number)
{
	ITEM_INFO* item;
	PHD_VECTOR pos;
	short room_number;

	item = &items[item_number];
	item->pos.y_rot = camera.target_angle;
	item->pos.x_pos = camera.pos.x;
	item->pos.y_pos = camera.pos.y;
	item->pos.z_pos = camera.pos.z;

	pos.x = 0;
	pos.y = 0;
	pos.z = 0;
	GetJointAbsPosition(item, &pos, 0);
	room_number = (short)GetCinematicRoom(pos.x, pos.y, pos.z);

	if (room_number != -1 && item->room_number != room_number)
		ItemNewRoom(item_number, room_number);

	if (item->dynamic_light && item->status != ITEM_INVISIBLE)
	{
		pos.x = 0;
		pos.y = 0;
		pos.z = 0;
		GetJointAbsPosition(item, &pos, 0);
	}

	AnimateItem(item);
}

long DoCinematic(long nframes)
{
	ITEM_INFO* item;
	FX_INFO* fx;
	static long framecount, delay = 0x8000;
	short item_number, nex;
	
	for (framecount += delay * nframes; framecount > 0; framecount -= 0x10000)
	{
		if (S_UpdateInput())
			return 3;

		if (input & IN_ACTION && !pictureFading)
			return 1;

		if (input & IN_OPTION && !pictureFading)
			return 2;

		ClearDynamics();
		item_number = next_item_active;

		while (item_number != NO_ITEM)
		{
			item = &items[item_number];
			nex = item->next_active;

			if (objects[item->object_number].control)
				objects[item->object_number].control(item_number);

			item_number = nex;
		}

		item_number = next_fx_active;

		while (item_number != NO_ITEM)
		{
			fx = &effects[item_number];
			nex = fx->next_active;

			if (objects[fx->object_number].control)
				objects[fx->object_number].control(item_number);

			item_number = nex;
		}

		UpdateLaraGuns();
		UpdateSparks();
		HairControl(1);
		CalculateCinematicCamera();
		cine_frame++;

		if (cine_frame >= num_cine_frames)
			return 1;
	}

	actual_current_frame = S_CDGetLoc();
	return 0;
}

long StartCinematic(long level_number)
{
	long old_sound, ret, nframes;

	title_loaded = 0;
	cinematic_level = (short)level_number;
	InitCinematicRooms();
	InitialisePlayer1(lara.item_number);
	old_sound = sound_active;
	camera.target_angle = GF_Cutscene_Orientation;
	sound_active = 0;
	cine_frame = 0;
	S_ClearScreen();

	if (!S_StartSyncedAudio(cutscene_track))
		return 1;

	S_CDVolume(255);
	actual_current_frame = 0;
	GnGameMode = GAMEMODE_IN_CUTSCENE;

	do
	{
		DrawPhaseCinematic();
		nframes = actual_current_frame + TICKS_PER_FRAME * (4 - cine_frame);

		if (nframes < TICKS_PER_FRAME)
			nframes = TICKS_PER_FRAME;

		if (framedump)
			nframes = TICKS_PER_FRAME;

		ret = DoCinematic(nframes);

	} while (!ret);

	GnGameMode = GAMEMODE_NOT_IN_GAME;

	if (Option_Music_Volume)
		S_CDVolume(25 * Option_Music_Volume + 5);
	else
		S_CDVolume(0);

	S_CDStop();
	sound_active = old_sound;
	S_SoundStopAllSamples();
	level_complete = 1;
	return ret;
}
