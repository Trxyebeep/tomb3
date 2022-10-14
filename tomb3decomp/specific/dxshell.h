#pragma once
#include "../global/vars.h"

void inject_dxshell(bool replace);

long BPPToDDBD(long BPP);

#define DXBitMask2ShiftCnt	( (void(__cdecl*)(ulong, uchar*, uchar*)) 0x0048F1C0 )
#define DXSwitchVideoMode	( (bool(__cdecl*)(long, long, bool)) 0x004B3550 )
