#include "../tomb3/pch.h"
#include "audio.h"

//statics
#define hACMDriverID	VAR_(0x0062FCA8, HACMDRIVERID)

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

void inject_audio(bool replace)
{
	INJECT(0x004742A0, ACMEnumCallBack, replace);
}
