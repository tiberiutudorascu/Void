#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Servo.h>
#include "images.h"
#include "DHT.h"
#include "icons.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
#define DHTPIN 17
#define DHTTYPE DHT11

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
DHT dht(DHTPIN, DHTTYPE);

int page = 0;
int infoPage = 0;

unsigned long lastButtonPress1 = 0;
unsigned long lastButtonPress2 = 0;
unsigned long lastButtonPress3 = 0;
const unsigned long debounceDelay = 200;
unsigned long lastTempCheck = 0;
int buttonPressCount = 0;
int buttonPressCount2 = 0;

bool apasat = false;
bool apasat2 = false;
bool apasat3 = false;
enum Mode
{
    IDLE,
    CUTE,
    ANGRY,
    SLEEP,
    PLAY,
    HUNGRY,
};
Mode currentMode = IDLE; // idle e activ initial
Mode lastMode = IDLE;

bool infoBool = false;
bool infoConfirm = false;

int totalPages = 5;
int totalInfoPages = 5;

float temperature;
int healthpoints = 0;
float angrypoints = 0;
int sleeppoints = 0;
int playpoints = 0;
int hungerpoints = 0;

int AngryTarget;

unsigned long sleepTime = 0;

unsigned long HPtime = 0;

Servo stangaS;
int PositionSS = 90;
int currentAngleSS;
Servo dreaptaF;
int PositionDF = 0;
int currentAngleDF;
Servo dreaptaS;
int PositionDS = 0;
int currentAngleDS;
Servo stangaF;
int PositionSF = 90;
int currentAngleSF;
int lastMoveTimeSS = 0;
int lastMoveTimeDF = 0;
int lastMoveTimeDS = 0;
int lastMoveTimeSF = 0;
int targetAngles1[] = {45, 0, 45, 0, 45, 0, 45, 0};

int money = 2000;
enum CoinflipState
{
    COINFLIP_IDLE,
    COINFLIP_FLIPPING,
    COINFLIP_SHOW_RESULT
};

CoinflipState coinState = COINFLIP_IDLE;
unsigned long coinStartTime = 0;
int winlose = 0;
bool coinInProgress = false;

int food = 4, drinks = 40;
bool haveTV = false, tvON = false;
bool haveCards = false, cardsON = false;
bool haveBook = false, bookON = false;
bool haveRadio = false, radioON = false;
int itemPage = 0;
bool pickingItem = false;

void setup()
{
    Serial.begin(9600);
    dht.begin();
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    {
        Serial.println(F("Eroare la initializarea OLED!"));
        for (;;)
            ;
    }

    stangaS.attach(18);
    dreaptaF.attach(19);
    dreaptaS.attach(15);
    stangaF.attach(23);

    stangaS.write(PositionSS);
    currentAngleSS = PositionSS;
    dreaptaF.write(PositionDF);
    currentAngleDF = PositionDF;
    dreaptaS.write(PositionDS);
    currentAngleDS = PositionDS;
    stangaF.write(PositionSF);
    currentAngleSF = PositionSF;

    pinMode(4, INPUT_PULLUP);
    pinMode(16, INPUT_PULLUP);
    pinMode(5, INPUT_PULLUP);
    display.clearDisplay();
    display.display();
}
void servoMovement(Servo &s, int targetAngle, int &currentAngle, int &lastMoveTime, int step = 5, unsigned long stepDelay = 10)
{
    unsigned long currentTime7 = millis();
    if (currentTime7 - lastMoveTime >= stepDelay)
    {
        lastMoveTime = currentTime7;
        if (currentAngle < targetAngle)
        {
            currentAngle += step;
            if (currentAngle > targetAngle)
                currentAngle = targetAngle;
            s.write(currentAngle);
        }
        else if (currentAngle > targetAngle)
        {
            currentAngle -= step;
            if (currentAngle < targetAngle)
                currentAngle = targetAngle;
            s.write(currentAngle);
        }
    }
}
void servoMultipleMovement(Servo &s, int targetAngles[], int size, int &currentAngle, int &lastMoveTime, int step = 5, unsigned long stepDelay = 15)
{
    static int i = 0;
    unsigned long currentTime = millis();

    if (i >= size)
        i = 0;

    if (currentTime - lastMoveTime >= stepDelay)
    {
        lastMoveTime = currentTime;
        int target = targetAngles[i];

        if (currentAngle < target)
        {
            currentAngle += step;
            if (currentAngle > target)
                currentAngle = target;
            s.write(currentAngle);
        }
        else if (currentAngle > target)
        {
            currentAngle -= step;
            if (currentAngle < target)
                currentAngle = target;
            s.write(currentAngle);
        }

        if (currentAngle == target)
        {
            i++;
        }
    }
}

