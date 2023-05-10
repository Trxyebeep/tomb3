#include "../tomb3/pch.h"
#include "file.h"
#include "dd.h"
#include "init.h"
#include "hwrender.h"
#include "game.h"
#include "../game/setup.h"
#include "../game/objects.h"
#include "../game/items.h"
#include "ds.h"
#include "specific.h"
#include "picture.h"
#include "smain.h"
#include "../game/gameflow.h"
#include "../game/box.h"
#include "../game/sound.h"
#include "../3dsystem/3d_gen.h"
#include "winmain.h"
#include "../game/demo.h"
#include "../game/control.h"
#include "../game/camera.h"
#include "../game/effects.h"
#include "../game/effect2.h"
#include "../game/cinema.h"
#if (DIRECT3D_VERSION >= 0x900)
#include "../newstuff/Picture2.h"
#endif
#include "../tomb3/tomb3.h"
//#include "../script/scripter.h"

//gameflow loading checks
#define LOAD_GF(main, allocSize, buffer, readSize)\
{\
main = (char**)GlobalAlloc(GMEM_FIXED, allocSize);\
if (!main) return 0;\
if (!Read_Strings(readSize, main, &buffer, &read, file)) return 0;\
}

CHANGE_STRUCT* changes;
RANGE_STRUCT* ranges;
short* aranges;
short* frames;
short* commands;
short* floor_data;
short* mesh_base;
long number_cameras;
long nTInfos;

PHDTEXTURESTRUCT phdtextinfo[MAX_TINFOS];
PHDSPRITESTRUCT phdspriteinfo[512];
uchar G_GouraudPalette[1024];
static uchar TexturesUVFlag[MAX_TINFOS];
static uchar game_palette[768];
static char LastLoadedLevelPath[256];

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
	uchar* p;
	ulong read;
	long nPages, size;
#if (DIRECT3D_VERSION < 0x900)
	bool _16bit;
#endif

	MyReadFile(file, &nPages, sizeof(long), &read, 0);

#if (DIRECT3D_VERSION >= 0x900)
	size = 0x20000;
#else
	_16bit = !App.lpDeviceInfo->DDInfo[App.lpDXConfig->nDD].D3DInfo[App.lpDXConfig->nD3D].Texture[App.lpDXConfig->D3DTF].bPalette;
	size = _16bit ? 0x20000 : 0x10000;
#endif
	p = (uchar*)GlobalAlloc(GMEM_FIXED, nPages * size);

	if (!p)
		return 0;

#if (DIRECT3D_VERSION >= 0x900)
	SetFilePointer(file, nPages << 16, 0, FILE_CURRENT);

	for (int i = 0; i < nPages; i++)
		MyReadFile(file, p + (size * i), size, &read, 0);

	HWR_LoadTexturePages(nPages, p, 0);
