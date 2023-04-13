#include "../tomb3/pch.h"
#include "specific.h"
#include "file.h"
#include "utils.h"
#include "ds.h"
#include "audio.h"
#include "winmain.h"
#include "../game/sound.h"

static long MasterVolume;

long CalcVolume(long volume)
{
	if (!volume || !MasterVolume)
		return DSBVOLUME_MIN;

	return long((float(MasterVolume * volume) * 0.00019074068F - 400.0F) * 6.0F);
}

long CalcPan(short angle)
{
	angle += 0x4000;

	if (angle < 0)
	{
		angle = -angle;

		if (angle < 0)
			angle = 0x7FFF;
	}

	return (angle - 0x4000) >> 4;
}

void S_SoundSetMasterVolume(long volume)
{
	MasterVolume = volume;
}

long S_SoundPlaySample(long num, ushort volume, long pitch, short pan)
{
	if (!sound_active)
		return -3;

	return DS_StartSample(num, CalcVolume(volume), pitch, CalcPan(pan), 0);
}

long S_SoundPlaySampleLooped(long num, ushort volume, long pitch, short pan)
{
	if (!sound_active)
		return -3;

	return DS_StartSample(num, CalcVolume(volume), pitch, CalcPan(pan), DSBPLAY_LOOPING);
}

void S_SoundSetPanAndVolume(long num, short angle, ushort volume)
{
	if (sound_active)
		DS_AdjustVolumeAndPan(num, CalcVolume(volume), CalcPan(angle));
}

void S_SoundSetPitch(long num, long pitch)
{
	if (sound_active)
		DS_AdjustPitch(num, pitch);
}

void S_SoundStopSample(long num)
{
	if (sound_active)
		DS_StopSample(num);
}

void S_SoundStopAllSamples()
{
	if (sound_active)
	{
		for (int i = 0; i < 32; i++)
		{
			if (DS_Samples[i])
			{
				DS_Samples[i]->Stop();
				DS_Samples[i]->Release();
				DS_Samples[i] = 0;
			}
		}
	}
}

long S_SoundSampleIsPlaying(long num)
{
	if (sound_active)
		return DS_IsChannelPlaying(num);

	return 0;
}

void S_CDPlay(short track, long mode)
{
	ACMEmulateCDPlay(track, mode);
}

void S_CDStop()
{
	ACMEmulateCDStop();
}

long S_CDGetLoc()
{
	return ACMGetTrackLocation();
}

void S_CDMute()
{
	ACMMute();
}

void S_CDVolume(long volume)
{
	ACMSetVolume(volume);
}

long S_StartSyncedAudio(long track)
{
	if (App.DXConfig.sound)
	{
		ACMEmulateCDPlay(track, 0);
		return 1;
	}

	return 0;
}
