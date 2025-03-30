#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
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
const unsigned long debounceDelay = 200;
unsigned long lastTempCheck = 0;
int buttonPressCount = 0;

bool apasat = false;
bool apasat2 = false;
enum Mode
{
    IDLE,
    CUTE
};
Mode currentMode = IDLE;

bool infoBool = false;

int totalPages = 3;
int totalInfoPages = 5;

float temperature;
int healthpoints = 0;

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
    pinMode(4, INPUT_PULLUP);
    pinMode(16, INPUT_PULLUP);
    display.clearDisplay();
    display.display();
}

void loop()
{
    unsigned long currentMillis = millis();
    temperature = dht.readTemperature();

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

    if (digitalRead(16) == LOW && apasat2 && (currentMillis - lastButtonPress2 > debounceDelay))
    {
        apasat2 = false;
        lastButtonPress2 = currentMillis;

        if (page == 1)
        {
            infoBool = !infoBool;
            buttonPressCount++;
            if (!infoBool)
                previousTime2 = currentMillis;
        }

        else if (page == 0)
        {
            buttonPressCount++;
            Serial.println(buttonPressCount);
            if (buttonPressCount == 15)
            {
                currentMode = CUTE;
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
            cute(&healthpoints);
    }
    else if (page == 1)
    {
        if (infoBool)
            stats();
        else
            progress();
    }
    else if (page == 2 || page == 3)
    {
        display.clearDisplay();
        display.display();
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
    if (currentTime - previousTime >= framestime[currentFrame])
    {
        previousTime = currentTime;
        display.clearDisplay();
        display.drawBitmap(0, 0, frames[currentFrame], 128, 64, SSD1306_WHITE);
        display.display();
        currentFrame++;
        if (currentFrame >= totalFrames)
            currentFrame = 0;
    }
}

void cute(int *health)
{
    unsigned long currentTime3 = millis();
    if (currentTime3 - previousTime3 >= framestime3[currentFrame3])
    {
        previousTime3 = currentTime3;
        display.clearDisplay();
        display.drawBitmap(0, 0, frames3[currentFrame3], 128, 64, SSD1306_WHITE);
        display.display();
        currentFrame3++;
    }
    if (currentFrame3 >= totalFrames3)
    {
        (*health)++;
        currentMode = IDLE;
        currentFrame3 = 0;
    }
}

void progress()
{
    unsigned long currentTime2 = millis();
    display.clearDisplay();

    if (currentTime2 - previousTime2 >= 300)
    {
        previousTime2 = currentTime2;
        if (temperature >= 19 && temperature <= 27)
            display.drawBitmap(0, 0, progiconsSun[currentFrame2], 128, 64, SSD1306_WHITE);
        if (temperature >= 15 && temperature <= 18)
            display.drawBitmap(0, 0, progiconsCold[currentFrame2], 128, 64, SSD1306_WHITE);
        if (temperature >= -5 && temperature <= 14)
            display.drawBitmap(0, 0, progiconsFreeze[currentFrame2], 128, 64, SSD1306_WHITE);
        if (temperature >= 28 && temperature <= 40)
            display.drawBitmap(0, 0, progiconsHot[currentFrame2], 128, 64, SSD1306_WHITE);

        display.drawRoundRect(5, 16, 10, 40, 10, SSD1306_WHITE);
        display.fillRoundRect(5, 46, 10, 10, 10, SSD1306_WHITE);

        display.drawRoundRect(25, 16, 10, 40, 10, SSD1306_WHITE);
        display.drawRoundRect(25, 16, 10, 40, 10, SSD1306_WHITE);

        display.drawRoundRect(45, 16, 10, 40, 10, SSD1306_WHITE);
        display.drawRoundRect(65, 16, 10, 40, 10, SSD1306_WHITE);

        currentFrame2++;
        if (currentFrame2 >= 3)
            currentFrame2 = 0;
    }
    else
    {
        display.drawRoundRect(5, 16, 10, 40, 10, SSD1306_WHITE);
        display.fillRoundRect(5, 46, 10, 10, 10, SSD1306_WHITE);

        display.drawRoundRect(25, 16, 10, 40, 10, SSD1306_WHITE);
        display.drawRoundRect(25, 16, 10, 40, 10, SSD1306_WHITE);

        display.drawRoundRect(45, 16, 10, 40, 10, SSD1306_WHITE);
        display.drawRoundRect(65, 16, 10, 40, 10, SSD1306_WHITE);

        // Redesenarea pictogramelor corespunzătoare în funcție de temperatura citită
        if (temperature >= 19 && temperature <= 27)
            display.drawBitmap(0, 0, progiconsSun[currentFrame2], 128, 64, SSD1306_WHITE);
        if (temperature >= 15 && temperature <= 18)
            display.drawBitmap(0, 0, progiconsCold[currentFrame2], 128, 64, SSD1306_WHITE);
        if (temperature >= -5 && temperature <= 14)
            display.drawBitmap(0, 0, progiconsFreeze[currentFrame2], 128, 64, SSD1306_WHITE);
        if (temperature >= 28 && temperature <= 40)
            display.drawBitmap(0, 0, progiconsHot[currentFrame2], 128, 64, SSD1306_WHITE);
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
