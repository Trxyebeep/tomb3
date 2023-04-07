#pragma once
#include "../global/types.h"

void ShutdownGame();
void CalculateWibbleTable();
ushort GetRandom(WATERTAB* wt, long lp);
void init_water_table();
void init_game_malloc();
void* game_malloc(long size, long type);
void game_free(long size, long type);
long S_InitialiseSystem();

extern char* malloc_ptr;
extern char* malloc_buffer;

extern D3DTLVERTEX* CurrentTLVertex;
extern D3DTLVERTEX* VertexBuffer;
extern D3DTLVERTEX* UnRollBuffer;

extern long DynamicColorTable[33][33][256];
extern WATERTAB WaterTable[22][64];
extern float wibble_table[32];
