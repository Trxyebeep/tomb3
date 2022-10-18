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

void ShutdownGame()
{
	GLOBALFREE(TLVertexBuffer);
	GLOBALFREE(TLUnRollBuffer);
	DXFreeTPages();
	ACMClose();
	DXSetCooperativeLevel(App.lpDD, App.WindowHandle, DDSCL_NORMAL);

	if (malloc_buffer)
		GLOBALFREE(malloc_buffer);

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

void inject_init(bool replace)
{
	INJECT(0x00485EA0, ShutdownGame, replace);
	INJECT(0x00486050, CalculateWibbleTable, replace);
	INJECT(0x00485CA0, GetRandom, replace);
	INJECT(0x00485AB0, init_water_table, replace);
}
