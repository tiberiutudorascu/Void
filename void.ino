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
    BORED
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
int lastMoveTime = 0;

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

    stangaS.attach(23);
    dreaptaF.attach(18);
    dreaptaS.attach(15);
    stangaF.attach(19);

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
void servoMovement(Servo &s, int targetAngle, int &currentAngle, int &lastMoveTime, int step = 1, unsigned long stepDelay = 15)
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
        if (infoPage > totalInfoPages)
            infoPage = 0;
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

        if (page == 1 || page == 2)
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
            AngryTarget = temperature >= 19 && temperature <= 27 ? 15 : temperature >= 15 && temperature <= 18 ?
                                                                                                               : 5;
            if (buttonPressCount == AngryTarget && angrypoints == 3)
            {
                currentMode = ANGRY;
                if (lastMode != currentMode)
                    lastMode = currentMode;
                buttonPressCount = 0;
            }
            else if (buttonPressCount == AngryTarget && angrypoints < 3)
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
    if (digitalRead(16) == HIGH)
        apasat2 = true;

    static int lastPage = -1;
    if (page != lastPage)
    {
        Serial.print("Pagina curenta: ");
        Serial.println(page);
        lastPage = page;
    }

    if (page == 0)
    {
        if (currentMode == IDLE)
            idle();
        else if (currentMode == CUTE)
            cute(healthpoints);
        else if (currentMode == ANGRY)
            angry(healthpoints);
        else if (currentMode == SLEEP)
            sleepy(sleeppoints);
    }
    else if (page == 1)
    {
        if (infoBool)
            stats();
        else
            progress(healthpoints, sleeppoints, hungerpoints, playpoints);
    }
    else if (page == 2)
    {
        if (currentMode == PLAY)
            instrument(playpoints);
        else
            bedroom();
    }
    else if (page == 3)
    {
        kitchen();
    }
    else if (page == 4)
    {
        coinflip();
    }
    else if (page == 5)
    {
        shop();
    }
    if (currentMode == SLEEP)
    {
        servoMovement(stangaS, 0, currentAngleSS, lastMoveTime);
        servoMovement(dreaptaF, 90, currentAngleDF, lastMoveTime);
        servoMovement(dreaptaS, 90, currentAngleDS, lastMoveTime);
        servoMovement(stangaF, 0, currentAngleSF, lastMoveTime);
    }
    if (currentMode == IDLE)
    {
        servoMovement(stangaS, 90, currentAngleSS, lastMoveTime);
        servoMovement(dreaptaF, 0, currentAngleDF, lastMoveTime);
        servoMovement(dreaptaS, 0, currentAngleDS, lastMoveTime);
        servoMovement(stangaF, 90, currentAngleSF, lastMoveTime);
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
        if (sleeppoints >= 45)
            display.drawBitmap(0, 0, frames[currentFrames], 128, 64, SSD1306_WHITE);
        display.display();
        currentFrames++;
        if (currentFrames >= totalFrames)
            currentFrames = 0;
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
        health++;
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
            health--;
            currentMode = IDLE;
            lastMode = ANGRY;
        }
    }
}
void bedroom()
{
    unsigned long currentTime6 = millis();

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

        if (infoPage == 0 && currentMode == IDLE)
        {
            display.setTextSize(1.5);
            display.println(">Sleep");
            display.setTextSize(1);
            display.println("Instrument");
            if (infoConfirm)
            {
                currentMode = SLEEP;
                infoConfirm = false;
            }
        }
        else if (infoPage == 0 && currentMode == SLEEP)
        {
            display.setTextSize(1.5);
            display.println(">Wake Up");
            display.setTextSize(1);
            display.println("Instrument");
            if (infoConfirm)
            {
                currentMode = IDLE;
                infoConfirm = false;
            }
        }
        else if (infoPage == 1)
        {
            display.setTextSize(1);
            display.println(currentMode == SLEEP ? "Wake Up" : "Sleep");
            display.setTextSize(1.5);
            display.println(">Instrument");
            if (infoConfirm)
            {
                currentMode = PLAY;
            }
        }

        if (infoPage > 1)
            infoPage = 0;

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
        sleeppoints = sleeppoints + 2;
    }

    if (sleeppoints > 100)
    {
        currentMode = IDLE;
        lastMode = SLEEP;
    }

    if (sleeppoints < 100 && currentMode == SLEEP && buttonPressCount == 15)
    {
        currentMode = ANGRY;
        lastMode = SLEEP;
    }
}
void instrument(int &playpoints)
{
}

