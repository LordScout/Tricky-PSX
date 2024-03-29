
/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "menu.h"

#include "mem.h"
#include "main.h"
#include "timer.h"
#include "io.h"
#include "gfx.h"
#include "audio.h"
#include "pad.h"
#include "archive.h"
#include "mutil.h"
#include "network.h"

#include "font.h"
#include "trans.h"
#include "loadscr.h"

#include "stage.h"
#include "character/mticky.h"
#include "character/bf.h"
#include "character/clown.h"

//Menu messages
static const char* funny_messages[][12] = {
	{"SCREW YOU GUYS", "IM GOING HOME", "I PUT MYSELF ON PSX MORTY", "IM PSX RIIIIIIIIIIIIIIICK", "LORD SCOUT", "AWARD WINNING SONIC NERD", "lool", "inverted colours", "NO FMV CUTSCENES", "SORRY", "USE A CONTROLLER", "LOL"},
	{"LORD SCOUT", "CHECK OUT HIS YOUTUBE", "SHOUTOUT TO ROZEBUD", "HE DID MUSIC", "SHOUTOUT TO CVAL", "HE CHARTED", "SHOUTOUT TO BANBUDS", "HE ARTED", "SHOUTOUT TO SMOKEY", "HE FARTED", "SHOUTOUT TO KADEDEV", "HE IS KADEDEV"},
	{"PLAYING ON EPSXE HUH", "YOURE THE PROBLEM", "YOU DO NOT KILL CLOWN", "CLOWN KILLS YOU", "TRICKY X CAROL", "DIE", "HE DOES NOT", "SOUND LIKE WHITTY", "AVERAGE WHITTY FAN", "AVERAGE TRICKY ENJOYER", "CHECK OUT JADS", "HE DOES MUSIC"},
	
};

#ifdef PSXF_NETWORK

//Menu string type
#define MENUSTR_CHARS 0x20
typedef char MenuStr[MENUSTR_CHARS + 1];

#endif

//Menu state
static struct
{
	//Menu state
	u8 page, next_page;
	boolean page_swap;
	u8 select, next_select, select_alt;
	
	//stuff for music
	boolean swapmusic;
	
	fixed_t scroll;
	fixed_t trans_time;
	
	//Page specific state
	union
	{
		struct
		{
			u8 funny_message;
		} opening;
		struct
		{
			fixed_t logo_bump;
			fixed_t fade, fadespd;
		} title;
		struct
		{
			fixed_t fade, fadespd;
		} story;
		struct
		{
			fixed_t back_r, back_g, back_b;
		} freeplay;
	#ifdef PSXF_NETWORK
		struct
		{
			boolean type;
			MenuStr port;
			MenuStr pass;
		} net_host;
		struct
		{
			boolean type;
			MenuStr ip;
			MenuStr port;
			MenuStr pass;
		} net_join;
		struct
		{
			boolean swap;
		} net_op;
	#endif
	} page_state;
	
	union
	{
		struct
		{
			u8 id, diff;
			boolean story;
		} stage;
	} page_param;
	
	//Menu assets
	Gfx_Tex tex_back, tex_ng, tex_credits0, tex_credits1, tex_story, tex_title, tex_menu0, tex_menu1, tex_credits;
	FontData font_bold, font_arial;
	
	Character *tricky; //Title Tricky
	Character* boyfriend; //Title bf
} menu;

#ifdef PSXF_NETWORK

//Menu string functions
static void MenuStr_Process(MenuStr *this, s32 x, s32 y, const char *fmt, boolean select, boolean type)
{
	//Append typed input
	if (select && type)
	{
		if (pad_type[0] != '\0')
			strncat(*this, pad_type, MENUSTR_CHARS - strlen(*this));
		if (pad_backspace)
		{
			size_t i = strlen(*this);
			if (i != 0)
				(*this)[i - 1] = '\0';
		}
	}
	
	//Get text to draw
	char buf[0x100];
	sprintf(buf, fmt, *this);
	if (select && type && (animf_count & 2))
		strcat(buf, "_");
	
	//Draw text
	menu.font_arial.draw_col(&menu.font_arial, buf, x, y, FontAlign_Left, 0x80, 0x80, select ? 0x00 : 0x80);
	menu.font_arial.draw_col(&menu.font_arial, buf, x+1, y+1, FontAlign_Left, 0x00, 0x00, 0x00);
}

#endif


//Internal menu functions
char menu_text_buffer[0x100];

static void Menu_Tab(s8 *movetab, boolean moving, boolean flip, u16 x, u8 y)
{
	//move code for tab
	if ((moving && *movetab < 12 && flip == false) || (moving && *movetab > -12 && flip == true))
	 *movetab+= (flip) ? -1 : 1;

	 else if ((moving == false && *movetab != 0))
	 *movetab-= (flip) ? -1 : 1;

	//get src and dst of tab
	RECT tab_src = {186, 0, 69, 30};
	RECT tab_dst = {x, y - *movetab, 69, 30};

	//invert tab
	if (flip)
	tab_dst.h = -tab_dst.h;


   // FntPrint("movetab %d", *movetab);

	//draw tab
	Gfx_DrawTex(&menu.tex_menu0, &tab_src, &tab_dst);

}

static void Menu_TabRed(s8* movetab, boolean moving, boolean flip, u16 x, u8 y)
{
	//move code for tab
	if ((moving && *movetab < 12 && flip == false) || (moving && *movetab > -12 && flip == true))
		*movetab += (flip) ? -1 : 1;

	else if ((moving == false && *movetab != 0))
		*movetab -= (flip) ? -1 : 1;

	//get src and dst of tab
	RECT tabred_src = { 186, 0, 69, 30 };
	RECT tabred_dst = { x, y - *movetab, 69, 30 };

	//invert tab
	if (flip)
		tabred_dst.h = -tabred_dst.h;


	// FntPrint("movetab %d", *movetab);

	 //draw tab
	Gfx_DrawTexCol(&menu.tex_menu0, &tabred_src, &tabred_dst, 255, 0, 0);

}

static const char *Menu_LowerIf(const char *text, boolean lower)
{
	//Copy text
	char *dstp = menu_text_buffer;
	if (lower)
	{
		for (const char *srcp = text; *srcp != '\0'; srcp++)
		{
			if (*srcp >= 'A' && *srcp <= 'Z')
				*dstp++ = *srcp | 0x20;
			else
				*dstp++ = *srcp;
		}
	}
	else
	{
		for (const char *srcp = text; *srcp != '\0'; srcp++)
		{
			if (*srcp >= 'a' && *srcp <= 'z')
				*dstp++ = *srcp & ~0x20;
			else
				*dstp++ = *srcp;
		}
	}
	
	//Terminate text
	*dstp++ = '\0';
	return menu_text_buffer;
}

static void Menu_DrawBack(boolean flash, s32 scroll, u8 r0, u8 g0, u8 b0, u8 r1, u8 g1, u8 b1)
{
	RECT back_src = {0, 0, 255, 255};
	RECT back_dst = {0, -scroll - SCREEN_WIDEADD2, SCREEN_WIDTH, SCREEN_WIDTH * 4 / 5};
	
	if (flash || (animf_count & 4) == 0)
		Gfx_DrawTexCol(&menu.tex_back, &back_src, &back_dst, r0, g0, b0);
	else
		Gfx_DrawTexCol(&menu.tex_back, &back_src, &back_dst, r1, g1, b1);

}
static void Menu_DifficultySelectorMainMenu(s32 x, s32 y)
{
	//Change difficulty
	if (menu.next_page == menu.page && Trans_Idle())
	{
		if (pad_state.press & PAD_LEFT)
		{
			if (menu.page_param.stage.diff > StageDiff_Easy)
				menu.page_param.stage.diff--;
			else
				menu.page_param.stage.diff = StageDiff_Hard;
		}
		if (pad_state.press & PAD_RIGHT)
		{
			if (menu.page_param.stage.diff < StageDiff_Hard)
				menu.page_param.stage.diff++;
			else
				menu.page_param.stage.diff = StageDiff_Easy;
		}
	}
	
	//Draw difficulty
	static const RECT diff_srcs[] = {
		{  1, 60, 28, 10},
		{ 34, 60, 47, 10},
		{ 88, 60, 31, 10},
	};
	
	for (u8 i = 0; i < 3; i++)
	{
	const RECT *diff_src = &diff_srcs[i];
	Gfx_BlitTexCol(&menu.tex_menu1, diff_src, x +  (i *65), y - 9 + ((pad_state.press & (PAD_LEFT | PAD_RIGHT)) != 0), (i != menu.page_param.stage.diff) ? 255 : 0x80, (i != menu.page_param.stage.diff) ? 0 : 0x80, (i != menu.page_param.stage.diff) ? 0 : 0x80);
	}
}

