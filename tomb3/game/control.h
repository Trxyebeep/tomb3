#pragma once
#include "../global/vars.h"

void inject_control(bool replace);

long ControlPhase(long nframes, long demo_mode);
void AnimateItem(ITEM_INFO* item);
long GetChange(ITEM_INFO* item, ANIM_STRUCT* anim);
void TranslateItem(ITEM_INFO* item, long x, long y, long z);
FLOOR_INFO* GetFloor(long x, long y, long z, short* room_number);
long GetWaterHeight(long x, long y, long z, short room_number);
long GetHeight(FLOOR_INFO* floor, long x, long y, long z);
void RefreshCamera(short type, short* data);
void TestTriggers(short* data, long heavy);
long TriggerActive(ITEM_INFO* item);
long GetCeiling(FLOOR_INFO* floor, long x, long y, long z);
short GetDoor(FLOOR_INFO* floor);
long LOS(GAME_VECTOR* start, GAME_VECTOR* target);
long zLOS(GAME_VECTOR* start, GAME_VECTOR* target);
long xLOS(GAME_VECTOR* start, GAME_VECTOR* target);
long ClipTarget(GAME_VECTOR* start, GAME_VECTOR* target);

#define CheckNoColFloorTriangle	( (long(__cdecl*)(FLOOR_INFO*, long, long)) 0x004231F0 )
#define CheckNoColCeilingTriangle	( (long(__cdecl*)(FLOOR_INFO*, long, long)) 0x004232B0 )
#define ObjectOnLOS	( (long(__cdecl*)(GAME_VECTOR*, GAME_VECTOR*)) 0x00422C30 )
#define IsRoomOutside	( (long(__cdecl*)(long, long, long)) 0x004233B0 )
#define TriggerCDTrack	( (void(__cdecl*)(short, short, short)) 0x00423110 )
#define FlipMap	( (void(__cdecl*)()) 0x00422F40 )
