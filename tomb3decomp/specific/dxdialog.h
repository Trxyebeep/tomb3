#pragma once
#include "../global/vars.h"

void inject_dxdialog(bool replace);

BOOL CALLBACK DXSetupDlgProc(HWND dlg, UINT message, WPARAM wParam, LPARAM lParam);
bool DXUserDialog(DEVICEINFO* device, DXCONFIG* config, HINSTANCE hinstance);
void DXInitD3DDrivers(HWND hwnd, long nDrivers);
void DXInitVideoModes(HWND hwnd, long nDD, long nD3D);
void DXInitTextures(HWND hwnd, long nDD, long nD3D);
void DXInitDSAdapters(HWND hwnd);
void DXInitJoystickAdapter(HWND hwnd);
void DXCheckMMXTechnology(HWND hwnd);
void DXInitDialogBox(HWND hwnd);
