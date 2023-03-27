#pragma once
#include "../global/vars.h"

void inject_demo(bool replace);

void GetDemoInput();
long DoDemoSequence(long level);
void LoadLaraDemoPos();
long StartDemo(long level);

extern ulong* demoptr;
extern long democount;
extern long demo_loaded;
extern long DemoPlay;
