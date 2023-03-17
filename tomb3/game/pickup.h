#pragma once
#include "../global/vars.h"

void inject_pickup(bool replace);

void PickUpCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll);
void BossDropIcon(short item_number);
void AnimatingPickUp(short item_number);
void PuzzleHoleCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void KeyHoleCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void DetonatorCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void SwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void SwitchCollision2(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void SwitchControl(short item_number);
long SwitchTrigger(short item_number, short timer);
long KeyTrigger(short item_number);

#define PickupTrigger	( (long(__cdecl*)(short)) 0x0045CDA0 )

#define orig_PuzzleHoleCollision	( (void(__cdecl*)(short, ITEM_INFO*, COLL_INFO*)) 0x0045C900 )
#define orig_PickUpCollision	( (void(__cdecl*)(short, ITEM_INFO*, COLL_INFO*)) 0x0045BC00 )
