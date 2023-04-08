#pragma once
#include "../global/types.h"

long Key(long number);
long S_UpdateInput();

extern const char* KeyboardButtons[272];
extern short layout[2][NLAYOUTKEYS];
extern long bLaraOn;
extern long bRoomOn;
extern long bObjectOn;
extern long bAObjectOn;
extern long bEffectOn;
extern char bInvItemsOff;
extern long input;
extern long FinishLevelCheat;
extern long conflict[15];
extern uchar keymap[256];
