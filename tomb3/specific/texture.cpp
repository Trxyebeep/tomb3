#include "../tomb3/pch.h"
#include "texture.h"
#include "dd.h"
#include "winmain.h"

DXTEXTURE Textures[MAX_TPAGES];
DXTEXTURE* TexturePtrs[MAX_TPAGES];
long nTextures;
#if (DIRECT3D_VERSION < 0x900)
TEXTURE TextureSurfaces[MAX_TPAGES];
LPDIRECTDRAWPALETTE DXPalette;
#endif

static bool bSetColorKey = 1;
static bool bMakeGrey;

#if (DIRECT3D_VERSION < 0x900)
long DXTextureNewPalette(uchar* palette)
{
	ulong data[256];

	DXReleasePalette();

	for (int i = 0; i < 256; i++, palette += 3)
		data[i] = RGB(palette[0], palette[1], palette[2]);

	return App.DDraw->CreatePalette(DDPCAPS_8BIT | DDPCAPS_ALLOW256, (LPPALETTEENTRY)data, &DXPalette, 0);
}

void DXResetPalette()
{
	DXPalette = 0;
	bSetColorKey = 1;
}

void DXReleasePalette()
{
	if (DXPalette)
	{
		DXPalette->Release();
		DXPalette = 0;
	}
}

LPDIRECT3DTEXTUREX DXTextureGetInterface(LPDIRECTDRAWSURFACEX surf)
{
	LPDIRECT3DTEXTUREX tex;

	if (SUCCEEDED(surf->QueryInterface(D3DTEXGUID, (LPVOID*)&tex)))
		return tex;
	
	return 0;
}

bool DXCreateTextureSurface(TEXTURE* tex, LPDDPIXELFORMAT ddpf)
{
	DDSURFACEDESC desc;
	DDCOLORKEY ckey;

	memset(&desc, 0, sizeof(DDSURFACEDESC));
	desc.dwSize = sizeof(DDSURFACEDESC);
	desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
	desc.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_VIDEOMEMORY;

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
		if (!App.lpDeviceInfo->DDInfo[App.lpDXConfig->nDD].D3DInfo[App.lpDXConfig->nD3D].Texture[App.lpDXConfig->D3DTF].bAlpha)
		{
			ckey.dwColorSpaceLowValue = 0;
			ckey.dwColorSpaceHighValue = 0;
			tex->pSurf->SetColorKey(DDCKEY_SRCBLT, &ckey);
		}
	}

	tex->pTexture = DXTextureGetInterface(tex->pSurf);
	tex->pTexture->GetHandle(App.D3DDev, &tex->handle);
	tex->num = 2;
	tex->DXTex = 0;
	tex->bpp = desc.ddpfPixelFormat.dwRGBBitCount;
	return 1;
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
	tex->dwFlags = TF_USED;
	tex->pPalette = palette;
	ddpf = &App.lpDeviceInfo->DDInfo[App.lpDXConfig->nDD].D3DInfo[App.lpDXConfig->nD3D].Texture[App.lpDXConfig->D3DTF].ddsd.ddpfPixelFormat;
	tex->bpp = ddpf->dwRGBBitCount;

	if (DXTextureMakeSystemSurface(tex, ddpf))
		return index;

	return -1;
}
#else
long DXTextureMakeDeviceSurface(long w, long h, DXTEXTURE* list)
{
	DXTEXTURE* tex;
	long index;

	index = DXTextureFindTextureSlot(list);

	if (index < 0)
		return -1;

	tex = &list[index];
	memset(tex, 0, sizeof(DXTEXTURE));
	tex->nWidth = w;
	tex->nHeight = h;
	tex->dwFlags = TF_USED;

	if FAILED(App.D3DDev->CreateTexture(w, h, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &tex->tex, 0))
		return -1;

	return index;
}
#endif

void DXTextureSetGreyScale(bool set)
{
	bMakeGrey = set;
}

long DXTextureFindTextureSlot(DXTEXTURE* tex)
{
	for (int i = 0; i < MAX_TPAGES; i++)
	{
		if (!(tex[i].dwFlags & TF_USED))
			return i;
	}

	return -1;
}

void DXClearAllTextures(DXTEXTURE* list)
{
	for (int i = 0; i < MAX_TPAGES; i++)
		DXTextureCleanup(i, list);
}

void DXTextureCleanup(long index, DXTEXTURE* list)
{
	DXTEXTURE* tex;

	tex = &list[index];

#if (DIRECT3D_VERSION < 0x900)
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
		GlobalFree(tex->pData);
		tex->pData = 0;
	}
#else
	if (tex->tex)
	{
		tex->tex->Release();
		tex->tex = 0;
	}
#endif

	tex->dwFlags = TF_EMPTY;
	tex->nWidth = 0;
	tex->nHeight = 0;
}

DXTEXTURE* DXRestoreSurfaceIfLost(long index, DXTEXTURE* list)
{
	DXTEXTURE* tex;

	tex = &list[index];

#if (DIRECT3D_VERSION < 0x900)
	if (tex->tex && tex->tex->pSurf)
	{
		if (tex->tex->pSurf->IsLost() == DDERR_SURFACELOST)
		{
			tex->tex->pSurf->Restore();
			tex->tex->DXTex = 0;
			tex->tex = 0;
		}
	}
#endif

	return &list[index];
}

