#pragma once
#include "../global/vars.h"

void inject_moveblok(bool replace);

void ClearMovableBlockSplitters(long x, long y, long z, short room_number);
void AlterFloorHeight(ITEM_INFO* item, long height);

#define MovableBlock	( (void(__cdecl*)(short)) 0x00456DD0 )
#define SetupCleanerFromSavegame	( (void(__cdecl*)(ITEM_INFO*, long)) 0x00457800 )
