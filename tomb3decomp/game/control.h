#pragma once
#include "../global/vars.h"

void inject_control(bool replace);

long ControlPhase(long nframes, long demo_mode);
void AnimateItem(ITEM_INFO* item);
long GetChange(ITEM_INFO* item, ANIM_STRUCT* anim);
void TranslateItem(ITEM_INFO* item, long x, long y, long z);

#define GetFloor	( (FLOOR_INFO*(__cdecl*)(long, long, long, short*)) 0x00420A80 )
#define GetHeight	( (long(__cdecl*)(FLOOR_INFO*, long, long, long)) 0x00420E10 )
#define GetCeiling	( (long(__cdecl*)(FLOOR_INFO*, long, long, long)) 0x00421DE0 )
#define GetWaterHeight	( (long(__cdecl*)(long, long, long, short)) 0x00420C70 )
#define TestTriggers	( (void(__cdecl*)(short*, long)) 0x00421460 )
#define TriggerActive	( (long(__cdecl*)(ITEM_INFO*)) 0x00421D80 )
#define LOS	( (long(__cdecl*)(GAME_VECTOR*, GAME_VECTOR*)) 0x00422370 )
