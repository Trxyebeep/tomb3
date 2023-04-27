#include "../tomb3/pch.h"
#include "setupdlg.h"
#include "../specific/winmain.h"
#include "../tomb3/tomb3.h"
#include "../specific/smain.h"
#include "registry.h"
#include "../specific/utils.h"
#include "../../resource.h"

static DXCONFIG* config;
static DEVICEINFO* dev;

static void InitDDDrivers(HWND hwnd)
{
#if (DIRECT3D_VERSION >= 0x900)
	HWND gfx;

	gfx = GetDlgItem(hwnd, IDC_GRAPHICS_ADAPTER);
	EnableWindow(gfx, 0);
#else
	HWND gfx;
	long lp;
	char about[512];

	gfx = GetDlgItem(hwnd, IDC_GRAPHICS_ADAPTER);

	for (lp = 0; lp < dev->nDDInfo; lp++)
	{
		sprintf(about, "%s (%s)", dev->DDInfo[lp].About, dev->DDInfo[lp].Name);
		SendMessage(gfx, CB_ADDSTRING, 0, (LPARAM)about);
	}
#endif
}

#if (DIRECT3D_VERSION >= 0x900)
static void InitD3DDrivers(HWND hwnd)
{
	HWND d3d;
	ulong lp;
	long nHWDriver;
	static long selected = -1;
	char buf[256];
	char about[256];

	d3d = GetDlgItem(hwnd, IDC_D3D);
	nHWDriver = -1;

	if (selected != -1)
		SendMessage(d3d, CB_GETLBTEXT, SendMessage(d3d, CB_GETCURSEL, 0, 0), (LPARAM)buf);

	SendMessage(d3d, CB_RESETCONTENT, 0, 0);

	for (lp = 0; lp < dev->nD3DInfo; lp++)
	{
		sprintf(about, "%s", dev->D3DInfo[lp].About);
		SendMessage(d3d, CB_ADDSTRING, 0, (LPARAM)about);

		if (selected == -1 || !strcmp(buf, about))
			nHWDriver = lp;
	}

	if (nHWDriver == -1)
		nHWDriver = 0;

	selected = nHWDriver;
	SendMessage(d3d, CB_SETCURSEL, selected, 0);
}
#else
static void InitD3DDrivers(HWND hwnd, long nDrivers)
{
	DIRECTDRAWINFO* ddinfo;
	HWND d3d, zbuffer, dither, filter;
	long nHWDriver;
	static long selected = -1;
	char buf[256];
	char about[256];

	d3d = GetDlgItem(hwnd, IDC_D3D);
	zbuffer = GetDlgItem(hwnd, IDC_ZBUFFER);
	dither = GetDlgItem(hwnd, IDC_DITHER);
	filter = GetDlgItem(hwnd, IDC_BILINEAR);
	nHWDriver = -1;

	if (selected != -1)
		SendMessage(d3d, CB_GETLBTEXT, SendMessage(d3d, CB_GETCURSEL, 0, 0), (LPARAM)buf);

	SendMessage(d3d, CB_RESETCONTENT, 0, 0);
	ddinfo = &dev->DDInfo[nDrivers];

	for (int i = 0; i < ddinfo->nD3DInfo; i++)
	{
		sprintf(about, "%s", ddinfo->D3DInfo[i].About);
		SendMessage(d3d, CB_ADDSTRING, 0, (LPARAM)about);

		if (selected == -1 || !strcmp(buf, about))
			nHWDriver = i;
	}

	if (nHWDriver == -1)
		nHWDriver = 0;

	if (selected == -1)
	{
		SendMessage(zbuffer, BM_SETCHECK, 1, 0);
		SendMessage(filter, BM_SETCHECK, 1, 0);
		SendMessage(dither, BM_SETCHECK, 1, 0);
	}

	selected = nHWDriver;
	SendMessage(d3d, CB_SETCURSEL, selected, 0);
	
	EnableWindow(zbuffer, 1);
	EnableWindow(dither, 1);
	EnableWindow(filter, 1);
	SendMessage(zbuffer, BM_SETCHECK, 1, 0);
	SendMessage(filter, BM_SETCHECK, 1, 0);
	SendMessage(dither, BM_SETCHECK, 1, 0);
}
#endif

