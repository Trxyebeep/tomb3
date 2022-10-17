#pragma once
#include "../global/vars.h"

void inject_utils(bool replace);

double UT_GetAccurateTimer();
void UT_InitAccurateTimer();
void UT_CenterWindow(HWND hwnd);

#define UT_OKCancelBox	( (bool(__cdecl*)(char*, HWND)) 0x0048E530 )
