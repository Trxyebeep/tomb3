#include "../tomb3/pch.h"
#include "winmain.h"
#include "dxshell.h"
#include "drawprimitive.h"
#include "hwrender.h"
#include "texture.h"
#include "init.h"
#include "specific.h"
#include "smain.h"
#include "utils.h"
#include "dxdialog.h"
#include "ds.h"
#include "time.h"
#include "di.h"
#include "audio.h"
#include "display.h"
#include "picture.h"
#ifdef TROYESTUFF
#include "fmv.h"
#include "../game/invfunc.h"
#include "../tomb3/tomb3.h"
#endif

#ifdef DO_LOG
FILE* logF = 0;
#endif

WINAPP App;
HWCONFIG HWConfig;
char* G_lpCmdLine;

bool WinDXInit(DEVICEINFO* device, DXCONFIG* config, bool createNew)
{
	DISPLAYMODE* dm;
	DIRECT3DINFO* d3d;
	DDSURFACEDESCX desc;
	D3DMATERIALX m;
	DDSCAPSX caps;
	D3DMATERIALHANDLE handle;

#ifdef TROYESTUFF
	return DXStartRenderer(device, config, createNew, tomb3.Windowed);
#endif

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

	memset(&desc, 0, sizeof(DDSURFACEDESCX));
	desc.dwSize = sizeof(DDSURFACEDESCX);
	desc.dwBackBufferCount = 1;
	desc.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	desc.ddsCaps.dwCaps = DDSCAPS_COMPLEX | DDSCAPS_FLIP | DDSCAPS_PRIMARYSURFACE | DDSCAPS_3DDEVICE;

	if (!DXCreateSurface(App.lpDD, &desc, (LPDIRECTDRAWSURFACEX)&App.lpFrontBuffer))
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
		App.unk = (ulong*)malloc(4 * dm->w * dm->h);
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

	memset(&m, 0, sizeof(D3DMATERIALX));
	m.dwSize = sizeof(D3DMATERIALX);

	App.lpD3D->CreateMaterial(&App.lpViewPortMaterial, 0);
	App.lpViewPortMaterial->SetMaterial(&m);
	App.lpViewPortMaterial->GetHandle(App.lpD3DDevice, &handle);
	App.lpViewPort->SetBackground(handle);

	memset(&desc, 0, sizeof(DDSURFACEDESCX));
	desc.dwSize = sizeof(DDSURFACEDESCX);
	desc.dwWidth = 640;
	desc.dwHeight = 480;
	desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
	desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
	DXCreateSurface(App.lpDD, &desc, (LPDIRECTDRAWSURFACEX)&App.lpPictureBuffer);
	DXClearBuffers(11, 0);
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
#ifndef TROYESTUFF
		ShowCursor(0);
#endif
		break;

	case WM_ACTIVATE:

		switch ((short)wParam)
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

#ifdef TROYESTUFF
	case WM_MOVE:
		DXMove((short)lParam, short((lParam >> 16) & 0xFFFF));
		break;
#endif
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

long WinRegisterWindow(HINSTANCE hinstance)
{
	App.WindowClass.hIcon = LoadIcon(hinstance, (LPCSTR)115);		//todo: icon resource define
	App.WindowClass.lpszMenuName = 0;
	App.WindowClass.lpszClassName = "Window Class";
	App.WindowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	App.WindowClass.hInstance = hinstance;
	App.WindowClass.style = CS_VREDRAW | CS_HREDRAW;
	App.WindowClass.lpfnWndProc = WinAppProc;
	App.WindowClass.cbClsExtra = 0;
	App.WindowClass.cbWndExtra = 0;
#ifdef TROYESTUFF
	App.WindowClass.hCursor = LoadCursor(0, IDC_ARROW);
#endif
	return RegisterClass(&App.WindowClass);
}

#ifdef TROYESTUFF
static bool WinCreateWindow()
{
	App.WindowHandle = CreateWindowEx(WS_EX_APPWINDOW, "Window Class", "Tomb Raider III", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		0, 0, App.hInstance, 0);

	if (!App.WindowHandle)
		return 0;

	return 1;
}
#else
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
#endif

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
		free(App.unk);

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

#ifdef TROYESTUFF
void WinSetStyle(bool fullscreen, ulong& set)
{
	ulong style;

	style = GetWindowLong(App.WindowHandle, GWL_STYLE);

	if (fullscreen)
		style = (style & ~WS_OVERLAPPEDWINDOW) | WS_POPUP;
	else
		style = (style & ~WS_POPUP) | WS_OVERLAPPEDWINDOW;

//	style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX | WS_SYSMENU);	//removing WS_THICKFRAME creates a weird thick white border around the window when the app is moved?
	style &= ~(WS_MAXIMIZEBOX | WS_SYSMENU);
	SetWindowLong(App.WindowHandle, GWL_STYLE, style);

	if (set)
		set = style;
}
#endif

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd)
{
	DIRECT3DINFO* d3dinfo;
#ifdef TROYESTUFF
	HWND desktop;
	HDC hdc;
	DEVMODE devmode;
	static ulong bpp;
#endif
	bool hw;

	G_lpCmdLine = lpCmdLine;
	memset(&App, 0, sizeof(WINAPP));
	App.hInstance = hInstance;

#ifdef TROYESTUFF
	tomb3.WindowStyle = WS_OVERLAPPEDWINDOW;
#endif

	if (!hPrevInstance && !WinRegisterWindow(hInstance))
	{
		MessageBox(0, "Unable To Register Window Class", "", MB_OK);
		return 0;
	}

#ifdef TROYESTUFF
	if (!WinCreateWindow())
#else
	App.WindowHandle = WinCreateWindow(hInstance, nShowCmd);

	if (!App.WindowHandle)
#endif
	{
		MessageBox(0, "Unable To Create Window", "", MB_OK);
		return 0;
	}

#ifdef TROYESTUFF
	ShowWindow(App.WindowHandle, SW_HIDE);
	UpdateWindow(App.WindowHandle);
#endif

#ifndef TROYESTUFF	//nocd
	if (!CD_Init())
		return 0;
#endif

	DXGetDeviceInfo(&App.DeviceInfo, App.WindowHandle, App.hInstance);
	App.DXConfigPtr = &App.DXConfig;
	App.DeviceInfoPtr = &App.DeviceInfo;

#ifdef TROYESTUFF
	tomb3.gold = UT_FindArg("gold");

	if (tomb3.gold)
		memcpy(LevelSecrets, gLevelSecrets, sizeof(LevelSecrets));
#endif

#ifdef TROYESTUFF
	if ((!S_LoadSettings() || UT_FindArg("setup")))
	{
		DXUserDialog(&App.DeviceInfo, &App.DXConfig, App.hInstance);
		DXFreeDeviceInfo(&App.DeviceInfo);

		desktop = GetDesktopWindow();
		hdc = GetDC(desktop);
		bpp = GetDeviceCaps(hdc, BITSPIXEL);
		ReleaseDC(desktop, hdc);

		desktop = GetDesktopWindow();
		hdc = GetDC(desktop);
		devmode.dmSize = sizeof(DEVMODE);
		devmode.dmBitsPerPel = bpp;
		ReleaseDC(desktop, hdc);
		devmode.dmFields = DM_BITSPERPEL;
		ChangeDisplaySettings(&devmode, 0);
		return 0;
	}
#else
	if ((!S_LoadSettings() || UT_FindArg("setup")) && !DXUserDialog(&App.DeviceInfo, &App.DXConfig, App.hInstance))
	{
		DXFreeDeviceInfo(&App.DeviceInfo);
		return 0;
	}
#endif

#ifdef TROYESTUFF
	tomb3.WinPlayLoaded = LoadWinPlay();

	if (!tomb3.WinPlayLoaded)
		MessageBox(0, "Failed to load WinPlay", "tomb3", 0);

	SetWindowPos(App.WindowHandle, 0, tomb3.rScreen.left, tomb3.rScreen.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	desktop = GetDesktopWindow();
	hdc = GetDC(desktop);
	bpp = GetDeviceCaps(hdc, BITSPIXEL);
	ReleaseDC(desktop, hdc);
#endif

	if (!WinDXInit(&App.DeviceInfo, &App.DXConfig, 1))
	{
		WinFreeDX(1);
		DXFreeDeviceInfo(&App.DeviceInfo);
		ShowWindow(App.WindowHandle, 0);
		MessageBox(App.WindowHandle, "Tomb Raider 3 Failed To Initialise, Please Run Setup", "Error", MB_ICONEXCLAMATION);
		return 0;
	}

#ifdef TROYESTUFF
	if (tomb3.Windowed)
	{
		WinSetStyle(0, tomb3.WindowStyle);
		SetCursor(LoadCursor(0, IDC_ARROW));
		ShowCursor(1);
	}
	else
	{
		WinSetStyle(1, tomb3.WindowStyle);
		SetCursor(0);
		ShowCursor(0);
	}

	UpdateWindow(App.WindowHandle);
	ShowWindow(App.WindowHandle, nShowCmd);
#endif

	d3dinfo = &App.DeviceInfoPtr->DDInfo[App.DXConfigPtr->nDD].D3DInfo[App.DXConfigPtr->nD3D];

	if (!(d3dinfo->DeviceDesc.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_ALPHA))
		d3dinfo->Texture[App.DXConfigPtr->D3DTF].bAlpha = 0;

	HWConfig.Perspective = 1;
	HWConfig.Dither = App.DXConfig.Dither;
	HWConfig.nFilter = D3DFILTER_NEAREST + (App.DXConfig.Filter != 0);
	HWConfig.nShadeMode = D3DSHADE_GOURAUD;
	HWConfig.nFillMode = D3DFILL_SOLID;
	HWConfig.TrueAlpha = 0;

	hw = App.DeviceInfoPtr->DDInfo[App.DXConfigPtr->nDD].D3DInfo[App.DXConfigPtr->nD3D].bHardware;
	framedump = 0;
	App.nUVAdd = hw ? 256 : 128;
	UT_InitAccurateTimer();
	DXResetPalette(PictureTextures);
	InitDrawPrimitive(App.lpD3DDevice, App.lpBackBuffer, hw);
	farz = 0x5000;
	distanceFogValue = 0x3000;
	DS_Init();
	TIME_Init();
	HWR_Init();
	DS_Start(0);
	DI_Start();
	ACMInit();
	setup_screen_size();
	game_closedown = 0;
	GtWindowClosed = 0;
	GtFullScreenClearNeeded = 0;
	GameMain();

#ifdef TROYESTUFF
	desktop = GetDesktopWindow();
	hdc = GetDC(desktop);
	devmode.dmSize = sizeof(DEVMODE);
	devmode.dmBitsPerPel = bpp;
	ReleaseDC(desktop, hdc);
	devmode.dmFields = DM_BITSPERPEL;
	ChangeDisplaySettings(&devmode, 0);
#endif

	return 0;
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
	INJECT(0x004B2940, WinMain, replace);
	INJECT(0x004B37C0, S_ExitSystem, replace);
}
