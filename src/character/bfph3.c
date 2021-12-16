/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "bfph3.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//Boyfriend Phase 3 skull fragments
static SkullFragment char_bfph3_skull[15] = {
	{ 1 * 8, -87 * 8, -13, -13},
	{ 9 * 8, -88 * 8,   5, -22},
	{18 * 8, -87 * 8,   9, -22},
	{26 * 8, -85 * 8,  13, -13},
	
	{-3 * 8, -82 * 8, -13, -11},
	{ 8 * 8, -85 * 8,  -9, -15},
	{20 * 8, -82 * 8,   9, -15},
	{30 * 8, -79 * 8,  13, -11},
	
	{-1 * 8, -74 * 8, -13, -5},
	{ 8 * 8, -77 * 8,  -9, -9},
	{19 * 8, -75 * 8,   9, -9},
	{26 * 8, -74 * 8,  13, -5},
	
	{ 5 * 8, -73 * 8, -5, -3},
	{14 * 8, -76 * 8,  9, -6},
	{26 * 8, -67 * 8, 15, -3},
};

//Boyfriend Phase 3 player types
enum
{
	BFPH3_ArcMain_BF0,
	BFPH3_ArcDead_Dead0,
	
	BFPH3_ArcMain_Max,
};

enum
{
	BFPH3_ArcDead_Dead1,
	BFPH3_ArcDead_Dead2,
	
	BFPH3_ArcDead_Max,
};

#define BFPH3_Arc_Max BFPH3_ArcMain_Max

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main, arc_dead;
	CdlFILE file_dead_arc; //dead.arc file position
	IO_Data arc_ptr[BFPH3_Arc_Max];
	
	Gfx_Tex tex, tex_retry;
	u8 frame, tex_id;
	
	u8 retry_bump;
	
	SkullFragment skull[COUNT_OF(char_bfph3_skull)];
	u8 skull_scale;
} Char_BF;

//Boyfriend Phase 3 player definitions
static const CharFrame char_bfph3_frame[] = {
	{BFPH3_ArcMain_BF0, {  0,   0,  39,  39}, { 22,  35}}, //0 idle 1
	{BFPH3_ArcMain_BF0, { 40,   0,  40,  39}, { 23,  35}}, //1 idle 2
	{BFPH3_ArcMain_BF0, { 81,   0,  41,  40}, { 23,  36}}, //2 idle 3
	{BFPH3_ArcMain_BF0, {123,   0,  41,  41}, { 23,  37}}, //3 idle 4
	{BFPH3_ArcMain_BF0, {165,  0,  41,  41}, { 23,  37}}, //4 idle 5
	{BFPH3_ArcMain_BF0, {0,   40,  41,  41}, { 23,  37}}, //5 idle 4
	{BFPH3_ArcMain_BF0, { 42,  40,  42,  41}, { 23,  36}}, //6 idle 5
	
	{BFPH3_ArcMain_BF0, { 86,  42,  37,  40}, { 21,  36}}, //7 left 1
	{BFPH3_ArcMain_BF0, {124,  43,  36,  40}, { 21,  36}}, //8 left 2
	{BFPH3_ArcMain_BF0, {161,  43,  38,  40}, { 21,  36}}, //9 left 2
	
	{BFPH3_ArcMain_BF0, { 77,  81,  37,  36}, { 19,  33}}, //10 down 1
	{BFPH3_ArcMain_BF0, {115,  83,  36,  36}, { 19,  32}}, //11 down 2
	{BFPH3_ArcMain_BF0, {152,  84,  37,  36}, { 19,  32}}, //12 down 2
	
	{BFPH3_ArcMain_BF0, { 78, 120,  37,  45}, { 17,  38}}, //13 up 1
	{BFPH3_ArcMain_BF0, {116, 121,  36,  44}, { 17,  38}}, //14 up 2
	{BFPH3_ArcMain_BF0, {153, 121,  38,  44}, { 18,  38}}, //15 up 2
	
	{BFPH3_ArcMain_BF0, { 76, 166,  41,  40}, { 17,  36}}, //16 right 1
	{BFPH3_ArcMain_BF0, {118, 166,  41,  40}, { 16,  36}}, //17 right 2

	{BFPH3_ArcMain_BF0, {  0,  82,  37,  41}, { 20,  37}}, //18 miss left 
	{BFPH3_ArcMain_BF0, { 39,  83,  37,  42}, { 20,  38}}, //19 miss left 
	{BFPH3_ArcMain_BF0, {  0, 124,  38,  37}, { 19,  34}}, //20 miss down 
	{BFPH3_ArcMain_BF0, { 39, 126,  38,  37}, { 19,  34}}, //21 miss down 
	{BFPH3_ArcMain_BF0, {  0, 162,  37,  43}, { 18,  37}}, //22 miss up 
	{BFPH3_ArcMain_BF0, { 39, 164,  36,  43}, { 18,  38}}, //23 miss up 
	{BFPH3_ArcMain_BF0, {160, 166,  40,  42}, { 17,  39}}, //24 miss right 
	{BFPH3_ArcMain_BF0, {201, 166,  40,  42}, { 18,  39}}, //25 miss right

	{BFPH3_ArcDead_Dead0, {  0,   0, 128, 128}, { 53,  98}}, //26 dead0 0
	{BFPH3_ArcDead_Dead0, {128,   0, 128, 128}, { 53,  98}}, //27 dead0 1
	{BFPH3_ArcDead_Dead0, {  0, 128, 128, 128}, { 53,  98}}, //28 dead0 2
	{BFPH3_ArcDead_Dead0, {128, 128, 128, 128}, { 53,  98}}, //29 dead0 3

	{BFPH3_ArcDead_Dead1, {  0,   0, 128, 128}, { 53,  98}}, //36 dead1 0
	{BFPH3_ArcDead_Dead1, {128,   0, 128, 128}, { 53,  98}}, //37 dead1 1
	{BFPH3_ArcDead_Dead1, {  0, 128, 128, 128}, { 53,  98}}, //38 dead1 2
	{BFPH3_ArcDead_Dead1, {128, 128, 128, 128}, { 53,  98}}, //39 dead1 3

	{BFPH3_ArcDead_Dead2, {  0,   0, 128, 128}, { 53,  98}}, //40 dead2 body twitch 0
	{BFPH3_ArcDead_Dead2, {128,   0, 128, 128}, { 53,  98}}, //41 dead2 body twitch 1
	{BFPH3_ArcDead_Dead2, {  0, 128, 128, 128}, { 53,  98}}, //42 dead2 balls twitch 0
	{BFPH3_ArcDead_Dead2, {128, 128, 128, 128}, { 53,  98}}, //43 dead2 balls twitch 1
};

