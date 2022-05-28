#pragma once
#include "../global/vars.h"

void inject_sub(bool replace);

void SubInitialise(short item_number);

#define SubControl	( (long(__cdecl*)()) 0x00468C10 )
