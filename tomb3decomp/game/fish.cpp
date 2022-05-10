#include "../tomb3/pch.h"
#include "fish.h"
#include "gameflow.h"

uchar jungle_fish_ranges[1][3] =
{
	{ 8, 20, 3 }
};

uchar temple_fish_ranges[3][3] =
{
	{ 4, 4, 2 },
	{ 4, 16, 2 },
	{ 4, 28, 3 }
};

uchar quadchase_fish_ranges[8][3] =
{
	{ 4, 12, 1 },
	{ 0, 12, 2 },
	{ 8, 4, 2 },
	{ 4, 8, 1 },
	{ 4, 16, 2 },
	{ 4, 24, 1 },
	{ 12, 4, 1 },
	{ 16, 4, 1 }
};

uchar house_fish_ranges[7][3]
{
	{ 4, 4, 1 },
	{ 16, 8, 2 },
	{ 24, 8, 2 },
	{ 8, 16, 2 },
	{ 8, 12, 1 },
	{ 20, 8, 2 },
	{ 16, 8, 1 }
};

uchar shore_fish_ranges[3][3] =
{
	{ 12, 12, 6 },
	{ 12, 20, 6 },
	{ 20, 4, 8 }
};

uchar crash_fish_ranges[1][3]
{
	{ 20, 4, 6 }
};

uchar rapids_fish_ranges[2][3]
{
	{ 16, 16, 8 },
	{ 4, 8, 5 }
};

void SetupShoal(long shoal_number)
{
	LEADER_INFO* leader;

	leader = &lead_info[shoal_number];

	if (CurrentLevel == LV_JUNGLE)
	{
		leader->Xrange = (jungle_fish_ranges[shoal_number][0] + 2) << 8;
		leader->Yrange = jungle_fish_ranges[shoal_number][2] << 8;
		leader->Zrange = (jungle_fish_ranges[shoal_number][1] + 2) << 8;
	}
	else if (CurrentLevel == LV_TEMPLE)
	{
		leader->Xrange = (temple_fish_ranges[shoal_number][0] + 2) << 8;
		leader->Yrange = temple_fish_ranges[shoal_number][2] << 8;
		leader->Zrange = (temple_fish_ranges[shoal_number][1] + 2) << 8;
	}
	else if (CurrentLevel == LV_QUADBIKE)
	{
		leader->Xrange = (quadchase_fish_ranges[shoal_number][0] + 2) << 8;
		leader->Yrange = quadchase_fish_ranges[shoal_number][2] << 8;
		leader->Zrange = (quadchase_fish_ranges[shoal_number][1] + 2) << 8;
	}
	else if (CurrentLevel == LV_GYM)
	{
		leader->Xrange = (house_fish_ranges[shoal_number][0] + 2) << 8;
		leader->Yrange = house_fish_ranges[shoal_number][2] << 8;
		leader->Zrange = (house_fish_ranges[shoal_number][1] + 2) << 8;
	}
	else if (CurrentLevel == LV_SHORE)
	{
		leader->Xrange = (shore_fish_ranges[shoal_number][0] + 2) << 8;
		leader->Yrange = shore_fish_ranges[shoal_number][2] << 8;
		leader->Zrange = (shore_fish_ranges[shoal_number][1] + 2) << 8;
	}
	else if (CurrentLevel == LV_CRASH)
	{
		leader->Xrange = (crash_fish_ranges[shoal_number][0] + 2) << 8;
		leader->Yrange = (crash_fish_ranges[shoal_number][2]) << 8;
		leader->Zrange = (crash_fish_ranges[shoal_number][1] + 2) << 8;
	}
	else if (CurrentLevel == LV_RAPIDS)
	{
		leader->Xrange = (rapids_fish_ranges[shoal_number][0] + 2) << 8;
		leader->Yrange = rapids_fish_ranges[shoal_number][2] << 8;
		leader->Zrange = (rapids_fish_ranges[shoal_number][1] + 2) << 8;
	}
	else
	{
		leader->Xrange = 256;
		leader->Zrange = 256;
		leader->Yrange = 256;
	}
}

void inject_fish(bool replace)
{
	INJECT(0x00430050, SetupShoal, replace);
}
