#define XA_LENGTH(x) (((u64)(x) * 75) / 100 * IO_SECT_SIZE) //Centiseconds to sectors in bytes (w)

typedef struct
{
	XA_File file;
	u32 length;
} XA_TrackDef;

static const XA_TrackDef xa_tracks[] = {
	//MENU.XA
	{XA_Menu, XA_LENGTH(13820)}, //XA_GettinFreaky
	{XA_Menu, XA_LENGTH(3800)},  //XA_GameOver
	//WEEK1A.XA
	{XA_Week1A, XA_LENGTH(12100)}, //XA_Bopeebo
	{XA_Week1A, XA_LENGTH(18000)}, //XA_Fresh
	//WEEK1B.XA
	{XA_Week1B, XA_LENGTH(22400)}, //XA_Dadbattle
	{XA_Week1B, XA_LENGTH(19300)}, //XA_Tutorial
	//WEEK2A.XA
	{XA_Week2A, XA_LENGTH(9923)}, //XA_Spookeez
	{XA_Week2A, XA_LENGTH(8880)}, //XA_South
	//WEEK2B.XA
	{XA_Week2B, XA_LENGTH(17778)}, //XA_Monster
	{XA_Week2B, XA_LENGTH(11000)}, //XA_Clucked
	//WEEK3A.XA
	{XA_Week3A, XA_LENGTH(8400)},  //XA_Pico
	{XA_Week3A, XA_LENGTH(10000)}, //XA_Philly
	//WEEK3B.XA
	{XA_Week3B, XA_LENGTH(10700)}, //XA_Blammed
	//WEEK4A.XA
	{XA_Week4A, XA_LENGTH(9300)},  //XA_SatinPanties
	{XA_Week4A, XA_LENGTH(10300)}, //XA_High
	//WEEK4B.XA
	{XA_Week4B, XA_LENGTH(12300)}, //XA_MILF
	{XA_Week4B, XA_LENGTH(10300)}, //XA_Test
	//WEEK5A.XA
	{XA_Week5A, XA_LENGTH(11520)}, //XA_Cocoa
	{XA_Week5A, XA_LENGTH(9401)},  //XA_Eggnog
	//WEEK5B.XA
	{XA_Week5B, XA_LENGTH(13223)}, //XA_WinterHorrorland
	//WEEK6A.XA
	{XA_Week6A, XA_LENGTH(9800)}, //XA_Senpai
	{XA_Week6A, XA_LENGTH(8928)}, //XA_Roses
	//WEEK6B.XA
	{XA_Week6B, XA_LENGTH(10298)}, //XA_Thorns
	//WEEK7A.XA
	{XA_Week7A, XA_LENGTH(8493)},  //XA_Ugh
	{XA_Week7A, XA_LENGTH(13866)}, //XA_Guns
	//WEEK7B.XA
	{XA_Week7B, XA_LENGTH(12200)}, //XA_Stress
	//KAPIA.XA
	{XA_KapiA, XA_LENGTH(12138)}, //XA_Wocky
	{XA_KapiA, XA_LENGTH(12000)}, //XA_Beathoven
	//KAPIB.XA
	{XA_KapiB, XA_LENGTH(17159)}, //XA_Hairball
	{XA_KapiB, XA_LENGTH(15157)}, //XA_Nyaw
	//CLWNA.XA
	{XA_ClwnA, XA_LENGTH(10740)}, //XA_ImprobableOutset
	{XA_ClwnA, XA_LENGTH(15169)}, //XA_Madness
	//CLWNB.XA
	{XA_ClwnB, XA_LENGTH(21886)}, //XA_Hellclown
	{XA_ClwnB, XA_LENGTH(19550)}, //XA_Expurgation
};

static const char *xa_paths[] = {
	"\\MUSIC\\MENU.XA;1",   //XA_Menu
	"\\MUSIC\\WEEK1A.XA;1", //XA_Week1A
	"\\MUSIC\\WEEK1B.XA;1", //XA_Week1B
	NULL,
};

typedef struct
{
	const char *name;
	boolean vocal;
} XA_Mp3;

static const XA_Mp3 xa_mp3s[] = {
	//MENU.XA
	{"freaky", false},   //XA_GettinFreaky
	{"gameover", false}, //XA_GameOver
	//WEEK1A.XA
	{"bopeebo", true}, //XA_Bopeebo
	{"fresh", true},   //XA_Fresh
	//WEEK1B.XA
	{"dadbattle", true}, //XA_Dadbattle
	{"tutorial", false}, //XA_Tutorial
	//WEEK2A.XA
	{"spookeez", true}, //XA_Spookeez
	{"south", true},    //XA_South
	//WEEK2B.XA
	{"monster", true}, //XA_Monster
	{"clucked", true}, //XA_Clucked
	//WEEK3A.XA
	{"pico", true},   //XA_Pico
	{"philly", true}, //XA_Philly
	//WEEK3B.XA
	{"blammed", true}, //XA_Blammed
	//WEEK4A.XA
	{"satinpanties", true}, //XA_SatinPanties
	{"high", true},         //XA_High
	//WEEK4B.XA
	{"milf", true}, //XA_MILF
	{"test", true}, //XA_Test
	//WEEK5A.XA
	{"cocoa", true},  //XA_Cocoa
	{"eggnog", true}, //XA_Eggnog
	//WEEK5B.XA
	{"winterhorrorland", true}, //XA_WinterHorrorland
	//WEEK6A.XA
	{"senpai", true}, //XA_Senpai
	{"roses", true},  //XA_Roses
	//WEEK6B.XA
	{"thorns", true}, //XA_Thorns
	//WEEK7A.XA
	{"ugh", true},  //XA_Ugh
	{"guns", true}, //XA_Guns
	//WEEK7B.XA
	{"stress", true}, //XA_Stress
	
	{NULL, false}
};
