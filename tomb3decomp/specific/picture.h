#pragma once
#include "../global/vars.h"

void inject_picture(bool replace);

void CrossFadePicture();
void S_FadePicture();
void DrawTile(long x, long y, long w, long h, long tpage, long tU, long tV, long tW, long tH, long c0, long c1, long c2, long c3, float z);
void DrawPictureAlpha(long col, long* indices, float z);

#define FreePictureTextures	( (void(__cdecl*)(long*)) 0x0048B190 )
#define TRDrawPicture	( (void(__cdecl*)(long, long*, float)) 0x0048BA30 )
