#pragma once
#include "../global/vars.h"

void inject_dd(bool replace);

bool DD_SpinMessageLoop(bool wait);
HRESULT DD_LockSurface(LPDIRECTDRAWSURFACE3 surf, DDSURFACEDESC& desc, ulong flags);
HRESULT DD_UnlockSurface(LPDIRECTDRAWSURFACE3 surf, DDSURFACEDESC& desc);
HRESULT DD_CreateSurface(DDSURFACEDESC& desc, LPDIRECTDRAWSURFACE3& surf);
HRESULT DD_EnsureSurfaceAvailable(LPDIRECTDRAWSURFACE3 surf, LPDIRECTDRAWSURFACE3 tSurf, bool clear);
bool DD_ClearSurface(LPDIRECTDRAWSURFACE3 surf, LPRECT rect, ulong col);
