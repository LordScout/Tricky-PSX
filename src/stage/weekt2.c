/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "weekt2.h"

#include "../mem.h"
#include "../archive.h"

//Week Tricky 2 background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back1; //Window
	Gfx_Tex tex_back0;

	//Cutscene
	Gfx_Tex tex_cut0; 
} Back_WeekT2;

void Back_WeekT2_DrawFG(StageBack* back)
{
	Back_WeekT2* this = (Back_WeekT2*)back;

	fixed_t fx, fy;
	fx = stage.camera.x;
	fy = stage.camera.y;

	if (stage.stage_id == StageId_1_2 && stage.song_step >= 128 && stage.song_step <= 130)
	{
		RECT suffer_src = { 0, 57, 85, 24};
		RECT_FIXED suffer_dst = {
			FIXED_DEC(-40,1) - fx,
			FIXED_DEC(0,1) - fy,
			FIXED_DEC(85,1),
			FIXED_DEC(24,1)
		};

		Stage_DrawTex(&this->tex_cut0, &suffer_src, &suffer_dst, stage.camera.bzoom);
	}

	if (stage.stage_id == StageId_1_2 && stage.song_step >= 220 && stage.song_step <= 222)
	{
		RECT incorrect1_src = { 0, 110, 135, 28 };
		RECT_FIXED incorrect1_dst = {
			FIXED_DEC(-50,1) - fx,
			FIXED_DEC(-50,1) - fy,
			FIXED_DEC(135,1),
			FIXED_DEC(28,1)
		};

		Stage_DrawTex(&this->tex_cut0, &incorrect1_src, &incorrect1_dst, stage.camera.bzoom);
	}

	if (stage.stage_id == StageId_1_2 && stage.song_step >= 272 && stage.song_step <= 274)
	{
		RECT invalid_src = { 5, 139, 87, 24 };
		RECT_FIXED invalid_dst = {
			FIXED_DEC(-50,1) - fx,
			FIXED_DEC(0,1) - fy,
			FIXED_DEC(87,1),
			FIXED_DEC(24,1)
		};

		Stage_DrawTex(&this->tex_cut0, &invalid_src, &invalid_dst, stage.camera.bzoom);
	}

	if (stage.stage_id == StageId_1_2 && stage.song_step >= 280 && stage.song_step <= 282)
	{
		RECT error_src = { 0, 83, 72, 24};
		RECT_FIXED error_dst = {
			FIXED_DEC(-40,1) - fx,
			FIXED_DEC(15,1) - fy,
			FIXED_DEC(72,1),
			FIXED_DEC(24,1)
		};

		Stage_DrawTex(&this->tex_cut0, &error_src, &error_dst, stage.camera.bzoom);
	}

	if (stage.stage_id == StageId_1_2 && stage.song_step >= 597 && stage.song_step <= 600)
	{
		RECT error9_src = { 0, 83, 72, 24 };
		RECT_FIXED error9_dst = {
			FIXED_DEC(-40,1) - fx,
			FIXED_DEC(15,1) - fy,
			FIXED_DEC(72,1),
			FIXED_DEC(24,1)
		};

		Stage_DrawTex(&this->tex_cut0, &error9_src, &error9_dst, stage.camera.bzoom);
	}

	if (stage.stage_id == StageId_1_2 && stage.song_step >= 316 && stage.song_step <= 318)
	{
		RECT suffer1_src = { 0, 57, 85, 24 };
		RECT_FIXED suffer1_dst = {
			FIXED_DEC(-80,1) - fx,
			FIXED_DEC(-50,1) - fy,
			FIXED_DEC(85,1),
			FIXED_DEC(24,1)
		};

		Stage_DrawTex(&this->tex_cut0, &suffer1_src, &suffer1_dst, stage.camera.bzoom);
	}

	if (stage.stage_id == StageId_1_2 && stage.song_step >= 396 && stage.song_step <= 398)
	{
		RECT terrible_src = { 104, 225, 106, 24};
		RECT_FIXED terrible_dst = {
			FIXED_DEC(-80,1) - fx,
			FIXED_DEC(-15,1) - fy,
			FIXED_DEC(106,1),
			FIXED_DEC(24,1)
		};

		Stage_DrawTex(&this->tex_cut0, &terrible_src, &terrible_dst, stage.camera.bzoom);
	}

	if (stage.stage_id == StageId_1_2 && stage.song_step >= 410 && stage.song_step <= 414)
	{
		RECT waste_src = { 0, 223, 79, 24 };
		RECT_FIXED waste_dst = {
			FIXED_DEC(-130,1) - fx,
			FIXED_DEC(50,1) - fy,
			FIXED_DEC(79, 1),
			FIXED_DEC(24,1)
		};

		Stage_DrawTex(&this->tex_cut0, &waste_src, &waste_dst, stage.camera.bzoom);
	}

	if (stage.stage_id == StageId_1_2 && stage.song_step >= 542 && stage.song_step <= 544)
	{
		RECT correction_src = {96, 57, 145, 24 };
		RECT_FIXED correction_dst = {
			FIXED_DEC(-110,1) - fx,
			FIXED_DEC(40,1) - fy,
			FIXED_DEC(145, 1),
			FIXED_DEC(24,1)
		};

		Stage_DrawTex(&this->tex_cut0, &correction_src, &correction_dst, stage.camera.bzoom);
	}


	if (stage.stage_id == StageId_1_2 && stage.song_step >= 2172 && stage.song_step <= 2214)
	{
		RECT youclown_src = { 0, 0, 249, 26 };
		RECT_FIXED youclown_dst = {
			FIXED_DEC(-180,1) - fx,
			FIXED_DEC(50,1) - fy,
			FIXED_DEC(256,1),
			FIXED_DEC(26,1)
		};

		Stage_DrawTex(&this->tex_cut0, &youclown_src, &youclown_dst, stage.camera.bzoom);
	}

	if (stage.stage_id == StageId_1_2 && stage.song_step >= 606 && stage.song_step <= 608) 
	{
		RECT incorrect_src = { 0, 110, 135, 28};
		RECT_FIXED incorrect_dst = {
			FIXED_DEC(-140,1) - fx,
			FIXED_DEC(50,1) - fy,
			FIXED_DEC(131,1),
			FIXED_DEC(24,1)
		};

		Stage_DrawTex(&this->tex_cut0, &incorrect_src, &incorrect_dst, stage.camera.bzoom);
	}

	if (stage.stage_id == StageId_1_2 && stage.song_step >= 670 && stage.song_step <= 673)
	{
		RECT mistake_src = { 0, 192, 100, 24 };
		RECT_FIXED mistake_dst = {
			FIXED_DEC(-150,1) - fx,
			FIXED_DEC(10,1) - fy,
			FIXED_DEC(100, 1),
			FIXED_DEC(24,1)
		};

		Stage_DrawTex(&this->tex_cut0, &mistake_src, &mistake_dst, stage.camera.bzoom);
	}

	if (stage.stage_id == StageId_1_2 && stage.song_step >= 720 && stage.song_step <= 724)
	{
		RECT improbable_src = { 103, 139, 147, 24 };
		RECT_FIXED improbable_dst = {
			FIXED_DEC(-150,1) - fx,
			FIXED_DEC(10,1) - fy,
			FIXED_DEC(147, 1),
			FIXED_DEC(24,1)
		};

		Stage_DrawTex(&this->tex_cut0, &improbable_src, &improbable_dst, stage.camera.bzoom);
	}

	if (stage.stage_id == StageId_1_2 && stage.song_step >= 769 && stage.song_step <= 773)
	{
		RECT insufficient_src = { 86, 84, 156, 24 };
		RECT_FIXED insufficient_dst = {
			FIXED_DEC(-80,1) - fx,
			FIXED_DEC(20,1) - fy,
			FIXED_DEC(156, 1),
			FIXED_DEC(24,1)
		};

		Stage_DrawTex(&this->tex_cut0, &insufficient_src, &insufficient_dst, stage.camera.bzoom);
	}

	if (stage.stage_id == StageId_1_2 && stage.song_step >= 776 && stage.song_step <= 779)
	{
		RECT suffer2_src = { 0, 57, 85, 24 };
		RECT_FIXED suffer2_dst = {
			FIXED_DEC(-100,1) - fx,
			FIXED_DEC(-30,1) - fy,
			FIXED_DEC(85,1),
			FIXED_DEC(24,1)
		};

		Stage_DrawTex(&this->tex_cut0, &suffer2_src, &suffer2_dst, stage.camera.bzoom);
	}

	if (stage.stage_id == StageId_1_2 && stage.song_step >= 800 && stage.song_step <= 804)
	{
		RECT waste1_src = { 0, 223, 79, 24 };
		RECT_FIXED waste1_dst = {
			FIXED_DEC(-130,1) - fx,
			FIXED_DEC(50,1) - fy,
			FIXED_DEC(79, 1),
			FIXED_DEC(24,1)
		};

		Stage_DrawTex(&this->tex_cut0, &waste1_src, &waste1_dst, stage.camera.bzoom);
	}

	if (stage.stage_id == StageId_1_2 && stage.song_step >= 812 && stage.song_step <= 815)
	{
		RECT incorrect2_src = { 0, 110, 135, 28 };
		RECT_FIXED incorrect2_dst = {
			FIXED_DEC(-80,1) - fx,
			FIXED_DEC(0,1) - fy,
			FIXED_DEC(135,1),
			FIXED_DEC(28,1)
		};

		Stage_DrawTex(&this->tex_cut0, &incorrect2_src, &incorrect2_dst, stage.camera.bzoom);
	}

	if (stage.stage_id == StageId_1_2 && stage.song_step >= 2230 && stage.song_step <= 2260)
	{
	RECT incorrect9_src = { 0, 110, 135, 28 };
	RECT_FIXED incorrect9_dst = {
		FIXED_DEC(-140,1) - fx,
		FIXED_DEC(50,1) - fy,
		FIXED_DEC(131,1),
		FIXED_DEC(24,1)
	};

	Stage_DrawTex(&this->tex_cut0, &incorrect9_src, &incorrect9_dst, stage.camera.bzoom);
	}

	if (stage.stage_id == StageId_1_2 && stage.song_step >= 2294 && stage.song_step <= 2330)
	{
		RECT clownyou_src = { 0, 25, 256, 28 };
		RECT_FIXED clownyou_dst = {
			FIXED_DEC(-160,1) - fx,
			FIXED_DEC(70,1) - fy,
			FIXED_DEC(248,1),
			FIXED_DEC(24,1)
		};

		Stage_DrawTex(&this->tex_cut0, &clownyou_src, &clownyou_dst, stage.camera.bzoom);
	}


	if (stage.stage_id == StageId_1_4 && stage.song_step >= 2128 && stage.song_step <= 2140)
	{
		RECT hank_src = { 110, 195, 141, 28 };
		RECT_FIXED hank_dst = {
			FIXED_DEC(-160,1) - fx,
			FIXED_DEC(30,1) - fy,
			FIXED_DEC(282,1),
			FIXED_DEC(56,1)
		};

		Stage_DrawTex(&this->tex_cut0, &hank_src, &hank_dst, stage.camera.bzoom);
	}
}

