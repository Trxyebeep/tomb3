#pragma once
#include "../global/vars.h"

void inject_phdmath(bool replace);

ulong __fastcall phd_sqrt(ulong num);

#define phd_sin	( (long(__fastcall*)(long)) 0x004B4C5E )
#define phd_cos	( (long(__fastcall*)(long)) 0x004B4C58 )
#define phd_atan	( (long(__fastcall*)(long, long)) 0x004B4C10 )
