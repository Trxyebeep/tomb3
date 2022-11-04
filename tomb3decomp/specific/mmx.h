#pragma once
#include "../global/vars.h"

void inject_mmx(bool replace);

void MMXBlit32to15(uchar* dest, ulong* src, long w);

#define MMXBlit32to16	( (void(__cdecl*)(uchar*, ulong*, long)) 0x00496A70 )
#define MMXBlit32to24	( (void(__cdecl*)(uchar*, ulong*, long)) 0x00496B20 )
