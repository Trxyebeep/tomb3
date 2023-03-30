#include "../tomb3/pch.h"
#include "sound.h"
#include "../specific/specific.h"
#include "../specific/game.h"
#include "../3dsystem/phd_math.h"
#include "control.h"
#include "camera.h"

static SOUND_SLOT LaSlot[32];
short sample_lut[370];
SAMPLE_INFO* sample_infos;
long num_sample_infos;
long sound_active;

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

void StopSoundEffect(long sfx)
{
	long lut;

	if (sound_active)
	{
		lut = sample_lut[sfx];

		for (int i = 0; i < 32; i++)
		{
			if (LaSlot[i].nSampleInfo >= lut && LaSlot[i].nSampleInfo < (lut + ((sample_infos[lut].flags >> 2) & 0xF)))
			{
				S_SoundStopSample(i);
				LaSlot[i].nSampleInfo = -1;
			}
		}
	}
}

long SoundEffect(long sfx, PHD_3DPOS* pos, long flags)
{
	SAMPLE_INFO* info;
	ulong distance;
	long lut, radius, pan, dx, dy, dz, volume, pitch, rnd, sample, flag, vol, slot;

	if (!sound_active || !(flags & SFX_ALWAYS) && (flags & SFX_WATER) != (room[camera.pos.room_number].flags & ROOM_UNDERWATER))
		return 0;

	lut = sample_lut[sfx];

	if (lut == -1)
	{
		sample_lut[sfx] = -2;
		return 0;
	}

	if (lut == -2)
		return 0;

	info = &sample_infos[lut];

	if (info->randomness)
	{
		if ((GetRandomDraw() & 0xFF) > info->randomness)
			return 0;
	}

	radius = info->radius << 10;
	pan = 0;

	if (pos)
	{
		dx = pos->x_pos - camera.mike_pos.x;
		dy = pos->y_pos - camera.mike_pos.y;
		dz = pos->z_pos - camera.mike_pos.z;

		if (dx < -radius || dx > radius || dy < -radius || dy > radius || dz < -radius || dz > radius)
			return 0;

		distance = SQUARE(dx) + SQUARE(dy) + SQUARE(dz);

		if ((long)distance > SQUARE(radius))
			return 0;

		if (distance >= 0x100000)
			distance = phd_sqrt(distance) - 1024;
		else
			distance = 0;

		if (!(info->flags & 0x1000))
			pan = phd_atan(dz, dx) - camera.actual_angle;
	}
	else
		distance = 0;

	volume = info->volume << 7;

	if (info->flags & 0x4000)
		volume -= (0x1000 * GetRandomDraw()) >> 15;

	vol = SQUARE(distance) / (SQUARE(radius) / 0x10000);
	volume = (volume * (0x10000 - vol)) >> 16;

	if (volume <= 0)
		return 0;

	if (volume > 0x7FFF)
		volume = 0x7FFF;

	if (flags & SFX_SETPITCH)
		pitch = (flags >> 8) & 0xFFFFFF;
	else
		pitch = 0x10000;

	pitch += info->pitch << 9;

	if (info->flags & 0x2000)
		pitch += ((6000 * GetRandomDraw()) >> 14) - 6000;

	if (info->number < 0)
		return 0;

	rnd = (info->flags >> 2) & 0xF;

	if (rnd == 1)
		sample = info->number;
	else
		sample = info->number + ((rnd * GetRandomDraw()) >> 15);

	flag = info->flags & 3;

	switch (flag)
	{
	case 1:

		for (int i = 0; i < 32; i++)
		{
			if (LaSlot[i].nSampleInfo == lut)
			{
				if (S_SoundSampleIsPlaying(i))
					return 0;

				LaSlot[i].nSampleInfo = -1;
			}
		}

		break;

	case 2:

		for (int i = 0; i < 32; i++)
		{
			if (LaSlot[i].nSampleInfo == lut)
			{
				S_SoundStopSample(i);
				LaSlot[i].nSampleInfo = -1;
				break;
			}
		}

		break;

	case 3:

		for (int i = 0; i < 32; i++)
		{
			if (LaSlot[i].nSampleInfo == lut)
			{
				if (volume > LaSlot[i].nVolume)
				{
					LaSlot[i].nVolume = volume;
					LaSlot[i].nPan = pan;
					LaSlot[i].nPitch = pitch;
					return 1;
				}

				return 0;
			}
		}

		break;
	}

	if (flag == 3)
		dx = S_SoundPlaySampleLooped(sample, (ushort)volume, pitch, (short)pan);
	else
		dx = S_SoundPlaySample(sample, (ushort)volume, pitch, (short)pan);

	if (dx >= 0)
	{
		LaSlot[dx].nVolume = volume;
		LaSlot[dx].nPan = pan;
		LaSlot[dx].nPitch = pitch;
		LaSlot[dx].nSampleInfo = lut;
		return 1;
	}

	if (dx != -1)
	{
		info->number = -1;
		return 0;
	}

	vol = 0x8000;
	slot = -1;

	for (int i = 1; i < 32; i++)
	{
		if (LaSlot[i].nSampleInfo >= 0 && LaSlot[i].nVolume < vol)
		{
			vol = LaSlot[i].nVolume;
			slot = i;
		}
	}

	if (slot >= 0 && volume >= vol)
	{
		S_SoundStopSample(slot);
		LaSlot[slot].nSampleInfo = -1;

		if (flag == 3)
			dx = S_SoundPlaySampleLooped(sample, (ushort)volume, pitch, (short)pan);
		else
			dx = S_SoundPlaySample(sample, (ushort)volume, pitch, (short)pan);

		if (dx >= 0)
		{
			LaSlot[dx].nVolume = volume;
			LaSlot[dx].nPan = pan;
			LaSlot[dx].nPitch = pitch;
			LaSlot[dx].nSampleInfo = lut;
			return 1;
		}
	}

	return 0;
}
