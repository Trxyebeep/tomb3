#pragma once
#include "../global/types.h"

void HWR_EnableZBuffer(bool write, bool compare);
void HWR_EnableColorKey(bool enable);
void HWR_EnableAlphaBlend(bool enable);
void HWR_EnableColorAddition(bool enable);
void HWR_EnableColorSubtraction(bool enable);
void HWR_ResetZBuffer();
void HWR_ResetColorKey();
void HWR_EnablePerspCorrect(bool enable);
void HWR_EnableFilter(bool enable);
void HWR_ResetCurrentTexture();
void HWR_BeginScene();
void HWR_EndScene();
void HWR_DrawRoutines(long nVtx, D3DTLVERTEX* vtx, long nDrawType, long TPage);
void HWR_DrawRoutinesStippledAlpha(long nVtx, D3DTLVERTEX* vtx, long nDrawType, long TPage);
void HWR_DrawRoutinesNoAlpha(long nVtx, D3DTLVERTEX* vtx, long nDrawType, long TPage);
__inline void HWR_InitGamma(float gamma);
void HWR_InitState();
bool HWR_Init();
void HWR_DrawPolyList(long num, long* pSort);
void HWR_DrawPolyListBF(long num, long* pSort);
void HWR_FreeTexturePages();
void HWR_GetAllTextureHandles();
void HWR_LoadTexturePages(long nPages, uchar* src, uchar* palette);
void HWR_SetCurrentTexture(DXTEXTURE* tex);

extern HRESULT (*DrawPrimitive)(D3DPRIMITIVETYPE, D3DVERTEXTYPE, LPVOID, ulong, ulong);
extern HRESULT (*SetRenderState)(D3DRENDERSTATETYPE, ulong);
extern HRESULT (*BeginScene)();
extern HRESULT (*EndScene)();
extern void (*DrawRoutine)(long, D3DTLVERTEX*, long, long);

extern float GammaOption;
extern uchar ColorTable[256];
extern bool bAlphaTesting;
