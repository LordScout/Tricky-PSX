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
	{ExpurC_ArcMain_Idle0, {  0,   0, 192, 201}, { 97, 195}}, //0 idle 1
	{ExpurC_ArcMain_Idle1, {  0,   0, 241, 200}, { 122, 192}}, //1 idle 2
	{ExpurC_ArcMain_Idle2, {  0,   0, 173, 197}, { 82, 190}}, //2 idle 3
	{ExpurC_ArcMain_Idle3, {  0,   0, 254, 192}, { 144, 186}}, //3 idle 4
	{ExpurC_ArcMain_Idle4, {  0,   0, 230, 194}, { 140, 186}}, //4 idle 2
	{ExpurC_ArcMain_Idle5, {  0,   0, 224, 185}, { 121, 180}}, //5 idle 3
	
	{ExpurC_ArcMain_Left0, {  0,   0, 230, 217}, { 166, 195}}, //6 left 1
	{ExpurC_ArcMain_Left1, {  0,   0, 186, 206}, { 146, 186}}, //7 left 1
	{ExpurC_ArcMain_Left2, {  0,   0, 170, 197}, { 149, 180}}, //8 left 1
	{ExpurC_ArcMain_Left3, {  0,   0, 154, 205}, { 128, 186}}, //9 left 1
	{ExpurC_ArcMain_Left4, {  0,   0, 151, 196}, { 127, 178}}, //10 left 1
	{ExpurC_ArcMain_Left5, {  0,   0, 145, 226}, { 118, 207}}, //11 left 1
	{ExpurC_ArcMain_Left6, {  0,   0, 138, 193}, { 117, 176}}, //12 left 1
	{ExpurC_ArcMain_Left7, {  0,   0, 136, 205}, { 105, 185}}, //13 left 1
	
	{ExpurC_ArcMain_Down0, {  0,   0, 170, 137}, { 86, 120}}, //14 down 1
	{ExpurC_ArcMain_Down1, {  0,   0, 160, 140}, { 77, 125}}, //15 down 2
	{ExpurC_ArcMain_Down2, {  0,   0, 140, 129}, { 70, 121}}, //16 down 1
	{ExpurC_ArcMain_Down3, {  0,   0, 140, 139}, { 73, 129}}, //17 down 2
	{ExpurC_ArcMain_Down4, {  0,   0, 140, 139}, { 73, 129}}, //18 down 1
	{ExpurC_ArcMain_Down5, {  0,   0, 140, 127}, { 73, 117}}, //19 down 1
	{ExpurC_ArcMain_Down6, {  0,   0, 150, 126}, { 81, 118}}, //20 down 1
	{ExpurC_ArcMain_Down7, {  0,   0, 156, 128}, { 70, 118}}, //21 down 1
	
	{ExpurC_ArcMain_Up0, {  0,   0, 240, 232}, { 136, 221}}, //22 up 1
	{ExpurC_ArcMain_Up1, {  0,   0, 218, 214}, { 122, 204}}, //23 up 2
	{ExpurC_ArcMain_Up2, {  0,   0, 196, 194}, { 109, 186}}, //24 up 1
	{ExpurC_ArcMain_Up3, {  0,   0, 194, 198}, { 106, 191}}, //25 up 2
	{ExpurC_ArcMain_Up4, {  0,   0, 198, 188}, { 111, 179}}, //26 up 1
	{ExpurC_ArcMain_Up5, {  0,   0, 196, 193}, { 95, 185}}, //27 up 2
	{ExpurC_ArcMain_Up6, {  0,   0, 198, 199}, { 105, 192}}, //28 up 2
	
	{ExpurC_ArcMain_Right0, {  0,   0, 249, 199}, { 41, 180}}, //29 right 1
	{ExpurC_ArcMain_Right1, {  0,   0, 233, 198}, { 39, 179}}, //30 right 2
	{ExpurC_ArcMain_Right2, {  0,   0, 197, 189}, { 34, 172}}, //31 right 1
	{ExpurC_ArcMain_Right3, {  0,   0, 199, 174}, { 33, 154}}, //32 right 2
	{ExpurC_ArcMain_Right4, {  0,   0, 182, 183}, { 35, 158}}, //33 right 1
	{ExpurC_ArcMain_Right5, {  0,   0, 192, 174}, { 33, 155}}, //34 right 2
	{ExpurC_ArcMain_Right6, {  0,   0, 204, 181}, { 34, 162}}, //35 right 2
	{ExpurC_ArcMain_Right7, {  0,   0, 176, 185}, { 34, 166}}, //36 right 2

	{ExpurC_ArcMain_Hank0, {  0,   0, 256, 223}, { 129, 220}}, //37 hank 1
	{ExpurC_ArcMain_Hank1, {  0,   0, 235, 214}, { 126, 212}}, //38 hank 2
	{ExpurC_ArcMain_Hank2, {  0,   0, 200, 192}, { 102, 189}}, //39 hank 1
	{ExpurC_ArcMain_Hank3, {  0,   0, 195, 196}, { 88, 192}}, //40 hank 2
	{ExpurC_ArcMain_Hank4, {  0,   0, 207, 180}, { 84, 176}}, //41 hank 1
	{ExpurC_ArcMain_Hank5, {  0,   0, 185, 187}, { 77, 184}}, //42 hank 2
	{ExpurC_ArcMain_Hank6, {  0,   0, 195, 191}, { 93, 188}}, //43 hank 2

	{ExpurC_ArcMain_Intro0, {  0,   0, 124, 60}, { 70, 35}}, //44 intro 1
	{ExpurC_ArcMain_Intro1, {  0,   0, 172, 165}, { 96, 141}}, //45 intro 2
	{ExpurC_ArcMain_Intro2, {  0,   0, 174, 244}, { 97, 239}}, //46 intro 1
	{ExpurC_ArcMain_Intro3, {  0,   0, 169, 202}, { 98, 195}}, //47 intro 2
	{ExpurC_ArcMain_Intro4, {  0,   0, 160, 185}, { 95, 180}}, //48 intro 1
	{ExpurC_ArcMain_Intro5, {  0,   0, 159, 185}, { 95, 180}}, //49 intro 2
	{ExpurC_ArcMain_Intro6, {  0,   0, 152, 187}, { 83, 181}}, //50 intro 2
	{ExpurC_ArcMain_Intro7, {  0,   0, 200, 200}, { 92, 197}}, //51 intro 1
	{ExpurC_ArcMain_Intro8, {  0,   0, 204, 182}, { 77, 171}}, //52 intro 2
	{ExpurC_ArcMain_Intro9, {  0,   0, 179, 174}, { 56, 161}}, //53 intro 1
	{ExpurC_ArcMain_Intro10, {  0,   0, 187, 177}, { 59, 164}}, //54 intro 2
	{ExpurC_ArcMain_Intro11, {  0,   0, 183, 179}, { 57, 166}}, //55 intro 1
	{ExpurC_ArcMain_Intro12, {  0,   0, 184, 175}, { 62, 164}}, //56 intro 2
	{ExpurC_ArcMain_Intro13, {  0,   0, 162, 182}, { 46, 171}}, //57 intro 2
	{ExpurC_ArcMain_Intro14, {  0,   0, 178, 182}, { 57, 170}}, //58 intro 1
	{ExpurC_ArcMain_Intro15, {  0,   0, 200, 182}, { 75, 171}}, //59 intro 2
	{ExpurC_ArcMain_Intro16, {  0,   0, 224, 195}, { 96, 186}}, //60 intro 1
	{ExpurC_ArcMain_Intro17, {  0,   0, 209, 194}, { 106, 186}}, //61 intro 2
	{ExpurC_ArcMain_Intro18, {  0,   0, 219, 193}, { 109, 186}}, //62 intro 1
	{ExpurC_ArcMain_Intro19, {  0,   0, 156, 193}, { 54, 187}}, //63 intro 2
	{ExpurC_ArcMain_Intro20, {  0,   0, 196, 192}, { 91, 185}}, //64 intro 2
	{ExpurC_ArcMain_Intro0, {  0,   0, 0, 0}, { 74, 139}}, //65 intro 2
};

