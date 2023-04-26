#pragma once
#if (DIRECT3D_VERSION >= 0x900)
#include "../global/types.h"

/*mono*/
void CreateMonoScreen();
void DrawMonoScreen(long r, long g, long b);
void RemoveMonoScreen(long fade);

/*picture*/
void LoadPicture(const char* name);
void FreePictureTextures(long* indices);
void FreePictureTextures();
void DrawPicture(long col, long* indices, float z);
void FadePictureUp(long steps);
void FadePictureDown(long steps);
void S_FadePicture();
void S_FadeToBlack();
void DoInventoryPicture();

/*shared*/
void MemBlt(char* dest, long x, long y, long w, long h, long sz, char* source, long x2, long y2, LPDDSURFACEDESCX desc);
void DrawTile(long x, long y, long w, long h, long tpage, long tU, long tV, long tW, long tH, long c0, long c1, long c2, long c3, float z);

extern long bMonoScreen;
#endif
