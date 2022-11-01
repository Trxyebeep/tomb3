#include "../tomb3/pch.h"
#include "texture.h"
#include "dd.h"

#define bSetColorKey	VAR_(0x004CEEC4, bool)
#define bMakeGrey	VAR_(0x006CED60, bool)

long DXTextureNewPalette(uchar* palette)
{
	ulong data[256];

	if (DXPalette)
	{
		DXPalette->Release();
		DXPalette = 0;
	}

	for (int i = 0; i < 256; i++, palette += 3)
		data[i] = RGB(palette[0], palette[1], palette[2]);

	return App.lpDD->CreatePalette(DDPCAPS_8BIT | DDPCAPS_ALLOW256, (LPPALETTEENTRY)data, &DXPalette, 0);
}

void DXResetPalette(DXTEXTURE* tex)
{
	DXPalette = 0;
	bSetColorKey = 1;
}

void DXTextureSetGreyScale(bool set)
{
	bMakeGrey = set;
}

LPDIRECT3DTEXTUREX DXTextureGetInterface(LPDIRECTDRAWSURFACEX surf)
{
	LPDIRECT3DTEXTUREX tex;

	if (SUCCEEDED(surf->QueryInterface(D3DTEXGUID, (LPVOID*)&tex)))
		return tex;
	
	return 0;
}

long DXTextureFindTextureSlot(DXTEXTURE* tex)
{
	for (int i = 0; i < 32; i++)
	{
		if (!(tex[i].dwFlags & 1))
			return i;
	}

	return -1;
}

bool DXTextureMakeSystemSurface(DXTEXTURE* tex, LPDDPIXELFORMAT ddpf)
{
	DDSURFACEDESCX desc;

	memset(&desc, 0, sizeof(DDSURFACEDESCX));
	desc.dwSize = sizeof(DDSURFACEDESCX);
	desc.dwHeight = tex->nHeight;
	desc.dwWidth = tex->nWidth;
	desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
	desc.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY | DDSCAPS_TEXTURE;
	memcpy(&desc.ddpfPixelFormat, ddpf, sizeof(DDPIXELFORMAT));

	if (FAILED(DD_CreateSurface(desc, tex->pSystemSurface)))
		return 0;

	return !tex->pPalette || SUCCEEDED(tex->pSystemSurface->SetPalette(tex->pPalette));
}

long DXTextureMakeDeviceSurface(long w, long h, LPDIRECTDRAWPALETTE palette, DXTEXTURE* list)
{
	DXTEXTURE* tex;
	LPDDPIXELFORMAT ddpf;
	long index;

	index = DXTextureFindTextureSlot(list);

	if (index < 0)
		return -1;

	tex = &list[index];
	memset(tex, 0, sizeof(DXTEXTURE));
	tex->nWidth = w;
	tex->nHeight = h;
	tex->dwFlags = 1;
	tex->pPalette = palette;
	ddpf = &App.DeviceInfoPtr->DDInfo[App.DXConfigPtr->nDD].D3DInfo[App.DXConfigPtr->nD3D].Texture[App.DXConfigPtr->D3DTF].ddsd.ddpfPixelFormat;
	tex->bpp = ddpf->dwRGBBitCount;

	if (DXTextureMakeSystemSurface(tex, ddpf))
		return index;

	return -1;
}

void DXClearAllTextures(DXTEXTURE* list)
{
	for (int i = 0; i < 32; i++)
		DXTextureCleanup(i, list);

	if (DXPalette)
	{
		DXPalette->Release();
		DXPalette = 0;
	}
}

