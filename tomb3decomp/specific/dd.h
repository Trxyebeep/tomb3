#pragma once
#include "../global/vars.h"

void inject_dd(bool replace);

bool DD_SpinMessageLoop(bool wait);
HRESULT DD_LockSurface(LPDIRECTDRAWSURFACE3 surf, DDSURFACEDESC& desc, ulong flags);
HRESULT DD_UnlockSurface(LPDIRECTDRAWSURFACE3 surf, DDSURFACEDESC& desc);
