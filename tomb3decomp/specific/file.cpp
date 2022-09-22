#include "../tomb3/pch.h"
#include "file.h"
#include "dd.h"
#include "init.h"
#include "hwrender.h"
#include "game.h"
#include "../game/setup.h"
#include "../game/objects.h"

long MyReadFile(HANDLE hFile, LPVOID lpBuffer, ulong nNumberOfBytesToRead, ulong* lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped)
{
	static ulong nBytesRead;

	nBytesRead += nNumberOfBytesToRead;

	if (nBytesRead > 0x4000)
	{
		nBytesRead = 0;
		DD_SpinMessageLoop(0);
	}

	return ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
}

bool LoadPalette(HANDLE file)
{
	uchar* pGP;
	ulong nBytesRead;

	MyReadFile(file, game_palette, sizeof(game_palette), &nBytesRead, 0);
	game_palette[0] = 0;
	game_palette[1] = 0;
	game_palette[2] = 0;

	for (int i = 3; i < sizeof(game_palette); i++)
		game_palette[i] <<= 2;

	MyReadFile(file, G_GouraudPalette, sizeof(G_GouraudPalette), &nBytesRead, 0);
	BlackGouraudIndex = 0;
	pGP = G_GouraudPalette;

	while (pGP[0] || pGP[1] || pGP[2])
	{
		pGP += 4;
		BlackGouraudIndex++;

		if (BlackGouraudIndex > sizeof(G_GouraudPalette) / 4)
			break;
	}

	bFixSkyColour = 1;
	return 1;
}

long LoadTexturePages(HANDLE file)
{
	char* p;
	ulong read;
	long nPages, size;
	bool _16bit;

	MyReadFile(file, &nPages, sizeof(long), &read, 0);

	if (!App.nRenderMode)
	{
		for (int i = 0; i < nPages; i++)
		{
			if (!texture_page_ptrs[i])
				texture_page_ptrs[i] = (char*)game_malloc(0x10000, 1);

			MyReadFile(file, texture_page_ptrs[i], 0x10000, &read, 0);
		}

		SetFilePointer(file, nPages << 17, 0, FILE_CURRENT);
		return 1;
	}

	_16bit = !App.DeviceInfoPtr->DDInfo[App.DXConfigPtr->nDD].D3DInfo[App.DXConfigPtr->nD3D].Texture[App.DXConfigPtr->D3DTF].bPalette;
	size = _16bit ? 0x20000 : 0x10000;
	p = (char*)GlobalAlloc(GMEM_FIXED, nPages * size);

	if (!p)
		return 0;

	if (_16bit)
	{
		SetFilePointer(file, nPages << 16, 0, FILE_CURRENT);

		for (int i = 0; i < nPages; i++)
			MyReadFile(file, p + (size * i), size, &read, 0);

		HWR_LoadTexturePages(nPages, p, 0);
	}
	else
	{
		for (int i = 0; i < nPages; i++)
			MyReadFile(file, p + (size * i), size, &read, 0);

		SetFilePointer(file, nPages << 17, 0, FILE_CURRENT);
		HWR_LoadTexturePages(nPages, p, game_palette);
	}

	GlobalFree(p);
	return 1;
}

