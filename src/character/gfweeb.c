/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "gfweeb.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

#include "../stage/week7.h"

//GF character structure
enum
{
	GFWeeb_ArcMain_BopLeft,
	GFWeeb_ArcMain_BopRight,
	
	GFWeeb_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[GFWeeb_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
	
	//Pico test
	u16 *pico_p;
} Char_GF;

//GF character definitions
static const CharFrame char_gfweeb_frame[] = {
	{GFWeeb_ArcMain_BopLeft, {  0,   0, 69, 63}, { 36,  31}}, //0 bop left 1
	{GFWeeb_ArcMain_BopLeft, { 70,   0, 71, 63}, { 37,  31}}, //1 bop left 1
	{GFWeeb_ArcMain_BopLeft, { 142,   0, 71, 63}, { 35,  31}}, //2 bop left 2
	{GFWeeb_ArcMain_BopLeft, {  0,  65, 70, 63}, { 35,  31}}, //3 bop left 2
	{GFWeeb_ArcMain_BopLeft, { 71,  66, 70, 63}, { 35,  30}}, //4 bop left 3
	{GFWeeb_ArcMain_BopLeft, {144,  65, 70, 63}, { 35,  31}}, //5 bop left 4
	{GFWeeb_ArcMain_BopLeft, {  0, 129, 70, 63}, { 36,  31}}, //6 bop left 1
	{GFWeeb_ArcMain_BopLeft, { 71, 129, 70, 64}, { 36,  32}}, //7 bop left 2
	{GFWeeb_ArcMain_BopLeft, { 142, 129, 70, 64}, { 36,  32}}, //8 bop left 2
	
	{GFWeeb_ArcMain_BopRight, {  0,   0, 70, 64}, { 36,  32}}, //9 bop right 1
	{GFWeeb_ArcMain_BopRight, { 71,   0, 70, 63}, { 36,  31}}, //10 bop right 2
	{GFWeeb_ArcMain_BopRight, { 142,  0, 71, 63}, { 36,  31}}, //11 bop right 3
	{GFWeeb_ArcMain_BopRight, {  0,  66, 70, 63}, { 36,  31}}, //12 bop right 3
	{GFWeeb_ArcMain_BopRight, { 71,  64,  70, 63}, { 36,  31}}, //13 bop right 4
	{GFWeeb_ArcMain_BopRight, {142, 64,  70, 63}, { 37,  31}}, //14 bop right 4
	{GFWeeb_ArcMain_BopRight, { 0, 130, 70, 63}, { 37,  31}}, //15 bop right 3
	{GFWeeb_ArcMain_BopRight, { 71, 128, 70, 63}, { 37,  31}}, //16 bop right 3
	{GFWeeb_ArcMain_BopRight, {142, 128, 69, 63}, { 35,  31}}, //16 bop right 3
};

static const Animation char_gfweeb_anim[CharAnim_Max] = {
	{0, (const u8[]) { ASCR_CHGANI, CharAnim_Left }},                           //CharAnim_Idle
	{1, (const u8[]) { 0,  1,  1,  2,  3, 3,  4, 5, 6, 7, 8, ASCR_BACK, 1 }}, //CharAnim_Left
	{0, (const u8[]) { ASCR_CHGANI, CharAnim_Left }},                           //CharAnim_LeftAlt
	{2, (const u8[]) { 17, ASCR_REPEAT }},                                  //CharAnim_Down
	{0, (const u8[]) { ASCR_CHGANI, CharAnim_Left }},                           //CharAnim_DownAlt
	{0, (const u8[]) { ASCR_CHGANI, CharAnim_Left }},                           //CharAnim_Up
	{0, (const u8[]) { ASCR_CHGANI, CharAnim_Left }},                           //CharAnim_UpAlt
	{1, (const u8[]) { 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, ASCR_BACK, 1 }}, //CharAnim_Right
	{0, (const u8[]) { ASCR_CHGANI, CharAnim_Left }},                               //CharAnim_RightAlt
};

//GF character functions
void Char_GFWeeb_SetFrame(void *user, u8 frame)
{
	Char_GF *this = (Char_GF*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_gfweeb_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_GFWeeb_Tick(Character *character)
{
	Char_GF *this = (Char_GF*)character;
	
	//Initialize Pico test
	if (stage.stage_id == StageId_7_3 && stage.back != NULL && this->pico_p == NULL)
		this->pico_p = ((Back_Week7*)stage.back)->pico_chart;
	
	if (this->pico_p != NULL)
	{
		if (stage.note_scroll >= 0)
		{
			//Scroll through Pico chart
			u16 substep = stage.note_scroll >> FIXED_SHIFT;
			while (substep >= ((*this->pico_p) & 0x7FFF))
			{
			}
		}
	}
	else
	{
		if (stage.flag & STAGE_FLAG_JUST_STEP)
		{
			//Perform dance
			if ((stage.song_step % stage.gf_speed) == 0)
			{
				//Switch animation
				if (character->animatable.anim == CharAnim_Left)
					character->set_anim(character, CharAnim_Right);
				else
					character->set_anim(character, CharAnim_Left);
			}
		}
	}
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_GFWeeb_SetFrame);
	Character_Draw(character, &this->tex, &char_gfweeb_frame[this->frame]);
}

void Char_GFWeeb_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
}

void Char_GFWeeb_Free(Character *character)
{
	Char_GF *this = (Char_GF*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_GFWeeb_New(fixed_t x, fixed_t y)
{
	//Allocate gf object
	Char_GF *this = Mem_Alloc(sizeof(Char_GF));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_GF_New] Failed to allocate gf object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_GFWeeb_Tick;
	this->character.set_anim = Char_GFWeeb_SetAnim;
	this->character.free = Char_GFWeeb_Free;
	
	Animatable_Init(&this->character.animatable, char_gfweeb_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.health_i = 1;
	
	this->character.focus_x = FIXED_DEC(16,1);
	this->character.focus_y = FIXED_DEC(-50,1);
	this->character.focus_zoom = FIXED_DEC(13,10);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\GFWEEB.ARC;1");
	
	const char **pathp = (const char *[]){
		"bopleft.tim",  //GF_ArcMain_BopLeft
		"bopright.tim", //GF_ArcMain_BopRight
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	//Initialize Pico test
	if (stage.stage_id == StageId_7_3 && stage.back != NULL)
		this->pico_p = ((Back_Week7*)stage.back)->pico_chart;
	else
		this->pico_p = NULL;
	
	return (Character*)this;
}
