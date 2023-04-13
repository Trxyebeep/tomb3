#pragma once
#include "../global/types.h"

long CalcVolume(long volume);
long CalcPan(short angle);
void S_SoundSetMasterVolume(long volume);
long S_SoundPlaySample(long num, ushort volume, long pitch, short pan);
long S_SoundPlaySampleLooped(long num, ushort volume, long pitch, short pan);
void S_SoundSetPanAndVolume(long num, short angle, ushort volume);
void S_SoundSetPitch(long num, long pitch);
void S_SoundStopSample(long num);
void S_SoundStopAllSamples();
long S_SoundSampleIsPlaying(long num);
void S_CDPlay(short track, long mode);
void S_CDStop();
long S_CDGetLoc();
void S_CDMute();
void S_CDVolume(long volume);
long S_StartSyncedAudio(long track);
