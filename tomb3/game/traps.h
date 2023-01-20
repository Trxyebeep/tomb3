#pragma once
#include "../global/vars.h"

void inject_traps(bool replace);

void LaraBurn();
void LavaBurn(ITEM_INFO* item);
void SpikeControl(short item_number);
void PropellerControl(short item_number);
void SideFlameDetection(FX_INFO* fx, long length);
void FlameControl(short fx_number);
void ControlSpikeWall(short item_number);
void ControlCeilingSpikes(short item_number);
void TriggerPendulumFlame(short item_number);
