#pragma once
#include "../global/vars.h"

void inject_items(bool replace);

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

#define EffectNewRoom	( (void(__cdecl*)(short, short)) 0x0043B290 )
#define ClearBodyBag	( (void(__cdecl*)()) 0x0043B330 )
