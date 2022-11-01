#pragma once
#include "../global/vars.h"

void inject_texture(bool replace);

long DXTextureNewPalette(uchar* palette);
void DXResetPalette(DXTEXTURE* tex);
void DXTextureSetGreyScale(bool set);
LPDIRECT3DTEXTUREX DXTextureGetInterface(LPDIRECTDRAWSURFACEX surf);
long DXTextureFindTextureSlot(DXTEXTURE* tex);
bool DXTextureMakeSystemSurface(DXTEXTURE* tex, LPDDPIXELFORMAT ddpf);
long DXTextureMakeDeviceSurface(long w, long h, LPDIRECTDRAWPALETTE palette, DXTEXTURE* list);
void DXClearAllTextures(DXTEXTURE* list);
bool DXCreateTextureSurface(TEXTURE* tex, LPDDPIXELFORMAT ddpf);
void DXTextureCleanup(long index, DXTEXTURE* list);
DXTEXTURE* DXRestoreSurfaceIfLost(long index, DXTEXTURE* list);

#define DXTextureAdd	( (long(__cdecl*)(long, long, ushort*, DXTEXTURE*, long, long)) 0x004B23D0 )
#define DXFreeTPages	( (void(__cdecl*)()) 0x004B1F10 )
#define DXCreateMaxTPages	( (void(__cdecl*)(long)) 0x004B1D90 )
