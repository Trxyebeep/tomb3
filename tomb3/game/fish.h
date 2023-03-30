#pragma once
#include "../global/types.h"

void SetupShoal(long shoal_number);
void SetupFish(long leader, ITEM_INFO* item);
void ControlFish(short item_number);

extern FISH_INFO fish[200];
extern LEADER_INFO lead_info[8];
extern short CarcassItem;;
