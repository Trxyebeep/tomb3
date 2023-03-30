#pragma once
#include "../global/types.h"

long WeaponObject(long weapon_type);
long FireWeapon(long weapon_type, ITEM_INFO* target, ITEM_INFO* source, short* angles);
void HitTarget(ITEM_INFO* item, GAME_VECTOR* hitpos, long damage);
void SmashItem(short item_number, long weapon_type);
void AimWeapon(WEAPON_INFO* winfo, LARA_ARM* arm);
void find_target_point(ITEM_INFO* item, GAME_VECTOR* target);
void LaraTargetInfo(WEAPON_INFO* winfo);
void LaraGetNewTarget(WEAPON_INFO* winfo);
void InitialiseNewWeapon();
void LaraGun();
void DoProperDetection(short item_number, long x, long y, long z, long xv, long yv, long zv);

extern WEAPON_INFO weapons[11];
