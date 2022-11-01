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

void inject_texture(bool replace)
{
	INJECT(0x004B1B80, DXTextureNewPalette, replace);
	INJECT(0x004B1FB0, DXResetPalette, replace);
	INJECT(0x004B1B70, DXTextureSetGreyScale, replace);
}
