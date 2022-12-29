#pragma once
#include "../global/vars.h"

void inject_moveblok(bool replace);

void ClearMovableBlockSplitters(long x, long y, long z, short room_number);
void AlterFloorHeight(ITEM_INFO* item, long height);
void DrawUnclippedItem(ITEM_INFO* item);
void SetupCleanerFromSavegame(ITEM_INFO* item, long block);

#define MovableBlock	( (void(__cdecl*)(short)) 0x00456DD0 )
