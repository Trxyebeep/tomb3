#include "../tomb3/pch.h"
#include "audio.h"

//statics
#define hACMDriverID	VAR_(0x0062FCA8, HACMDRIVERID)
#define acm_file	VAR_(0x0062FD10, HANDLE)
#define acm_ready	VAR_(0x006300B0, bool)
#define DSBuffer	VAR_(0x006300C4, LPDIRECTSOUNDBUFFER)

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

void inject_audio(bool replace)
{
	INJECT(0x004742A0, ACMEnumCallBack, replace);
	INJECT(0x004748E0, ACMCloseFile, replace);
	INJECT(0x004748B0, ACMOpenFile, replace);
	INJECT(0x00474D50, ACMEmulateCDStop, replace);
}