void loop()
{
    unsigned long currentMillis = millis();
    temperature = dht.readTemperature();
    if (currentMillis - HPtime >= 300000)
    {
        healthpoints--;
        sleeppoints--;
        HPtime = currentMillis;
    }

    if (digitalRead(4) == LOW && apasat && (currentMillis - lastButtonPress1 > debounceDelay))
    {
        apasat = false;
        lastButtonPress1 = currentMillis;
        if (infoBool)
            infoPage++;
        else
            page++;
        if (page > totalPages)
            page = 0;
        if (page == 1 && infoPage > totalInfoPages)
            infoPage = 0;
        if (pickingItem)
            itemPage++;
        if (itemPage > totalItems)
            itemPage = 0;
    }
    if (digitalRead(4) == HIGH)
        apasat = true;

    if (digitalRead(5) == LOW && apasat3 && (currentMillis - lastButtonPress3 > debounceDelay))
    {
        apasat3 = false;
        lastButtonPress3 = currentMillis;
        if (infoBool)
        {
            infoBool = false;
            infoConfirm = false;
            pickingItem = false;
            itemPage = 0;
        }
        else
        {
            page = 0;
        }
    }
    if (digitalRead(5) == HIGH)
        apasat3 = true;

    if (digitalRead(16) == LOW && apasat2 && (currentMillis - lastButtonPress2 > debounceDelay))
    {
        apasat2 = false;
        lastButtonPress2 = currentMillis;

        if (page != 0)
        {
            if (!infoBool)
            {
                infoBool = true;
                infoConfirm = false;
            }
            else
            {
                infoConfirm = true;
                Serial.println("Confirmare in meniu activata.");
            }
            if (!infoBool)
                previousTime2 = currentMillis;
        }

        else if (page == 0)
        {
            buttonPressCount++;
            Serial.println(buttonPressCount);
            Serial.print("AngryPoints: ");
            Serial.println(angrypoints);
            AngryTarget = temperature >= 19 && temperature <= 27 ? 15 : temperature >= 15 && temperature <= 18 ? 10
                                                                                                               : 5;
            if (buttonPressCount == AngryTarget)
            {
                if (currentMode == SLEEP)
                {
                    currentMode = ANGRY;
                    if (lastMode != currentMode)
                        lastMode = currentMode;
                    buttonPressCount = 0;
                }
                else if (angrypoints == 3)
                {
                    currentMode = ANGRY;
                    if (lastMode != currentMode)
                        lastMode = currentMode;
                    buttonPressCount = 0;
                }
                else if (angrypoints < 3)
                {
                    currentMode = CUTE;
                    if (lastMode != currentMode)
                        lastMode = currentMode;
                    angrypoints = angrypoints + 0.5;
                    Serial.print("Health: ");
                    Serial.println(healthpoints);
                    buttonPressCount = 0;
                }
            }
        }
    }
    if (digitalRead(16) == HIGH)
        apasat2 = true;

    static int lastPage = -1;
    if (page != lastPage)
    {
        Serial.print("Pagina curenta: ");
        Serial.println(page);
        lastPage = page;
    }
    static bool wasOnPage0 = false;
    if (page == 0)
    {
        if (!wasOnPage0)
        {
            currentFrames = 0;
            wasOnPage0 = true;
        }
        if (currentMode == IDLE)
            idle();
        else if (currentMode == CUTE)
            cute(healthpoints);
        else if (currentMode == ANGRY)
            angry(healthpoints);
        else if (currentMode == SLEEP)
            sleepy(sleeppoints);
        else if (currentMode == PLAY)
            idleplay(playpoints);
    }
    else
    {
        wasOnPage0 = false;
    }
    if (page == 1)
    {
        if (infoBool)
            stats();
        else
            progress(healthpoints, sleeppoints, hungerpoints, playpoints);
    }
    else if (page == 2)
    {
        bedroom();
    }
    else if (page == 3)
    {
        kitchen(hungerpoints, drinks, food);
    }
    else if (page == 4)
    {
        coinflip(money);
    }
    else if (page == 5)
    {
        shop(money, drinks, food, haveCards, haveBook, haveRadio, haveTV);
    }
    if (currentMode == SLEEP)
    {
        servoMovement(stangaS, 0, currentAngleSS, lastMoveTimeSS);
        servoMovement(dreaptaF, 90, currentAngleDF, lastMoveTimeDF);
        servoMovement(dreaptaS, 90, currentAngleDS, lastMoveTimeDS);
        servoMovement(stangaF, 0, currentAngleSF, lastMoveTimeSF);
    }
    if (currentMode == IDLE)
    {
        servoMovement(stangaS, 90, currentAngleSS, lastMoveTimeSS);
        servoMovement(dreaptaF, 0, currentAngleDF, lastMoveTimeDF);
        servoMovement(dreaptaS, 0, currentAngleDS, lastMoveTimeDS);
        servoMovement(stangaF, 90, currentAngleSF, lastMoveTimeSF);
    }
    if (currentMode == CUTE)
    {
        servoMovement(stangaS, 45, currentAngleSS, lastMoveTimeSS);
        servoMovement(dreaptaS, 45, currentAngleDS, lastMoveTimeDS);
        servoMovement(dreaptaF, 0, currentAngleDF, lastMoveTimeDF);
        servoMultipleMovement(stangaF, targetAngles1, 8, currentAngleSF, lastMoveTimeSF);
    }

    if (currentMillis - lastTempCheck >= 10000)
    {
        Serial.println(temperature);
        lastTempCheck = currentMillis;
    }
}
void idle()
{
    unsigned long currentTime = millis();
    if (currentTime - previousTime >= framestime[currentFrames])
    {
        previousTime = currentTime;
        display.clearDisplay();
        display.drawBitmap(0, 0, frames[currentFrames], 128, 64, SSD1306_WHITE);
        display.display();
        currentFrames++;
        if (currentFrames >= totalFrames)
            currentFrames = 0;
    }
}
void idleplay(int &playpoints)
{
    unsigned long currentTime = millis();
    if (currentTime - previousTime >= framestime[currentFrames])
    {
        previousTime = currentTime;
        display.clearDisplay();

        display.drawBitmap(0, 0, frames[currentFrames], 128, 64, SSD1306_WHITE);

        if (currentMode == PLAY)
        {
            if (cardsON)
            {
                display.drawBitmap(0, 0, items[0], 128, 64, SSD1306_WHITE);
                playpoints += 2;
            }
            else if (bookON)
            {
                display.drawBitmap(0, 0, items[1], 128, 64, SSD1306_WHITE);
                playpoints += 4;
            }
            else if (radioON)
            {
                display.drawBitmap(0, 0, items[2], 128, 64, SSD1306_WHITE);
                playpoints += 5;
            }
            else if (tvON)
            {
                display.drawBitmap(0, 0, items[3], 128, 64, SSD1306_WHITE);
                playpoints += 8;
            }
        }

        currentFrames++;
        if (currentFrames >= totalFrames)
            currentFrames = 0;

        display.display();
    }
}

