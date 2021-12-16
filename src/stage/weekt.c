/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "weekt.h"

#include "../mem.h"
#include "../archive.h"

//Week Tricky background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back2; //Window
	Gfx_Tex tex_back1; //Window
	Gfx_Tex tex_back0;

	//Cutscene
	Gfx_Tex tex_cut0; 
	Gfx_Tex tex_cut1;
	Gfx_Tex tex_cut2; 
	Gfx_Tex tex_cut3;
	Gfx_Tex tex_cut4;
	Gfx_Tex tex_cut5;
} Back_WeekT;

void Back_WeekT_DrawFG(StageBack* back)
{
	Back_WeekT* this = (Back_WeekT*)back;

	fixed_t fx, fy;
	fx = stage.camera.x;
	fy = stage.camera.y;

	if (stage.stage_id == StageId_1_1 && stage.song_step >= 1424 && stage.song_step <= 1482)
	{
		RECT cutscene0_src = { 0, 0, 256, 256 };
		RECT_FIXED cutscene0_dst = {
			FIXED_DEC(-115,1) - fx,
			FIXED_DEC(-100,1) - fy,
			FIXED_DEC(350,1),
			FIXED_DEC(275,1)
		};

		Stage_DrawTex(&this->tex_cut0, &cutscene0_src, &cutscene0_dst, stage.camera.bzoom);
	}

	if (stage.stage_id == StageId_1_1 && stage.song_step >= 1482 && stage.song_step <= 1492)
	{
		RECT cutscene1_src = { 0, 0, 256, 256 };
		RECT_FIXED cutscene1_dst = {
			FIXED_DEC(-115,1) - fx,
			FIXED_DEC(-100,1) - fy,
			FIXED_DEC(350,1),
			FIXED_DEC(275,1)
		};

		Stage_DrawTex(&this->tex_cut1, &cutscene1_src, &cutscene1_dst, stage.camera.bzoom);
	}

	if (stage.stage_id == StageId_1_1 && stage.song_step >= 1492 && stage.song_step <= 1520)
	{
		RECT cutscene2_src = { 0, 0, 256, 256 };
		RECT_FIXED cutscene2_dst = {
			FIXED_DEC(-115,1) - fx,
			FIXED_DEC(-100,1) - fy,
			FIXED_DEC(350,1),
			FIXED_DEC(275,1)
		};

		Stage_DrawTex(&this->tex_cut2, &cutscene2_src, &cutscene2_dst, stage.camera.bzoom);
	}

	if (stage.stage_id == StageId_1_1 && stage.song_step >= 1520 && stage.song_step <= 1540)
	{
		RECT cutscene3_src = { 0, 0, 256, 256 };
			RECT_FIXED cutscene3_dst = {
				FIXED_DEC(-115,1) - fx,
				FIXED_DEC(-100,1) - fy,
				FIXED_DEC(350,1),
				FIXED_DEC(275,1)
		};

			Stage_DrawTex(&this->tex_cut3, &cutscene3_src, &cutscene3_dst, stage.camera.bzoom);
	}


	if (stage.stage_id == StageId_1_1 && stage.song_step >= 1540 && stage.song_step <= 1570)
	{
		RECT cutscene4_src = { 0, 0, 256, 256 };
		RECT_FIXED cutscene4_dst = {
			FIXED_DEC(-115,1) - fx,
			FIXED_DEC(-100,1) - fy,
			FIXED_DEC(350,1),
			FIXED_DEC(275,1)
		};

		Stage_DrawTex(&this->tex_cut4, &cutscene4_src, &cutscene4_dst, stage.camera.bzoom);
	}

	if (stage.stage_id == StageId_1_1 && stage.song_step >= 1570 && stage.song_step <= 1700)
	{
		RECT cutscene5_src = { 0, 0, 256, 256 };
		RECT_FIXED cutscene5_dst = {
			FIXED_DEC(-160,1) - fx,
			FIXED_DEC(-160,1) - fy,
			FIXED_DEC(425,1),
			FIXED_DEC(350,1)
		};

		Stage_DrawTex(&this->tex_cut5, &cutscene5_src, &cutscene5_dst, stage.camera.bzoom);
	}

	if (stage.stage_id == StageId_1_1 && stage.song_step >= 1424)
	{
		RECT back0_src = { 0, 0, 256, 256 };
		RECT_FIXED back0_dst = {
			FIXED_DEC(-100,1) - fx,
			FIXED_DEC(-150,1) - fy,
			FIXED_DEC(800,1),
			FIXED_DEC(800,1)
		};

		Stage_DrawTex(&this->tex_back0, &back0_src, &back0_dst, stage.camera.bzoom);
	}

}

//Week Tricky background functions
void Back_WeekT_DrawBG(StageBack *back)
{
	Back_WeekT *this = (Back_WeekT*)back;
	
	fixed_t fx, fy;
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	//Draw window
	RECT window_src = {0, 0, 256, 226};
	RECT_FIXED window_dst = {
		FIXED_DEC(-500,1) - fx,
		FIXED_DEC(-165,1) - fy,
		FIXED_DEC(512,1),
		FIXED_DEC(400,1)
	};
	
	Stage_DrawTex(&this->tex_back1, &window_src, &window_dst, stage.camera.bzoom);

	//Draw window
	RECT window1_src = { 0, 0, 256, 226};
	RECT_FIXED window1_dst = {
		FIXED_DEC(3,1) - fx,
		FIXED_DEC(-165,1) - fy,
		FIXED_DEC(512,1),
		FIXED_DEC(400,1)
	};

	Stage_DrawTex(&this->tex_back2, &window1_src, &window1_dst, stage.camera.bzoom);
}

void Back_WeekT_Free(StageBack *back)
{
	Back_WeekT *this = (Back_WeekT*)back;
	
	//Free structure
	Mem_Free(this);

}

StageBack *Back_WeekT_New(void)
{
	//Allocate background structure
	Back_WeekT *this = (Back_WeekT*)Mem_Alloc(sizeof(Back_WeekT));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = Back_WeekT_DrawFG;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_WeekT_DrawBG;
	this->back.free = Back_WeekT_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\WEEKT\\BACK.ARC;1");
	Gfx_LoadTex(&this->tex_back2, Archive_Find(arc_back, "back2.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_cut0, Archive_Find(arc_back, "cut0.tim"), 0);
	Gfx_LoadTex(&this->tex_cut1, Archive_Find(arc_back, "cut1.tim"), 0);
	Gfx_LoadTex(&this->tex_cut2, Archive_Find(arc_back, "cut2.tim"), 0);
	Gfx_LoadTex(&this->tex_cut3, Archive_Find(arc_back, "cut3.tim"), 0);
	Gfx_LoadTex(&this->tex_cut4, Archive_Find(arc_back, "cut4.tim"), 0);
	Gfx_LoadTex(&this->tex_cut5, Archive_Find(arc_back, "cut5.tim"), 0);
	Mem_Free(arc_back);
	
	return (StageBack*)this;
}
