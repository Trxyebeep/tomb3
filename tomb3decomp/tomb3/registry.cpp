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

void REG_WriteString(char* SubKeyName, char* string, long length)
{
	long checkLength;

	if (string)
	{
		if (length < 0)
			checkLength = strlen(string);
		else
			checkLength = length;

		RegSetValueEx(phkResult, SubKeyName, 0, REG_SZ, (CONST BYTE*)string, checkLength + 1);
	}
	else
		RegDeleteValue(phkResult, SubKeyName);
}

void REG_WriteFloat(char* SubKeyName, float value)
{
	long length;
	char buf[64];

	length = sprintf(buf, "%.5f", value);
	REG_WriteString(SubKeyName, buf, length);
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

bool REG_ReadString(char* SubKeyName, char* value, long length, char* defaultValue)
{
	ulong type;
	ulong cbData;
	long len;

	cbData = length;

	if (RegQueryValueEx(phkResult, SubKeyName, 0, &type, (LPBYTE)value, (LPDWORD)&cbData) == ERROR_SUCCESS && type == REG_SZ)
		return 1;

	if (defaultValue)
	{
		REG_WriteString(SubKeyName, defaultValue, -1);
		len = strlen(defaultValue) + 1;

		if (len > length)
		{
			len = length - 1;
			value[len] = 0;
		}

		memcpy(value, defaultValue, len);
	}
	else
		RegDeleteValue(phkResult, SubKeyName);

	return 0;
}

bool REG_ReadFloat(char* SubKeyName, float& value, float defaultValue)
{
	char buf[64];

	if (REG_ReadString(SubKeyName, buf, sizeof(buf), 0))
	{
		value = (float)atof(buf);
		return 1;
	}

	REG_WriteFloat(SubKeyName, defaultValue);
	value = defaultValue;
	return 0;
}

bool REG_KeyWasCreated()
{
	return dwDisposition == REG_CREATED_NEW_KEY;
}