#else
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
#endif

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

	room = (ROOM_INFO*)game_malloc(sizeof(ROOM_INFO) * number_rooms);

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
		r->data = (short*)game_malloc(sizeof(short)* num);
		MyReadFile(file, r->data, sizeof(short) * num, &read, 0);

		MyReadFile(file, &nDoors, sizeof(short), &read, 0);

		if (!nDoors)
			r->door = 0;
		else
		{
			r->door = (short*)game_malloc((16 * nDoors + 1) * sizeof(short));
			r->door[0] = (short)nDoors;
			MyReadFile(file, r->door + 1, 16 * nDoors * sizeof(short), &read, 0);
		}

		MyReadFile(file, &r->x_size, sizeof(short), &read, 0);
		MyReadFile(file, &r->y_size, sizeof(short), &read, 0);
		num = r->x_size * r->y_size;
		r->floor = (FLOOR_INFO*)game_malloc(sizeof(FLOOR_INFO) * num);
		MyReadFile(file, r->floor, sizeof(FLOOR_INFO) * num, &read, 0);

		MyReadFile(file, &r->ambient, sizeof(short), &read, 0);
		MyReadFile(file, &r->lighting, sizeof(short), &read, 0);
		MyReadFile(file, &r->num_lights, sizeof(short), &read, 0);

		if (!r->num_lights)
			r->light = 0;
		else
		{
			r->light = (LIGHT_INFO*)game_malloc(sizeof(LIGHT_INFO) * r->num_lights);
			MyReadFile(file, r->light, sizeof(LIGHT_INFO) * r->num_lights, &read, 0);
		}

		MyReadFile(file, &r->num_meshes, sizeof(short), &read, 0);

		if (!r->num_meshes)
			r->mesh = 0;
		else
		{
			r->mesh = (MESH_INFO*)game_malloc(sizeof(MESH_INFO) * r->num_meshes);
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
	floor_data = (short*)game_malloc(sizeof(short) * num);
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
	mesh_base = (short*)game_malloc(sizeof(short) * num);
	MyReadFile(file, mesh_base, sizeof(short) * num, &read, 0);

	MyReadFile(file, &num, sizeof(long), &read, 0);
	meshes = (short**)game_malloc(sizeof(short*) * num);
	MyReadFile(file, meshes, sizeof(short*) * num, &read, 0);
	
	for (int i = 0; i < num; i++)
		meshes[i] = mesh_base + (long)meshes[i] / 2;

	MyReadFile(file, &nAnims, sizeof(long), &read, 0);
	anims = (ANIM_STRUCT*)game_malloc(sizeof(ANIM_STRUCT) * nAnims);
	MyReadFile(file, anims, sizeof(ANIM_STRUCT) * nAnims, &read, 0);

	MyReadFile(file, &num, sizeof(long), &read, 0);
	changes = (CHANGE_STRUCT*)game_malloc(sizeof(CHANGE_STRUCT) * num);
	MyReadFile(file, changes, sizeof(CHANGE_STRUCT) * num, &read, 0);

	MyReadFile(file, &num, sizeof(long), &read, 0);
	ranges = (RANGE_STRUCT*)game_malloc(sizeof(RANGE_STRUCT) * num);
	MyReadFile(file, ranges, sizeof(RANGE_STRUCT) * num, &read, 0);

	MyReadFile(file, &num, sizeof(long), &read, 0);
	commands = (short*)game_malloc(sizeof(short) * num);
	MyReadFile(file, commands, sizeof(short) * num, &read, 0);

	MyReadFile(file, &num, sizeof(long), &read, 0);
	bones = (long*)game_malloc(sizeof(long) * num);
	MyReadFile(file, bones, sizeof(long) * num, &read, 0);

	MyReadFile(file, &num, sizeof(long), &read, 0);
	frames = (short*)game_malloc(sizeof(short) * num);
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
		camera.fixed = (OBJECT_VECTOR*)game_malloc(sizeof(OBJECT_VECTOR) * number_cameras);

		if (!camera.fixed)
			return 0;

		MyReadFile(file, camera.fixed, sizeof(OBJECT_VECTOR) * number_cameras, &read, 0);
	}

	return 1;
}

long LoadSoundEffects(HANDLE file)
{
	ulong read;

	MyReadFile(file, &number_sound_effects, sizeof(long), &read, 0);

	if (number_sound_effects)
	{
		sound_effects = (OBJECT_VECTOR*)game_malloc(sizeof(OBJECT_VECTOR) * number_sound_effects);

		if (!sound_effects)
			return 0;

		MyReadFile(file, sound_effects, sizeof(OBJECT_VECTOR) * number_sound_effects, &read, 0);
	}

	return 1;
}

long LoadBoxes(HANDLE file)
{
	ulong read;
	long nOverlaps;

	MyReadFile(file, &number_boxes, sizeof(long), &read, 0);
	boxes = (BOX_INFO*)game_malloc(sizeof(BOX_INFO) * number_boxes);
	MyReadFile(file, boxes, sizeof(BOX_INFO) * number_boxes, &read, 0);

	if (read != sizeof(BOX_INFO) * number_boxes)
	{
		lstrcpy(exit_message, "LoadBoxes(): Unable to load boxes");
		return 0;
	}

	MyReadFile(file, &nOverlaps, sizeof(long), &read, 0);
	overlap = (short*)game_malloc(sizeof(short) * nOverlaps);
	MyReadFile(file, overlap, sizeof(short) * nOverlaps, &read, 0);

	if (read != sizeof(short) * nOverlaps)
	{
		lstrcpy(exit_message, "LoadBoxes(): Unable to load box overlaps");
		return 0;
	}

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			ground_zone[j][i] = (short*)game_malloc(sizeof(short) * number_boxes);
			MyReadFile(file, ground_zone[j][i], sizeof(short) * number_boxes, &read, 0);

			if (read != sizeof(short) * number_boxes)
			{
				lstrcpy(exit_message, "LoadBoxes(): Unable to load 'ground_zone'");
				return 0;
			}
		}

		fly_zone[i] = (short*)game_malloc(sizeof(short) * number_boxes);
		MyReadFile(file, fly_zone[i], sizeof(short) * number_boxes, &read, 0);

		if (read != sizeof(short) * number_boxes)
		{
			lstrcpy(exit_message, "LoadBoxes(): Unable to load 'fly_zone'");
			return 0;
		}
	}

	for (int i = 0; i < number_boxes; i++)
	{
		if (boxes[i].overlap_index & 0x8000)
			boxes[i].overlap_index |= 0x4000;
	}

	return 1;
}

