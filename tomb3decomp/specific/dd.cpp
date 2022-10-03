#include "../tomb3/pch.h"
#include "dd.h"

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

HRESULT DD_LockSurface(LPDIRECTDRAWSURFACE3 surf, DDSURFACEDESC& desc, ulong flags)
{
	memset(&desc, 0, sizeof(DDSURFACEDESC));
	desc.dwSize = sizeof(DDSURFACEDESC);
	return surf->Lock(0, &desc, flags, 0);
}

HRESULT DD_UnlockSurface(LPDIRECTDRAWSURFACE3 surf, DDSURFACEDESC& desc)
{
	return surf->Unlock(desc.lpSurface);
}

void inject_dd(bool replace)
{
	INJECT(0x004B3830, DD_SpinMessageLoop, replace);
	INJECT(0x004B3900, DD_LockSurface, replace);
	INJECT(0x004B3930, DD_UnlockSurface, replace);
}