//Week Tricky 2 background functions
void Back_WeekT2_DrawBG(StageBack *back)
{
	Back_WeekT2 *this = (Back_WeekT2*)back;
	
	fixed_t fx, fy;
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	//Draw window
	RECT window_src = { 0, 0, 256, 226 };
	RECT_FIXED window_dst = {
		FIXED_DEC(-500,1) - fx,
		FIXED_DEC(-175,1) - fy,
		FIXED_DEC(512,1),
		FIXED_DEC(400,1)
	};

	Stage_DrawTex(&this->tex_back0, &window_src, &window_dst, stage.camera.bzoom);

	//Draw window
	RECT window1_src = { 0, 0, 256, 226 };
	RECT_FIXED window1_dst = {
		FIXED_DEC(3,1) - fx,
		FIXED_DEC(-175,1) - fy,
		FIXED_DEC(512,1),
		FIXED_DEC(400,1)
	};

	Stage_DrawTex(&this->tex_back1, &window1_src, &window1_dst, stage.camera.bzoom);
}

void Back_WeekT2_Free(StageBack *back)
{
	Back_WeekT2 *this = (Back_WeekT2*)back;
	
	//Free structure
	Mem_Free(this);

}

StageBack *Back_WeekT2_New(void)
{
	//Allocate background structure
	Back_WeekT2 *this = (Back_WeekT2*)Mem_Alloc(sizeof(Back_WeekT2));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = Back_WeekT2_DrawFG;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_WeekT2_DrawBG;
	this->back.free = Back_WeekT2_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\WEEKT2\\BACK.ARC;1");
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back2.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_cut0, Archive_Find(arc_back, "cut0.tim"), 0);
	Mem_Free(arc_back);
	
	return (StageBack*)this;
}