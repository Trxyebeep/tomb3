#include "../tomb3/pch.h"
#include "dxdialog.h"
#include "winmain.h"
#ifdef TROYESTUFF
#include "smain.h"
#endif

//these are the BOXES/BUTTONS not the text
#define IDD_SETUPDIALOG			101
#define IDC_GRAPHICS_ADAPTER	1000
#define IDC_OUTPUT_SETTINGS		1001
#define IDC_RESOLUTION			1002
#define IDC_D3DTF				1003	//hidden
#define IDC_ZBUFFER				1004
#define IDC_DITHER				1005
#define IDC_BILINEAR			1006
#define IDC_HARDWARE			1007
#define IDC_SOFTWARE			1008
#define IDC_8BIT_TEXTURES		1009
#define IDC_SOUND				1010
#define IDC_JOYSTICK			1011
#define IDC_AGPMEM				1012
#define IDC_DISABLE_JOYSTICK	1014
#define IDC_DISABLE_SOUND		1015
#define IDC_TEST				1019

#define G_DXConfig	VAR_(0x006CE508, DXCONFIG*)
#define G_DeviceInfo	VAR_(0x006CE50C, DEVICEINFO*)
#define bSoftwareDefault	VAR_(0x006CE514, bool)

BOOL CALLBACK DXSetupDlgProc(HWND dlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND output_setting, resolution;
	bool pass;

	if (message == WM_INITDIALOG)
	{
		DXInitDialogBox(dlg);
		output_setting = GetDlgItem(dlg, IDC_OUTPUT_SETTINGS);
		resolution = GetDlgItem(dlg, IDC_RESOLUTION);
		return 1;
	}

	if (message == WM_COMMAND)
	{
		switch ((ushort)wParam)
		{
		case IDOK:
		case IDC_TEST:
			G_DXConfig->nDD = SendMessage(GetDlgItem(dlg, IDC_GRAPHICS_ADAPTER), CB_GETCURSEL, 0, 0);
			G_DXConfig->nD3D = SendMessage(GetDlgItem(dlg, IDC_OUTPUT_SETTINGS), CB_GETCURSEL, 0, 0);
			G_DXConfig->nVMode = SendMessage(GetDlgItem(dlg, IDC_RESOLUTION), CB_GETITEMDATA,
				SendMessage(GetDlgItem(dlg, IDC_RESOLUTION), CB_GETCURSEL, 0, 0), 0);
			G_DXConfig->D3DTF = SendMessage(GetDlgItem(dlg, IDC_D3DTF), CB_GETCURSEL, 0, 0);
			G_DXConfig->bZBuffer = (bool)SendMessage(GetDlgItem(dlg, IDC_ZBUFFER), BM_GETCHECK, 0, 0);
			G_DXConfig->DS = SendMessage(GetDlgItem(dlg, IDC_SOUND), CB_GETCURSEL, 0, 0);
			G_DXConfig->DI = SendMessage(GetDlgItem(dlg, IDC_JOYSTICK), CB_GETCURSEL, 0, 0);
			G_DXConfig->AGP = (bool)SendMessage(GetDlgItem(dlg, IDC_AGPMEM), BM_GETCHECK, 0, 0);
			G_DXConfig->Dither = (bool)SendMessage(GetDlgItem(dlg, IDC_DITHER), BM_GETCHECK, 0, 0);
			G_DXConfig->Filter = (bool)SendMessage(GetDlgItem(dlg, IDC_BILINEAR), BM_GETCHECK, 0, 0);
			G_DXConfig->sound = !(bool)SendMessage(GetDlgItem(dlg, IDC_DISABLE_SOUND), BM_GETCHECK, 0, 0);
			G_DXConfig->Joystick = !(bool)SendMessage(GetDlgItem(dlg, IDC_DISABLE_JOYSTICK), BM_GETCHECK, 0, 0);

			if ((ushort)wParam == IDOK)
			{
#ifdef TROYESTUFF
				S_SaveSettings();
#endif
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
		case IDC_OUTPUT_SETTINGS:

			if (HIWORD(wParam) == CBN_SELCHANGE)
			{
				DXInitD3DDrivers(dlg, SendMessage(GetDlgItem(dlg, IDC_GRAPHICS_ADAPTER), CB_GETCURSEL, 0, 0));
				DXInitVideoModes(dlg, SendMessage(GetDlgItem(dlg, IDC_GRAPHICS_ADAPTER), CB_GETCURSEL, 0, 0),
					SendMessage(GetDlgItem(dlg, IDC_OUTPUT_SETTINGS), CB_GETCURSEL, 0, 0));
				DXInitTextures(dlg, SendMessage(GetDlgItem(dlg, IDC_GRAPHICS_ADAPTER), CB_GETCURSEL, 0, 0),
					SendMessage(GetDlgItem(dlg, IDC_OUTPUT_SETTINGS), CB_GETCURSEL, 0, 0));
			}

			break;

		case IDC_HARDWARE:

			if (!HIWORD(wParam))
			{
				SendMessage(output_setting, CB_SETCURSEL, 1, 0);
				DXInitD3DDrivers(dlg, SendMessage(GetDlgItem(dlg, IDC_GRAPHICS_ADAPTER), CB_GETCURSEL, 0, 0));
				DXInitVideoModes(dlg, SendMessage(GetDlgItem(dlg, IDC_GRAPHICS_ADAPTER), CB_GETCURSEL, 0, 0),
					SendMessage(GetDlgItem(dlg, IDC_OUTPUT_SETTINGS), CB_GETCURSEL, 0, 0));
				DXInitTextures(dlg, SendMessage(GetDlgItem(dlg, IDC_GRAPHICS_ADAPTER), CB_GETCURSEL, 0, 0),
					SendMessage(GetDlgItem(dlg, IDC_OUTPUT_SETTINGS), CB_GETCURSEL, 0, 0));
			}

			break;

		case IDC_SOFTWARE:

			if (!HIWORD(wParam))
			{
				SendMessage(output_setting, CB_SETCURSEL, 0, 0);
				SendMessage(resolution, CB_SETCURSEL, 0, 0);
				DXInitD3DDrivers(dlg, SendMessage(GetDlgItem(dlg, IDC_GRAPHICS_ADAPTER), CB_GETCURSEL, 0, 0));
				DXInitVideoModes(dlg, SendMessage(GetDlgItem(dlg, IDC_GRAPHICS_ADAPTER), CB_GETCURSEL, 0, 0),
					SendMessage(GetDlgItem(dlg, IDC_OUTPUT_SETTINGS), CB_GETCURSEL, 0, 0));
				DXInitTextures(dlg, SendMessage(GetDlgItem(dlg, IDC_GRAPHICS_ADAPTER), CB_GETCURSEL, 0, 0),
					SendMessage(GetDlgItem(dlg, IDC_OUTPUT_SETTINGS), CB_GETCURSEL, 0, 0));
				EnableWindow(GetDlgItem(dlg, IDC_8BIT_TEXTURES), 0);
			}

			break;

		case IDC_8BIT_TEXTURES:

			if (!HIWORD(wParam))
				DXInitTextures(dlg, SendMessage(GetDlgItem(dlg, IDC_GRAPHICS_ADAPTER), CB_GETCURSEL, 0, 0),
					SendMessage(GetDlgItem(dlg, IDC_OUTPUT_SETTINGS), CB_GETCURSEL, 0, 0));

			break;

		case IDC_DISABLE_JOYSTICK:

			if (!HIWORD(wParam))
			{
				if (SendMessage(GetDlgItem(dlg, IDC_DISABLE_JOYSTICK), BM_GETCHECK, 0, 0))
					EnableWindow(GetDlgItem(dlg, IDC_JOYSTICK), 0);
				else
					EnableWindow(GetDlgItem(dlg, IDC_JOYSTICK), 1);
			}

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

bool DXUserDialog(DEVICEINFO* device, DXCONFIG* config, HINSTANCE hinstance)
{
	INT_PTR ret;

	G_DeviceInfo = device;
	G_DXConfig = config;
	ShowCursor(1);
	ret = DialogBoxParam(hinstance, MAKEINTRESOURCE(IDD_SETUPDIALOG), 0, DXSetupDlgProc, 0);
	ShowCursor(0);

	if (ret == -1)
	{
		MessageBox(0, "Unable To Initialise Dialog", "", MB_OK);
		return 0;
	}

	return ret != 0;
}

void DXInitD3DDrivers(HWND hwnd, long nDrivers)
{
	DIRECTDRAWINFO* ddinfo;
	DIRECT3DINFO* d3dinfo;
	HWND output_setting, zbuffer, dither, filter, tex_8bit, agp_mem, HWR, SWR;
	long nHWDriver;
	static long selected = -1;
	char buf[80];
	char abt[80];

	output_setting = GetDlgItem(hwnd, IDC_OUTPUT_SETTINGS);
	zbuffer = GetDlgItem(hwnd, IDC_ZBUFFER);
	dither = GetDlgItem(hwnd, IDC_DITHER);
	filter = GetDlgItem(hwnd, IDC_BILINEAR);
	tex_8bit = GetDlgItem(hwnd, IDC_8BIT_TEXTURES);
	agp_mem = GetDlgItem(hwnd, IDC_AGPMEM);
	HWR = GetDlgItem(hwnd, IDC_HARDWARE);
	SWR = GetDlgItem(hwnd, IDC_SOFTWARE);
	nHWDriver = -1;

	if (selected != -1)
		SendMessage(output_setting, CB_GETLBTEXT, SendMessage(output_setting, CB_GETCURSEL, 0, 0), (LPARAM)buf);

	SendMessage(output_setting, CB_RESETCONTENT, 0, 0);
	ddinfo = &G_DeviceInfo->DDInfo[nDrivers];

	for (int i = 0; i < ddinfo->nD3DInfo; i++)
	{
		sprintf(abt, "%s", ddinfo->D3DInfo[i].About);
		SendMessage(output_setting, CB_ADDSTRING, 0, (LPARAM)abt);

		if (selected == -1 && !bSoftwareDefault && ddinfo->D3DInfo[i].bHardware)
			nHWDriver = i;

		if (!strcmp(buf, abt))
			nHWDriver = i;
	}

	if (nHWDriver == -1)
		nHWDriver = 0;

	EnableWindow(agp_mem, 0);

	if (selected == -1)
	{
		SendMessage(zbuffer, BM_SETCHECK, 1, 0);
		SendMessage(filter, BM_SETCHECK, 1, 0);
		SendMessage(dither, BM_SETCHECK, 1, 0);

		if (ddinfo->D3DInfo[nHWDriver].bAGP)
			SendMessage(agp_mem, BM_SETCHECK, 1, 0);
	}

	SendMessage(output_setting, CB_SETCURSEL, nHWDriver, 0);
	selected = nHWDriver;

	d3dinfo = &G_DeviceInfo->DDInfo[nDrivers].D3DInfo[nHWDriver];

	if (d3dinfo->bHardware)
	{
		EnableWindow(zbuffer, 1);
		EnableWindow(dither, 1);
		EnableWindow(filter, 1);
		EnableWindow(tex_8bit, 1);
		EnableWindow(agp_mem, 0);
		SendMessage(agp_mem, BM_SETCHECK, 0, 0);

		if (d3dinfo->bAGP)
		{
			EnableWindow(agp_mem, 1);
			SendMessage(agp_mem, BM_SETCHECK, 1, 0);
		}

		SendMessage(HWR, BM_SETCHECK, 1, 0);
		SendMessage(SWR, BM_SETCHECK, 0, 0);
		SendMessage(zbuffer, BM_SETCHECK, 1, 0);
		SendMessage(filter, BM_SETCHECK, 1, 0);
		SendMessage(dither, BM_SETCHECK, 1, 0);
		SendMessage(tex_8bit, BM_SETCHECK, 0, 0);
	}
	else
	{
		SendMessage(zbuffer, BM_SETCHECK, 0, 0);
		SendMessage(filter, BM_SETCHECK, 0, 0);
		SendMessage(dither, BM_SETCHECK, 0, 0);
		SendMessage(SWR, BM_SETCHECK, 1, 0);
		SendMessage(HWR, BM_SETCHECK, 0, 0);
		EnableWindow(zbuffer, 0);
		EnableWindow(dither, 0);

		if (G_DXConfig->MMX)
		{
			EnableWindow(filter, 1);
			SendMessage(filter, BM_SETCHECK, 1, 0);
		}
		else
			EnableWindow(filter, 0);

		if (!G_DXConfig->MMX)
			EnableWindow(tex_8bit, 0);

		EnableWindow(agp_mem, 0);

		if (!G_DXConfig->MMX)
			SendMessage(tex_8bit, BM_SETCHECK, 0, 0);

		SendMessage(agp_mem, BM_SETCHECK, 0, 0);
	}
}

void inject_dxdialog(bool replace)
{
	INJECT(0x00496C20, DXSetupDlgProc, replace);
	INJECT(0x00496BB0, DXUserDialog, replace);
	INJECT(0x004977D0, DXInitD3DDrivers, replace);
}
