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

void inject_sound(bool replace)
{
	INJECT(0x00467E20, SOUND_Init, replace);
	INJECT(0x00467DF0, SOUND_Stop, replace);
}
