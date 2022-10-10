#pragma once
#include "../global/vars.h"

bool OpenRegistry(LPCSTR SubKeyName);
void CloseRegistry();
void REG_WriteLong(char* SubKeyName, ulong value);
void REG_WriteBool(char* SubKeyName, bool value);
void REG_WriteBlock(char* SubKeyName, LPVOID block, long size);
bool REG_ReadLong(char* SubKeyName, ulong& value, ulong defaultValue);
bool REG_ReadBool(char* SubKeyName, bool& value, bool defaultValue);
bool REG_ReadBlock(char* SubKeyName, LPVOID block, long size, LPVOID dBlock);
bool REG_KeyWasCreated();

#define MAIN_KEY	"Software\\Core Design\\Tomb Raider III"
#define SUB_KEY		"tomb3"
