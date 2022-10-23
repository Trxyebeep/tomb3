#include "../tomb3/pch.h"
#include "ds.h"

#define DS_Samples	ARRAY_(0x00632B50, LPDIRECTSOUNDBUFFER, [32])

bool DS_IsChannelPlaying(long num)
{
	LPDIRECTSOUNDBUFFER buffer;
	ulong status;

	buffer = DS_Samples[num];

	if (buffer && SUCCEEDED(buffer->GetStatus(&status)))
	{
		if (status & DSBSTATUS_PLAYING)
			return 1;

		buffer->Release();
		DS_Samples[num] = 0;
	}

	return 0;
}

void inject_ds(bool replace)
{
	INJECT(0x00480740, DS_IsChannelPlaying, replace);
}
