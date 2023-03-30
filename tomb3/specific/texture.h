#pragma once
#include "../global/types.h"

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
long DXTextureAddPal(long w, long h, uchar* src, DXTEXTURE* list, ulong flags);
void MMXTextureCopy(ulong* dest, uchar* src, ulong step);
long DXTextureAdd(long w, long h, uchar* src, DXTEXTURE* list, long bpp, ulong flags);
void DXCreateMaxTPages(long create);
void DXFreeTPages();

extern DXTEXTURE* TPages[MAX_TPAGES];
extern long nTPages;
extern TEXTURE Textures[MAX_TPAGES];
extern LPDIRECTDRAWPALETTE DXPalette;
