#pragma once
#include "../global/vars.h"

void inject_draweffects(bool replace);

void LaraElectricDeath(long lr, ITEM_INFO* item);
void DrawExplosionRings();
bool ClipLine(long& x1, long& y1, long& x2, long& y2, long w, long h);
void S_DrawWakeFX(ITEM_INFO* item);
void DrawTonyBossShield(ITEM_INFO* item);
void TriggerElectricBeam(ITEM_INFO* item, GAME_VECTOR* src, long copy);
void TriggerTribeBossHeadElectricity(ITEM_INFO* item, long copy);
void DrawTribeBossShield(ITEM_INFO* item);
void DoRain();
void DoSnow();
void DrawLondonBossShield(ITEM_INFO* item);

#define S_DrawSparks	( (void(__cdecl*)()) 0x0047B2C0 )
#define S_DrawSplashes	( (void(__cdecl*)()) 0x0047BAA0 )
#define S_DrawBat	( (void(__cdecl*)()) 0x00476420 )
#define S_DrawLaserBeam	( (void(__cdecl*)(GAME_VECTOR*, GAME_VECTOR*, uchar, uchar, uchar)) 0x00479810 )
