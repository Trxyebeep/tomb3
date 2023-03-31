#include "../tomb3/pch.h"
#include "init.h"
#include "texture.h"
#include "audio.h"
#include "dxshell.h"
#include "di.h"
#include "ds.h"
#include "drawprimitive.h"
#include "winmain.h"
#include "../3dsystem/phd_math.h"
#include "game.h"
#include "transform.h"
#include "../3dsystem/hwinsert.h"
#include "picture.h"
#include "smain.h"
#include "display.h"
#include "fmv.h"
#include "../newstuff/discord.h"

const char* game_malloc_types[47] =
{
	"Temp Alloc",
	"Texture Pages",
	"Mesh Pointers",
	"Meshes",
	"Anims",
	"Structs",
	"Ranges",
	"Commands",
	"Bones",
	"Frames",
	"Room Textures",
	"Room Infos",
	"Room Mesh",
	"Room Door",
	"Room Floor",
	"Room Lights",
	"Room Static Mesh Infos",
	"Floor Data",
	"ITEMS!!",
	"Cameras",
	"Sound FX",
	"Boxes",
	"Overlaps",
	"GroundZone",
	"FlyZone",
	"Animating Texture Ranges",
	"Cinematic Frames",
	"LoadDemo Buffer",
	"SaveDemo Buffer",
	"Cinematic Effects",
	"Mummy Head Turn",
	"Extra Door stuff",
	"Effects_Array",
	"Creature Data",
	"Creature LOT",
	"Sample Infos",
	"Samples",
	"Sample Offsets",
	"Rolling Ball Stuff",
	"Skidoo Stuff",
	"Load Piccy Buffer",
	"FMV Buffers",
	"Polygon Buffers",
	"Order Tables",
	"CLUTs",
	"Texture Infos",
	"Sprite Infos"
};

char* malloc_ptr;
char* malloc_buffer;
static long malloc_free;
static long malloc_size;
static long malloc_used;

D3DTLVERTEX* CurrentTLVertex;
D3DTLVERTEX* VertexBuffer;
D3DTLVERTEX* UnRollBuffer;

static D3DTLVERTEX* TLVertexBuffer;
static D3DTLVERTEX* TLUnRollBuffer;

long RColorTable[33][33][33];
long GColorTable[33][33][33];
long BColorTable[33][33][33];
WATERTAB WaterTable[22][64];
float wibble_table[32];

static short shade_table[32];
static long rand_table[32];
static long wibble_light[32][32];
static long SqrtTable[1024];

void ShutdownGame()
{
	RPC_close();
	FreeWinPlay();
	GlobalFree(TLVertexBuffer);
	GlobalFree(TLUnRollBuffer);
	DXFreeTPages();
	ACMClose();
	DXSetCooperativeLevel(App.lpDD, App.WindowHandle, DDSCL_NORMAL);

	if (malloc_buffer)
		GlobalFree(malloc_buffer);

	DXClearAllTextures(PictureTextures);
	DI_Finish();
	DS_Finish();

	if (!App.DeviceInfoPtr->DDInfo[App.DXConfigPtr->nDD].D3DInfo[App.DXConfigPtr->nD3D].bHardware)
		CloseDrawPrimitive();

	WinFreeDX(1);
	DXFreeDeviceInfo(&App.DeviceInfo);

#ifdef DO_LOG
	if (logF)
		fclose(logF);
#endif
}

void CalculateWibbleTable()
{
	long sin;

	for (int i = 0; i < 32; i++)
	{
		sin = phd_sin(i * 0x10000 / 32);
		wibble_table[i] = float((2 * sin) >> W2V_SHIFT);
		shade_table[i] = short((768 * sin) >> W2V_SHIFT);
		rand_table[i] = (GetRandomDraw() >> 5) - 511;

		for (int j = 0; j < 31; j++)
			wibble_light[i][j] = ((j - 16 * i) << 9) / 31;
	}
}

ushort GetRandom(WATERTAB* wt, long lp)
{
	long loop;
	ushort ret;

	do
	{
		ret = rand() & 0xFC;

		for (loop = 0; loop < lp; loop++)
			if (wt[loop].random == ret)
				break;

	} while (loop != lp);

	return ret;
}

