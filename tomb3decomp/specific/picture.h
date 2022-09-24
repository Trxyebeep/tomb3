#pragma once
#include "../global/vars.h"

void inject_picture(bool replace);

void CrossFadePicture();
void S_FadePicture();
void DrawTile(long x, long y, long w, long h, long tpage, long tU, long tV, long tW, long tH, long c0, long c1, long c2, long c3, float z);
void DrawPictureAlpha(long col, long* indices, float z);
void TRDrawPicture(long col, long* indices, float z);
void MemBlt(char* dest, long x, long y, long w, long h, long sz, char* source, long x2, long y2, DDSURFACEDESC desc);
void ConvertSurfaceToTextures16Bit(LPDIRECTDRAWSURFACE3 surf);
void ConvertSurfaceToTextures(LPDIRECTDRAWSURFACE3 surf);

#define FreePictureTextures	( (void(__cdecl*)(long*)) 0x0048B190 )
#define LoadPicture	( (bool(__cdecl*)(char*, LPDIRECTDRAWSURFACE3, long)) 0x0048B0D0 )
#define FadePictureUp	( (void(__cdecl*)(long)) 0x0048BC70 )
#define FadePictureDown	( (void(__cdecl*)(long)) 0x0048BD10 )
