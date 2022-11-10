#include "../tomb3/pch.h"
#include "sound.h"
#include "../specific/specific.h"

void SOUND_Init()
{
	for (int i = 0; i < 32; i++)
		LaSlot[i].nSampleInfo = -1;

	sound_active = 1;
}

void SOUND_Stop()
{
	if (sound_active)
	{
		S_SoundStopAllSamples();

		for (int i = 0; i < 32; i++)
			LaSlot[i].nSampleInfo = -1;
	}
}

void SOUND_EndScene()
{
	SOUND_SLOT* slot;

	if (sound_active)
	{
		for (int i = 0; i < 32; i++)
		{
			slot = &LaSlot[i];

			if (slot->nSampleInfo < 0)
				continue;

			if ((sample_infos[slot->nSampleInfo].flags & 3) == 3)
			{
				if (!slot->nVolume)
				{
					S_SoundStopSample(i);
					slot->nSampleInfo = -1;
					continue;
				}

				S_SoundSetPanAndVolume(i, (short)slot->nPan, (ushort)slot->nVolume);
				S_SoundSetPitch(i, slot->nPitch);
				slot->nVolume = 0;
			}
			else if (!S_SoundSampleIsPlaying(i))
				slot->nSampleInfo = -1;
		}
	}
}

void inject_sound(bool replace)
{
	INJECT(0x00467E20, SOUND_Init, replace);
	INJECT(0x00467DF0, SOUND_Stop, replace);
	INJECT(0x00467D60, SOUND_EndScene, replace);
}
