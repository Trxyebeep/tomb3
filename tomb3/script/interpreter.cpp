#include "../tomb3/pch.h"
#include "interpreter.h"
#include "../game/gameflow.h"

static const char* inv_item_refs[] =
{
	"PISTOLS",
	"SHOTGUN",
	"AUTOPISTOLS",
	"UZIS",
	"HARPOON",
	"M16",
	"ROCKET",
	"GRENADE",
	"PISTOLS_AMMO",
	"SHOTGUN_AMMO",
	"AUTOPISTOLS_AMMO",
	"UZI_AMMO",
	"HARPOON_AMMO",
	"M16_AMMO",
	"ROCKET_AMMO",
	"GRENADE_AMMO",
	"FLARES",
	"MEDI",
	"BIGMEDI",
	"PICKUP1",
	"PICKUP2",
	"PUZZLE1",
	"PUZZLE2",
	"PUZZLE3",
	"PUZZLE4",
	"KEY1",
	"KEY2",
	"KEY3",
	"KEY4",
	"CRYSTAL"
};

static const char* death_tile_refs[] =
{
	"Lava",
	"Rapids",
	"Electric"
};

static void WriteItems(FILE* fp, long level)
{
	char str[128];

	sprintf(str, GF_Pickup1Strings[level]);

	if (strncmp(str, "P1", 2))
		fprintf(fp, "Pickup1: %s\n", str);

	sprintf(str, GF_Pickup2Strings[level]);

	if (strncmp(str, "P2", 2))
		fprintf(fp, "Pickup2: %s\n", str);

	sprintf(str, GF_Puzzle1Strings[level]);

	if (strncmp(str, "P1", 2))
		fprintf(fp, "Puzzle1: %s\n", str);

	sprintf(str, GF_Puzzle2Strings[level]);

	if (strncmp(str, "P2", 2))
		fprintf(fp, "Puzzle2: %s\n", str);

	sprintf(str, GF_Puzzle3Strings[level]);

	if (strncmp(str, "P3", 2))
		fprintf(fp, "Puzzle3: %s\n", str);

	sprintf(str, GF_Puzzle4Strings[level]);

	if (strncmp(str, "P4", 2))
		fprintf(fp, "Puzzle4: %s\n", str);

	sprintf(str, GF_Key1Strings[level]);

	if (strncmp(str, "K1", 2))
		fprintf(fp, "Key1: %s\n", str);

	sprintf(str, GF_Key2Strings[level]);

	if (strncmp(str, "K2", 2))
		fprintf(fp, "Key2: %s\n", str);

	sprintf(str, GF_Key3Strings[level]);

	if (strncmp(str, "K3", 2))
		fprintf(fp, "Key3: %s\n", str);

	sprintf(str, GF_Key4Strings[level]);

	if (strncmp(str, "K4", 2))
		fprintf(fp, "Key4: %s\n", str);
}

void interpretSequence(FILE* fp, short* ptr, long level)
{
	long c;

	while (*ptr != GFE_END_SEQ)
	{
		switch (*ptr)
		{
		case GFE_PICTURE:
			ptr += 2;
			break;

		case GFE_LIST_START:
		case GFE_LIST_END:
			ptr++;
			break;

		case GFE_PLAYFMV:
			fprintf(fp, "fmv: %s\n", GF_fmvfilenames[ptr[1]]);
			ptr += 2;
			break;

		case GFE_STARTLEVEL:
			fprintf(fp, "game: %s\n", GF_levelfilenames[ptr[1]]);
			ptr += 2;

			WriteItems(fp, level);

			break;

		case GFE_CUTSCENE:
			fprintf(fp, "cut: %s\n", GF_cutscenefilenames[ptr[1]]);
			ptr += 2;
			break;

		case GFE_LEVCOMPLETE:
			fprintf(fp, "complete:\n");
			ptr++;
			break;

		case GFE_DEMOPLAY:
			fprintf(fp, "demo: %s\n", GF_levelfilenames[ptr[1]]);
			ptr += 2;
			break;

		case GFE_JUMPTO_SEQ:
			ptr += 2;
			break;

		case GFE_SETTRACK:
			fprintf(fp, "track: %i\n", ptr[1]);
			ptr += 2;
			break;

		case GFE_SUNSET:
			fprintf(fp, "sunset:\n");
			ptr++;
			break;

		case GFE_LOADINGPIC:
			fprintf(fp, "Load_pic: %s\n", GF_picfilenames[ptr[1]]);
			ptr += 2;
			break;

		case GFE_DEADLY_WATER:
			fprintf(fp, "deadly_water:\n");
			ptr++;
			break;

		case GFE_REMOVE_WEAPONS:
			fprintf(fp, "remove_weapons:\n");
			ptr++;
			break;

		case GFE_REMOVE_AMMO:
			fprintf(fp, "remove_ammo:\n");
			ptr++;
			break;

		case GFE_GAMECOMPLETE:
			fprintf(fp, "gamecomplete:\n");
			ptr++;
			break;

		case GFE_CUTANGLE:
			fprintf(fp, "cutangle: %i\n", (ushort)ptr[1]);
			ptr += 2;
			break;

		case GFE_NOFLOOR:
			fprintf(fp, "NoFloor: %i\n", ptr[1]);
			ptr += 2;
			break;

		case GFE_ADD2INV:

			if (ptr[1] < 1000)
				fprintf(fp, "Bonus: %s\n", inv_item_refs[ptr[1]]);
			else
				fprintf(fp, "StartInv: %s\n", inv_item_refs[ptr[1] - 1000]);

			ptr += 2;
			break;

		case GFE_STARTANIM:
			fprintf(fp, "StartAnim: %i\n", ptr[1]);
			ptr += 2;
			break;

		case GFE_NUMSECRETS:
			fprintf(fp, "Secrets: %i\n", ptr[1]);
			ptr += 2;
			break;

		case GFE_KILL2COMPLETE:
			fprintf(fp, "Kill2Complete:\n");
			ptr++;
			break;

		case GFE_RAIN:
			fprintf(fp, "Rain:\n");
			ptr++;
			break;

		case GFE_SNOW:
			fprintf(fp, "Snow:\n");
			ptr++;
			break;

		case GFE_WATER_PARTS:
			fprintf(fp, "WaterParts:\n");
			ptr++;
			break;

		case GFE_COLD:
			fprintf(fp, "Cold:\n");
			ptr++;
			break;

		case GFE_DEATHTILE:
			fprintf(fp, "Death: %s\n", death_tile_refs[ptr[1]]);
			ptr += 2;
			break;

		case GFE_WATERCLR:
			c = (ushort)ptr[1] | (ushort)ptr[2] << 16;
			fprintf(fp, "WaterClr: %i, %i, %i\n", RGB_GETRED(c), RGB_GETGREEN(c), RGB_GETBLUE(c));
			ptr += 3;
			break;
		}
	}
}
