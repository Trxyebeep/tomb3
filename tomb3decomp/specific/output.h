#pragma once
#include "../global/vars.h"

void inject_output(bool replace);

void S_PrintShadow(short size, short* box, ITEM_INFO* item);
void S_SetupAboveWater(long underwater);
void S_SetupBelowWater(long underwater);
void S_OutputPolyList();
void S_LightRoom(ROOM_INFO* r);
