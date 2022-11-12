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
#define IDC_VERSION				1016
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

void DXInitVideoModes(HWND hwnd, long nDD, long nD3D)
{
	DIRECT3DINFO* d3dinfo;
	DISPLAYMODE* dm;
	HWND res;
	static long selected = -1;
	long nVM, nListed;
	char Default[32];
	char vm[40];

	nVM = -1;

	if (bSoftwareDefault)
		strcpy(Default, "320x200 High Colour (16 Bit)");
	else
		strcpy(Default, "640x480 High Colour (16 Bit)");

	res = GetDlgItem(hwnd, IDC_RESOLUTION);

	if (selected != -1)
		SendMessage(res, CB_GETLBTEXT, SendMessage(res, CB_GETCURSEL, 0, 0), (LPARAM)Default);

	SendMessage(res, CB_RESETCONTENT, 0, 0);
	nListed = 0;

	d3dinfo = &G_DeviceInfo->DDInfo[nDD].D3DInfo[nD3D];

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

		if (dm->bpp == 24)
		{
			sprintf(vm, "%dx%d True Colour (%d Bit)", dm->w, dm->h, 24);
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

void DXInitTextures(HWND hwnd, long nDD, long nD3D)
{
	DIRECT3DINFO* d3dinfo;
	D3DTEXTUREINFO* tex;
	HWND format;
	static long selected = -1;
	long nTF;
	char old[20];
	char buf[20];
	char bit[12];
	bool palette, found;

	nTF = -1;
	format = GetDlgItem(hwnd, IDC_D3DTF);

	if (selected != -1)
		SendMessage(format, CB_GETLBTEXT, SendMessage(format, CB_GETCURSEL, 0, 0), (LPARAM)old);

	SendMessage(format, CB_RESETCONTENT, 0, 0);
	palette = 0;

	d3dinfo = &G_DeviceInfo->DDInfo[nDD].D3DInfo[nD3D];

	for (int i = 0; i < d3dinfo->nTexture; i++)
	{
		tex = &d3dinfo->Texture[i];

		if (tex->bPalette)
		{
			palette = 1;
			sprintf(buf, "%d Bit", tex->bpp);
		}
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

	if (palette)
	{
		if (SendMessage(GetDlgItem(hwnd, IDC_8BIT_TEXTURES), BM_GETCHECK, 0, 0))
		{
			strcpy(bit, "8 Bit");

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
	}
	else
		EnableWindow(GetDlgItem(hwnd, IDC_8BIT_TEXTURES), 0);

	if (G_DXConfig->MMX && SendMessage(GetDlgItem(hwnd, IDC_SOFTWARE), BM_GETCHECK, 0, 0))
	{
		if (!found)
		{
			strcpy(bit, "8888");

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
	}

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

void DXInitDSAdapters(HWND hwnd)
{
	HWND sound, disable_sound;

	sound = GetDlgItem(hwnd, IDC_SOUND);
	disable_sound = GetDlgItem(hwnd, IDC_DISABLE_SOUND);

	if (G_DeviceInfo->DSInfo)
	{
		SendMessage(sound, CB_RESETCONTENT, 0, 0);

		for (int i = 0; i < G_DeviceInfo->nDSInfo; i++)
			SendMessage(sound, CB_ADDSTRING, 0, (LPARAM)G_DeviceInfo->DSInfo[i].About);

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

void DXInitJoystickAdapter(HWND hwnd)
{
	HWND joystick, disable_joystick;

	joystick = GetDlgItem(hwnd, IDC_JOYSTICK);
	disable_joystick = GetDlgItem(hwnd, IDC_DISABLE_JOYSTICK);

	if (G_DeviceInfo->DIInfo)
	{
		SendMessage(joystick, CB_RESETCONTENT, 0, 0);

		for (int i = 0; i < G_DeviceInfo->nDIInfo; i++)
			SendMessage(joystick, CB_ADDSTRING, 0, (LPARAM)G_DeviceInfo->DIInfo[i].About);

		SendMessage(joystick, CB_SETCURSEL, 0, 0);
	}
	else
	{
		EnableWindow(disable_joystick, 0);
		SendMessage(disable_joystick, BM_SETCHECK, 1, 0);
		SendMessage(joystick, CB_ADDSTRING, 0, (LPARAM)"No Joystick Adapter Available");
		SendMessage(joystick, CB_SETCURSEL, 0, 0);
		EnableWindow(joystick, 0);
	}
}

void DXCheckMMXTechnology(HWND hwnd)
{
	HWND version;
	ulong maxCPUID, processorType, info, features, unk1, unk2;
	long mmx;
	char buf[32];
	char name[13];

	version = GetDlgItem(hwnd, IDC_VERSION);
	maxCPUID = 0;
	processorType = 0;
	info = 0;
	features = 0;
	unk1 = 0;
	unk2 = 0;
	strcpy(name, "AnonymousCPU");

	__asm
	{
		pushad
		mov processorType, 4;
		pushfd
		pop eax
		mov ecx, eax
		xor eax, 200000h
		push eax
		popfd
		pushfd
		pop eax
		xor eax, ecx
		je end

		mov maxCPUID, 0
		mov eax, 0;	//get largest eax value, and CPU name
		cpuid
		mov maxCPUID, eax
		mov dword ptr[name], ebx
		mov dword ptr[name + 4], edx
		mov dword ptr[name + 8], ecx

		mov eax, 1;	//get version info and feature bits
		cpuid
		mov info, eax
		mov features, edx

		shr eax, 8; //actually getting the type now
		and eax, 0Fh
		mov processorType, eax

		cmp eax, 5;	//no idea tbh
		jl end
		shr eax, 8
		and eax, 100h
		setne byte ptr unk1

		and edx, 10h
		je end
		shr eax, 8
		and eax, 4
		sete byte ptr unk2

	end:
		popad
	}

	mmx = (features >> 23) & 1;

	if (!strcmp("GenuineIntel", name))
	{
		if (processorType == 5)
		{
			if (mmx)
				strcpy(buf, "Intel Pentium MMX");
			else
				strcpy(buf, "Intel Pentium");
		}
		else if (processorType == 6)
		{
			if (mmx)
				strcpy(buf, "Intel Pentium Pro MMX");
			else
				strcpy(buf, "Intel Pentium Pro");
		}
	}

	if (!strcmp("AuthenticAMD", name))
	{
		if (processorType == 5)
			strcpy(buf, "AMD K5");
		else if (processorType == 6)
			strcpy(buf, "AMD K6");
	}

	if (!strcmp("CyrixInstead", name))
	{
		if (processorType == 5)
			strcpy(buf, "Cyrix 6x86");
		else if (processorType == 6)
			strcpy(buf, "Cyrix M2");
	}

	SetWindowText(version, buf);
	G_DXConfig->MMX = mmx;
}

void DXInitDialogBox(HWND hwnd)
{
	HWND gfx;
	long nDD;
	char abt[80];

	gfx = GetDlgItem(hwnd, IDC_GRAPHICS_ADAPTER);

	for (int i = 0; i < G_DeviceInfo->nDDInfo; i++)
	{
		sprintf(abt, "%s (%s)", G_DeviceInfo->DDInfo[i].About, G_DeviceInfo->DDInfo[i].Name);
		SendMessage(gfx, CB_ADDSTRING, 0, (LPARAM)abt);
	}

	if (bSoftwareDefault)
		nDD = 0;
	else
		nDD = G_DeviceInfo->nDDInfo - 1;

	SendMessage(gfx, CB_SETCURSEL, nDD, 0);
	DXCheckMMXTechnology(hwnd);
	DXInitD3DDrivers(hwnd, nDD);
	DXInitVideoModes(hwnd, nDD, SendMessage(GetDlgItem(hwnd, IDC_OUTPUT_SETTINGS), CB_GETCURSEL, 0, 0));
	DXInitTextures(hwnd, nDD, SendMessage(GetDlgItem(hwnd, IDC_OUTPUT_SETTINGS), CB_GETCURSEL, 0, 0));
	DXInitDSAdapters(hwnd);
	DXInitJoystickAdapter(hwnd);
}

void inject_dxdialog(bool replace)
{
	INJECT(0x00496C20, DXSetupDlgProc, replace);
	INJECT(0x00496BB0, DXUserDialog, replace);
	INJECT(0x004977D0, DXInitD3DDrivers, replace);
	INJECT(0x00497C20, DXInitVideoModes, replace);
	INJECT(0x00497FE0, DXInitTextures, replace);
	INJECT(0x00497630, DXInitDSAdapters, replace);
	INJECT(0x00497700, DXInitJoystickAdapter, replace);
	INJECT(0x00497290, DXCheckMMXTechnology, replace);
	INJECT(0x00497530, DXInitDialogBox, replace);
}
