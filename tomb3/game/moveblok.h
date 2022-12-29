#pragma once
#include "../global/vars.h"

void inject_moveblok(bool replace);

void ClearMovableBlockSplitters(long x, long y, long z, short room_number);
void AlterFloorHeight(ITEM_INFO* item, long height);
void DrawUnclippedItem(ITEM_INFO* item);
void SetupCleanerFromSavegame(ITEM_INFO* item, long block);
void InitialiseMovingBlock(short item_number);
void MovableBlock(short item_number);
void MovableBlockCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void DrawMovableBlock(ITEM_INFO* item);

#define orig_MovableBlock	( (void(__cdecl*)(short)) 0x00456DD0 )
