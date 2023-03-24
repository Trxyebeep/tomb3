#pragma once
#include "../global/vars.h"

void inject_inventry(bool replace);

long GetDebouncedInput(long in);
void SelectMeshes(INVENTORY_ITEM* item);
long AnimateInventoryItem(INVENTORY_ITEM* item);
void DrawInventoryItem(INVENTORY_ITEM* item);
void GetGlobeMeshFlags();
void Construct_Inventory();
long Display_Inventory(long mode);

extern long Inventory_ExtraData[8];
