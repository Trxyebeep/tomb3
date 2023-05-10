#include "../tomb3/pch.h"
#include "camera.h"
#include "../3dsystem/3d_gen.h"
#include "../specific/game.h"
#include "control.h"
#include "../3dsystem/phd_math.h"
#include "lara.h"
#include "sound.h"
#include "../specific/specific.h"
#include "cinema.h"
#include "draw.h"
#include "inventry.h"
#include "../tomb3/tomb3.h"
#include "../newstuff/LaraDraw.h"

CAMERA_INFO camera;

GAME_VECTOR ForcedFixedCamera;
char UseForcedFixedCamera;

static OLD_CAMERA old_cam;
static GAME_VECTOR last_target;
static GAME_VECTOR last_ideal;
static GAME_VECTOR static_lookcamp;
static GAME_VECTOR static_lookcamt;
static PHD_VECTOR old_target;
static char camerasnaps;

void InitialiseCamera()
{
	camera.shift = lara_item->pos.y_pos - 1024;
	last_target.x = lara_item->pos.x_pos;
	last_target.y = camera.shift;
	last_target.z = lara_item->pos.z_pos;
	last_target.room_number = lara_item->room_number;
	camera.target.x = last_target.x;
	camera.target.y = camera.shift;
	camera.target.z = last_target.z;
	camera.target.room_number = last_target.room_number;
	camera.pos.x = last_target.x;
	camera.pos.y = camera.shift;
	camera.pos.z = last_target.z - 100;
	camera.pos.room_number = last_target.room_number;
	camera.target_distance = 1536;
	camera.item = 0;
	camera.number_frames = 1;

	if (!lara.extra_anim)
		camera.type = CHASE_CAMERA;

	camera.speed = 1;
	camera.flags = 0;
	camera.bounce = 0;
	camera.number = -1;
	camera.fixed_camera = 0;
	AlterFOV(14560);
	UseForcedFixedCamera = 0;
	CalculateCamera();
}

long mgLOS(GAME_VECTOR* start, GAME_VECTOR* target, long push)
{
	FLOOR_INFO* floor;
	long x, y, z, h, c, cdiff, hdiff, dx, dy, dz, clipped, nc, i;
	short room_number, room_number2;

	dx = (target->x - start->x) >> 3;
	dy = (target->y - start->y) >> 3;
	dz = (target->z - start->z) >> 3;
	x = start->x;
	y = start->y;
	z = start->z;
	room_number = start->room_number;
	room_number2 = room_number;
	nc = 0;
	clipped = 0;

	for (i = 0; i < 8; i++)
	{
		room_number = room_number2;
		floor = GetFloor(x, y, z, &room_number2);

		if (room[room_number2].flags & ROOM_SWAMP)
		{
			clipped = 1;
			break;
		}

		h = GetHeight(floor, x, y, z);
		c = GetCeiling(floor, x, y, z);

		if (h == NO_HEIGHT || c == NO_HEIGHT || c >= h)
		{
			if (!nc)
			{
				x += dx;
				y += dy;
				z += dz;
				continue;
			}

			clipped = 1;
			break;
		}

		if (y > h)
		{
			hdiff = y - h;

			if (hdiff < push)
				y = h;
			else
			{
				clipped = 1;
				break;
			}
		}

		if (y < c)
		{
			cdiff = c - y;

			if (cdiff < push)
				y = c;
			else
			{
				clipped = 1;
				break;
			}
		}

		nc = 1;
		x += dx;
		y += dy;
		z += dz;
	}

	if (i)
	{
		x -= dx;
		y -= dy;
		z -= dz;
	}

	target->x = x;
	target->y = y;
	target->z = z;
	GetFloor(x, y, z, &room_number);
	target->room_number = room_number;
	return !clipped;
}

