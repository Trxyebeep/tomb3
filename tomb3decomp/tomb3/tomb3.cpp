#include "../tomb3/pch.h"
#include "tomb3.h"

TOMB3_OPTIONS tomb3;

static void T3_InitSettings()
{
	tomb3.footprints = 1;
	tomb3.shadow_mode = SHADOW_PSX;
	tomb3.bar_mode = BAR_PSX;
}

void T3_SaveSettings()
{
	FILE* file;

	file = fopen("tomb3.txt", "w+");

	if (!file)
		return;

	fprintf(file, "footprints %d\n", tomb3.footprints);
	fprintf(file, "shadowmode %d\n", tomb3.shadow_mode);
	fprintf(file, "barmode %d\n", tomb3.bar_mode);
	fprintf(file, "pickupdisplay %d\n", tomb3.pickup_display);
	fclose(file);
}

void T3_LoadSettings()
{
	FILE* file;
	long f;
	char s[80];

	file = fopen("tomb3.txt", "r");

	if (!file)
	{
		T3_InitSettings();
		T3_SaveSettings();
		return;
	}

	f = fscanf(file, "%s", &s);	//footprints
	f = fscanf(file, "%d\n", &tomb3.footprints);

	f = fscanf(file, "%s", &s);	//shadow
	f = fscanf(file, "%d\n", &tomb3.shadow_mode);

	f = fscanf(file, "%s", &s);	//bars
	f = fscanf(file, "%d\n", &tomb3.bar_mode);

	f = fscanf(file, "%s", &s);	//pickup display
	f = fscanf(file, "%d\n", &tomb3.pickup_display);

	fclose(file);
}
