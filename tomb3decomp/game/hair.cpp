#include "../tomb3/pch.h"
#include "hair.h"
#include "objects.h"

void InitialiseHair()
{
	long* bone;

	bone = &bones[objects[HAIR].bone_index + 2];
	first_hair = 1;
	hair[0].x_rot = -0x4000;
	hair[0].y_rot = 0;

	for (int i = 1; i < 7; i++, bone += 4)
	{
		hair[i].x_pos = bone[1];
		hair[i].y_pos = bone[2];
		hair[i].z_pos = bone[3];
		hair[i].x_rot = -0x4000;
		hair[i].y_rot = 0;
		hair[i].z_rot = 0;
		hvel[i].x = 0;
		hvel[i].y = 0;
		hvel[i].z = 0;
	}
}

void inject_hair(bool replace)
{
	INJECT(0x00433790, InitialiseHair, replace);
}
