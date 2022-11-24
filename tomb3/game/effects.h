#pragma once
#include "../global/vars.h"

void inject_effects(bool replace);

void LaraBreath(ITEM_INFO* item);
long ItemNearLara(PHD_3DPOS* pos, long rad);
void SoundEffects();
void Richochet(GAME_VECTOR* pos);
void CreateBubble(PHD_3DPOS* pos, short room_number, long size, long sizerange);
void LaraBubbles(ITEM_INFO* item);
void ControlBubble1(short fx_number);
void Splash(ITEM_INFO* item);
void WadeSplash(ITEM_INFO* item, long water, long depth);
void WaterFall(short item_number);
void finish_level_effect(ITEM_INFO* item);
void turn180_effect(ITEM_INFO* item);
void floor_shake_effect(ITEM_INFO* item);
void lara_normal_effect(ITEM_INFO* item);
void BoilerFX(ITEM_INFO* item);
void FloodFX(ITEM_INFO* item);
void RubbleFX(ITEM_INFO* item);
void ChandelierFX(ITEM_INFO* item);
void ExplosionFX(ITEM_INFO* item);
void PistonFX(ITEM_INFO* item);
void CurtainFX(ITEM_INFO* item);
void StatueFX(ITEM_INFO* item);
void SetChangeFX(ITEM_INFO* item);
void ControlDingDong(short item_number);
void ControlLaraAlarm(short item_number);
void ControlBirdTweeter(short item_number);
void DoChimeSound(ITEM_INFO* item);
void ControlClockChimes(short item_number);
void lara_hands_free(ITEM_INFO* item);
void flip_map_effect(ITEM_INFO* item);
void draw_right_gun(ITEM_INFO* item);
void draw_left_gun(ITEM_INFO* item);
void shoot_right_gun(ITEM_INFO* item);
void shoot_left_gun(ITEM_INFO* item);
void swap_meshes_with_meshswap1(ITEM_INFO* item);
void swap_meshes_with_meshswap2(ITEM_INFO* item);
void swap_meshes_with_meshswap3(ITEM_INFO* item);
void invisibility_on(ITEM_INFO* item);
void invisibility_off(ITEM_INFO* item);
void dynamic_light_on(ITEM_INFO* item);
void dynamic_light_off(ITEM_INFO* item);
void reset_hair(ITEM_INFO* item);
void AssaultStart(ITEM_INFO* item);
void AssaultStop(ITEM_INFO* item);
void AssaultReset(ITEM_INFO* item);
void AssaultPenalty4(ITEM_INFO* item);
void AssaultPenalty30(ITEM_INFO* item);
void AssaultFinished(ITEM_INFO* item);
void QuadStart(ITEM_INFO* item);
void QuadStop(ITEM_INFO* item);
void QuadFinished(ITEM_INFO* item);
void Exercise1_105(ITEM_INFO* item);
void Exercise1_104(ITEM_INFO* item);
void Exercise2_112(ITEM_INFO* item);
void Exercise2_83(ITEM_INFO* item);
void Exercise3_115(ITEM_INFO* item);
void Exercise3_107(ITEM_INFO* item);
void Exercise3_79(ITEM_INFO* item);

#define effect_routines (*(void(__cdecl*(*)[60])(ITEM_INFO*)) 0x004C5478)

#define DoBloodSplat	( (short(__cdecl*)(long, long, long, short, short, short)) 0x0042E2C0 )
#define DoLotsOfBlood	( (void(__cdecl*)(long, long, long, short, short, short, long)) 0x0042E3B0 )
#define DoLotsOfBloodD	( (void(__cdecl*)(long, long, long, short, short, short, long)) 0x0042E460 )
