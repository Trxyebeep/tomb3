#include "../tomb3/pch.h"
#include "mmx.h"

void MMXBlit32to15(uchar* dest, ulong* src, long w)
{
	__int64 m0 = 0x2000000820000008;	//mul factor
	__int64 m1 = 0x00F800F800F800F8;	//rgb mask1
	__int64 m2 = 0x0000F8000000F800;	//rgb mask2

	__asm
	{
		mov eax, [w]
		mov ebx, [src]
		sub eax, 1
		mov edx, [dest]
		and eax, 0FFFFFFF8h
		movq mm7, [m0]
		movq mm6, [m2]
		movq mm2, [ebx + eax * 4 + 8]
		movq mm0, [ebx + eax * 4]
		movq mm3, mm2
		pand mm3, [m1]
		movq mm1, mm0
		pand mm1, [m1]
		pmaddwd mm3, mm7
		pmaddwd mm1, mm7
		pand mm2, mm6

	lp:
		movq mm4, [ebx + eax * 4 + 18h]
		pand mm0, mm6
		movq mm5, [ebx + eax * 4 + 10h]
		por mm3, mm2
		psrld mm3, 6
		por mm1, mm0
		movq mm0, mm4
		psrld mm1, 6
		pand mm0, [m1]
		packssdw mm1, mm3
		movq mm3, mm5
		pmaddwd mm0, mm7
		pand mm3, [m1]
		pand mm4, mm6
		movq[edx + eax * 2], mm1
		pmaddwd mm3, mm7
		sub eax, 8
		por mm4, mm0
		pand mm5, mm6
		psrld mm4, 6
		movq mm2, [ebx + eax * 4 + 8]
		por mm5, mm3
		movq mm0, [ebx + eax * 4]
		psrld mm5, 06
		movq mm3, mm2
		movq mm1, mm0
		pand mm3, [m1]
		packssdw mm5, mm4
		pand mm1, [m1]
		pand mm2, mm6
		movq[edx + eax * 2 + 18h], mm5
		pmaddwd mm3, mm7
		pmaddwd mm1, mm7
		jg lp
		movq mm4, [ebx + eax * 4 + 18h]
		pand mm0, mm6
		movq mm5, [ebx + eax * 4 + 10h]
		por mm3, mm2
		psrld mm3, 6
		por mm1, mm0
		movq mm0, mm4
		psrld mm1, 6
		pand mm0, [m1]
		packssdw mm1, mm3
		movq mm3, mm5
		pmaddwd mm0, mm7
		pand mm3, [m1]
		pand mm4, mm6
		movq[edx + eax * 2], mm1
		pmaddwd mm3, mm7
		sub eax, 8
		por mm4, mm0
		pand mm5, mm6
		psrld mm4, 6
		por mm5, mm3
		psrld mm5, 6
		packssdw mm5, mm4
		movq[edx + eax * 2 + 18h], mm5
		emms
	}
}

void inject_mmx(bool replace)
{
	INJECT(0x00496930, MMXBlit32to15, replace);
}
