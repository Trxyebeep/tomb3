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
				if (SendMessageA(GetDlgItem(dlg, IDC_DISABLE_SOUND), BM_GETCHECK, 0, 0))
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

void inject_dxdialog(bool replace)
{
	INJECT(0x00496C20, DXSetupDlgProc, replace);
	INJECT(0x00496BB0, DXUserDialog, replace);
}
