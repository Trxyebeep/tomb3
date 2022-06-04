#pragma once
#include "../global/vars.h"

void inject_box(bool replace);

void AlertNearbyGuards(ITEM_INFO* item);

#define CreatureActive	( (long(__cdecl*)(short)) 0x00414330 )
#define	CreatureTilt	( (void(__cdecl*)(ITEM_INFO*, short)) 0x004164D0 )
#define CreatureJoint	( (void(__cdecl*)(ITEM_INFO*, short, short)) 0x00416510 )
#define CreatureAnimation	( (void(__cdecl*)(short, short, short)) 0x00415940 )
#define GetAITarget	( (void(__cdecl*)(CREATURE_INFO*)) 0x00416B60 )
#define CreatureAIInfo	( (void(__cdecl*)(ITEM_INFO*, AI_INFO*)) 0x00414390 )
#define GetCreatureMood	( (void(__cdecl*)(ITEM_INFO*, AI_INFO*, long)) 0x00414C10 )
#define CreatureMood	( (void(__cdecl*)(ITEM_INFO*, AI_INFO*, long)) 0x00414E50 )
#define CreatureTurn	( (short(__cdecl*)(ITEM_INFO*, short)) 0x00416400 )
#define AlertAllGuards	( (void(__cdecl*)(short)) 0x004169C0 )
#define AIGuard	( (short(__cdecl*)(CREATURE_INFO*)) 0x00416AC0 )
#define InitialiseCreature	( (void(__cdecl*)(short)) 0x004142E0 )
#define CreatureVault	( (long(__cdecl*)(short, short, long, long)) 0x004166D0 )
#define CreatureEffect	( (void(__cdecl*)(ITEM_INFO*, BITE_INFO*, void(__cdecl*)(long, long, long, short, short, short))) 0x00416670 )
#define CreatureDie	( (void(__cdecl*)(short, long)) 0x00415820 )
