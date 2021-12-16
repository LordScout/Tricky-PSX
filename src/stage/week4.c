/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "week4.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

//Week 4 background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	IO_Data arc_hench, arc_hench_ptr[2];
	
	Gfx_Tex tex_back1;
	Gfx_Tex tex_back2; //Sunset
	
	//Henchmen state
	Gfx_Tex tex_hench;
	u8 hench_frame, hench_tex_id;
	
	Animatable hench_animatable;
} Back_Week4;

//Henchmen animation and rects
static const CharFrame henchmen_frame[10] = {
	{0, {  0,   0,  99,  99}, { 71,  98}}, //0 left 1
	{0, { 99,   0,  99,  98}, { 71,  97}}, //1 left 2
	{0, {  0,  99,  98,  97}, { 69,  95}}, //2 left 3
	{0, { 98,  98,  62,  89}, { 42,  88}}, //3 left 4
	{0, {160,  98,  62,  89}, { 42,  88}}, //4 left 5
	
	{1, {  0,   0, 101, 103}, { 35, 101}}, //5 right 1
	{1, {101,   0,  99, 101}, { 33, 100}}, //6 right 2
	{1, {  0, 103,  99, 101}, { 33,  99}}, //7 right 3
	{1, { 99, 101,  64,  90}, { 26,  89}}, //8 right 4
	{1, {163, 101,  64,  90}, { 26,  89}}, //9 right 5
};

static const Animation henchmen_anim[2] = {
	{2, (const u8[]){0, 1, 1, 2, 2, 3, 4, ASCR_BACK, 1}}, //Left
	{2, (const u8[]){5, 6, 6, 7, 7, 8, 9, ASCR_BACK, 1}}, //Right
};

//Henchmen functions
void Week4_Henchmen_SetFrame(void *user, u8 frame)
{
	Back_Week4 *this = (Back_Week4*)user;
	
	//Check if this is a new frame
	if (frame != this->hench_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &henchmen_frame[this->hench_frame = frame];
		if (cframe->tex != this->hench_tex_id)
			Gfx_LoadTex(&this->tex_hench, this->arc_hench_ptr[this->hench_tex_id = cframe->tex], 0);
	}
}

void Week4_Henchmen_Draw(Back_Week4 *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &henchmen_frame[this->hench_frame];
	
	fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = {ox, oy, src.w << FIXED_SHIFT, src.h << FIXED_SHIFT};
	Stage_DrawTex(&this->tex_hench, &src, &dst, stage.camera.bzoom);
}

void Back_Week4_DrawBG(StageBack *back)
{
	Back_Week4 *this = (Back_Week4*)back;
	
	fixed_t fx, fy;
	
	//Animate and draw henchmen
	fx = stage.camera.x ;
	fy = stage.camera.y;
	
	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{
		switch (stage.song_step & 28)
		{
			case 0:
				Animatable_SetAnim(&this->hench_animatable, 0);
				break;
			case 16:
				Animatable_SetAnim(&this->hench_animatable, 1);
				break;
		}
	}
	Animatable_Animate(&this->hench_animatable, (void*)this, Week4_Henchmen_SetFrame);
	
	Week4_Henchmen_Draw(this,  FIXED_DEC(50,1) - fx, FIXED_DEC(30,1) - fy);

	//Draw sunset
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT sunset_src = {0, 0, 256, 256};
	RECT_FIXED sunset_dst = {
		FIXED_DEC(-410,1) - fx,
		FIXED_DEC(-100,1) - fy,
		FIXED_DEC(450,1),
		FIXED_DEC(315,1)
	};
	
	Stage_DrawTex(&this->tex_back1, &sunset_src, &sunset_dst, stage.camera.bzoom);

	RECT sunset1_src = { 0, 0, 256, 256 };
	RECT_FIXED sunset1_dst = {
		FIXED_DEC(-50,1) - fx,
		FIXED_DEC(-100,1) - fy,
		FIXED_DEC(450,1),
		FIXED_DEC(315,1)
	};

	Stage_DrawTex(&this->tex_back2, &sunset1_src, &sunset1_dst, stage.camera.bzoom);
}

void Back_Week4_Free(StageBack *back)
{
	Back_Week4 *this = (Back_Week4*)back;
	
	//Free henchmen archive
	Mem_Free(this->arc_hench);
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_Week4_New(void)
{
	//Allocate background structure
	Back_Week4 *this = (Back_Week4*)Mem_Alloc(sizeof(Back_Week4));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = NULL;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Week4_DrawBG;
	this->back.free = Back_Week4_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\WEEK4\\BACK.ARC;1");
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Gfx_LoadTex(&this->tex_back2, Archive_Find(arc_back, "back2.tim"), 0);
	Mem_Free(arc_back);
	
	//Load henchmen textures
	this->arc_hench = IO_Read("\\WEEK4\\HENCH.ARC;1");
	this->arc_hench_ptr[0] = Archive_Find(this->arc_hench, "hench0.tim");
	this->arc_hench_ptr[1] = Archive_Find(this->arc_hench, "hench1.tim");
	
	//Initialize henchmen state
	Animatable_Init(&this->hench_animatable, henchmen_anim);
	Animatable_SetAnim(&this->hench_animatable, 0);
	this->hench_frame = this->hench_tex_id = 0xFF; //Force art load
	
	return (StageBack*)this;
}
