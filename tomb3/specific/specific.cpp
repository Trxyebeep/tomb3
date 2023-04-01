#include "../tomb3/pch.h"
#include "specific.h"
#include "file.h"
#include "utils.h"
#include "ds.h"
#include "audio.h"
#include "winmain.h"
#include "../game/sound.h"

static long MasterVolume;
char cd_drive;

bool CD_Init()
{
	FILE* file;
	char VFAW[] = "d:\\VFAW.AFP";
	char NEIR[] = "d:\\NEIR.AFP";
	char OKET[] = "d:\\OKET.AFP";
	char AWCS[] = "d:\\AWCS.AFP";
	bool found;

	while (!FindCDDrive())
	{
		if (!UT_OKCancelBox(MAKEINTRESOURCE(102), App.WindowHandle))
			return 0;
	}

	VFAW[0] = cd_drive;
	NEIR[0] = cd_drive;
	OKET[0] = cd_drive;
	AWCS[0] = cd_drive;

	while (1)
	{
		file = fopen(VFAW, "rb");

		if (!file)
			found = 0;
		else
		{
			fseek(file, 0x29845000, SEEK_SET);
			found = fgetc(file) == '{';
			fclose(file);
		}

		if (found)
			break;

		file = fopen(NEIR, "rb");

		if (!file)
			found = 0;
		else
		{
			fseek(file, 0x29825800, SEEK_SET);
			found = fgetc(file) == '3';
			fclose(file);
		}

		if (found)
			break;

		file = fopen(OKET, "rb");

		if (!file)
			found = 0;
		else
		{
			fseek(file, 0x29842800, SEEK_SET);
			found = fgetc(file) == 'u';
			fclose(file);
		}

		if (found)
			break;

		file = fopen(AWCS, "rb");

		if (!file)
			found = 0;
		else
		{
			fseek(file, 0x2981E000, SEEK_SET);
			found = fgetc(file) == '{';
			fclose(file);
		}

		if (found)
			break;

		if (!UT_OKCancelBox(MAKEINTRESOURCE(102), App.WindowHandle))
			return 0;
	}

	return 1;
}

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

void S_CDLoop()	//old code
{
	MCI_PLAY_PARMS playParams;
	MCI_STATUS_PARMS statusParams;
	static MCIDEVICEID mciId;
	static long CD_LoopTrack;

	if (CD_LoopTrack)
	{
		statusParams.dwItem = MCI_STATUS_MODE;

		if (!mciSendCommand(mciId, MCI_STATUS, MCI_STATUS_ITEM, (DWORD_PTR)&statusParams) && statusParams.dwReturn == MCI_MODE_STOP)
		{
			playParams.dwFrom = CD_LoopTrack;
			playParams.dwTo = CD_LoopTrack + 1;
			mciSendCommand(mciId, MCI_PLAY, MCI_FROM | MCI_TO, (DWORD_PTR)&playParams);
		}
	}
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
