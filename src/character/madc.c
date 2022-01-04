/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "madc.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"
#include "../timer.h"

//Mad Tricky character structure
enum
{
	MadC_ArcMain_Idle2,
	MadC_ArcMain_Idle3,
	MadC_ArcMain_Idle4,
	MadC_ArcMain_Idle5,
	MadC_ArcMain_Idle6,
	MadC_ArcMain_Idle10,

	MadC_ArcMain_Left0,
	MadC_ArcMain_Left1,
	MadC_ArcMain_Left2,
	MadC_ArcMain_Left3,
	MadC_ArcMain_Left4,
	MadC_ArcMain_Left5,
	MadC_ArcMain_Down0,
	MadC_ArcMain_Down1,
	MadC_ArcMain_Down2,
	MadC_ArcMain_Down3,
	MadC_ArcMain_Down4,
	MadC_ArcMain_Down5,
	MadC_ArcMain_Up0,
	MadC_ArcMain_Up1,
	MadC_ArcMain_Up2,
	MadC_ArcMain_Up3,
	MadC_ArcMain_Up4,
	MadC_ArcMain_Up5,
	MadC_ArcMain_Right0,
	MadC_ArcMain_Right1,
	MadC_ArcMain_Right2,
	MadC_ArcMain_Right3,
	MadC_ArcMain_Right4,
	MadC_ArcMain_Right5,

	MadC_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;

	//Render data and state
	IO_Data arc_main, arc_scene;
	IO_Data arc_ptr[MadC_Arc_Max];

	Gfx_Tex tex;
	u8 frame, tex_id;

	//Mouth state
	fixedu_t mouth_i;
} Char_MadC;

//Mad Tricky character definitions
static const CharFrame char_madc_frame[] = {
	{MadC_ArcMain_Idle2, {  0,   0, 152, 169}, { 63, 159}}, //0 idle 3
	{MadC_ArcMain_Idle3, {  0,   0, 158, 169}, { 70, 158}}, //1 idle 4
	{MadC_ArcMain_Idle4, {  0,   0, 155, 171}, { 70, 161}}, //2 idle 1
	{MadC_ArcMain_Idle5, {  0,   0, 157, 169}, { 70, 159}}, //3 idle 2
	{MadC_ArcMain_Idle6, {  0,   0, 157, 168}, { 70, 158}}, //4 idle 3
	{MadC_ArcMain_Idle10, {  0,   0, 155, 166}, { 70, 156}}, //5 idle 3

	{MadC_ArcMain_Left0, {  0,   0, 194, 169}, { 109, 159}}, //6 left 1
	{MadC_ArcMain_Left1, {  0,   0, 185, 169}, { 105, 160}}, //7 left 2
	{MadC_ArcMain_Left2, {  0,   0, 183, 171}, { 109, 162}}, //8 left 1
	{MadC_ArcMain_Left3, {  0,   0, 176, 171}, { 106, 161}}, //9 left 2
	{MadC_ArcMain_Left4, {  0,   0, 183, 169}, { 109, 160}}, //10 left 1
	{MadC_ArcMain_Left5, {  0,   0, 180, 170}, { 106, 161}}, //11 left 2

	{MadC_ArcMain_Down0, {  0,   0, 179, 161}, { 72, 149}}, //12 left 1
	{MadC_ArcMain_Down1, {  0,   0, 178, 161}, { 73, 149}}, //13 left 2
	{MadC_ArcMain_Down2, {  0,   0, 179, 166}, { 79, 154}}, //14 left 1
	{MadC_ArcMain_Down3, {  0,   0, 177, 166}, { 76, 153}}, //15 left 2
	{MadC_ArcMain_Down4, {  0,   0, 179, 162}, { 80, 150}}, //16 left 1
	{MadC_ArcMain_Down5, {  0,   0, 178, 160}, { 78, 148}}, //17 left 2

	{MadC_ArcMain_Up0, {  0,   0, 150, 169}, { 89, 160}}, //18 left 1
	{MadC_ArcMain_Up1, {  0,   0, 150, 169}, { 93, 160}}, //19 left 2
	{MadC_ArcMain_Up2, {  0,   0, 147, 169}, { 94, 160}}, //20 left 1
	{MadC_ArcMain_Up3, {  0,   0, 154, 169}, { 101, 160}}, //21 left 2
	{MadC_ArcMain_Up4, {  0,   0, 145, 168}, { 92, 159}}, //22 left 1
	{MadC_ArcMain_Up5, {  0,   0, 149, 166}, { 95, 158}}, //23 left 2


	{MadC_ArcMain_Right0, {  0,   0, 236, 137}, { 85, 128}}, //24 Right 1
	{MadC_ArcMain_Right1, {  0,   0, 226, 139}, { 80, 130}}, //25 Right 2
	{MadC_ArcMain_Right2, {  0,   0, 219, 144}, { 76, 135}}, //26 Right 1
	{MadC_ArcMain_Right3, {  0,   0, 216, 144}, { 73, 135}}, //27 Right 2
	{MadC_ArcMain_Right4, {  0,   0, 218, 143}, { 75, 133}}, //28 Right 1
	{MadC_ArcMain_Right5, {  0,   0, 220, 143}, { 76, 133}}, //29 Right 2

};

