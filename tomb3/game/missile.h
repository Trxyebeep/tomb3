#pragma once
#include "../global/vars.h"

void inject_missile(bool replace);

void ControlMissile(short fx_number);
void ShootAtLara(FX_INFO* fx);
long ExplodingDeath(short item_number, long mesh_bits, short counter);
void ControlBodyPart(short fx_number);
