/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "hellc.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//HellClown character structure
enum
{
	Hellc_ArcMain_Idle0,
	Hellc_ArcMain_Idle1,
	Hellc_ArcMain_Idle2,
	Hellc_ArcMain_Idle3,
	Hellc_ArcMain_Idle4,
	Hellc_ArcMain_Idle5,
	Hellc_ArcMain_Idle6,
	Hellc_ArcMain_Idle7,
	Hellc_ArcMain_Left0,
	Hellc_ArcMain_Left1,
	Hellc_ArcMain_Left2,
	Hellc_ArcMain_Left3,
	Hellc_ArcMain_Left4,
	Hellc_ArcMain_Left5,
	Hellc_ArcMain_Down0,
	Hellc_ArcMain_Down1,
	Hellc_ArcMain_Down2,
	Hellc_ArcMain_Down3,
	Hellc_ArcMain_Down4,
	Hellc_ArcMain_Down5,
	Hellc_ArcMain_Up0,
	Hellc_ArcMain_Up1,
	Hellc_ArcMain_Up2,
	Hellc_ArcMain_Up3,
	Hellc_ArcMain_Up4,
	Hellc_ArcMain_Up5,
	Hellc_ArcMain_Right0,
	Hellc_ArcMain_Right1,
	Hellc_ArcMain_Right2,
	Hellc_ArcMain_Right3,
	Hellc_ArcMain_Right4,
	Hellc_ArcMain_Right5,
	
