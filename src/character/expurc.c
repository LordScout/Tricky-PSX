/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "expurc.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"
#include "../timer.h"

//Expurgation character structure
enum
{
	ExpurC_ArcMain_Idle0,
	ExpurC_ArcMain_Idle1,
	ExpurC_ArcMain_Idle2,
	ExpurC_ArcMain_Idle3,
	ExpurC_ArcMain_Idle4,
	ExpurC_ArcMain_Idle5,
	ExpurC_ArcMain_Left0,
	ExpurC_ArcMain_Left1,
	ExpurC_ArcMain_Left2,
	ExpurC_ArcMain_Left3,
	ExpurC_ArcMain_Left4,
	ExpurC_ArcMain_Left5,
	ExpurC_ArcMain_Left6,
	ExpurC_ArcMain_Left7,
	ExpurC_ArcMain_Down0,
	ExpurC_ArcMain_Down1,
	ExpurC_ArcMain_Down2,
	ExpurC_ArcMain_Down3,
	ExpurC_ArcMain_Down4,
	ExpurC_ArcMain_Down5,
	ExpurC_ArcMain_Down6,
	ExpurC_ArcMain_Down7,
	ExpurC_ArcMain_Up0,
	ExpurC_ArcMain_Up1,
	ExpurC_ArcMain_Up2,
	ExpurC_ArcMain_Up3,
	ExpurC_ArcMain_Up4,
	ExpurC_ArcMain_Up5,
	ExpurC_ArcMain_Up6,
	ExpurC_ArcMain_Right0,
	ExpurC_ArcMain_Right1,
	ExpurC_ArcMain_Right2,
	ExpurC_ArcMain_Right3,
	ExpurC_ArcMain_Right4,
	ExpurC_ArcMain_Right5,
	ExpurC_ArcMain_Right6,
	ExpurC_ArcMain_Right7,

	ExpurC_ArcMain_Hank0,
	ExpurC_ArcMain_Hank1,
	ExpurC_ArcMain_Hank2,
	ExpurC_ArcMain_Hank3,
	ExpurC_ArcMain_Hank4,
	ExpurC_ArcMain_Hank5,
	ExpurC_ArcMain_Hank6,

