#include "../tomb3/pch.h"
#include "dx.h"
#include "dd.h"

void DX_SaveScreen(LPDIRECTDRAWSURFACE3 surf)
{
	FILE* file;
	DDSURFACEDESC desc;
	ushort* pSurf;
	short* pDest;
#ifdef TROYESTUFF
	char* pM;
#endif
	long r, g, b;
	static long num;
	ushort c;
	char buf[16];
	static char tga_header[18] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 64, 1, 0, 1, 16, 0 };

	memset(&desc, 0, sizeof(DDSURFACEDESC));
	desc.dwSize = sizeof(DDSURFACEDESC);

	if (FAILED(surf->GetSurfaceDesc(&desc)))
		return;

	memset(&desc, 0, sizeof(DDSURFACEDESC));
	desc.dwSize = sizeof(DDSURFACEDESC);

	if (FAILED(DD_LockSurface(surf, desc, DDLOCK_WAIT | DDLOCK_WRITEONLY)))
		return;

	pSurf = (ushort*)desc.lpSurface;
	num++;
	sprintf(buf, "tomb%04d.tga", num);
	file = fopen(buf, "wb");

	if (file)
	{
		*(short*)&tga_header[12] = (short)desc.dwWidth;
		*(short*)&tga_header[14] = (short)desc.dwHeight;
		fwrite(tga_header, sizeof(tga_header), 1, file);

#ifdef TROYESTUFF
		pM = (char*)malloc(2 * desc.dwWidth * desc.dwHeight);
		pDest = (short*)pM;
#else
		pDest = (short*)malloc_ptr;
#endif
		pSurf += desc.dwHeight * (desc.lPitch / 2);

		for (ulong h = 0; h < desc.dwHeight; h++)
		{
			for (ulong w = 0; w < desc.dwWidth; w++)
			{
				c = pSurf[w];

				if (desc.ddpfPixelFormat.dwRBitMask == 0xF800)
				{
					r = c >> 11;
					g = (c >> 6) & 0x1F;
					b = c & 0x1F;
					*pDest++ = short(r << 10 | g << 5 | b);
				}
				else
					*pDest++ = (short)c;
			}

			pSurf -= desc.lPitch / 2;
		}

#ifdef TROYESTUFF
		fwrite(pM, 2 * desc.dwWidth * desc.dwHeight, 1, file);
		free(pM);
#else
		fwrite(malloc_ptr, 2 * desc.dwWidth * desc.dwHeight, 1, file);
#endif
		fclose(file);

		buf[7]++;

		if (buf[7] > '9')
		{
			buf[7] = '0';
			buf[6]++;
		}
	}

	DD_UnlockSurface(surf, desc);
}

void inject_dx(bool replace)
{
	INJECT(0x004B40A0, DX_SaveScreen, replace);
}
