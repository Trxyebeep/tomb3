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

#define Inv_AddItem	( (long(__cdecl*)(long)) 0x004378B0 )
#define Inv_RemoveItem	( (long(__cdecl*)(long)) 0x00438420 )
#define Inv_RequestItem	( (long(__cdecl*)(long)) 0x00438380 )
