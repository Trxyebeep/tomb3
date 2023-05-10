#pragma once
#include "../global/types.h"

void setup_screen_size();
void IncreaseScreenSize();
void DecreaseScreenSize();
void TempVideoAdjust(long a, double sizer);
void TempVideoRemove();

extern double screen_sizer;
extern double game_sizer;
extern long VidSizeLocked;
extern short DumpX;
extern short DumpY;
extern short DumpWidth;
extern short DumpHeight;
extern char GtFullScreenClearNeeded;
