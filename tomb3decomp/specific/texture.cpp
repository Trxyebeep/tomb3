#include "../tomb3/pch.h"
#include "texture.h"

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

void inject_texture(bool replace)
{
	INJECT(0x004B1B80, DXTextureNewPalette, replace);
	INJECT(0x004B1FB0, DXResetPalette, replace);
	INJECT(0x004B1B70, DXTextureSetGreyScale, replace);
	INJECT(0x004B1FD0, DXTextureGetInterface, replace);
	INJECT(0x004B2000, DXTextureFindTextureSlot, replace);
}
