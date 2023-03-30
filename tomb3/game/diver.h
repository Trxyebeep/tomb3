#pragma once
#include "../global/types.h"

void inject_diver(bool replace);

long GetWaterSurface(long x, long y, long z, short room_number);
void ControlGhostGasEmitter(short item_number);
void DiverControl(short item_number);
