#pragma once
#include "../global/vars.h"

#define AlterFOV	( (void(__cdecl*)(short)) 0x00402030 )


#define InsertLine	( *(void(__cdecl**)(long, long, long, long, long, long, long)) 0x005A6AF8 )
