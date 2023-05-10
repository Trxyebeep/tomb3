#pragma once
#include "../global/types.h"

bool DXCreateDirect3DDevice(LPDIRECT3DX dd3x, GUID guid, LPDIRECTDRAWSURFACEX surf, LPDIRECT3DDEVICEX* device);
void DXBitMask2ShiftCnt(ulong mask, uchar* shift, uchar* count);

#if (DIRECT3D_VERSION < 0x900)
void DXDoFlipWait();
bool DXCheckForLostSurfaces();
long BPPToDDBD(long BPP);
bool DXSetVideoMode(LPDIRECTDRAWX ddx, long w, long h, long bpp);
bool DXCreateSurface(LPDIRECTDRAWX ddx, LPDDSURFACEDESCX desc, LPDIRECTDRAWSURFACEX* surf);
bool DXGetAttachedSurface(LPDIRECTDRAWSURFACEX surf, LPDDSCAPSX caps, LPDIRECTDRAWSURFACEX* attached);
bool DXAddAttachedSurface(LPDIRECTDRAWSURFACEX surf, LPDIRECTDRAWSURFACEX attached);
bool DXCreateViewPort(LPDIRECT3DX dd3x, LPDIRECT3DDEVICEX device, long w, long h, LPDIRECT3DVIEWPORTX* lpvp);
bool DXSurfBlt(LPDIRECTDRAWSURFACEX surf, LPRECT rect, long FillColor);
bool DXCreateDirectDraw(DEVICEINFO* dev, DXCONFIG* conf, LPDIRECTDRAWX* ddx);
bool DXCreateDirect3D(LPDIRECTDRAWX ddx, LPDIRECT3DX* d3dx);
bool DXSetCooperativeLevel(LPDIRECTDRAWX ddx, HWND hwnd, long flags);
HRESULT CALLBACK DXEnumDisplayModes(LPDDSURFACEDESCX lpDDSurfaceDesc, LPVOID lpContext);
bool DXCreateZBuffer(DEVICEINFO* device, DXCONFIG* config);
BOOL CALLBACK DXEnumDirectDraw(GUID FAR* lpGUID, LPSTR lpDriverDescription, LPSTR lpDriverName, LPVOID lpContext);
HRESULT CALLBACK DXEnumTextureFormats(LPDDSURFACEDESCX lpDDPixFmt, LPVOID lpContext);
HRESULT CALLBACK DXEnumDirect3D(LPGUID lpGuid, LPSTR description, LPSTR name, LPD3DDEVICEDESC lpHWDesc, LPD3DDEVICEDESC lpHELDesc, LPVOID lpContext);
void DXSaveScreen(LPDIRECTDRAWSURFACEX surf);
void DXClearBuffers(ulong flags, ulong color);
void DXGetDeviceInfo(DEVICEINFO* device, HWND hWnd, HINSTANCE hInstance);
bool DXStartRenderer(DEVICEINFO* device, DXCONFIG* config, bool createNew, bool windowed);
bool DXSwitchVideoMode(long needed, long current, bool disableZBuffer);
#else
void DXClearBuffers(ulong flags, ulong color);
bool DXGetDeviceInfo(DEVICEINFO* device);
bool DXStartRenderer(DEVICEINFO* device, DXCONFIG* config, bool createNew, bool windowed);
bool DXSwitchVideoMode(long needed, long current);
bool DXToggleZbuffer();
void DXCreateCaptureBuffer();
void DXFreeCaptureBuffer();
void DXCreatePictureBuffer();
void DXFreePictureBuffer();
#endif

BOOL CALLBACK DXEnumDirectSound(LPGUID lpGuid, LPCSTR lpcstrDescription, LPCSTR lpcstrModule, LPVOID lpContext);
void DXFreeDeviceInfo(DEVICEINFO* device);
bool DXUpdateFrame(bool runMessageLoop, LPRECT rect);
long DXToggleFullScreen();
void DXMove(long x, long y);
