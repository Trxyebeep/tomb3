#pragma once
#include "../global/vars.h"

void inject_effects(bool replace);

void LaraBreath(ITEM_INFO* item);
long ItemNearLara(PHD_3DPOS* pos, long rad);
void SoundEffects();
void Richochet(GAME_VECTOR* pos);
void CreateBubble(PHD_3DPOS* pos, short room_number, long size, long sizerange);
void LaraBubbles(ITEM_INFO* item);

#define effect_routines (*(void(__cdecl*(*)[60])(ITEM_INFO*)) 0x004C5478)

#define WadeSplash	( (void(__cdecl*)(ITEM_INFO*, long, long)) 0x0042E9F0 )
#define Splash	( (void(__cdecl*)(ITEM_INFO*)) 0x0042E8C0 )
#define DoBloodSplat	( (short(__cdecl*)(long, long, long, short, short, short)) 0x0042E2C0 )
#define DoLotsOfBlood	( (void(__cdecl*)(long, long, long, short, short, short, long)) 0x0042E3B0 )
#define DoLotsOfBloodD	( (void(__cdecl*)(long, long, long, short, short, short, long)) 0x0042E460 )
