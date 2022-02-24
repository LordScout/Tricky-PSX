/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "clown.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"
#include "../timer.h"

//Tricky character structure
enum
{
	Clown_ArcMain_Idle0,
	Clown_ArcMain_Idle1,
	Clown_ArcMain_Idle2,
	Clown_ArcMain_Idle3,
	Clown_ArcMain_Idle4,
	Clown_ArcMain_Idle5,
	Clown_ArcMain_Idle6,
	Clown_ArcMain_Left0,
	Clown_ArcMain_Left1,
	Clown_ArcMain_Left2,
	Clown_ArcMain_Left3,
	Clown_ArcMain_Left4,
	Clown_ArcMain_Left5,
	Clown_ArcMain_Left6,
	Clown_ArcMain_Down0,
	Clown_ArcMain_Down1,
	Clown_ArcMain_Down2,
	Clown_ArcMain_Down3,
	Clown_ArcMain_Down4,
	Clown_ArcMain_Down5,
	Clown_ArcMain_Up0,
	Clown_ArcMain_Up1,
	Clown_ArcMain_Up2,
	Clown_ArcMain_Up3,
	Clown_ArcMain_Up4,
	Clown_ArcMain_Up5,
	Clown_ArcMain_Up6,
	Clown_ArcMain_Right0,
	Clown_ArcMain_Right1,
	Clown_ArcMain_Right2,
	Clown_ArcMain_Right3,
	Clown_ArcMain_Right4,
	Clown_ArcMain_Right5,
	Clown_ArcMain_Right6,


	Clown_ArcScene_0, //ugh0 good0
	Clown_ArcScene_1, //ugh1 good1
	Clown_ArcScene_2, //good2
	Clown_ArcScene_3, //good3
	
