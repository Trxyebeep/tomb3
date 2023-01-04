#pragma once
#include "../global/vars.h"

void inject_lara1gun(bool replace);

void ControlHarpoonBolt(short item_number);
void ControlRocket(short item_number);
void ControlGrenade(short item_number);
void draw_shotgun_meshes(long weapon_type);
void undraw_shotgun_meshes(long weapon_type);

#define TriggerUnderwaterExplosion	( (void(__cdecl*)(ITEM_INFO*)) 0x00447D90 )
#define draw_shotgun	( (void(__cdecl*)(long)) 0x004475D0 )
#define undraw_shotgun	( (void(__cdecl*)(long)) 0x00447770 )
#define RifleHandler	( (void(__cdecl*)(long)) 0x00445340 )
