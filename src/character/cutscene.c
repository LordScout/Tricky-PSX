/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "cutscene.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"
#include "../timer.h"

//Tricky character structure
enum
{
	Cutscene_ArcMain_Idle0,
	Cutscene_ArcMain_Idle1,
	Cutscene_ArcMain_Idle2,
	Cutscene_ArcMain_Idle3,
	Cutscene_ArcMain_Idle4,
	Cutscene_ArcMain_Idle5,
	Cutscene_ArcMain_Idle6,
	Cutscene_ArcMain_Left0,
	Cutscene_ArcMain_Left1,
	Cutscene_ArcMain_Left2,
	Cutscene_ArcMain_Left3,
	Cutscene_ArcMain_Left4,
	Cutscene_ArcMain_Left5,
	Cutscene_ArcMain_Left6,
	Cutscene_ArcMain_Down0,
	Cutscene_ArcMain_Down1,
	Cutscene_ArcMain_Down2,
	Cutscene_ArcMain_Down3,
	Cutscene_ArcMain_Down4,
	Cutscene_ArcMain_Down5,
	Cutscene_ArcMain_Up0,
	Cutscene_ArcMain_Up1,
	Cutscene_ArcMain_Up2,
	Cutscene_ArcMain_Up3,
	Cutscene_ArcMain_Up4,
	Cutscene_ArcMain_Up5,
	Cutscene_ArcMain_Up6,
	Cutscene_ArcMain_Right0,
	Cutscene_ArcMain_Right1,
	Cutscene_ArcMain_Right2,
	Cutscene_ArcMain_Right3,
	Cutscene_ArcMain_Right4,
	Cutscene_ArcMain_Right5,
	Cutscene_ArcMain_Right6,
	
	Cutscene_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Cutscene_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
	
	//Mouth state
	fixedu_t mouth_i;
} Char_Cutscene;

