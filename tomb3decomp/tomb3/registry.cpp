#include "../tomb3/pch.h"
#include "registry.h"

static HKEY phkResult;
static DWORD dwDisposition;

bool REG_OpenKey(LPCSTR lpSubKey)
{
	return RegCreateKeyEx(HKEY_CURRENT_USER, lpSubKey, 0, (CHAR*)"", REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0, &phkResult, &dwDisposition) == ERROR_SUCCESS;
}

bool OpenRegistry(LPCSTR SubKeyName)
{
	char buffer[256];

	if (!SubKeyName)
	{
		if (!REG_OpenKey(MAIN_KEY))
			return 0;

		return !REG_KeyWasCreated();
	}

	sprintf(buffer, "%s\\%s", MAIN_KEY, SubKeyName);

	if (!REG_OpenKey(buffer))
		return 0;

	return !REG_KeyWasCreated();
}

void REG_CloseKey()
{
	RegCloseKey(phkResult);
}

void CloseRegistry()
{
	REG_CloseKey();
}

void REG_WriteLong(char* SubKeyName, ulong value)
{
	RegSetValueEx(phkResult, SubKeyName, 0, REG_DWORD, (CONST BYTE*)&value, sizeof(ulong));
}

void REG_WriteBool(char* SubKeyName, bool value)
{
	ulong Lvalue;

	Lvalue = (ulong)value;
	RegSetValueEx(phkResult, SubKeyName, 0, REG_DWORD, (CONST BYTE*)&Lvalue, sizeof(ulong));
}

void REG_WriteBlock(char* SubKeyName, LPVOID block, long size)
{
	if (block)
		RegSetValueEx(phkResult, SubKeyName, 0, REG_BINARY, (const BYTE*)block, size);
	else
		RegDeleteValue(phkResult, SubKeyName);
}

bool REG_ReadLong(char* SubKeyName, ulong& value, ulong defaultValue)
{
	ulong type;
	ulong cbData;

	cbData = 4;

	if (RegQueryValueEx(phkResult, SubKeyName, 0, &type, (LPBYTE)&value, &cbData) == ERROR_SUCCESS && type == REG_DWORD && cbData == 4)
		return 1;

	REG_WriteLong(SubKeyName, defaultValue);
	value = defaultValue;
	return 0;
}

bool REG_ReadBool(char* SubKeyName, bool& value, bool defaultValue)
{
	ulong type;
	ulong cbData;
	ulong data;

	cbData = 4;

	if (RegQueryValueEx(phkResult, SubKeyName, 0, &type, (LPBYTE)&data, &cbData) == ERROR_SUCCESS && type == REG_DWORD && cbData == 4)
	{
		value = (bool)data;
		return 1;
	}

	REG_WriteBool(SubKeyName, defaultValue);
	value = defaultValue;
	return 0;
}

bool REG_ReadBlock(char* SubKeyName, LPVOID block, long size, LPVOID dBlock)
{
	ulong type;
	ulong cbData;

	cbData = size;

	if (RegQueryValueEx(phkResult, SubKeyName, 0, &type, (LPBYTE)block, &cbData) == ERROR_SUCCESS && type == REG_BINARY && size == cbData)
		return 1;

	if (dBlock)
		REG_WriteBlock(SubKeyName, block, size);
	else
		RegDeleteValue(phkResult, SubKeyName);

	return 0;
}

bool REG_KeyWasCreated()
{
	return dwDisposition == REG_CREATED_NEW_KEY;
}
