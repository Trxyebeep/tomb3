#pragma once
#include "../global/types.h"

bool OpenRegistry(LPCSTR SubKeyName);
void CloseRegistry();
void REG_WriteLong(char* SubKeyName, ulong value);
void REG_WriteBool(char* SubKeyName, bool value);
void REG_WriteBlock(char* SubKeyName, LPVOID block, long size);
void REG_WriteString(char* SubKeyName, char* string, long length);
void REG_WriteFloat(char* SubKeyName, float value);
bool REG_ReadLong(char* SubKeyName, ulong& value, ulong defaultValue);
bool REG_ReadBool(char* SubKeyName, bool& value, bool defaultValue);
bool REG_ReadBlock(char* SubKeyName, LPVOID block, long size, LPVOID dBlock);
bool REG_ReadString(char* SubKeyName, char* value, long length, char* defaultValue);
bool REG_ReadFloat(char* SubKeyName, float& value, float defaultValue);
bool REG_KeyWasCreated();

#define MAIN_KEY	"Software\\Core Design\\Tomb Raider III"
#define SUB_KEY		"tomb3"
