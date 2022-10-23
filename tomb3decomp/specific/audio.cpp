#include "../tomb3/pch.h"
#include "audio.h"
#include "file.h"

//statics
#define hACMDriver	VAR_(0x006300BC, HACMDRIVER)
#define hACMDriverID	VAR_(0x0062FCA8, HACMDRIVERID)
#define hACMStream	VAR_(0x0062FD08, HACMSTREAM)
#define acm_file	VAR_(0x0062FD10, HANDLE)
#define acm_ready	VAR_(0x006300B0, bool)
#define acm_wait	VAR_(0x006300CC, bool)
#define acm_done	VAR_(0x0062731C, bool)
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
#define DSNotify	VAR_(0x006300C8, LPDIRECTSOUNDNOTIFY)
#define ADPCMBuffer	VAR_(0x006300B4, uchar*)
#define XATrack	VAR_(0x0062FCFC, long)
#define StreamSize	VAR_(0x0062FCA0, ulong)

#define TrackInfos	ARRAY_(0x00627480, TRACK_INFO, [130])
#define StreamHeaders	ARRAY_(0x00627320, ACMSTREAMHEADER, [4])
#define NotifyEventHandles	ARRAY_(0x00627478, HANDLE, [2])

static uchar* AllocBuffer;

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

void ThreadACMEmulateCDPlay(long track, long mode)
{
	if (!acm_ready)
		return;

	acm_wait = 1;
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

long ACMGetTrackLocation()
{
	return long((float(timeGetTime() - acm_start_time) / (float)CLOCKS_PER_SEC) * 60.0F);
}

void ACMSetVolume(long volume)
{
	acm_volume = long(float(volume * 1.5625F - 400.0F) * 6.0F);

	if (DSBuffer)
		DSBuffer->SetVolume(acm_volume);
}

long ACMHandleNotifications()
{
	uchar* write;
	ulong wait, bytes, flag, abytes;
	static long count;
	bool eot;
	volatile bool lostFocus = 0;

	while ((wait = WaitForMultipleObjects(2, NotifyEventHandles, 0, INFINITE)) != WAIT_FAILED)
	{
		if (acm_wait)
			continue;

		if (acm_done == 1)
			break;

		while (!App.bFocus)
			lostFocus = 1;

		if (lostFocus)
		{
			ThreadACMEmulateCDPlay(XATrack, acm_loop_track);
			lostFocus = 0;
			continue;
		}

		if (!wait)
		{
			if (!acm_eof)
			{
				eot = 0;
				bytes = 0x5800;
				flag = 4;

				if (acm_total_read + bytes > TrackInfos[XATrack].size)
				{
					flag = 0;
					bytes = TrackInfos[XATrack].size - acm_total_read;
					eot = 1;
				}

				ReadFile(acm_file, ADPCMBuffer, bytes, &acm_read, 0);
				acm_total_read += bytes;

				if (eot)
				{
					if (acm_loop_track)
					{
						SetFilePointer(acm_file, -(long)acm_total_read, 0, FILE_CURRENT);	//smart
						acm_total_read = 0;
					}
					else
						acm_eof = 1;
				}

				acm_locked = 1;
				DSBuffer->Lock(NextWriteOffset, NotifySize, (LPVOID*)&write, &abytes, 0, 0, 0);

				if (eot)
				{
					memset(write, 0, NotifySize);
					StreamHeaders[CurrentNotify].cbSrcLength = bytes;
				}

				acmStreamConvert(hACMStream, &StreamHeaders[CurrentNotify], flag);
				while (!(StreamHeaders[CurrentNotify].fdwStatus & ACMSTREAMHEADER_STATUSF_DONE));

				DSBuffer->Unlock(write, abytes, 0, 0);
				acm_locked = 0;

				if (eot)
					StreamHeaders[CurrentNotify].cbSrcLength = 0x5800;

				NextWriteOffset += abytes;

				if (NextWriteOffset >= audio_buffer_size)
					NextWriteOffset -= audio_buffer_size;
			}
			else
			{
				count++;
				acm_locked = 1;
				DSBuffer->Lock(NextWriteOffset, NotifySize, (LPVOID*)&write, &abytes, 0, 0, 0);
				memset(write, 0, NotifySize);
				NextWriteOffset += abytes;

				if (NextWriteOffset >= audio_buffer_size)
					NextWriteOffset -= audio_buffer_size;

				DSBuffer->Unlock(write, abytes, 0, 0);
				acm_locked = 0;

				if (count > 4)
				{
					count = 0;
					DSBuffer->Stop();

					if (CurrentAtmosphere)
					{
						ThreadACMEmulateCDPlay(CurrentAtmosphere, 1);
						continue;
					}
				}
				else
				{
					acm_locked = 1;
					DSBuffer->Lock(NextWriteOffset, NotifySize, (LPVOID*)&write, &abytes, 0, 0, 0);
					memset(write, 0, NotifySize);
					DSBuffer->Unlock(write, abytes, 0, 0);
					acm_locked = 0;
				}
			}

			CurrentNotify++;

			if (CurrentNotify > 3)
				CurrentNotify = 0;
		}
	}

	CloseHandle(NotifyEventHandles[0]);
	CloseHandle(NotifyEventHandles[1]);
	NotifyEventHandles[1] = 0;
	NotifyEventHandles[0] = 0;
	ExitThread(1);
	return 1;
}

long ACMSetupNotifications()
{
	DSBPOSITIONNOTIFY posNotif[5];
	static HANDLE handle;
	ulong ThreadId;
	long result;

	NotifyEventHandles[0] = CreateEvent(0, 0, 0, 0);
	NotifyEventHandles[1] = CreateEvent(0, 0, 0, 0);
	posNotif[0].dwOffset = NotifySize;
	posNotif[0].hEventNotify = NotifyEventHandles[0];

	for (int i = 1; i < 4; i++)
	{
		posNotif[i].dwOffset = NotifySize + posNotif[i - 1].dwOffset;
		posNotif[i].hEventNotify = NotifyEventHandles[0];
	}

	posNotif[3].dwOffset--;
	posNotif[4].dwOffset = -1;
	posNotif[4].hEventNotify = NotifyEventHandles[1];
	handle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ACMHandleNotifications, 0, 0, &ThreadId);
	result = DSNotify->SetNotificationPositions(5, posNotif);

	if (result != DS_OK)
	{
		CloseHandle(NotifyEventHandles[0]);
		CloseHandle(NotifyEventHandles[1]);
		NotifyEventHandles[1] = 0;
		NotifyEventHandles[0] = 0;
	}

	return result;
}

