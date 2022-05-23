#pragma once
#include "../global/vars.h"

void inject_picture(bool replace);

void CrossFadePicture();

#define S_FadePicture	( (void(__cdecl*)()) 0x0048C0A0 )
#define DrawPictureAlpha	( (void(__cdecl*)(long, long*)) 0x0048ADA0 )
#define FreePictureTextures	( (void(__cdecl*)(long*)) 0x0048B190 )
#define TRDrawPicture	( (void(__cdecl*)(long, long*)) 0x0048BA30 )
