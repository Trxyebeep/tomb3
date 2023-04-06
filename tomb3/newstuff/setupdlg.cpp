#include "../tomb3/pch.h"
#include "setupdlg.h"
#include "../specific/winmain.h"
#include "../tomb3/tomb3.h"
#include "../specific/smain.h"
#include "../../resource.h"
#include "registry.h"

static DXCONFIG* config;
static DEVICEINFO* dev;

static void InitVideoModes(HWND hwnd, long nDD, long nD3D)
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

	for (int i = 0; i < d3dinfo->nDisplayMode; i++)
	{
		dm = &d3dinfo->DisplayMode[i];

		if (dm->bpp == 32)
		{
			sprintf(vm, "%dx%d True Colour (%d Bit)", dm->w, dm->h, 32);
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
	long nDD;

	nDD = dev->nDDInfo - 1;
	config->MMX = 1;

	InitVideoModes(hwnd, nDD, 0);
	InitTextures(hwnd, nDD, 0);
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

			if (REG_ReadLong((char*)"VM", ul, 0))
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
			config->nDD = dev->nDDInfo - 1;
			config->nD3D = 0;
			config->nVMode = SendMessage(GetDlgItem(dlg, IDC_RESOLUTION), CB_GETITEMDATA, SendMessage(GetDlgItem(dlg, IDC_RESOLUTION), CB_GETCURSEL, 0, 0), 0);
			config->D3DTF = SendMessage(GetDlgItem(dlg, IDC_D3DTF), CB_GETCURSEL, 0, 0);
			config->bZBuffer = (bool)SendMessage(GetDlgItem(dlg, IDC_ZBUFFER), BM_GETCHECK, 0, 0);
			config->DS = SendMessage(GetDlgItem(dlg, IDC_SOUND), CB_GETCURSEL, 0, 0);
			config->DI = -1;
			config->AGP = 1;
			config->Dither = (bool)SendMessage(GetDlgItem(dlg, IDC_DITHER), BM_GETCHECK, 0, 0);
			config->Filter = (bool)SendMessage(GetDlgItem(dlg, IDC_BILINEAR), BM_GETCHECK, 0, 0);
			config->sound = !(bool)SendMessage(GetDlgItem(dlg, IDC_DISABLE_SOUND), BM_GETCHECK, 0, 0);
			config->Joystick = 0;
			tomb3.Windowed = (bool)SendMessage(GetDlgItem(dlg, IDC_WINDOWED), BM_GETCHECK, 0, 0);

			if ((ushort)wParam == IDOK)
			{
				S_SaveSettings();
				ShowWindow(App.WindowHandle, 1);
				ShowCursor(0);
				EndDialog(dlg, 1);
			}
			else
			{
				App.DXConfigPtr = &App.DXConfig;
				App.DeviceInfoPtr = &App.DeviceInfo;
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
