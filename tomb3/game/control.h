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
long ObjectOnLOS(GAME_VECTOR* start, GAME_VECTOR* target);
void FlipMap();
void RemoveRoomFlipItems(ROOM_INFO* r);
void AddRoomFlipItems(ROOM_INFO* r);
void TriggerCDTrack(short value, short flags, short type);
void TriggerNormalCDTrack(short value, short flags, short type);
long CheckNoColFloorTriangle(FLOOR_INFO* floor, long x, long z);
long CheckNoColCeilingTriangle(FLOOR_INFO* floor, long x, long z);

#define IsRoomOutside	( (long(__cdecl*)(long, long, long)) 0x004233B0 )
