#pragma once

#pragma pack(push, 1)

/*typedefs*/
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;

/*Injection macro, originally by Arsunt, modified by ChocolateFan to allow deinjection*/
struct JMP
{
	uchar opCode;	// must be 0xE9;
	ulong offset;	// jump offset
};

#define INJECT(from,to,replace) \
do \
{ \
	if (replace) \
		INJECT_JMP(from,to); \
	else \
		INJECT_JMP(to,from); \
} while (false)

#define INJECT_JMP(from,to) \
do \
{ \
	((JMP*)(from))->opCode = 0xE9; \
	((JMP*)(from))->offset = (DWORD)(to) - ((DWORD)(from) + sizeof(JMP)); \
} while (false)

#define ABS(x) (((x)<0) ? (-(x)) : (x))
#define SQUARE(x) ((x)*(x))

/*enums*/
enum mood_type
{
	BORED_MOOD,
	ATTACK_MOOD,
	ESCAPE_MOOD,
	STALK_MOOD,
};

enum input_buttons
{
	IN_NONE = 0x0,
	IN_FORWARD = 0x1,
	IN_BACK = 0x2,
	IN_LEFT = 0x4,
	IN_RIGHT = 0x8,
	IN_JUMP = 0x10,
	IN_DRAW = 0x20,
	IN_ACTION = 0x40,
	IN_WALK = 0x80,
	IN_OPTION = 0x100,
	IN_LOOK = 0x200,
	IN_LSTEP = 0x400,
	IN_RSTEP = 0x800,
	IN_ROLL = 0x1000,
	IN_PAUSE = 0x2000,
	IN_A = 0x4000,
	IN_B = 0x8000,
	IN_C = 0x10000,
	IN_D = 0x20000,
	IN_E = 0x40000,
	IN_FLARE = 0x80000,
	IN_SELECT = 0x100000,
	IN_DESELECT = 0x200000,
	IN_SAVE = 0x400000,
	IN_LOAD = 0x800000,
	IN_STEPSHIFT = 0x1000000,
	IN_LOOKLEFT = 0x2000000,
	IN_LOOKRIGHT = 0x4000000,
	IN_LOOKFORWARD = 0x8000000,
	IN_LOOKBACK = 0x10000000,
	IN_DUCK = 0x20000000,
	IN_SPRINT = 0x40000000,
	IN_ALL = 0xFFFFFFFF
};

enum lara_gun_status
{
	LG_ARMLESS,
	LG_HANDSBUSY,
	LG_DRAW,
	LG_UNDRAW,
	LG_READY,
	LG_SPECIAL,
};

enum collision_types
{
	CT_NONE = 0x0,
	CT_FRONT = 0x1,
	CT_LEFT = 0x2,
	CT_RIGHT = 0x4,
	CT_TOP = 0x8,
	CT_TOP_FRONT = 0x10,
	CT_CLAMP = 0x20
};

enum room_flags
{
	ROOM_UNDERWATER = 0x1,
	ROOM_OUTSIDE = 0x8,
	ROOM_DYNAMIC_LIT = 0x10,
	ROOM_NOT_INSIDE = 0x20,
	ROOM_INSIDE = 0x40,
	ROOM_SWAMP = 0x80
};

enum lara_water_states
{
	LARA_ABOVEWATER,
	LARA_UNDERWATER,
	LARA_SURFACE,
	LARA_CHEAT,
	LARA_WADE
};

enum camera_type
{
	CHASE_CAMERA,
	FIXED_CAMERA,
	LOOK_CAMERA,
	COMBAT_CAMERA,
	CINEMATIC_CAMERA,
	HEAVY_CAMERA
};

enum height_types
{
	WALL,
	SMALL_SLOPE,
	BIG_SLOPE,
	DIAGONAL,
	SPLIT_TRI
};

enum quadrant_names
{
	NORTH,
	EAST,
	SOUTH,
	WEST
};

enum lara_gun_types
{
	LG_UNARMED,
	LG_PISTOLS,
	LG_MAGNUMS,
	LG_UZIS,
	LG_SHOTGUN,
	LG_M16,
	LG_ROCKET,
	LG_GRENADE,
	LG_HARPOON,
	LG_FLARE,
	LG_SKIDOO,
	NUM_WEAPONS
};