static void Menu_DifficultySelector(s32 x, s32 y)
{
	//Change difficulty
	if (menu.next_page == menu.page && Trans_Idle())
	{
		if (pad_state.press & PAD_LEFT)
		{
			if (menu.page_param.stage.diff > StageDiff_Easy)
				menu.page_param.stage.diff--;
			else
				menu.page_param.stage.diff = StageDiff_Hard;
		}
		if (pad_state.press & PAD_RIGHT)
		{
			if (menu.page_param.stage.diff < StageDiff_Hard)
				menu.page_param.stage.diff++;
			else
				menu.page_param.stage.diff = StageDiff_Easy;
		}
	}
	
	//Draw difficulty arrows
	static const RECT arrow_src[2][2] = {
		{{224, 64, 16, 32}, {224, 96, 16, 32}}, //left
		{{240, 64, 16, 32}, {240, 96, 16, 32}}, //right
	};
	
	Gfx_BlitTex(&menu.tex_story, &arrow_src[0][(pad_state.held & PAD_LEFT) != 0], x - 40 - 16, y - 16);
	Gfx_BlitTex(&menu.tex_story, &arrow_src[1][(pad_state.held & PAD_RIGHT) != 0], x + 40, y - 16);
	
	//Draw difficulty
	static const RECT diff_srcs[] = {
		{  0, 96, 64, 18},
		{ 95, 138, 117, 19},
		{144, 96, 64, 18},
	};
	
	const RECT *diff_src = &diff_srcs[menu.page_param.stage.diff];
	Gfx_BlitTex(&menu.tex_story, diff_src, x - (diff_src->w >> 1), y - 9 + ((pad_state.press & (PAD_LEFT | PAD_RIGHT)) != 0));
}

static void Menu_DrawWeek(const char *week, s32 x, s32 y)
{
	//Draw label
	if (week == NULL)
	{
		//Tutorial
		RECT label_src = {0, 0, 112, 32};
		Gfx_BlitTex(&menu.tex_story, &label_src, x, y);
	}
	else
	{
		//Week
		RECT label_src = {0, 32, 80, 32};
		Gfx_BlitTex(&menu.tex_story, &label_src, x, y);
		
		//Number
		x += 80;
		for (; *week != '\0'; week++)
		{
			//Draw number
			u8 i = *week - '0';
			
			RECT num_src = {128 + ((i & 3) << 5), ((i >> 2) << 5), 32, 32};
			Gfx_BlitTex(&menu.tex_story, &num_src, x, y);
			x += 32;
		}
	}
}

//Menu functions
void Menu_Load(MenuPage page)
{
	//Load menu assets
	IO_Data menu_arc = IO_Read("\\MENU\\MENU.ARC;1");
	Gfx_LoadTex(&menu.tex_back, Archive_Find(menu_arc, "back.tim"), 0);
	Gfx_LoadTex(&menu.tex_ng, Archive_Find(menu_arc, "ng.tim"), 0);
	Gfx_LoadTex(&menu.tex_credits0, Archive_Find(menu_arc, "credits0.tim"), 0);
	Gfx_LoadTex(&menu.tex_credits1, Archive_Find(menu_arc, "credits1.tim"), 0);
	Gfx_LoadTex(&menu.tex_story, Archive_Find(menu_arc, "story.tim"), 0);
	Gfx_LoadTex(&menu.tex_title, Archive_Find(menu_arc, "title.tim"), 0);
	Gfx_LoadTex(&menu.tex_menu0, Archive_Find(menu_arc, "menu0.tim"), 0);
	Gfx_LoadTex(&menu.tex_menu1, Archive_Find(menu_arc, "menu1.tim"), 0);
	Gfx_LoadTex(&menu.tex_credits, Archive_Find(menu_arc, "credits.tim"), 0);
	Mem_Free(menu_arc);

	FontData_Load(&menu.font_bold, Font_Bold);
	FontData_Load(&menu.font_arial, Font_Arial);

	menu.tricky = Char_MTicky_New(FIXED_DEC(40, 1), FIXED_DEC(80, 1));
	menu.boyfriend = Char_BF_New(FIXED_DEC(-90, 1), FIXED_DEC(65, 1));
	stage.camera.x = stage.camera.y = FIXED_DEC(0, 1);
	stage.camera.bzoom = FIXED_UNIT;
	stage.MTicky_speed = 4;

	//Initialize menu state
	menu.select = menu.next_select = 0;

	switch (menu.page = menu.next_page = page)
	{
	case MenuPage_Opening:
		//Get funny message to use
		//Do this here so timing is less reliant on VSync
#ifdef PSXF_PC
		menu.page_state.opening.funny_message = time(NULL) % COUNT_OF(funny_messages);
#else
		menu.page_state.opening.funny_message = ((*((volatile u32*)0xBF801120)) >> 3) % COUNT_OF(funny_messages); //sysclk seeding
#endif
		break;
	default:
		break;
	}
	menu.page_swap = true;

	menu.trans_time = 0;
	Trans_Clear();

	stage.song_step = 0;

	//Play menu music
	if (menu.swapmusic == false)
	Audio_PlayXA_Track(XA_GettinFreaky, 0x40, 0, true);
	else
	Audio_PlayXA_Track(XA_Nexus, 0x80, 2, true);
	
	Audio_WaitPlayXA();

	//Set background colour
		Gfx_SetClear(0, 0, 0);
}

void Menu_Unload(void)
{
	//Free title Girlfriend
	Character_Free(menu.tricky);
	Character_Free(menu.boyfriend);
}

void Menu_ToStage(StageId id, StageDiff diff, boolean story)
{
	menu.next_page = MenuPage_Stage;
	menu.page_param.stage.id = id;
	menu.page_param.stage.story = story;
	menu.page_param.stage.diff = diff;
	Trans_Start();
}

