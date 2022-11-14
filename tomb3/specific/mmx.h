#pragma once
#include "../global/vars.h"

void inject_mmx(bool replace);

void MMXBlit32to15(uchar* dest, ulong* src, long w);
void MMXBlit32to16(uchar* dest, ulong* src, long w);
void MMXBlit32to24(uchar* dest, ulong* src, long w);
