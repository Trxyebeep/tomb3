#pragma once
#include "../global/types.h"

void InitialiseItemArray(short num_items);
void KillItem(short item_num);
short CreateItem();
void InitialiseItem(short item_num);
void RemoveActiveItem(short item_num);
void RemoveDrawnItem(short item_num);
void AddActiveItem(short item_num);
void ItemNewRoom(short item_num, short room_num);
long GlobalItemReplace(long in, long out);
void InitialiseFXArray();
short CreateEffect(short room_num);
void KillEffect(short fx_num);
void EffectNewRoom(short fx_num, short room_num);
void ClearBodyBag();

extern short next_item_active;
extern short next_fx_active;
extern short body_bag;
