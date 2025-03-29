// Declarație de array-uri pentru diferitele cadre și pictograme utilizate în animații și afișări
const unsigned char *frames[] = { happyidle1, happyidle2, happyidle3, happyidle4, happyidle5, happyidle6,
                                  happyidle7, happyidle8, happyidle9, happyidle8, happyidle7, happyidle6, happyidle5, happyidle4, happyidle3, happyidle2,
                                  happyidle1, happyidle1, happyidle2, happyidle3, happyidle4, happyidle5, happyidle6, happyidle7, happyidle8, happyidle9,
                                  happyidle8, happyidle7, happyidle6, happyidle5, happyidle4, happyidle3, happyidle2, happyidle1, happyidle1, happyidle2,
                                  happyidle3, happyidle4, happyidle5, happyidle6, happyidle7, happyidle8, happyidle9, happyidle8, happyidle7, happyidle6,
                                  happyidle5, happyidle4, happyidle3, happyidle2, happyidle1, happyidle10, happyidle11, happyidle12, happyidle13, happyidle14,
                                  happyidle13, happyidle12, happyidle11, happyidle10, happyidle1 };

const unsigned char *progiconsSun[] = { infosun1, infosun2, infosun3 };
const unsigned char *progiconsCold[] = { infocold1, infocold2, infocold3 };
const unsigned char *progiconsHot[] = { infohot1, infohot2, infohot3 };
const unsigned char *progiconsFreeze[] = { infofreeze1, infofreeze2, infofreeze3 };

// Array-ul care definește durata fiecărui cadru în animația "idle"
const unsigned framestime[] = { 10, 10, 10, 10, 10, 10, 10, 10, 30, 10, 10, 10, 10, 10, 10, 10, 5000,
                                10, 10, 10, 10, 10, 10, 10, 10, 30, 10, 10, 10, 10, 10, 10, 10, 5000,
                                10, 10, 10, 10, 10, 10, 10, 10, 30, 10, 10, 10, 10, 10, 10, 10, 5000,
                                15, 15, 15, 15, 3000, 15, 15, 15, 15, 3000 };
const int totalFrames = sizeof(frames) / sizeof(frames[0]);  // Calculul numărului total de cadre din animația "idle"
unsigned long previousTime = 0;                              // Variabilă pentru a stoca momentul ultimei actualizări a cadrului în animația "idle"
unsigned long previousTime2 = 0;                             // Variabilă pentru a stoca momentul ultimei actualizări a cadrului în animația de progres

int currentFrame = 0;   // Indexul cadrului curent pentru animația "idle"
int currentFrame2 = 0;  // Indexul cadrului curent pentru animația de progres