long CameraCollisionBounds(GAME_VECTOR* ideal, long push, long yfirst)
{
	FLOOR_INFO* floor;
	long wx, wy, wz, h, c;
	short room_number;

	wx = ideal->x;
	wy = ideal->y;
	wz = ideal->z;

	if (yfirst)
	{
		room_number = ideal->room_number;
		floor = GetFloor(wx, wy, wz, &room_number);
		h = GetHeight(floor, wx, wy, wz);
		c = GetCeiling(floor, wx, wy, wz);

		if (c > wy - 255 && h < wy + 255 && c < h && c != NO_HEIGHT && h != NO_HEIGHT)
			wy = (c + h) >> 1;
		else if (h < wy + 255 && c < h && c != NO_HEIGHT && h != NO_HEIGHT)
			wy = h - 255;
		else if (c > wy - 255 && c < h && c != NO_HEIGHT && h != NO_HEIGHT)
			wy = c + 255;
	}

	room_number = ideal->room_number;
	floor = GetFloor(wx - push, wy, wz, &room_number);
	h = GetHeight(floor, wx - push, wy, wz);
	c = GetCeiling(floor, wx - push, wy, wz);

	if (h < wy || h == NO_HEIGHT || c == NO_HEIGHT || c >= h || wy < c)
		wx = push + (wx & ~WALL_MASK);

	room_number = ideal->room_number;
	floor = GetFloor(wx, wy, wz - push, &room_number);
	h = GetHeight(floor, wx, wy, wz - push);
	c = GetCeiling(floor, wx, wy, wz - push);

	if (h < wy || h == NO_HEIGHT || c == NO_HEIGHT || c >= h || wy < c)
		wz = push + (wz & ~WALL_MASK);

	room_number = ideal->room_number;
	floor = GetFloor(wx + push, wy, wz, &room_number);
	h = GetHeight(floor, wx + push, wy, wz);
	c = GetCeiling(floor, wx + push, wy, wz);

	if (h < wy || h == NO_HEIGHT || c == NO_HEIGHT || c >= h || wy < c)
		wx = (wx | WALL_MASK) - push;

	room_number = ideal->room_number;
	floor = GetFloor(wx, wy, wz + push, &room_number);
	h = GetHeight(floor, wx, wy, wz + push);
	c = GetCeiling(floor, wx, wy, wz + push);

	if (h < wy || h == NO_HEIGHT || c == NO_HEIGHT || c >= h || wy < c)
		wz = (wz | WALL_MASK) - push;

	if (!yfirst)
	{
		room_number = ideal->room_number;
		floor = GetFloor(wx, wy, wz, &room_number);
		h = GetHeight(floor, wx, wy, wz);
		c = GetCeiling(floor, wx, wy, wz);

		if (c > wy - 255 && h < wy + 255 && c < h && c != NO_HEIGHT && h != NO_HEIGHT)
			wy = (h + c) >> 1;
		else if (h < wy + 255 && c < h && c != NO_HEIGHT && h != NO_HEIGHT)
			wy = h - 255;
		else if (c > wy - 255 && c < h && c != NO_HEIGHT && h != NO_HEIGHT)
			wy = c + 255;
	}

	room_number = ideal->room_number;
	floor = GetFloor(wx, wy, wz, &room_number);
	h = GetHeight(floor, wx, wy, wz);
	c = GetCeiling(floor, wx, wy, wz);

	if (h < wy || wy < c || h == NO_HEIGHT || c == NO_HEIGHT || c >= h)
		return 1;

	GetFloor(wx, wy, wz, &ideal->room_number);
	ideal->x = wx;
	ideal->y = wy;
	ideal->z = wz;
	return 0;
}