long LoadAnimatedTextures(HANDLE file)
{
	short* uv;
	ulong read;
	long num;
	uchar flag;

	MyReadFile(file, &num, sizeof(long), &read, 0);
	aranges = (short*)game_malloc(sizeof(short) * num);
	MyReadFile(file, aranges, sizeof(short) * num, &read, 0);

	MyReadFile(file, &num, sizeof(long), &read, 0);

	if (num > MAX_TINFOS)
	{
		lstrcpy(exit_message, "Too many Textures in level");
		return 0;
	}

	MyReadFile(file, phdtextinfo, sizeof(PHDTEXTURESTRUCT) * num, &read, 0);
	nTInfos = num;

	for (int i = 0; i < num; i++)
	{
		uv = (short*)&phdtextinfo[i].u1;
		flag = 0;

		for (int j = 0; j < 8; j++)
		{
			if (uv[j] & 0x80)
			{
				uv[j] |= 0xFF;
				flag |= 1 << j;
			}
			else
				uv[j] &= 0xFF00;
		}

		TexturesUVFlag[i] = flag;
	}

	AdjustTextureUVs(1);
	return 1;
}

long LoadItems(HANDLE file)
{
	ITEM_INFO* item;
	ulong read;
	long num;

	MyReadFile(file, &num, sizeof(long), &read, 0);

	if (!num)
		return 1;

	if (num > MAX_ITEMS)
	{
		lstrcpy(exit_message, "LoadItems(): Too Many Items being Loaded!!");
		return 0;
	}

	items = (ITEM_INFO*)game_malloc(sizeof(ITEM_INFO) * MAX_ITEMS);

	if (!items)
	{
		lstrcpy(exit_message, "LoadItems(): Unable to allocate memory for 'items'");
		return 0;
	}

	level_items = num;
	InitialiseItemArray(MAX_ITEMS);

	for (int i = 0; i < level_items; i++)
	{
		item = &items[i];

		MyReadFile(file, &item->object_number, sizeof(short), &read, 0);
		MyReadFile(file, &item->room_number, sizeof(short), &read, 0);

		MyReadFile(file, &item->pos.x_pos, sizeof(long), &read, 0);
		MyReadFile(file, &item->pos.y_pos, sizeof(long), &read, 0);
		MyReadFile(file, &item->pos.z_pos, sizeof(long), &read, 0);
		MyReadFile(file, &item->pos.y_rot, sizeof(short), &read, 0);

		MyReadFile(file, &item->shade, sizeof(short), &read, 0);
		MyReadFile(file, &item->shadeB, sizeof(short), &read, 0);
		MyReadFile(file, &item->flags, sizeof(short), &read, 0);

		if (item->object_number < 0 || item->object_number > NUMBER_OBJECTS)
		{
			wsprintf(exit_message, "LoadItems(): Bad Object number (%d) on Item %d", item->object_number, i);
			return 0;
		}
	}

	for (int i = 0; i < level_items; i++)
		InitialiseItem(i);

	return 1;
}

long LoadDepthQ(HANDLE file)
{
	ulong read;
	static uchar water_palette[768];
	static char depthq_table[33][256];
	static char gouraud_table[256][32];

	MyReadFile(file, depthq_table, 0x2000, &read, 0);

	for (int i = 0; i < 33; i++)
		depthq_table[i][0] = 0;

	memcpy(depthq_table[32], depthq_table[24], 256);

	for (int i = 0; i < 32; i++)
	{
		for (int j = 0; j < 256; j++)
			gouraud_table[j][i] = depthq_table[i][j];
	}

	for (int i = 0; i < 768; i += 3)
	{
		water_palette[i] = 2 * game_palette[i] / 3;
		water_palette[i + 1] = 2 * game_palette[i + 1] / 3;
		water_palette[i + 2] = game_palette[i + 2];
	}

	return 1;
}

