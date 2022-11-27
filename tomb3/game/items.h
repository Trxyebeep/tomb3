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

#define CreateEffect	( (short(__cdecl*)(short)) 0x0043B130 )
#define KillEffect	( (void(__cdecl*)(short)) 0x0043B1A0 )
#define ItemNewRoom	( (void(__cdecl*)(short, short)) 0x0043AFD0 )
#define EffectNewRoom	( (void(__cdecl*)(short, short)) 0x0043B290 )
#define GlobalItemReplace	( (void(__cdecl*)(long, long)) 0x0043B080 )
#define ClearBodyBag	( (void(__cdecl*)()) 0x0043B330 )
#define InitialiseFXArray	( (void(__cdecl*)()) 0x0043B100 )