void MoveCamera(GAME_VECTOR* ideal, long speed)
{
	FLOOR_INFO* floor;
	GAME_VECTOR start;
	GAME_VECTOR target;
	long shake, x, y, z, h, c;
	short room_number;

	if (old_cam.pos.x_rot == lara_item->pos.x_rot && old_cam.pos.y_rot == lara_item->pos.y_rot && old_cam.pos.z_rot == lara_item->pos.z_rot &&
		old_cam.pos2.x_rot == lara.head_x_rot && old_cam.pos2.y_rot == lara.head_y_rot && old_cam.pos2.x_pos == lara.torso_x_rot &&
		old_cam.pos2.y_pos == lara.torso_y_rot && old_cam.pos.x_pos == lara_item->pos.x_pos && old_cam.pos.y_pos == lara_item->pos.y_pos &&
		old_cam.pos.z_pos == lara_item->pos.z_pos && old_cam.current_anim_state == lara_item->current_anim_state &&
		old_cam.goal_anim_state == lara_item->goal_anim_state && camera.old_type == camera.type)
	{
		ideal->x = last_ideal.x;
		ideal->y = last_ideal.y;
		ideal->z = last_ideal.z;
		ideal->room_number = last_ideal.room_number;
	}
	else
	{
		old_cam.pos.x_rot = lara_item->pos.x_rot;
		old_cam.pos.y_rot = lara_item->pos.y_rot;
		old_cam.pos.z_rot = lara_item->pos.z_rot;
		old_cam.pos2.x_rot = lara.head_x_rot;
		old_cam.pos2.y_rot = lara.head_y_rot;
		old_cam.pos2.x_pos = lara.torso_x_rot;
		old_cam.pos2.y_pos = lara.torso_y_rot;
		old_cam.pos.x_pos = lara_item->pos.x_pos;
		old_cam.pos.y_pos = lara_item->pos.y_pos;
		old_cam.pos.z_pos = lara_item->pos.z_pos;
		old_cam.current_anim_state = lara_item->current_anim_state;
		old_cam.goal_anim_state = lara_item->goal_anim_state;
		last_ideal.x = ideal->x;
		last_ideal.y = ideal->y;
		last_ideal.z = ideal->z;
		last_ideal.room_number = ideal->room_number;
	}

	camera.pos.x += (ideal->x - camera.pos.x) / speed;
	camera.pos.y += (ideal->y - camera.pos.y) / speed;
	camera.pos.z += (ideal->z - camera.pos.z) / speed;
	camera.pos.room_number = ideal->room_number;

	if (camera.bounce)
	{
		if (camera.bounce <= 0)
		{
			shake = camera.bounce * (GetRandomControl() - 0x4000) / 0x7FFF;
			camera.pos.x += shake;
			camera.target.y += shake;

			shake = camera.bounce * (GetRandomControl() - 0x4000) / 0x7FFF;
			camera.pos.y += shake;
			camera.target.y += shake;

			shake = camera.bounce * (GetRandomControl() - 0x4000) / 0x7FFF;
			camera.pos.z += shake;
			camera.target.z += shake;

			camera.bounce += 5;
		}
		else
		{
			camera.pos.y += camera.bounce;
			camera.target.y += camera.bounce;
			camera.bounce = 0;
		}
	}

	x = camera.pos.x;
	y = camera.pos.y;
	z = camera.pos.z;
	room_number = camera.pos.room_number;
	floor = GetFloor(x, y + 256, z, &room_number);

	if (room[room_number].flags & ROOM_SWAMP)
	{
		y = room[room_number].maxceiling - 256;
		floor = GetFloor(x, y, z, &camera.pos.room_number);
	}

	floor = GetFloor(x, y, z, &room_number);
	h = GetHeight(floor, x, y, z);
	c = GetCeiling(floor, x, y, z);

	if (y < c || y > h)
	{
		mgLOS(&camera.target, &camera.pos, 0);
		x = abs(camera.pos.x - ideal->x);
		y = abs(camera.pos.y - ideal->y);
		z = abs(camera.pos.z - ideal->z);

		if (x < 768 && y < 768 && z < 768)
		{
			start.x = ideal->x;
			start.y = ideal->y;
			start.z = ideal->z;
			start.room_number = ideal->room_number;

			target.x = camera.pos.x;
			target.y = camera.pos.y;
			target.z = camera.pos.z;
			target.room_number = camera.pos.room_number;

			if (!mgLOS(&start, &target, 0))
			{
				camerasnaps++;

				if (camerasnaps >= 8)
				{
					camera.pos.x = ideal->x;
					camera.pos.y = ideal->y;
					camera.pos.z = ideal->z;
					camera.pos.room_number = ideal->room_number;
					camerasnaps = 0;
				}
			}
		}
	}

	x = camera.pos.x;
	y = camera.pos.y;
	z = camera.pos.z;
	room_number = camera.pos.room_number;
	floor = GetFloor(camera.pos.x, camera.pos.y, camera.pos.z, &room_number);
	h = GetHeight(floor, x, y, z);
	c = GetCeiling(floor, x, y, z);

	if (y - 255 < c && y + 255 > h && c < h && c != NO_HEIGHT && h != NO_HEIGHT)
		camera.pos.y = (c + h) >> 1;
	else if (y + 255 > h && c < h && c != NO_HEIGHT && h != NO_HEIGHT)
		camera.pos.y = h - 255;
	else if (y - 255 < c && c < h && c != NO_HEIGHT && h != NO_HEIGHT)
		camera.pos.y = c + 255;
	else if (c >= h || h == NO_HEIGHT || c == NO_HEIGHT)
	{
		camera.pos.x = ideal->x;
		camera.pos.y = ideal->y;
		camera.pos.z = ideal->z;
		camera.pos.room_number = ideal->room_number;
	}

	GetFloor(camera.pos.x, camera.pos.y, camera.pos.z, &camera.pos.room_number);
	phd_LookAt(camera.pos.x, camera.pos.y, camera.pos.z, camera.target.x, camera.target.y, camera.target.z, 0);

	if (camera.mike_at_lara)
	{
		camera.actual_angle = lara.torso_y_rot + lara.head_y_rot + lara_item->pos.y_rot;
		camera.mike_pos.x = lara_item->pos.x_pos;
		camera.mike_pos.y = lara_item->pos.y_pos;
		camera.mike_pos.z = lara_item->pos.z_pos;
	}
	else
	{
		x = camera.target.x - camera.pos.x;
		z = camera.target.z - camera.pos.z;
		camera.actual_angle = (short)phd_atan(z, x);
		camera.mike_pos.x = ((phd_sin(camera.actual_angle) * phd_persp) >> W2V_SHIFT) + camera.pos.x;
		camera.mike_pos.z = ((phd_cos(camera.actual_angle) * phd_persp) >> W2V_SHIFT) + camera.pos.z;
		camera.mike_pos.y = camera.pos.y;
	}

	camera.old_type = camera.type;
}