//Cutscene character definitions
static const CharFrame char_cutscene_frame[] = {
	{Cutscene_ArcMain_Idle0, {  0,   0, 180, 173}, { 130, 153}}, //0 idle 1
	{Cutscene_ArcMain_Idle1, {  0,   0, 180, 173}, { 250, 153}}, //1 idle 2
	{Cutscene_ArcMain_Idle2, {  0,   0, 180, 173}, { 130, 153}}, //2 idle 3
	{Cutscene_ArcMain_Idle3, {  0,   0, 180, 173}, { 130, 153}}, //3 idle 4
	{Cutscene_ArcMain_Idle4, {  0,   0, 180, 173}, { 130, 153}}, //4 idle 2
	{Cutscene_ArcMain_Idle5, {  0,   0, 180, 173}, { 130, 153}}, //5 idle 3
	{Cutscene_ArcMain_Idle6, {  0,   0, 180, 173}, { 130, 153}}, //6 idle 4
	
	{Cutscene_ArcMain_Left0, {  0,   0, 180, 173}, { 130, 153}}, //7 left 1
	{Cutscene_ArcMain_Left1, {  0,   0, 180, 173}, { 130, 153}}, //8 left 1
	{Cutscene_ArcMain_Left2, {  0,   0, 180, 173}, { 130, 153}}, //9 left 1
	{Cutscene_ArcMain_Left3, {  0,   0, 180, 173}, { 130, 153}}, //10 left 1
	{Cutscene_ArcMain_Left4, {  0,   0, 180, 173}, { 130, 153}}, //11 left 1
	{Cutscene_ArcMain_Left5, {  0,   0, 180, 173}, { 130, 153}}, //12 left 1
	{Cutscene_ArcMain_Left6, {  0,   0, 180, 173}, { 130, 153}}, //13 left 1
	
	{Cutscene_ArcMain_Down0, {  0,   0, 180, 173}, { 130, 153}}, //14 down 1
	{Cutscene_ArcMain_Down1, {  0,   0, 180, 173}, { 130, 153}}, //15 down 2
	{Cutscene_ArcMain_Down2, {  0,   0, 180, 173}, { 130, 153}}, //16 down 1
	{Cutscene_ArcMain_Down3, {  0,   0, 180, 173}, { 130, 153}}, //17 down 2
	{Cutscene_ArcMain_Down4, {  0,   0, 180, 173}, { 130, 153}}, //18 down 1
	{Cutscene_ArcMain_Down5, {  0,   0, 180, 173}, { 130, 153}}, //19 down 1
	
	{Cutscene_ArcMain_Up0, {  0,   0, 180, 173}, { 130, 153}}, //20 up 1
	{Cutscene_ArcMain_Up1, {  0,   0, 180, 173}, { 130, 153}}, //21 up 2
	{Cutscene_ArcMain_Up2, {  0,   0, 180, 173}, { 130, 153}}, //22 up 1
	{Cutscene_ArcMain_Up3, {  0,   0, 180, 173}, { 130, 153}}, //23 up 2
	{Cutscene_ArcMain_Up4, {  0,   0, 180, 173}, { 130, 153}}, //24 up 1
	{Cutscene_ArcMain_Up5, {  0,   0, 180, 173}, { 130, 153}}, //25 up 2
	{Cutscene_ArcMain_Up6, {  0,   0, 180, 173}, { 130, 153}}, //26 up 2
	
	{Cutscene_ArcMain_Right0, {  0,   0, 180, 173}, { 130, 153}}, //27 right 1
	{Cutscene_ArcMain_Right1, {  0,   0, 180, 173}, { 130, 153}}, //28 right 2
	{Cutscene_ArcMain_Right2, {  0,   0, 180, 173}, { 130, 153}}, //29 right 1
	{Cutscene_ArcMain_Right3, {  0,   0, 180, 173}, { 130, 153}}, //30 right 2
	{Cutscene_ArcMain_Right4, {  0,   0, 180, 173}, { 130, 153}}, //31 right 1

	{Cutscene_ArcMain_Idle1, {  0,   0, 180, 173}, { 180, 153}}, //32 idle 2
	{Cutscene_ArcMain_Idle1, {  0,   0, 180, 173}, { 130, 153}}, //33 idle 2
};

static const Animation char_cutscene_anim[CharAnim_Max] = {
	{1, (const u8[]){ 0, 0, 0, 0, 0, ASCR_BACK, 1}},                                           //CharAnim_Idle
	{2, (const u8[]){ 7, 8, 9, 10, 11, 12, 13, ASCR_BACK, 3}},                                                   //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},                                             //CharAnim_LeftAlt
	{2, (const u8[]){ 14, 15, 16, 17, 18, 19, ASCR_BACK, 3}},                                                   //CharAnim_Down
	{2, (const u8[]){16, 17, 18, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, ASCR_BACK, 1}},   //CharAnim_DownAlt
	{2, (const u8[]){20, 21,  22, 23, 24, 25, 26, ASCR_BACK, 3}},                                                   //CharAnim_Up
	{2, (const u8[]){12, 13, 14, 15, ASCR_BACK, 1}},                                           //CharAnim_UpAlt
	{2, (const u8[]){ 27, 28, 29, 30, 31, 32, 33, ASCR_BACK, 3}},                                                   //CharAnim_Right
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},     //CharAnim_RightAlt
	{1, (const u8[]){ 1, 32, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23, 24, 24, 25, 25, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 27, 28, 29, 30, 31, 27, 28, 29, 30, 31, 27, 28, 29, 30, 31, 27, 28, 29, 30, 31, 27, 28, 29, 30, 31, 27, 28, 29, 30, 31, 27, 28, 29, 30, 31, 27, 28, 29, 30, 31, 27, 28, 29, 30, 31, 27, 28, 29, 30, 31, 27, 28, 29, 30, 31, 27, 28, 29, 30, 31, 27, 28, 29, 30, 31, 27, 28, 29, 30, 31, 27, 28, 29, 30, 31, 27, 28, 29, 30, 31, 27, 28, 29, 30, 31, 27, 28, 29, 30, 31, 27, 28, 29, 30, 31, 27, 28, 29, 30, 31, 27, 28, 29, 30, 31, 27, 28, 29, 30, 31, 27, 28, 29, 30, 31, 27, 28, 29, 30, 31, ASCR_BACK, 5}},
};

