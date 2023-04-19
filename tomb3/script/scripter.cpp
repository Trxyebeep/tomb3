#include "../tomb3/pch.h"
#include "scripter.h"
#include "../game/gameflow.h"
#include "interpreter.h"

static char* OptionToText(long opt)
{
	static char buf[128];

	if (opt == EXIT_TO_TITLE)
		sprintf(buf, "EXIT_TO_TITLE");
	else if (opt & STARTDEMO)
		sprintf(buf, "DEMO %i", opt & ~STARTDEMO);	//check me
	else if (opt == EXITGAME)
		sprintf(buf, "EXITGAME");
	else
		sprintf(buf, "%i", opt);

	return buf;
}

char* LanguageToText(uchar language)
{
	static char buf[64];

	switch (language)
	{
	case 0:
		sprintf(buf, "ENGLISH");
		break;

	case 1:
		sprintf(buf, "FRENCH");
		break;

	case 2:
		sprintf(buf, "GERMAN");
		break;

	case 3:
		sprintf(buf, "AMERICAN");
		break;

	case 4:
		sprintf(buf, "JAPANESE");
		break;

	case 5:
		sprintf(buf, "ITALIAN");
		break;

	case 6:
		sprintf(buf, "SPANISH");
		break;

	default:
		sprintf(buf, "ENGLISH");
		break;
	}

	return buf;
}

static void WriteDescription(FILE* fp)
{
	fprintf(fp, "Description: %s\n", GF_Description);
	fprintf(fp, "\n");
	fprintf(fp, "/********************************************************************************************/\n");
	fprintf(fp, "\n");
}

static void WriteMainOptions(FILE* fp)
{
	fprintf(fp, "Options:\n");

	fprintf(fp, "firstoption: %s\n", OptionToText(gameflow.firstOption));
	fprintf(fp, "title_replace: %i\n", gameflow.title_replace);
	fprintf(fp, "ondeath_demo_mode: %s\n", OptionToText(gameflow.ondeath_demo_mode));

	if (gameflow.ondeath_ingame)
		fprintf(fp, "ondeath_ingame: %s\n", OptionToText(gameflow.ondeath_ingame));

	fprintf(fp, "noinput_time: %i\n", gameflow.noinput_time);
	fprintf(fp, "on_demo_interrupt: %s\n", OptionToText(gameflow.on_demo_interrupt));
	fprintf(fp, "on_demo_end: %s\n", OptionToText(gameflow.on_demo_end));

	fprintf(fp, "singlelevel: %i\n", gameflow.singlelevel);

	if (gameflow.demoversion) fprintf(fp, "demoversion:\n");
	if (gameflow.title_disabled) fprintf(fp, "title_disabled:\n");
	if (gameflow.cheatmodecheck_disabled) fprintf(fp, "cheatmodecheck_disabled:\n");
	if (gameflow.noinput_timeout) fprintf(fp, "noinput_timeout:\n");
	if (gameflow.loadsave_disabled) fprintf(fp, "loadsave_disabled:\n");
	if (gameflow.screensizing_disabled) fprintf(fp, "screensizing_disabled:\n");
	if (gameflow.lockout_optionring) fprintf(fp, "lockout_optionring:\n");
	if (gameflow.dozy_cheat_enabled) fprintf(fp, "dozy_cheat_enabled:\n");
	if (gameflow.cyphered_strings) fprintf(fp, "cypher_code: %i\n", gameflow.cypher_code);
	if (gameflow.play_any_level) fprintf(fp, "select_any_level:\n");
	if (gameflow.cheat_enable) fprintf(fp, "enable_cheat_key:\n");
	if (gameflow.securitytag) fprintf(fp, "use_security_tag:\n");

	fprintf(fp, "Map: %i\n", gameflow.map_enabled);
	fprintf(fp, "Globe: %i\n", gameflow.globe_enabled);
	fprintf(fp, "Water: %i\n", gameflow.force_water_color);

	fprintf(fp, "language: %s\n", LanguageToText(gameflow.language));
	fprintf(fp, "secret_track: %i\n", gameflow.secret_track);

	fprintf(fp, "end:\n");

	fprintf(fp, "\n");
	fprintf(fp, "/********************************************************************************************/\n");
	fprintf(fp, "\n");
}