long LoadRooms(HANDLE file)
{
	ROOM_INFO* r;
	ulong read;
	long num;
	short nDoors;

	SeedRandomDraw(0xD371F947);
	SeedRandomControl(0xD371F947);
	wibble = 0;
	CurrentAtmosphere = 0;
	init_water_table();
	CalculateWibbleTable();
	MyReadFile(file, &number_rooms, sizeof(short), &read, 0);

	if (number_rooms < 0 || number_rooms > 1024)
	{
		lstrcpy(exit_message, "LoadRoom(): Too many rooms");
		return 0;
	}

	room = (ROOM_INFO*)game_malloc(sizeof(ROOM_INFO) * number_rooms, 11);

	if (!room)
	{
		lstrcpy(exit_message, "LoadRoom(): Could not allocate memory for rooms");
		return 0;
	}

	for (int i = 0; i < number_rooms; i++)
	{
		r = &room[i];

		MyReadFile(file, &r->x, sizeof(long), &read, 0);
		r->y = 0;
		MyReadFile(file, &r->z, sizeof(long), &read, 0);
		MyReadFile(file, &r->minfloor, sizeof(long), &read, 0);
		MyReadFile(file, &r->maxceiling, sizeof(long), &read, 0);

		MyReadFile(file, &num, sizeof(long), &read, 0);
		r->data = (short*)game_malloc(sizeof(short)* num, 12);
		MyReadFile(file, r->data, sizeof(short) * num, &read, 0);

		MyReadFile(file, &nDoors, sizeof(short), &read, 0);

		if (!nDoors)
			r->door = 0;
		else
		{
			r->door = (short*)game_malloc((16 * nDoors + 1) * sizeof(short), 13);
			r->door[0] = (short)nDoors;
			MyReadFile(file, r->door + 1, 16 * nDoors * sizeof(short), &read, 0);
		}

		MyReadFile(file, &r->x_size, sizeof(short), &read, 0);
		MyReadFile(file, &r->y_size, sizeof(short), &read, 0);
		num = r->x_size * r->y_size;
		r->floor = (FLOOR_INFO*)game_malloc(sizeof(FLOOR_INFO) * num, 14);
		MyReadFile(file, r->floor, sizeof(FLOOR_INFO) * num, &read, 0);

		MyReadFile(file, &r->ambient, sizeof(short), &read, 0);
		MyReadFile(file, &r->lighting, sizeof(short), &read, 0);
		MyReadFile(file, &r->num_lights, sizeof(short), &read, 0);

		if (!r->num_lights)
			r->light = 0;
		else
		{
			r->light = (LIGHT_INFO*)game_malloc(sizeof(LIGHT_INFO) * r->num_lights, 15);
			MyReadFile(file, r->light, sizeof(LIGHT_INFO) * r->num_lights, &read, 0);
		}

		MyReadFile(file, &r->num_meshes, sizeof(short), &read, 0);

		if (!r->num_meshes)
			r->mesh = 0;
		else
		{
			r->mesh = (MESH_INFO*)game_malloc(sizeof(MESH_INFO) * r->num_meshes, 16);
			MyReadFile(file, r->mesh, sizeof(MESH_INFO) * r->num_meshes, &read, 0);
		}

		MyReadFile(file, &r->flipped_room, sizeof(short), &read, 0);
		MyReadFile(file, &r->flags, sizeof(ushort), &read, 0);
		MyReadFile(file, &r->MeshEffect, sizeof(char), &read, 0);
		MyReadFile(file, &r->ReverbType, sizeof(short), &read, 0);
		r->bound_active = 0;
		r->left = phd_winxmax;
		r->top = phd_winymax;
		r->bottom = 0;
		r->right = 0;
		r->item_number = NO_ITEM;
		r->fx_number = NO_ITEM;
	}

	BuildOutsideTable();
	MyReadFile(file, &num, sizeof(long), &read, 0);
	floor_data = (short*)game_malloc(sizeof(short) * num, 17);
	MyReadFile(file, floor_data, sizeof(short) * num, &read, 0);
	return 1;
}

