#include "../tomb3/pch.h"
#include "dd.h"
#include "winmain.h"

bool DD_SpinMessageLoop(bool wait)
{
	MSG msg;
	static long nRecalls;
	static bool bWM_QUIT;

	nRecalls++;

	if (bWM_QUIT)
	{
		nRecalls--;
		return 0;
	}

	if (wait)
		WaitMessage();

	while (1)
	{
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
			{
				bWM_QUIT = 1;
				GtWindowClosed = 1;
				nRecalls--;
				game_closedown = 1;
				return 0;
			}
		}

		if (App.bFocus)
			break;

		WaitMessage();
	}

	nRecalls--;
	return 1;
}

#if (DIRECT3D_VERSION < 0x900)
HRESULT DD_LockSurface(LPDIRECTDRAWSURFACEX surf, DDSURFACEDESCX& desc, ulong flags)
{
	memset(&desc, 0, sizeof(DDSURFACEDESCX));
	desc.dwSize = sizeof(DDSURFACEDESCX);
	return surf->Lock(0, &desc, flags, 0);
}

HRESULT DD_UnlockSurface(LPDIRECTDRAWSURFACEX surf, DDSURFACEDESCX& desc)
{
	return surf->Unlock(desc.lpSurface);
}

HRESULT DD_CreateSurface(DDSURFACEDESCX& desc, LPDIRECTDRAWSURFACEX& surf)
{
	HRESULT res;
	LPDIRECTDRAWSURFACE ps;

	res = App.DDraw->CreateSurface(&desc, &ps, 0);

	if (FAILED(res))
		return res;

	res = ps->QueryInterface(DDSGUID, (LPVOID*)&surf);
	ps->Release();
	return res;
}

HRESULT DD_EnsureSurfaceAvailable(LPDIRECTDRAWSURFACEX surf, LPDIRECTDRAWSURFACEX tSurf, bool clear)
{
	HRESULT res;

	res = surf->IsLost();

	if (res == DDERR_SURFACELOST)
	{
		if (!tSurf)
			tSurf = surf;

		res = surf->Restore();

		if (SUCCEEDED(res) && clear)
			DD_ClearSurface(surf, 0, 0);
	}

	return res;
}

bool DD_ClearSurface(LPDIRECTDRAWSURFACEX surf, LPRECT rect, ulong col)
{
	DDBLTFX blt;

	memset(&blt, 0, sizeof(DDBLTFX));
	blt.dwSize = sizeof(DDBLTFX);
	blt.dwFillColor = col;
	return SUCCEEDED(surf->Blt(rect, 0, 0, DDBLT_WAIT | DDBLT_COLORFILL, &blt));
}
#endif
