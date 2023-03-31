#pragma once
#include "../global/types.h"

void CrossFadePicture();
void S_FadePicture();
void DrawTile(long x, long y, long w, long h, long tpage, long tU, long tV, long tW, long tH, long c0, long c1, long c2, long c3, float z);
void DrawPictureAlpha(long col, long* indices, float z);
void TRDrawPicture(long col, long* indices, float z);
void MemBlt(char* dest, long x, long y, long w, long h, long sz, char* source, long x2, long y2, DDSURFACEDESCX desc);
void ConvertSurfaceToTextures16Bit(LPDIRECTDRAWSURFACEX surf);
void ConvertSurfaceToTextures(LPDIRECTDRAWSURFACEX surf);
void CreateMonoScreen();
void SetPictureToFade(long fade);
void ForceFadeDown(long fade);
bool LoadPicture(const char* name, LPDIRECTDRAWSURFACEX surf, long a);
void FadePictureUp(long steps);
void FadePictureDown(long steps);
void S_FadeToBlack();
void DrawMonoScreen(long r, long g, long b);
void RemoveMonoScreen(long fade);
void DoInventoryPicture();
void FreePictureTextures(long* indices);

extern DXTEXTURE PictureTextures[MAX_TPAGES];
extern long OldPicTexIndices[5];
extern long CurPicTexIndices[5];
extern long nLoadedPictures;
extern bool forceFadeDown;
extern bool pictureFading;
extern bool dontFadePicture;
extern bool bDontGreyOut;