void cute(int &health)
{
    unsigned long currentTime3 = millis();

    Serial.print("Frame: ");
    Serial.print(currentFrames3);
    Serial.print(" / ");
    Serial.println(totalFrames3);

    if (currentTime3 - previousTime3 >= framestime3[currentFrames3])
    {
        previousTime3 = currentTime3;
        display.clearDisplay();
        display.drawBitmap(0, 0, frames3[currentFrames3], 128, 64, SSD1306_WHITE);
        display.display();
        currentFrames3++;
    }

    if (currentFrames3 >= totalFrames3)
    {
        Serial.println("ANIMATIE TERMINATA - +1 HEALTH");
        health += 10;
        Serial.print("HEALTH: ");
        Serial.println(health);
        currentMode = IDLE;
        lastMode = CUTE;
        currentFrames3 = 0;
    }
}

void angry(int &health)
{
    unsigned long currentTime4 = millis();
    if (currentTime4 - previousTime4 >= framestime4[currentFrames4])
    {
        previousTime4 = currentTime4;
        display.clearDisplay();
        display.drawBitmap(0, 0, frames4[currentFrames4], 128, 64, SSD1306_WHITE);
        display.display();
        currentFrames4++;
        if (currentFrames4 >= totalFrames4)
        {
            currentFrames4 = 0;
            angrypoints = 0;
            health -= 2;
            currentMode = IDLE;
            lastMode = ANGRY;
        }
    }
}
void bedroom()
{
    unsigned long currentTime6 = millis();

    if (!pickingItem)
    {
        if (!infoBool)
        {
            if (currentTime6 - previousTime6 >= 300)
            {
                previousTime6 = currentTime6;
                display.clearDisplay();
                display.drawBitmap(0, 0, bedrooms[currentFrames6], 128, 64, SSD1306_WHITE);
                display.setTextColor(SSD1306_WHITE);
                display.setTextSize(1);
                display.setCursor(0, 0);
                display.println("Bedroom");

                display.display();
                currentFrames6++;
                if (currentFrames6 >= totalFrames6)
                    currentFrames6 = 0;
            }
        }
        else
        {
            display.clearDisplay();
            display.drawBitmap(0, 0, roomStatic[0], 128, 64, SSD1306_WHITE);
            display.setCursor(0, 0);
            display.setTextColor(SSD1306_WHITE);

            if (currentMode == SLEEP)
            {
                if (infoPage == 0)
                {
                    display.setTextSize(1.5);
                    display.println(">Wake Up");
                    display.setTextSize(1);
                    display.println("Items");

                    if (infoConfirm)
                    {
                        currentMode = IDLE;
                        infoConfirm = false;
                    }
                }
                else if (infoPage == 1)
                {
                    display.setTextSize(1);
                    display.println("Wake Up");
                    display.setTextSize(1.5);
                    display.println(">Pick Item");

                    if (infoConfirm)
                    {
                        pickingItem = true;
                        itemPage = 0;
                        infoConfirm = false;
                    }
                }
            }
            else
            {
                if (infoPage == 0)
                {
                    display.setTextSize(1.5);
                    display.println(">Sleep");
                    display.setTextSize(1);
                    display.println("Items");

                    if (infoConfirm)
                    {
                        currentMode = SLEEP;
                        infoConfirm = false;
                    }
                }
                else if (infoPage == 1)
                {
                    display.setTextSize(1);
                    display.println("Sleep");
                    display.setTextSize(1.5);
                    display.println(">Pick Item");

                    if (infoConfirm)
                    {
                        pickingItem = true;
                        itemPage = 0;
                        infoConfirm = false;
                    }
                }
            }

            if (infoPage > 1)
                infoPage = 0;

            display.display();
        }
    }
    else
    {
        display.clearDisplay();
        display.drawBitmap(0, 0, roomStatic[0], 128, 64, SSD1306_WHITE);
        display.setCursor(0, 0);
        display.setTextColor(SSD1306_WHITE);

        if (itemPage == 0)
            display.println(">Cards");
        else if (itemPage == 1)
            display.println(">Book");
        else if (itemPage == 2)
            display.println(">Radio");
        else if (itemPage == 3)
            display.println(">TV");

        if (infoConfirm)
        {
            if (itemPage == 0 && haveCards)
            {
                if (cardsON)
                {
                    cardsON = false;
                    currentMode = IDLE;
                }
                else
                {
                    cardsON = true;
                    bookON = false;
                    radioON = false;
                    tvON = false;
                    currentMode = PLAY;
                }
            }
            else if (itemPage == 1 && haveBook)
            {
                if (bookON)
                {
                    bookON = false;
                    currentMode = IDLE;
                }
                else
                {
                    cardsON = false;
                    bookON = true;
                    radioON = false;
                    tvON = false;
                    currentMode = PLAY;
                }
            }
            else if (itemPage == 2 && haveRadio)
            {
                if (radioON)
                {
                    radioON = false;
                    currentMode = IDLE;
                }
                else
                {
                    cardsON = false;
                    bookON = false;
                    radioON = true;
                    tvON = false;
                    currentMode = PLAY;
                }
            }
            else if (itemPage == 3 && haveTV)
            {
                if (tvON)
                {
                    tvON = false;
                    currentMode = IDLE;
                }
                else
                {
                    cardsON = false;
                    bookON = false;
                    radioON = false;
                    tvON = true;
                    currentMode = PLAY;
                }
            }
            else
            {
                display.clearDisplay();
                display.setCursor(0, 0);
                display.setTextSize(1);
                display.println("LOCKED ITEM");
                display.display();
                delay(1000);
            }

            infoConfirm = false;
            pickingItem = false;
            infoBool = false;
            infoPage = 0;
            itemPage = 0;
        }

        if (itemPage > 3)
            itemPage = 0;

        display.display();
    }
}

