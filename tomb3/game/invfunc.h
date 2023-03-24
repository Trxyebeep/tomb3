#pragma once
#include "../global/vars.h"

void inject_invfunc(bool replace);

void InitColours();

void Init_Requester(REQUEST_INFO* req);
void Remove_Requester(REQUEST_INFO* req);
void ReqItemCentreAlign(REQUEST_INFO* req, TEXTSTRING* txt);
void ReqItemLeftalign(REQUEST_INFO* req, TEXTSTRING* txt);
void ReqItemRightalign(REQUEST_INFO* req, TEXTSTRING* txt);
long Display_Requester(REQUEST_INFO* req, long des, long backgrounds);
void SetRequesterHeading(REQUEST_INFO* req, const char* text1, ulong flags1, const char* text2, ulong flags2);
void RemoveAllReqItems(REQUEST_INFO* req);
void ChangeRequesterItem(REQUEST_INFO* req, long item, const char* text1, ulong flags1, const char* text2, ulong flags2);
void AddRequesterItem(REQUEST_INFO* req, const char* text1, ulong flags1, const char* text2, ulong flags2);
void SetPCRequesterSize(REQUEST_INFO* req, long nLines, long y);
long AddAssaultTime(ulong time);
long AddQuadbikeTime(ulong time);
void ShowGymStatsText(const char* time, long type);
void ShowStatsText(const char* time, long type);
void ShowEndStatsText();

void Inv_RingMotionInit(RING_INFO* ring, short count, short status, short status_target);
void Inv_RingMotionSetup(RING_INFO* ring, short status, short status_target, short count);
void Inv_RingMotionRadius(RING_INFO* ring, short target);
void Inv_RingMotionRotation(RING_INFO* ring, short rotation, short target);
void Inv_RingMotionCameraPos(RING_INFO* ring, short target);
void Inv_RingMotionCameraPitch(RING_INFO* ring, short target);
void Inv_RingMotionItemSelect(RING_INFO* ring, INVENTORY_ITEM* inv_item);
void Inv_RingMotionItemDeselect(RING_INFO* ring, INVENTORY_ITEM* inv_item);
void Inv_RingDoMotions(RING_INFO* ring);

void Inv_RingInit(RING_INFO* ring, short type, INVENTORY_ITEM** list, short qty, short current, IMOTION_INFO* imo);
void Inv_RingGetView(RING_INFO* ring, PHD_3DPOS* viewer);
void Inv_RingLight(RING_INFO* ring, short object_number);
void Inv_GlobeLight(short mesh_number);
void Inv_RingCalcAdders(RING_INFO* ring, short duration);
void Inv_RingRotateLeft(RING_INFO* ring);
void Inv_RingRotateRight(RING_INFO* ring);
void RingIsOpen(RING_INFO* ring);
void RingIsNotOpen(RING_INFO* ring);
void RingActive();
void RingNotActive(INVENTORY_ITEM* inv_item);

long Inv_GetItemOption(long item_number);
long Inv_RequestItem(long item_number);
void Inv_InsertItem(INVENTORY_ITEM* item);
long Inv_RemoveItem(long item_number);
void Inv_RemoveAllItems();
long Inv_AddItem(long item_number);

void RemoveInventoryText();

extern GOURAUD_FILL req_main_gour1;
extern GOURAUD_OUTLINE req_main_gour2;
extern GOURAUD_FILL req_bgnd_gour1;
extern GOURAUD_OUTLINE req_bgnd_gour2;
extern GOURAUD_FILL req_sel_gour1;
extern GOURAUD_OUTLINE req_sel_gour2;

extern ulong RequesterFlags1[24];
extern ulong RequesterFlags2[24];
extern ulong SaveGameReqFlags1[24];
extern ulong SaveGameReqFlags2[24];
extern short inv_colours[17];
extern char Valid_Level_Strings[24][50];
extern char Valid_Level_Strings2[24][50];

#ifdef TROYESTUFF
extern short gLevelSecrets[21];
#endif
extern short LevelSecrets[21];

#ifdef TROYESTUFF
extern bool noAdditiveBG;
#endif
