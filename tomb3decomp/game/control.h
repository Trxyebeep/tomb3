#pragma once
#include "../global/vars.h"

#define GetFloor	( (FLOOR_INFO*(__cdecl*)(long, long, long, short*)) 0x00420A80 )
#define GetHeight	( (long(__cdecl*)(FLOOR_INFO*, long, long, long)) 0x00420E10 )
#define GetCeiling	( (long(__cdecl*)(FLOOR_INFO*, long, long, long)) 0x00421DE0 )
#define GetWaterHeight	( (long(__cdecl*)(long, long, long, short)) 0x00420C70 )
#define TestTriggers	( (void(__cdecl*)(short*, long)) 0x00421460 )
