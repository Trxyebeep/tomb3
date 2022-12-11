#pragma once
#include "../global/vars.h"

void inject_effect2(bool replace);

void TriggerDynamic(long x, long y, long z, long falloff, long r, long g, long b);
void ClearDynamics();
void KillEverything();
void TriggerBreath(long x, long y, long z, long xv, long yv, long zv);
void TriggerAlertLight(long x, long y, long z, long r, long g, long b, short angle, short rn);
void TriggerFireFlame(long x, long y, long z, long body_part, long type);
void TriggerFireSmoke(long x, long y, long z, long body_part, long type);
void TriggerStaticFlame(long x, long y, long z, long size);
void TriggerSideFlame(long x, long y, long z, long angle, long speed, long pilot);
void TriggerRocketSmoke(long x, long y, long z, long c);
void TriggerRicochetSpark(GAME_VECTOR* pos, long angle, long size);
void TriggerBlood(long x, long y, long z, long angle, long num);
void TriggerBloodD(long x, long y, long z, long angle, long num);
void TriggerUnderwaterBlood(long x, long y, long z, long size);
void TriggerUnderwaterBloodD(long x, long y, long z, long size);
void TriggerFlareSparks(long x, long y, long z, long xv, long yv, long zv, long smoke, long unused);
void TriggerGunSmoke(long x, long y, long z, long xv, long yv, long zv, long initial, long weapon, long shade);
void TriggerExplosionSparks(long x, long y, long z, long extras, long dynamic, long uw, short room_number);
void TriggerExplosionSmoke(long x, long y, long z, long uw);
void TriggerExplosionSmokeEnd(long x, long y, long z, long uw);
void TriggerShotgunSparks(long x, long y, long z, long xv, long yv, long zv);
void TriggerRocketFlame(long x, long y, long z, long xv, long yv, long zv, long itemNum);
void TriggerWaterfallMist(long x, long y, long z, long ang);
void TriggerDartSmoke(long x, long y, long z, long xv, long zv, long hit);
void TriggerExplosionBubble(long x, long y, long z, short room_number);
void TriggerBubble(long x, long y, long z, long size, long sizerange, short fxNum);
void ControlSmokeEmitter(short item_number);
void DetatchSpark(long num, long type);
long GetFreeSpark();
void InitialiseSparks();
void UpdateSparks();
void TriggerGunShell(short lr, long objNum, long weapon);
void ControlGunShell(short fx_number);

#define UpdateSplashes	( (void(__cdecl*)()) 0x0042CED0 )
#define UpdateBats	( (void(__cdecl*)()) 0x0042D3D0 )
#define SetupRipple	( (RIPPLE_STRUCT*(__cdecl*)(long, long, long, long, long)) 0x0042D080 )
#define SetupSplash	( (void(__cdecl*)(SPLASH_SETUP*)) 0x0042CAC0 )