bool ACMInit()
{
	DSBUFFERDESC desc;
	static WAVEFORMATEX wav_format;
	ulong pMetric;
	long nEmpty;
	char wadname[80];
	static char source_wav_format[50];

	acm_ready = 0;

	if (!App.DXConfig.sound || !lpDirectSound)
		return 0;

	acmDriverEnum(ACMEnumCallBack, 0, 0);

	if (!hACMDriverID)
		return 0;

	if (acmDriverOpen(&hACMDriver, hACMDriverID, 0))
		return 0;

	strcpy(wadname, "audio\\cdaudio.wad");
	memset(&wadname[18], 0, 80 - 18);

	if (!ACMOpenFile(wadname))
		ACMOpenFile(GetFullPath(wadname));

	AllocBuffer = (uchar*)GlobalAlloc(GMEM_FIXED, 0x16040);
	ADPCMBuffer = (uchar*)(((long)AllocBuffer + 32) & 0xFFFFFFE0);
	ReadFile(acm_file, TrackInfos, 130 * sizeof(TRACK_INFO), &acm_read, 0);
	nEmpty = 0;

	while (!TrackInfos[nEmpty].size)
		nEmpty++;

	SetFilePointer(acm_file, TrackInfos[nEmpty].offset, 0, FILE_BEGIN);
	SetFilePointer(acm_file, 20, 0, FILE_CURRENT);
	ReadFile(acm_file, &source_wav_format, 50, &acm_read, 0);
	wav_format.wFormatTag = WAVE_FORMAT_PCM;
	acmMetrics(0, ACM_METRIC_MAX_SIZE_FORMAT, &pMetric);
	acmFormatSuggest(hACMDriver, (LPWAVEFORMATEX)&source_wav_format, &wav_format, pMetric, ACM_FORMATSUGGESTF_WFORMATTAG);
	audio_buffer_size = 0x577C0;
	NotifySize = 0x15DF0;
	desc.dwBufferBytes = 0x577C0;
	desc.dwReserved = 0;
	desc.dwSize = sizeof(DSBUFFERDESC);
	desc.dwFlags = DSBCAPS_LOCSOFTWARE | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GETCURRENTPOSITION2;
	desc.lpwfxFormat = &wav_format;
	lpDirectSound->CreateSoundBuffer(&desc, &DSBuffer, 0);
	DSBuffer->QueryInterface(IID_IDirectSoundNotify, (LPVOID*)&DSNotify);

	ACMSetupNotifications();
	acmStreamOpen(&hACMStream, hACMDriver, (LPWAVEFORMATEX)&source_wav_format, &wav_format, 0, 0, 0, 0);

	acm_ready = 1;
	while (DSBuffer->Lock(0, audio_buffer_size, (LPVOID*)&pAudioWrite, &AudioBytes, 0, 0, 0) != DS_OK);

	acmStreamSize(hACMStream, 0x5800, &StreamSize, 0);
	memset(pAudioWrite, 0, audio_buffer_size);

	for (int i = 0; i < 4; i++)
	{
		memset(&StreamHeaders[i], 0, sizeof(ACMSTREAMHEADER));
		StreamHeaders[i].cbStruct = sizeof(ACMSTREAMHEADER);
		StreamHeaders[i].pbSrc = ADPCMBuffer;
		StreamHeaders[i].cbSrcLength = 0x5800;
		StreamHeaders[i].cbDstLength = StreamSize;
		StreamHeaders[i].pbDst = &pAudioWrite[NotifySize * i];
		acmStreamPrepareHeader(hACMStream, &StreamHeaders[i], 0);
	}

	DSBuffer->Unlock(pAudioWrite, audio_buffer_size, 0, 0);
	return 1;
}