enum floor_types
{
	FLOOR_TYPE,
	DOOR_TYPE,
	TILT_TYPE,
	ROOF_TYPE,
	TRIGGER_TYPE,
	LAVA_TYPE,
	CLIMB_TYPE,
	SPLIT1,
	SPLIT2,
	SPLIT3,
	SPLIT4,
	NOCOLF1T,
	NOCOLF1B,
	NOCOLF2T,
	NOCOLF2B,
	NOCOLC1T,
	NOCOLC1B,
	NOCOLC2T,
	NOCOLC2B,
	MONKEY_TYPE,
	MINEL_TYPE,
	MINER_TYPE
};

enum trigger_types
{
	TRIGGER,
	PAD,
	SWITCH,
	KEY,
	PICKUP,
	HEAVY,
	ANTIPAD,
	COMBAT,
	DUMMY,
	ANTITRIGGER
};

enum trigobj_types
{
	TO_OBJECT,
	TO_CAMERA,
	TO_SINK,
	TO_FLIPMAP,
	TO_FLIPON,
	TO_FLIPOFF,
	TO_TARGET,
	TO_FINISH,
	TO_CD,
	TO_FLIPEFFECT,
	TO_SECRET,
	TO_BODYBAG
};

enum lara_meshes
{
	HIPS,
	THIGH_L,
	CALF_L,
	FOOT_L,
	THIGH_R,
	CALF_R,
	FOOT_R,
	TORSO,
	UARM_R,
	LARM_R,
	HAND_R,
	UARM_L,
	LARM_L,
	HAND_L,
	HEAD
};

enum lara_get_meshes
{
	LEFT_HAND,
	RIGHT_HAND,
	LARA_HIPS,
	LARA_TORSO,
	LARA_HEAD,
	LEFT_FOOT,
	RIGHT_FOOT
};

enum sort_type
{
	MID_SORT,
	FAR_SORT,
	BACK_SORT
};

enum matrix_indices
{
	M00, M01, M02, M03,
	M10, M11, M12, M13,
	M20, M21, M22, M23,

	indices_count
};

/*structs*/
struct PHD_VECTOR
{
	long x;
	long y;
	long z;
};

struct PHD_3DPOS
{
	long x_pos;
	long y_pos;
	long z_pos;
	short x_rot;
	short y_rot;
	short z_rot;
};

struct GAME_VECTOR
{
	long x;
	long y;
	long z;
	short room_number;
	short box_number;
};

struct SPHERE
{
	long x;
	long y;
	long z;
	long r;
};

struct OBJECT_VECTOR
{
	long x;
	long y;
	long z;
	short data;
	short flags;
};

struct FX_INFO
{
	PHD_3DPOS pos;
	short room_number;
	short object_number;
	short next_fx;
	short next_active;
	short speed;
	short fallspeed;
	short frame_number;
	short counter;
	short shade;
	short flag1;
	short flag2;
};

struct LARA_ARM
{
	short* frame_base;
	short frame_number;
	short anim_number;
	short lock;
	short y_rot;
	short x_rot;
	short z_rot;
	short flash_gun;
};

struct ITEM_LIGHT
{
	PHD_VECTOR sun;
	PHD_VECTOR bulb;
	PHD_VECTOR dynamic;
	uchar sunr;
	uchar sung;
	uchar sunb;
	char init;
	uchar bulbr;
	uchar bulbg;
	uchar bulbb;
	uchar ambient;
	uchar dynamicr;
	uchar dynamicg;
	uchar dynamicb;
	uchar pad2;
};

