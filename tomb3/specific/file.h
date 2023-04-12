#pragma once
#include "../global/types.h"

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
const char* GetFullPath(const char* name);
void build_ext(char* name, const char* ext);
void AdjustTextureUVs(bool reset);
long Read_Strings(long num, char** strings, char** buffer, ulong* read, HANDLE file);
long S_LoadGameFlow(const char* name);

extern CHANGE_STRUCT* changes;
extern RANGE_STRUCT* ranges;
extern short* aranges;
extern short* frames;
extern short* commands;
extern short* floor_data;
extern short* mesh_base;
extern long number_cameras;
extern long nTInfos;

extern PHDTEXTURESTRUCT phdtextinfo[MAX_TINFOS];
extern PHDSPRITESTRUCT phdspriteinfo[512];
extern uchar G_GouraudPalette[1024];
