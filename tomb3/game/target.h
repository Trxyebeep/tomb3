#pragma once
#include "../global/vars.h"

void inject_target(bool replace);

void InitialiseTarget(short item_number);

#define ResetTargets	( (void(__cdecl*)()) 0x0046AC00 )
