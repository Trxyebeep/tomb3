#pragma once
#include "../global/vars.h"

void inject_drawbars(bool replace);

void S_DrawHealthBar(long percent);
void S_DrawDashBar(long percent);
void S_DrawAirBar(long percent);
void S_DrawColdBar(long percent);
