#include "../tomb3/pch.h"
#include "picture.h"
#include "hwrender.h"
#include "dx.h"
#include "time.h"

void CrossFadePicture()
{
	for (int i = 0, j = 255; i < 256; i += 8, j -= 8)
	{
		HWR_BeginScene();
		DrawPictureAlpha(i, CurPicTexIndices);
		DrawPictureAlpha(j, OldPicTexIndices);
		HWR_EndScene();
		DX_UpdateFrame(1, 0);
	}

	FreePictureTextures(CurPicTexIndices);
	CurPicTexIndices[0] = OldPicTexIndices[0];
	CurPicTexIndices[1] = OldPicTexIndices[1];
	CurPicTexIndices[2] = OldPicTexIndices[2];
	CurPicTexIndices[3] = OldPicTexIndices[3];
	CurPicTexIndices[4] = OldPicTexIndices[4];
	HWR_EnableColorKey(0);
	HWR_EnableAlphaBlend(0);
	HWR_EnableColorAddition(0);

	if (dontFadePicture)
		pictureFading = 0;
	else
		pictureFading = 1;

	HWR_BeginScene();
	TRDrawPicture(0, CurPicTexIndices);
	HWR_EndScene();
	DX_UpdateFrame(1, 0);
	TIME_Init();
}

void inject_picture(bool replace)
{
	INJECT(0x0048AFD0, CrossFadePicture, replace);
}