static const Animation char_expurc_anim[CharAnim_Max] = {
	{1, (const u8[]){ 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 44, 45, 46, 46, 46, 47, 47, 47, 48, 48, 49, 49, 50, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64,54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, ASCR_CHGANI, CharAnim_ClownKillsYou}},     //CharAnim_Idle
	{2, (const u8[]){ 6, 7, 8, 9, 10, 11, 12, 13, ASCR_CHGANI, CharAnim_ClownKillsYou}},                                                   //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},                                     //CharAnim_LeftAlt
	{2, (const u8[]){14, 15, 16, 17, 18, 19, 20, 21, ASCR_CHGANI, CharAnim_ClownKillsYou}},                                                   //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){22, 23,  24, 25, 26, 27, 28, ASCR_CHGANI, CharAnim_ClownKillsYou}},                                                   //CharAnim_Up
	{2, (const u8[]) { 37, 37, 37, 37, 38, 38, 39, 39, 40, 40, 41, 42, 43, 42, 43, 43, ASCR_BACK, 1 }},                                    //CharAnim_Hank
	{2, (const u8[]){29, 30, 31, 32, 33, 34, 35, 36, ASCR_CHGANI, CharAnim_ClownKillsYou}},                                                   //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},                                             //CharAnim_RightAlt
	{1, (const u8[]) { 0, 1,  2, 3, 4, 5, 0,  1, 2, 3, 4, 5, ASCR_REPEAT }},

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

	if (stage.stage_id == StageId_1_4 && stage.song_step >= 44)
	{
		this->character.focus_x = FIXED_DEC(50, 1);
		this->character.focus_y = FIXED_DEC(-80, 1);
		this->character.focus_zoom = FIXED_DEC(7, 10);
	}

	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0 &&
	     character->animatable.anim != CharAnim_DownAlt) //Don't interrupt "Heh, pretty good!" sequence
		Character_PerformIdle(character);
	
	//Animate
	Animatable_Animate(&character->animatable, (void*)this, Char_ExpurC_SetFrame);
	
	//Draw body
	Character_Draw(character, &this->tex, &char_expurc_frame[this->frame]);

	 //Stage specific animations

		 if (stage.song_step >= 2128 && stage.song_step <= 2140)
			 character->set_anim(character, CharAnim_UpAlt);
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
	
	this->character.health_i = 4;
	
	this->character.focus_x = FIXED_DEC(50,1);
	this->character.focus_y = FIXED_DEC(-60,1);
	this->character.focus_zoom = FIXED_DEC(7,10);
	
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
