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

void inject_init(bool replace)
{
	INJECT(0x00485EA0, ShutdownGame, replace);
	INJECT(0x00486050, CalculateWibbleTable, replace);
}
