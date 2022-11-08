#pragma once
#include "../global/vars.h"

void inject_minecart(bool replace);

void MineCartInitialise(short item_number);

#define MineCartControl	( (long(__cdecl*)()) 0x00453B80 )
