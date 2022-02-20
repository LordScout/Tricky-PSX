/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "weekt4.h"

#include "../mem.h"
#include "../archive.h"

//Week Tricky Phase 4 background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back2; //Right Rock
	Gfx_Tex tex_back1; //Left Rock
	Gfx_Tex tex_back0; //Background

	//Cutscene
	Gfx_Tex tex_cut0; //Cover Rock
	Gfx_Tex tex_cut1; //Beam of Light
	Gfx_Tex tex_cut2; //the one 8 bit texture... use it wisely...
	Gfx_Tex tex_cut3;
	Gfx_Tex tex_cut4;
	Gfx_Tex tex_cut5;
} Back_WeekT4;

void Back_WeekT4_DrawFG(StageBack* back)
{
	Back_WeekT4* this = (Back_WeekT4*)back;

	fixed_t fx, fy;
	fx = stage.camera.x;
	fy = stage.camera.y;

	RECT cover_src = { 0, 0, 182, 151 };
	RECT_FIXED cover_dst = {
		FIXED_DEC(-295,1) - fx,
		FIXED_DEC(35,1) - fy,
		FIXED_DEC(273,1),
		FIXED_DEC(226,1)
	};

	Stage_DrawTex(&this->tex_cut0, &cover_src, &cover_dst, stage.camera.bzoom);

}

//Week Tricky Phase 4 background functions
void Back_WeekT4_DrawBG(StageBack *back)
{
	Back_WeekT4 *this = (Back_WeekT4*)back;
	
	fixed_t fx, fy;
	fx = stage.camera.x;
	fy = stage.camera.y;

	
	//Draw 
	RECT RockL_src = {0, 0, 222, 256};
	RECT_FIXED RockL_dst = {
		FIXED_DEC(-383,1) - fx,
		FIXED_DEC(-275,1) - fy,
		FIXED_DEC(333,1),
		FIXED_DEC(384,1)
	};
	
	Stage_DrawTex(&this->tex_back1, &RockL_src, &RockL_dst, stage.camera.bzoom);

	//Draw
	RECT RockR_src = { 0, 0, 202, 256};
	RECT_FIXED RockR_dst = {
		FIXED_DEC(-50,1) - fx,
		FIXED_DEC(-110,1) - fy,
		FIXED_DEC(303,1),
		FIXED_DEC(384,1)
	};

	Stage_DrawTex(&this->tex_back2, &RockR_src, &RockR_dst, stage.camera.bzoom);

	//Draw
	RECT light_src = { 0, 0, 102, 255};
	RECT_FIXED light_dst = {
		FIXED_DEC(40,1) - fx,
		FIXED_DEC(-250,1) - fy,
		FIXED_DEC(204,1),
		FIXED_DEC(600,1)
	};

	Stage_DrawTex(&this->tex_cut1, &light_src, &light_dst, stage.camera.bzoom);

	//Draw
	RECT bg_src = { 0, 0, 256, 256};
	RECT_FIXED bg_dst = {
		FIXED_DEC(-400,1) - fx,
		FIXED_DEC(-400,1) - fy,
		FIXED_DEC(768,1),
		FIXED_DEC(768,1)
	};

	Stage_DrawTex(&this->tex_back0, &bg_src, &bg_dst, stage.camera.bzoom);
}

void Back_WeekT4_Free(StageBack *back)
{
	Back_WeekT4 *this = (Back_WeekT4*)back;
	
	//Free structure
	Mem_Free(this);

}

StageBack *Back_WeekT4_New(void)
{
	//Allocate background structure
	Back_WeekT4 *this = (Back_WeekT4*)Mem_Alloc(sizeof(Back_WeekT4));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = Back_WeekT4_DrawFG;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_WeekT4_DrawBG;
	this->back.free = Back_WeekT4_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\WEEKT4\\BACK.ARC;1");
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
