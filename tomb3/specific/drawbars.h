#pragma once
#include "../global/types.h"

void S_DrawHealthBar(long percent);
void S_DrawDashBar(long percent);
void S_DrawAirBar(long percent);
void S_DrawColdBar(long percent);
void DoPSXHealthBar(long x0, long y0, long x1, long y1, long bar, long p);
void DoPSXDashBar(long x0, long y0, long x1, long y1, long bar, long p);
void DoPSXAirBar(long x0, long y0, long x1, long y1, long bar, long p);
void DoPSXColdBar(long x0, long y0, long x1, long y1, long bar, long p);
