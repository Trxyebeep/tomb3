#pragma once
#include "../global/vars.h"

void inject_winmain(bool replace);

bool WinDXInit(DEVICEINFO* device, DXCONFIG* config, bool createNew);
void WinAppExit();
LRESULT CALLBACK WinAppProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
long WinRegisterWindow(HINSTANCE hinstance);
HWND WinCreateWindow(HINSTANCE hinstance, long nCmdShow);
float WinFrameRate();
void WinFreeDX(bool free_dd);

#define S_ExitSystem	( (void(__cdecl*)(const char*)) 0x004B37C0 )