void init_water_table()
{
	short sin;
	static short water_shimmer[4] = { 31, 63, 95, 127 };
	static short water_choppy[4] = { 16, 53, 90, 127 };
	static uchar water_abs[4] = { 4, 8, 12, 16 };

	srand(121197);

	for (int i = 0; i < 64; i++)
	{
		sin = rcossin_tbl[i << 7];
		WaterTable[0][i].shimmer = (63 * sin) >> 15;
		WaterTable[0][i].choppy = (16 * sin) >> 12;
		WaterTable[0][i].random = (uchar)GetRandom(&WaterTable[0][0], i);
		WaterTable[0][i].abs = 0;

		WaterTable[1][i].shimmer = (32 * sin) >> 15;
		WaterTable[1][i].choppy = 0;
		WaterTable[1][i].random = (uchar)GetRandom(&WaterTable[1][0], i);
		WaterTable[1][i].abs = -3;

		WaterTable[2][i].shimmer = (64 * sin) >> 15;
		WaterTable[2][i].choppy = 0;
		WaterTable[2][i].random = (uchar)GetRandom(&WaterTable[2][0], i);
		WaterTable[2][i].abs = 0;

		WaterTable[3][i].shimmer = (96 * sin) >> 15;
		WaterTable[3][i].choppy = 0;
		WaterTable[3][i].random = (uchar)GetRandom(&WaterTable[3][0], i);
		WaterTable[3][i].abs = 4;

		WaterTable[4][i].shimmer = (127 * sin) >> 15;
		WaterTable[4][i].choppy = 0;
		WaterTable[4][i].random = (uchar)GetRandom(&WaterTable[4][0], i);
		WaterTable[4][i].abs = 8;

		for (int j = 0, k = 5; j < 4; j++, k += 4)
		{
			for (int m = 0; m < 4; m++)
			{
				WaterTable[k + m][i].shimmer = -((sin * water_shimmer[m]) >> 15);
				WaterTable[k + m][i].choppy = sin * water_choppy[j] >> 12;
				WaterTable[k + m][i].random = (uchar)GetRandom(&WaterTable[k + m][0], i);
				WaterTable[k + m][i].abs = water_abs[m];
			}
		}
	}
}

void init_game_malloc()
{
	malloc_ptr = malloc_buffer;
	malloc_free = malloc_size;
	malloc_used = 0;
}

void* game_malloc(long size, long type)
{
	void* ptr;

	size = (size + 3) & ~3;

	if (size > malloc_free)
	{
		wsprintf(exit_message, "game_malloc(): OUT OF MEMORY %s %d", game_malloc_types[type], size);
		S_ExitSystem(exit_message);
	}

	ptr = malloc_ptr;
	malloc_free -= size;
	malloc_used += size;
	malloc_ptr += size;
	return ptr;
}

void game_free(long size, long type)
{
	size = (size + 3) & ~3;
	malloc_ptr -= size;
	malloc_free += size;
	malloc_used -= size;
}

long S_InitialiseSystem()
{
	DISPLAYMODE* dm;
	long cval;

	dm = &App.DeviceInfoPtr->DDInfo[App.DXConfigPtr->nDD].D3DInfo[App.DXConfigPtr->nD3D].DisplayMode[App.DXConfigPtr->nVMode];
	DumpX = 0;
	DumpY = 0;
	DumpWidth = (short)dm->w;
	DumpHeight = (short)dm->h;
	InitZTable();
	InitUVTable();

	TLVertexBuffer = (D3DTLVERTEX*)GlobalAlloc(GMEM_FIXED, MAX_TLVERTICES * sizeof(D3DTLVERTEX));
	VertexBuffer = (D3DTLVERTEX*)(((long)TLVertexBuffer + 32) & 0xFFFFFFE0);

	TLUnRollBuffer = (D3DTLVERTEX*)GlobalAlloc(GMEM_FIXED, MAX_TLVERTICES * sizeof(D3DTLVERTEX));
	UnRollBuffer = (D3DTLVERTEX*)(((long)TLUnRollBuffer + 32) & 0xFFFFFFE0);

	for (int i = 0; i < 1024; i++)
		SqrtTable[i] = (long)sqrt((float)i);

	for (int a = 1; a < 33; a++)
	{
		for (int b = 1; b < 33; b++)
		{
			for (int c = 1; c < 33; c++)
			{
				cval = ((a - b) * c) / a;
				RColorTable[a][b][c] = cval << 10;
				GColorTable[a][b][c] = cval << 5;
				BColorTable[a][b][c] = cval;
			}
		}
	}

	malloc_size = MALLOC_SIZE;
	return 1;
}
