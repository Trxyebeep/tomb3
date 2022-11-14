#pragma once
#include "../global/vars.h"

void inject_dd(bool replace);

bool DD_SpinMessageLoop(bool wait);
HRESULT DD_LockSurface(LPDIRECTDRAWSURFACEX surf, DDSURFACEDESCX& desc, ulong flags);
HRESULT DD_UnlockSurface(LPDIRECTDRAWSURFACEX surf, DDSURFACEDESCX& desc);
HRESULT DD_CreateSurface(DDSURFACEDESCX& desc, LPDIRECTDRAWSURFACEX& surf);
HRESULT DD_EnsureSurfaceAvailable(LPDIRECTDRAWSURFACEX surf, LPDIRECTDRAWSURFACEX tSurf, bool clear);
bool DD_ClearSurface(LPDIRECTDRAWSURFACEX surf, LPRECT rect, ulong col);
