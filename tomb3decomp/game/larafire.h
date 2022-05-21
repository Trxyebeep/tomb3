#pragma once
#include "../global/vars.h"

void inject_larafire(bool replace);

long WeaponObject(long weapon_type);

#define LaraGun	( (void(__cdecl*)()) 0x00449BB0 )
#define FireWeapon	( (long(__cdecl*)(long, ITEM_INFO*, ITEM_INFO*, short*)) 0x0044A890 )
#define InitialiseNewWeapon	( (void(__cdecl*)()) 0x0044A1E0 )