void ChaseCamera(ITEM_INFO* item)
{
	FLOOR_INFO* floor;
	GAME_VECTOR ideal;
	GAME_VECTOR ideals[5];
	GAME_VECTOR temp[2];
	long distance, dx, dz, farthest, farthestnum, h, c, x, y, z;
	short angle, room_number;

	if (!camera.target_elevation)
		camera.target_elevation = -1820;

	camera.target_elevation += item->pos.x_rot;

	if (camera.target_elevation > 15470)
		camera.target_elevation = 15470;
	else if (camera.target_elevation < -15470)
		camera.target_elevation = -15470;

	distance = (phd_cos(camera.target_elevation) * camera.target_distance) >> W2V_SHIFT;
	room_number = camera.target.room_number;
	floor = GetFloor(camera.target.x, camera.target.y + 256, camera.target.z, &room_number);

	if (room[room_number].flags & ROOM_SWAMP)
		camera.target.y = room[room_number].maxceiling - 256;

	x = camera.target.x;
	y = camera.target.y;
	z = camera.target.z;
	floor = GetFloor(x, y, z, &camera.target.room_number);
	h = GetHeight(floor, x, y, z);
	c = GetCeiling(floor, x, y, z);

	if (c + 16 > h - 16 && h != NO_HEIGHT && c != NO_HEIGHT)
	{
		camera.target.y = (h + c) >> 1;
		camera.target_elevation = 0;
	}
	else if (y > h - 16 && h != NO_HEIGHT)
	{
		camera.target.y = h - 16;
		camera.target_elevation = 0;
	}
	else if (y < c + 16 && c != NO_HEIGHT)
	{
		camera.target.y = c + 16;
		camera.target_elevation = 0;
	}

	floor = GetFloor(camera.target.x, camera.target.y, camera.target.z, &camera.target.room_number);
	x = camera.target.x;
	y = camera.target.y;
	z = camera.target.z;
	room_number = camera.target.room_number;
	floor = GetFloor(camera.target.x, camera.target.y, camera.target.z, &room_number);
	h = GetHeight(floor, x, y, z);
	c = GetCeiling(floor, x, y, z);

	if (y < c || y > h || c >= h || h == NO_HEIGHT || c == NO_HEIGHT)
	{
		camera.target.x = last_target.x;
		camera.target.y = last_target.y;
		camera.target.z = last_target.z;
		camera.target.room_number = last_target.room_number;
	}

	for (int i = 0; i < 5; i++)
		ideals[i].y = ((phd_sin(camera.target_elevation) * camera.target_distance) >> W2V_SHIFT) + camera.target.y;

	farthest = 0x7FFFFFFF;
	farthestnum = 0;

	for (int i = 0; i < 5; i++)
	{
		if (i)
			angle = (i - 1) << W2V_SHIFT;
		else
			angle = camera.target_angle + item->pos.y_rot;

		ideals[i].x = camera.target.x - ((distance * phd_sin(angle)) >> W2V_SHIFT);
		ideals[i].z = camera.target.z - ((distance * phd_cos(angle)) >> W2V_SHIFT);
		ideals[i].room_number = camera.target.room_number;

		if (mgLOS(&camera.target, &ideals[i], 200))
		{
			temp[0].x = ideals[i].x;
			temp[0].y = ideals[i].y;
			temp[0].z = ideals[i].z;
			temp[0].room_number = ideals[i].room_number;
			temp[1].x = camera.pos.x;
			temp[1].y = camera.pos.y;
			temp[1].z = camera.pos.z;
			temp[1].room_number = camera.pos.room_number;

			if (!i || mgLOS(&temp[0], &temp[1], 0))
			{
				if (!i)
				{
					farthestnum = 0;
					break;
				}

				dx = SQUARE(camera.pos.x - ideals[i].x);
				dz = SQUARE(camera.pos.z - ideals[i].z);
				dz += dx;

				if (dz < farthest)
				{
					farthest = dz;
					farthestnum = i;
				}
			}
		}
		else if (!i)
		{
			temp[0].x = ideals[i].x;
			temp[0].y = ideals[i].y;
			temp[0].z = ideals[i].z;
			temp[0].room_number = ideals[i].room_number;
			temp[1].x = camera.pos.x;
			temp[1].y = camera.pos.y;
			temp[1].z = camera.pos.z;
			temp[1].room_number = camera.pos.room_number;
			dx = SQUARE(camera.target.x - ideals[i].x);
			dz = SQUARE(camera.target.z - ideals[i].z);
			dz += dx;

			if (dz > 0x90000)
			{
				farthestnum = 0;
				break;
			}
		}
	}

	ideal.x = ideals[farthestnum].x;
	ideal.y = ideals[farthestnum].y;
	ideal.z = ideals[farthestnum].z;
	ideal.room_number = ideals[farthestnum].room_number;
	CameraCollisionBounds(&ideal, 384, 1);

	if (camera.old_type == FIXED_CAMERA)
		camera.speed = 1;

	MoveCamera(&ideal, camera.speed);
}

