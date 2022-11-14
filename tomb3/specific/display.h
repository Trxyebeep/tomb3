#pragma once
#include "../global/vars.h"

void inject_display(bool replace);

void setup_screen_size();
void IncreaseScreenSize();
void DecreaseScreenSize();
void TempVideoAdjust(long a, double sizer);
void TempVideoRemove();
void S_FadeInInventory(long fade);
void S_FadeOutInventory(long fade);