void kitchen()
{
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("Esti in bucatarie.");
    display.println();
    display.println("Aici o sa apara");
    display.println("un frigider pe care");
    display.println("poti intra.");
    display.display();
}
void coinflip()
{
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("Esti in minigameul coinflip.");
    display.println();
    display.println("Aici o sa apara");
    display.println("un joc pe care");
    display.println("poti intra.");
    display.display();
}
void shop()
{
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("Esti in shop.");
    display.println();
    display.println("Aici o sa apara");
    display.println("un shop pe care");
    display.println("poti intra sa cheltui banii.");
    display.display();
}

void progress(int &healthpoints, int &sleeppoints, int &hungrypoints, int &playpoints)
{
    unsigned long currentTime2 = millis();
    display.clearDisplay();

    if (currentTime2 - previousTime2 >= 300)
    {
        previousTime2 = currentTime2;
        if (temperature >= 19 && temperature <= 27)
        {
            display.drawBitmap(0, 0, progiconsSun[currentFrames2], 128, 64, SSD1306_WHITE);
            if (currentTime2 - HPtime >= 300000)
            {
                healthpoints--;
                sleeppoints--;
                HPtime = currentTime2;
            }
        }
        if (temperature >= 15 && temperature <= 18)
            display.drawBitmap(0, 0, progiconsCold[currentFrames2], 128, 64, SSD1306_WHITE);
        if (temperature >= -5 && temperature <= 14)
            display.drawBitmap(0, 0, progiconsFreeze[currentFrames2], 128, 64, SSD1306_WHITE);
        if (temperature >= 28 && temperature <= 40)
            display.drawBitmap(0, 0, progiconsHot[currentFrames2], 128, 64, SSD1306_WHITE);

        display.drawRoundRect(5, 16, 10, 40, 10, SSD1306_WHITE);
        display.fillRoundRect(5, 46, 10, 10, 10, SSD1306_WHITE);

        display.drawRoundRect(25, 16, 10, 40, 10, SSD1306_WHITE);
        display.drawRoundRect(25, 16, 10, 40, 10, SSD1306_WHITE);

        display.drawRoundRect(45, 16, 10, 40, 10, SSD1306_WHITE);
        display.drawRoundRect(65, 16, 10, 40, 10, SSD1306_WHITE);

        currentFrames2++;
        if (currentFrames2 >= 3)
            currentFrames2 = 0;
    }
    else
    {
        display.drawRoundRect(5, 16, 10, 40, 10, SSD1306_WHITE);
        display.fillRoundRect(5, 46, 10, 10, 10, SSD1306_WHITE);

        display.drawRoundRect(25, 16, 10, 40, 10, SSD1306_WHITE);
        display.drawRoundRect(25, 16, 10, 40, 10, SSD1306_WHITE);

        display.drawRoundRect(45, 16, 10, 40, 10, SSD1306_WHITE);
        display.drawRoundRect(65, 16, 10, 40, 10, SSD1306_WHITE);

        if (temperature >= 19 && temperature <= 27)
        {
            display.drawBitmap(0, 0, progiconsSun[currentFrames2], 128, 64, SSD1306_WHITE);
            if (currentTime2 - HPtime >= 300000)
            {
                healthpoints--;
                sleeppoints--;
                HPtime = currentTime2;
            }
        }
        if (temperature >= 15 && temperature <= 18)
            display.drawBitmap(0, 0, progiconsCold[currentFrames2], 128, 64, SSD1306_WHITE);
        if (temperature >= -5 && temperature <= 14)
            display.drawBitmap(0, 0, progiconsFreeze[currentFrames2], 128, 64, SSD1306_WHITE);
        if (temperature >= 28 && temperature <= 40)
            display.drawBitmap(0, 0, progiconsHot[currentFrames2], 128, 64, SSD1306_WHITE);
    }
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
        display.println("You can fill up Void's Health bar by giving him a shower and keeping him well fed.");
        display.setCursor(0, 0);
    }
    else if (infoPage == 2)
    {
        display.println("How to: Hunger");
        display.setCursor(0, 16);
        display.println("You can fill up Void's Hunger bar by interacting with the human touch sensor. He feeds off souls.");
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