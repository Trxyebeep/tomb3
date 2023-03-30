#pragma once
#include "../global/types.h"

void ControlHarpoonBolt(short item_number);
void ControlRocket(short item_number);
void ControlGrenade(short item_number);
void draw_shotgun_meshes(long weapon_type);
void undraw_shotgun_meshes(long weapon_type);
void ready_shotgun(long weapon_type);
void draw_shotgun(long weapon_type);
void undraw_shotgun(long weapon_type);
void FireHarpoon();
void FireRocket();
void FireGrenade();
void FireShotgun();
void FireM16(long running);
void AnimateShotgun(long weapon_type);
void RifleHandler(long weapon_type);
void TriggerUnderwaterExplosion(ITEM_INFO* item);
