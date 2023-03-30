#pragma once
#include "../global/types.h"

long DrawPhaseCinematic();
void InitialiseGenPlayer(short item_number);
void InitCinematicRooms();
long GetCinematicRoom(long x, long y, long z);
void LaraControlCinematic(short item_number);
void InitialisePlayer1(short item_number);
void UpdateLaraGuns();
void CalculateCinematicCamera();
void InGameCinematicCamera();
void ControlCinematicPlayer(short item_number);
long DoCinematic(long nframes);
long StartCinematic(long level_number);

extern short* cine;
extern PHD_3DPOS cinematic_pos;
extern long cutscene_track;
extern short cine_loaded;
extern short cine_frame;
extern short num_cine_frames;
