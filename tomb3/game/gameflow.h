#pragma once
#include "../global/types.h"

long GF_LoadScriptFile(const char* name);
long GF_DoFrontEndSequence();
long GF_DoLevelSequence(long level, long type);
void GF_ModifyInventory(long level, long type);
long GF_InterpretSequence(short* ptr, long type, long seq_type);

enum game_levels
{
	LV_GYM,
	LV_FIRSTLEVEL,
	LV_JUNGLE = 1,
	LV_TEMPLE,
	LV_QUADBIKE,
	LV_INDIABOSS,
	LV_SHORE,
	LV_CRASH,
	LV_RAPIDS,
	LV_PACBOSS,
	LV_ROOFTOPS,
	LV_SEWER,
	LV_TOWER,
	LV_OFFICE,
	LV_DESERT,
	LV_COMPOUND,
	LV_AREA51,
	LV_ANTARC,
	LV_MINES,
	LV_CITY,
	LV_CHAMBER,
	LV_STPAULS
};

enum pc_string_ids
{
	PCSTR_DETAILLEVEL,
	PCSTR_DEMOMODE,
	PCSTR_SOUND,
	PCSTR_CONTROLS,
	PCSTR_GAMMA,
	PCSTR_SETVOLUME,
	PCSTR_USERKEYS,
	PCSTR_SAVEMESS1,
	PCSTR_SAVEMESS2,
	PCSTR_SAVEMESS3,
	PCSTR_SAVEMESS4,
	PCSTR_SAVEMESS5,
	PCSTR_SAVEMESS6,
	PCSTR_SAVEMESS7,
	PCSTR_SAVEMESS8,
	PCSTR_SAVESLOT,
	PCSTR_OFF,
	PCSTR_ON,
	PCSTR_SETUPSOUND,
	PCSTR_DEFAULTKEYS,
	PCSTR_DOZY_STRING,
	PCSTR_VIDEOTITLE,
	PCSTR_RESOLUTION,
	PCSTR_ZBUFFER,
	PCSTR_FILTERING,
	PCSTR_DITHER,
	PCSTR_TRUEALPHA,
	PCSTR_SKY,
	PCSTR_SPARE8,
	PCSTR_SPARE9,
	PCSTR_SPARE10,
	PCSTR_SPARE11,
	PCSTR_SPARE12,
	PCSTR_SPARE13,
	PCSTR_SPARE14,
	PCSTR_SPARE15,
	PCSTR_SPARE16,
	PCSTR_SPARE17,
	PCSTR_SPARE18,
	PCSTR_SPARE19,
	PCSTR_SPARE20,
	PCSTR_NUM_STRINGS
};

enum game_string_ids
{
	GT_MAIN_HEADING,
	GT_OPTION_HEADING,
	GT_KEYS_HEADING,
	GT_GAMEOVER_HEADING,
	GT_LOADGAME,
	GT_SAVEGAME,
	GT_STARTGAME,
	GT_RESTARTLEVEL,
	GT_EXIT2TITLE,
	GT_EXITDEMO,
	GT_EXITGAME,
	GT_SELECTLEVEL,
	GT_SAVEPOSITION,
	GT_DETAIL,
	GT_HIGH_DETAIL,
	GT_MEDIUM_DETAIL,
	GT_LOW_DETAIL,
	GT_WALK,
	GT_ROLL,
	GT_RUN,
	GT_LEFT,
	GT_RIGHT,
	GT_BACK,
	GT_STEPLEFT1,
	GT_STEPLEFT2,
	GT_STEPRIGHT1,
	GT_STEPRIGHT2,
	GT_LOOK,
	GT_JUMP,
	GT_ACTION,
	GT_DRAWWEAPON1,
	GT_DRAWWEAPON2,
	GT_INVENTORY,
	GT_USEFLARE,
	GT_STEPSHIFT,
	GT_STOPWATCH,
	GT_PISTOLS,
	GT_SHOTGUN,
	GT_AUTOPISTOLS,
	GT_UZIS,
	GT_HARPOON,
	GT_M16,
	GT_ROCKETLAUNCHER,
	GT_GRENADELAUNCHER,
	GT_FLARE,
	GT_PISTOLCLIPS,
	GT_SHOTGUNSHELLS,
	GT_AUTOPISTOLCLIPS,
	GT_UZICLIPS,
	GT_HARPOONBOLTS,
	GT_M16CLIPS,
	GT_ROCKETS,
	GT_GRENADES,
	GT_SMALLMEDI,
	GT_LARGEMEDI,
	GT_PICKUP,
	GT_PUZZLE,
	GT_KEY,
	GT_GAME,
	GT_GYM,
	GT_LOADING,
	GT_STAT_TIME,
	GT_STAT_SECRETS,
	GT_STAT_LOCATION,
	GT_STAT_KILLS,
	GT_STAT_AMMO,
	GT_STAT_RATIO,
	GT_STAT_SAVES,
	GT_STAT_DISTANCE,
	GT_STAT_HEALTH,
	GT_SECURITY_TAG,
	GT_NONE,
	GT_FINISH,
	GT_BESTTIMES,
	GT_NOTIMES,
	GT_NOTAVAILABLE,
	GT_CURRENTPOS,
	GT_GAMESTATS,
	GT_OF,
	GT_STORY_SO_FAR,
	GT_ICON1,
	GT_ICON2,
	GT_ICON3,
	GT_ICON4,
	GT_CRYSTAL,
	GT_LSLONDON,
	GT_LSNEVADA,
	GT_LSSPAC,
	GT_LSANTARC,
	GT_LSPERU,
	GT_LEVELSELECT,
	GT_SPACE,
	GT_NUM_GAMESTRINGS
};

