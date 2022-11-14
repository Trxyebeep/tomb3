#pragma once
#include "../global/vars.h"

void inject_larafire(bool replace);

long WeaponObject(long weapon_type);
long FireWeapon(long weapon_type, ITEM_INFO* target, ITEM_INFO* source, short* angles);

#define LaraGun	( (void(__cdecl*)()) 0x00449BB0 )
#define InitialiseNewWeapon	( (void(__cdecl*)()) 0x0044A1E0 )
#define DoProperDetection	( (void(__cdecl*)(short, long, long, long, long, long, long)) 0x0044AFB0 )
#define HitTarget	( (void(__cdecl*)(ITEM_INFO*, GAME_VECTOR*, long)) 0x0044AE20 )
#define SmashItem	( (void(__cdecl*)(short, long)) 0x0044AEE0 )
