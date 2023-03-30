#pragma once
#include "../global/types.h"

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
long PickupTrigger(short item_number);