struct ITEM_INFO
{
	long floor;
	ulong touch_bits;
	ulong mesh_bits;
	short object_number;
	short current_anim_state;
	short goal_anim_state;
	short required_anim_state;
	short anim_number;
	short frame_number;
	short room_number;
	short next_item;
	short next_active;
	short speed;
	short fallspeed;
	short hit_points;
	ushort box_number;
	short timer;
	short flags;
	short shade;
	short shadeB;
	short carried_item;
	short after_death;
	ushort fired_weapon;
	short item_flags[4];
	void* data;
	PHD_3DPOS pos;
	ITEM_LIGHT il;
	ushort active : 1;
	ushort status : 2;
	ushort gravity_status : 1;
	ushort hit_status : 1;
	ushort collidable : 1;
	ushort looked_at : 1;
	ushort dynamic_light : 1;
	ushort clear_body : 1;
	ushort ai_bits : 5;
	ushort really_active : 1;
};

struct box_node
{
	short exit_box;
	ushort search_number;
	short next_expansion;
	short box_number;
};

struct LOT_INFO
{
	box_node* node;
	short head;
	short tail;
	ushort search_number;
	ushort block_mask;
	short step;
	short drop;
	short fly;
	short zone_count;
	short target_box;
	short required_box;
	PHD_VECTOR target;
};

struct CREATURE_INFO
{
	short joint_rotation[4];
	short maximum_turn;
	short flags;
	ushort alerted : 1;
	ushort head_left : 1;
	ushort head_right : 1;
	ushort reached_goal : 1;
	ushort hurt_by_lara : 1;
	ushort patrol2 : 1;
	mood_type mood;
	short item_num;
	PHD_VECTOR target;
	ITEM_INFO* enemy;
	LOT_INFO LOT;
};

struct AMMO_INFO
{
	long ammo;
};

struct LARA_INFO
{
	short item_number;
	short gun_status;
	short gun_type;
	short request_gun_type;
	short last_gun_type;
	short calc_fallspeed;
	short water_status;
	short climb_status;
	short pose_count;
	short hit_frame;
	short hit_direction;
	short air;
	short dive_count;
	short death_count;
	short current_active;
	short current_xvel;
	short current_yvel;
	short current_zvel;
	short spaz_effect_count;
	short flare_age;
	short skidoo;
	short weapon_item;
	short back_gun;
	short flare_frame;
	short poisoned;
	short electric;
	ushort flare_control_left : 1;
	ushort flare_control_right : 1;
	ushort extra_anim : 1;
	ushort look : 1;
	ushort burn : 1;
	ushort keep_ducked : 1;
	ushort CanMonkeySwing : 1;
	ushort MineL : 1;
	ushort MineR : 1;
	ushort BurnGreen : 1;
	ushort IsDucked : 1;
	ushort has_fired : 1;
	long water_surface_dist;
	PHD_VECTOR last_pos;
	FX_INFO* spaz_effect;
	long mesh_effects;
	short* mesh_ptrs[15];
	ITEM_INFO* target;
	short target_angles[2];
	short turn_rate;
	short move_angle;
	short head_y_rot;
	short head_x_rot;
	short head_z_rot;
	short torso_y_rot;
	short torso_x_rot;
	short torso_z_rot;
	LARA_ARM left_arm;
	LARA_ARM right_arm;
	AMMO_INFO pistols;
	AMMO_INFO magnums;
	AMMO_INFO uzis;
	AMMO_INFO shotgun;
	AMMO_INFO harpoon;
	AMMO_INFO rocket;
	AMMO_INFO grenade;
	AMMO_INFO m16;
	CREATURE_INFO* creature;
};

struct COLL_INFO
{
	long mid_floor;
	long mid_ceiling;
	long mid_type;
	long front_floor;
	long front_ceiling;
	long front_type;
	long left_floor;
	long left_ceiling;
	long left_type;
	long right_floor;
	long right_ceiling;
	long right_type;
	long left_floor2;
	long left_ceiling2;
	long left_type2;
	long right_floor2;
	long right_ceiling2;
	long right_type2;
	long radius;
	long bad_pos;
	long bad_neg;
	long bad_ceiling;
	PHD_VECTOR shift;
	PHD_VECTOR old;
	short old_anim_state;
	short old_anim_number;
	short old_frame_number;
	short facing;
	short quadrant;
	short coll_type;
	short* trigger;
	char tilt_x;
	char tilt_z;
	char hit_by_baddie;
	char hit_static;
	ushort slopes_are_walls : 2;
	ushort slopes_are_pits : 1;
	ushort lava_is_pit : 1;
	ushort enable_baddie_push : 1;
	ushort enable_spaz : 1;
	ushort hit_ceiling : 1;
};

