#include "../tomb3/pch.h"
#include "camera.h"
#include "../3dsystem/3d_gen.h"
#include "../specific/function_stubs.h"
#include "control.h"
#include "../3dsystem/phd_math.h"

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
		x = ABS(camera.pos.x - ideal->x);
		y = ABS(camera.pos.y - ideal->y);
		z = ABS(camera.pos.z - ideal->z);

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
		camera.actual_angle = (short)phd_atan(x, z);
		camera.mike_pos.x = ((phd_sin(camera.actual_angle) * phd_persp) >> 14) + camera.pos.x;
		camera.mike_pos.z = ((phd_cos(camera.actual_angle) * phd_persp) >> 14) + camera.pos.z;
		camera.mike_pos.y = camera.pos.y;
	}

	camera.old_type = camera.type;
}

void inject_camera(bool replace)
{
	INJECT(0x00417300, InitialiseCamera, replace);
	INJECT(0x0041743C, MoveCamera, replace);
}
