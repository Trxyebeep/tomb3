#include "../tomb3/pch.h"
#include "health.h"
#include "objects.h"
#include "../3dsystem/scalespr.h"

long FlashIt()
{
	static long flash_state = 0, flash_count = 0;

	if (flash_count)
		flash_count--;
	else
	{
		flash_state ^= 1;
		flash_count = 5;
	}

	return flash_state;
}

void DrawAssaultTimer()
{
	char* txt;
	long timer, x;
	char buffer[8];

	if (!CurrentLevel && assault_timer_display && !QuadbikeLapTimeDisplayTimer)
	{
		if (savegame.timer > 0x1A5DD)
			savegame.timer = 0x1A5DD;

		timer = savegame.timer;

		if (timer < 2)
			timer = 0;

		sprintf(buffer, "%d:%02d.%02d", timer / 30 / 60, timer / 30 % 60, 334 * (timer % 30) / 100);
		x = (phd_winxmax >> 1) - 50;

		for (txt = buffer; *txt; txt++)
		{
			if (*txt == ':')
			{
				x -= 6;
				S_DrawScreenSprite2d(x, 36, 0, 0x10000, 0x10000, objects[ASSAULT_NUMBERS].mesh_index + 10, 2, 0);
				x += 14;
			}
			else if (*txt == '.')
			{
				x -= 6;
				S_DrawScreenSprite2d(x, 36, 0, 0x10000, 0x10000, objects[ASSAULT_NUMBERS].mesh_index + 11, 2, 0);
				x += 14;
			}
			else
			{
				S_DrawScreenSprite2d(x, 36, 0, 0x10000, 0x10000, *txt + objects[ASSAULT_NUMBERS].mesh_index - '0', 2, 0);
				x += 20;
			}
		}
	}
}

void DrawAssaultPenalties(long type)
{
	char* txt;
	long timer, x, y;
	char buffer[12];

	if (!CurrentLevel && assault_penalty_display_timer && assault_timer_display)
	{
		if (type)
		{
			if (!assault_target_penalties)
				return;

			if (assault_target_penalties > 0x1A5DD)
				assault_target_penalties = 0x1A5DD;

			x = (phd_winxmax >> 1) - 193;
			y = !assault_penalties ? 36 : 64;
			timer = assault_target_penalties;
			sprintf(buffer, "T %d:%02d s", timer / 30 / 60, timer / 30 % 60);
		}
		else
		{
			if (!assault_penalties)
				return;

			if (assault_penalties > 0x1A5DD)
				assault_penalties = 0x1A5DD;

			x = (phd_winxmax >> 1) - 175;
			y = 36;
			timer = assault_penalties;
			sprintf(buffer, "%d:%02d s", timer / 30 / 60, timer / 30 % 60);
		}

		for (txt = buffer; *txt; txt++)
		{
			if (*txt == ' ')
				x += 8;
			else if (*txt == 'T')
			{
				x -= 6;
				S_DrawScreenSprite2d(x, y + 1, 0, 0x10000, 0x10000, objects[ASSAULT_NUMBERS].mesh_index + 12, 0, 0);
				x += 16;
			}
			else if (*txt == 's')
			{
				x -= 6;
				S_DrawScreenSprite2d(x - 4, y, 0, 0x10000, 0x10000, objects[ASSAULT_NUMBERS].mesh_index + 13, 9, 0);
				x += 14;
			}
			else if (*txt == ':')
			{
				x -= 6;
				S_DrawScreenSprite2d(x, y, 0, 0x10000, 0x10000, objects[ASSAULT_NUMBERS].mesh_index + 10, 9, 0);
				x += 14;
			}
			else if (*txt == '.')
			{
				x -= 6;
				S_DrawScreenSprite2d(x, y, 0, 0x10000, 0x10000, objects[ASSAULT_NUMBERS].mesh_index + 11, 9, 0);
				x += 14;
			}
			else
			{
				S_DrawScreenSprite2d(x, y, 0, 0x10000, 0x10000, *txt + objects[ASSAULT_NUMBERS].mesh_index - '0', 9, 0);
				x += 20;
			}
		}
	}
}

void DrawQuadbikeLapTime()
{
	char* txt;
	long timer, x, hundredth;
	short col;
	char buffer[8];

	if (!CurrentLevel && QuadbikeLapTimeDisplayTimer)
	{
		for (int i = 0; i < 2; i++)
		{
			if (i)
			{
				timer = savegame.best_quadbike_times[0] / 0x1E;
				col = 10;
				hundredth = 334 * (savegame.best_quadbike_times[0] % 0x1E) / 0x64;
				x = (phd_winxmax >> 1) + 100;
			}
			else
			{
				col = 9;
				timer = QuadbikeLapTime / 30;
				hundredth = 334 * (QuadbikeLapTime % 30) / 100;
				x = (phd_winxmax >> 1) - 50;
			}

			sprintf(buffer, "%d:%02d.%02d", timer / 60, timer % 60, hundredth);

			for (txt = buffer; *txt; txt++)
			{
				if (*txt == ':')
				{
					x -= 6;
					S_DrawScreenSprite2d(x, 36, 0, 0x10000, 0x10000, objects[ASSAULT_NUMBERS].mesh_index + 10, col, 0);
					x += 14;
				}
				else if (*txt == '.')
				{
					x -= 6;
					S_DrawScreenSprite2d(x, 36, 0, 0x10000, 0x10000, objects[ASSAULT_NUMBERS].mesh_index + 11, col, 0);
					x += 14;
				}
				else
				{
					S_DrawScreenSprite2d(x, 36, 0, 0x10000, 0x10000, *txt + objects[ASSAULT_NUMBERS].mesh_index - '0', col, 0);
					x += 20;
				}
			}
		}
	}
}

void inject_health(bool replace)
{
	INJECT(0x00434360, FlashIt, replace);
	INJECT(0x00434390, DrawAssaultTimer, replace);
	INJECT(0x00434510, DrawAssaultPenalties, replace);
	INJECT(0x00434770, DrawQuadbikeLapTime, replace);
}