void sleepy(int &sleeppoints)
{
    unsigned long currentTime5 = millis();
    if (currentTime5 - previousTime5 >= framestime5[currentFrames5])
    {
        previousTime5 = currentTime5;
        display.clearDisplay();
        display.drawBitmap(0, 0, frames5[currentFrames5], 128, 64, SSD1306_WHITE);
        display.display();
        currentFrames5++;
        Serial.println("Sleep points: ");
        Serial.println(sleeppoints);

        if (currentFrames5 >= totalFrames5)
        {
            currentFrames5 = 0;
            playpoints = playpoints + 1;
        }
    }
    if (currentTime5 - sleepTime > 60000)
    {
        sleepTime = currentTime5;
        sleeppoints = sleeppoints + 5;
    }

    if (sleeppoints > 100)
    {
        currentMode = IDLE;
        lastMode = SLEEP;
    }
    if (playpoints > 100)
        playpoints = 100;
    if (buttonPressCount == 15)
    {
        currentMode = ANGRY;
        lastMode = SLEEP;
        buttonPressCount = 0;
    }
}
void kitchen(int &hungrypoints, int &drinks, int &food)
{
    unsigned long currentTime8 = millis();

    if (!infoBool)
    {
        if (currentTime8 - previousTime8 >= 300)
        {
            previousTime8 = currentTime8;
            display.clearDisplay();
            display.drawBitmap(0, 0, kitchens[currentFrames8], 128, 64, SSD1306_WHITE);
            display.setTextColor(SSD1306_WHITE);
            display.setTextSize(1);
            display.setCursor(0, 0);
            display.println("Kitchen");

            display.display();
            currentFrames8++;
            if (currentFrames8 >= totalFrames8)
                currentFrames8 = 0;
        }
    }
    else
    {

        display.clearDisplay();
        display.drawBitmap(0, 0, roomStatic[2], 128, 64, SSD1306_WHITE);
        display.setCursor(0, 0);
        display.setTextColor(SSD1306_WHITE);
        if (currentMode != SLEEP)
        {
            if (infoPage == 0)
            {
                display.setTextSize(1.5);
                display.print(">Drink");
                display.println(drinks);
                display.setTextSize(1);
                display.println("Burger");
                if (infoConfirm && drinks >= 1)
                {
                    infoConfirm = false;
                    drinks--;
                    Serial.println("Drank water.");
                    Serial.print("Water bottles left: ");
                    Serial.println(drinks);
                    hungrypoints += 5;
                }
            }
            else if (infoPage == 1 && currentMode != SLEEP)
            {
                display.setTextSize(1);
                display.println("Water");
                display.setTextSize(1.5);
                display.print(">Eat");
                display.println(food);

                if (infoConfirm && food >= 1)
                {
                    infoConfirm = false;
                    food--;
                    Serial.println("Ate burger.");
                    Serial.print("Food left: ");
                    Serial.println(food);
                    hungrypoints += 10;
                }
            }
            if (hungerpoints > 100)
                hungerpoints = 100;
            if (infoPage > 1)
                infoPage = 0;

            display.display();
        }
        else
        {
            display.clearDisplay();
            display.drawBitmap(0, 0, kitchens[currentFrames8], 128, 64, SSD1306_WHITE);
            display.setTextColor(SSD1306_WHITE);
            display.setTextSize(1);
            display.setCursor(0, 0);
            display.println("Kitchen");
            display.println("Void is asleep");

            display.display();
        }
    }
}