bool DXCreateTextureSurface(TEXTURE* tex, LPDDPIXELFORMAT ddpf)
{
	DDSURFACEDESC desc;
	DDCOLORKEY ckey;

	memset(&desc, 0, sizeof(DDSURFACEDESC));
	desc.dwSize = sizeof(DDSURFACEDESC);
	desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
	desc.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_VIDEOMEMORY;

	if (App.DeviceInfoPtr->DDInfo[App.DXConfigPtr->nDD].D3DInfo[App.DXConfigPtr->nD3D].bAGP)
		desc.ddsCaps.dwCaps |= DDSCAPS_NONLOCALVIDMEM;

	desc.dwWidth = 256;
	desc.dwHeight = 256;
	memcpy(&desc.ddpfPixelFormat, ddpf, sizeof(DDPIXELFORMAT));

	if (FAILED(DD_CreateSurface(desc, tex->pSurf)))
	{
		tex->num = 4;
		return 0;
	}

	if (desc.ddpfPixelFormat.dwRGBBitCount == 8 && DXPalette)
	{
		tex->pSurf->SetPalette(DXPalette);
		tex->pPalette = DXPalette;
		DXPalette->AddRef();
	}

	if (bSetColorKey)
	{
		if (!App.DeviceInfoPtr->DDInfo[App.DXConfigPtr->nDD].D3DInfo[App.DXConfigPtr->nD3D].Texture[App.DXConfigPtr->D3DTF].bAlpha)
		{
			ckey.dwColorSpaceLowValue = 0;
			ckey.dwColorSpaceHighValue = 0;
			tex->pSurf->SetColorKey(8, &ckey);
		}
	}

	tex->pTexture = DXTextureGetInterface(tex->pSurf);
	tex->pTexture->GetHandle(App.lpD3DDevice, &tex->handle);
	tex->num = 2;
	tex->DXTex = 0;
	tex->bpp = desc.ddpfPixelFormat.dwRGBBitCount;
	return 1;
}

void DXTextureCleanup(long index, DXTEXTURE* list)
{
	DXTEXTURE* tex;

	tex = &list[index];

	if (tex->pPalette)
	{
		tex->pPalette->Release();
		tex->pPalette = 0;
	}

	while (tex->pSystemSurface)
	{
		if (!tex->pSystemSurface->Release())
			tex->pSystemSurface = 0;
	}

	if (tex->tex)
	{
		tex->tex->nFrames = 0;
		tex->tex->num = 0;
		tex->tex->DXTex = 0;
		tex->tex = 0;
	}

	if (tex->pData)
	{
		GLOBALFREE(tex->pData);
		tex->pData = 0;
	}

	tex->dwFlags = 0;
	tex->nWidth = 0;
	tex->nHeight = 0;
}

DXTEXTURE* DXRestoreSurfaceIfLost(long index, DXTEXTURE* list)
{
	DXTEXTURE* tex;

	tex = &list[index];

	if (tex->tex && tex->tex->pSurf)
	{
		if (tex->tex->pSurf->IsLost() == DDERR_SURFACELOST)
		{
			tex->tex->pSurf->Restore();
			tex->tex->DXTex = 0;
			tex->tex = 0;
		}
	}

	return &list[index];
}

long DXTextureAddPal(long w, long h, uchar* src, DXTEXTURE* list, ulong flags)
{
	DXTEXTURE* tex;
	DDSURFACEDESCX desc;
	char* dest;
	long index;

	index = DXTextureMakeDeviceSurface(w, h, DXPalette, list);

	if (index < 0)
		return -1;

	tex = &list[index];
	tex->dwFlags |= flags;

	if (FAILED(DD_LockSurface(tex->pSystemSurface, desc, DDLOCK_WAIT | DDLOCK_WRITEONLY)))
		return -1;

	dest = (char*)desc.lpSurface;

	while (h)
	{
		memcpy(dest, src, w);
		src += w;
		dest += desc.lPitch;
		h--;
	}

	DD_UnlockSurface(tex->pSystemSurface, desc);
	return index;
}

void inject_texture(bool replace)
{
	INJECT(0x004B1B80, DXTextureNewPalette, replace);
	INJECT(0x004B1FB0, DXResetPalette, replace);
	INJECT(0x004B1B70, DXTextureSetGreyScale, replace);
	INJECT(0x004B1FD0, DXTextureGetInterface, replace);
	INJECT(0x004B2000, DXTextureFindTextureSlot, replace);
	INJECT(0x004B2020, DXTextureMakeSystemSurface, replace);
	INJECT(0x004B20C0, DXTextureMakeDeviceSurface, replace);
	INJECT(0x004B21F0, DXClearAllTextures, replace);
	INJECT(0x004B1BF0, DXCreateTextureSurface, replace);
	INJECT(0x004B2180, DXTextureCleanup, replace);
	INJECT(0x004B2230, DXRestoreSurfaceIfLost, replace);
	INJECT(0x004B2280, DXTextureAddPal, replace);
}
