#pragma once
#include "../global/vars.h"

void inject_input(bool replace);

long Key(long number);
long S_UpdateInput();

extern const char* KeyboardButtons[272];
extern short layout[2][NLAYOUTKEYS];
extern long conflict[15];
extern uchar keymap[256];