void coinflip(int &money)
{
    unsigned long currentTime7 = millis();
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("$");
    display.println(money);

    if (!infoBool)
    {
        display.drawBitmap(0, 0, roomStatic[1], 128, 64, SSD1306_WHITE);
    }
    else
    {
        switch (coinState)
        {
        case COINFLIP_IDLE:
            coinInProgress = true;
            coinStartTime = currentTime7;
            currentFrames7 = 0;
            winlose = random(1, 3); // 1 win, 2 lose
            coinState = COINFLIP_FLIPPING;
            break;

        case COINFLIP_FLIPPING:
            if (currentTime7 - previousTime7 >= framestime7[currentFrames7] && currentFrames7 < totalFrames7)
            {
                previousTime7 = currentTime7;
                display.drawBitmap(0, 0, coinflipanimation[currentFrames7], 128, 64, SSD1306_WHITE);
                currentFrames7++;
            }
            else if (currentFrames7 >= totalFrames7)
            {
                coinStartTime = currentTime7;
                coinState = COINFLIP_SHOW_RESULT;
            }
            break;

        case COINFLIP_SHOW_RESULT:
            if (winlose == 1)
            {
                display.drawBitmap(0, 0, winloseframes[0], 128, 64, SSD1306_WHITE);
            }
            else
            {
                display.drawBitmap(0, 0, winloseframes[1], 128, 64, SSD1306_WHITE);
            }

            if (currentTime7 - coinStartTime >= 2300)
            {
                if (winlose == 1)
                {
                    money++;
                }
                infoBool = false;
                coinInProgress = false;
                coinState = COINFLIP_IDLE;
            }
            break;
        }
    }

    display.display();
}

