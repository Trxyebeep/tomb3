#include "../tomb3/pch.h"
#include "invfunc.h"
#include "../specific/frontend.h"
#include "../specific/option.h"
#include "text.h"

void InitColours()
{
	inv_colours[0] = S_COLOUR(0, 0, 0);
	inv_colours[1] = S_COLOUR(64, 64, 64);
	inv_colours[2] = S_COLOUR(255, 255, 255);
	inv_colours[3] = S_COLOUR(255, 0, 0);
	inv_colours[4] = S_COLOUR(255, 128, 0);
	inv_colours[5] = S_COLOUR(255, 255, 0);
	inv_colours[12] = S_COLOUR(0, 128, 0);
	inv_colours[13] = S_COLOUR(0, 255, 0);
	inv_colours[14] = S_COLOUR(0, 255, 255);
	inv_colours[15] = S_COLOUR(0, 0, 255);
	inv_colours[16] = S_COLOUR(255, 0, 255);
}

/************Requester stuff************/

void Init_Requester(REQUEST_INFO* req)
{
	req->background_flags = 1;
	req->moreup_flags = 1;
	req->moredown_flags = 1;
	req->item = 0;
	req->heading1text = 0;
	req->heading2text = 0;
	req->heading1_flags = 0;
	req->heading2_flags = 0;
	req->backgroundtext = 0;
	req->moreuptext = 0;
	req->moredowntext = 0;

	for (int i = 0; i < 24; i++)
	{
		req->texts1[i] = 0;
		req->texts2[i] = 0;
		req->texts1_flags[i] = 0;
		req->texts2_flags[i] = 0;
	}

	req->itemtexts1_flags = RequesterFlags1;
	req->itemtexts2_flags = RequesterFlags2;
	req->original_render_width = GetRenderWidth();
	req->original_render_height = GetRenderHeight();
}

void Remove_Requester(REQUEST_INFO* req)
{
	T_RemovePrint(req->heading1text);
	req->heading1text = 0;

	T_RemovePrint(req->heading2text);
	req->heading2text = 0;

	T_RemovePrint(req->backgroundtext);
	req->backgroundtext = 0;

	T_RemovePrint(req->moreuptext);
	req->moreuptext = 0;

	T_RemovePrint(req->moredowntext);
	req->moredowntext = 0;

	for (int i = 0; i < 24; i++)
	{
		T_RemovePrint(req->texts1[i]);
		req->texts1[i] = 0;

		T_RemovePrint(req->texts2[i]);
		req->texts2[i] = 0;
	}
}

void ReqItemCentreAlign(REQUEST_INFO* req, TEXTSTRING* txt)
{
	if (txt)
	{
		txt->bgndOffX = 0;
		txt->xpos = req->xpos;
	}
}

void ReqItemLeftalign(REQUEST_INFO* req, TEXTSTRING* txt)
{
	ulong h;

	h = GetTextScaleH(txt->scaleH);

	if (txt)
	{
		txt->bgndOffX = short(((h * req->pixwidth) >> 17) - ((8 * h) >> 16) - T_GetTextWidth(txt) / 2);
		txt->xpos = req->xpos - txt->bgndOffX;
	}
}

void ReqItemRightalign(REQUEST_INFO* req, TEXTSTRING* txt)
{
	ulong h;

	h = GetTextScaleH(txt->scaleH);

	if (txt)
	{
		txt->bgndOffX = -short(((h * req->pixwidth) >> 17) - ((8 * h) >> 16) - T_GetTextWidth(txt) / 2);
		txt->xpos = req->xpos - txt->bgndOffX;
	}
}

void inject_invfunc(bool replace)
{
	INJECT(0x00437050, InitColours, replace);
	INJECT(0x00439150, Init_Requester, replace);
	INJECT(0x004391E0, Remove_Requester, replace);
	INJECT(0x00439290, ReqItemCentreAlign, replace);
	INJECT(0x004392B0, ReqItemLeftalign, replace);
	INJECT(0x00439310, ReqItemRightalign, replace);
}
