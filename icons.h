const unsigned char *frames[] = {happyidle1, happyidle2, happyidle3, happyidle4, happyidle5, happyidle6,
                                 happyidle7, happyidle8, happyidle9, happyidle8, happyidle7, happyidle6, happyidle5, happyidle4, happyidle3, happyidle2,
                                 happyidle1, happyidle1, happyidle2, happyidle3, happyidle4, happyidle5, happyidle6, happyidle7, happyidle8, happyidle9,
                                 happyidle8, happyidle7, happyidle6, happyidle5, happyidle4, happyidle3, happyidle2, happyidle1, happyidle1, happyidle2,
                                 happyidle3, happyidle4, happyidle5, happyidle6, happyidle7, happyidle8, happyidle9, happyidle8, happyidle7, happyidle6,
                                 happyidle5, happyidle4, happyidle3, happyidle2, happyidle1, happyidle10, happyidle11, happyidle12, happyidle13, happyidle14,
                                 happyidle13, happyidle12, happyidle11, happyidle10, happyidle1};
const unsigned char *frames3[] = {cute1, cute2, cute3, cute4, cute5, cute6, cute7, cute6, cute5, cute4, cute3, cute2, cute1};
const unsigned char *frames4[] = {angry1, angry2, angry3, angry4, angry5, angry6, angry7, angry8, angry7, angry8, angry7, angry8, angry7, angry8, angry7, angry8, angry7, angry8, angry6, angry5, angry4, angry3, angry2, angry1};
const unsigned char *frames5[] = {sleep1, sleep2, sleep3, sleep4};
const unsigned char *bedrooms[] = {bedroompic2, bedroompic3, bedroompic4};
const unsigned char *progiconsSun[] = {infosun1, infosun2, infosun3};
const unsigned char *progiconsCold[] = {infocold1, infocold2, infocold3};
const unsigned char *progiconsHot[] = {infohot1, infohot2, infohot3};
const unsigned char *progiconsFreeze[] = {infofreeze1, infofreeze2, infofreeze3};

const unsigned char *roomStatic[] = {bedroompic};
const unsigned framestime[] = {10, 10, 10, 10, 10, 10, 10, 10, 30, 10, 10, 10, 10, 10, 10, 10, 5000,
                               10, 10, 10, 10, 10, 10, 10, 10, 30, 10, 10, 10, 10, 10, 10, 10, 5000,
                               10, 10, 10, 10, 10, 10, 10, 10, 30, 10, 10, 10, 10, 10, 10, 10, 5000,
                               15, 15, 15, 15, 3000, 15, 15, 15, 15};
const unsigned framestime3[] = {10, 10, 10, 10, 10, 10, 2000, 5000, 10, 10, 10, 10, 10, 10};
const unsigned int framestime4[] = {20, 20, 20, 20, 50, 200, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 200, 100, 80, 50, 40, 30, 20, 20};
const unsigned int framestime5[] = {4000, 200, 200, 200};

const int totalFrames = sizeof(frames) / sizeof(frames[0]);
const int totalFrames3 = sizeof(frames3) / sizeof(frames3[0]);
const int totalFrames4 = sizeof(frames4) / sizeof(frames4[0]);
const int totalFrames5 = sizeof(frames5) / sizeof(frames5[0]);
const int totalFrames6 = sizeof(bedrooms) / sizeof(bedrooms[0]);

unsigned long previousTime = 0;
unsigned long previousTime2 = 0;
unsigned long previousTime3 = 0;
unsigned long previousTime4 = 0;
unsigned long previousTime5 = 0;
unsigned long previousTime6 = 0;

int currentFrames = 0;
int currentFrames2 = 0;
int currentFrames3 = 0;
int currentFrames4 = 0;
int currentFrames5 = 0;
int currentFrames6 = 0;