#include "../tomb3/pch.h"
#include "dxshell.h"

long BPPToDDBD(long BPP)
{
	switch (BPP)
	{
	case 1:
		return DDBD_1;

	case 2:
		return DDBD_2;

	case 4:
		return DDBD_4;

	case 8:
		return DDBD_8;

	case 16:
		return DDBD_16;

	case 24:
		return DDBD_24;

	case 32:
		return DDBD_32;

	default:
		return 0;
	}
}

bool DXSetVideoMode(LPDIRECTDRAW2 ddx, long w, long h, long bpp)
{
	return ddx->SetDisplayMode(w, h, bpp, 0, 0) == DD_OK;
}

void inject_dxshell(bool replace)
{
	INJECT(0x0048FDB0, BPPToDDBD, replace);
	INJECT(0x0048FEE0, DXSetVideoMode, replace);
}
