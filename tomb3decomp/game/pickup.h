#pragma once
#include "../global/vars.h"

void inject_pickup(bool replace);

void PickUpCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll);
void BossDropIcon(short item_number);
void AnimatingPickUp(short item_number);

#define SwitchTrigger	( (long(__cdecl*)(short, short)) 0x0045CCB0 )
#define KeyTrigger	( (long(__cdecl*)(short)) 0x0045CD50 )
#define PickupTrigger	( (long(__cdecl*)(short)) 0x0045CDA0 )
