/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "mticky.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Menu Tricky character structure
enum
{
	MTicky_ArcMain_Ticky0,
	MTicky_ArcMain_Ticky1,
	MTicky_ArcMain_Ticky2,
	MTicky_ArcMain_Ticky3,
	MTicky_ArcMain_Ticky4,
	MTicky_ArcMain_Ticky5,
	MTicky_ArcMain_Ticky6,
	MTicky_ArcMain_Ticky7,
	MTicky_ArcMain_Ticky8,
	MTicky_ArcMain_Ticky9,
	MTicky_ArcMain_Ticky10,
	
	MTicky_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[MTicky_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
	
} Char_MTicky;

//MTicky character definitions
static const CharFrame char_MTicky_frame[] = {
	{MTicky_ArcMain_Ticky0, {  0,   0, 207, 111}, { 102, 111}}, //0
	{MTicky_ArcMain_Ticky0, {  0, 112, 200, 113}, { 99, 113}}, //1
	{MTicky_ArcMain_Ticky1, {  0,   0, 194, 116}, { 96, 116}}, //2 
	{MTicky_ArcMain_Ticky1, {  0, 117, 195, 118}, { 96, 118}}, //3
	{MTicky_ArcMain_Ticky2, {  0,   0, 195, 120}, { 96, 120}}, //4
	{MTicky_ArcMain_Ticky2, {  1, 122, 194, 121}, { 96, 121}}, //5
	{MTicky_ArcMain_Ticky3, {  0,   0, 195, 122}, { 96, 122}}, //6 
	{MTicky_ArcMain_Ticky3, {  0, 123, 195, 122}, { 96, 122}}, //7
	{MTicky_ArcMain_Ticky4, {  0,   0, 195, 122}, { 96, 121}}, //8 
	{MTicky_ArcMain_Ticky4, {  0, 123, 195, 122}, { 96, 120}}, //9
	{MTicky_ArcMain_Ticky5, {  0,   0, 195, 114}, { 96, 114}}, //10
	{MTicky_ArcMain_Ticky5, {  0, 115, 207, 111}, {102, 111}}, //11
	
	{MTicky_ArcMain_Ticky6, {  0,   0, 201, 113}, { 99, 113}}, //12 
	{MTicky_ArcMain_Ticky6, {  0, 114, 194, 116}, { 96, 116}}, //13
	{MTicky_ArcMain_Ticky7, {  0,   0, 195, 120}, { 96, 120}}, //14
	{MTicky_ArcMain_Ticky7, {  0, 121, 195, 120}, { 96, 120}}, //15
	{MTicky_ArcMain_Ticky8, {  0,   0, 194, 121}, { 96, 121}}, //16 
	{MTicky_ArcMain_Ticky8, {  0, 122, 195, 122}, { 96, 122}}, //17
	{MTicky_ArcMain_Ticky9, {  0,   0, 195, 121}, { 96, 121}}, //18 
	{MTicky_ArcMain_Ticky9, {  0, 122, 195, 122}, { 96, 122}}, //19
	{MTicky_ArcMain_Ticky10, {  0,   0, 194, 121}, { 96, 121}}, //20 
	{MTicky_ArcMain_Ticky10, {  0, 122, 194, 114}, { 96, 114}}, //21
};

static const Animation char_MTicky_anim[CharAnim_Max] = {
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Left}},                           //CharAnim_Idle
	{1, (const u8[]){ 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, ASCR_BACK, 1}}, //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Left}},                           //CharAnim_LeftAlt
	{2, (const u8[]){12, 13, ASCR_REPEAT}},                                  //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Left}},                           //CharAnim_DownAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Left}},                           //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Left}},                           //CharAnim_UpAlt
	{1, (const u8[]){12, 13, 14, 15, 16, 17, 18, 19, 20, 21, ASCR_BACK, 1}}, //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Left}},                           //CharAnim_RightAlt
};

//MTicky character functions
void Char_MTicky_SetFrame(void *user, u8 frame)
{
	Char_MTicky *this = (Char_MTicky*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_MTicky_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_MTicky_Tick(Character *character)
{
	Char_MTicky *this = (Char_MTicky*)character;
	
		if (stage.flag & STAGE_FLAG_JUST_STEP)
		{
			//Perform dance
			if ((stage.song_step % stage.MTicky_speed) == 0)
			{
				//Switch animation
				if (character->animatable.anim == CharAnim_Left)
					character->set_anim(character, CharAnim_Right);
				else
					character->set_anim(character, CharAnim_Left);
				
			}
		}
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_MTicky_SetFrame);
	Character_Draw(character, &this->tex, &char_MTicky_frame[this->frame]);
	
}

void Char_MTicky_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
}

void Char_MTicky_Free(Character *character)
{
	Char_MTicky *this = (Char_MTicky*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_MTicky_New(fixed_t x, fixed_t y)
{
	//Allocate MTicky object
	Char_MTicky *this = Mem_Alloc(sizeof(Char_MTicky));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_MTicky_New] Failed to allocate MTicky object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_MTicky_Tick;
	this->character.set_anim = Char_MTicky_SetAnim;
	this->character.free = Char_MTicky_Free;
	
	Animatable_Init(&this->character.animatable, char_MTicky_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;
	
	this->character.focus_x = FIXED_DEC(16,1);
	this->character.focus_y = FIXED_DEC(-50,1);
	this->character.focus_zoom = FIXED_DEC(13,10);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\MTICKY.ARC;1");
	const char **pathp = (const char *[]){
		"ticky0.tim", 
		"ticky1.tim", 
		"ticky2.tim", 
		"ticky3.tim",
		"ticky4.tim", 
		"ticky5.tim", 
		"ticky6.tim", 
		"ticky7.tim", 
		"ticky8.tim", 
		"ticky9.tim", 
		"ticky10.tim",      
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
