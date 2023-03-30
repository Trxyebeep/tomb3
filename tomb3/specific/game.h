#pragma once
#include "../global/types.h"

long GetRandomControl();
void SeedRandomControl(long seed);
long GetRandomDraw();
void SeedRandomDraw(long seed);
long GameStats(long level_num, long type);
void SortOutAdventureSave(long world);
long Level2World(long level);
long World2Level(long world);
long LevelStats(long level);
void GetValidLevelsList(REQUEST_INFO* req);
void GetSavedGamesList(REQUEST_INFO* req);
void DisplayCredits();
long LevelCompleteSequence();
long S_FrontEndCheck(SAVEGAME_INFO* pData, long nBytes);
long S_LoadGame(LPVOID data, long size, long slot);
long S_SaveGame(LPVOID data, long size, long slot);
ulong mGetAngle(long x, long z, long x1, long z1);
long GameLoop(long demo_mode);
long StartGame(long level, long type);
