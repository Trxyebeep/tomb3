#include "../tomb3/pch.h"
#include "inventry.h"
#include "objects.h"
#include "../3dsystem/3d_gen.h"
#include "../3dsystem/scalespr.h"
#include "../specific/frontend.h"
#include "../specific/output.h"
#include "draw.h"
#include "invfunc.h"
#include "../specific/display.h"
#include "../specific/picture.h"
#include "text.h"
#include "../specific/specific.h"
#include "sound.h"
#include "laramisc.h"
#include "../specific/input.h"
#include "demo.h"
#include "health.h"
#include "../specific/option.h"
#include "gameflow.h"
#include "../specific/winmain.h"
#include "../specific/smain.h"
#include "control.h"
#include "camera.h"
#include "savegame.h"
#if (DIRECT3D_VERSION >= 0x900)
#include "../newstuff/Picture2.h"
#endif
#include "../newstuff/discord.h"
#include "../tomb3/tomb3.h"

long Inventory_ExtraData[8];
long GlobePointLight;
short GlobeXRot;
short GlobeYRot;
short GlobeZRot;
short GlobeLevel;
uchar GlobeLevelComplete[6];

long inputDB;
short Option_Music_Volume = 255;
short Option_SFX_Volume = 165;
long Inventory_Mode = INV_TITLE_MODE;
TEXTSTRING* passport_text1;
TEXTSTRING* Inv_ringText;
TEXTSTRING* Inv_tagText;
TEXTSTRING* Inv_upArrow1;
TEXTSTRING* Inv_upArrow2;
TEXTSTRING* Inv_downArrow1;
TEXTSTRING* Inv_downArrow2;
long Inventory_DemoMode;
static long OpenInvOnGym = 1;
long idelay;
long idcount;
short inv_keys_objects;
short inv_keys_current;
short inv_main_objects = 8;
short inv_main_current;
short Inventory_Displaying;
short Inventory_Chosen;
short inv_option_objects;
short inv_option_current;
short item_data;

long GetDebouncedInput(long in)
{
	static long db;
	long i;

	i = ~db;
	db = in;
	return i & in;
}

void SelectMeshes(INVENTORY_ITEM* item)
{
	if (item->object_number == PASSPORT_OPTION)
	{
		if (item->current_frame < 4)
			item->drawn_meshes = 23;
		else if (item->current_frame <= 16)
			item->drawn_meshes = 87;
		else if (item->current_frame < 19)
			item->drawn_meshes = 95;
		else if (item->current_frame == 19)
			item->drawn_meshes = 91;
		else if (item->current_frame < 24)
			item->drawn_meshes = 123;
		else if (item->current_frame < 29)
			item->drawn_meshes = 59;
		else if (item->current_frame == 29)
			item->drawn_meshes = 19;
	}
	else if (item->object_number == MAP_OPTION)
	{
		if (!item->current_frame || item->current_frame >= 18)
			item->drawn_meshes = item->which_meshes;
		else
			item->drawn_meshes = -1;
	}
	else if (item->object_number != GAMMA_OPTION)
		item->drawn_meshes = -1;
}

long AnimateInventoryItem(INVENTORY_ITEM* item)
{
	if (item->current_frame == item->goal_frame)
	{
		SelectMeshes(item);
		return 0;
	}

	if (item->anim_count)
		item->anim_count--;
	else
	{
		item->anim_count = item->anim_speed;
		item->current_frame += item->anim_direction;

		if (item->current_frame >= item->frames_total)
			item->current_frame = 0;
		else if (item->current_frame < 0)
			item->current_frame = item->frames_total - 1;
	}

	SelectMeshes(item);
	return 1;
}