	Hellc_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Hellc_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Hellc;

//Hellc character definitions
static const CharFrame char_hellc_frame[] = {
	{Hellc_ArcMain_Idle0, {  0,   0, 188, 253}, { 86, 252}}, //0 idle 1
	{Hellc_ArcMain_Idle1, {  0,   0, 189, 253}, { 90, 251}}, //1 idle 2
	{Hellc_ArcMain_Idle2, {  0,   0, 187, 256}, { 86, 256}}, //2 idle 3
	{Hellc_ArcMain_Idle3, {  0,   0, 192, 256}, { 89, 256}}, //3 idle 4
	{Hellc_ArcMain_Idle4, {  0,   0, 191, 251}, { 85, 251}}, //4 idle 2
	{Hellc_ArcMain_Idle5, {  0,   0, 193, 248}, { 86, 248}}, //5 idle 3
	{Hellc_ArcMain_Idle6, {  0,   0, 192, 256}, { 85, 256}}, //6 idle 4
	{Hellc_ArcMain_Idle7, {  0,   0, 189, 254}, { 85, 254}}, //7 idle 4
	
	{Hellc_ArcMain_Left0, {  0,   0, 179, 247}, { 87, 229}}, //8 left 1
	{Hellc_ArcMain_Left1, {  0,   0, 178, 237}, { 83, 229}}, //9 left 2
	{Hellc_ArcMain_Left2, {  0,   0, 171, 239}, { 78, 232}}, //10 left 1
	{Hellc_ArcMain_Left3, {  0,   0, 175, 232}, { 83, 228}}, //11 left 2
	{Hellc_ArcMain_Left4, {  0,   0, 167, 237}, { 70, 229}}, //12 left 1
	{Hellc_ArcMain_Left5, {  0,   0, 172, 231}, { 76, 228}}, //13 left 2
	
	{Hellc_ArcMain_Down0, {  0,   0, 199, 224}, { 72, 214}}, //14 up 1
	{Hellc_ArcMain_Down1, {  0,   0, 204, 208}, { 72, 201}}, //15 up 2
	{Hellc_ArcMain_Down3, {  0,   0, 207, 209}, { 72, 207}}, //16 up 1
	{Hellc_ArcMain_Down3, {  0,   0, 221, 214}, { 72, 214}}, //17 up 2
	{Hellc_ArcMain_Down4, {  0,   0, 192, 222}, { 72, 220}}, //18 up 1
	{Hellc_ArcMain_Down5, {  0,   0, 202, 206}, { 72, 202}}, //19 up 2
	
	{Hellc_ArcMain_Up0, {  0,   0, 221, 255}, { 95, 212}}, //20 up 1
	{Hellc_ArcMain_Up1, {  0,   0, 225, 254}, { 98, 213}}, //21 up 2
	{Hellc_ArcMain_Up2, {  0,   0, 217, 250}, { 94, 213}}, //22 up 1
	{Hellc_ArcMain_Up3, {  0,   0, 219, 242}, { 97, 202}}, //23 up 2
	{Hellc_ArcMain_Up4, {  0,   0, 223, 253}, { 100, 212}}, //24 up 1
	{Hellc_ArcMain_Up5, {  0,   0, 220, 249}, { 97, 210}}, //25 up 2
	
	{Hellc_ArcMain_Right0, {  0,   0, 216, 214}, { 93, 214}}, //26 right 1
	{Hellc_ArcMain_Right1, {  0,   0, 197, 228}, { 80, 228}}, //27 right 2
	{Hellc_ArcMain_Right2, {  0,   0, 207, 221}, { 77, 221}}, //28 right 1
	{Hellc_ArcMain_Right3, {  0,   0, 206, 235}, { 73, 235}}, //29 right 2
	{Hellc_ArcMain_Right4, {  0,   0, 214, 216}, { 80, 216}}, //30 right 1
	{Hellc_ArcMain_Right5, {  0,   0, 204, 228}, { 74, 228}}, //31 right 2
};

static const Animation char_hellc_anim[CharAnim_Max] = {
	{1, (const u8[]){ 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 8, 9, 10, 11, 12, 13, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 14, 15, 16, 17, 18, 19, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 20, 21, 22, 23, 24, 25, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){26, 27, 28, 29, 30, 31, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Hellc character functions
void Char_Hellc_SetFrame(void *user, u8 frame)
{
	Char_Hellc *this = (Char_Hellc*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_hellc_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Hellc_Tick(Character *character)
{
	Char_Hellc *this = (Char_Hellc*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Hellc_SetFrame);
	Character_Draw(character, &this->tex, &char_hellc_frame[this->frame]);
}

void Char_Hellc_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Hellc_Free(Character *character)
{
	Char_Hellc *this = (Char_Hellc*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Hellc_New(fixed_t x, fixed_t y)
{
	//Allocate Hellc object
	Char_Hellc *this = Mem_Alloc(sizeof(Char_Hellc));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Hellc_New] Failed to allocate Hellc object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Hellc_Tick;
	this->character.set_anim = Char_Hellc_SetAnim;
	this->character.free = Char_Hellc_Free;
	
	Animatable_Init(&this->character.animatable, char_hellc_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = CHAR_SPEC_SHAKE;
	
	this->character.health_i = 3;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-85,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\HELLC.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //Hellc_ArcMain_Idle0
		"idle1.tim", //Hellc_ArcMain_Idle1
		"idle2.tim", //Hellc_ArcMain_Idle0
		"idle3.tim", //Hellc_ArcMain_Idle1
		"idle4.tim", //Hellc_ArcMain_Idle0
		"idle5.tim", //Hellc_ArcMain_Idle1
		"idle6.tim", //Hellc_ArcMain_Idle1
		"idle7.tim",
		"left0.tim",  //Hellc_ArcMain_Left
		"left1.tim",  //Hellc_ArcMain_Left
		"left2.tim",  //Hellc_ArcMain_Left
		"left3.tim",  //Hellc_ArcMain_Left
		"left4.tim",  //Hellc_ArcMain_Left
		"left5.tim",  //Hellc_ArcMain_Left
		"down0.tim",  //Hellc_ArcMain_Down
		"down1.tim",
		"down2.tim",
		"down3.tim",
		"down4.tim",
		"down5.tim",
		"up0.tim",    //Hellc_ArcMain_Up
		"up1.tim",
		"up2.tim",
		"up3.tim",
		"up4.tim",
		"up5.tim",
		"right0.tim", //Hellc_ArcMain_Right
		"right1.tim",
		"right2.tim",
		"right3.tim",
		"right4.tim",
		"right5.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