void CombatCamera(ITEM_INFO* item)
{
	FLOOR_INFO* floor;
	GAME_VECTOR ideal;
	GAME_VECTOR ideals[9];
	GAME_VECTOR temp[2];
	long distance, dx, dz, farthest, farthestnum, h, c, x, y, z;
	short angle, room_number;

	camera.target.x = item->pos.x_pos;
	camera.target.z = item->pos.z_pos;

	if (lara.target)
	{
		camera.target_angle = lara.target_angles[0] + item->pos.y_rot;
		camera.target_elevation = lara.target_angles[1] + item->pos.x_rot;
	}
	else
	{
		camera.target_angle = lara.torso_y_rot + lara.head_y_rot + item->pos.y_rot;
		camera.target_elevation = lara.head_x_rot + item->pos.x_rot + lara.torso_x_rot - 2730;
	}

	room_number = camera.target.room_number;
	floor = GetFloor(camera.target.x, camera.target.y + 256, camera.target.z, &room_number);

	if (room[room_number].flags & ROOM_SWAMP)
		camera.target.y = room[room_number].maxceiling - 256;

	x = camera.target.x;
	y = camera.target.y;
	z = camera.target.z;
	floor = GetFloor(x, y, z, &camera.target.room_number);
	h = GetHeight(floor, x, y, z);
	c = GetCeiling(floor, x, y, z);

	if (c + 64 > h - 64 && h != NO_HEIGHT && c != NO_HEIGHT)
	{
		camera.target.y = (c + h) >> 1;
		camera.target_elevation = 0;
	}
	else if (camera.target.y > h - 64 && h != NO_HEIGHT)
	{
		camera.target.y = h - 64;
		camera.target_elevation = 0;
	}
	else if (camera.target.y < c + 64 && c != NO_HEIGHT)
	{
		camera.target.y = c + 64;
		camera.target_elevation = 0;
	}

	x = camera.target.x;
	y = camera.target.y;
	z = camera.target.z;
	GetFloor(x, y, z, &camera.target.room_number);
	room_number = camera.target.room_number;
	floor = GetFloor(x, y, z, &room_number);
	h = GetHeight(floor, x, y, z);
	c = GetCeiling(floor, x, y, z);

	if (y < c || y > h || c >= h || h == NO_HEIGHT || c == NO_HEIGHT)
	{
		camera.target.x = last_target.x;
		camera.target.y = last_target.y;
		camera.target.z = last_target.z;
		camera.target.room_number = last_target.room_number;
	}

	camera.target_distance = 1536;
	distance = camera.target_distance * phd_cos(camera.target_elevation) >> W2V_SHIFT;

	for (int i = 0; i < 5; i++)
		ideals[i].y = (camera.target_distance * phd_sin(camera.target_elevation) >> W2V_SHIFT) + camera.target.y;

	farthest = 0x7FFFFFFF;
	farthestnum = 0;

	for (int i = 0; i < 5; i++)
	{
		if (i)
			angle = (i - 1) << W2V_SHIFT;
		else
			angle = camera.target_angle;

		ideals[i].x = camera.target.x - ((distance * phd_sin(angle)) >> W2V_SHIFT);
		ideals[i].z = camera.target.z - ((distance * phd_cos(angle)) >> W2V_SHIFT);
		ideals[i].room_number = camera.target.room_number;

		if (mgLOS(&camera.target, &ideals[i], 200))
		{
			temp[0].x = ideals[i].x;
			temp[0].y = ideals[i].y;
			temp[0].z = ideals[i].z;
			temp[0].room_number = ideals[i].room_number;
			temp[1].x = camera.pos.x;
			temp[1].y = camera.pos.y;
			temp[1].z = camera.pos.z;
			temp[1].room_number = camera.pos.room_number;
			if (!i || mgLOS(temp, &temp[1], 0))
			{
				if (!i)
				{
					farthestnum = 0;
					break;
				}

				dx = SQUARE(camera.pos.x - ideals[i].x);
				dz = SQUARE(camera.pos.z - ideals[i].z);
				dz += dx;

				if (dz < farthest)
				{
					farthest = dz;
					farthestnum = i;
				}
			}
		}
		else if (!i)
		{
			temp[0].x = ideals[i].x;
			temp[0].y = ideals[i].y;
			temp[0].z = ideals[i].z;
			temp[0].room_number = ideals[i].room_number;
			temp[1].x = camera.pos.x;
			temp[1].y = camera.pos.y;
			temp[1].z = camera.pos.z;
			temp[1].room_number = camera.pos.room_number;
			dx = SQUARE(camera.target.x - ideals[i].x);
			dz = SQUARE(camera.target.z - ideals[i].z);
			dz += dx;

			if (dz > 0x90000)
			{
				farthestnum = 0;
				break;
			}
		}
	}

	ideal.x = ideals[farthestnum].x;
	ideal.y = ideals[farthestnum].y;
	ideal.z = ideals[farthestnum].z;
	ideal.room_number = ideals[farthestnum].room_number;
	CameraCollisionBounds(&ideal, 384, 1);

	if (camera.old_type == FIXED_CAMERA)
		camera.speed = 1;

	MoveCamera(&ideal, camera.speed);
}

