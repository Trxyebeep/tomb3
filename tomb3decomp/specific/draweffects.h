#pragma once
#include "../global/vars.h"

void inject_draweffects(bool replace);

void LaraElectricDeath(long lr, ITEM_INFO* item);
bool ClipLine(long& x1, long& y1, long& x2, long& y2, long w, long h);
void S_DrawWakeFX(ITEM_INFO* item);
void DoRain();
void DoSnow();
void DrawExplosionRings();
void DrawSummonRings();
void DrawKnockBackRings();
void TriggerElectricBeam(ITEM_INFO* item, GAME_VECTOR* src, long copy);
void TriggerTribeBossHeadElectricity(ITEM_INFO* item, long copy);
void DrawTonyBossShield(ITEM_INFO* item);
void DrawTribeBossShield(ITEM_INFO* item);
void DrawLondonBossShield(ITEM_INFO* item);
void DrawWillBossShield(ITEM_INFO* item);
void S_DrawLaserBeam(GAME_VECTOR* src, GAME_VECTOR* dest, uchar cr, uchar cg, uchar cb);
void S_DrawBat();

#ifdef TROYESTUFF
void S_PrintSpriteShadow(short size, short* box, ITEM_INFO* item);
void S_DrawFootPrints();
void DoUwEffect();
void DrawBetterLasers(GAME_VECTOR* src, GAME_VECTOR* dest, uchar cr, uchar cg, uchar cb);
#endif

#define S_DrawSparks	( (void(__cdecl*)()) 0x0047B2C0 )
#define S_DrawSplashes	( (void(__cdecl*)()) 0x0047BAA0 )