void shop(int &money, int &drinks, int &food, bool &haveCards, bool &haveBook, bool &haveRadio, bool &haveTV)
{
    unsigned long currentTime9 = millis();

    if (!infoBool)
    {
        if (currentTime9 - previousTime9 >= 300)
        {
            previousTime9 = currentTime9;
            display.clearDisplay();
            display.drawBitmap(0, 0, shops[currentFrames9], 128, 64, SSD1306_WHITE);
            display.setTextColor(SSD1306_WHITE);
            display.setTextSize(1);
            display.setCursor(0, 0);
            display.println("Shop");

            display.display();
            currentFrames9++;
            if (currentFrames9 >= totalFrames9)
                currentFrames9 = 0;
        }
    }
    else
    {
        display.clearDisplay();
        if (currentMode != SLEEP)
        {
            display.setCursor(0, 0);
            display.setTextColor(SSD1306_WHITE);
            display.setTextSize(1);

            if (infoPage == 0)
            {
                display.print(">Buy Water $5\n");
                display.println("Buy Burger");
                display.println("Buy Cards");
                display.println("Buy Book");
                display.println("Buy Radio");
                display.println("Buy TV");

                if (infoConfirm && money >= 5)
                {
                    drinks++;
                    money -= 5;
                    Serial.println("Bought Water");
                    infoConfirm = false;
                    infoPage = 0;
                }
            }
            else if (infoPage == 1)
            {
                display.println("Buy Water");
                display.print(">Buy Burger $10\n");
                display.println("Buy Cards");
                display.println("Buy Book");
                display.println("Buy Radio");
                display.println("Buy TV");

                if (infoConfirm && money >= 10)
                {
                    food++;
                    money -= 10;
                    Serial.println("Bought Burger");
                    infoConfirm = false;
                    infoPage = 0;
                }
            }
            else if (infoPage == 2)
            {
                display.println("Buy Water");
                display.println("Buy Burger");
                display.print(">Buy Cards $20\n");
                display.println("Buy Book");
                display.println("Buy Radio");
                display.println("Buy TV");

                if (infoConfirm && money >= 20 && !haveCards)
                {
                    haveCards = true;
                    money -= 20;
                    Serial.println("Bought Cards");
                    infoConfirm = false;
                    infoPage = 0;
                }
                else if (infoConfirm && haveCards)
                {
                    Serial.println("Already own Cards!");
                    infoConfirm = false;
                }
            }
            else if (infoPage == 3)
            {
                display.println("Buy Water");
                display.println("Buy Burger");
                display.println("Buy Cards");
                display.print(">Buy Book $30\n");
                display.println("Buy Radio");
                display.println("Buy TV");

                if (infoConfirm && money >= 30 && !haveBook)
                {
                    haveBook = true;
                    money -= 30;
                    Serial.println("Bought Book");
                    infoConfirm = false;
                    infoPage = 0;
                }
                else if (infoConfirm && haveBook)
                {
                    Serial.println("Already own Book!");
                    infoConfirm = false;
                }
            }
            else if (infoPage == 4)
            {
                display.println("Buy Water");
                display.println("Buy Burger");
                display.println("Buy Cards");
                display.println("Buy Book");
                display.print(">Buy Radio $50\n");
                display.println("Buy TV");

                if (infoConfirm && money >= 50 && !haveRadio)
                {
                    haveRadio = true;
                    money -= 50;
                    Serial.println("Bought Radio");
                    infoConfirm = false;
                    infoPage = 0;
                }
                else if (infoConfirm && haveRadio)
                {
                    Serial.println("Already own Radio!");
                    infoConfirm = false;
                }
            }
            else if (infoPage == 5)
            {
                display.println("Buy Water");
                display.println("Buy Burger");
                display.println("Buy Cards");
                display.println("Buy Book");
                display.println("Buy Radio");
                display.print(">Buy TV $200\n");

                if (infoConfirm && money >= 200 && !haveTV)
                {
                    haveTV = true;
                    money -= 200;
                    Serial.println("Bought TV");
                    infoConfirm = false;
                    infoPage = 0;
                }
                else if (infoConfirm && haveTV)
                {
                    Serial.println("Already own TV!");
                    infoConfirm = false;
                }
            }

            if (infoPage > 5)
                infoPage = 0;
        }
        else
        {
            display.drawBitmap(0, 0, shops[currentFrames8], 128, 64, SSD1306_WHITE);
            display.setCursor(0, 0);
            display.setTextColor(SSD1306_WHITE);
            display.setTextSize(1);
            display.println("Shop");
            display.println("Void is asleep");
        }
        display.display();
    }
}

