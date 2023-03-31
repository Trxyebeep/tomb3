#pragma once
#include "../global/types.h"

bool S_LoadSettings();
void S_SaveSettings();
void CheckCheatMode();
long TitleSequence();
long GameMain();

extern long HiResFlag;
extern long title_loaded;
extern char exit_message[128];
extern bool inject_rando;
