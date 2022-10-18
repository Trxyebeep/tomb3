#include "../tomb3/pch.h"
#include "init.h"
#include "texture.h"
#include "audio.h"
#include "dxshell.h"
#include "di.h"
#include "ds.h"
#include "drawprimitive.h"
#include "winmain.h"

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
}

void inject_init(bool replace)
{
	INJECT(0x00485EA0, ShutdownGame, replace);
}
