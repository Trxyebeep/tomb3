#pragma once
#include "../global/vars.h"

void inject_dxdialog(bool replace);

BOOL CALLBACK DXSetupDlgProc(HWND dlg, UINT message, WPARAM wParam, LPARAM lParam);
bool DXUserDialog(DEVICEINFO* device, DXCONFIG* config, HINSTANCE hinstance);

#define DXInitDialogBox	( (void(__cdecl*)(HWND)) 0x00497530 )
#define DXInitD3DDrivers	( (void(__cdecl*)(HWND, long)) 0x004977D0 )
#define DXInitVideoModes	( (void(__cdecl*)(HWND, long, long)) 0x00497C20 )
#define DXInitTextures	( (void(__cdecl*)(HWND, long, long)) 0x00497FE0 )
