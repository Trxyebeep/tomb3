#include "../tomb3/pch.h"
#include "winmain.h"
#include "dxshell.h"
#include "dx.h"
#include "drawprimitive.h"
#include "hwrender.h"
#include "texture.h"
#include "init.h"

bool WinDXInit(DEVICEINFO* device, DXCONFIG* config, bool createNew)
{
	DISPLAYMODE* dm;
	DIRECT3DINFO* d3d;
	DDSURFACEDESC desc;
	D3DMATERIAL m;
	DDSCAPS caps;
	D3DMATERIALHANDLE handle;

	App.nRenderMode = 1;

	if (createNew)
	{
		if (!DXCreateDirectDraw(device, config, &App.lpDD) || !DXCreateDirect3D(App.lpDD, &App.lpD3D))
			return 0;
	}

	if (!DXSetCooperativeLevel(App.lpDD, App.WindowHandle, DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE))
		return 0;

	dm = &device->DDInfo[config->nDD].D3DInfo[config->nD3D].DisplayMode[config->nVMode];

	if (!DXSetVideoMode(App.lpDD, dm->w, dm->h, dm->bpp))
		return 0;

	memset(&desc, 0, sizeof(DDSURFACEDESC));
	desc.dwSize = sizeof(DDSURFACEDESC);
	desc.dwBackBufferCount = 1;
	desc.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	desc.ddsCaps.dwCaps = DDSCAPS_COMPLEX | DDSCAPS_FLIP | DDSCAPS_PRIMARYSURFACE | DDSCAPS_3DDEVICE;

	if (!DXCreateSurface(App.lpDD, &desc, (LPDIRECTDRAWSURFACE3)&App.lpFrontBuffer))
		return 0;

	App.lpFrontBuffer->GetSurfaceDesc(&desc);
	d3d = &device->DDInfo[config->nDD].D3DInfo[config->nD3D];
	dm = &d3d->DisplayMode[config->nVMode];

	DXBitMask2ShiftCnt(desc.ddpfPixelFormat.dwRBitMask, &dm->rshift, &dm->rbpp);
	DXBitMask2ShiftCnt(desc.ddpfPixelFormat.dwGBitMask, &dm->gshift, &dm->gbpp);
	DXBitMask2ShiftCnt(desc.ddpfPixelFormat.dwBBitMask, &dm->bshift, &dm->bbpp);

	if (d3d->bHardware)
	{
		caps.dwCaps = DDSCAPS_BACKBUFFER;

		if (!DXGetAttachedSurface(App.lpFrontBuffer, &caps, &App.lpBackBuffer))
			return 0;
	}
	else
	{
		App.unk = malloc(4 * dm->w * dm->h);
		caps.dwCaps = DDSCAPS_BACKBUFFER;
		DXGetAttachedSurface(App.lpFrontBuffer, &caps, &App.lpBackBuffer);
	}

	if (!DXCreateZBuffer(device, config))
		return 0;

	if (!DXCreateDirect3DDevice(App.lpD3D, d3d->Guid, App.lpBackBuffer, &App.lpD3DDevice))
		return 0;

	dm = &device->DDInfo[config->nDD].D3DInfo[config->nD3D].DisplayMode[config->nVMode];

	if (!DXCreateViewPort(App.lpD3D, App.lpD3DDevice, dm->w, dm->h, &App.lpViewPort))
		return 0;

	memset(&m, 0, sizeof(D3DMATERIAL));
	m.dwSize = sizeof(D3DMATERIAL);

	App.lpD3D->CreateMaterial(&App.lpViewPortMaterial, 0);
	App.lpViewPortMaterial->SetMaterial(&m);
	App.lpViewPortMaterial->GetHandle(App.lpD3DDevice, &handle);
	App.lpViewPort->SetBackground(handle);

	memset(&desc, 0, sizeof(DDSURFACEDESC));
	desc.dwSize = sizeof(DDSURFACEDESC);
	desc.dwWidth = 640;
	desc.dwHeight = 480;
	desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
	desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
	DXCreateSurface(App.lpDD, &desc, (LPDIRECTDRAWSURFACE3)&App.lpPictureBuffer);
	DX_ClearBuffers(11, 0);
	InitDrawPrimitive(App.lpD3DDevice, App.lpBackBuffer, App.DeviceInfoPtr->DDInfo[App.DXConfigPtr->nDD].D3DInfo[App.DXConfigPtr->nD3D].bHardware);
	HWR_InitState();

	if (device->DDInfo[config->nDD].D3DInfo[config->nD3D].bHardware)
	{
		DXCreateMaxTPages(1);

		if (!nTPages)
			return 0;
	}

	return 1;
}

void WinAppExit()
{
	ShutdownGame();
	exit(0);
}

LRESULT CALLBACK WinAppProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		ShowCursor(0);
		break;

	case WM_ACTIVATE:

		switch (LOWORD(wParam))
		{
		case WA_INACTIVE:
			App.bFocus = 0;
			break;

		case WA_ACTIVE:
		case WA_CLICKACTIVE:
			App.bFocus = 1;
			break;
		}

		break;

	case WM_CLOSE:
		WinAppExit();
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

long WinRegisterWindow(HINSTANCE hinstance)
{
	App.WindowClass.hIcon = LoadIconA(hinstance, (LPCSTR)115);		//todo: icon resource define
	App.WindowClass.lpszMenuName = 0;
	App.WindowClass.lpszClassName = "Window Class";
	App.WindowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	App.WindowClass.hInstance = hinstance;
	App.WindowClass.style = CS_VREDRAW | CS_HREDRAW;
	App.WindowClass.lpfnWndProc = WinAppProc;
	App.WindowClass.cbClsExtra = 0;
	App.WindowClass.cbWndExtra = 0;
	return RegisterClass(&App.WindowClass);
}

void inject_winmain(bool replace)
{
	INJECT(0x004B2F80, WinDXInit, replace);
	INJECT(0x004B2C50, WinAppExit, replace);
	INJECT(0x004B2E10, WinAppProc, replace);
	INJECT(0x004B2D40, WinRegisterWindow, replace);
}