enum add_inv_types
{
	ADDINV_PISTOLS,
	ADDINV_SHOTGUN,
	ADDINV_AUTOPISTOLS,
	ADDINV_UZIS,
	ADDINV_HARPOON,
	ADDINV_M16,
	ADDINV_ROCKET,
	ADDINV_GRENADE,
	ADDINV_PISTOL_AMMO,
	ADDINV_SHOTGUN_AMMO,
	ADDINV_AUTOPISTOLS_AMMO,
	ADDINV_UZI_AMMO,
	ADDINV_HARPOON_AMMO,
	ADDINV_M16_AMMO,
	ADDINV_ROCKET_AMMO,
	ADDINV_GRENADE_AMMO,
	ADDINV_FLARES,
	ADDINV_MEDI,
	ADDINV_BIGMEDI,
	ADDINV_PICKUP1,
	ADDINV_PICKUP2,
	ADDINV_PUZZLE1,
	ADDINV_PUZZLE2,
	ADDINV_PUZZLE3,
	ADDINV_PUZZLE4,
	ADDINV_KEY1,
	ADDINV_KEY2,
	ADDINV_KEY3,
	ADDINV_KEY4,
	ADDINV_SAVEGAME_CRYSTAL,
	ADDINV_NUMBEROF
};

enum gf_event_types
{
	GFE_PICTURE,
	GFE_LIST_START,
	GFE_LIST_END,
	GFE_PLAYFMV,
	GFE_STARTLEVEL,
	GFE_CUTSCENE,
	GFE_LEVCOMPLETE,
	GFE_DEMOPLAY,
	GFE_JUMPTO_SEQ,
	GFE_END_SEQ,
	GFE_SETTRACK,
	GFE_SUNSET,
	GFE_LOADINGPIC,
	GFE_DEADLY_WATER,
	GFE_REMOVE_WEAPONS,
	GFE_GAMECOMPLETE,
	GFE_CUTANGLE,
	GFE_NOFLOOR,
	GFE_ADD2INV,
	GFE_STARTANIM,
	GFE_NUMSECRETS,
	GFE_KILL2COMPLETE,
	GFE_REMOVE_AMMO,
	//NEW EVENTS:
	GFE_RAIN,
	GFE_SNOW,
	GFE_WATER_PARTS,
	GFE_COLD,
	GFE_DEATHTILE,
	GFE_WATERCLR,
};

extern GAMEFLOW_INFO gameflow;

extern short* GF_level_sequence_list[24];
extern short GF_Offsets[200];
extern short GF_valid_demos[24];
extern short GF_CDtracks[16];
extern char GF_Description[256];
extern char GF_Add2InvItems[30];
extern char GF_SecretInvItems[30];
extern char** GF_picfilenames;
extern char** GF_fmvfilenames;
extern char** GF_titlefilenames;
extern char** GF_levelfilenames;
extern char** GF_cutscenefilenames;
extern char** GF_Level_Names;
extern char** GF_GameStrings;
extern char** GF_PCStrings;
extern char** GF_Pickup1Strings;
extern char** GF_Pickup2Strings;
extern char** GF_Puzzle1Strings;
extern char** GF_Puzzle2Strings;
extern char** GF_Puzzle3Strings;
extern char** GF_Puzzle4Strings;
extern char** GF_Key1Strings;
extern char** GF_Key2Strings;
extern char** GF_Key3Strings;
extern char** GF_Key4Strings;
extern char* GF_picfilenames_buffer;
extern char* GF_fmvfilenames_buffer;
extern char* GF_titlefilenames_buffer;
extern char* GF_levelfilenames_buffer;
extern char* GF_cutscenefilenames_buffer;
extern char* GF_levelnames_buffer;
extern char* GF_GameStrings_buffer;
extern char* GF_PCStrings_buffer;
extern char* GF_Pickup1Strings_buffer;
extern char* GF_Pickup2Strings_buffer;
extern char* GF_Puzzle1Strings_buffer;
extern char* GF_Puzzle2Strings_buffer;
extern char* GF_Puzzle3Strings_buffer;
extern char* GF_Puzzle4Strings_buffer;
extern char* GF_Key1Strings_buffer;
extern char* GF_Key2Strings_buffer;
extern char* GF_Key3Strings_buffer;
extern char* GF_Key4Strings_buffer;
extern short* GF_sequence_buffer;
extern short* GF_frontendSequence;
extern long GF_ScriptVersion;
extern long GF_BonusLevelEnabled;
extern long GF_PlayingFMV;
extern long GF_LaraStartAnim;
extern ushort GF_Cutscene_Orientation;
extern short GF_LoadingPic;
extern short GF_NoFloor;
extern short GF_DeadlyWater;
extern short GF_SunsetEnabled;
extern short GF_RemoveWeapons;
extern short GF_NumSecrets;
extern char GF_Rain;
extern char GF_Snow;
extern char GF_WaterParts;
extern char GF_Cold;
extern short GF_DeathTile;
extern long GF_WaterColor;
extern short GF_RemoveAmmo;
extern char GF_StartGame;
extern char GF_Kill2Complete;
extern char GF_Playing_Story;

extern short NextAdventure;
