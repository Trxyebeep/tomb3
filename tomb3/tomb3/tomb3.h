#pragma once
#include "../global/types.h"

void T3_SaveSettings();
bool T3_LoadSettings();
void T3_GoldifyString(char* string);

extern TOMB3_OPTIONS tomb3;
extern TOMB3_SAVE tomb3_save;
extern ulong tomb3_save_size;
extern ulong water_color[24];
