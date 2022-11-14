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
	for (int i = 0; i < MAX_TPAGES; i++)
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
	for (int i = 0; i < MAX_TPAGES; i++)
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
			tex->pSurf->SetColorKey(DDCKEY_SRCBLT, &ckey);
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

void MMXTextureCopy(ulong* dest, uchar* src, ulong step)
{
	ulong* pSrc;
	long add;

	add = step << 10;

	for (int i = 0; i < 256; i += step)
	{
		pSrc = (ulong*)src;

		for (int j = 0; j < 256; j += step)
		{
			*dest++ = *pSrc;
			pSrc += step;
		}

		src += add;
	}
}

long DXTextureAdd(long w, long h, uchar* src, DXTEXTURE* list, long bpp, ulong flags)
{
	DXTEXTURE* tex;
	D3DTEXTUREINFO* tinfo;
	DDCOLORKEY ckey;
	DDSURFACEDESCX desc;
	uchar* udest;
	char* dest;
	ulong col;
	long oldTF, index, lw, a, r, g, b;

	if (App.DeviceInfoPtr->DDInfo[App.DXConfigPtr->nDD].D3DInfo[App.DXConfigPtr->nD3D].Texture[App.DXConfigPtr->D3DTF].bPalette)
	{
		oldTF = App.DXConfigPtr->D3DTF;
		App.DXConfigPtr->D3DTF = 0;
	}
	else
		oldTF = -1;

	index = DXTextureMakeDeviceSurface(w, h, 0, list);

	if (index < 0)
		return -1;

	tex = &list[index];
	tex->dwFlags |= flags;

	if (FAILED(DD_LockSurface(tex->pSystemSurface, desc, DDLOCK_WAIT | DDLOCK_WRITEONLY)))
		return -1;

	dest = (char*)desc.lpSurface;

	while (h)
	{
		udest = (uchar*)dest;
		lw = w;

		while (lw)
		{
			if (bpp == 8888)	//get full color
			{
				col = *(ulong*)src;
				src += 4;
			}
			else if (bpp == 888)
			{
				col = *(ulong*)src & 0xFFFFFF;
				src += 3;
			}
			else
			{
				col = *(ushort*)src;
				src += 2;
			}

			switch (bpp)	//extract RGBA
			{
			case 8888:
				a = 0;
				r = RGBA_GETRED(col);
				g = RGBA_GETGREEN(col);
				b = RGBA_GETBLUE(col);

				if (r && g && b)
					a = 255;

				break;

			case 888:
				a = 0;
				r = RGBA_GETRED(col);
				g = RGBA_GETGREEN(col);
				b = RGBA_GETBLUE(col);

				if (r && g && b)
					a = 255;

				break;

			case 565:
				r = (col >> 8) & 0xF8;
				g = (col >> 3) & 0xF8;
				b = (col << 3) & 0xF8;
				a = 0;
				break;

			case 555:
				r = (col >> 7) & 0xF8;
				g = (col >> 2) & 0xF8;
				b = (col << 3) & 0xF8;
				a = (short)col >> 15;
				break;
			}

			if (flags == 16)
				a = 255;

			if (bMakeGrey)
			{
				r = b;
				g = b;
			}

			tinfo = &App.DeviceInfoPtr->DDInfo[App.DXConfigPtr->nDD].D3DInfo[App.DXConfigPtr->nD3D].Texture[App.DXConfigPtr->D3DTF];

			if (tinfo->bAlpha || App.DXConfigPtr->MMX)
			{
				col =
					(r >> (8 - tinfo->rbpp) << tinfo->rshift) |
					(g >> (8 - tinfo->gbpp) << tinfo->gshift) |
					(b >> (8 - tinfo->bbpp) << tinfo->bshift) |
					(a >> (8 - tinfo->abpp) << tinfo->ashift);
			}
			else
			{
				col =
					(r >> (8 - tinfo->rbpp) << tinfo->rshift) |
					(g >> (8 - tinfo->gbpp) << tinfo->gshift) |
					(b >> (8 - tinfo->bbpp) << tinfo->bshift);
			}

			for (ulong i = 0; i < tinfo->bpp; i += 8)
			{
				*udest++ = (uchar)col;
				col >>= 8;
			}

			lw--;
		}

		dest += desc.lPitch;
		h--;
	}

	if (!App.DeviceInfoPtr->DDInfo[App.DXConfigPtr->nDD].D3DInfo[App.DXConfigPtr->nD3D].bHardware)
	{
		if (App.DXConfig.MMX)
		{
			tex->pData = (ulong*)GLOBALALLOC(GMEM_FIXED, 0x55400);
			memcpy(tex->pData, desc.lpSurface, 0x40000);
			MMXTextureCopy(tex->pData + 0x10000, (uchar*)desc.lpSurface, 2);
			MMXTextureCopy(tex->pData + 0x14000, (uchar*)desc.lpSurface, 4);
			MMXTextureCopy(tex->pData + 0x15000, (uchar*)desc.lpSurface, 8);
			MMXTextureCopy(tex->pData + 0x15400, (uchar*)desc.lpSurface, 16);
		}
		else
		{
			tex->pData = (ulong*)GLOBALALLOC(GMEM_FIXED, 0x20000);
			memcpy(tex->pData, desc.lpSurface, 0x20000);
		}
	}

	if (flags == 8 && !App.DeviceInfoPtr->DDInfo[App.DXConfigPtr->nDD].D3DInfo[App.DXConfigPtr->nD3D].Texture[App.DXConfigPtr->D3DTF].bAlpha)
	{
		ckey.dwColorSpaceLowValue = 0;
		ckey.dwColorSpaceHighValue = 0;
		tex->pSystemSurface->SetColorKey(DDCKEY_SRCBLT, &ckey);
	}

	DD_UnlockSurface(tex->pSystemSurface, desc);

	if (!App.DeviceInfoPtr->DDInfo[App.DXConfigPtr->nDD].D3DInfo[App.DXConfigPtr->nD3D].bHardware)
	{
		tex->pSystemSurface->Release();
		tex->pSystemSurface = 0;
	}

	if (oldTF != -1)
		App.DXConfigPtr->D3DTF = oldTF;

	return index;
}