#if (DIRECT3D_VERSION >= 0x900)
static void InitVideoModes(HWND hwnd, long nD3D)
#else
static void InitVideoModes(HWND hwnd, long nDD, long nD3D)
#endif
{
	DIRECT3DINFO* d3dinfo;
	DISPLAYMODE* dm;
	HWND res;
	static long selected = -1;
	long nVM, nListed;
	char Default[32];
	char vm[40];

	nVM = -1;
	strcpy(Default, "640x480 High Colour (16 Bit)");
	res = GetDlgItem(hwnd, IDC_RESOLUTION);

	if (selected != -1)
		SendMessage(res, CB_GETLBTEXT, SendMessage(res, CB_GETCURSEL, 0, 0), (LPARAM)Default);

	SendMessage(res, CB_RESETCONTENT, 0, 0);
	nListed = 0;

#if (DIRECT3D_VERSION >= 0x900)
	d3dinfo = &dev->D3DInfo[nD3D];
#else
	d3dinfo = &dev->DDInfo[nDD].D3DInfo[nD3D];

	for (int i = 0; i < d3dinfo->nDisplayMode; i++)
	{
		dm = &d3dinfo->DisplayMode[i];

		if (dm->bpp == 16)
		{
			sprintf(vm, "%dx%d High Colour (%d Bit)", dm->w, dm->h, 16);
			SendMessage(res, CB_ADDSTRING, 0, (LPARAM)vm);
			SendMessage(res, CB_SETITEMDATA, nListed, i);

			if (!strcmp(Default, vm))
				nVM = nListed;

			nListed++;
		}
	}
#endif

	for (int i = 0; i < d3dinfo->nDisplayMode; i++)
	{
		dm = &d3dinfo->DisplayMode[i];

		if (dm->bpp == 32)
		{
#if (DIRECT3D_VERSION >= 0x900)
			sprintf(vm, "%d x %d", dm->w, dm->h);
#else
			sprintf(vm, "%dx%d True Colour (%d Bit)", dm->w, dm->h, 32);
#endif
			SendMessage(res, CB_ADDSTRING, 0, (LPARAM)vm);
			SendMessage(res, CB_SETITEMDATA, nListed, i);

			if (!strcmp(Default, vm))
				nVM = nListed;

			nListed++;
		}
	}

	if (nVM == -1)
	{
		selected = 0;
		SendMessage(res, CB_SETCURSEL, 0, 0);
	}
	else
	{
		selected = nVM;
		SendMessage(res, CB_SETCURSEL, nVM, 0);
	}
}