static const Animation char_bfph3_anim[PlayerAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3, 4, 5, 6, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 7, 8, 9, ASCR_BACK, 1}},            //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},      //CharAnim_LeftAlt
	{2, (const u8[]){ 10, 11, 12, ASCR_BACK, 1}},            //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},      //CharAnim_DownAlt
	{2, (const u8[]){ 13, 14, 15, ASCR_BACK, 1}},            //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},      //CharAnim_UpAlt
	{2, (const u8[]){16, 17, ASCR_BACK, 1}},            //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},      //CharAnim_RightAlt
	
	{2, (const u8[]){ 18, 19, ASCR_BACK, 1 }},      //PlayerAnim_LeftMiss
	{2, (const u8[]){ 20, 21, ASCR_BACK, 1 }},      //PlayerAnim_DownMiss
	{2, (const u8[]){ 22, 23, ASCR_BACK, 1 }},      //PlayerAnim_UpMiss
	{2, (const u8[]){ 24, 25, ASCR_BACK, 1 }},      //PlayerAnim_RightMiss
	
	{2, (const u8[]){20, 21, 22, ASCR_BACK, 1}},        //PlayerAnim_Peace
	{2, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},      //PlayerAnim_Sweat
	
	{5, (const u8[]){23+3, 24+3, 25+3, 26+3, 26+3, 26+3, 26+3, 26+3, 26+3, 26+3, ASCR_CHGANI, PlayerAnim_Dead1}}, //PlayerAnim_Dead0
	{5, (const u8[]){26+3, ASCR_REPEAT}},                                                       //PlayerAnim_Dead1
	{3, (const u8[]){27+3, 28+3, 29+3, 30+3, 30+3, 30+3, 30+3, 30+3, 30+3, 30+3, ASCR_CHGANI, PlayerAnim_Dead3}}, //PlayerAnim_Dead2
	{3, (const u8[]){30+3, ASCR_REPEAT}},                                                       //PlayerAnim_Dead3
	{3, (const u8[]){31+3, 32+3, 30+3, 30+3, 30+3, 30+3, 30+3, ASCR_CHGANI, PlayerAnim_Dead3}},             //PlayerAnim_Dead4
	{3, (const u8[]){33+3, 34+3, 30+3, 30+3, 30+3, 30+3, 30+3, ASCR_CHGANI, PlayerAnim_Dead3}},             //PlayerAnim_Dead5
	
	{10, (const u8[]){30+3, 30+3, 30+3, ASCR_BACK, 1}}, //PlayerAnim_Dead4
	{ 3, (const u8[]){33+3, 34+3, 30+3, ASCR_REPEAT}},  //PlayerAnim_Dead5
};

