#pragma once
#include "../global/vars.h"

void inject_box(bool replace);

void AlertNearbyGuards(ITEM_INFO* item);
void InitialiseCreature(short item_number);
long CreatureActive(short item_number);
void CreatureAIInfo(ITEM_INFO* item, AI_INFO* info);
long SearchLOT(LOT_INFO* LOT, long expansion);
long UpdateLOT(LOT_INFO* LOT, long expansion);
void TargetBox(LOT_INFO* LOT, short box_number);
long EscapeBox(ITEM_INFO* item, ITEM_INFO* enemy, short box_number);
long ValidBox(ITEM_INFO* item, short zone_number, short box_number);
long StalkBox(ITEM_INFO* item, ITEM_INFO* enemy, short box_number);
target_type CalculateTarget(PHD_VECTOR* target, ITEM_INFO* item, LOT_INFO* LOT);
void CreatureMood(ITEM_INFO* item, AI_INFO* info, long violent);
void GetCreatureMood(ITEM_INFO* item, AI_INFO* info, long violent);
long BadFloor(long x, long y, long z, long box_height, long next_height, short room_number, LOT_INFO* LOT);
long CreatureCreature(short item_number);
void CreatureDie(short item_number, long explode);
short CreatureTurn(ITEM_INFO* item, short maximum_turn);

#define	CreatureTilt	( (void(__cdecl*)(ITEM_INFO*, short)) 0x004164D0 )
#define CreatureJoint	( (void(__cdecl*)(ITEM_INFO*, short, short)) 0x00416510 )
#define CreatureAnimation	( (void(__cdecl*)(short, short, short)) 0x00415940 )
#define GetAITarget	( (void(__cdecl*)(CREATURE_INFO*)) 0x00416B60 )
#define AlertAllGuards	( (void(__cdecl*)(short)) 0x004169C0 )
#define AIGuard	( (short(__cdecl*)(CREATURE_INFO*)) 0x00416AC0 )
#define CreatureVault	( (long(__cdecl*)(short, short, long, long)) 0x004166D0 )
#define CreatureEffect	( (void(__cdecl*)(ITEM_INFO*, BITE_INFO*, short(__cdecl*)(long, long, long, short, short, short))) 0x00416670 )
#define CreatureFloat	( (void(__cdecl*)(short)) 0x00416570 )
#define CreatureUnderwater	( (void(__cdecl*)(ITEM_INFO*, long)) 0x00416620 )
#define CreatureKill	( (void(__cdecl*)(ITEM_INFO*, long, long, long)) 0x00416840 )
#define AdjustStopperFlag	( (void(__cdecl*)(ITEM_INFO*, long, long)) 0x00417210 )