void Menu_Tick(void)
{
	//Clear per-frame flags
	stage.flag &= ~STAGE_FLAG_JUST_STEP;
	
	//Get song position
	u16 next_step = Audio_TellXA_Milli() / 108; //100 BPM
	if (next_step != stage.song_step)
	{
		if (next_step >= stage.song_step)
			stage.flag |= STAGE_FLAG_JUST_STEP;
		stage.song_step = next_step;
	}
	
	//Handle transition out
	if (Trans_Tick())
	{
		//Change to set next page
		menu.page_swap = true;
		menu.page = menu.next_page;
		menu.select = menu.next_select;
	}
	
	//Tick menu page
	MenuPage exec_page;
	switch (exec_page = menu.page)
	{
		case MenuPage_Opening:
		{
			u16 beat = stage.song_step >> 2;
			
			//Start title screen if opening ended
			if (beat >= 32)
			{
				menu.page = menu.next_page = MenuPage_Title;
				menu.page_swap = true;
				//Fallthrough
			}
			else
			{
				//Start title screen if start pressed
				if (pad_state.held & PAD_START)
					menu.page = menu.next_page = MenuPage_Title;
				
				//Draw different text depending on beat
				RECT src_ng = {0, 0, 128, 128};
				RECT src_credits0 = { 0, 0, 255, 107 };
				RECT src_credits1 = { 0, 1, 255, 119 };
				RECT src_credits2 = { 0, 118, 102, 102};
				const char **funny_message = funny_messages[menu.page_state.opening.funny_message];
				
				switch (beat)
				{
					case 5:
						Gfx_BlitTex(&menu.tex_credits0, &src_credits0, (SCREEN_WIDTH - 256) >> 1, SCREEN_HEIGHT2 + 0);
					case 4:
						menu.font_bold.draw(&menu.font_bold, "ROSEBUD", SCREEN_WIDTH2, SCREEN_HEIGHT2 - 16, FontAlign_Center);
					case 3:
						menu.font_bold.draw(&menu.font_bold, "CVAL", SCREEN_WIDTH2, SCREEN_HEIGHT2 - 32, FontAlign_Center);
					case 2:
						menu.font_bold.draw(&menu.font_bold, "BANBUDS", SCREEN_WIDTH2, SCREEN_HEIGHT2 - 48, FontAlign_Center);
				//Fallthrough
					case 1:
						menu.font_bold.draw(&menu.font_bold, "KADEDEV",   SCREEN_WIDTH2, SCREEN_HEIGHT2 - 64, FontAlign_Center);
						break;
					
					case 10:
						Gfx_BlitTex(&menu.tex_credits1, &src_credits1, (SCREEN_WIDTH - 256) >> 1, SCREEN_HEIGHT2 - 16);
					case 9:
						menu.font_bold.draw(&menu.font_bold, "JADS", SCREEN_WIDTH2, SCREEN_HEIGHT2 - 32, FontAlign_Center);
					case 8:
						menu.font_bold.draw(&menu.font_bold, "YINGYANG", SCREEN_WIDTH2, SCREEN_HEIGHT2 - 48, FontAlign_Center);
					case 7:
						menu.font_bold.draw(&menu.font_bold, "MORO",    SCREEN_WIDTH2, SCREEN_HEIGHT2 - 64, FontAlign_Center);
				//Fallthrough
					case 6:
						menu.font_bold.draw(&menu.font_bold, "WITH HELP FROM", SCREEN_WIDTH2, SCREEN_HEIGHT2 - 80, FontAlign_Center);
						break;
					
					case 13:
						menu.font_bold.draw(&menu.font_bold, "lets play worms", SCREEN_WIDTH2, SCREEN_HEIGHT2 + 80, FontAlign_Center);
					case 12:
						menu.font_bold.draw(&menu.font_bold, "LORD SCOUT", SCREEN_WIDTH2, SCREEN_HEIGHT2 - 64, FontAlign_Center);
						Gfx_BlitTex(&menu.tex_ng, &src_ng, (SCREEN_WIDTH - 128) >> 1, SCREEN_HEIGHT2 - 42);
					case 11:
						menu.font_bold.draw(&menu.font_bold, "PSX PORT BY", SCREEN_WIDTH2, SCREEN_HEIGHT2 - 80, FontAlign_Center);
						break;

					case 15:
						menu.font_bold.draw(&menu.font_bold, funny_message[1], SCREEN_WIDTH2, SCREEN_HEIGHT2, FontAlign_Center);
				//Fallthrough
					case 14:
						menu.font_bold.draw(&menu.font_bold, funny_message[0], SCREEN_WIDTH2, SCREEN_HEIGHT2 - 16, FontAlign_Center);
						break;

					case 17:
						menu.font_bold.draw(&menu.font_bold, funny_message[3], SCREEN_WIDTH2, SCREEN_HEIGHT2, FontAlign_Center);
						//Fallthrough
					case 16:
						menu.font_bold.draw(&menu.font_bold, funny_message[2], SCREEN_WIDTH2, SCREEN_HEIGHT2 - 16, FontAlign_Center);
						break;

					case 19:
						menu.font_bold.draw(&menu.font_bold, funny_message[5], SCREEN_WIDTH2, SCREEN_HEIGHT2, FontAlign_Center);
						//Fallthrough
					case 18:
						menu.font_bold.draw(&menu.font_bold, funny_message[4], SCREEN_WIDTH2, SCREEN_HEIGHT2 - 16, FontAlign_Center);
						break;

					case 21:
						menu.font_bold.draw(&menu.font_bold, funny_message[7], SCREEN_WIDTH2, SCREEN_HEIGHT2, FontAlign_Center);
						//Fallthrough
					case 20:
						menu.font_bold.draw(&menu.font_bold, funny_message[6], SCREEN_WIDTH2, SCREEN_HEIGHT2 - 16, FontAlign_Center);
						break;

					case 23:
						menu.font_bold.draw(&menu.font_bold, funny_message[9], SCREEN_WIDTH2, SCREEN_HEIGHT2, FontAlign_Center);
						//Fallthrough
					case 22:
						menu.font_bold.draw(&menu.font_bold, funny_message[8], SCREEN_WIDTH2, SCREEN_HEIGHT2 - 16, FontAlign_Center);
						break;

					case 25:
						menu.font_bold.draw(&menu.font_bold, funny_message[11], SCREEN_WIDTH2, SCREEN_HEIGHT2, FontAlign_Center);
						//Fallthrough
					case 24:
						menu.font_bold.draw(&menu.font_bold, funny_message[10], SCREEN_WIDTH2, SCREEN_HEIGHT2 - 16, FontAlign_Center);
						break;

					case 29:
					case 28:
						menu.font_bold.draw(&menu.font_bold, "ADDITIONAL PORT HELP", SCREEN_WIDTH2, SCREEN_HEIGHT2 + 80, FontAlign_Center);
						menu.font_bold.draw(&menu.font_bold, "IGORSOU", SCREEN_WIDTH2, SCREEN_HEIGHT2 + 64, FontAlign_Center);
						Gfx_BlitTex(&menu.tex_credits1, &src_credits2, SCREEN_WIDTH2 - 48, SCREEN_HEIGHT2 - 40);
					case 27:
						menu.font_bold.draw(&menu.font_bold, "BY TSURARAN", SCREEN_WIDTH2, SCREEN_HEIGHT2 - 64, FontAlign_Center);
					case 26:
						menu.font_bold.draw(&menu.font_bold, "CHICKEN DANCE REMIX", SCREEN_WIDTH2, SCREEN_HEIGHT2 - 80, FontAlign_Center);
						break;

					case 31:
						menu.font_bold.draw(&menu.font_bold, "OR SMTH LOL", SCREEN_WIDTH2, SCREEN_HEIGHT2 - 32, FontAlign_Center);
					case 30:
						menu.font_bold.draw(&menu.font_bold, "THE DROP", SCREEN_WIDTH2, SCREEN_HEIGHT2 - 48, FontAlign_Center);
						break;
				}
				break;
			}
		}
	//Fallthrough
		case MenuPage_Title:
		{
			 menu.swapmusic = false;
			//Initialize page
			if (menu.page_swap)
			{
				menu.page_state.title.logo_bump = (FIXED_DEC(7,1) / 24) - 1;
				menu.page_state.title.fade = FIXED_DEC(255,1);
				menu.page_state.title.fadespd = FIXED_DEC(90,1);

			}
			
			//Draw white fade
			if (menu.page_state.title.fade > 0)
			{
				static const RECT flash = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
				u8 flash_col = menu.page_state.title.fade >> FIXED_SHIFT;
				Gfx_BlendRect(&flash, flash_col, flash_col, flash_col, 1);
				menu.page_state.title.fade -= FIXED_MUL(menu.page_state.title.fadespd, timer_dt);
			}
			
			//Go to main menu when start is pressed
			if (menu.trans_time > 0 && (menu.trans_time -= timer_dt) <= 0)
				Trans_Start();

			
			if ((pad_state.press & PAD_START) && menu.next_page == menu.page && Trans_Idle())
			{
				menu.trans_time = FIXED_UNIT;
				menu.page_state.title.fade = FIXED_DEC(255,1);
				menu.page_state.title.fadespd = FIXED_DEC(300,1);
				menu.next_page = MenuPage_Main;
				menu.next_select = 0;
			}
			
			//Draw Friday Night Funkin' logo
			if ((stage.flag & STAGE_FLAG_JUST_STEP) && (stage.song_step & 0x3) == 0 && menu.page_state.title.logo_bump == 0)
				menu.page_state.title.logo_bump = (FIXED_DEC(7,1) / 24) - 1;
			
			static const fixed_t logo_scales[] = {
				FIXED_DEC(1,1),
				FIXED_DEC(101,100),
				FIXED_DEC(102,100),
				FIXED_DEC(103,100),
				FIXED_DEC(105,100),
				FIXED_DEC(110,100),
				FIXED_DEC(97,100),
			};
			fixed_t logo_scale = logo_scales[(menu.page_state.title.logo_bump * 24) >> FIXED_SHIFT];
			u32 x_rad = (logo_scale * (200 >> 1)) >> FIXED_SHIFT;
			u32 y_rad = (logo_scale * (120 >> 1)) >> FIXED_SHIFT;
			
			RECT logo_src = {0, 0, 246, 160};
			RECT logo_dst = {
				100 - x_rad + (SCREEN_WIDEADD2 >> 1),
				68 - y_rad,
				x_rad << 1,
				y_rad << 1
			};
			Gfx_DrawTex(&menu.tex_title, &logo_src, &logo_dst);
			
			if (menu.page_state.title.logo_bump > 0)
				if ((menu.page_state.title.logo_bump -= timer_dt) < 0)
					menu.page_state.title.logo_bump = 0;
			
			//Draw "Press Start to Begin"
			if (menu.next_page == menu.page)
			{
				//Blinking blue
				s16 press_lerp = (MUtil_Cos(animf_count << 3) + 0x100) >> 1;
				u8 press_r = 51 >> 1;
				u8 press_g = (58  + ((press_lerp * (255 - 58))  >> 8)) >> 1;
				u8 press_b = (206 + ((press_lerp * (255 - 206)) >> 8)) >> 1;
				
				RECT press_src = {0, 203, 207, 18};
				Gfx_BlitTexCol(&menu.tex_title, &press_src, 50, SCREEN_HEIGHT - 35, press_r, press_g, press_b);
			}
			else
			{
				//Flash white
				RECT press_src = {0, (animf_count & 1) ? 203 : 221, 207, 18};
				Gfx_BlitTex(&menu.tex_title, &press_src, 50, SCREEN_HEIGHT - 35);
			}
			
			//Draw Girlfriend
			menu.tricky->tick(menu.tricky);

			//background color
			Gfx_SetClear(75, 0, 0);
			break;
		}
		case MenuPage_Main:
		{
			static const char* menu_options[] = {
				"CLOWN",
				"FREEPLAY",
				"CREDITS",
				"OPTIONS",
				#ifdef PSXF_NETWORK
					"JOIN SERVER",
					"HOST SERVER",
				#endif
			};

			u16 next_step = Audio_TellXA_Milli() / 91; //100 BPM
			if (next_step != stage.song_step)
			{
				if (next_step >= stage.song_step)
					stage.flag |= STAGE_FLAG_JUST_STEP;
				stage.song_step = next_step;
			}

			//Initialize page
			if (menu.page_swap && menu.swapmusic == false)
			{
			Audio_StopXA();
			Audio_PlayXA_Track(XA_Nexus, 0x80, 2, true);
			menu.swapmusic = true;
		}

			//Handle option and selection
			if (menu.trans_time > 0 && (menu.trans_time -= timer_dt) <= 0)
				Trans_Start();
			
			if (menu.next_page == menu.page && Trans_Idle())
			{
				//Change option
				if (pad_state.press & PAD_DOWN)
				{
					if (menu.select > 0)
						menu.select--;
					else
						menu.select = COUNT_OF(menu_options) - 1;
				}
				if (pad_state.press & PAD_UP)
				{
					if (menu.select < COUNT_OF(menu_options) - 1)
						menu.select++;
					else
						menu.select = 0;
				}
				
				//Select option if cross is pressed
				if (pad_state.press & (PAD_START | PAD_CROSS))
				{
					switch (menu.select)
					{
						case 0: //Story Mode
							if (pad_state.press & (PAD_START | PAD_CROSS))
							{
								menu.next_page = MenuPage_Stage;
								menu.page_param.stage.id = StageId_2_1;
								menu.page_param.stage.story = true;
								menu.trans_time = FIXED_UNIT;
								menu.page_state.title.fade = FIXED_DEC(255, 10);
								menu.page_state.title.fadespd = FIXED_DEC(510, 1);
							};
							break;
						case 1: //Freeplay
							menu.next_page = MenuPage_Freeplay;
							break;
						case 2: //Mods
							menu.next_page = MenuPage_Mods;
							break;
						case 3: //Options
							menu.next_page = MenuPage_Options;
							break;
					#ifdef PSXF_NETWORK
						case 4: //Join Server
							menu.next_page = Network_Inited() ? MenuPage_NetJoin : MenuPage_NetInitFail;
							break;
						case 5: //Host Server
							menu.next_page = Network_Inited() ? MenuPage_NetHost : MenuPage_NetInitFail;
							break;
					#endif
					}
					menu.next_select = 0;
					menu.trans_time = FIXED_UNIT;
				}
				
				//Return to title screen if circle is pressed
				if (pad_state.press & PAD_CIRCLE)
				{
					menu.next_page = MenuPage_Title;
					Audio_StopXA();
					Audio_PlayXA_Track(XA_GettinFreaky, 0x40, 0, false);
					Trans_Start();
				}
			}

			//draw "mad"
			RECT madness0_src = {0, 0, 138, 33};
			Gfx_BlitTex(&menu.tex_menu0, &madness0_src, 10, 15);

			//draw "ness"
			RECT madness1_src = {0, 36, 103, 33};
			Gfx_BlitTex(&menu.tex_menu0, &madness1_src, madness0_src.w + 11, 15);

			//draw "difficulty"
			RECT difficulty_src = { 1, 25, 85, 20};
			Gfx_BlitTex(&menu.tex_menu1, &difficulty_src, 170, 94);

			//draw CuckyDev Credits
			RECT ckdev_src = { 102, 153, 154, 48 };
			Gfx_BlitTex(&menu.tex_credits1, &ckdev_src, 80, 180);

			//draw "difficulty lines"
			RECT lines_src = { 104, 41, 145, 10 };
			Gfx_BlitTex(&menu.tex_menu1, &lines_src, 140, 114);

				//draw normal square
				RECT squaren_src = {0, 84, 128, 52};
				RECT squaren_dst = {0,166, SCREEN_WIDTH, 74};
				Gfx_DrawTex(&menu.tex_menu0, &squaren_src, &squaren_dst);

				//invert square
				squaren_dst.y = squaren_dst.h;
				squaren_dst.h = -squaren_dst.h;
				Gfx_DrawTex(&menu.tex_menu0, &squaren_src, &squaren_dst);

				Menu_DifficultySelectorMainMenu(135, 136);

				//tabs for main menu
				s8 static tab0;

				//draw "clown"
				RECT clown_src = {63, 7, 53, 11};
				RECT clown_dst = {165, 52 - tab0 + 10, 53, 11};

				Gfx_DrawTex(&menu.tex_menu1, &clown_src, &clown_dst);

				Menu_TabRed(&tab0,menu.select == 0, true, 157,  80);

				//invert tabs
				s8 static tab1;

				//draw "freeplay"
				RECT freeplay_src =  { 3, 7, 55, 10};
				RECT freeplay_dst = {242, 52 - tab1 + 10, 53, 11};

				Gfx_DrawTex(&menu.tex_menu1, &freeplay_src, &freeplay_dst);

				Menu_TabRed(&tab1,menu.select == 1, true, 234,  80);

				s8 static tab2;

				//draw "credits"
				RECT credits_src =  {64,  78, 44, 12};
				RECT credits_dst = {155, 165 - tab2 + 1, 58, 10};

				Gfx_DrawTex(&menu.tex_menu1, &credits_src, &credits_dst);

				Menu_Tab(&tab2,menu.select == 2, false, 149, 155);

				s8 static tab3;

				//draw "gameoptions"
				RECT options_src =  { 1, 80, 58, 10};
				RECT options_dst = {232, 165 - tab3 + 1, 58, 10};

				Gfx_DrawTex(&menu.tex_menu1, &options_src, &options_dst);

				Menu_Tab(&tab3,menu.select == 3, false, 226, 155);

				menu.boyfriend->tick(menu.boyfriend);



			//Draw options
			s32 next_scroll = menu.select *
			#ifndef PSXF_NETWORK
				FIXED_DEC(8,1);
			#else
				FIXED_DEC(12,1);
			#endif
			menu.scroll += (next_scroll - menu.scroll) >> 2;

			//draw black square
			RECT squareb_src = {0,137, 255,120};
			RECT squareb_dst = {-10, 50, 400, 120};
			Gfx_DrawTex(&menu.tex_menu0, &squareb_src, &squareb_dst);
			break;
		}
		case MenuPage_Story:
		{
			static const struct
			{
				const char *week;
				StageId stage;
				const char *name;
				const char *tracks[3];
			} menu_options[] = {
				{"1", StageId_2_1, "THE CLOWN", {"IMPROBABLE", "MADNESS", "HELLCLOWN"}},
				{NULL, StageId_1_4, "EXPURGATION", {"EXPURGATION", NULL, NULL}},
			};
			
			//Initialize page
			if (menu.page_swap)
			{
				menu.scroll = 0;
				menu.page_param.stage.diff = StageDiff_Normal;
				menu.page_state.title.fade = FIXED_DEC(0,1);
				menu.page_state.title.fadespd = FIXED_DEC(0,1);
			}
			
			//Draw white fade
			if (menu.page_state.title.fade > 0)
			{
				static const RECT flash = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
				u8 flash_col = menu.page_state.title.fade >> FIXED_SHIFT;
				Gfx_BlendRect(&flash, flash_col, flash_col, flash_col, 1);
				menu.page_state.title.fade -= FIXED_MUL(menu.page_state.title.fadespd, timer_dt);
			}
			
			//Draw difficulty selector
			Menu_DifficultySelector(SCREEN_WIDTH - 75, 80);
			
			//Handle option and selection
			if (menu.trans_time > 0 && (menu.trans_time -= timer_dt) <= 0)
				Trans_Start();
			
			if (menu.next_page == menu.page && Trans_Idle())
			{
				//Change option
				if (pad_state.press & PAD_UP)
				{
					if (menu.select > 0)
						menu.select--;
					else
						menu.select = COUNT_OF(menu_options) - 1;
				}
				if (pad_state.press & PAD_DOWN)
				{
					if (menu.select < COUNT_OF(menu_options) - 1)
						menu.select++;
					else
						menu.select = 0;
				}
				
				//Select option if cross is pressed
				if (pad_state.press & (PAD_START | PAD_CROSS))
				{
					menu.next_page = MenuPage_Stage;
					menu.page_param.stage.id = menu_options[menu.select].stage;
					menu.page_param.stage.story = true;
					menu.trans_time = FIXED_UNIT;
					menu.page_state.title.fade = FIXED_DEC(255,1);
					menu.page_state.title.fadespd = FIXED_DEC(510,1);
				}
				
				//Return to main menu if circle is pressed
				if (pad_state.press & PAD_CIRCLE)
				{
					menu.next_page = MenuPage_Main;
					menu.next_select = 0; //Story Mode
					Trans_Start();
				}
			}
			
			//Draw week name and tracks
			menu.font_bold.draw(&menu.font_bold,
				menu_options[menu.select].name,
				SCREEN_WIDTH - 16,
				24,
				FontAlign_Right
			);
			
			const char * const *trackp = menu_options[menu.select].tracks;
			for (size_t i = 0; i < COUNT_OF(menu_options[menu.select].tracks); i++, trackp++)
			{
				if (*trackp != NULL)
					menu.font_bold.draw(&menu.font_bold,
						*trackp,
						SCREEN_WIDTH - 16,
						SCREEN_HEIGHT - (4 * 24) + (i * 24),
						FontAlign_Right
					);
			}
			
			//Draw upper strip
			RECT name_bar = {0, 16, SCREEN_WIDTH, 32};
			Gfx_DrawRect(&name_bar, 249, 207, 81);
			
			//Draw options
			s32 next_scroll = menu.select * FIXED_DEC(48,1);
			menu.scroll += (next_scroll - menu.scroll) >> 3;
			
			if (menu.next_page == menu.page || menu.next_page == MenuPage_Main)
			{
				//Draw all options
				for (u8 i = 0; i < COUNT_OF(menu_options); i++)
				{
					s32 y = 64 + (i * 48) - (menu.scroll >> FIXED_SHIFT);
					if (y <= 16)
						continue;
					if (y >= SCREEN_HEIGHT)
						break;
					Menu_DrawWeek(menu_options[i].week, 48, y);
				}
			}
			else if (animf_count & 2)
			{
				//Draw selected option
				Menu_DrawWeek(menu_options[menu.select].week, 48, 64 + (menu.select * 48) - (menu.scroll >> FIXED_SHIFT));
			}
			
			break;
		}
		case MenuPage_Freeplay:
		{
			static const struct
			{
				StageId stage;
				const char *text;
				boolean difficulty;
			} menu_options[] = {
				//{StageId_4_4, "TEST"},
				{StageId_1_1, "IMPROBABLE OUTSET", true},
				{StageId_1_2, "MADNESS", true},
				{StageId_1_3, "HELLCLOWN", true},
				{StageId_1_4, "EXPURGATION", false},
			};
			
			//Initialize page
			if (menu.page_swap)
			{
				menu.scroll = COUNT_OF(menu_options) * FIXED_DEC(SCREEN_HEIGHT2,1);
				menu.page_param.stage.diff = StageDiff_Hard;
				menu.page_state.freeplay.back_r = FIXED_DEC(255, 1);
				menu.page_state.freeplay.back_g = FIXED_DEC(255, 1);
				menu.page_state.freeplay.back_b = FIXED_DEC(255, 1);
			}

			
			//Draw difficulty selector
			if (menu_options[menu.select].difficulty)
				Menu_DifficultySelector(SCREEN_WIDTH - 272, SCREEN_HEIGHT2 + 98);
			
			
			//Handle option and selection
			if (menu.next_page == menu.page && Trans_Idle())
			{
				//Change option
				if (pad_state.press & PAD_UP)
				{
					if (menu.select > 0)
						menu.select--;
					else
						menu.select = COUNT_OF(menu_options) - 1;
				}
				if (pad_state.press & PAD_DOWN)
				{
					if (menu.select < COUNT_OF(menu_options) - 1)
						menu.select++;
					else
						menu.select = 0;
				}
				
				//Select option if cross is pressed
				if (pad_state.press & (PAD_START | PAD_CROSS))
				{
					menu.next_page = MenuPage_Stage;
					menu.page_param.stage.id = menu_options[menu.select].stage;
					menu.page_param.stage.story = false;
					Trans_Start();
				}
				
				//Return to main menu if circle is pressed
				if (pad_state.press & PAD_CIRCLE)
				{
					menu.next_page = MenuPage_Main;
					menu.next_select = 1; //Freeplay
					Trans_Start();
				}
			}
			
			RECT Improbable_src = { 0, 0, 207, 16 };
			RECT Improbable_dst = { 30, 50, 207, 16 };
			RECT Improbableselect_src = { 0, 116, 207, 16 };
			RECT Improbableselect_dst = { 33, 50, 207, 16 };

			if (menu.select == 0)
				Gfx_DrawTex(&menu.tex_story, &Improbableselect_src, &Improbableselect_dst);
			else
			Gfx_DrawTex(&menu.tex_story, &Improbable_src, &Improbable_dst);

			RECT madness_src = { 0, 18, 93, 16 };
			RECT madness_dst = { 30, 85, 93, 16 };
			RECT madnessselect_src = { 0, 133, 93, 16 };
			RECT madnessselect_dst = { 33, 85, 93, 16 };

			if (menu.select == 1)
				Gfx_DrawTex(&menu.tex_story, &madnessselect_src, &madnessselect_dst);
			else
				Gfx_DrawTex(&menu.tex_story, &madness_src, &madness_dst);

			RECT hellclown_src = { 0, 36, 118, 16 };
			RECT hellclown_dst = { 30, 120, 118, 16 };
			RECT hellclownselect_src = { 97, 18, 118, 16 };
			RECT hellclownselect_dst = { 33, 120, 118, 16 };

			if (menu.select == 2)
				Gfx_DrawTex(&menu.tex_story, &hellclownselect_src, &hellclownselect_dst);
			else
				Gfx_DrawTex(&menu.tex_story, &hellclown_src, &hellclown_dst);

			RECT expurgation_src = { 0, 55, 135, 16 };
			RECT expurgation_dst = { 90, 160, 135, 16 };
			RECT expurgationselect_src = { 90, 76, 135, 16 };
			RECT expurgationselect_dst = { 89, 159, 137, 17 };

			if (menu.select == 3)
				Gfx_DrawTex(&menu.tex_story, &expurgationselect_src, &expurgationselect_dst);
			else
				Gfx_DrawTex(&menu.tex_story, &expurgation_src, &expurgation_dst);

			RECT unfair_src = { 0, 74, 87, 21 };

			if (menu.select == 3)
				Gfx_BlitTex(&menu.tex_story, &unfair_src, 22, 208);


			//Draw background
			Menu_DrawBack(
				true,
				8,
				menu.page_state.freeplay.back_r >> (FIXED_SHIFT + 1),
				menu.page_state.freeplay.back_g >> (FIXED_SHIFT + 1),
				menu.page_state.freeplay.back_b >> (FIXED_SHIFT + 1),
				0, 0, 0
			);
			break;
		}
		case MenuPage_Mods:
		{
			static const struct
			{
				StageId stage;
				const char *text;
				boolean difficulty;
			} menu_options[] = {
				{StageId_Kapi_1, "LORD SCOUT", false},
				{StageId_Kapi_2, "  TRICKY PSX PORT", false},
				{StageId_Clwn_1, "CUCKYDEV", false},
				{StageId_Clwn_2, "	PSXFUNKIN", false},
				{StageId_Clwn_2, "BANBUDS", false},
				{StageId_Clwn_1, "	DIRECTOR AND ARTIST", false},
				{StageId_Clwn_1, "ROZEBUD", false},
				{StageId_Clwn_1, "	MUSICIAN AND CODER", false},
				{StageId_Clwn_1, "KADEDEV", false},
				{StageId_Clwn_1, "   PROGRAMMING", false},
				{StageId_Clwn_2, "CVAL", false},
				{StageId_Clwn_1, "	CHARTING", false},
				{StageId_Clwn_1, "YINGYANG48", false},
				{StageId_Clwn_1, "	COMPOSER", false},
				{StageId_Clwn_1, "JADS", false},
				{StageId_Clwn_1, "   COMPOSER", false},
				{StageId_Clwn_1, "MORO", false},
				{StageId_Clwn_1, "	ARTIST", false},
				{StageId_Clwn_1, "TSURARAN", false},
				{StageId_Clwn_1, "   MENU MUSIC", false},
				{StageId_Clwn_1, "ADDITIONAL PORT HELP", false},
				{StageId_Kapi_1, "	IGORSOUTHREETHOUSAND", false},
				{StageId_Kapi_1, "	UNSTOPFOURBLE", false},
				{StageId_Clwn_1, "expurgation specifically", false},
				{StageId_Kapi_1, "	MUGENLUCKY", false},
				{StageId_Clwn_1, "SPECIAL THANKS", false},
				{StageId_Kapi_2, "	KRINKLES", false},
				{StageId_Clwn_1, "  TOM FULP", false},
			};
			
			//Initialize page
			if (menu.page_swap)
			{
				menu.scroll = COUNT_OF(menu_options) * FIXED_DEC(24 + SCREEN_HEIGHT2,1);
				menu.page_param.stage.diff = StageDiff_Normal;
			}
			
			//Draw difficulty selector
			if (menu_options[menu.select].difficulty)
				Menu_DifficultySelector(SCREEN_WIDTH - 100, SCREEN_HEIGHT2 - 48);
			
			//Handle option and selection
			if (menu.next_page == menu.page && Trans_Idle())
			{
				//Change option
				if (pad_state.press & PAD_UP)
				{
					if (menu.select > 0)
						menu.select--;
					else
						menu.select = COUNT_OF(menu_options) - 1;
				}
				if (pad_state.press & PAD_DOWN)
				{
					if (menu.select < COUNT_OF(menu_options) - 1)
						menu.select++;
					else
						menu.select = 0;
				}
				
				//Return to main menu if circle is pressed
				if (pad_state.press & PAD_CIRCLE)
				{
					menu.next_page = MenuPage_Main;
					menu.next_select = 2; //Mods
					Trans_Start();
				}
			}
			
			//Draw options
			s32 next_scroll = menu.select * FIXED_DEC(24,1);
			menu.scroll += (next_scroll - menu.scroll) >> 4;
			
			for (u8 i = 0; i < COUNT_OF(menu_options); i++)
			{
				//Get position on screen
				s32 y = (i * 24) - 8; //- (menu.scroll >> FIXED_SHIFT);
				if (y <= -SCREEN_HEIGHT2 - 8)
					continue;
				if (y >= SCREEN_HEIGHT2 + 8)
					break;
			}
			
			RECT credits_src = { 0, 0, 256, 256 };
			RECT credits_dst = { 30, -20, 256, 256};

			Gfx_DrawTex(&menu.tex_credits, &credits_src, &credits_dst);

			//Draw background

			RECT creditss_src = { 0, 0, 4, 256 };
			RECT creditss_dst = { -20, -15, 128, 256 };
			
			Gfx_DrawTexCol(&menu.tex_credits, &creditss_src, &creditss_dst, 0, 0, 0);

			RECT credits2s_src = { 0, 0, 4, 256 };
			RECT credits2s_dst = { 100, -15, 512, 256 };
			
			Gfx_DrawTexCol(&menu.tex_credits, &credits2s_src, &credits2s_dst, 0, 0, 0);

			break;
		}
		case MenuPage_Options:
		{
			static const char *gamemode_strs[] = {"NORMAL", "SWAP", "TWO PLAYER"};
			static const struct
			{
				enum
				{
					OptType_Boolean,
					OptType_Enum,
				} type;
				const char *text;
				void *value;
				union
				{
					struct
					{
						int a;
					} spec_boolean;
					struct
					{
						s32 max;
						const char **strs;
					} spec_enum;
				} spec;
			} menu_options[] = {
				{OptType_Enum,    "GAMEMODE", &stage.mode, {.spec_enum = {COUNT_OF(gamemode_strs), gamemode_strs}}},
				//{OptType_Boolean, "INTERPOLATION", &stage.expsync},
				{OptType_Boolean, "GHOST TAP ", &stage.ghost, {.spec_boolean = {0}}},
				{OptType_Boolean, "DOWNSCROLL", &stage.downscroll, {.spec_boolean = {0}}},
			};
			
			//Initialize page
			if (menu.page_swap)
				menu.scroll = COUNT_OF(menu_options) * FIXED_DEC(24 + SCREEN_HEIGHT2,1);
			
			//Handle option and selection
			if (menu.next_page == menu.page && Trans_Idle())
			{
				//Change option
				if (pad_state.press & PAD_UP)
				{
					if (menu.select > 0)
						menu.select--;
					else
						menu.select = COUNT_OF(menu_options) - 1;
				}
				if (pad_state.press & PAD_DOWN)
				{
					if (menu.select < COUNT_OF(menu_options) - 1)
						menu.select++;
					else
						menu.select = 0;
				}
				
				//Handle option changing
				switch (menu_options[menu.select].type)
				{
					case OptType_Boolean:
						if (pad_state.press & (PAD_CROSS | PAD_LEFT | PAD_RIGHT))
							*((boolean*)menu_options[menu.select].value) ^= 1;
						break;
					case OptType_Enum:
						if (pad_state.press & PAD_LEFT)
							if (--*((s32*)menu_options[menu.select].value) < 0)
								*((s32*)menu_options[menu.select].value) = menu_options[menu.select].spec.spec_enum.max - 1;
						if (pad_state.press & PAD_RIGHT)
							if (++*((s32*)menu_options[menu.select].value) >= menu_options[menu.select].spec.spec_enum.max)
								*((s32*)menu_options[menu.select].value) = 0;
						break;
				}
				
				//Return to main menu if circle is pressed
				if (pad_state.press & PAD_CIRCLE)
				{
					menu.next_page = MenuPage_Main;
					menu.next_select = 3; //Options
					Trans_Start();
				}
			}
			
			//Draw options
			s32 next_scroll = menu.select * FIXED_DEC(24,1);
			menu.scroll += (next_scroll - menu.scroll) >> 4;
			
			for (u8 i = 0; i < COUNT_OF(menu_options); i++)
			{
				//Get position on screen
				s32 y = (i * 24) - 8;// -(menu.scroll >> FIXED_SHIFT);
				if (y <= -SCREEN_HEIGHT2 - 8)
					continue;
				if (y >= SCREEN_HEIGHT2 + 8)
					break;
				
				//Draw text
				char text[0x80];
				switch (menu_options[i].type)
				{
					case OptType_Boolean:
						sprintf(text, "%s %s", menu_options[i].text, *((boolean*)menu_options[i].value) ? "ON" : "OFF");
						break;
					case OptType_Enum:
						sprintf(text, "%s %s", menu_options[i].text, menu_options[i].spec.spec_enum.strs[*((s32*)menu_options[i].value)]);
						break;
				}
				menu.font_arial.draw_col(&menu.font_arial,
					Menu_LowerIf(text, menu.select != i),
					48 + (y >> 2),
					SCREEN_HEIGHT2 + y - 55,
					FontAlign_Left, 255, 0, 0
				);
			}

			//Draw background
			Menu_DrawBack(
				true,
				8,
				100, 100, 100,
				0, 0, 0
			);
			break;
		}
	#ifdef PSXF_NETWORK
		case MenuPage_NetHost:
		{
			const size_t menu_options = 3;
			
			//Initialize page
			if (menu.page_swap)
			{
				menu.page_state.net_host.type = false;
				menu.page_state.net_host.port[0] = '\0';
				menu.page_state.net_host.pass[0] = '\0';
			}
			
			//Handle option and selection
			if (menu.next_page == menu.page && Trans_Idle())
			{
				if (!menu.page_state.net_host.type)
				{
					//Change option
					if (pad_state.press & PAD_UP)
					{
						if (menu.select > 0)
							menu.select--;
						else
							menu.select = menu_options - 1;
					}
					if (pad_state.press & PAD_DOWN)
					{
						if (menu.select < menu_options - 1)
							menu.select++;
						else
							menu.select = 0;
					}
					
					//Handle selection when cross is pressed
					if (pad_state.press & (PAD_START | PAD_CROSS))
					{
						switch (menu.select)
						{
							case 0: //Port
							case 1: //Pass
								menu.page_state.net_host.type = true;
								break;
							case 2: //Host
								if (!Network_HostPort(menu.page_state.net_host.port, menu.page_state.net_host.pass))
								{
									menu.next_page = MenuPage_NetOpWait;
									menu.next_select = 0;
									Trans_Start();
								}
								break;
						}
					}
					
					//Return to main menu if circle is pressed
					if (pad_state.press & PAD_CIRCLE)
					{
						menu.next_page = MenuPage_Main;
						menu.next_select = 5; //Host Server
						Trans_Start();
					}
				}
				else
				{
					//Stop typing when start is pressed
					if (pad_state.press & PAD_START)
					{
						switch (menu.select)
						{
							case 0: //Port
							case 1: //Pass
								menu.page_state.net_host.type = false;
								break;
						}
					}
				}
			}
			
			//Draw page label
			menu.font_bold.draw(&menu.font_bold,
				"HOST SERVER",
				16,
				SCREEN_HEIGHT - 32,
				FontAlign_Left
			);
			
			//Draw options
			MenuStr_Process(&menu.page_state.net_host.port, 64 + 3 * 0, 64 + 16 * 0, "Port: %s", menu.select == 0, menu.page_state.net_host.type);
			MenuStr_Process(&menu.page_state.net_host.pass, 64 + 3 * 1, 64 + 16 * 1, "Pass: %s", menu.select == 1, menu.page_state.net_host.type);
			menu.font_bold.draw(&menu.font_bold, Menu_LowerIf("HOST", menu.select != 2), 64 + 3 * 2, 64 + 16 * 2, FontAlign_Left);
			
			//Draw background
			Menu_DrawBack(
				true,
				8,
				146 >> 1, 113 >> 1, 253 >> 1,
				0, 0, 0
			);
			break;
		}
		case MenuPage_NetJoin:
		{
			const size_t menu_options = 4;
			
			//Initialize page
			if (menu.page_swap)
			{
				menu.page_state.net_join.type = false;
				menu.page_state.net_join.ip[0] = '\0';
				menu.page_state.net_join.port[0] = '\0';
				menu.page_state.net_join.pass[0] = '\0';
			}
			
			//Handle option and selection
			if (menu.next_page == menu.page && Trans_Idle())
			{
				if (!menu.page_state.net_join.type)
				{
					//Change option
					if (pad_state.press & PAD_UP)
					{
						if (menu.select > 0)
							menu.select--;
						else
							menu.select = menu_options - 1;
					}
					if (pad_state.press & PAD_DOWN)
					{
						if (menu.select < menu_options - 1)
							menu.select++;
						else
							menu.select = 0;
					}
					
					//Handle selection when cross is pressed
					if (pad_state.press & (PAD_START | PAD_CROSS))
					{
						switch (menu.select)
						{
							case 0: //Ip
							case 1: //Port
							case 2: //Pass
								menu.page_state.net_join.type = true;
								break;
							case 3: //Join
								if (!Network_Join(menu.page_state.net_join.ip, menu.page_state.net_join.port, menu.page_state.net_join.pass))
								{
									menu.next_page = MenuPage_NetConnect;
									menu.next_select = 0;
									Trans_Start();
								}
								break;
						}
					}
					
					//Return to main menu if circle is pressed
					if (pad_state.press & PAD_CIRCLE)
					{
						menu.next_page = MenuPage_Main;
						menu.next_select = 4; //Join Server
						Trans_Start();
					}
				}
				else
				{
					//Stop typing when start is pressed
					if (pad_state.press & PAD_START)
					{
						switch (menu.select)
						{
							case 0: //Join
							case 1: //Port
							case 2: //Pass
								menu.page_state.net_join.type = false;
								break;
						}
					}
				}
			}
			
			//Draw page label
			menu.font_bold.draw(&menu.font_bold,
				"JOIN SERVER",
				16,
				SCREEN_HEIGHT - 32,
				FontAlign_Left
			);
			
			//Draw options
			MenuStr_Process(&menu.page_state.net_join.ip, 64 + 3 * 0, 64 + 16 * 0, "Address: %s", menu.select == 0, menu.page_state.net_join.type);
			MenuStr_Process(&menu.page_state.net_join.port, 64 + 3 * 1, 64 + 16 * 1, "Port: %s", menu.select == 1, menu.page_state.net_join.type);
			MenuStr_Process(&menu.page_state.net_join.pass, 64 + 3 * 2, 64 + 16 * 2, "Pass: %s", menu.select == 2, menu.page_state.net_join.type);
			menu.font_bold.draw(&menu.font_bold, Menu_LowerIf("JOIN", menu.select != 3), 64 + 3 * 3, 64 + 16 * 3, FontAlign_Left);
			
			//Draw background
			Menu_DrawBack(
				true,
				8,
				146 >> 1, 113 >> 1, 253 >> 1,
				0, 0, 0
			);
			break;
		}
		case MenuPage_NetConnect:
		{
			//Change state according to network state
			if (menu.next_page == menu.page && Trans_Idle())
			{
				if (!Network_Connected())
				{
					//Disconnected
					menu.next_page = MenuPage_NetFail;
					menu.next_select = 0;
					Trans_Start();
				}
				else if (Network_Allowed())
				{
					//Allowed to join
					menu.next_page = MenuPage_NetLobby;
					menu.next_select = 0;
					Trans_Start();
				}
			}
			
			//Draw page label
			menu.font_bold.draw(&menu.font_bold,
				"CONNECTING",
				SCREEN_WIDTH2,
				SCREEN_HEIGHT2 - 8,
				FontAlign_Center
			);
			
			//Draw background
			Menu_DrawBack(
				true,
				8,
				113 >> 1, 146 >> 1, 253 >> 1,
				0, 0, 0
			);
			break;
		}
		case MenuPage_NetOpWait:
		{
			//Change state according to network state
			if (menu.next_page == menu.page && Trans_Idle())
			{
				if (!Network_Connected())
				{
					//Disconnected
					menu.next_page = MenuPage_NetFail;
					menu.next_select = 0;
					Trans_Start();
				}
				else if (Network_HasPeer())
				{
					//Peer has joined
					menu.next_page = MenuPage_NetOp;
					menu.next_select = 0;
					Trans_Start();
				}
			}
			
			//Draw page label
			menu.font_bold.draw(&menu.font_bold,
				"WAITING FOR PEER",
				SCREEN_WIDTH2,
				SCREEN_HEIGHT2 - 8,
				FontAlign_Center
			);
			
			//Draw background
			Menu_DrawBack(
				true,
				8,
				113 >> 1, 146 >> 1, 253 >> 1,
				0, 0, 0
			);
			break;
		}
		case MenuPage_NetOp:
		{
			static const struct
			{
				boolean diff;
				StageId stage;
				const char *text;
			} menu_options[] = {
				//{StageId_4_4, "TEST"},
				{false,  StageId_1_4, "TUTORIAL"},
				{true,  StageId_1_1, "BOPEEBO"},
				{true,  StageId_1_2, "FRESH"},
				{true,  StageId_1_3, "DADBATTLE"},
				{true,  StageId_2_1, "SPOOKEEZ"},
				{true,  StageId_2_2, "SOUTH"},
				{true,  StageId_2_3, "MONSTER"},
				{true,  StageId_3_1, "PICO"},
				{true,  StageId_3_2, "PHILLY NICE"},
				{true,  StageId_3_3, "BLAMMED"},
				{true,  StageId_4_1, "SATIN PANTIES"},
				{true,  StageId_4_2, "HIGH"},
				{true,  StageId_4_3, "MILF"},
				{true,  StageId_5_1, "COCOA"},
				{true,  StageId_5_2, "EGGNOG"},
				{true,  StageId_5_3, "WINTER HORRORLAND"},
				{true,  StageId_6_1, "SENPAI"},
				{true,  StageId_6_2, "ROSES"},
				{true,  StageId_6_3, "THORNS"},
				{true,  StageId_7_1, "UGH"},
				{true,  StageId_7_2, "GUNS"},
				{true,  StageId_7_3, "STRESS"},
				{false, StageId_Kapi_1, "WOCKY"},
				{false, StageId_Kapi_2, "BEATHOVEN"},
				{false, StageId_Kapi_3, "HAIRBALL"},
				{false, StageId_Kapi_4, "NYAW"},
				{true,  StageId_Clwn_1, "IMPROBABLE OUTSET"},
				{true,  StageId_Clwn_2, "MADNESS"},
				{true,  StageId_Clwn_3, "HELLCLOWN"},
				{true, StageId_Clwn_4, "EXPURGATION"},
				{false, StageId_2_4, "CLUCKED"},
			};
			
			//Initialize page
			if (menu.page_swap)
			{
				menu.scroll = COUNT_OF(menu_options) * FIXED_DEC(24 + SCREEN_HEIGHT2,1);
				menu.page_param.stage.diff = StageDiff_Normal;
				menu.page_state.net_op.swap = false;
			}
			
			//Handle option and selection
			if (menu.next_page == menu.page && Trans_Idle())
			{
				//Check network state
				if (!Network_Connected())
				{
					//Disconnected
					menu.next_page = MenuPage_NetFail;
					menu.next_select = 0;
					Trans_Start();
				}
				else if (!Network_HasPeer())
				{
					//Peer disconnected
					menu.next_page = MenuPage_NetOpWait;
					menu.next_select = 0;
					Trans_Start();
				}
				
				//Change option
				if (pad_state.press & PAD_UP)
				{
					if (menu.select > 0)
						menu.select--;
					else
						menu.select = COUNT_OF(menu_options) - 1;
				}
				if (pad_state.press & PAD_DOWN)
				{
					if (menu.select < COUNT_OF(menu_options) - 1)
						menu.select++;
					else
						menu.select = 0;
				}
				
				//Select option if cross is pressed
				if (pad_state.press & (PAD_START | PAD_CROSS))
				{
					//Load stage
					Network_SetReady(false);
					stage.mode = menu.page_state.net_op.swap ? StageMode_Net2 : StageMode_Net1;
					menu.next_page = MenuPage_Stage;
					menu.page_param.stage.id = menu_options[menu.select].stage;
					if (!menu_options[menu.select].diff)
						menu.page_param.stage.diff = StageDiff_Hard;
					menu.page_param.stage.story = false;
					Trans_Start();
				}
				
				//Swap characters if triangle is pressed
				if (pad_state.press & PAD_TRIANGLE)
					menu.page_state.net_op.swap ^= true;
			}
			
			//Draw op controls
			const char *control_txt;
			
			control_txt = menu.page_state.net_op.swap ? "You will be Player 2. Press Triangle to swap." : "You will be Player 1. Press Triangle to swap.";
			menu.font_arial.draw_col(&menu.font_arial, control_txt, 24, SCREEN_HEIGHT - 24 - 12, FontAlign_Left, 0x80, 0x80, 0x80);
			menu.font_arial.draw_col(&menu.font_arial, control_txt, 24 + 1, SCREEN_HEIGHT - 24 - 12 + 1, FontAlign_Left, 0x00, 0x00, 0x00);
			
			//Draw difficulty selector
			if (menu_options[menu.select].diff)
				Menu_DifficultySelector(SCREEN_WIDTH - 100, SCREEN_HEIGHT2 - 48);
			
			//Draw options
			s32 next_scroll = menu.select * FIXED_DEC(24,1);
			menu.scroll += (next_scroll - menu.scroll) >> 4;
			
			for (u8 i = 0; i < COUNT_OF(menu_options); i++)
			{
				//Get position on screen
				s32 y = (i * 24) - 8 - (menu.scroll >> FIXED_SHIFT);
				if (y <= -SCREEN_HEIGHT2 - 8)
					continue;
				if (y >= SCREEN_HEIGHT2 + 8)
					break;
				
				//Draw text
				menu.font_bold.draw(&menu.font_bold,
					Menu_LowerIf(menu_options[i].text, menu.select != i),
					48 + (y >> 2),
					SCREEN_HEIGHT2 + y - 8,
					FontAlign_Left
				);
			}
			
			//Draw background
			Menu_DrawBack(
				true,
				8,
				113 >> 1, 253 >> 1, 146 >> 1,
				0, 0, 0
			);
			break;
		}
		case MenuPage_NetLobby:
		{
			//Check network state
			if (menu.next_page == menu.page && Trans_Idle())
			{
				if (!Network_Connected())
				{
					//Disconnected
					menu.next_page = MenuPage_NetFail;
					menu.next_select = 0;
					Trans_Start();
				}
			}
			
			//Draw page label
			menu.font_bold.draw(&menu.font_bold,
				"WAITING FOR HOST",
				SCREEN_WIDTH2,
				SCREEN_HEIGHT2 - 8,
				FontAlign_Center
			);
			
			//Draw background
			Menu_DrawBack(
				true,
				8,
				253 >> 1, 146 >> 1, 113 >> 1,
				0, 0, 0
			);
			break;
		}
		case MenuPage_NetFail:
		{
			//Return to main menu if circle or start is pressed
			if (menu.next_page == menu.page && Trans_Idle())
			{
				if (pad_state.press & (PAD_CIRCLE | PAD_START))
				{
					menu.next_page = MenuPage_Main;
					menu.next_select = 0;
					Trans_Start();
				}
			}
			
			//Draw page label
			menu.font_bold.draw(&menu.font_bold,
				"DISCONNECTED",
				SCREEN_WIDTH2,
				SCREEN_HEIGHT2 - 8,
				FontAlign_Center
			);
			
			//Draw background
			Menu_DrawBack(
				true,
				8,
				253 >> 1, 30 >> 1, 15 >> 1,
				0, 0, 0
			);
			break;
		}
		case MenuPage_NetInitFail:
		{
			//Return to main menu if circle or start is pressed
			if (menu.next_page == menu.page && Trans_Idle())
			{
				if (pad_state.press & (PAD_CIRCLE | PAD_START))
				{
					menu.next_page = MenuPage_Main;
					menu.next_select = 0;
					Trans_Start();
				}
			}
			
			//Draw page label
			menu.font_bold.draw(&menu.font_bold,
				"WSA INIT FAILED",
				SCREEN_WIDTH2,
				SCREEN_HEIGHT2 - 8,
				FontAlign_Center
			);
			
			//Draw background
			Menu_DrawBack(
				true,
				8,
				253 >> 1, 30 >> 1, 15 >> 1,
				0, 0, 0
			);
			break;
		}
	#endif
		case MenuPage_Stage:
		{
			//Unload menu state
			Menu_Unload();
			
			//Load new stage
			LoadScr_Start();
			Stage_Load(menu.page_param.stage.id, menu.page_param.stage.diff, menu.page_param.stage.story);
			gameloop = GameLoop_Stage;
			LoadScr_End();
			break;
		}
		default:
			break;
	}
	
	//Clear page swap flag
	menu.page_swap = menu.page != exec_page;
}