void DrawInventoryItem(INVENTORY_ITEM* item)
{
	INVENTORY_SPRITE** slist;
	INVENTORY_SPRITE* sp;
	OBJECT_INFO* obj;
	long* bone;
	short* frame;
	short* rot;
	long m, h, s, x, y, z, clip;
	short mesh, bit;

	nPolyType = 7;

	if (bInvItemsOff)
		return;

	if (item->object_number == MAP_OPTION)
	{
		s = savegame.timer / 30;
		m = -91 * (s % 3600) / 5;
		h = -91 * (s / 12) / 5;
		s = -1092 * (s % 60);
	}
	else
	{
		m = 0;
		h = 0;
		s = 0;
	}

	phd_TranslateRel(0, item->ytrans, item->ztrans);
	phd_RotYXZ(item->y_rot, item->x_rot, 0);

	if (item->object_number == GAMMA_OPTION)
		phd_RotYXZ(GlobeYRot, GlobeXRot, GlobeZRot);

	obj = &objects[item->object_number];

	if (!obj->loaded)
		return;

	if (obj->nmeshes < 0)
	{
		S_DrawSprite(0, 0, 0, 0, obj->mesh_index, 0, 0);
		return;
	}

	slist = item->sprlist;

	if (slist)
	{
		z = phd_mxptr[M23];
		x = phd_centerx + phd_mxptr[M03] / (z / phd_persp);
		y = phd_centery + phd_mxptr[M13] / (z / phd_persp);

		while (sp = *slist++)
		{
			if (z < phd_znear || z > phd_zfar)
				break;

			while (sp->shape)
			{
				switch (sp->shape)
				{
				case 1:
					S_DrawScreenSprite(x + sp->x, y + sp->y, sp->z, sp->param1, sp->param2, static_objects[ALPHABET].mesh_number + sp->sprnum, 0x200000, 0);
					break;

				case 2:
					S_DrawScreenLine(x + sp->x, y + sp->y, sp->z, sp->param1, sp->param2, sp->sprnum, sp->grdptr, 0);
					break;

				case 3:
					S_DrawScreenBox(x + sp->x, y + sp->y, sp->z, sp->param1, sp->param2, sp->sprnum, sp->grdptr, 0);
					break;

				case 4:
					S_DrawScreenFBox(x + sp->x, y + sp->y, sp->z, sp->param1, sp->param2, sp->sprnum, sp->grdptr, 0);
					break;
				}

				s++;
			}
		}
	}

	frame = &obj->frame_base[item->current_frame * (anims[obj->anim_index].interpolation >> 8)];

	phd_PushMatrix();

	clip = S_GetObjectBounds(frame);

	if (!clip)
	{
		phd_PopMatrix();
		return;
	}

	phd_TranslateRel(frame[6], frame[7], frame[8]);
	rot = frame + 9;
	gar_RotYXZsuperpack(&rot, 0);
	bone = &bones[obj->bone_index];
	mesh = obj->mesh_index;
	bit = 1;

	if (item->object_number == GAMMA_OPTION)
		item->drawn_meshes = ~8;

	if (item->drawn_meshes & bit)
	{
		if (item->object_number == GAMMA_OPTION)
			Inv_GlobeLight(bit);

		phd_PutPolygons(meshes[mesh], clip);
	}

	for (int i = obj->nmeshes - 1; i > 0; i--, bone += 4)
	{
		mesh++;
		bit <<= 1;

		if (bone[0] & 1)
			phd_PopMatrix();

		if (bone[0] & 2)
			phd_PushMatrix();

		phd_TranslateRel(bone[1], bone[2], bone[3]);
		gar_RotYXZsuperpack(&rot, 0);

		if (item->object_number == MAP_OPTION)
		{
			if (i == 1)
			{
				phd_RotZ((short)s);
				item->misc_data[1] = item->misc_data[0];
				item->misc_data[0] = s;
			}

			if (i == 2)
				phd_RotZ((short)m);

			if (i == 3)
				phd_RotZ((short)h);
		}

		if (item->object_number == GAMMA_OPTION)
			Inv_GlobeLight(bit);

		if (item->drawn_meshes & bit)
			phd_PutPolygons(meshes[mesh], clip);
	}

	phd_PopMatrix();
}

void GetGlobeMeshFlags()
{
	GlobeLevelComplete[0] = savegame.IndiaComplete;
	GlobeLevelComplete[1] = savegame.SPacificComplete;
	GlobeLevelComplete[2] = savegame.NevadaComplete;
	GlobeLevelComplete[3] = -1;
	GlobeLevelComplete[4] = savegame.LondonComplete;

	if (savegame.IndiaComplete && savegame.SPacificComplete && savegame.NevadaComplete && savegame.LondonComplete)
		GlobeLevelComplete[5] = savegame.AntarcticaComplete;
	else
		GlobeLevelComplete[5] = -1;

	GlobeLevel = 0;
	while (GlobeLevelComplete[GlobeLevel]) GlobeLevel++;
}

