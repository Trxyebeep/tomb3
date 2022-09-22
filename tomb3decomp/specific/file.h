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
