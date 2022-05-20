#pragma once
#include "../global/vars.h"

void inject_laraflar(bool replace);

void DrawFlareInAir(ITEM_INFO* item);
long DoFlareLight(PHD_VECTOR* pos, long flare_age);
