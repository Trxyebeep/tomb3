#pragma once
#include "../global/types.h"

long GetDebouncedInput(long in);
void SelectMeshes(INVENTORY_ITEM* item);
long AnimateInventoryItem(INVENTORY_ITEM* item);
void DrawInventoryItem(INVENTORY_ITEM* item);
void GetGlobeMeshFlags();
void Construct_Inventory();
long Display_Inventory(long mode);

extern long Inventory_ExtraData[8];
extern long GlobePointLight;
extern short GlobeXRot;
extern short GlobeYRot;
extern short GlobeZRot;
extern short GlobeLevel;
extern uchar GlobeLevelComplete[6];

extern long inputDB;
extern short Option_Music_Volume;
extern short Option_SFX_Volume;
extern long Inventory_Mode;
extern TEXTSTRING* passport_text1;
extern TEXTSTRING* Inv_ringText;
extern TEXTSTRING* Inv_tagText;
extern TEXTSTRING* Inv_upArrow1;
extern TEXTSTRING* Inv_upArrow2;
extern TEXTSTRING* Inv_downArrow1;
extern TEXTSTRING* Inv_downArrow2;
extern long Inventory_DemoMode;
extern long OpenInvOnGym;
extern long idelay;
extern long idcount;
extern short inv_keys_objects;
extern short inv_keys_current;
extern short inv_main_objects;
extern short inv_main_current;
extern short Inventory_Displaying;
extern short Inventory_Chosen;
extern short inv_option_objects;
extern short inv_option_current;
extern short item_data;