void Construct_Inventory()
{
	INVENTORY_ITEM* item;
	float scale;

	S_SetupAboveWater(0);

	if (Inventory_Mode != INV_TITLE_MODE)
		TempVideoAdjust(HiResFlag, 1.0);

	memset(Inventory_ExtraData, 0, sizeof(Inventory_ExtraData));
	phd_right = phd_winxmax;
	phd_left = 0;
	phd_top = 0;
	phd_bottom = phd_winymax;

	Inventory_Displaying = 1;
	Inventory_Chosen = 0;

	if (Inventory_Mode == INV_TITLE_MODE)
		inv_option_objects = 5;
	else
		inv_option_objects = 4;

	for (int i = 0; i < inv_main_objects; i++)
	{
		item = inv_main_list[i];
		item->current_frame = 0;
		item->drawn_meshes = item->which_meshes;
		item->goal_frame = 0;
		item->anim_count = 0;
		item->y_rot = 0;
	}

	for (int i = 0; i < inv_option_objects; i++)
	{
		item = inv_option_list[i];
		item->current_frame = 0;
		item->goal_frame = 0;
		item->anim_count = 0;
		item->y_rot = 0;
	}

	for (int i = 0; i < 1; i++)
	{
		item = inv_levelselect_list[i];

		if (i)
			item->drawn_meshes = 0xF81;
		else
			item->drawn_meshes = -1;

		item->current_frame = 0;
		item->goal_frame = 0;
		item->anim_count = 0;
		item->y_rot = 0;
	}

	inv_main_current = 0;

	if (OpenInvOnGym && Inventory_Mode == INV_TITLE_MODE && !gameflow.loadsave_disabled && gameflow.gym_enabled)
		inv_option_current = 4;
	else
	{
		inv_option_current = 0;
		OpenInvOnGym = 0;
	}

	item_data = 0;
	scale = (float)GetRenderScale(480) / (float)GetRenderHeight();

	if (scale < 1.5F)
		icompass_option.ytrans_sel = tomb3.psx_fov ? -140 : -170;
	else if (scale < 1.7F)
		icompass_option.ytrans_sel = long(-15 - (scale - 1.5F) * 35.0F);
	else
		icompass_option.ytrans_sel = long(-22 - (scale - 1.7F) / 0.0075F);
}

