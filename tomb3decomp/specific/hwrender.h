#pragma once
#include "../global/vars.h"

void inject_hwrender(bool replace);

void HWR_EnableZBuffer(bool write, bool compare);
void HWR_EnableColorKey(bool enable);
void HWR_EnableAlphaBlend(bool enable);
void HWR_EnableColorAddition(bool enable);
void HWR_ResetZBuffer();
void HWR_ResetColorKey();
void HWR_EnablePerspCorrect(bool enable);
void HWR_EnableFilter(bool enable);

#define HWR_SetCurrentTexture	( (void(__cdecl*)(DXTEXTURE*)) 0x00484C30 )

#define SetRenderState	( *(HRESULT(__cdecl**)(D3DRENDERSTATETYPE, long)) 0x006CA1B8 )	//here FOR NOW
#define DrawPrimitive	( *(HRESULT(__cdecl**)(D3DPRIMITIVETYPE, D3DVERTEXTYPE, LPVOID, ulong, ulong)) 0x006CA1C0 )
