#pragma once
#include "../global/types.h"

#if (DIRECT3D_VERSION < 0x900)
long DXTextureNewPalette(uchar* palette);
void DXResetPalette();
void DXReleasePalette();
LPDIRECT3DTEXTUREX DXTextureGetInterface(LPDIRECTDRAWSURFACEX surf);
bool DXCreateTextureSurface(TEXTURE* tex, LPDDPIXELFORMAT ddpf);
bool DXTextureMakeSystemSurface(DXTEXTURE* tex, LPDDPIXELFORMAT ddpf);
long DXTextureMakeDeviceSurface(long w, long h, LPDIRECTDRAWPALETTE palette, DXTEXTURE* list);
#endif

void DXTextureSetGreyScale(bool set);
long DXTextureFindTextureSlot(DXTEXTURE* tex);
void DXClearAllTextures(DXTEXTURE* list);
void DXTextureCleanup(long index, DXTEXTURE* list);
DXTEXTURE* DXRestoreSurfaceIfLost(long index, DXTEXTURE* list);
#if (DIRECT3D_VERSION < 0x900)
long DXTextureAddPal(long w, long h, uchar* src, DXTEXTURE* list, ulong flags);
#endif
long DXTextureAdd(long w, long h, uchar* src, DXTEXTURE* list, long bpp, ulong flags);
#if (DIRECT3D_VERSION < 0x900)
void DXCreateMaxTPages(long create);
void DXFreeTPages();
#endif

extern DXTEXTURE Textures[MAX_TPAGES];
extern DXTEXTURE* TexturePtrs[MAX_TPAGES];
extern long nTextures;
#if (DIRECT3D_VERSION < 0x900)
extern TEXTURE TextureSurfaces[MAX_TPAGES];
extern LPDIRECTDRAWPALETTE DXPalette;
#endif