long Display_Inventory(long mode)
{
	INVENTORY_ITEM* item;
	RING_INFO ring;
	IMOTION_INFO imo;
	PHD_3DPOS viewer;
	float vol;
	static long nframes;
	long demo_needed, pass_open, dy, busy;
	static short JustSaved;
	short first_time, yang;

	memset(&imo, 0, sizeof(IMOTION_INFO));
	memset(&ring, 0, sizeof(RING_INFO));
	first_time = 0;
	demo_needed = 0;
	pass_open = 0;
	busy = 0;

	if (mode == INV_LEVELSELECT_MODE)
	{
		first_time = 1;
		GetGlobeMeshFlags();
		JustSaved = 0;
		GlobePointLight = 0;
	}

	if (mode == INV_KEYS_MODE && !inv_keys_objects)
	{
		Inventory_Chosen = NO_ITEM;
		return 0;
	}

	if (mode != INV_TITLE_MODE && mode != INV_LEVELSELECT_MODE)
		CreateMonoScreen();

	T_RemovePrint(ammotext);
	ammotext = 0;

	AlterFOV(14560);
	Inventory_Mode = mode;
	nframes = TICKS_PER_FRAME;
	Construct_Inventory();
	SOUND_Stop();

	if (mode != INV_TITLE_MODE && mode != INV_LEVELSELECT_MODE)
	{
		vol = (1.0F - tomb3.inv_music_mute) * float(25 * Option_Music_Volume + 5);

		if (vol >= 1)
			S_CDVolume((long)vol);
		else
			S_CDVolume(0);
	}

	switch (mode)
	{
	case INV_TITLE_MODE:
	case INV_SAVE_MODE:
	case INV_LOAD_MODE:
	case INV_DEATH_MODE:
		Inv_RingInit(&ring, 1, inv_option_list, inv_option_objects, inv_option_current, &imo);
		break;

	case INV_KEYS_MODE:
		Inv_RingInit(&ring, 2, inv_keys_list, inv_keys_objects, inv_main_current, &imo);
		break;

	case INV_LEVELSELECT_MODE:
		Inv_RingInit(&ring, 3, inv_levelselect_list, 1, 0, &imo);
		break;

	default:

		if (inv_main_objects)
			Inv_RingInit(&ring, 0, inv_main_list, inv_main_objects, inv_main_current, &imo);
		else
			Inv_RingInit(&ring, 1, inv_option_list, inv_option_objects, inv_option_current, &imo);

		break;
	}

	if (mode != INV_TITLE_MODE)
		SoundEffect(SFX_MENU_SPININ, 0, SFX_ALWAYS);

	nframes = TICKS_PER_FRAME;

	do
	{
		RPC_title = mode == INV_TITLE_MODE;
		RPC_Update();
		RPC_title = 0;

		GlobePointLight = (GlobePointLight + 16) & 0xFF;

		Inv_RingCalcAdders(&ring, 24);
		S_UpdateInput();

		if (Inventory_DemoMode)
		{
			if (input)
				return gameflow.on_demo_interrupt;

			GetDemoInput();

			if (input == IN_ALL)
				return gameflow.on_demo_end;
		}
		else if (input)
			noinput_count = 0;

		inputDB = GetDebouncedInput(input);

		if (Inventory_Mode != INV_TITLE_MODE || input || inputDB)
		{
			noinput_count = 0;
			reset_flag = 0;
		}
		else if (gameflow.num_demos || gameflow.noinput_timeout)
		{
			noinput_count++;

			if (noinput_count > gameflow.noinput_time)
			{
				demo_needed = 1;
				reset_flag = 1;
			}
		}

		if (game_closedown)
			return EXIT_TO_TITLE;

		if ((Inventory_Mode == INV_SAVE_MODE || Inventory_Mode == INV_LOAD_MODE || Inventory_Mode == INV_DEATH_MODE) && !pass_open)
		{
			inputDB = IN_SELECT;
			input = 0;
		}

		if (Inventory_Mode == INV_LEVELSELECT_MODE && first_time && !ring.imo->count)
		{
			inputDB = IN_SELECT;
			input = 0;
			first_time = 0;
		}

		for (int i = 0; i < nframes; i++)
		{
			if (idelay)
			{
				if (idcount)
					idcount--;
				else
					idelay = 0;
			}

			Inv_RingDoMotions(&ring);
		}

		ring.camerapos.z_pos = ring.radius + 598;

		/*draw phase*/
		S_InitialisePolyList(0);

		if (Inventory_Mode != INV_TITLE_MODE && Inventory_Mode != INV_LEVELSELECT_MODE)
			DrawMonoScreen(80, 80, 112);
		else
			DoInventoryPicture();

		S_AnimateTextures(nframes);
		Inv_RingGetView(&ring, &viewer);
		phd_GenerateW2V(&viewer);
		Inv_RingLight(&ring, 0);

		phd_PushMatrix();
		phd_TranslateAbs(ring.ringpos.x_pos, ring.ringpos.y_pos, ring.ringpos.z_pos);
		phd_RotYXZ(ring.ringpos.y_rot, ring.ringpos.x_rot, ring.ringpos.z_rot);
		yang = 0;

		for (int i = 0; i < ring.number_of_objects; i++)
		{
			item = ring.list[i];

			if (i == ring.current_object)
			{
				for (int j = 0; j < nframes; j++)
				{
					if (ring.rotating)
					{
						if (item->y_rot)
						{
							if (item->y_rot < 0)
								item->y_rot += 512;
							else
								item->y_rot -= 512;
						}
					}
					else if (imo.status != RNG_SELECTED && imo.status != RNG_DESELECTING && imo.status != RNG_SELECTING &&
						imo.status != RNG_DESELECT && imo.status != RNG_CLOSING_ITEM)
					{
						if (ring.number_of_objects == 1 || !(input & (IN_LEFT | IN_RIGHT)))
							item->y_rot += 256;
					}
					else if (item->y_rot != item->y_rot_sel)
					{
						dy = item->y_rot_sel - item->y_rot;

						if (dy > 0 && dy < 0x8000)
							item->y_rot += 1024;
						else
							item->y_rot -= 1024;

						item->y_rot &= ~0x3FF;
					}
				}

				if ((imo.status == RNG_OPEN || imo.status == RNG_SELECTING || imo.status == RNG_SELECTED || imo.status == RNG_DESELECTING ||
					imo.status == RNG_DESELECT || imo.status == RNG_CLOSING_ITEM) && !ring.rotating && !(input & (IN_LEFT | IN_RIGHT)))
					RingNotActive(item);
			}
			else
			{
				for (int j = 0; j < nframes; j++)
				{
					if (item->y_rot)
					{
						if (item->y_rot < 0)
							item->y_rot += 256;
						else
							item->y_rot -= 256;
					}
				}
			}

			if (imo.status == RNG_OPEN || imo.status == RNG_SELECTING || imo.status == RNG_SELECTED || imo.status == RNG_DESELECTING ||
				imo.status == RNG_DESELECT || imo.status == RNG_CLOSING_ITEM)
				RingIsOpen(&ring);
			else
				RingIsNotOpen(&ring);

			if (imo.status == RNG_OPENING || imo.status == RNG_CLOSING || imo.status == RNG_MAIN2OPTION || imo.status == RNG_OPTION2MAIN ||
				imo.status == RNG_EXITING_INVENTORY || imo.status == RNG_DONE || ring.rotating)
				RingActive();

			phd_PushMatrix();
			phd_RotYXZ(yang, 0, 0);
			phd_TranslateRel(ring.radius, 0, 0);
			phd_RotYXZ(0x4000, item->pt_xrot, 0);
			DrawInventoryItem(item);
			phd_PopMatrix();
			yang += ring.angle_adder;
		}

		phd_PopMatrix();

		DrawModeInfo();
		T_DrawText();
		S_OutputPolyList();
		SOUND_EndScene();
		nframes = S_DumpScreen();
		camera.number_frames = nframes;
		/*end draw phase*/

		/*control phase*/
		if (CurrentLevel)
			savegame.timer += nframes / TICKS_PER_FRAME;

		if (ring.rotating)
			continue;

		switch (imo.status)
		{
		case RNG_OPEN:

			if (input & IN_RIGHT && ring.number_of_objects > 1)
			{
				Inv_RingRotateLeft(&ring);
				SoundEffect(SFX_MENU_ROTATE, 0, SFX_ALWAYS);
				break;
			}

			if (input & IN_LEFT && ring.number_of_objects > 1)
			{
				Inv_RingRotateRight(&ring);
				SoundEffect(SFX_MENU_ROTATE, 0, SFX_ALWAYS);
				break;
			}

			if (reset_flag || (inputDB & (IN_DESELECT | IN_OPTION)) &&
				(reset_flag || (Inventory_Mode != INV_TITLE_MODE && Inventory_Mode != INV_LEVELSELECT_MODE)))
			{
				SoundEffect(SFX_MENU_SPINOUT, 0, SFX_ALWAYS);
				Inventory_Chosen = NO_ITEM;

				if (ring.type)
					inv_option_current = ring.current_object;
				else
					inv_main_current = ring.current_object;

				Inv_RingMotionSetup(&ring, RNG_CLOSING, RNG_DONE, 32);
				Inv_RingMotionRadius(&ring, 0);
				Inv_RingMotionCameraPos(&ring, -1536);
				Inv_RingMotionRotation(&ring, -0x8000, ring.ringpos.y_rot + 0x8000);
				input = 0;
				inputDB = 0;
			}

			if (inputDB & IN_SELECT)
			{
				if ((Inventory_Mode == INV_SAVE_MODE || Inventory_Mode == INV_LOAD_MODE || Inventory_Mode == INV_DEATH_MODE) && !pass_open)
					pass_open = 1;

				item_data = 0;

				if (!ring.type)
				{
					inv_main_current = ring.current_object;
					item = inv_main_list[ring.current_object];
				}
				else if (ring.type == 1)
				{
					inv_option_current = ring.current_object;
					item = inv_option_list[ring.current_object];
				}
				else if (ring.type == 3)
				{
					inv_option_current = ring.current_object;
					item = inv_levelselect_list[ring.current_object];
				}
				else
				{
					inv_keys_current = ring.current_object;
					item = inv_keys_list[ring.current_object];
				}

				item->goal_frame = item->open_frame;
				item->anim_direction = 1;
				Inv_RingMotionSetup(&ring, RNG_SELECTING, RNG_SELECTED, 16);
				Inv_RingMotionRotation(&ring, 0, -0x4000 - ring.angle_adder * ring.current_object);
				Inv_RingMotionItemSelect(&ring, item);
				input = 0;
				inputDB = 0;

				if (item->object_number == MAP_OPTION)
					SoundEffect(SFX_MENU_STOPWATCH, 0, SFX_ALWAYS);
				else if (item->object_number >= GUN_OPTION && item->object_number <= GRENADE_OPTION)
					SoundEffect(SFX_MENU_GUNS, 0, SFX_ALWAYS);
				else
					SoundEffect(SFX_MENU_CHOOSE, 0, SFX_ALWAYS);
			}

			if (inputDB & IN_FORWARD && mode != INV_TITLE_MODE && mode != INV_LEVELSELECT_MODE && mode != INV_KEYS_MODE)
			{
				if (!ring.type)
				{
					if (inv_keys_objects)
					{
						Inv_RingMotionSetup(&ring, RNG_CLOSING, RNG_MAIN2KEYS, 24);
						Inv_RingMotionRadius(&ring, 0);
						Inv_RingMotionRotation(&ring, -0x8000, ring.ringpos.y_rot + 0x8000);
						Inv_RingMotionCameraPitch(&ring, 0x2000);
						imo.misc = 0x2000;
					}

					input = 0;
					inputDB = 0;
				}
				else if (ring.type == 1)
				{
					if (inv_main_objects)
					{
						Inv_RingMotionSetup(&ring, RNG_CLOSING, RNG_OPTION2MAIN, 24);
						Inv_RingMotionRadius(&ring, 0);
						Inv_RingMotionRotation(&ring, -0x8000, ring.ringpos.y_rot + 0x8000);
						Inv_RingMotionCameraPitch(&ring, 0x2000);
						imo.misc = 0x2000;
					}

					inputDB = 0;
				}
			}
			else if (inputDB & IN_BACK && mode != INV_TITLE_MODE && mode != INV_LEVELSELECT_MODE && mode != INV_KEYS_MODE)
			{
				if (ring.type == 2)
				{
					if (inv_main_objects)
					{
						Inv_RingMotionSetup(&ring, RNG_CLOSING, RNG_KEYS2MAIN, 24);
						Inv_RingMotionRadius(&ring, 0);
						Inv_RingMotionRotation(&ring, -0x8000, ring.ringpos.y_rot + 0x8000);
						Inv_RingMotionCameraPitch(&ring, -0x2000);
						imo.misc = -0x2000;
					}

					input = 0;
					inputDB = 0;
				}
				else if (!ring.type)
				{
					if (inv_option_objects && !gameflow.lockout_optionring)
					{
						Inv_RingMotionSetup(&ring, RNG_CLOSING, RNG_MAIN2OPTION, 24);
						Inv_RingMotionRadius(&ring, 0);
						Inv_RingMotionRotation(&ring, -0x8000, ring.ringpos.y_rot + 0x8000);
						Inv_RingMotionCameraPitch(&ring, -0x2000);
						imo.misc = -0x2000;
					}

					inputDB = 0;
				}
			}

			break;

		case RNG_MAIN2OPTION:
			Inv_RingMotionSetup(&ring, RNG_OPENING, RNG_OPEN, 24);
			Inv_RingMotionRadius(&ring, 688);
			ring.camera_pitch = -(short)imo.misc;
			imo.camera_pitch_rate = short(imo.misc / 24);
			imo.camera_pitch_target = 0;
			ring.list = inv_option_list;
			ring.type = 1;
			inv_main_current = ring.current_object;
			ring.number_of_objects = inv_option_objects;
			ring.current_object = inv_option_current;
			Inv_RingCalcAdders(&ring, 24);
			Inv_RingMotionRotation(&ring, -0x8000, -0x4000 - ring.angle_adder * ring.current_object);
			ring.ringpos.y_rot = imo.rotate_target + 0x8000;
			break;

		case RNG_MAIN2KEYS:
			Inv_RingMotionSetup(&ring, RNG_OPENING, RNG_OPEN, 24);
			Inv_RingMotionRadius(&ring, 688);
			imo.camera_pitch_target = 0;
			ring.camera_pitch = -(short)imo.misc;
			imo.camera_pitch_rate = short(imo.misc / 24);
			inv_main_current = ring.current_object;
			inv_main_objects = ring.number_of_objects;
			ring.list = inv_keys_list;
			ring.type = 2;
			ring.number_of_objects = inv_keys_objects;
			ring.current_object = inv_keys_current;
			Inv_RingCalcAdders(&ring, 24);
			Inv_RingMotionRotation(&ring, -0x8000, -0x4000 - ring.angle_adder * ring.current_object);
			ring.ringpos.y_rot = imo.rotate_target + 0x8000;
			break;

		case RNG_KEYS2MAIN:
			Inv_RingMotionSetup(&ring, RNG_OPENING, RNG_OPEN, 24);
			Inv_RingMotionRadius(&ring, 688);
			ring.camera_pitch = -(short)imo.misc;
			imo.camera_pitch_rate = short(imo.misc / 24);
			imo.camera_pitch_target = 0;
			ring.list = inv_main_list;
			ring.type = 0;
			inv_keys_current = ring.current_object;
			ring.number_of_objects = inv_main_objects;
			ring.current_object = inv_main_current;
			Inv_RingCalcAdders(&ring, 24);
			Inv_RingMotionRotation(&ring, -0x8000, -0x4000 - ring.angle_adder * ring.current_object);
			ring.ringpos.y_rot = imo.rotate_target + 0x8000;
			break;

		case RNG_OPTION2MAIN:
			Inv_RingMotionSetup(&ring, RNG_OPENING, RNG_OPEN, 24);
			Inv_RingMotionRadius(&ring, 688);
			ring.camera_pitch = -(short)imo.misc;
			imo.camera_pitch_rate = short(imo.misc / 24);
			inv_option_current = ring.current_object;
			inv_option_objects = ring.number_of_objects;
			imo.camera_pitch_target = 0;
			ring.list = inv_main_list;
			ring.type = 0;
			ring.number_of_objects = inv_main_objects;
			ring.current_object = inv_main_current;
			Inv_RingCalcAdders(&ring, 24);
			Inv_RingMotionRotation(&ring, -0x8000, -0x4000 - ring.angle_adder * ring.current_object);
			ring.ringpos.y_rot = imo.rotate_target + 0x8000;
			break;

		case RNG_SELECTED:
			item = ring.list[ring.current_object];

			if (item->object_number == PASSPORT_CLOSED)
				item->object_number = PASSPORT_OPTION;

			for (int i = 0; i < nframes; i++)
			{
				busy = 0;

				if (item->y_rot == item->y_rot_sel)
					busy = AnimateInventoryItem(item);
			}

			if (!busy && !idelay)
			{
				do_inventory_options(item);

				if (inputDB & IN_DESELECT)
				{
					item->sprlist = 0;
					Inv_RingMotionSetup(&ring, RNG_CLOSING_ITEM, RNG_DESELECT, 0);
					input = 0;
					inputDB = 0;

					if (Inventory_Mode == INV_LOAD_MODE || Inventory_Mode == INV_SAVE_MODE)
					{
						Inv_RingMotionSetup(&ring, RNG_CLOSING_ITEM, RNG_EXITING_INVENTORY, 0);
						inputDB = 0;
						input = 0;
					}
				}

				if (inputDB & IN_SELECT)
				{
					item->sprlist = 0;
					Inventory_Chosen = item->object_number;

					if (ring.type)
						inv_option_current = ring.current_object;
					else
						inv_main_current = ring.current_object;

					if (Inventory_Mode == INV_LEVELSELECT_MODE)
					{
						if (item->object_number == PASSPORT_OPTION && JustSaved)
							Inv_RingMotionSetup(&ring, RNG_CLOSING_ITEM, RNG_DESELECT, 0);
						else
							Inv_RingMotionSetup(&ring, RNG_CLOSING_ITEM, RNG_EXITING_INVENTORY, 0);

						JustSaved = 0;
					}
					else if (Inventory_Mode == INV_TITLE_MODE && item->object_number == DETAIL_OPTION ||
						item->object_number == SOUND_OPTION || item->object_number == CONTROL_OPTION)
						Inv_RingMotionSetup(&ring, RNG_CLOSING_ITEM, RNG_DESELECT, 0);
					else
						Inv_RingMotionSetup(&ring, RNG_CLOSING_ITEM, RNG_EXITING_INVENTORY, 0);

					input = 0;
					inputDB = 0;
				}
			}

			break;

		case RNG_DESELECT:
			SoundEffect(SFX_MENU_SPINOUT, 0, SFX_ALWAYS);
			Inv_RingMotionSetup(&ring, RNG_DESELECTING, RNG_OPEN, 16);
			Inv_RingMotionRotation(&ring, 0, -0x4000 - ring.angle_adder * ring.current_object);
			input = 0;
			inputDB = 0;
			break;

		case RNG_CLOSING_ITEM:
			item = ring.list[ring.current_object];

			for (int i = 0; i < nframes; i++)
			{
				if (!AnimateInventoryItem(item))
				{
					if (item->object_number == PASSPORT_OPTION)
					{
						item->object_number = PASSPORT_CLOSED;
						item->current_frame = 0;
					}

					imo.count = 16;
					imo.status = imo.status_target;
					Inv_RingMotionItemDeselect(&ring, item);
					break;
				}
			}

			break;

		case RNG_EXITING_INVENTORY:

			if (!imo.count)
			{
				Inv_RingMotionSetup(&ring, RNG_CLOSING, RNG_DONE, 32);
				Inv_RingMotionRadius(&ring, 0);
				Inv_RingMotionCameraPos(&ring, -1536);
				Inv_RingMotionRotation(&ring, -0x8000, ring.ringpos.y_rot + 0x8000);
			}

			break;
		}

		/*end control phase*/

	} while (imo.status != RNG_DONE);

	if (mode != INV_TITLE_MODE && mode != INV_LEVELSELECT_MODE)
	{
		if (Inventory_Chosen != PASSPORT_OPTION || Inventory_ExtraData[0] == 1)
			RemoveMonoScreen(0);
		else
			RemoveMonoScreen(1);
	}

	RemoveInventoryText();
	S_FinishInventory();
	Inventory_Displaying = 0;

	if (reset_flag)
		return EXIT_TO_TITLE;

	if (demo_needed)
		return STARTDEMO;

	if (Inventory_Chosen == NO_ITEM)
	{
		if (mode != INV_TITLE_MODE && Option_Music_Volume)
		{
			if (camera.underwater)
			{
				vol = (1.0F - tomb3.unwater_music_mute) * float(25 * Option_Music_Volume + 5);

				if (vol >= 1)
					S_CDVolume((long)vol);
				else
					S_CDVolume(0);
			}
			else
				S_CDVolume(25 * Option_Music_Volume + 5);
		}

		return 0;
	}

	switch (Inventory_Chosen)
	{
	case PASSPORT_OPTION:

		if (Inventory_ExtraData[0] != 1 || !Option_Music_Volume)
			return 1;

		S_CDVolume(25 * Option_Music_Volume + 5);
		return 1;

	case PHOTO_OPTION:

		if (!gameflow.gym_enabled)
			break;

		Inventory_ExtraData[1] = 0;
		return 1;

	case GUN_OPTION:
	case SHOTGUN_OPTION:
	case MAGNUM_OPTION:
	case UZI_OPTION:
	case HARPOON_OPTION:
	case M16_OPTION:
	case ROCKET_OPTION:
	case GRENADE_OPTION:
	case MEDI_OPTION:
	case BIGMEDI_OPTION:
	case FLAREBOX_OPTION:
		UseItem(Inventory_Chosen);
		break;
	}

	if (mode != INV_TITLE_MODE && mode != INV_LEVELSELECT_MODE && Option_Music_Volume)
	{
		if (camera.underwater)
		{
			vol = (1.0F - tomb3.unwater_music_mute) * float(25 * Option_Music_Volume + 5);

			if (vol >= 1)
				S_CDVolume((long)vol);
			else
				S_CDVolume(0);
		}
		else
			S_CDVolume(25 * Option_Music_Volume + 5);
	}

	return 0;
}
