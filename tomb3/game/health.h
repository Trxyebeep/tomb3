#pragma once
#include "../global/types.h"

void inject_health(bool replace);

long FlashIt();
void DrawAssaultTimer();
void DrawAssaultPenalties(long type);
void DrawQuadbikeLapTime();
void DrawHealthBar(long flash_state);
void DrawAirBar(long flash_state);
void RemoveAmmoText();
void DrawModeInfo();
void DisplayModeInfo(char* string);
void DrawAmmoInfo();
void DrawGameInfo(long timed);
void InitialisePickUpDisplay();
void AddDisplayPickup(short objnum);

extern long health_bar_timer;
extern TEXTSTRING* ammotext;
extern DISPLAYPU pickups[];
#ifdef TROYESTUFF
extern short PickupX, PickupY, PickupVel, CurrentPickup;
#endif
