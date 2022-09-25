#pragma once
#include "../global/vars.h"

void inject_lara1gun(bool replace);

void ControlHarpoonBolt(short item_number);
void ControlRocket(short item_number);
void ControlGrenade(short item_number);
void draw_shotgun_meshes(long weapon_type);
void undraw_shotgun_meshes(long weapon_type);

#define TriggerUnderwaterExplosion	( (void(__cdecl*)(ITEM_INFO*)) 0x00447D90 )