static void WriteTitleOptions(FILE* fp)
{
	fprintf(fp, "Title:\n");
	fprintf(fp, "Game: %s\n", GF_titlefilenames[0]);

	for (int i = 1; i < gameflow.num_titlefiles; i++)
		fprintf(fp, "PCfile: %s\n", GF_titlefilenames[i]);

	fprintf(fp, "track: %i\n", gameflow.title_track);

	fprintf(fp, "end:\n");

	fprintf(fp, "\n");
	fprintf(fp, "/********************************************************************************************/\n");
	fprintf(fp, "\n");
}

static void WriteFrontEndSequence(FILE* fp)
{
	fprintf(fp, "Frontend:\n");
	fprintf(fp, "PCfmv: %s\n", GF_fmvfilenames[0]);
	fprintf(fp, "PCfmv: %s\n", GF_fmvfilenames[1]);
	fprintf(fp, "fmv: %s\n", GF_fmvfilenames[2]);
	fprintf(fp, "fmv: %s\n", GF_fmvfilenames[3]);
	fprintf(fp, "end:\n");

	fprintf(fp, "\n");
	fprintf(fp, "/********************************************************************************************/\n");
	fprintf(fp, "\n");
}

static void WriteGymSequence(FILE* fp)
{
	fprintf(fp, "Gym: %s\n", GF_Level_Names[0]);
	interpretSequence(fp, GF_level_sequence_list[0], 0);
	fprintf(fp, "end:\n");

	fprintf(fp, "\n");
	fprintf(fp, "/********************************************************************************************/\n");
	fprintf(fp, "\n");
}

static void WriteLevelSequences(FILE* fp)
{
	long total, lp;

	total = gameflow.num_levels - gameflow.num_demos;

	for (lp = 1; lp < total; lp++)
	{
		fprintf(fp, "//Level %i\n", lp);
		fprintf(fp, "Level: %s\n", GF_Level_Names[lp]);
		interpretSequence(fp, GF_level_sequence_list[lp], lp);
		fprintf(fp, "end:\n");
		fprintf(fp, "\n");
	}

	fprintf(fp, "/********************************************************************************************/\n");
	fprintf(fp, "\n");
}

static void WriteDemoLevelSequences(FILE* fp)
{
	long total, lp;

	total = gameflow.num_levels - gameflow.num_demos;

	for (lp = gameflow.num_levels; lp < total; lp++)	//I think that's right
	{
		fprintf(fp, "//Demo Level %i\n", lp);
		fprintf(fp, "DemoLevel: %s\n", GF_Level_Names[lp]);
		interpretSequence(fp, GF_level_sequence_list[lp], lp);
		fprintf(fp, "end:\n");
		fprintf(fp, "\n");
	}

	if (lp == gameflow.num_levels)
	{
		fprintf(fp, "//Demo levels go here\n");
		fprintf(fp, "\n");
	}

	fprintf(fp, "/********************************************************************************************/\n");
	fprintf(fp, "\n");
}

static void OutputStrings(FILE* fp)
{
	long lp;

	fprintf(fp, "game_strings:\n");
	fprintf(fp, "\n");

	for (lp = 0; lp < GT_NUM_GAMESTRINGS; lp++)
		fprintf(fp, "%s\n", GF_GameStrings[lp]);

	fprintf(fp, "\n");
	fprintf(fp, "end:\n");
	fprintf(fp, "\n");

	fprintf(fp, "/********************************************************************************************/\n");
	fprintf(fp, "\n");

	fprintf(fp, "pc_strings:\n");
	fprintf(fp, "\n");

	for (lp = 0; lp < PCSTR_NUM_STRINGS; lp++)
		fprintf(fp, "%s\n", GF_PCStrings[lp]);

	fprintf(fp, "\n");
	fprintf(fp, "end:\n");
}

void OutputScript()
{
	FILE* fp;
	long l, lp;
	char lang[128];

	fp = fopen("tombpc.txt", "wb");

	WriteDescription(fp);
	WriteMainOptions(fp);
	WriteTitleOptions(fp);
	WriteFrontEndSequence(fp);
	WriteGymSequence(fp);
	WriteLevelSequences(fp);
	WriteDemoLevelSequences(fp);

	strcpy(lang, LanguageToText(gameflow.language));
	strcat(lang, ".TXT");
	l = strlen(lang);

	for (lp = 0; lp < l; lp++)
		lang[lp] = tolower(lang[lp]);

	fprintf(fp, "Gamestrings: %s\n", lang);
	fclose(fp);

	fp = fopen(lang, "wb");
	OutputStrings(fp);
	fclose(fp);
}
