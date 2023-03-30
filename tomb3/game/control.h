#pragma once
#include "../global/types.h"

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
long IsRoomOutside(long x, long y, long z);

extern ITEM_INFO* items;
extern ANIM_STRUCT* anims;
extern ROOM_INFO* room;
extern short** meshes;
extern long* bones;
extern long level_items;
extern short number_rooms;

extern short* trigger_index;
extern long tiltxoff;
extern long tiltyoff;
extern long OnObject;
extern long height_type;
extern uchar CurrentAtmosphere;
extern uchar IsAtmospherePlaying;

extern long CurrentLevel;
extern long level_complete;
extern long chunky_flag;
extern long GnGameMode;
extern long reset_flag;
extern long noinput_count;
extern long overlay_flag;
extern short FXType;
extern short item_after_projectile;
extern short cdtrack;
extern char PoisonFlag;

extern long flipmap[10];
extern long flip_status;
extern long flipeffect;
extern long fliptimer;
extern char cd_flags[128];
extern uchar HeavyTriggered;