struct FLOOR_INFO
{
	ushort index;
	ushort fx : 4;
	ushort box : 11;
	ushort stopper : 1;
	uchar pit_room;
	char floor;
	uchar sky_room;
	char ceiling;
};

struct ANIM_STRUCT
{
	short* frame_ptr;
	short interpolation;
	short current_anim_state;
	long velocity;
	long acceleration;
	short frame_base;
	short frame_end;
	short jump_anim_num;
	short jump_frame_num;
	short number_changes;
	short change_index;
	short number_commands;
	short command_index;
};

struct WAKE_PTS
{
	long x[2];
	long y;
	long z[2];
	short xvel[2];
	short zvel[2];
	uchar life;
	uchar pad[3];
};

struct SUNLIGHT
{
	short nx;
	short ny;
	short nz;
	short pad;
};

struct SPOTLIGHT
{
	long intensity;
	long falloff;
};

struct LIGHT_INFO
{
	long x;
	long y;
	long z;
	uchar r;
	uchar g;
	uchar b;
	uchar type;

	union l
	{
		SUNLIGHT sun;
		SPOTLIGHT spot;
	};
};

struct MESH_INFO
{
	long x;
	long y;
	long z;
	short y_rot;
	short shade;
	short shadeB;
	short static_number;
};

struct ROOM_INFO
{
	short* data;
	short* door;
	FLOOR_INFO* floor;
	LIGHT_INFO* light;
	MESH_INFO* mesh;
	long x;
	long y;
	long z;
	long minfloor;
	long maxceiling;
	short x_size;
	short y_size;
	short ambient;
	short lighting;
	short num_lights;
	short num_meshes;
	short ReverbType;
	char MeshEffect;
	char bound_active;
	short left;
	short right;
	short top;
	short bottom;
	short test_left;
	short test_right;
	short test_top;
	short test_bottom;
	short item_number;
	short fx_number;
	short flipped_room;
	ushort flags;
};

struct CAMERA_INFO
{
	GAME_VECTOR pos;
	GAME_VECTOR target;
	camera_type type;
	camera_type old_type;
	long shift;
	long flags;
	long fixed_camera;
	long number_frames;
	long bounce;
	long underwater;
	long target_distance;
	long target_square;
	short target_angle;
	short actual_angle;
	short target_elevation;
	short box;
	short number;
	short last;
	short timer;
	short speed;
	long min_ypos;
	ITEM_INFO* item;
	ITEM_INFO* last_item;
	OBJECT_VECTOR* fixed;
	long mike_at_lara;
	PHD_VECTOR mike_pos;
};

struct GAMEFLOW_INFO
{
	long firstOption;
	long title_replace;
	long ondeath_demo_mode;
	long ondeath_ingame;
	long noinput_time;
	long on_demo_interrupt;
	long on_demo_end;
	long unused1[9];
	short num_levels;
	short num_picfiles;
	short num_titlefiles;
	short num_fmvfiles;
	short num_cutfiles;
	short num_demos;
	short title_track;
	short singlelevel;
	short unused2[16];
	ushort demoversion : 1;
	ushort title_disabled : 1;
	ushort cheatmodecheck_disabled : 1;
	ushort noinput_timeout : 1;
	ushort loadsave_disabled : 1;
	ushort screensizing_disabled : 1;
	ushort lockout_optionring : 1;
	ushort dozy_cheat_enabled : 1;
	ushort cyphered_strings : 1;
	ushort gym_enabled : 1;
	ushort play_any_level : 1;
	ushort cheat_enable : 1;
	ushort securitytag : 1;
	ushort unused3[3];
	uchar cypher_code;
	uchar language;
	uchar secret_track;
	uchar stats_track;
	char pads[4];
};

