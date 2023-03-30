#pragma once
#include "../global/types.h"

void ShiftItem(ITEM_INFO* item, COLL_INFO* coll);
void ObjectCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll);
void ObjectCollisionSub(short item_num, ITEM_INFO* l, COLL_INFO* coll);
void GetNewRoom(long x, long y, long z, short room_number);
void GetNearByRooms(long x, long y, long z, long r, long h, short room_number);
short GetTiltType(FLOOR_INFO* floor, long x, long y, long z);
long CollideStaticObjects(COLL_INFO* coll, long x, long y, long z, short room_number, long hite);
long FindGridShift(long src, long dst);
void GetCollisionInfo(COLL_INFO* coll, long x, long y, long z, short room_number, long hite);
void UpdateLaraRoom(ITEM_INFO* item, long height);
void DoorCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void TrapCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
long TestBoundsCollide(ITEM_INFO* item, ITEM_INFO* l, long rad);
long TestLaraPosition(short* bounds, ITEM_INFO* item, ITEM_INFO* l);
void AlignLaraPosition(PHD_VECTOR* pos, ITEM_INFO* item, ITEM_INFO* l);
long Move3DPosTo3DPos(PHD_3DPOS* pos, PHD_3DPOS* dest, long speed, short rotation);
long MoveLaraPosition(PHD_VECTOR* v, ITEM_INFO* item, ITEM_INFO* l);
void CreatureCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void LaraBaddieCollision(ITEM_INFO* l, COLL_INFO* coll);
void ItemPushLara(ITEM_INFO* item, ITEM_INFO* l, COLL_INFO* coll, long spaz, long BigPush);
void AIPickupCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
