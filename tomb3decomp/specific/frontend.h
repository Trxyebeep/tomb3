#pragma once
#include "../global/vars.h"

void inject_frontend(bool replace);

ushort S_COLOUR(long r, long g, long b);
void S_DrawScreenLine(long x, long y, long z, long w, long h, long c, ushort* grdptr, ushort f);
void S_DrawScreenBox(long x, long y, long z, long w, long h, long sprnum, ushort* grdptr, ushort f);
void S_DrawScreenFBox(long x, long y, long z, long w, long h, long c, ushort* grdptr, ushort f);
void S_FinishInventory();
void S_Wait(long nFrames, long skip);
long S_PlayFMV(char* name);
long S_IntroFMV(char* name1, char* name2);