//Cutscene character functions
void Char_Cutscene_SetFrame(void *user, u8 frame)
{
	Char_Cutscene *this = (Char_Cutscene*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_cutscene_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Cutscene_Tick(Character *character)
{
	Char_Cutscene *this = (Char_Cutscene*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);

	
	//Animate
	Animatable_Animate(&character->animatable, (void*)this, Char_Cutscene_SetFrame);
	
	//Draw body
	Character_Draw(character, &this->tex, &char_cutscene_frame[this->frame]);

	//Stage specific animations
		if (stage.note_scroll >= 0)
		{
	     if (stage.song_step == 24)
	      character->set_anim(character, CharAnim_ClownKillsYou);
		}
}

void Char_Cutscene_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Cutscene_Free(Character *character)
{
	Char_Cutscene *this = (Char_Cutscene*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Cutscene_New(fixed_t x, fixed_t y)
{
	//Allocate Cutscene object
	Char_Cutscene *this = Mem_Alloc(sizeof(Char_Cutscene));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Cutscene_New] Failed to allocate Cutscene object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Cutscene_Tick;
	this->character.set_anim = Char_Cutscene_SetAnim;
	this->character.free = Char_Cutscene_Free;
	
	Animatable_Init(&this->character.animatable, char_cutscene_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;
	
	this->character.focus_x = FIXED_DEC(50,1);
	this->character.focus_y = FIXED_DEC(-80,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\CUTSCENE.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //Cutscene_ArcMain_Idle0
		"idle1.tim", //Cutscene_ArcMain_Idle1
		"idle2.tim", //Cutscene_ArcMain_Idle0
		"idle3.tim", //Cutscene_ArcMain_Idle1
		"idle4.tim", //Cutscene_ArcMain_Idle0
		"idle5.tim", //Cutscene_ArcMain_Idle1
		"idle6.tim", //Cutscene_ArcMain_Idle1
		"left0.tim",  //Cutscene_ArcMain_Left
		"left1.tim",  //Cutscene_ArcMain_Left
		"left2.tim",  //Cutscene_ArcMain_Left
		"left3.tim",  //Cutscene_ArcMain_Left
		"left4.tim",  //Cutscene_ArcMain_Left
		"left5.tim",  //Cutscene_ArcMain_Left
		"left6.tim",  //Cutscene_ArcMain_Left
		"down0.tim",  //Cutscene_ArcMain_Down
		"down1.tim",  //Cutscene_ArcMain_Down
		"down2.tim",  //Cutscene_ArcMain_Down
		"down3.tim",  //Cutscene_ArcMain_Down
		"down4.tim",  //Cutscene_ArcMain_Down
		"down5.tim",  //Cutscene_ArcMain_Down
		"up0.tim",    //Cutscene_ArcMain_Up
		"up1.tim",    //Cutscene_ArcMain_Up
		"up2.tim",    //Cutscene_ArcMain_Up
		"up3.tim",    //Cutscene_ArcMain_Up
		"up4.tim",    //Cutscene_ArcMain_Up
		"up5.tim",    //Cutscene_ArcMain_Up
		"up6.tim",    //Cutscene_ArcMain_Up
		"right0.tim", //Cutscene_ArcMain_Right
		"right1.tim", //Cutscene_ArcMain_Right
		"right2.tim", //Cutscene_ArcMain_Right
		"right3.tim", //Cutscene_ArcMain_Right
		"right4.tim", //Cutscene_ArcMain_Right
		"right5.tim", //Cutscene_ArcMain_Right
		"right6.tim", //Cutscene_ArcMain_Right
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
