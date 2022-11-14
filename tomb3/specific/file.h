#pragma once
#include "../global/vars.h"

void inject_file(bool replace);

long MyReadFile(HANDLE hFile, LPVOID lpBuffer, ulong nNumberOfBytesToRead, ulong* lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped);
bool LoadPalette(HANDLE file);
long LoadTexturePages(HANDLE file);
long LoadRooms(HANDLE file);
long LoadObjects(HANDLE file);
long LoadSprites(HANDLE file);
long LoadCameras(HANDLE file);
long LoadSoundEffects(HANDLE file);
long LoadBoxes(HANDLE file);
long LoadAnimatedTextures(HANDLE file);
long LoadItems(HANDLE file);
long LoadDepthQ(HANDLE file);
long LoadCinematic(HANDLE file);
long LoadDemo(HANDLE file);
long LoadSamples(HANDLE file);
void LoadDemFile(const char* name);
long LoadLevel(const char* name, long number);
void S_UnloadLevelFile();
long S_LoadLevelFile(char* name, long number, long type);
long FindCDDrive();
const char* GetFullPath(const char* name);
void build_ext(char* name, const char* ext);
void AdjustTextureUVs(bool reset);
long Read_Strings(long num, char** strings, char** buffer, ulong* read, HANDLE file);
long S_LoadGameFlow(const char* name);
