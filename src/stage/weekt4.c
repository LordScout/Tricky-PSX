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
	IO_Data arc_clonexpur, arc_clonexpur_ptr[2];
	
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

	//clonexpur state
	Gfx_Tex tex_clonexpur;
	u8 clonexpur_frame,clonexpur_tex_id;
	
	Animatable clonexpur_animatable;
} Back_WeekT4;

//Clonexpur animation and rects
static const CharFrame clonexpur_frame[8] = {
	{0, {  224,   0,  3, 3}, { 71,  98}}, //0 left 1
	{0, {  0,   0,  71, 121}, { 20, 121}}, //1 left 2
	{0, {  78,  0,  68, 93}, { 43,  92}}, //2 left 3
	{0, {149,   0,  73, 105}, { 25, 103}}, //3 left 4
	{0, {  0, 139,  74, 113}, { 43, 113}}, //4 left 5
	{0, { 76,  94,  74, 101}, { 42, 101}}, //5 left 4
	{0, { 79, 198, 104, 60}, { 50,  60}}, //6 left 5
	{0, {206,  86,  51, 171}, { 33, 150}}, //7 left 4

};

static const Animation clonexpur_anim[] = {
	{2, (const u8[]){0, 1, 2, 3, 4, 5, 6,  2, 3, 4, 5, 6,  2, 3, 4, 5, 6, 2, 3, 4, 5, 6,  2, 3, 4, 5, 6,  2, 3, 4, 5, 6, 2, 3, 4, 5, 6,  2, 3, 4, 5, 6,  2, 3, 4, 5, 6, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ASCR_BACK, 1}}, //Left
};

//Clonexpur functions
void WeekT4_Clonexpur_SetFrame(void *user, u8 frame)
{
	Back_WeekT4 *this = (Back_WeekT4*)user;
	
	//Check if this is a new frame
	if (frame != this->clonexpur_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &clonexpur_frame[this->clonexpur_frame = frame];
		if (cframe->tex != this->clonexpur_tex_id)
			Gfx_LoadTex(&this->tex_clonexpur, this->arc_clonexpur_ptr[this->clonexpur_tex_id = cframe->tex], 0);
	}
}

void WeekT4_Clonexpur_Draw(Back_WeekT4 *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &clonexpur_frame[this->clonexpur_frame];
	
	fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = {ox, oy, src.w << FIXED_SHIFT, src.h << FIXED_SHIFT};
	Stage_DrawTex(&this->tex_clonexpur, &src, &dst, stage.camera.bzoom);
}

void Back_WeekT4_DrawFG(StageBack* back)
{
	Back_WeekT4* this = (Back_WeekT4*)back;

	fixed_t fx, fy;
	fx = stage.camera.x;
	fy = stage.camera.y;

	if (stage.stage_id == StageId_1_4 && stage.song_step >= 2128 && stage.song_step <= 2140)
	{
		RECT hank_src = { 0, 232, 250, 24 };
		RECT_FIXED hank_dst = {
			FIXED_DEC(-90,1) - fx,
			FIXED_DEC(-140,1) - fy,
			FIXED_DEC(300,1),
			FIXED_DEC(30,1)
		};

		Stage_DrawTex(&this->tex_cut1, &hank_src, &hank_dst, stage.camera.bzoom);
	}

	RECT cover_src = { 0, 0, 182, 151 };
	RECT_FIXED cover_dst = {
		FIXED_DEC(-295,1) - fx,
		FIXED_DEC(35,1) - fy,
		FIXED_DEC(273,1),
		FIXED_DEC(226,1)
	};

	Stage_DrawTex(&this->tex_cut0, &cover_src, &cover_dst, stage.camera.bzoom);
    
	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{
		switch (stage.song_step & 64)
		{
		case 0:
			Animatable_SetAnim(&this->clonexpur_animatable, 0);
			break;
		}
	}
	Animatable_Animate(&this->clonexpur_animatable, (void*)this, WeekT4_Clonexpur_SetFrame);

	WeekT4_Clonexpur_Draw(this, FIXED_DEC(-100,1) - fx, FIXED_DEC(55,1) - fy);


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

	fx = stage.camera.x * 3 / 4;
	fy = stage.camera.y * 3 / 4;

	//Draw
	RECT light_src = { 0, 0, 102, 255};
	RECT_FIXED light_dst = {
		FIXED_DEC(40,1) - fx,
		FIXED_DEC(-250,1) - fy,
		FIXED_DEC(204,1),
		FIXED_DEC(600,1)
	};

	Stage_DrawTex(&this->tex_cut1, &light_src, &light_dst, stage.camera.bzoom);

	fx = stage.camera.x * 2 / 3;
	fy = stage.camera.y * 2 / 3;

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

	//Free clonexpur archive
	Mem_Free(this->arc_clonexpur);
	
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

	//Load henchmen textures
	this->arc_clonexpur = IO_Read("\\WEEKT4\\HENCH.ARC;1");
	this->arc_clonexpur_ptr[0] = Archive_Find(this->arc_clonexpur, "hench0.tim");

	//Initialize clonexpur state
	Animatable_Init(&this->clonexpur_animatable, clonexpur_anim);
	Animatable_SetAnim(&this->clonexpur_animatable, 0);
	this->clonexpur_frame = this->clonexpur_tex_id = 0xFF; //Force art load
	
	return (StageBack*)this;
}