#if (DIRECT3D_VERSION < 0x900)
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

	if (App.lpDeviceInfo->DDInfo[App.lpDXConfig->nDD].D3DInfo[App.lpDXConfig->nD3D].Texture[App.lpDXConfig->D3DTF].bPalette)
	{
		oldTF = App.lpDXConfig->D3DTF;
		App.lpDXConfig->D3DTF = 0;
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
				r = (col >> 16) & 0xFF;
				g = (col >> 8) & 0xFF;
				b = col & 0xFF;

				if (r && g && b)
					a = 255;

				break;

			case 888:
				a = 0;
				r = (col >> 16) & 0xFF;
				g = (col >> 8) & 0xFF;
				b = col & 0xFF;

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

			if (flags == TF_PICTEX)
				a = 255;

			if (bMakeGrey)
			{
				r = b;
				g = b;
			}

			tinfo = &App.lpDeviceInfo->DDInfo[App.lpDXConfig->nDD].D3DInfo[App.lpDXConfig->nD3D].Texture[App.lpDXConfig->D3DTF];

			if (tinfo->bAlpha)
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

	if (flags == TF_LEVELTEX && !App.lpDeviceInfo->DDInfo[App.lpDXConfig->nDD].D3DInfo[App.lpDXConfig->nD3D].Texture[App.lpDXConfig->D3DTF].bAlpha)
	{
		ckey.dwColorSpaceLowValue = 0;
		ckey.dwColorSpaceHighValue = 0;
		tex->pSystemSurface->SetColorKey(DDCKEY_SRCBLT, &ckey);
	}

	DD_UnlockSurface(tex->pSystemSurface, desc);

	if (oldTF != -1)
		App.lpDXConfig->D3DTF = oldTF;

	return index;
}
#else
static void CopyTexture16(long w, long h, uchar* src, LPDDSURFACEDESCX desc, ulong flags)
{
	ushort* s;
	ulong* dest;
	uchar r, g, b, a;

	s = (ushort*)src;

	for (int i = 0; i < h; i++)
	{
		dest = (ulong*)((uchar*)desc->pBits + desc->Pitch * i);
		
		for (int j = 0; j < w; j++)
		{
			r = (*s >> 7) & 0xF8;
			g = (*s >> 2) & 0xF8;
			b = (*s << 3) & 0xF8;
			a = ((*s >> 15) & 1) ? 255 : 0;

			if (bMakeGrey)
			{
				r = b;
				g = b;
			}

			if (flags == TF_PICTEX)
				a = 255;

			*dest++ = RGBA_MAKE(r, g, b, a);
			s++;
		}
	}
}

static void CopyTexture32(long w, long h, uchar* src, LPDDSURFACEDESCX desc, ulong flags)
{
	ulong* s;
	ulong* dest;
	uchar r, g, b, a;

	s = (ulong*)src;

	for (int i = 0; i < h; i++)
	{
		dest = (ulong*)((uchar*)desc->pBits + desc->Pitch * i);

		for (int j = 0; j < w; j++)
		{
			r = RGBA_GETRED(*s);
			g = RGBA_GETGREEN(*s);
			b = RGBA_GETBLUE(*s);
			a = RGBA_GETALPHA(*s);

			if (bMakeGrey)
			{
				r = b;
				g = b;
			}

			if (flags == TF_PICTEX)
				a = 255;

			*dest++ = RGBA_MAKE(r, g, b, a);
			s++;
		}
	}
}

long DXTextureAdd(long w, long h, uchar* src, DXTEXTURE* list, long bpp, ulong flags)
{
	DXTEXTURE* tex;
	DDSURFACEDESCX desc;
	long index;

	index = DXTextureMakeDeviceSurface(w, h, list);

	if (index < 0)
		return -1;

	tex = &list[index];
	tex->dwFlags |= flags;

	if FAILED(tex->tex->LockRect(0, &desc, 0, 0))
		return -1;

	switch (bpp)
	{
	case 8888:
		CopyTexture32(w, h, src, &desc, flags);
		break;

	case 555:
		CopyTexture16(w, h, src, &desc, flags);
		break;
	}

	tex->tex->UnlockRect(0);
	return index;
}
#endif

#if (DIRECT3D_VERSION < 0x900)
void DXCreateMaxTPages(long create)
{
	DIRECT3DINFO* d3dinfo;
	TEXTURE* tex;
	LPDDPIXELFORMAT ddpf, ddpf2;
	long n, oldTF;

	d3dinfo = &App.lpDeviceInfo->DDInfo[App.lpDXConfig->nDD].D3DInfo[App.lpDXConfig->nD3D];
	ddpf = &d3dinfo->Texture[App.lpDXConfig->D3DTF].ddsd.ddpfPixelFormat;
	DXCreateTextureSurface(TextureSurfaces, ddpf);
	n = 1;

	if (create && ddpf->dwRGBBitCount == 8)
	{
		bSetColorKey = 0;
		oldTF = App.lpDXConfig->D3DTF;
		App.lpDXConfig->D3DTF = 0;
		ddpf2 = &d3dinfo->Texture[App.lpDXConfig->D3DTF].ddsd.ddpfPixelFormat;

		for (; n < 6; n++)
		{
			tex = &TextureSurfaces[n];

			if (!DXCreateTextureSurface(tex, ddpf2))
				break;
		}

		App.lpDXConfig->D3DTF = oldTF;
		bSetColorKey = 1;
		ddpf = &d3dinfo->Texture[App.lpDXConfig->D3DTF].ddsd.ddpfPixelFormat;
	}

	for (; n < MAX_TPAGES; n++)
	{
		tex = &TextureSurfaces[n];

		if (!DXCreateTextureSurface(tex, ddpf))
			break;
	}

	nTextures = n;
}

void DXFreeTPages()
{
	TEXTURE* tex;
	DXTEXTURE* DXTex;

	for (int i = 0; i < MAX_TPAGES; i++)
	{
		tex = &TextureSurfaces[i];

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

	nTextures = 0;
}
#endif