void LookCamera(ITEM_INFO* item)
{
	GAME_VECTOR ideal;
	PHD_VECTOR pos1;
	PHD_VECTOR pos2;
	PHD_VECTOR pos3;
	FLOOR_INFO* floor;
	long shake, dx, dy, dz, x, y, z, h, c, lp, clipped;
	short room_number, room_number2, hxrot, txrot, hyrot, tyrot;

	hxrot = lara.head_x_rot;
	hyrot = lara.head_y_rot;
	txrot = lara.torso_x_rot;
	tyrot = lara.torso_y_rot;
	lara.torso_x_rot = 0;
	lara.torso_y_rot = 0;
	lara.head_x_rot <<= 1;
	lara.head_y_rot <<= 1;

	if (lara.head_x_rot > 10010)
		lara.head_x_rot = 10010;
	else if (lara.head_x_rot < -13650)
		lara.head_x_rot = -13650;

	if (lara.head_y_rot < -14560)
		lara.head_y_rot = -14560;
	else if (lara.head_y_rot > 14560)
		lara.head_y_rot = 14560;

	pos1.x = 0;
	pos1.y = 16;
	pos1.z = 64;
	GetLaraMeshPos(&pos1, LMX_HEAD);
	clipped = 0;
	room_number = lara_item->room_number;
	floor = GetFloor(pos1.x, pos1.y + 256, pos1.z, &room_number);

	if (room[room_number].flags & ROOM_SWAMP)
		pos1.y = room[room_number].maxceiling - 256;

	floor = GetFloor(pos1.x, pos1.y, pos1.z, &room_number);
	h = GetHeight(floor, pos1.x, pos1.y, pos1.z);
	c = GetCeiling(floor, pos1.x, pos1.y, pos1.z);

	if (h == NO_HEIGHT || c == NO_HEIGHT || c >= h || pos1.y > h || pos1.y < c)
		clipped = 1;

	if (clipped)
	{
		pos1.x = 0;
		pos1.y = 16;
		pos1.z = 0;
		GetLaraMeshPos(&pos1, LMX_HEAD);
		clipped = 0;
		room_number = lara_item->room_number;
		floor = GetFloor(pos1.x, pos1.y + 256, pos1.z, &room_number);

		if (room[room_number].flags & ROOM_SWAMP)
			pos1.y = room[room_number].maxceiling - 256;

		floor = GetFloor(pos1.x, pos1.y, pos1.z, &room_number);
		h = GetHeight(floor, pos1.x, pos1.y, pos1.z);
		c = GetCeiling(floor, pos1.x, pos1.y, pos1.z);

		if (h == NO_HEIGHT || c == NO_HEIGHT || c >= h || pos1.y > h || pos1.y < c)
			clipped = 1;

		if (clipped)
		{
			pos1.x = 0;
			pos1.y = 16;
			pos1.z = -64;
			GetLaraMeshPos(&pos1, LMX_HEAD);
		}
	}

	pos2.x = 0;
	pos2.y = 0;
	pos2.z = -1024;
	GetLaraMeshPos(&pos2, LMX_HEAD);

	pos3.x = 0;
	pos3.y = 0;
	pos3.z = 2048;
	GetLaraMeshPos(&pos3, LMX_HEAD);

	dx = (pos2.x - pos1.x) >> 3;
	dy = (pos2.y - pos1.y) >> 3;
	dz = (pos2.z - pos1.z) >> 3;
	x = pos1.x;
	y = pos1.y;
	z = pos1.z;
	room_number2 = lara_item->room_number;
	room_number = room_number2;

	for (lp = 0; lp < 8; lp++)
	{
		room_number = room_number2;
		floor = GetFloor(x, y + 256, z, &room_number2);

		if (room[room_number2].flags & ROOM_SWAMP)
		{
			y = room[room_number2].maxceiling - 256;
			break;
		}

		floor = GetFloor(x, y, z, &room_number2);
		h = GetHeight(floor, x, y, z);
		c = GetCeiling(floor, x, y, z);

		if (h == NO_HEIGHT || c == NO_HEIGHT || c >= h || y > h || y < c)
			break;

		x += dx;
		y += dy;
		z += dz;
	}

	if (lp)
	{
		x -= dx;
		y -= dy;
		z -= dz;
	}

	ideal.x = x;
	ideal.y = y;
	ideal.z = z;
	ideal.room_number = room_number;

	if (old_cam.pos.x_rot == lara.head_x_rot && old_cam.pos.y_rot == lara.head_y_rot && old_cam.pos.x_pos == lara_item->pos.x_pos &&
		old_cam.pos.y_pos == lara_item->pos.y_pos && old_cam.pos.z_pos == lara_item->pos.z_pos && old_cam.current_anim_state == lara_item->current_anim_state &&
		old_cam.goal_anim_state == lara_item->goal_anim_state && camera.old_type == LOOK_CAMERA)
	{
		ideal.x = static_lookcamp.x;
		ideal.y = static_lookcamp.y;
		ideal.z = static_lookcamp.z;
		ideal.room_number = static_lookcamp.room_number;
		pos3.x = static_lookcamt.x;
		pos3.y = static_lookcamt.y;
		pos3.z = static_lookcamt.z;
	}
	else
	{
		old_cam.pos.x_rot = lara.head_x_rot;
		old_cam.pos.y_rot = lara.head_y_rot;
		old_cam.pos.x_pos = lara_item->pos.x_pos;
		old_cam.pos.y_pos = lara_item->pos.y_pos;
		old_cam.pos.z_pos = lara_item->pos.z_pos;
		old_cam.current_anim_state = lara_item->current_anim_state;
		old_cam.goal_anim_state = lara_item->goal_anim_state;
		static_lookcamp.x = ideal.x;
		static_lookcamp.y = ideal.y;
		static_lookcamp.z = ideal.z;
		static_lookcamp.room_number = ideal.room_number;
		static_lookcamt.x = pos3.x;
		static_lookcamt.y = pos3.y;
		static_lookcamt.z = pos3.z;
	}

	CameraCollisionBounds(&ideal, 224, 1);

	if (camera.old_type == FIXED_CAMERA)
	{
		camera.pos.x = ideal.x;
		camera.pos.y = ideal.y;
		camera.pos.z = ideal.z;
		camera.target.x = pos3.x;
		camera.target.y = pos3.y;
		camera.target.z = pos3.z;
	}
	else
	{
		camera.pos.x += (ideal.x - camera.pos.x) >> 2;
		camera.pos.y += (ideal.y - camera.pos.y) >> 2;
		camera.pos.z += (ideal.z - camera.pos.z) >> 2;
		camera.target.x += (pos3.x - camera.target.x) >> 2;
		camera.target.y += (pos3.y - camera.target.y) >> 2;
		camera.target.z += (pos3.z - camera.target.z) >> 2;
	}

	camera.target.room_number = lara_item->room_number;

	if (camera.bounce && camera.type == camera.old_type)
	{
		if (camera.bounce <= 0)
		{
			shake = (camera.bounce * (GetRandomControl() - 0x4000)) >> 15;
			camera.pos.x += shake;
			camera.target.y += shake;

			shake = (camera.bounce * (GetRandomControl() - 0x4000)) / 0x7FFF;
			camera.pos.y += shake;
			camera.target.y += shake;

			shake = (camera.bounce * (GetRandomControl() - 0x4000)) / 0x7FFF;
			camera.pos.z += shake;
			camera.target.z += shake;

			camera.bounce += 5;
		}
		else
		{
			camera.pos.y += camera.bounce;
			camera.target.y += camera.bounce;
			camera.bounce = 0;
		}
	}

	GetFloor(camera.pos.x, camera.pos.y, camera.pos.z, &camera.pos.room_number);
	x = camera.pos.x;
	y = camera.pos.y;
	z = camera.pos.z;
	room_number = camera.pos.room_number;
	floor = GetFloor(x, y, z, &room_number);
	h = GetHeight(floor, x, y, z);
	c = GetCeiling(floor, x, y, z);

	if (c > y - 255 && h < y + 255 && h > c && c != NO_HEIGHT && h != NO_HEIGHT)
		camera.pos.y = (c + h) >> 1;
	else if (h < y + 255 && h > c && c != NO_HEIGHT && h != NO_HEIGHT)
		camera.pos.y = h - 255;
	else if (c > y - 255 && h > c && c != NO_HEIGHT && h != NO_HEIGHT)
		camera.pos.y = c + 255;

	x = camera.pos.x;
	y = camera.pos.y;
	z = camera.pos.z;
	room_number = camera.pos.room_number;
	floor = GetFloor(x, y, z, &room_number);
	h = GetHeight(floor, x, y, z);
	c = GetCeiling(floor, x, y, z);

	if (room[room_number].flags & ROOM_SWAMP)
		camera.pos.y = room[room_number].y - 256;
	else if (y < c || y > h || c >= h || h == NO_HEIGHT || c == NO_HEIGHT)
		mgLOS(&camera.target, &camera.pos, 0);

	x = camera.pos.x;
	y = camera.pos.y;
	z = camera.pos.z;
	room_number = camera.pos.room_number;
	floor = GetFloor(x, y, z, &room_number);
	h = GetHeight(floor, x, y, z);
	c = GetCeiling(floor, x, y, z);

	if (y < c || y > h || c >= h || h == NO_HEIGHT || c == NO_HEIGHT || room[room_number].flags & ROOM_SWAMP)
	{
		camera.pos.x = pos1.x;
		camera.pos.y = pos1.y;
		camera.pos.z = pos1.z;
		camera.pos.room_number = lara_item->room_number;
	}

	GetFloor(camera.pos.x, camera.pos.y, camera.pos.z, &camera.pos.room_number);
	phd_LookAt(camera.pos.x, camera.pos.y, camera.pos.z, camera.target.x, camera.target.y, camera.target.z, 0);

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
		camera.mike_pos.x = ((phd_sin(camera.actual_angle) * phd_persp) >> W2V_SHIFT) + camera.pos.x;
		camera.mike_pos.z = ((phd_cos(camera.actual_angle) * phd_persp) >> W2V_SHIFT) + camera.pos.z;
		camera.mike_pos.y = camera.pos.y;
	}

	camera.old_type = camera.type;
	lara.head_x_rot = hxrot;
	lara.head_y_rot = hyrot;
	lara.torso_x_rot = txrot;
	lara.torso_y_rot = tyrot;
}

