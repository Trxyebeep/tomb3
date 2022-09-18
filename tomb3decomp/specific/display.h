#pragma once
#include "../global/vars.h"

void inject_display(bool replace);

void setup_screen_size();

#define IncreaseScreenSize	( (void(__cdecl*)()) 0x00475720 )
#define DecreaseScreenSize	( (void(__cdecl*)()) 0x00475790 )