void ACMClose()
{
	if (!acm_ready)
		return;

	acm_done = 1;

	if (DSBuffer)
	{
		DSBuffer->Play(0, 0, DSBPLAY_LOOPING);
		DSBuffer->Stop();
	}

	if (StreamHeaders[0].fdwStatus & ACMSTREAMHEADER_STATUSF_PREPARED)
	{
		StreamHeaders[0].cbSrcLength = 0x5800;
		StreamHeaders[0].cbDstLength = StreamSize;
		acmStreamUnprepareHeader(hACMStream, StreamHeaders, 0);
	}

	if (StreamHeaders[1].fdwStatus & ACMSTREAMHEADER_STATUSF_PREPARED)
	{
		StreamHeaders[0].cbSrcLength = 0x5800;
		StreamHeaders[0].cbDstLength = StreamSize;
		acmStreamUnprepareHeader(hACMStream, &StreamHeaders[1], 0);
	}

	if (StreamHeaders[2].fdwStatus & ACMSTREAMHEADER_STATUSF_PREPARED)
	{
		StreamHeaders[0].cbSrcLength = 0x5800;
		StreamHeaders[0].cbDstLength = StreamSize;
		acmStreamUnprepareHeader(hACMStream, &StreamHeaders[2], 0);
	}

	if (StreamHeaders[3].fdwStatus & ACMSTREAMHEADER_STATUSF_PREPARED)
	{
		StreamHeaders[0].cbSrcLength = 0x5800;
		StreamHeaders[0].cbDstLength = StreamSize;
		acmStreamUnprepareHeader(hACMStream, &StreamHeaders[3], 0);
	}

	acmStreamClose(hACMStream, 0);
	acmDriverClose(hACMDriver, 0);
	GlobalFree(AllocBuffer);

	if (DSNotify)
		DSNotify->Release();

	if (DSBuffer)
		DSBuffer->Release();

	ACMCloseFile();
}

void inject_audio(bool replace)
{
	INJECT(0x004742A0, ACMEnumCallBack, replace);
	INJECT(0x004748E0, ACMCloseFile, replace);
	INJECT(0x004748B0, ACMOpenFile, replace);
	INJECT(0x00474D50, ACMEmulateCDStop, replace);
	INJECT(0x00474900, ACMEmulateCDPlay, replace);
	INJECT(0x00474B30, ThreadACMEmulateCDPlay, replace);
	INJECT(0x00475240, ACMGetTrackLocation, replace);
	INJECT(0x00475280, ACMSetVolume, replace);
	INJECT(0x00474D70, ACMHandleNotifications, replace);
	INJECT(0x00475160, ACMSetupNotifications, replace);
	INJECT(0x00474330, ACMInit, replace);
	INJECT(0x00474760, ACMClose, replace);
}
