#pragma once
#include "../global/vars.h"

void inject_init(bool replace);

void ShutdownGame();
void CalculateWibbleTable();
ushort GetRandom(WATERTAB* wt, long lp);
void init_water_table();
void init_game_malloc();
void* game_malloc(long size, long type);
void game_free(long size, long type);
long S_InitialiseSystem();

extern D3DTLVERTEX* CurrentTLVertex;
extern D3DTLVERTEX* VertexBuffer;
extern D3DTLVERTEX* UnRollBuffer;

extern long RColorTable[33][33][33];
extern long GColorTable[33][33][33];
extern long BColorTable[33][33][33];
extern WATERTAB WaterTable[22][64];
extern float wibble_table[32];
