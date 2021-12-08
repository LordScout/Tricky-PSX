/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "week22.h"

#include "../mem.h"
#include "../archive.h"

//Week 2 background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back1; //Window
	Gfx_Tex tex_back0;

	//Cutscene
	Gfx_Tex tex_cut0; 
} Back_Week2;

void Back_Week22_DrawFG(StageBack* back)
{
	Back_Week2* this = (Back_Week2*)back;

	fixed_t fx, fy;
	fx = stage.camera.x;
	fy = stage.camera.y;

	if (stage.stage_id == StageId_1_2 && stage.song_step >= 2172 && stage.song_step <= 2214)
	{
		RECT cutscene0_src = { 0, 0, 249, 26 };
		RECT_FIXED cutscene0_dst = {
			FIXED_DEC(-180,1) - fx,
			FIXED_DEC(50,1) - fy,
			FIXED_DEC(256,1),
			FIXED_DEC(26,1)
		};

		Stage_DrawTex(&this->tex_cut0, &cutscene0_src, &cutscene0_dst, stage.camera.bzoom);
	}

	if (stage.stage_id == StageId_1_2 && stage.song_step >= 2230 && stage.song_step <= 2260)
	{
		RECT cutscene1_src = { 0, 110, 135, 28};
		RECT_FIXED cutscene1_dst = {
			FIXED_DEC(-140,1) - fx,
			FIXED_DEC(50,1) - fy,
			FIXED_DEC(131,1),
			FIXED_DEC(24,1)
		};

		Stage_DrawTex(&this->tex_cut0, &cutscene1_src, &cutscene1_dst, stage.camera.bzoom);
	}

	if (stage.stage_id == StageId_1_2 && stage.song_step >= 2294 && stage.song_step <= 2330)
	{
		RECT cutscene2_src = { 0, 25, 256, 28 };
		RECT_FIXED cutscene2_dst = {
			FIXED_DEC(-160,1) - fx,
			FIXED_DEC(70,1) - fy,
			FIXED_DEC(248,1),
			FIXED_DEC(24,1)
		};

		Stage_DrawTex(&this->tex_cut0, &cutscene2_src, &cutscene2_dst, stage.camera.bzoom);
	}

}

//Week 2 background functions
void Back_Week22_DrawBG(StageBack *back)
{
	Back_Week2 *this = (Back_Week2*)back;
	
	fixed_t fx, fy;
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	//Draw window
	RECT window_src = {0, 0, 256, 123};
	RECT_FIXED window_dst = {
		FIXED_DEC(-400,1) - fx,
		FIXED_DEC(-200,1) - fy,
		FIXED_DEC(896,1),
		FIXED_DEC(475,1)
	};
	
	Stage_DrawTex(&this->tex_back1, &window_src, &window_dst, stage.camera.bzoom);
	
	#if SCREEN_WIDTH > 320
		RECT backl_src = {0, 0, 1, 256};
		RECT_FIXED backl_dst = {
			FIXED_DEC(-185,1) - FIXED_DEC(SCREEN_WIDEADD,2) - fx,
			FIXED_DEC(-125,1) - fy,
			FIXED_DEC(SCREEN_WIDEADD,2),
			FIXED_DEC(267,1)
		};
		RECT backr_src = {255, 0, 0, 256};
		RECT_FIXED backr_dst = {
			FIXED_DEC(168,1) - fx,
			FIXED_DEC(-125,1) - fy,
			FIXED_DEC(SCREEN_WIDEADD,2),
			FIXED_DEC(267,1)
		};
		
		Stage_DrawTex(&this->tex_back0, &backl_src, &backl_dst, stage.camera.bzoom);
		Stage_DrawTex(&this->tex_back0, &backr_src, &backr_dst, stage.camera.bzoom);
	#endif
}

void Back_Week22_Free(StageBack *back)
{
	Back_Week2 *this = (Back_Week2*)back;
	
	//Free structure
	Mem_Free(this);

}

StageBack *Back_Week22_New(void)
{
	//Allocate background structure
	Back_Week2 *this = (Back_Week2*)Mem_Alloc(sizeof(Back_Week2));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = Back_Week22_DrawFG;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Week22_DrawBG;
	this->back.free = Back_Week22_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\WEEK22\\BACK.ARC;1");
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_cut0, Archive_Find(arc_back, "cut0.tim"), 0);
	Mem_Free(arc_back);
	
	return (StageBack*)this;
}
