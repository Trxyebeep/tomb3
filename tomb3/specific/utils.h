#pragma once
#include "../global/types.h"

double UT_GetAccurateTimer();
void UT_InitAccurateTimer();
void UT_CenterWindow(HWND hwnd);
char* UT_FindArg(const char* arg);
INT_PTR CALLBACK UT_OKCB_DlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
bool UT_OKCancelBox(char* lpTemplateName, HWND hWndParent);
