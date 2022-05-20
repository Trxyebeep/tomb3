#pragma once
#include "../global/vars.h"

void inject_draweffects(bool replace);

void LaraElectricDeath(long lr, ITEM_INFO* item);
void DrawExplosionRings();

#define S_DrawSparks	( (void(__cdecl*)()) 0x0047B2C0 )
#define S_DrawSplashes	( (void(__cdecl*)()) 0x0047BAA0 )
#define S_DrawBat	( (void(__cdecl*)()) 0x00476420 )
#define DoSnow	( (void(__cdecl*)()) 0x0047AA80 )
#define DoRain	( (void(__cdecl*)()) 0x0047A4B0 )
#define S_DrawLaserBeam	( (void(__cdecl*)(GAME_VECTOR*, GAME_VECTOR*, uchar, uchar, uchar)) 0x00479810 )
#define ClipLine	( (bool(__cdecl*)(long&, long&, long&, long&, long, long)) 0x00479510 )