	Clown_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main, arc_scene;
	IO_Data arc_ptr[Clown_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
	
	//Mouth state
	fixedu_t mouth_i;
} Char_Clown;

//Clown character definitions
static const CharFrame char_clown_frame[] = {
	{Clown_ArcMain_Idle0, {  0,   0, 152, 154}, { 74, 144}}, //0 idle 1
	{Clown_ArcMain_Idle1, {  0,   0, 153, 152}, { 73, 142}}, //1 idle 2
	{Clown_ArcMain_Idle2, {  0,   0, 155, 160}, { 75, 150}}, //2 idle 3
	{Clown_ArcMain_Idle3, {  0,   0, 154, 163}, { 72, 153}}, //3 idle 4
	{Clown_ArcMain_Idle4, {  0,   0, 155, 161}, { 75, 151}}, //4 idle 2
	{Clown_ArcMain_Idle5, {  0,   0, 155, 164}, { 74, 154}}, //5 idle 3
	{Clown_ArcMain_Idle6, {  0,   0, 154, 162}, { 72, 152}}, //6 idle 4
	
	{Clown_ArcMain_Left0, {  0,   0, 202, 169}, { 124, 158}}, //7 left 1
	{Clown_ArcMain_Left1, {  0,   0, 198, 169}, { 124, 158}}, //8 left 1
	{Clown_ArcMain_Left2, {  0,   0, 188, 173}, { 118, 162}}, //9 left 1
	{Clown_ArcMain_Left3, {  0,   0, 190, 169}, { 119, 159}}, //10 left 1
	{Clown_ArcMain_Left4, {  0,   0, 192, 171}, { 123, 160}}, //11 left 1
	{Clown_ArcMain_Left5, {  0,   0, 187, 170}, { 119, 159}}, //12 left 1
	{Clown_ArcMain_Left6, {  0,   0, 188, 173}, { 119, 162}}, //13 left 1
	
	{Clown_ArcMain_Down0, {  0,   0, 185, 149}, { 84, 138}}, //14 down 1
	{Clown_ArcMain_Down1, {  0,   0, 182, 150}, { 83, 138}}, //15 down 2
	{Clown_ArcMain_Down2, {  0,   0, 183, 153}, { 79, 142}}, //16 down 1
	{Clown_ArcMain_Down3, {  0,   0, 180, 152}, { 78, 140}}, //17 down 2
	{Clown_ArcMain_Down4, {  0,   0, 181, 154}, { 80, 142}}, //18 down 1
	{Clown_ArcMain_Down5, {  0,   0, 180, 153}, { 77, 142}}, //19 down 1
	
	{Clown_ArcMain_Up0, {  0,   0, 147, 169}, { 87, 160}}, //20 up 1
	{Clown_ArcMain_Up1, {  0,   0, 147, 166}, { 90, 157}}, //21 up 2
	{Clown_ArcMain_Up2, {  0,   0, 145, 169}, { 93, 159}}, //22 up 1
	{Clown_ArcMain_Up3, {  0,   0, 145, 165}, { 92, 155}}, //23 up 2
	{Clown_ArcMain_Up4, {  0,   0, 145, 167}, { 94, 157}}, //24 up 1
	{Clown_ArcMain_Up5, {  0,   0, 149, 167}, { 95, 157}}, //25 up 2
	{Clown_ArcMain_Up6, {  0,   0, 144, 170}, { 92, 160}}, //26 up 2
	
	{Clown_ArcMain_Right0, {  0,   0, 233, 142}, { 81, 132}}, //27 right 1
	{Clown_ArcMain_Right1, {  0,   0, 219, 146}, { 73, 136}}, //28 right 2
	{Clown_ArcMain_Right2, {  0,   0, 219, 147}, { 73, 138}}, //29 right 1
	{Clown_ArcMain_Right3, {  0,   0, 216, 147}, { 70, 138}}, //30 right 2
	{Clown_ArcMain_Right4, {  0,   0, 218, 146}, { 73, 137}}, //31 right 1
	{Clown_ArcMain_Right5, {  0,   0, 218, 147}, { 72, 137}}, //32 right 2
	{Clown_ArcMain_Right6, {  0,   0, 219, 148}, { 74, 139}}, //33 right 2
	
	{Clown_ArcScene_0, {  0,   0, 128, 256}, { 48, 125}}, //35 ugh 0
	{Clown_ArcScene_0, {128,   0, 128, 256}, { 50, 128}}, //36 ugh 1
	{Clown_ArcScene_1, {  0,   0, 128, 256}, { 49, 128}}, //37 ugh 2
	{Clown_ArcScene_1, {128,   0, 128, 256}, { 49, 128}}, //38 ugh 3
	
	
	{Clown_ArcScene_0, {  0,   0, 128, 256}, { 53, 128}}, //16 good 0
	{Clown_ArcScene_0, {128,   0, 128, 256}, { 53, 133}}, //17 good 1
	{Clown_ArcScene_1, {  0,   0, 128, 256}, { 53, 131}}, //18 good 2
	{Clown_ArcScene_1, {128,   0, 128, 256}, { 53, 131}}, //19 good 3
	{Clown_ArcScene_2, {  0,   0, 128, 256}, { 53, 131}}, //20 good 4
	{Clown_ArcScene_2, {128,   0, 128, 256}, { 52, 126}}, //21 good 5
	{Clown_ArcScene_3, {  0,   0, 132, 256}, { 53, 127}}, //22 good 6
};

static const Animation char_clown_anim[CharAnim_Max] = {
	{1, (const u8[]){ 0, 0,  1, 1,  2, 2,  3, 3, 4, 5, 6, ASCR_BACK, 3}},                                           //CharAnim_Idle
	{1, (const u8[]){ 7, 8, 9, 10, 11, 12, 13, ASCR_BACK, 3}},                                                   //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},                                             //CharAnim_LeftAlt
	{1, (const u8[]){ 14, 15, 16, 17, 18, 19, ASCR_BACK, 3}},                                                   //CharAnim_Down
	{2, (const u8[]){16, 17, 18, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,   //CharAnim_DownAlt
	                 19, 20, 21, 22, ASCR_BACK, 1}},
	{1, (const u8[]){ 20, 21,  22, 23, 24, 25, 26, ASCR_BACK, 3}},                                                   //CharAnim_Up
	{2, (const u8[]){12, 13, 14, 15, ASCR_BACK, 1}},                                           //CharAnim_UpAlt
	{1, (const u8[]){ 27, 28, 29, 30, 31, 32, 33, ASCR_BACK, 3}},                                                   //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},                                             //CharAnim_RightAlt
};

//Clown character functions
void Char_Clown_SetFrame(void *user, u8 frame)
{
	Char_Clown *this = (Char_Clown*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_clown_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Clown_Tick(Character *character)
{
	Char_Clown *this = (Char_Clown*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0 &&
	     character->animatable.anim != CharAnim_DownAlt) //Don't interrupt "Heh, pretty good!" sequence
		Character_PerformIdle(character);
	
	//Animate
	Animatable_Animate(&character->animatable, (void*)this, Char_Clown_SetFrame);
	
	//Draw mouth if saying "...pretty good!"
	if (this->frame == 22)
	{
		//Mouth mappings
		static const u8 mouth_map[] = {
			0, 1, 1, 2, 2, 2,       //etty
			4, 4, 5, 5, 6, 6, 7, 7, //good!
		};
		
		//Get mouth frame
		u8 mouth_frame = mouth_map[(this->mouth_i * 24) >> FIXED_SHIFT];
		
		this->mouth_i += timer_dt;
		if ((this->mouth_i * 24) >= (COUNT_OF(mouth_map) << FIXED_SHIFT))
			this->mouth_i = ((COUNT_OF(mouth_map) - 1) << FIXED_SHIFT) / 24;
		
		//Draw mouth
		RECT mouth_src = {
			144 + (mouth_frame & 3) * 24,
			(mouth_frame >> 2) << 4,
			24,
			16
		};
		RECT_FIXED mouth_dst = {
			character->x - FIXED_DEC(12,1) - stage.camera.x,
			character->y - FIXED_DEC(84,1) - stage.camera.y,
			FIXED_DEC(24,1),
			FIXED_DEC(16,1)
		};
		
		Stage_DrawTex(&this->tex, &mouth_src, &mouth_dst, stage.camera.bzoom);
	}
	
	//Draw body
	Character_Draw(character, &this->tex, &char_clown_frame[this->frame]);
}

void Char_Clown_SetAnim(Character *character, u8 anim)
{
	//Set animation
	if (anim == CharAnim_DownAlt && character->animatable.anim != CharAnim_DownAlt)
	{
		character->focus_x = FIXED_DEC(120,1);
		character->focus_y = FIXED_DEC(-100,1);
		character->focus_zoom = FIXED_DEC(78, 100);
	}
	else if (anim != CharAnim_DownAlt && character->animatable.anim == CharAnim_DownAlt)
	{
		character->focus_x = FIXED_DEC(65,1);
		character->focus_y = FIXED_DEC(-80,1);
		character->focus_zoom = FIXED_UNIT;
	}
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Clown_Free(Character *character)
{
	Char_Clown *this = (Char_Clown*)character;
	
	//Free art
	Mem_Free(this->arc_main);
	Mem_Free(this->arc_scene);
}

Character *Char_Clown_New(fixed_t x, fixed_t y)
{
	//Allocate Clown object
	Char_Clown *this = Mem_Alloc(sizeof(Char_Clown));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Clown_New] Failed to allocate Clown object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Clown_Tick;
	this->character.set_anim = Char_Clown_SetAnim;
	this->character.free = Char_Clown_Free;
	
	Animatable_Init(&this->character.animatable, char_clown_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;
	
	this->character.focus_x = FIXED_DEC(50,1);
	this->character.focus_y = FIXED_DEC(-80,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\CLOWN.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //Clown_ArcMain_Idle0
		"idle1.tim", //Clown_ArcMain_Idle1
		"idle2.tim", //Clown_ArcMain_Idle0
		"idle3.tim", //Clown_ArcMain_Idle1
		"idle4.tim", //Clown_ArcMain_Idle0
		"idle5.tim", //Clown_ArcMain_Idle1
		"idle6.tim", //Clown_ArcMain_Idle1
		"left0.tim",  //Clown_ArcMain_Left
		"left1.tim",  //Clown_ArcMain_Left
		"left2.tim",  //Clown_ArcMain_Left
		"left3.tim",  //Clown_ArcMain_Left
		"left4.tim",  //Clown_ArcMain_Left
		"left5.tim",  //Clown_ArcMain_Left
		"left6.tim",  //Clown_ArcMain_Left
		"down0.tim",  //Clown_ArcMain_Down
		"down1.tim",  //Clown_ArcMain_Down
		"down2.tim",  //Clown_ArcMain_Down
		"down3.tim",  //Clown_ArcMain_Down
		"down4.tim",  //Clown_ArcMain_Down
		"down5.tim",  //Clown_ArcMain_Down
		"up0.tim",    //Clown_ArcMain_Up
		"up1.tim",    //Clown_ArcMain_Up
		"up2.tim",    //Clown_ArcMain_Up
		"up3.tim",    //Clown_ArcMain_Up
		"up4.tim",    //Clown_ArcMain_Up
		"up5.tim",    //Clown_ArcMain_Up
		"up6.tim",    //Clown_ArcMain_Up
		"right0.tim", //Clown_ArcMain_Right
		"right1.tim", //Clown_ArcMain_Right
		"right2.tim", //Clown_ArcMain_Right
		"right3.tim", //Clown_ArcMain_Right
		"right4.tim", //Clown_ArcMain_Right
		"right5.tim", //Clown_ArcMain_Right
		"right6.tim", //Clown_ArcMain_Right
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
