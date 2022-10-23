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

long DS_GetFreeChannel()
{
	for (int i = 0; i < 32; i++)
	{
		if (!DS_Samples[i])
			return i;
	}

	for (int i = 0; i < 32; i++)
	{
		if (!DS_IsChannelPlaying(i))
			return i;
	}

	return -1;
}

void inject_ds(bool replace)
{
	INJECT(0x00480740, DS_IsChannelPlaying, replace);
	INJECT(0x004808B0, DS_GetFreeChannel, replace);
}