void DXCreateMaxTPages(long create)
{
	DIRECT3DINFO* d3dinfo;
	TEXTURE* tex;
	LPDDPIXELFORMAT ddpf, ddpf2;
	long n, oldTF;

	d3dinfo = &App.DeviceInfoPtr->DDInfo[App.DXConfigPtr->nDD].D3DInfo[App.DXConfigPtr->nD3D];

	if (!d3dinfo->bHardware)
		return;

	ddpf = &d3dinfo->Texture[App.DXConfigPtr->D3DTF].ddsd.ddpfPixelFormat;
	DXCreateTextureSurface(Textures, ddpf);
	n = 1;

	if (create && ddpf->dwRGBBitCount == 8)
	{
		bSetColorKey = 0;
		oldTF = App.DXConfigPtr->D3DTF;
		App.DXConfigPtr->D3DTF = 0;
		ddpf2 = &d3dinfo->Texture[App.DXConfigPtr->D3DTF].ddsd.ddpfPixelFormat;

		for (; n < 6; n++)
		{
			tex = &Textures[n];

			if (!DXCreateTextureSurface(tex, ddpf2))
				break;
		}

		App.DXConfigPtr->D3DTF = oldTF;
		bSetColorKey = 1;
		ddpf = &d3dinfo->Texture[App.DXConfigPtr->D3DTF].ddsd.ddpfPixelFormat;
	}

	for (; n < MAX_TPAGES; n++)
	{
		tex = &Textures[n];

		if (!DXCreateTextureSurface(tex, ddpf))
			break;
	}

	nTPages = n;
}

void DXFreeTPages()
{
	TEXTURE* tex;
	DXTEXTURE* DXTex;

	if (!App.DeviceInfoPtr->DDInfo[App.DXConfigPtr->nDD].D3DInfo[App.DXConfigPtr->nD3D].bHardware)
		return;

	for (int i = 0; i < MAX_TPAGES; i++)
	{
		tex = &Textures[i];
		DXTex = (DXTEXTURE*)tex->DXTex;

		if (DXTex)
			DXTex->tex = 0;

		if (tex->pTexture)
		{
			tex->pTexture->Release();
			tex->pTexture = 0;
		}

		if (tex->pSurf)
		{
			tex->pSurf->Release();
			tex->pSurf = 0;
		}

		if (tex->pPalette)
		{
			tex->pPalette->Release();
			tex->pPalette = 0;
		}

		memset(tex, 0, sizeof(TEXTURE));
	}

	nTPages = 0;
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
	INJECT(0x004B2370, MMXTextureCopy, replace);
	INJECT(0x004B23D0, DXTextureAdd, replace);
	INJECT(0x004B1D90, DXCreateMaxTPages, replace);
	INJECT(0x004B1F10, DXFreeTPages, replace);
}
