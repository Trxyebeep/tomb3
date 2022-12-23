#pragma once
#include "../global/vars.h"

void inject_savegame(bool replace);

void ModifyStartInfo(long level);
void InitialiseStartInfo();
void CreateStartInfo(long level);
void ResetSG();
void WriteSG(void* pointer, long size);
void ReadSG(void* pointer, long size);
void CreateSaveGameInfo();
void ExtractSaveGameInfo();
#ifdef TROYESTUFF
void save_tomb3_data();
void load_tomb3_data();
#endif