/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "weekt3.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

//Week Tricky 3 background structure
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
} Back_WeekT3;

//Henchmen animation and rects
static const CharFrame henchmen_frame[18] = {
	{0, {  0,   0,  61,  55}, { 20,  55}}, //0 left 1
	{0, { 62,   0,  62,  55}, { 20,  55}}, //1 left 2
	{0, {125,   0,  60,  55}, { 20,  55}}, //2 left 3
	{0, {186,   0,  61,  54}, { 21,  54}}, //3 left 4
	{0, {  0,  56,  62,  55}, { 20,  55}}, //4 left 5
	{0, { 64,  57,  62,  55}, { 20,  55}}, //5 left 5
	
	{1, {  0,   0, 61, 55}, { 20,  55}}, //6 right 1
	{1, { 67,   0, 52, 55}, { 20,  55}}, //7 right 2
	{1, {133,   0,  65, 53}, { 21,  54}}, //8 right 3
	{1, {  0,  56,  64,  57}, { 21,  57}}, //9 right 4
	{1, { 66,  58,  66,  57}, { 20,  57}}, //10 right 5
	{1, {134,  55,  65,  54}, { 20,  54}}, //11 right 1
	{1, {  0, 114,  64,  57}, { 21,  56}}, //12 right 2
	{1, { 66, 116,  65,  57}, { 19,  56}}, //13 right 3
	{1, {132, 110,  65,  54}, { 20,  53}}, //14 right 4
	{1, {  0, 173,  64,  57}, { 21,  57}}, //15 right 5
	{1, { 68, 173,  65,  58}, { 20,  58}}, //16 right 4
	{1, {133, 164,  64,  54}, { 21,  54}}, //17 right 5
};

static const Animation henchmen_anim[2] = {
	{2, (const u8[]){ 3, 3, 3, 4, 5, 0, 1, 2, 3, 3, 3, 3, 4, 5, 0, 1, 3, ASCR_BACK, 1}}, //Left
	{1, (const u8[]){6, 6, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, ASCR_BACK, 1}}, //Right
};

//Henchmen functions
void WeekT3_Henchmen_SetFrame(void *user, u8 frame)
{
	Back_WeekT3 *this = (Back_WeekT3*)user;
	
	//Check if this is a new frame
	if (frame != this->hench_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &henchmen_frame[this->hench_frame = frame];
		if (cframe->tex != this->hench_tex_id)
			Gfx_LoadTex(&this->tex_hench, this->arc_hench_ptr[this->hench_tex_id = cframe->tex], 0);
	}
}

void WeekT3_Henchmen_Draw(Back_WeekT3 *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &henchmen_frame[this->hench_frame];
	
	fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = {ox, oy, src.w << FIXED_SHIFT, src.h << FIXED_SHIFT};
	Stage_DrawTex(&this->tex_hench, &src, &dst, stage.camera.bzoom);
}

void Back_WeekT3_DrawBG(StageBack *back)
{
	Back_WeekT3 *this = (Back_WeekT3*)back;
	
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
	Animatable_Animate(&this->hench_animatable, (void*)this, WeekT3_Henchmen_SetFrame);
	
	WeekT3_Henchmen_Draw(this,  FIXED_DEC(5,1) - fx, FIXED_DEC(30,1) - fy);

	//Draw sunset
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT sunset_src = {0, 0, 256, 256};
	RECT_FIXED sunset_dst = {
		FIXED_DEC(-410,1) - fx,
		FIXED_DEC(-145,1) - fy,
		FIXED_DEC(450,1),
		FIXED_DEC(400,1)
	};
	
	Stage_DrawTex(&this->tex_back1, &sunset_src, &sunset_dst, stage.camera.bzoom);

	RECT sunset1_src = { 0, 0, 256, 256 };
	RECT_FIXED sunset1_dst = {
		FIXED_DEC(-50,1) - fx,
		FIXED_DEC(-145,1) - fy,
		FIXED_DEC(450,1),
		FIXED_DEC(400,1)
	};

	Stage_DrawTex(&this->tex_back2, &sunset1_src, &sunset1_dst, stage.camera.bzoom);
}

void Back_WeekT3_Free(StageBack *back)
{
	Back_WeekT3 *this = (Back_WeekT3*)back;
	
	//Free henchmen archive
	Mem_Free(this->arc_hench);
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_WeekT3_New(void)
{
	//Allocate background structure
	Back_WeekT3 *this = (Back_WeekT3*)Mem_Alloc(sizeof(Back_WeekT3));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = NULL;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_WeekT3_DrawBG;
	this->back.free = Back_WeekT3_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\WEEKT3\\BACK.ARC;1");
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Gfx_LoadTex(&this->tex_back2, Archive_Find(arc_back, "back2.tim"), 0);
	Mem_Free(arc_back);
	
	//Load henchmen textures
	this->arc_hench = IO_Read("\\WEEKT3\\HENCH.ARC;1");
	this->arc_hench_ptr[0] = Archive_Find(this->arc_hench, "hench0.tim");
	this->arc_hench_ptr[1] = Archive_Find(this->arc_hench, "hench1.tim");
	
	//Initialize henchmen state
	Animatable_Init(&this->hench_animatable, henchmen_anim);
	Animatable_SetAnim(&this->hench_animatable, 0);
	this->hench_frame = this->hench_tex_id = 0xFF; //Force art load
	
	return (StageBack*)this;
}
