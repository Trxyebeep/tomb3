#pragma once
#include "../global/types.h"

void inject_phdmath(bool replace);

ulong __fastcall phd_sqrt(ulong num);
long __fastcall phd_atan(long x, long y);
long __fastcall phd_sin(long angle);
long __fastcall phd_cos(long angle);
