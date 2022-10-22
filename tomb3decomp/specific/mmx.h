#pragma once
#include "../global/vars.h"

#define MMXBlit32to15	( (void(__cdecl*)(uchar*, ulong*, long)) 0x00496930 )
#define MMXBlit32to16	( (void(__cdecl*)(uchar*, ulong*, long)) 0x00496A70 )
#define MMXBlit32to24	( (void(__cdecl*)(uchar*, ulong*, long)) 0x00496B20 )