static const Animation char_madc_anim[CharAnim_Max] = {
	{1, (const u8[]) { 0, 0, 1, 1, 0,  2, 2, 1, 3, 3, 2, 4, 4, 3, 5, 5, 4, 5, ASCR_BACK, 15 }}, //CharAnim_Idle
	{2, (const u8[]) { 6, 7, 8, 9, 10, 11, ASCR_BACK, 3 }},         //CharAnim_Left
	{0, (const u8[]) { ASCR_CHGANI, CharAnim_Idle }},   //CharAnim_LeftAlt
	{2, (const u8[]) { 12, 13, 14, 15, 16, 17, ASCR_BACK, 3 }},         //CharAnim_Down
	{0, (const u8[]) { ASCR_CHGANI, CharAnim_Idle }},   //CharAnim_DownAlt
	{2, (const u8[]) { 18, 19, 20, 21, 22, 23, ASCR_BACK, 3 }},         //CharAnim_Up
	{0, (const u8[]) { ASCR_CHGANI, CharAnim_Idle }},   //CharAnim_UpAlt
	{2, (const u8[]) { 24, 25, 26, 27, 28, 29, ASCR_BACK, 3 }},         //CharAnim_Right
	{0, (const u8[]) { ASCR_CHGANI, CharAnim_Idle }},   //CharAnim_RightAlt 
};

//MadC character functions
void Char_MadC_SetFrame(void* user, u8 frame)
{
	Char_MadC* this = (Char_MadC*)user;

	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame* cframe = &char_madc_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_MadC_Tick(Character* character)
{
	Char_MadC* this = (Char_MadC*)character;

	//Stage specific animations
	if (stage.note_scroll >= 0)
	{
		switch (stage.stage_id)
		{
		case StageId_5_1: //Tutorial peace
			if (stage.song_step > 64 && stage.song_step < 192 && (stage.song_step & 0x3F) == 60)
				character->set_anim(character, PlayerAnim_Peace);
			break;
		default:
			break;
		}
	}

	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0 &&
		character->animatable.anim != CharAnim_DownAlt) //Don't interrupt "Heh, pretty good!" sequence
		Character_PerformIdle(character);

	//Animate
	Animatable_Animate(&character->animatable, (void*)this, Char_MadC_SetFrame);

	//Draw body
	Character_Draw(character, &this->tex, &char_madc_frame[this->frame]);
}

void Char_MadC_SetAnim(Character* character, u8 anim)
{

	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_MadC_Free(Character* character)
{
	Char_MadC* this = (Char_MadC*)character;

	//Free art
	Mem_Free(this->arc_main);
	Mem_Free(this->arc_scene);
}

Character* Char_MadC_New(fixed_t x, fixed_t y)
{
	//Allocate MadC object
	Char_MadC* this = Mem_Alloc(sizeof(Char_MadC));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_MadC_New] Failed to allocate MadC object");
		ErrorLock();
		return NULL;
	}

	//Initialize character
	this->character.tick = Char_MadC_Tick;
	this->character.set_anim = Char_MadC_SetAnim;
	this->character.free = Char_MadC_Free;

	Animatable_Init(&this->character.animatable, char_madc_anim);
	Character_Init((Character*)this, x, y);

	//Set character information
	this->character.spec = 0;

	this->character.health_i = 2;

	this->character.focus_x = FIXED_DEC(50, 1);
	this->character.focus_y = FIXED_DEC(-80, 1);
	this->character.focus_zoom = FIXED_DEC(1, 1);

	//Load art
	this->arc_main = IO_Read("\\CHAR\\MADC.ARC;1");

	const char** pathp = (const char* []){
		"idle2.tim", //MadC_ArcMain_Idle0
		"idle3.tim", //MadC_ArcMain_Idle0
		"idle4.tim", //MadC_ArcMain_Idle0
		"idle5.tim", //MadC_ArcMain_Idle0
		"idle6.tim", //MadC_ArcMain_Idle0
		"idle10.tim", //MadC_ArcMain_Idle0
		"left0.tim", //MadC_ArcMain_left
		"left1.tim", //MadC_ArcMain_left
		"left2.tim", //MadC_ArcMain_left
		"left3.tim", //MadC_ArcMain_left
		"left4.tim", //MadC_ArcMain_left
		"left5.tim", //MadC_ArcMain_left
		"down0.tim", //MadC_ArcMain_down
		"down1.tim", //MadC_ArcMain_down
		"down2.tim", //MadC_ArcMain_down
		"down3.tim", //MadC_ArcMain_down
		"down4.tim", //MadC_ArcMain_down
		"down5.tim", //MadC_ArcMain_down
		"up0.tim", //MadC_ArcMain_Up
		"up1.tim", //MadC_ArcMain_Up
		"up2.tim", //MadC_ArcMain_Up
		"up3.tim", //MadC_ArcMain_up
		"up4.tim", //MadC_ArcMain_up
		"up5.tim", //MadC_ArcMain_up
		"right0.tim", //MadC_ArcMain_right
		"right1.tim", //MadC_ArcMain_right
		"right2.tim", //MadC_ArcMain_right
		"right3.tim", //MadC_ArcMain_right
		"right4.tim", //MadC_ArcMain_right
		"right5.tim", //MadC_ArcMain_right
		"upb0.tim",
		"upb1.tim",
		"rightb0.tim",
		"rightb1.tim",
		"downb0.tim",
		"downb1.tim",
		"downb2.tim",
		NULL
	};
	IO_Data* arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);

	//Initialize render state
	this->tex_id = this->frame = 0xFF;

	return (Character*)this;
}