void progress(int &healthpoints, int &sleeppoints, int &hungrypoints, int &playpoints)
{
    unsigned long currentTime2 = millis();
    display.clearDisplay();

    if (currentTime2 - previousTime2 >= 300)
    {
        previousTime2 = currentTime2;
        currentFrames2++;
        if (currentFrames2 >= 3)
            currentFrames2 = 0;
    }

    if (temperature >= 19 && temperature <= 27)
    {
        display.drawBitmap(0, 0, progiconsSun[currentFrames2], 128, 64, SSD1306_WHITE);
        if (currentTime2 - HPtime >= 300000)
        {
            healthpoints -= 5;
            sleeppoints -= 5;
            hungrypoints -= 5;
            playpoints -= 5;
            HPtime = currentTime2;
        }
    }
    else if (temperature >= 15 && temperature <= 18)
    {
        display.drawBitmap(0, 0, progiconsCold[currentFrames2], 128, 64, SSD1306_WHITE);
    }
    else if (temperature >= -5 && temperature <= 14)
    {
        display.drawBitmap(0, 0, progiconsFreeze[currentFrames2], 128, 64, SSD1306_WHITE);
        if (currentTime2 - HPtime >= 300000)
        {
            healthpoints -= 5;
            sleeppoints -= 5;
            hungrypoints -= 7;
            playpoints += 2;
            HPtime = currentTime2;
        }
    }
    else if (temperature >= 28 && temperature <= 40)
    {
        display.drawBitmap(0, 0, progiconsHot[currentFrames2], 128, 64, SSD1306_WHITE);
        if (currentTime2 - HPtime >= 300000)
        {
            healthpoints -= 10;
            sleeppoints -= 10;
            hungrypoints -= 5;
            playpoints -= 5;
            HPtime = currentTime2;
        }
    }

    display.drawRoundRect(5, 16, 10, 40, 5, SSD1306_WHITE);  // Health
    display.drawRoundRect(25, 16, 10, 40, 5, SSD1306_WHITE); // Hunger
    display.drawRoundRect(45, 16, 10, 40, 5, SSD1306_WHITE); // Joy
    display.drawRoundRect(65, 16, 10, 40, 5, SSD1306_WHITE); // Sleep

    int barHeightHealth = map(healthpoints, 0, 100, 0, 40);
    int barYHealth = 16 + (40 - barHeightHealth);
    display.fillRoundRect(5, barYHealth, 10, barHeightHealth, 5, SSD1306_WHITE);

    // HUNGER BAR
    int barHeightHunger = map(hungrypoints, 0, 100, 0, 40);
    int barYHunger = 16 + (40 - barHeightHunger);
    display.fillRoundRect(25, barYHunger, 10, barHeightHunger, 5, SSD1306_WHITE);

    // JOY BAR
    int barHeightJoy = map(playpoints, 0, 100, 0, 40);
    int barYJoy = 16 + (40 - barHeightJoy);
    display.fillRoundRect(45, barYJoy, 10, barHeightJoy, 5, SSD1306_WHITE);

    // SLEEP BAR
    int barHeightSleep = map(sleeppoints, 0, 100, 0, 40);
    int barYSleep = 16 + (40 - barHeightSleep);
    display.fillRoundRect(65, barYSleep, 10, barHeightSleep, 5, SSD1306_WHITE);

    display.display();
}

