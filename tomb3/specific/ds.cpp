#include "../tomb3/pch.h"
#include "ds.h"
#include <list>
#include "winmain.h"
#include "../game/camera.h"

static LPDIRECTSOUNDBUFFER DS_Buffers[256];
static ulong DS_SampleFrequencies[256];
static long DS_SamplesPlaying[32];

LPDIRECTSOUNDBUFFER DS_Samples[32];
LPDIRECTSOUND8 lpDirectSound;

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

long DS_StartSample(long num, long volume, long pitch, long pan, ulong flags)
{
	LPDIRECTSOUNDBUFFER buffer;
	ulong fq;
	long channel;

	channel = DS_GetFreeChannel();

	if (channel < 0)
		return -1;

	if (FAILED(lpDirectSound->DuplicateSoundBuffer(DS_Buffers[num], &buffer)))
		return -2;

	fq = (pitch * DS_SampleFrequencies[num]) >> 16;

	if (fq < 100)
		fq = 100;

	if (fq > 100000)
		fq = 100000;

	if (FAILED(buffer->SetVolume(volume)) || FAILED(buffer->SetFrequency(fq)) || FAILED(buffer->SetPan(pan)) ||
		FAILED(buffer->SetCurrentPosition(0)) || FAILED(buffer->Play(0, 0, flags)))
		return -2;

	DS_SamplesPlaying[channel] = num;
	DS_Samples[channel] = buffer;
	return channel;
}

void DS_FreeAllSamples()
{
	if (App.DXConfig.sound)
	{
		for (int i = 0; i < 256; i++)
		{
			if (DS_Buffers[i])
			{
				DS_Buffers[i]->Release();
				DS_Buffers[i] = 0;
			}
		}
	}
}

bool DS_MakeSample(long num, LPWAVEFORMATEX fmt, LPVOID data, ulong bytes)
{
	DSBUFFERDESC desc;
	LPVOID pWrite;
	ulong aBytes;

	if (!App.DXConfig.sound || num > 256)
		return 0;

	desc.dwSize = sizeof(DSBUFFERDESC);
	desc.lpwfxFormat = fmt;
	desc.dwFlags = DSBCAPS_STATIC | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME;
	desc.dwBufferBytes = bytes;
	desc.dwReserved = 0;

	if (FAILED(lpDirectSound->CreateSoundBuffer(&desc, &DS_Buffers[num], 0)))
		return 0;

	if (FAILED(DS_Buffers[num]->Lock(0, bytes, &pWrite, &aBytes, 0, 0, 0)))
		return 0;

	memcpy(pWrite, data, aBytes);

	if (FAILED(DS_Buffers[num]->Unlock(pWrite, aBytes, 0, 0)))
		return 0;

	DS_SampleFrequencies[num] = fmt->nSamplesPerSec;
	return 1;
}

void DS_AdjustVolumeAndPan(long num, long volume, long pan)
{
	if (num >= 0 && DS_Samples[num])
	{
		DS_Samples[num]->SetVolume(volume);
		DS_Samples[num]->SetPan(pan);
	}
}

void DS_AdjustPitch(long num, long pitch)
{
	ulong fq;

	if (num >= 0 && DS_Samples[num])
	{
		fq = (pitch * DS_SampleFrequencies[DS_SamplesPlaying[num]]) >> 16;
		DS_Samples[num]->SetFrequency(fq);
	}
}

void DS_StopSample(long num)
{
	if (num >= 0 && DS_Samples[num])
	{
		DS_Samples[num]->Stop();
		DS_Samples[num]->Release();
		DS_Samples[num] = 0;
	}
}

bool DS_Create(LPGUID lpGuid)
{
	return SUCCEEDED(DirectSoundCreate8(lpGuid, &lpDirectSound, 0));
}

bool DS_IsSoundEnabled()
{
	return App.DXConfig.sound;
}

bool DS_SetOutputFormat()
{
	LPDIRECTSOUNDBUFFER buffer;
	DSBUFFERDESC desc;
	WAVEFORMATEX pcfxFormat;
	bool ret;

	memset(&desc, 0, sizeof(desc));
	desc.dwSize = sizeof(desc);
	desc.dwFlags = DSBCAPS_PRIMARYBUFFER;

	if (FAILED(lpDirectSound->CreateSoundBuffer(&desc, &buffer, 0)))
		return 0;

	pcfxFormat.wFormatTag = WAVE_FORMAT_PCM;
	pcfxFormat.nChannels = 2;
	pcfxFormat.nSamplesPerSec = 22050;
	pcfxFormat.nAvgBytesPerSec = 4 * 22050;
	pcfxFormat.cbSize = 0;
	pcfxFormat.nBlockAlign = 4;
	pcfxFormat.wBitsPerSample = 16;
	ret = SUCCEEDED(buffer->SetFormat(&pcfxFormat));

	if (buffer)
		buffer->Release();

	return ret;
}

void DS_Start(HWND hwnd)
{
	memset(DS_Buffers, 0, sizeof(DS_Buffers));
	memset(DS_Samples, 0, sizeof(DS_Samples));
	camera.mike_at_lara = 0;

	if (DS_Create(App.lpDeviceInfo->DSInfo[App.lpDXConfig->nDS].lpGuid))
	{
		if (!hwnd)
			hwnd = App.WindowHandle;

		if (FAILED(lpDirectSound->SetCooperativeLevel(hwnd, DSSCL_EXCLUSIVE)))
			S_ExitSystem("DS_Start: SetCooperativeLevel failed");

		DS_SetOutputFormat();
	}
}

void DS_Finish()
{
	DS_FreeAllSamples();

	if (lpDirectSound)
	{
		lpDirectSound->Release();
		lpDirectSound = 0;
	}
}