long LoadObjects(HANDLE file)
{
	OBJECT_INFO* obj;
	STATIC_INFO* sinfo;
	ulong read;
	long num, nAnims, slot, off;

	MyReadFile(file, &num, sizeof(long), &read, 0);
	mesh_base = (short*)game_malloc(sizeof(short) * num, 3);
	MyReadFile(file, mesh_base, sizeof(short) * num, &read, 0);

	MyReadFile(file, &num, sizeof(long), &read, 0);
	meshes = (short**)game_malloc(sizeof(short*) * num, 2);
	MyReadFile(file, meshes, sizeof(short*) * num, &read, 0);
	
	for (int i = 0; i < num; i++)
		meshes[i] = mesh_base + (long)meshes[i] / 2;

	MyReadFile(file, &nAnims, sizeof(long), &read, 0);
	anims = (ANIM_STRUCT*)game_malloc(sizeof(ANIM_STRUCT) * nAnims, 4);
	MyReadFile(file, anims, sizeof(ANIM_STRUCT) * nAnims, &read, 0);

	MyReadFile(file, &num, sizeof(long), &read, 0);
	changes = (CHANGE_STRUCT*)game_malloc(sizeof(CHANGE_STRUCT) * num, 5);
	MyReadFile(file, changes, sizeof(CHANGE_STRUCT) * num, &read, 0);

	MyReadFile(file, &num, sizeof(long), &read, 0);
	ranges = (RANGE_STRUCT*)game_malloc(sizeof(RANGE_STRUCT) * num, 6);
	MyReadFile(file, ranges, sizeof(RANGE_STRUCT) * num, &read, 0);

	MyReadFile(file, &num, sizeof(long), &read, 0);
	commands = (short*)game_malloc(sizeof(short) * num, 7);
	MyReadFile(file, commands, sizeof(short) * num, &read, 0);

	MyReadFile(file, &num, sizeof(long), &read, 0);
	bones = (long*)game_malloc(sizeof(long) * num, 8);
	MyReadFile(file, bones, sizeof(long) * num, &read, 0);

	MyReadFile(file, &num, sizeof(long), &read, 0);
	frames = (short*)game_malloc(sizeof(short) * num, 9);
	MyReadFile(file, frames, sizeof(short) * num, &read, 0);

	for (int i = 0; i < nAnims; i++)
		anims[i].frame_ptr = (short*)((long)anims[i].frame_ptr + (long)frames);

	MyReadFile(file, &num, sizeof(long), &read, 0);

	for (int i = 0; i < num; i++)
	{
		MyReadFile(file, &slot, sizeof(long), &read, 0);
		obj = &objects[slot];

		MyReadFile(file, &obj->nmeshes, sizeof(short), &read, 0);
		MyReadFile(file, &obj->mesh_index, sizeof(short), &read, 0);
		MyReadFile(file, &obj->bone_index, sizeof(long), &read, 0);

		MyReadFile(file, &off, sizeof(long), &read, 0);
		obj->frame_base = (short*)((long)frames + off);

		MyReadFile(file, &obj->anim_index, sizeof(short), &read, 0);
		obj->loaded = 1;
	}

	InitialiseObjects();

	MyReadFile(file, &num, sizeof(long), &read, 0);

	for (int i = 0; i < num; i++)
	{
		MyReadFile(file, &slot, sizeof(long), &read, 0);
		sinfo = &static_objects[slot];

		MyReadFile(file, &sinfo->mesh_number, sizeof(short), &read, 0);
		MyReadFile(file, &sinfo->x_minp, sizeof(short) * 6, &read, 0);
		MyReadFile(file, &sinfo->x_minc, sizeof(short) * 6, &read, 0);
		MyReadFile(file, &sinfo->flags, sizeof(short), &read, 0);
	}

	return 1;
}

long LoadSprites(HANDLE file)
{
	OBJECT_INFO* obj;
	ulong read;
	long num, slot;

	MyReadFile(file, &num, sizeof(long), &read, 0);
	MyReadFile(file, phdspriteinfo, sizeof(PHDSPRITESTRUCT) * num, &read, 0);

	MyReadFile(file, &num, sizeof(long), &read, 0);

	for (int i = 0; i < num; i++)
	{
		MyReadFile(file, &slot, sizeof(long), &read, 0);

		if (slot >= NUMBER_OBJECTS)
		{
			slot -= NUMBER_OBJECTS;
			SetFilePointer(file, sizeof(short), 0, FILE_CURRENT);
			MyReadFile(file, &static_objects[slot].mesh_number, sizeof(short), &read, 0);
		}
		else
		{
			obj = &objects[slot];
			MyReadFile(file, &obj->nmeshes, sizeof(short), &read, 0);
			MyReadFile(file, &obj->mesh_index, sizeof(short), &read, 0);
			obj->loaded = 1;
		}
	}

	return 1;
}

long LoadCameras(HANDLE file)
{
	ulong read;

	MyReadFile(file, &number_cameras, sizeof(long), &read, 0);

	if (number_cameras)
	{
		camera.fixed = (OBJECT_VECTOR*)game_malloc(sizeof(OBJECT_VECTOR) * number_cameras, 19);

		if (!camera.fixed)
			return 0;

		MyReadFile(file, camera.fixed, sizeof(OBJECT_VECTOR) * number_cameras, &read, 0);
	}

	return 1;
}

void inject_file(bool replace)
{
	INJECT(0x00480D50, MyReadFile, replace);
	INJECT(0x00481CA0, LoadPalette, replace);
	INJECT(0x00480DA0, LoadTexturePages, replace);
	INJECT(0x00480F70, LoadRooms, replace);
	INJECT(0x004813D0, LoadObjects, replace);
	INJECT(0x00481890, LoadSprites, replace);
	INJECT(0x00481D50, LoadCameras, replace);
}
