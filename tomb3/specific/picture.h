#pragma once
#include "../global/types.h"

void SetPictureToFade(long fade);
void ForceFadeDown(long fade);
#if (DIRECT3D_VERSION < 0x900)
void DoInventoryPicture();
void FadePictureUp(long steps);
void FadePictureDown(long steps);
void CrossFadePicture();
void S_FadePicture();
void S_FadeToBlack();
bool LoadPicture(const char* name, LPDIRECTDRAWSURFACEX surf);
void FreePictureTextures(long* indices);
void CreateMonoScreen();
void DrawMonoScreen(long r, long g, long b);
void RemoveMonoScreen(long fade);
void ConvertSurfaceToTextures(LPDIRECTDRAWSURFACEX surf);
void DrawTile(long x, long y, long w, long h, long tpage, long tU, long tV, long tW, long tH, long c0, long c1, long c2, long c3, float z);
void DrawPictureAlpha(long col, long* indices, float z);
void TRDrawPicture(long col, long* indices, float z);
#endif

extern long OldPicTexIndices[5];
extern long CurPicTexIndices[5];
extern long nLoadedPictures;
extern bool forceFadeDown;
extern bool pictureFading;
extern bool dontFadePicture;
extern bool bDontGreyOut;
