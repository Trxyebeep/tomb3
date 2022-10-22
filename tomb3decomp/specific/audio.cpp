#include "../tomb3/pch.h"
#include "audio.h"

//statics
#define hACMDriverID	VAR_(0x0062FCA8, HACMDRIVERID)
#define hACMStream	VAR_(0x0062FD08, HACMSTREAM)
#define acm_file	VAR_(0x0062FD10, HANDLE)
#define acm_ready	VAR_(0x006300B0, bool)
#define acm_wait	VAR_(0x006300CC, bool)
#define acm_loop_track	VAR_(0x0062FCA4, bool)
#define acm_eof	VAR_(0x0062FD0C, bool)
#define acm_locked	VAR_(0x006300D4, volatile bool)
#define acm_read	VAR_(0x0062FC9C, ulong)
#define acm_total_read	VAR_(0x006300B8, ulong)
#define acm_start_time	VAR_(0x00627314, ulong)
#define acm_volume	VAR_(0x00627470, long)
#define audio_buffer_size	VAR_(0x006300C0, long)
#define CurrentNotify	VAR_(0x00627318, long)
#define NotifySize	VAR_(0x0062FCB0, long)
#define NextWriteOffset	VAR_(0x0062FCB8, long)
#define pAudioWrite	VAR_(0x0062FCC0, uchar*)
#define AudioBytes	VAR_(0x0062FD00, ulong)
#define DSBuffer	VAR_(0x006300C4, LPDIRECTSOUNDBUFFER)
#define ADPCMBuffer	VAR_(0x006300B4, uchar*)
#define XATrack	VAR_(0x0062FCFC, long)

#define TrackInfos	ARRAY_(0x00627480, TRACK_INFO, [130])
#define StreamHeaders	ARRAY_(0x00627320, ACMSTREAMHEADER, [4])

BOOL __stdcall ACMEnumCallBack(HACMDRIVERID hadid, DWORD_PTR dwInstance, DWORD fdwSupport)
{
	ACMDRIVERDETAILS driver;

	memset(&driver, 0, sizeof(driver));
	driver.cbStruct = sizeof(ACMDRIVERDETAILS);
	acmDriverDetails(hadid, &driver, 0);

	if (strcmp(driver.szShortName, "MS-ADPCM"))
		return 1;

	hACMDriverID = hadid;
	return 0;
}

void ACMCloseFile()
{
	if (acm_file)
	{
		CloseHandle(acm_file);
		acm_file = 0;
	}
}

bool ACMOpenFile(const char* name)
{
	ACMCloseFile();
	acm_file = CreateFile(name, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, 0);
	return acm_file != INVALID_HANDLE_VALUE;
}

void ACMEmulateCDStop()
{
	if (acm_ready && DSBuffer)
		DSBuffer->Stop();
}

void ACMEmulateCDPlay(long track, long mode)
{
	if (!acm_ready)
		return;

	acm_wait = 1;
	while (acm_locked);

	ACMEmulateCDStop();

	if (!DSBuffer || track > 130 || !TrackInfos[track].size)
		return;

	SetFilePointer(acm_file, TrackInfos[track].offset + 90, 0, FILE_BEGIN);
	XATrack = track;

	ReadFile(acm_file, ADPCMBuffer, 0x5800, &acm_read, 0);
	acm_total_read = acm_read;
	DSBuffer->SetVolume(acm_volume);
	DSBuffer->Lock(0, audio_buffer_size, (LPVOID*)&pAudioWrite, &AudioBytes, 0, 0, 0);
	acmStreamConvert(hACMStream, &StreamHeaders[0], ACM_STREAMCONVERTF_BLOCKALIGN | ACM_STREAMCONVERTF_START);
	while (!(StreamHeaders[0].fdwStatus & ACMSTREAMHEADER_STATUSF_DONE));

	ReadFile(acm_file, ADPCMBuffer, 0x5800, &acm_read, 0);
	acm_total_read += acm_read;
	acmStreamConvert(hACMStream, &StreamHeaders[1], ACM_STREAMCONVERTF_BLOCKALIGN);
	while (!(StreamHeaders[1].fdwStatus & ACMSTREAMHEADER_STATUSF_DONE));

	DSBuffer->Unlock(pAudioWrite, audio_buffer_size, 0, 0);

	if (DSBuffer)
	{
		acm_start_time = timeGetTime();
		DSBuffer->Stop();
		DSBuffer->SetCurrentPosition(0);
		DSBuffer->SetVolume(acm_volume);
		DSBuffer->Play(0, 0, 1);
	}

	CurrentNotify = 2;
	NextWriteOffset = 2 * NotifySize;
	acm_loop_track = mode != 0;

	if (acm_total_read > TrackInfos[track].size)
	{
		acm_total_read = TrackInfos[track].size;
		acm_eof = 1;
	}
	else
		acm_eof = 0;

	acm_wait = 0;
}

void inject_audio(bool replace)
{
	INJECT(0x004742A0, ACMEnumCallBack, replace);
	INJECT(0x004748E0, ACMCloseFile, replace);
	INJECT(0x004748B0, ACMOpenFile, replace);
	INJECT(0x00474D50, ACMEmulateCDStop, replace);
	INJECT(0x00474900, ACMEmulateCDPlay, replace);
}