#if (DIRECT3D_VERSION < 0x900)
static void InitTextures(HWND hwnd, long nDD, long nD3D)
{
	DIRECT3DINFO* d3dinfo;
	D3DTEXTUREINFO* tex;
	HWND format;
	static long selected = -1;
	long nTF;
	char old[20];
	char buf[20];
	char bit[12];
	bool found;

	nTF = -1;
	format = GetDlgItem(hwnd, IDC_D3DTF);

	if (selected != -1)
		SendMessage(format, CB_GETLBTEXT, SendMessage(format, CB_GETCURSEL, 0, 0), (LPARAM)old);

	SendMessage(format, CB_RESETCONTENT, 0, 0);

	d3dinfo = &dev->DDInfo[nDD].D3DInfo[nD3D];

	for (int i = 0; i < d3dinfo->nTexture; i++)
	{
		tex = &d3dinfo->Texture[i];

		if (tex->bPalette)
			sprintf(buf, "%d Bit", tex->bpp);
		else if (tex->abpp)
			sprintf(buf, "%d%d%d%d", tex->rbpp, tex->gbpp, tex->bbpp, tex->abpp);
		else
			sprintf(buf, "%d%d%d", tex->rbpp, tex->gbpp, tex->bbpp);

		SendMessage(format, CB_ADDSTRING, 0, (LPARAM)buf);

		if (selected == -1)
		{
			if (!strcmp(bit, buf))
				nTF = i;
		}
		else if (!strcmp(old, buf))
			nTF = i;
	}

	found = 0;

	if (!found)
	{
		strcpy(bit, "5551");

		for (int i = 0; i < d3dinfo->nTexture; i++)
		{
			SendMessage(format, CB_GETLBTEXT, i, (LPARAM)buf);

			if (!strcmp(bit, buf))
			{
				nTF = i;
				found = 1;
				break;
			}
		}
	}

	if (!found)
	{
		strcpy(bit, "4444");

		for (int i = 0; i < d3dinfo->nTexture; i++)
		{
			SendMessage(format, CB_GETLBTEXT, i, (LPARAM)buf);

			if (!strcmp(bit, buf))
			{
				nTF = i;
				found = 1;
				break;
			}
		}
	}

	if (!found)
	{
		strcpy(bit, "555");

		for (int i = 0; i < d3dinfo->nTexture; i++)
		{
			SendMessage(format, CB_GETLBTEXT, i, (LPARAM)buf);

			if (!strcmp(bit, buf))
			{
				nTF = i;
				found = 1;
				break;
			}
		}
	}

	if (!found)
	{
		strcpy(bit, "565");

		for (int i = 0; i < d3dinfo->nTexture; i++)
		{
			SendMessage(format, CB_GETLBTEXT, i, (LPARAM)buf);

			if (!strcmp(bit, buf))
			{
				nTF = i;
				found = 1;
				break;
			}
		}
	}

	if (nTF == -1)
	{
		selected = 0;
		SendMessage(format, CB_SETCURSEL, 0, 0);
	}
	else
	{
		selected = nTF;
		SendMessage(format, CB_SETCURSEL, nTF, 0);
	}
}
#endif

static void InitDSAdapters(HWND hwnd)
{
	HWND sound, disable_sound;

	sound = GetDlgItem(hwnd, IDC_SOUND);
	disable_sound = GetDlgItem(hwnd, IDC_DISABLE_SOUND);

	if (dev->DSInfo)
	{
		SendMessage(sound, CB_RESETCONTENT, 0, 0);

		for (int i = 0; i < dev->nDSInfo; i++)
			SendMessage(sound, CB_ADDSTRING, 0, (LPARAM)dev->DSInfo[i].About);

		SendMessage(sound, CB_SETCURSEL, 0, 0);
	}
	else
	{
		EnableWindow(disable_sound, 0);
		SendMessage(disable_sound, BM_SETCHECK, 1, 0);
		SendMessage(sound, CB_ADDSTRING, 0, (LPARAM)"No Sound Adapter Available");
		SendMessage(sound, CB_SETCURSEL, 0, 0);
		EnableWindow(sound, 0);
	}
}

static void InitDialogBox(HWND hwnd)
{
	long n;

	UT_CenterWindow(hwnd);
	InitDDDrivers(hwnd);	//for DX9 this just disables the dropdown

#if (DIRECT3D_VERSION >= 0x900)
	InitD3DDrivers(hwnd);
	n = dev->nD3DInfo - 1;
	SendMessage(GetDlgItem(hwnd, IDC_D3D), CB_SETCURSEL, n, 0);
	InitVideoModes(hwnd, n);
#else
	n = dev->nDDInfo - 1;
	SendMessage(GetDlgItem(hwnd, IDC_GRAPHICS_ADAPTER), CB_SETCURSEL, n, 0);
	InitD3DDrivers(hwnd, n);
	InitVideoModes(hwnd, n, SendMessage(GetDlgItem(hwnd, IDC_D3D), CB_GETCURSEL, 0, 0));
	InitTextures(hwnd, n, SendMessage(GetDlgItem(hwnd, IDC_D3D), CB_GETCURSEL, 0, 0));
#endif

	InitDSAdapters(hwnd);
}

