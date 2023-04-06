#pragma once
#include "../global/types.h"

bool WinDXInit(DEVICEINFO* device, DXCONFIG* config, bool createNew);
void WinAppExit();
LRESULT CALLBACK WinAppProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
long WinRegisterWindow(HINSTANCE hinstance);
float WinFrameRate();
void WinFreeDX(bool free_dd);
void WinSetStyle(bool fullscreen, ulong& set);
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd);
void S_ExitSystem(const char* msg);
void Log(const char* s, ...);		//NOT present in original code

#ifdef DO_LOG
extern FILE* logF;
#endif

extern WINAPP App;
extern HWCONFIG HWConfig;
extern char* G_lpCmdLine;
extern long game_closedown;
extern bool GtWindowClosed;
extern long distanceFogValue;
extern long farz;
