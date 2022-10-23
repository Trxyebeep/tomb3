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
void CreateMonoScreen();
void SetPictureToFade(long fade);
void ForceFadeDown(long fade);
bool LoadPicture(const char* name, LPDIRECTDRAWSURFACE3 surf, long a);
void FadePictureUp(long steps);
void FadePictureDown(long steps);

#define FreePictureTextures	( (void(__cdecl*)(long*)) 0x0048B190 )
#define RemoveMonoScreen	( (void(__cdecl*)(long)) 0x0048C030 )
#define DoInventoryPicture	( (void(__cdecl*)()) 0x0048C070 )
#define DrawMonoScreen	( (void(__cdecl*)(long, long, long)) 0x0048C010 )