static BOOL CALLBACK SetupDlgProc(HWND dlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	ulong ul;
	bool pass, bl;

	if (message == WM_INITDIALOG)
	{
		InitDialogBox(dlg);
		
		if (!OpenRegistry(SUB_KEY))
		{
			SendMessage(GetDlgItem(dlg, IDC_FULLSCREEN), BM_SETCHECK, 0, 0);
			SendMessage(GetDlgItem(dlg, IDC_WINDOWED), BM_SETCHECK, 1, 0);
			SendMessage(GetDlgItem(dlg, IDC_ZBUFFER), BM_SETCHECK, 1, 0);
			SendMessage(GetDlgItem(dlg, IDC_DITHER), BM_SETCHECK, 1, 0);
			SendMessage(GetDlgItem(dlg, IDC_BILINEAR), BM_SETCHECK, 1, 0);
		}
		else
		{
			REG_ReadBool((char*)"Window", bl, 1);
			SendMessage(GetDlgItem(dlg, IDC_FULLSCREEN), BM_SETCHECK, !bl, 0);
			SendMessage(GetDlgItem(dlg, IDC_WINDOWED), BM_SETCHECK, bl, 0);

			REG_ReadLong((char*)"zbuffer", ul, 1);
			SendMessage(GetDlgItem(dlg, IDC_ZBUFFER), BM_SETCHECK, ul, 0);

			REG_ReadLong((char*)"dither", ul, 1);
			SendMessage(GetDlgItem(dlg, IDC_DITHER), BM_SETCHECK, ul, 0);

			REG_ReadLong((char*)"filter", ul, 1);
			SendMessage(GetDlgItem(dlg, IDC_BILINEAR), BM_SETCHECK, ul, 0);

			REG_ReadLong((char*)"VM", ul, 0);
			SendMessage(GetDlgItem(dlg, IDC_RESOLUTION), CB_SETCURSEL, ul, 0);
		}
		
		return 1;
	}

	if (message == WM_COMMAND)
	{
		switch ((ushort)wParam)
		{
		case IDOK:
		case IDC_TEST:
#if (DIRECT3D_VERSION < 0x900)
			config->nDD = SendMessage(GetDlgItem(dlg, IDC_GRAPHICS_ADAPTER), CB_GETCURSEL, 0, 0);
#endif
			config->nD3D = SendMessage(GetDlgItem(dlg, IDC_D3D), CB_GETCURSEL, 0, 0);
			config->nVMode = SendMessage(GetDlgItem(dlg, IDC_RESOLUTION), CB_GETITEMDATA,
				SendMessage(GetDlgItem(dlg, IDC_RESOLUTION), CB_GETCURSEL, 0, 0), 0);
#if (DIRECT3D_VERSION < 0x900)
			config->D3DTF = SendMessage(GetDlgItem(dlg, IDC_D3DTF), CB_GETCURSEL, 0, 0);
#endif
			config->bZBuffer = (bool)SendMessage(GetDlgItem(dlg, IDC_ZBUFFER), BM_GETCHECK, 0, 0);
			config->nDS = SendMessage(GetDlgItem(dlg, IDC_SOUND), CB_GETCURSEL, 0, 0);
			config->Dither = (bool)SendMessage(GetDlgItem(dlg, IDC_DITHER), BM_GETCHECK, 0, 0);
			config->Filter = (bool)SendMessage(GetDlgItem(dlg, IDC_BILINEAR), BM_GETCHECK, 0, 0);
			config->sound = !(bool)SendMessage(GetDlgItem(dlg, IDC_DISABLE_SOUND), BM_GETCHECK, 0, 0);
			App.Windowed = (bool)SendMessage(GetDlgItem(dlg, IDC_WINDOWED), BM_GETCHECK, 0, 0);

			if ((ushort)wParam == IDOK)
			{
				S_SaveSettings();
				ShowWindow(App.WindowHandle, 1);
				ShowCursor(0);
				EndDialog(dlg, 1);
			}
			else
			{
				App.lpDXConfig = &App.DXConfig;
				App.lpDeviceInfo = &App.DeviceInfo;
				pass = WinDXInit(&App.DeviceInfo, &App.DXConfig, 1);
				WinFreeDX(1);
				ShowWindow(App.WindowHandle, 0);
				UpdateWindow(dlg);
				ShowCursor(1);

				if (pass)
					MessageBox(App.WindowHandle, "Tomb Raider 3 Configuration Test Has Passed", "Setup Test", MB_ICONASTERISK);
				else
					MessageBox(App.WindowHandle, "Tomb Raider 3 Configuration Test Has Failed", "Setup Test", MB_ICONEXCLAMATION);
			}

			return 1;

		case IDCANCEL:
			EndDialog(dlg, 0);
			return 1;

		case IDC_GRAPHICS_ADAPTER:
		case IDC_D3D:

			if (HIWORD(wParam) == CBN_SELCHANGE)
			{
#if (DIRECT3D_VERSION >= 0x900)
				InitD3DDrivers(dlg);
				InitVideoModes(dlg, SendMessage(GetDlgItem(dlg, IDC_D3D), CB_GETCURSEL, 0, 0));
#else
				InitD3DDrivers(dlg, SendMessage(GetDlgItem(dlg, IDC_GRAPHICS_ADAPTER), CB_GETCURSEL, 0, 0));
				InitVideoModes(dlg, SendMessage(GetDlgItem(dlg, IDC_GRAPHICS_ADAPTER), CB_GETCURSEL, 0, 0),
					SendMessage(GetDlgItem(dlg, IDC_D3D), CB_GETCURSEL, 0, 0));
				InitTextures(dlg, SendMessage(GetDlgItem(dlg, IDC_GRAPHICS_ADAPTER), CB_GETCURSEL, 0, 0),
					SendMessage(GetDlgItem(dlg, IDC_D3D), CB_GETCURSEL, 0, 0));
#endif
			}

			break;

		case IDC_WINDOWED:

			if (!HIWORD(wParam))
				SendMessage(GetDlgItem(dlg, IDC_FULLSCREEN), BM_SETCHECK, 0, 0);

			break;

		case IDC_FULLSCREEN:

			if (!HIWORD(wParam))
				SendMessage(GetDlgItem(dlg, IDC_WINDOWED), BM_SETCHECK, 0, 0);

			break;

		case IDC_DISABLE_SOUND:

			if (!HIWORD(wParam))
			{
				if (SendMessage(GetDlgItem(dlg, IDC_DISABLE_SOUND), BM_GETCHECK, 0, 0))
					EnableWindow(GetDlgItem(dlg, IDC_SOUND), 0);
				else
					EnableWindow(GetDlgItem(dlg, IDC_SOUND), 1);
			}

			break;
		}
	}

	return 0;
}

bool SetupDialog(DEVICEINFO* device, DXCONFIG* cfg, HINSTANCE hinstance)
{
	INT_PTR ret;

	dev = device;
	config = cfg;
	ShowCursor(1);
	ret = DialogBoxParam(hinstance, MAKEINTRESOURCE(IDD_SETUPDIALOG), 0, SetupDlgProc, 0);
	ShowCursor(0);

	if (ret == -1)
	{
		MessageBox(0, "Unable To Initialise Dialog", "", MB_OK);
		return 0;
	}

	return ret != 0;
}