//Boyfriend Weeb player functions
void Char_BFPH3_SetFrame(void *user, u8 frame)
{
	Char_BF *this = (Char_BF*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_bfph3_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_BFPH3_Tick(Character *character)
{
	Char_BF *this = (Char_BF*)character;
	
	//Handle animation updates
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0 ||
	    (character->animatable.anim != CharAnim_Left &&
	     character->animatable.anim != CharAnim_LeftAlt &&
	     character->animatable.anim != CharAnim_Down &&
	     character->animatable.anim != CharAnim_DownAlt &&
	     character->animatable.anim != CharAnim_Up &&
	     character->animatable.anim != CharAnim_UpAlt &&
	     character->animatable.anim != CharAnim_Right &&
	     character->animatable.anim != CharAnim_RightAlt))
		Character_CheckEndSing(character);
	
	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{
		//Perform idle dance
		if (Animatable_Ended(&character->animatable) &&
			(character->animatable.anim != CharAnim_Left &&
		     character->animatable.anim != CharAnim_LeftAlt &&
		     character->animatable.anim != PlayerAnim_LeftMiss &&
		     character->animatable.anim != CharAnim_Down &&
		     character->animatable.anim != CharAnim_DownAlt &&
		     character->animatable.anim != PlayerAnim_DownMiss &&
		     character->animatable.anim != CharAnim_Up &&
		     character->animatable.anim != CharAnim_UpAlt &&
		     character->animatable.anim != PlayerAnim_UpMiss &&
		     character->animatable.anim != CharAnim_Right &&
		     character->animatable.anim != CharAnim_RightAlt &&
		     character->animatable.anim != PlayerAnim_RightMiss) &&
			(stage.song_step & 0x7) == 0)
			character->set_anim(character, CharAnim_Idle);
	}
	
	//Retry screen
	if (character->animatable.anim >= PlayerAnim_Dead3)
	{
		//Tick skull fragments
		if (this->skull_scale)
		{
			SkullFragment *frag = this->skull;
			for (size_t i = 0; i < COUNT_OF_MEMBER(Char_BF, skull); i++, frag++)
			{
				//Draw fragment
				RECT frag_src = {
					(i & 1) ? 112 : 96,
					(i >> 1) << 4,
					16,
					16
				};
				fixed_t skull_dim = (FIXED_DEC(16,1) * this->skull_scale) >> 6;
				fixed_t skull_rad = skull_dim >> 1;
				RECT_FIXED frag_dst = {
					character->x + (((fixed_t)frag->x << FIXED_SHIFT) >> 3) - skull_rad - stage.camera.x,
					character->y + (((fixed_t)frag->y << FIXED_SHIFT) >> 3) - skull_rad - stage.camera.y,
					skull_dim,
					skull_dim,
				};
				Stage_DrawTex(&this->tex_retry, &frag_src, &frag_dst, FIXED_MUL(stage.camera.zoom, stage.bump));
				
				//Move fragment
				frag->x += frag->xsp;
				frag->y += ++frag->ysp;
			}
			
			//Decrease scale
			this->skull_scale--;
		}
		
		//Draw input options
		u8 input_scale = 16 - this->skull_scale;
		if (input_scale > 16)
			input_scale = 0;
		
		RECT button_src = {
			 0, 96,
			16, 16
		};
		RECT_FIXED button_dst = {
			character->x - FIXED_DEC(32,1) - stage.camera.x,
			character->y - FIXED_DEC(88,1) - stage.camera.y,
			(FIXED_DEC(16,1) * input_scale) >> 4,
			FIXED_DEC(16,1),
		};
		
		//Cross - Retry
		Stage_DrawTex(&this->tex_retry, &button_src, &button_dst, FIXED_MUL(stage.camera.zoom, stage.bump));
		
		//Circle - Blueball
		button_src.x = 16;
		button_dst.y += FIXED_DEC(56,1);
		Stage_DrawTex(&this->tex_retry, &button_src, &button_dst, FIXED_MUL(stage.camera.zoom, stage.bump));
		
		//Draw 'RETRY'
		u8 retry_frame;
		
		if (character->animatable.anim == PlayerAnim_Dead6)
		{
			//Selected retry
			retry_frame = 2 - (this->retry_bump >> 3);
			if (retry_frame >= 3)
				retry_frame = 0;
			if (this->retry_bump & 2)
				retry_frame += 3;
			
			if (++this->retry_bump == 0xFF)
				this->retry_bump = 0xFD;
		}
		else
		{
			//Idle
			retry_frame = 1 +  (this->retry_bump >> 2);
			if (retry_frame >= 3)
				retry_frame = 0;
			
			if (++this->retry_bump >= 55)
				this->retry_bump = 0;
		}
		
		RECT retry_src = {
			(retry_frame & 1) ? 48 : 0,
			(retry_frame >> 1) << 5,
			48,
			32
		};
		RECT_FIXED retry_dst = {
			character->x -  FIXED_DEC(7,1) - stage.camera.x,
			character->y - FIXED_DEC(92,1) - stage.camera.y,
			FIXED_DEC(48,1),
			FIXED_DEC(32,1),
		};
		Stage_DrawTex(&this->tex_retry, &retry_src, &retry_dst, FIXED_MUL(stage.camera.zoom, stage.bump));
	}
	
	//Animate and draw character
	Animatable_Animate(&character->animatable, (void*)this, Char_BFPH3_SetFrame);
	Character_Draw(character, &this->tex, &char_bfph3_frame[this->frame]);
}

void Char_BFPH3_SetAnim(Character *character, u8 anim)
{
	Char_BF *this = (Char_BF*)character;
	
	//Perform animation checks
	switch (anim)
	{
		case PlayerAnim_Dead0:
			//Begin reading dead.arc and adjust focus
			this->arc_dead = IO_AsyncReadFile(&this->file_dead_arc);
			character->focus_x = FIXED_DEC(0,1);
			character->focus_y = FIXED_DEC(-40,1);
			character->focus_zoom = FIXED_DEC(125,100);			
			break;
		case PlayerAnim_Dead2:
			//Unload main.arc
			Mem_Free(this->arc_main);
			this->arc_main = this->arc_dead;
			this->arc_dead = NULL;
			
			//Find dead.arc files
			const char **pathp = (const char *[]){
				"dead1.tim", //BF_ArcDead_Dead1
				"dead2.tim", //BF_ArcDead_Dead2
				"retry.tim", //BF_ArcDead_Retry
				NULL
			};
			IO_Data *arc_ptr = this->arc_ptr;
			for (; *pathp != NULL; pathp++)
				*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
			break;
	}
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_BFPH3_Free(Character *character)
{
	Char_BF *this = (Char_BF*)character;
	
	//Free art
	Mem_Free(this->arc_main);
	Mem_Free(this->arc_dead);
}

Character *Char_BFPH3_New(fixed_t x, fixed_t y)
{
	//Allocate boyfriend object
	Char_BF *this = Mem_Alloc(sizeof(Char_BF));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BFPH3_New] Failed to allocate boyfriend weeb object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BFPH3_Tick;
	this->character.set_anim = Char_BFPH3_SetAnim;
	this->character.free = Char_BFPH3_Free;
	
	Animatable_Init(&this->character.animatable, char_bfph3_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = CHAR_SPEC_MISSANIM;
	
	this->character.health_i = 5;
	
	this->character.focus_x = FIXED_DEC(-30, 1);
	this->character.focus_y = FIXED_DEC(-60, 1);
	this->character.focus_zoom = FIXED_DEC(1, 1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\BFPH3.ARC;1");
	this->arc_dead = NULL;
	IO_FindFile(&this->file_dead_arc, "\\CHAR\\BFDEAD.ARC;1");
	
	const char **pathp = (const char *[]){
		"ph3.tim",  //BFPH3_ArcMain_BF0
		"dead0.tim",  //BFPH3_ArcMain_BF0
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	//Initialize player state
	this->retry_bump = 0;
	
	//Copy skull fragments
	memcpy(this->skull, char_bfph3_skull, sizeof(char_bfph3_skull));
	this->skull_scale = 64;
	
	SkullFragment *frag = this->skull;
	for (size_t i = 0; i < COUNT_OF_MEMBER(Char_BF, skull); i++, frag++)
	{
		//Randomize trajectory
		frag->xsp += RandomRange(-4, 4);
		frag->ysp += RandomRange(-2, 2);
	}
	
	return (Character*)this;
}
