#include "../tomb3/pch.h"
#include "winmain.h"
#include "dxshell.h"
#include "drawprimitive.h"
#include "hwrender.h"
#include "texture.h"
#include "init.h"

#ifdef DO_LOG
FILE* logF = 0;
#endif

bool WinDXInit(DEVICEINFO* device, DXCONFIG* config, bool createNew)
{
	DISPLAYMODE* dm;
	DIRECT3DINFO* d3d;
	DDSURFACEDESC desc;
	D3DMATERIAL m;
	DDSCAPS caps;
	D3DMATERIALHANDLE handle;

	Log("Starting WinDXInit");
	App.nRenderMode = 1;

	if (createNew)
	{
		if (!DXCreateDirectDraw(device, config, &App.lpDD) || !DXCreateDirect3D(App.lpDD, &App.lpD3D))
		{
			Log("Failed to create DirectDraw or Direct3D, exitting..");
			return 0;
		}
	}

	if (!DXSetCooperativeLevel(App.lpDD, App.WindowHandle, DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE))
	{
		Log("DXSetCooperativeLevel failed: DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE, exitting..");
		return 0;
	}

	dm = &device->DDInfo[config->nDD].D3DInfo[config->nD3D].DisplayMode[config->nVMode];

	if (!DXSetVideoMode(App.lpDD, dm->w, dm->h, dm->bpp))
	{
		Log("DXSetVideoMode failed, exitting..");
		return 0;
	}

	memset(&desc, 0, sizeof(DDSURFACEDESC));
	desc.dwSize = sizeof(DDSURFACEDESC);
	desc.dwBackBufferCount = 1;
	desc.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	desc.ddsCaps.dwCaps = DDSCAPS_COMPLEX | DDSCAPS_FLIP | DDSCAPS_PRIMARYSURFACE | DDSCAPS_3DDEVICE;

	if (!DXCreateSurface(App.lpDD, &desc, (LPDIRECTDRAWSURFACE3)&App.lpFrontBuffer))
	{
		Log("DXCreateSurface failed to create front buffer, exitting..");
		return 0;
	}

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
		{
			Log("DXGetAttachedSurface failed to get back buffer, exitting..");
			return 0;
		}
	}
	else
	{
		App.unk = malloc(4 * dm->w * dm->h);
		caps.dwCaps = DDSCAPS_BACKBUFFER;
		DXGetAttachedSurface(App.lpFrontBuffer, &caps, &App.lpBackBuffer);
	}

	if (!DXCreateZBuffer(device, config))
	{
		Log("DXCreateZBuffer failed, exitting..");
		return 0;
	}

	if (!DXCreateDirect3DDevice(App.lpD3D, d3d->Guid, App.lpBackBuffer, &App.lpD3DDevice))
	{
		Log("DXCreateDirect3DDevice failed, exitting..");
		return 0;
	}

	dm = &device->DDInfo[config->nDD].D3DInfo[config->nD3D].DisplayMode[config->nVMode];

	if (!DXCreateViewPort(App.lpD3D, App.lpD3DDevice, dm->w, dm->h, &App.lpViewPort))
	{
		Log("DXCreateViewPort failed, exitting..");
		return 0;
	}

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
	DXClearBuffers(11, 0);
	InitDrawPrimitive(App.lpD3DDevice, App.lpBackBuffer, App.DeviceInfoPtr->DDInfo[App.DXConfigPtr->nDD].D3DInfo[App.DXConfigPtr->nD3D].bHardware);
	HWR_InitState();

	if (device->DDInfo[config->nDD].D3DInfo[config->nD3D].bHardware)
	{
		DXCreateMaxTPages(1);

		if (!nTPages)
		{
			Log("nTPages is 0, DXCreateMaxTPages failed, exitting..");
			return 0;
		}
	}

	Log("WinDXInit finished successfully");
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

HWND WinCreateWindow(HINSTANCE hinstance, long nCmdShow)
{
	HWND hwnd;

	hwnd = CreateWindowEx(WS_EX_APPWINDOW, "Window Class", "Tomb Raider III", WS_POPUP, 0, 0, 0, 0, 0, 0, hinstance, 0);

	if (hwnd)
	{
		ShowWindow(hwnd, nCmdShow);
		UpdateWindow(hwnd);
	}

	return hwnd;
}

float WinFrameRate()
{
	double t, time_now;
	static float fps;
	static long time, counter;
	static char first_time;

	if (!(first_time & 1))
	{
		first_time |= 1;
		time = clock();
	}

	counter++;

	if (counter == 10)
	{
		time_now = clock();
		t = (time_now - time) / (double)CLOCKS_PER_SEC;
		time = (long)time_now;
		fps = float(counter / t);
		counter = 0;
	}

	App.fps = fps;
	return fps;
}

void WinFreeDX(bool free_dd)
{
	DXFreeTPages();

	if (App.Palette)
	{
		App.Palette->Release();
		App.Palette = 0;
	}

	if (App.lpViewPort)
	{
		App.lpViewPort->Release();
		App.lpViewPort = 0;
	}

	if (App.lpD3DDevice)
	{
		App.lpD3DDevice->Release();
		App.lpD3DDevice = 0;
	}

	if (App.lpZBuffer)
	{
		App.lpZBuffer->Release();
		App.lpZBuffer = 0;
	}

	if (App.lpBackBuffer)
	{
		App.lpBackBuffer->Release();
		App.lpBackBuffer = 0;
	}

	if (App.lpFrontBuffer)
	{
		App.lpFrontBuffer->Release();
		App.lpFrontBuffer = 0;
	}

	if (App.lpPictureBuffer)
	{
		App.lpPictureBuffer->Release();
		App.lpPictureBuffer = 0;
	}

	if (App.unk)
		FREE(App.unk);

	if (free_dd)
	{
		if (App.lpD3D)
		{
			App.lpD3D->Release();
			App.lpD3D = 0;
		}

		if (App.lpDD)
		{
			App.lpDD->Release();
			App.lpDD = 0;
		}
	}
}

void S_ExitSystem(const char* msg)
{
	DXSetCooperativeLevel(App.lpDD, App.WindowHandle, DDSCL_NORMAL);
	MessageBox(App.WindowHandle, msg, 0, MB_OK);
	ShutdownGame();
	strcpy(exit_message, msg);
	exit(1);
}

void Log(const char* s, ...)		//NOT present in original code
{
#ifdef DO_LOG
	va_list list;
	char buf[4096];

	if (!logF)
		logF = fopen("tomb3_log.txt", "w+");

	va_start(list, s);
	vsprintf(buf, s, list);
	strcat(buf, "\n");
	va_end(list);
	fwrite(buf, strlen(buf), 1, logF);
#endif
}

void inject_winmain(bool replace)
{
	INJECT(0x004B2F80, WinDXInit, replace);
	INJECT(0x004B2C50, WinAppExit, replace);
	INJECT(0x004B2E10, WinAppProc, replace);
	INJECT(0x004B2D40, WinRegisterWindow, replace);
	INJECT(0x004B2DC0, WinCreateWindow, replace);
	INJECT(0x004B34D0, WinFrameRate, replace);
	INJECT(0x004B2C60, WinFreeDX, replace);
	INJECT(0x004B37C0, S_ExitSystem, replace);
}
