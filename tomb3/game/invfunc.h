#pragma once
#include "../global/types.h"

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

enum INV_ORDER	//for main ring
{
	MAP_POS,
	GUN_POS,
	SGN_POS,
	MAG_POS,
	UZI_POS,
	M16_POS,
	ROCKET_POS,
	GRENADE_POS,
	HARPOON_POS,
	FLR_POS,
	EXP_POS,
	BGM_POS,
	MED_POS,
	SGC_POS
};

enum INV_ORDER2	//for keys ring
{
	LBAR_POS = 100,
	KY1_POS,
	KY2_POS,
	KY3_POS,
	KY4_POS,
	PZ4_POS,
	PZ3_POS,
	PZ2_POS,
	PZ1_POS,
	SCN_POS,
	PK2_POS,
	PK1_POS,
	IC1_POS,
	IC2_POS,
	IC3_POS,
	IC4_POS
};


enum INV_ORDER3	//for options ring
{
	PAS_POS,
	DTL_POS,
	SND_POS,
	CNT_POS,
	GAM_POS,
	PIC_POS
};

extern GOURAUD_FILL req_main_gour1;
extern GOURAUD_OUTLINE req_main_gour2;
extern GOURAUD_FILL req_bgnd_gour1;
extern GOURAUD_OUTLINE req_bgnd_gour2;
extern GOURAUD_FILL req_sel_gour1;
extern GOURAUD_OUTLINE req_sel_gour2;

extern ulong inv_colours[17];

extern ulong RequesterFlags1[24];
extern ulong RequesterFlags2[24];
extern ulong SaveGameReqFlags1[24];
extern ulong SaveGameReqFlags2[24];
extern char Save_Game_Strings[24][50];
extern char Save_Game_Strings2[24][50];
extern char Valid_Level_Strings[24][50];
extern char Valid_Level_Strings2[24][50];

extern TEXTSTRING* Inv_itemText[3];
extern short inv_main_qtys[23];
extern short inv_keys_qtys[23];
extern INVENTORY_ITEM icompass_option;
extern INVENTORY_ITEM igun_option;
extern INVENTORY_ITEM ishotgun_option;
extern INVENTORY_ITEM imagnum_option;
extern INVENTORY_ITEM iuzi_option;
extern INVENTORY_ITEM im16_option;
extern INVENTORY_ITEM irocket_option;
extern INVENTORY_ITEM igrenade_option;
extern INVENTORY_ITEM iharpoon_option;
extern INVENTORY_ITEM iflare_option;
extern INVENTORY_ITEM igunammo_option;
extern INVENTORY_ITEM isgunammo_option;
extern INVENTORY_ITEM imagammo_option;
extern INVENTORY_ITEM iuziammo_option;
extern INVENTORY_ITEM im16ammo_option;
extern INVENTORY_ITEM irocketammo_option;
extern INVENTORY_ITEM igrenadeammo_option;
extern INVENTORY_ITEM iharpoonammo_option;
extern INVENTORY_ITEM ibigmedi_option;
extern INVENTORY_ITEM imedi_option;
extern INVENTORY_ITEM sgcrystal_option;
extern INVENTORY_ITEM ipassport_option;
extern INVENTORY_ITEM idetail_option;
extern INVENTORY_ITEM isound_option;
extern INVENTORY_ITEM icontrol_option;
extern INVENTORY_ITEM igamma_option;
extern INVENTORY_ITEM iphoto_option;
extern INVENTORY_ITEM ikey1_option;
extern INVENTORY_ITEM ikey2_option;
extern INVENTORY_ITEM ikey3_option;
extern INVENTORY_ITEM ikey4_option;
extern INVENTORY_ITEM ipuzzle4_option;
extern INVENTORY_ITEM ipuzzle3_option;
extern INVENTORY_ITEM ipuzzle2_option;
extern INVENTORY_ITEM ipuzzle1_option;
extern INVENTORY_ITEM ipickup2_option;
extern INVENTORY_ITEM ipickup1_option;
extern INVENTORY_ITEM icon1_option;
extern INVENTORY_ITEM icon2_option;
extern INVENTORY_ITEM icon3_option;
extern INVENTORY_ITEM icon4_option;
extern REQUEST_INFO Load_Game_Requester;
extern REQUEST_INFO Level_Select_Requester;
extern INVENTORY_ITEM* inv_main_list[23];
extern INVENTORY_ITEM* inv_keys_list[23];
extern INVENTORY_ITEM* inv_option_list[5];
extern INVENTORY_ITEM* inv_levelselect_list[1];
extern short gLevelSecrets[21];
extern short LevelSecrets[21];
extern bool noAdditiveBG;