long LoadCinematic(HANDLE file)
{
	ulong read;

	MyReadFile(file, &num_cine_frames, sizeof(short), &read, 0);

	if (num_cine_frames)
	{
		cine = (short*)game_malloc(16 * num_cine_frames);
		MyReadFile(file, cine, 16 * num_cine_frames, &read, 0);
		cine_loaded = 1;
	}
	else
		cine_loaded = 0;

	return 1;
}

long LoadDemo(HANDLE file)
{
	ulong read;
	short size;

	democount = 0;
	demoptr = (ulong*)game_malloc(3608 * sizeof(ulong));
	MyReadFile(file, &size, sizeof(short), &read, 0);

	if (size)
	{
		MyReadFile(file, demoptr, size, &read, 0);
		demo_loaded = 1;
	}
	else
		demo_loaded = 0;

	return 1;
}

long LoadSamples(HANDLE file)
{
	LPWAVEFORMATEX fmt;
	HANDLE sfxFile;
	char* data;
	ulong read;
	long nSamples, size, fSize;
	long used_samples[500];	//the samples the level actually needs to load from main.sfx
	long header[11];	//sample file header

	sound_active = 0;

	if (!DS_IsSoundEnabled())
		return 1;

	DS_FreeAllSamples();
	MyReadFile(file, sample_lut, sizeof(short) * 370, &read, 0);
	MyReadFile(file, &num_sample_infos, sizeof(long), &read, 0);

	if (!num_sample_infos)
		return 0;

	sample_infos = (SAMPLE_INFO*)game_malloc(sizeof(SAMPLE_INFO) * num_sample_infos);
	MyReadFile(file, sample_infos, sizeof(SAMPLE_INFO) * num_sample_infos, &read, 0);
	MyReadFile(file, &nSamples, sizeof(long), &read, 0);

	if (!nSamples)
		return 0;

	MyReadFile(file, used_samples, sizeof(long) * nSamples, &read, 0);

	if (tomb3.gold)
		sfxFile = CreateFile(GetFullPath("datag\\main.sfx"), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	else
		sfxFile = CreateFile(GetFullPath("data\\main.sfx"), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (sfxFile == INVALID_HANDLE_VALUE)
	{
		wsprintf(exit_message, "Could not open MAIN.SFX file");
		return 0;
	}

	for (int i = 0, n = 0; n < nSamples; i++)
	{
		MyReadFile(sfxFile, header, sizeof(long) * 11, &read, 0);

		if (header[0] != 'FFIR' || header[2] != 'EVAW' || header[9] != 'atad')	//RIFF, WAVE, data. todo: make this look better
			return 0;

		fmt = (LPWAVEFORMATEX)&header[5];
		size = header[10];
		fSize = (size + 1) & ~1;
		fmt->cbSize = 0;

		if (used_samples[n] == i)
		{
			data = (char*)game_malloc(fSize);
			MyReadFile(sfxFile, data, fSize, &read, 0);

			if (!DS_MakeSample(n, fmt, data, size))
				return 0;

			game_free(fSize);
			n++;
		}
		else
			SetFilePointer(sfxFile, fSize, 0, FILE_CURRENT);
	}

	CloseHandle(sfxFile);
	sound_active = 1;
	return 1;
}

void LoadDemFile(const char* name)
{
	HANDLE file;
	ulong read;
	char buf[80];

	strcpy(buf, name);
	build_ext(buf, "DEM");
	file = CreateFile(buf, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (file != INVALID_HANDLE_VALUE)
	{
		MyReadFile(file, demoptr, 3608 * sizeof(ulong), &read, 0);
		demo_loaded = read != 0;
		CloseHandle(file);
	}
}

long LoadLevel(const char* name, long number)
{
	HANDLE file;
	const char* path;
	ulong read;
	long version, level_num;
	char dem[80];

	path = GetFullPath(name);
	strcpy(LastLoadedLevelPath, path);
	init_game_malloc();

	file = CreateFile(path, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, 0);

	if (file == INVALID_HANDLE_VALUE)
	{
		wsprintf(exit_message, "LoadLevel(): Could not open %s (level %d)", path, number);
		return 0;
	}

	MyReadFile(file, &version, sizeof(long), &read, 0);

	if (!LoadPalette(file))
		return 0;

	if (!LoadTexturePages(file))
		return 0;

	MyReadFile(file, &level_num, sizeof(long), &read, 0);

	if (!LoadRooms(file))
		return 0;

	if (!LoadObjects(file))
		return 0;

	if (!LoadSprites(file))
		return 0;

	if (!LoadCameras(file))
		return 0;

	if (!LoadSoundEffects(file))
		return 0;

	if (!LoadBoxes(file))
		return 0;

	if (!LoadAnimatedTextures(file))
		return 0;

	if (!LoadItems(file))
		return 0;

	if (!LoadDepthQ(file))
		return 0;

	if (!LoadCinematic(file))
		return 0;

	if (!LoadDemo(file))
		return 0;

	strcpy(dem, path);

	if (!LoadSamples(file))
		return 0;

	LoadDemFile(dem);
	CloseHandle(file);
	return 1;
}

void S_UnloadLevelFile()
{
	HWR_FreeTexturePages();
	LastLoadedLevelPath[0] = 0;
	nTInfos = 0;
}

long S_LoadLevelFile(char* name, long number, long type)
{
	long loaded;
	bool fade;
	char buf[128];

	S_UnloadLevelFile();
	S_CDStop();
	fade = 0;

	if (type && type != 6 && type != 3 && (type != 4 || GF_Playing_Story))
	{
		strcpy(buf, GF_picfilenames[GF_LoadingPic]);

		if (tomb3.gold)
			T3_GoldifyString(buf);

#if (DIRECT3D_VERSION >= 0x900)
		LoadPicture(buf);
#else
		LoadPicture(buf, App.PictureBuffer);
#endif
		FadePictureUp(32);
		fade = 1;
	}

	loaded = LoadLevel(name, number);

	if (fade)
		FadePictureDown(32);

	return loaded;
}

const char* GetFullPath(const char* name)
{
	static char path[128];

	wsprintf(path, "%s", name);
	return path;
}

void build_ext(char* name, const char* ext)
{
	char* p;

	p = name;

	while (*p && *p != '.')
		p++;

	*p++ = '.';
	*p++ = ext[0];
	*p++ = ext[1];
	*p++ = ext[2];
	*p = 0;
}

void AdjustTextureUVs(bool reset)
{
	short* uv;
	long num;
	uchar flag;

	if (reset)
		App.nUVAdd = 0;

	num = 256 - App.nUVAdd;

	for (int i = 0; i < nTInfos; i++)
	{
		uv = (short*)&phdtextinfo[i].u1;
		flag = TexturesUVFlag[i];

		for (int j = 0; j < 8; j++)
		{
			if (flag & 1)
				uv[j] -= (short)num;
			else
				uv[j] += (short)num;

			flag >>= 1;
		}
	}

	App.nUVAdd += num;
}

long Read_Strings(long num, char** strings, char** buffer, ulong* read, HANDLE file)
{
	short size;
	char* p;

	MyReadFile(file, GF_Offsets, sizeof(short) * num, read, 0);
	MyReadFile(file, &size, sizeof(short), read, 0);
	*buffer = (char*)GlobalAlloc(GMEM_FIXED, size);

	if (!*buffer)
		return 0;

	MyReadFile(file, *buffer, size, read, 0);

	if (gameflow.cyphered_strings)
	{
		p = *buffer;

		for (int i = 0; i < size; i++)
			p[i] ^= gameflow.cypher_code;
	}

	for (int i = 0; i < num; i++)
		strings[i] = *buffer + GF_Offsets[i];

	return 1;
}

long S_LoadGameFlow(const char* name)
{
	HANDLE file;
	ulong read;
	short num;

	name = GetFullPath(name);
	file = CreateFile(name, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (file == INVALID_HANDLE_VALUE)
		return 0;

	MyReadFile(file, &GF_ScriptVersion, sizeof(long), &read, 0);

	if (GF_ScriptVersion != 3)
		return 0;

	MyReadFile(file, GF_Description, 256, &read, 0);
	MyReadFile(file, &num, sizeof(short), &read, 0);

	if (num != sizeof(GAMEFLOW_INFO))
		return 0;

	MyReadFile(file, &gameflow, sizeof(GAMEFLOW_INFO), &read, 0);

	LOAD_GF(GF_Level_Names, sizeof(char*) * gameflow.num_levels, GF_levelnames_buffer, gameflow.num_levels)
	LOAD_GF(GF_picfilenames, sizeof(char*) * gameflow.num_picfiles, GF_picfilenames_buffer, gameflow.num_picfiles)
	LOAD_GF(GF_titlefilenames, sizeof(char*) * gameflow.num_titlefiles, GF_titlefilenames_buffer, gameflow.num_titlefiles)
	LOAD_GF(GF_fmvfilenames, sizeof(char*) * gameflow.num_fmvfiles, GF_fmvfilenames_buffer, gameflow.num_fmvfiles)
	LOAD_GF(GF_levelfilenames, sizeof(char*) * gameflow.num_levels, GF_levelfilenames_buffer, gameflow.num_levels)
	LOAD_GF(GF_cutscenefilenames, sizeof(char*) * gameflow.num_cutfiles, GF_cutscenefilenames_buffer, gameflow.num_cutfiles)

	MyReadFile(file, GF_Offsets, sizeof(short) * (gameflow.num_levels + 1), &read, 0);
	MyReadFile(file, &num, sizeof(short), &read, 0);
	GF_sequence_buffer = (short*)GlobalAlloc(GMEM_FIXED, num);

	if (!GF_sequence_buffer)
		return 0;

	MyReadFile(file, GF_sequence_buffer, num, &read, 0);
	GF_frontendSequence = GF_sequence_buffer;

	for (int i = 0; i < gameflow.num_levels; i++)
		GF_level_sequence_list[i] = GF_sequence_buffer + (GF_Offsets[i + 1] / 2);

	if (gameflow.num_demos)
		MyReadFile(file, GF_valid_demos, sizeof(short) * gameflow.num_demos, &read, 0);

	MyReadFile(file, &num, sizeof(short), &read, 0);

	if (num != GT_NUM_GAMESTRINGS)
		return 0;

	LOAD_GF(GF_GameStrings, sizeof(char*) * GT_NUM_GAMESTRINGS, GF_GameStrings_buffer, GT_NUM_GAMESTRINGS)
	LOAD_GF(GF_PCStrings, sizeof(char*) * PCSTR_NUM_STRINGS, GF_PCStrings_buffer, PCSTR_NUM_STRINGS)

	LOAD_GF(GF_Puzzle1Strings, sizeof(char*) * gameflow.num_levels, GF_Puzzle1Strings_buffer, gameflow.num_levels)
	LOAD_GF(GF_Puzzle2Strings, sizeof(char*) * gameflow.num_levels, GF_Puzzle2Strings_buffer, gameflow.num_levels)
	LOAD_GF(GF_Puzzle3Strings, sizeof(char*) * gameflow.num_levels, GF_Puzzle3Strings_buffer, gameflow.num_levels)
	LOAD_GF(GF_Puzzle4Strings, sizeof(char*) * gameflow.num_levels, GF_Puzzle4Strings_buffer, gameflow.num_levels)

	LOAD_GF(GF_Pickup1Strings, sizeof(char*) * gameflow.num_levels, GF_Pickup1Strings_buffer, gameflow.num_levels)
	LOAD_GF(GF_Pickup2Strings, sizeof(char*) * gameflow.num_levels, GF_Pickup2Strings_buffer, gameflow.num_levels)

	LOAD_GF(GF_Key1Strings, sizeof(char*) * gameflow.num_levels, GF_Key1Strings_buffer, gameflow.num_levels)
	LOAD_GF(GF_Key2Strings, sizeof(char*) * gameflow.num_levels, GF_Key2Strings_buffer, gameflow.num_levels)
	LOAD_GF(GF_Key3Strings, sizeof(char*) * gameflow.num_levels, GF_Key3Strings_buffer, gameflow.num_levels)
	LOAD_GF(GF_Key4Strings, sizeof(char*) * gameflow.num_levels, GF_Key4Strings_buffer, gameflow.num_levels)

	CloseHandle(file);

//	OutputScript();

	return 1;
}