	ExpurC_ArcMain_Intro0,
	ExpurC_ArcMain_Intro1,
	ExpurC_ArcMain_Intro2,
	ExpurC_ArcMain_Intro3,
	ExpurC_ArcMain_Intro4,
	ExpurC_ArcMain_Intro5,
	ExpurC_ArcMain_Intro6,
	ExpurC_ArcMain_Intro7,
	ExpurC_ArcMain_Intro8,
	ExpurC_ArcMain_Intro9,
	ExpurC_ArcMain_Intro10,
	ExpurC_ArcMain_Intro11,
	ExpurC_ArcMain_Intro12,
	ExpurC_ArcMain_Intro13,
	ExpurC_ArcMain_Intro14,
	ExpurC_ArcMain_Intro15,
	ExpurC_ArcMain_Intro16,
	ExpurC_ArcMain_Intro17,
	ExpurC_ArcMain_Intro18,
	ExpurC_ArcMain_Intro19,
	ExpurC_ArcMain_Intro20,

	
	ExpurC_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[ExpurC_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;

} Char_ExpurC;

//ExpurC character definitions
static const CharFrame char_expurc_frame[] = {
	{ExpurC_ArcMain_Idle0, {  0,   0, 152, 154}, { 74, 144}}, //0 idle 1
	{ExpurC_ArcMain_Idle1, {  0,   0, 153, 152}, { 73, 142}}, //1 idle 2
	{ExpurC_ArcMain_Idle2, {  0,   0, 155, 160}, { 75, 150}}, //2 idle 3
	{ExpurC_ArcMain_Idle3, {  0,   0, 154, 163}, { 72, 153}}, //3 idle 4
	{ExpurC_ArcMain_Idle4, {  0,   0, 155, 161}, { 75, 151}}, //4 idle 2
	{ExpurC_ArcMain_Idle5, {  0,   0, 155, 164}, { 74, 154}}, //5 idle 3
	
	{ExpurC_ArcMain_Left0, {  0,   0, 202, 169}, { 124, 158}}, //6 left 1
	{ExpurC_ArcMain_Left1, {  0,   0, 198, 169}, { 124, 158}}, //7 left 1
	{ExpurC_ArcMain_Left2, {  0,   0, 188, 173}, { 118, 162}}, //8 left 1
	{ExpurC_ArcMain_Left3, {  0,   0, 190, 169}, { 119, 159}}, //9 left 1
	{ExpurC_ArcMain_Left4, {  0,   0, 192, 171}, { 123, 160}}, //10 left 1
	{ExpurC_ArcMain_Left5, {  0,   0, 187, 170}, { 119, 159}}, //11 left 1
	{ExpurC_ArcMain_Left6, {  0,   0, 188, 173}, { 119, 162}}, //12 left 1
	{ExpurC_ArcMain_Left7, {  0,   0, 188, 173}, { 119, 162}}, //13 left 1
	
	{ExpurC_ArcMain_Down0, {  0,   0, 185, 149}, { 84, 138}}, //14 down 1
	{ExpurC_ArcMain_Down1, {  0,   0, 182, 150}, { 83, 138}}, //15 down 2
	{ExpurC_ArcMain_Down2, {  0,   0, 183, 153}, { 79, 142}}, //16 down 1
	{ExpurC_ArcMain_Down3, {  0,   0, 180, 152}, { 78, 140}}, //17 down 2
	{ExpurC_ArcMain_Down4, {  0,   0, 181, 154}, { 80, 142}}, //18 down 1
	{ExpurC_ArcMain_Down5, {  0,   0, 180, 153}, { 77, 142}}, //19 down 1
	{ExpurC_ArcMain_Down6, {  0,   0, 180, 153}, { 77, 142}}, //20 down 1
	{ExpurC_ArcMain_Down7, {  0,   0, 180, 153}, { 77, 142}}, //21 down 1
	
	{ExpurC_ArcMain_Up0, {  0,   0, 147, 169}, { 87, 160}}, //22 up 1
	{ExpurC_ArcMain_Up1, {  0,   0, 147, 166}, { 90, 157}}, //23 up 2
	{ExpurC_ArcMain_Up2, {  0,   0, 145, 169}, { 93, 159}}, //24 up 1
	{ExpurC_ArcMain_Up3, {  0,   0, 145, 165}, { 92, 155}}, //25 up 2
	{ExpurC_ArcMain_Up4, {  0,   0, 145, 167}, { 94, 157}}, //26 up 1
	{ExpurC_ArcMain_Up5, {  0,   0, 149, 167}, { 95, 157}}, //27 up 2
	{ExpurC_ArcMain_Up6, {  0,   0, 144, 170}, { 92, 160}}, //28 up 2
	
	{ExpurC_ArcMain_Right0, {  0,   0, 233, 142}, { 81, 132}}, //29 right 1
	{ExpurC_ArcMain_Right1, {  0,   0, 219, 146}, { 73, 136}}, //30 right 2
	{ExpurC_ArcMain_Right2, {  0,   0, 219, 147}, { 73, 138}}, //31 right 1
	{ExpurC_ArcMain_Right3, {  0,   0, 216, 147}, { 70, 138}}, //32 right 2
	{ExpurC_ArcMain_Right4, {  0,   0, 218, 146}, { 73, 137}}, //33 right 1
	{ExpurC_ArcMain_Right5, {  0,   0, 218, 147}, { 72, 137}}, //34 right 2
	{ExpurC_ArcMain_Right6, {  0,   0, 219, 148}, { 74, 139}}, //35 right 2
	{ExpurC_ArcMain_Right7, {  0,   0, 219, 148}, { 74, 139}}, //36 right 2

	{ExpurC_ArcMain_Hank0, {  0,   0, 233, 142}, { 81, 132}}, //37 hank 1
	{ExpurC_ArcMain_Hank1, {  0,   0, 219, 146}, { 73, 136}}, //38 hank 2
	{ExpurC_ArcMain_Hank2, {  0,   0, 219, 147}, { 73, 138}}, //39 hank 1
	{ExpurC_ArcMain_Hank3, {  0,   0, 216, 147}, { 70, 138}}, //40 hank 2
	{ExpurC_ArcMain_Hank4, {  0,   0, 218, 146}, { 73, 137}}, //41 hank 1
	{ExpurC_ArcMain_Hank5, {  0,   0, 218, 147}, { 72, 137}}, //42 hank 2
	{ExpurC_ArcMain_Hank6, {  0,   0, 219, 148}, { 74, 139}}, //43 hank 2

	{ExpurC_ArcMain_Intro0, {  0,   0, 233, 142}, { 81, 132}}, //44 intro 1
	{ExpurC_ArcMain_Intro1, {  0,   0, 219, 146}, { 73, 136}}, //45 intro 2
	{ExpurC_ArcMain_Intro2, {  0,   0, 219, 147}, { 73, 138}}, //46 intro 1
	{ExpurC_ArcMain_Intro3, {  0,   0, 216, 147}, { 70, 138}}, //47 intro 2
	{ExpurC_ArcMain_Intro4, {  0,   0, 218, 146}, { 73, 137}}, //48 intro 1
	{ExpurC_ArcMain_Intro5, {  0,   0, 218, 147}, { 72, 137}}, //49 intro 2
	{ExpurC_ArcMain_Intro6, {  0,   0, 219, 148}, { 74, 139}}, //50 intro 2
	{ExpurC_ArcMain_Intro7, {  0,   0, 233, 142}, { 81, 132}}, //51 intro 1
	{ExpurC_ArcMain_Intro8, {  0,   0, 219, 146}, { 73, 136}}, //52 intro 2
	{ExpurC_ArcMain_Intro9, {  0,   0, 219, 147}, { 73, 138}}, //53 intro 1
	{ExpurC_ArcMain_Intro10, {  0,   0, 216, 147}, { 70, 138}}, //54 intro 2
	{ExpurC_ArcMain_Intro11, {  0,   0, 218, 146}, { 73, 137}}, //55 intro 1
	{ExpurC_ArcMain_Intro12, {  0,   0, 218, 147}, { 72, 137}}, //56 intro 2
	{ExpurC_ArcMain_Intro13, {  0,   0, 219, 148}, { 74, 139}}, //57 intro 2
	{ExpurC_ArcMain_Intro14, {  0,   0, 233, 142}, { 81, 132}}, //58 intro 1
	{ExpurC_ArcMain_Intro15, {  0,   0, 219, 146}, { 73, 136}}, //59 intro 2
	{ExpurC_ArcMain_Intro16, {  0,   0, 219, 147}, { 73, 138}}, //60 intro 1
	{ExpurC_ArcMain_Intro17, {  0,   0, 216, 147}, { 70, 138}}, //61 intro 2
	{ExpurC_ArcMain_Intro18, {  0,   0, 218, 146}, { 73, 137}}, //62 intro 1
	{ExpurC_ArcMain_Intro19, {  0,   0, 218, 147}, { 72, 137}}, //63 intro 2
	{ExpurC_ArcMain_Intro20, {  0,   0, 219, 148}, { 74, 139}}, //64 intro 2
};

static const Animation char_expurc_anim[CharAnim_Max] = {
	{1, (const u8[]){ 0, 0,  1, 1,  2, 2,  3, 3, 4, 5, ASCR_BACK, 3}},                                           //CharAnim_Idle
	{2, (const u8[]){ 6, 7, 8, 9, 10, 11, 12, 13, ASCR_BACK, 3}},                                                   //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},                                             //CharAnim_LeftAlt
	{2, (const u8[]){14, 15, 16, 17, 18, 19, 20, 21, ASCR_BACK, 3}},                                                   //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){22, 23,  24, 25, 26, 27, 28, ASCR_BACK, 3}},                                                   //CharAnim_Up
	{2, (const u8[]){37, 38, 39, 40, 41, 42, 43, ASCR_BACK, 1}},                                     //CharAnim_UpAlt
	{2, (const u8[]){29, 30, 31, 32, 33, 34, 35, 36, ASCR_BACK, 3}},                                                   //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},                                             //CharAnim_RightAlt
	{2, (const u8[]){44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, ASCR_BACK, 3 }},

};

//ExpurC character functions
void Char_ExpurC_SetFrame(void *user, u8 frame)
{
	Char_ExpurC *this = (Char_ExpurC*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_expurc_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_ExpurC_Tick(Character *character)
{
	Char_ExpurC *this = (Char_ExpurC*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0 &&
	     character->animatable.anim != CharAnim_DownAlt) //Don't interrupt "Heh, pretty good!" sequence
		Character_PerformIdle(character);
	
	//Animate
	Animatable_Animate(&character->animatable, (void*)this, Char_ExpurC_SetFrame);
	
	//Draw body
	Character_Draw(character, &this->tex, &char_expurc_frame[this->frame]);

	 //Stage specific animations
	     if (stage.song_step <= 0)
	      character->set_anim(character, CharAnim_ClownKillsYou);

		 if (stage.song_step >= 2128 && stage.song_step <= 2140)
			 character->set_anim(character, CharAnim_Up);
}

void Char_ExpurC_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_ExpurC_Free(Character *character)
{
	Char_ExpurC *this = (Char_ExpurC*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_ExpurC_New(fixed_t x, fixed_t y)
{
	//Allocate ExpurC object
	Char_ExpurC *this = Mem_Alloc(sizeof(Char_ExpurC));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_ExpurC_New] Failed to allocate ExpurC object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_ExpurC_Tick;
	this->character.set_anim = Char_ExpurC_SetAnim;
	this->character.free = Char_ExpurC_Free;
	
	Animatable_Init(&this->character.animatable, char_expurc_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;
	
	this->character.focus_x = FIXED_DEC(50,1);
	this->character.focus_y = FIXED_DEC(-80,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\EXPURC.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //ExpurC_ArcMain_Idle0
		"idle1.tim", //ExpurC_ArcMain_Idle1
		"idle2.tim", //ExpurC_ArcMain_Idle0
		"idle3.tim", //ExpurC_ArcMain_Idle1
		"idle4.tim", //ExpurC_ArcMain_Idle0
		"idle5.tim", //ExpurC_ArcMain_Idle1
		"left0.tim",  //ExpurC_ArcMain_Left
		"left1.tim",  //ExpurC_ArcMain_Left
		"left2.tim",  //ExpurC_ArcMain_Left
		"left3.tim",  //ExpurC_ArcMain_Left
		"left4.tim",  //ExpurC_ArcMain_Left
		"left5.tim",  //ExpurC_ArcMain_Left
		"left6.tim",  //ExpurC_ArcMain_Left
		"left7.tim",  //ExpurC_ArcMain_Left
		"down0.tim",  //ExpurC_ArcMain_Down
		"down1.tim",  //ExpurC_ArcMain_Down
		"down2.tim",  //ExpurC_ArcMain_Down
		"down3.tim",  //ExpurC_ArcMain_Down
		"down4.tim",  //ExpurC_ArcMain_Down
		"down5.tim",  //ExpurC_ArcMain_Down
		"down6.tim",  //ExpurC_ArcMain_Down
		"down7.tim",  //ExpurC_ArcMain_Down
		"up0.tim",    //ExpurC_ArcMain_Up
		"up1.tim",    //ExpurC_ArcMain_Up
		"up2.tim",    //ExpurC_ArcMain_Up
		"up3.tim",    //ExpurC_ArcMain_Up
		"up4.tim",    //ExpurC_ArcMain_Up
		"up5.tim",    //ExpurC_ArcMain_Up
		"up6.tim",    //ExpurC_ArcMain_Up
		"right0.tim", //ExpurC_ArcMain_Right
		"right1.tim", //ExpurC_ArcMain_Right
		"right2.tim", //ExpurC_ArcMain_Right
		"right3.tim", //ExpurC_ArcMain_Right
		"right4.tim", //ExpurC_ArcMain_Right
		"right5.tim", //ExpurC_ArcMain_Right
		"right6.tim", //ExpurC_ArcMain_Right
		"right7.tim", //ExpurC_ArcMain_Right

		"hank0.tim",
		"hank1.tim",
		"hank2.tim",
		"hank3.tim",
		"hank4.tim",
		"hank5.tim",
		"hank6.tim",

		"intro0.tim",
		"intro1.tim",
		"intro2.tim",
		"intro3.tim",
		"intro4.tim",
		"intro5.tim",
		"intro6.tim",
		"intro7.tim",
		"intro8.tim",
		"intro9.tim",
		"intro10.tim",
		"intro11.tim",
		"intro12.tim",
		"intro13.tim",
		"intro14.tim",
		"intro15.tim",
		"intro16.tim",
		"intro17.tim",
		"intro18.tim",
		"intro19.tim",
		"intro20.tim",
		
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
