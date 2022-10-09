#pragma once
#include "../global/vars.h"

void inject_display(bool replace);

void setup_screen_size();
void IncreaseScreenSize();
void DecreaseScreenSize();
void TempVideoAdjust(long a, double sizer);
void TempVideoRemove();

#define S_FadeInInventory	( (void(__cdecl*)(long)) 0x00475990 )
#define S_FadeOutInventory	( (void(__cdecl*)(long)) 0x004759D0 )
