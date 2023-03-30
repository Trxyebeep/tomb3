#pragma once
#include "../global/types.h"

void GetDemoInput();
long DoDemoSequence(long level);
void LoadLaraDemoPos();
long StartDemo(long level);

extern ulong* demoptr;
extern long democount;
extern long demo_loaded;
extern long DemoPlay;
