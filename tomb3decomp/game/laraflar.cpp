#include "../tomb3/pch.h"
#include "laraflar.h"
#include "draw.h"
#include "../3dsystem/3d_gen.h"
#include "../specific/output.h"
#include "objects.h"
#include "../specific/litesrc.h"
#include "../specific/function_stubs.h"
#include "effect2.h"

void DrawFlareInAir(ITEM_INFO* item)
{
	short* frame[2];
	long rate, clip, x, y, z;

	GetFrames(item, frame, &rate);
	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
	clip = S_GetObjectBounds(frame[0]);

	if (clip)
	{
		CalculateObjectLighting(item, frame[0]);
		phd_PutPolygons(meshes[objects[FLARE_ITEM].mesh_index], clip);

		if ((long)item->data & 0x8000)
		{
			phd_TranslateRel(-6, 6, 48);
			phd_RotX(-16380);
			S_CalculateStaticLight(2048);

			phd_PushUnitMatrix();
			phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);

			phd_PushMatrix();
			phd_TranslateRel(-6, 6, 32);
			x = item->pos.x_pos + (phd_mxptr[M03] >> W2V_SHIFT);
			y = item->pos.y_pos + (phd_mxptr[M13] >> W2V_SHIFT);
			z = item->pos.z_pos + (phd_mxptr[M23] >> W2V_SHIFT);
			phd_PopMatrix();

			phd_TranslateRel((GetRandomDraw() & 0x7F) - 64, (GetRandomDraw() & 0x7F) - 64, (GetRandomDraw() & 0x1FF) + 512);

			for (int i = 0; i < (GetRandomDraw() & 3) + 4; i++)
				TriggerFlareSparks(x, y, z, phd_mxptr[M03] >> W2V_SHIFT, phd_mxptr[M13] >> W2V_SHIFT, phd_mxptr[M23] >> W2V_SHIFT, i >> 2, 0);

			phd_PopMatrix();
		}
	}

	phd_PopMatrix();
}

void inject_laraflar(bool replace)
{
	INJECT(0x0044BBD0, DrawFlareInAir, replace);
}