void FixedCamera()
{
	OBJECT_VECTOR* fixed;
	GAME_VECTOR ideal;

	if (UseForcedFixedCamera)
	{
		ideal.x = ForcedFixedCamera.x;
		ideal.y = ForcedFixedCamera.y;
		ideal.z = ForcedFixedCamera.z;
		ideal.room_number = ForcedFixedCamera.room_number;
	}
	else
	{
		fixed = &camera.fixed[camera.number];
		ideal.x = fixed->x;
		ideal.y = fixed->y;
		ideal.z = fixed->z;
		ideal.room_number = fixed->data;
	}

	camera.fixed_camera = 1;
	MoveCamera(&ideal, 1);

	if (camera.timer)
	{
		camera.timer--;

		if (!camera.timer)
			camera.timer = -1;
	}
}

void CalculateCamera()
{
	ITEM_INFO* item;
	short* bounds;
	float vol;
	long fixed_camera, y, shift, dx, dz;
	short angle, tilt;

	old_target.x = camera.target.x;
	old_target.y = camera.target.y;
	old_target.z = camera.target.z;

	if (UseForcedFixedCamera)
	{
		camera.type = FIXED_CAMERA;
		camera.speed = 1;
	}

	if (room[camera.pos.room_number].flags & ROOM_UNDERWATER)
	{
		SoundEffect(SFX_UNDERWATER, 0, SFX_ALWAYS);

		if (!camera.underwater)
		{
			vol = (1.0F - tomb3.unwater_music_mute) * float(25 * Option_Music_Volume + 5);

			if (Option_Music_Volume > 0 && vol >= 1)
				S_CDVolume((long)vol);
			else
				S_CDVolume(0);

			camera.underwater = 1;
		}
	}
	else if (camera.underwater)
	{
		if (Option_Music_Volume)
			S_CDVolume(25 * Option_Music_Volume + 5);

		camera.underwater = 0;
	}

	if (camera.type == CINEMATIC_CAMERA)
	{
		InGameCinematicCamera();
		return;
	}

	if (camera.flags != 2)
		chunky_flag = 0;

	fixed_camera = camera.item && (camera.type == FIXED_CAMERA || camera.type == HEAVY_CAMERA);

	if (fixed_camera)
		item = camera.item;
	else
		item = lara_item;

	bounds = GetBoundsAccurate(item);
	y = item->pos.y_pos;

	if (fixed_camera)
		y += (bounds[2] + bounds[3]) / 2;
	else
		y += bounds[3] + ((bounds[2] - bounds[3]) * 3 >> 2);

	if (camera.item && !fixed_camera)
	{
		dx = camera.item->pos.x_pos - item->pos.x_pos;
		dz = camera.item->pos.z_pos - item->pos.z_pos;
		shift = phd_sqrt(SQUARE(dx) + SQUARE(dz));
		angle = short(phd_atan(dz, dx) - item->pos.y_rot);
		bounds = GetBoundsAccurate(camera.item);
		tilt = (short)phd_atan(shift, y - ((bounds[3] + bounds[2]) / 2 + camera.item->pos.y_pos));
		angle >>= 1;
		tilt >>= 1;

		if (angle > -9100 && angle < 9100 && tilt > -15470 && tilt < 15470)
		{
			if (angle - lara.head_y_rot > 728)
				lara.head_y_rot += 728;
			else if (angle - lara.head_y_rot < -728)
				lara.head_y_rot -= 728;
			else
				lara.head_y_rot = angle;

			lara.torso_y_rot = lara.head_y_rot;

			if (tilt - lara.head_x_rot > 728)
				lara.head_x_rot += 728;
			else if (tilt - lara.head_x_rot < -728)
				lara.head_x_rot -= 728;
			else
				lara.head_x_rot = tilt;

			lara.torso_x_rot = lara.head_x_rot;
			camera.type = LOOK_CAMERA;
			camera.item->looked_at = 1;
		}
	}

	if (camera.type == LOOK_CAMERA || camera.type == COMBAT_CAMERA)
	{
		if (camera.type == COMBAT_CAMERA)
		{
			last_target.x = camera.target.x;
			last_target.y = camera.target.y;
			last_target.z = camera.target.z;
			last_target.room_number = camera.target.room_number;
		}

		y -= 256;
		camera.target.room_number = item->room_number;

		if (camera.fixed_camera)
		{
			camera.target.y = y;
			camera.speed = 1;
		}
		else
		{
			camera.target.y += (y - camera.target.y) >> 2;
			camera.speed = camera.type != LOOK_CAMERA ? 8 : 4;
		}

		camera.fixed_camera = 0;

		if (camera.type == LOOK_CAMERA)
			LookCamera(item);
		else
			CombatCamera(item);
	}
	else
	{
		last_target.x = camera.target.x;
		last_target.y = camera.target.y;
		last_target.z = camera.target.z;
		last_target.room_number = camera.target.room_number;
		camera.target.x = item->pos.x_pos;
		camera.target.z = item->pos.z_pos;

		if (camera.flags == 1)
		{
			shift = (bounds[5] + bounds[4]) / 2;
			camera.target.z += (shift * phd_cos(item->pos.y_rot)) >> W2V_SHIFT;
			camera.target.x += (shift * phd_sin(item->pos.y_rot)) >> W2V_SHIFT;
		}

		camera.target.room_number = item->room_number;
		camera.target.y = y;

		if (fixed_camera != camera.fixed_camera)
		{
			camera.fixed_camera = 1;
			camera.speed = 1;
		}
		else
			camera.fixed_camera = 0;

		if (camera.speed != 1 && camera.old_type != LOOK_CAMERA)
		{
			camera.target.x = ((camera.target.x - old_target.x) >> 2) + old_target.x;
			camera.target.y = ((camera.target.y - old_target.y) >> 2) + old_target.y;
			camera.target.z = ((camera.target.z - old_target.z) >> 2) + old_target.z;
		}

		GetFloor(camera.target.x, camera.target.y, camera.target.z, &camera.target.room_number);

		if (camera.type == CHASE_CAMERA || camera.flags == 3)
			ChaseCamera(item);
		else
			FixedCamera();
	}

	camera.last = camera.number;
	camera.fixed_camera = fixed_camera;

	if (camera.type != HEAVY_CAMERA || camera.timer == -1)
	{
		camera.type = CHASE_CAMERA;
		camera.speed = 10;
		camera.number = NO_ITEM;
		camera.last_item = camera.item;
		camera.item = 0;
		camera.target_elevation = 0;
		camera.target_angle = 0;
		camera.target_distance = 1536;
		camera.flags = 0;
	}

	chunky_flag = 0;
}
