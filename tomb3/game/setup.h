#pragma once
#include "../global/types.h"

void GetAIPickups();
void GetCarriedItems();
void InitialiseLevelFlags();
void InitialiseGameFlags();
long InitialiseLevel(long level, long type);
void BuildOutsideTable();
void InitialiseObjects();

extern short IsRoomOutsideNo;
extern char* OutsideRoomTable;
extern short OutsideRoomOffsets[729];
extern BOSS_STRUCT bossdata;
