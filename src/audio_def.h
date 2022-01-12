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
	{XA_WeekTA, XA_LENGTH(12100)}, //XA_Improbable Outset
	{XA_WeekTA, XA_LENGTH(18000)}, //XA_Madness
	//WEEK1B.XA
	{XA_WeekTB, XA_LENGTH(22400)}, //XA_Hellclown
	{XA_WeekTB, XA_LENGTH(19300)}, //XA_Expurgation
	//CUT.XA
	{XA_Cut, XA_LENGTH(1300)}, //XA_Cutscene
};

static const char *xa_paths[] = {
	"\\MUSIC\\MENU.XA;1",   //XA_Menu
	"\\MUSIC\\WEEKTA.XA;1", //XA_Week1A
	"\\MUSIC\\WEEKTB.XA;1", //XA_Week1B
	"\\MUSIC\\CUT.XA;1", //XA_Cut
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
	{"improbable", true}, //XA_improbable
	{"madness", true},   //XA_madness
	//WEEK1B.XA
	{"hellclown", true}, //XA_hellclown
	{"expurgation", false}, //XA_expurgation
	//CUT.XA
	{"cutscene", true}, //XA_cutscene
	
	{NULL, false}
};