void stats()
{
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);

    if (infoPage == 0)
    {
        display.println("INFORMATION - Stats");
        display.setCursor(0, 16);
        display.println("On the next pages you will find general informations and how to keep your Void pet healthy, safe and active.");
        display.setCursor(0, 0);
    }
    else if (infoPage == 1)
    {
        display.println("How to: Health");
        display.setCursor(0, 16);
        display.println("You can fill up Void's Health bar by petting him and keeping him well fed.");
        display.setCursor(0, 0);
    }
    else if (infoPage == 2)
    {
        display.println("How to: Hunger");
        display.setCursor(0, 16);
        display.println("You can fill up Void's Hunger bar by feeding him in the kitchen.");
        display.setCursor(0, 0);
    }
    else if (infoPage == 3)
    {
        display.println("How to: Joy");
        display.setCursor(0, 16);
        display.println("You can fill up Void's Joy bar by playing the different minigames. Don't try too hard, he doesn't mind losing.");
        display.setCursor(0, 0);
    }
    else if (infoPage == 4)
    {
        display.println("How to: Sleep");
        display.setCursor(0, 16);
        display.println("After you're done with everything be sure to lay him to bed. He loves sleeping..Maybe too much.");
        display.setCursor(0, 0);
    }
    else if (infoPage == 5)
    {
        display.println("How to: Climate");
        display.setCursor(0, 16);
        display.println("Be sure the temperature isn't too high or too low. He may be an alien, but he still likes comfort");
        display.setCursor(0, 0);
    }

    display.display();
}