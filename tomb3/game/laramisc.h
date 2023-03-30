#pragma once
#include "../global/types.h"

void LaraCheatGetStuff();
void LaraCheatyBits();
void LaraCheat(ITEM_INFO* item, COLL_INFO* coll);
void LaraInitialiseMeshes(long level_number);
void LaraControl(short item_number);
void AnimateLara(ITEM_INFO* item);
void UseItem(short object_number);
void ControlLaraExtra(short item_number);
void InitialiseLaraLoad(short item_number);
void InitialiseLara(long type);
void InitialiseLaraInventory(long level);

extern void (*extra_control_routines[13])(ITEM_INFO* item, COLL_INFO* coll);
extern void (*lara_control_routines[89])(ITEM_INFO* item, COLL_INFO* coll);
extern void (*lara_collision_routines[89])(ITEM_INFO* item, COLL_INFO* coll);