struct START_INFO
{
	ushort pistol_ammo;
	ushort magnum_ammo;
	ushort uzi_ammo;
	ushort shotgun_ammo;
	ushort m16_ammo;
	ushort rocket_ammo;
	ushort harpoon_ammo;
	ushort grenade_ammo;
	uchar num_medis;
	uchar num_big_medis;
	uchar num_scions;
	uchar num_flares;
	uchar num_sgcrystals;
	char gun_status;
	char gun_type;
	ushort available : 1;
	ushort got_pistols : 1;
	ushort got_magnums : 1;
	ushort got_uzis : 1;
	ushort got_shotgun : 1;
	ushort got_m16 : 1;
	ushort got_rocket : 1;
	ushort got_grenade : 1;
	ushort got_harpoon : 1;
	ushort secrets;
	uchar num_icon1;
	uchar num_icon2;
	uchar num_icon3;
	uchar num_icon4;
	ulong timer;
	ulong ammo_used;
	ulong ammo_hit;
	ulong distance_travelled;
	ushort kills;
	uchar secrets_found;
	uchar health_used;
};

struct SAVEGAME_INFO
{
	ulong best_assault_times[10];
	ulong best_quadbike_times[10];
	ulong QuadbikeKeyFlag;
	START_INFO start[25];
	ulong timer;
	ulong ammo_used;
	ulong ammo_hit;
	ulong distance_travelled;
	ushort kills;
	uchar secrets;
	uchar health_used;
	ulong cp_timer;
	ulong cp_ammo_used;
	ulong cp_ammo_hit;
	ulong cp_distance_travelled;
	ushort cp_kills;
	uchar cp_secrets;
	uchar cp_health_used;
	short current_level;
	uchar bonus_flag;
	uchar num_pickup1;
	uchar num_pickup2;
	uchar num_puzzle1;
	uchar num_puzzle2;
	uchar num_puzzle3;
	uchar num_puzzle4;
	uchar num_key1;
	uchar num_key2;
	uchar num_key3;
	uchar num_key4;
	char checksum;
	char GameComplete;
	ulong mid_level_save : 1;
	ulong AfterAdventureSave : 1;
	ulong WorldRequired : 3;
	ulong IndiaComplete : 1;
	ulong SPacificComplete : 1;
	ulong LondonComplete : 1;
	ulong NevadaComplete : 1;
	ulong AntarcticaComplete : 1;
	ulong PeruComplete : 1;
	ulong AfterIndia : 3;
	ulong AfterSPacific : 3;
	ulong AfterLondon : 3;
	ulong AfterNevada : 3;
	char buffer[12416];
};

struct OBJECT_INFO
{
	short nmeshes;
	short mesh_index;
	long bone_index;
	short* frame_base;
	void (*initialise)(short item_number);
	void (*control)(short item_number);
	void (*floor)(ITEM_INFO* item, long x, long y, long z, long* height);
	void (*ceiling)(ITEM_INFO* item, long x, long y, long z, long* height);
	void (*draw_routine)(ITEM_INFO* item);
	void (*collision)(short item_num, ITEM_INFO* laraitem, COLL_INFO* coll);
	short anim_index;
	short hit_points;
	short pivot_length;
	short radius;
	short shadow_size;
	ushort bite_offset;
	ushort loaded : 1;
	ushort intelligent : 1;
	ushort non_lot : 1;
	ushort save_position : 1;
	ushort save_hitpoints : 1;
	ushort save_flags : 1;
	ushort save_anim : 1;
	ushort semi_transparent : 1;
	ushort water_creature : 1;
};

struct TEXTSTRING
{
	long flags;
	ushort textflags;
	ushort bgndflags;
	ushort outlflags;
	short xpos;
	short ypos;
	short zpos;
	short letterSpacing;
	short wordSpacing;
	short flashRate;
	short flashCount;
	short bgndColour;
	ushort* bgndGour;
	short outlColour;
	ushort* outlGour;
	short bgndSizeX;
	short bgndSizeY;
	short bgndOffX;
	short bgndOffY;
	short bgndOffZ;
	long scaleH;
	long scaleV;
	short Colour;
	char* string;
};

struct DISPLAYPU
{
	short duration;
	short sprnum;
};

struct OLD_CAMERA
{
	short current_anim_state;
	short goal_anim_state;
	PHD_3DPOS pos;
	PHD_3DPOS pos2;
};
#pragma pack(pop)