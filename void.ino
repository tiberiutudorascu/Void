#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "images.h"
#include "DHT.h"
#include "icons.h"
#include "functions.h"

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
enum Mode {
  IDLE,
  CUTE,
  ANGRY
};
Mode currentMode = IDLE;  // idle e activ initial

bool infoBool = false;
bool infoConfirm = false;

int totalPages = 5;
int totalInfoPages = 5;

float temperature;
int healthpoints = 0;
float angrypoints = 0;

unsigned long HPtime = 0;

void setup() {
  Serial.begin(9600);
  dht.begin();
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("Eroare la initializarea OLED!"));
    for (;;);
  }
  pinMode(4, INPUT_PULLUP);
  pinMode(16, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  display.clearDisplay();
  display.display();
}

void loop() {
  unsigned long currentMillis = millis();
  temperature = dht.readTemperature();
  if (currentMillis - HPtime >= 300000) {
    healthpoints--;
    HPtime = currentMillis;
  }

  if (digitalRead(4) == LOW && apasat && (currentMillis - lastButtonPress1 > debounceDelay)) {
    apasat = false;
    lastButtonPress1 = currentMillis;
    if (infoBool)
      infoPage++;
    else
      page++;
    if (page > totalPages) page = 0;
    if (infoPage > totalInfoPages) infoPage = 0;
  }
  if (digitalRead(4) == HIGH) apasat = true;

  if (digitalRead(5) == LOW && apasat3 && (currentMillis - lastButtonPress3 > debounceDelay)) {
    apasat3 = false;
    lastButtonPress3 = currentMillis;
    if (infoBool) {
      infoBool = false;
      infoConfirm = false;
    } else {
      page = 0;
    }
  }
  if (digitalRead(5) == HIGH) apasat3 = true;

  if (digitalRead(16) == LOW && apasat2 && (currentMillis - lastButtonPress2 > debounceDelay)) {
    apasat2 = false;
    lastButtonPress2 = currentMillis;

    if (page == 1 || page == 2) {
      if (!infoBool) {
        infoBool = true;
        infoConfirm = false;
      } else {
        infoConfirm = true;
        Serial.println("Confirmare in meniu activata.");
      }
      if (!infoBool) previousTime2 = currentMillis;
    }

    else if (page == 0) {
      buttonPressCount++;
      Serial.println(buttonPressCount);
      Serial.print("AngryPoints: ");
      Serial.println(angrypoints);

      if (buttonPressCount == 15 && angrypoints == 3) {
        currentMode = ANGRY;
        buttonPressCount = 0;
      } else if (buttonPressCount == 15 && angrypoints < 3) {
        currentMode = CUTE;
        angrypoints = angrypoints + 0.5;
        Serial.print("Health: ");
        Serial.println(healthpoints);
        buttonPressCount = 0;
      }
    }
  }
  if (digitalRead(16) == HIGH) apasat2 = true;

  static int lastPage = -1;
  if (page != lastPage) {
    Serial.print("Pagina curenta: ");
    Serial.println(page);
    lastPage = page;
  }

  if (page == 0) {
    if (currentMode == IDLE)
      idle();
    else if (currentMode == CUTE)
      cute(healthpoints);
    else if (currentMode == ANGRY)
      angry(healthpoints);
  } else if (page == 1) {
    if (infoBool)
      stats();
    else
      progress();
  } else if (page == 2) {
    bedroom();
  } else if (page == 3) {
    kitchen();
  } else if (page == 4) {
    coinflip();
  } else if (page == 5) {
    shop();
  }

  if (currentMillis - lastTempCheck >= 10000) {
    Serial.println(temperature);
    lastTempCheck = currentMillis;
  }
}

// Restul funcțiilor rămân neschimbate

void idle() {
  unsigned long currentTime = millis();
  if (currentTime - previousTime >= framestime[currentFrames]) {
    previousTime = currentTime;
    display.clearDisplay();
    display.drawBitmap(0, 0, frames[currentFrames], 128, 64, SSD1306_WHITE);
    display.display();
    currentFrames++;
    if (currentFrames >= totalFrames)
      currentFrames = 0;
  }
}

void cute(int &health) {
  unsigned long currentTime3 = millis();

  Serial.print("Frame: ");
  Serial.print(currentFrames3);
  Serial.print(" / ");
  Serial.println(totalFrames3);

  if (currentTime3 - previousTime3 >= framestime3[currentFrames3]) {
    previousTime3 = currentTime3;
    display.clearDisplay();
    display.drawBitmap(0, 0, frames3[currentFrames3], 128, 64, SSD1306_WHITE);
    display.display();
    currentFrames3++;
  }

  if (currentFrames3 >= totalFrames3) {
    Serial.println("ANIMATIE TERMINATA - +1 HEALTH");
    health++;
    Serial.print("HEALTH: ");
    Serial.println(health);
    currentMode = IDLE;
    currentFrames3 = 0;
  }
}

void angry(int &health) {
  unsigned long currentTime4 = millis();
  if (currentTime4 - previousTime4 >= framestime4[currentFrames4]) {
    previousTime4 = currentTime4;
    display.clearDisplay();
    display.drawBitmap(0, 0, frames4[currentFrames4], 128, 64, SSD1306_WHITE);
    display.display();
    currentFrames4++;
    if (currentFrames4 >= totalFrames4) {
      currentFrames4 = 0;
      angrypoints = 0;
      health--;
      currentMode = IDLE;
    }
  }
}
void bedroom() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("BedRoom");
  display.println();
  display.drawBitmap(0, 0, rooms[0], 128, 64, SSD1306_WHITE);

  if (infoBool) {
    if (infoPage == 0) {
      display.println(">Sleep");
      display.println("Browse items");
      if(buttonPressCount2 == 1)
      {
      }
    } else if (infoPage == 1) {
      display.println("Sleep");
      display.println(">Browse items");
    }
    if (infoPage > 1)
      infoPage = 0;
  } else {
    display.println("animatie");
  }

  display.display();
}

void kitchen() {
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
void coinflip() {
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
void shop() {
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

void progress() {
  unsigned long currentTime2 = millis();
  display.clearDisplay();

  if (currentTime2 - previousTime2 >= 300) {
    previousTime2 = currentTime2;
    if (temperature >= 19 && temperature <= 27)
      display.drawBitmap(0, 0, progiconsSun[currentFrames2], 128, 64, SSD1306_WHITE);
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
  } else {
    display.drawRoundRect(5, 16, 10, 40, 10, SSD1306_WHITE);
    display.fillRoundRect(5, 46, 10, 10, 10, SSD1306_WHITE);

    display.drawRoundRect(25, 16, 10, 40, 10, SSD1306_WHITE);
    display.drawRoundRect(25, 16, 10, 40, 10, SSD1306_WHITE);

    display.drawRoundRect(45, 16, 10, 40, 10, SSD1306_WHITE);
    display.drawRoundRect(65, 16, 10, 40, 10, SSD1306_WHITE);

    if (temperature >= 19 && temperature <= 27)
      display.drawBitmap(0, 0, progiconsSun[currentFrames2], 128, 64, SSD1306_WHITE);
    if (temperature >= 15 && temperature <= 18)
      display.drawBitmap(0, 0, progiconsCold[currentFrames2], 128, 64, SSD1306_WHITE);
    if (temperature >= -5 && temperature <= 14)
      display.drawBitmap(0, 0, progiconsFreeze[currentFrames2], 128, 64, SSD1306_WHITE);
    if (temperature >= 28 && temperature <= 40)
      display.drawBitmap(0, 0, progiconsHot[currentFrames2], 128, 64, SSD1306_WHITE);
  }
  display.display();
}

void stats() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);

  if (infoPage == 0) {
    display.println("INFORMATION - Stats");
    display.setCursor(0, 16);
    display.println("On the next pages you will find general informations and how to keep your Void pet healthy, safe and active.");
    display.setCursor(0, 0);
  } else if (infoPage == 1) {
    display.println("How to: Health");
    display.setCursor(0, 16);
    display.println("You can fill up Void's Health bar by giving him a shower and keeping him well fed.");
    display.setCursor(0, 0);
  } else if (infoPage == 2) {
    display.println("How to: Hunger");
    display.setCursor(0, 16);
    display.println("You can fill up Void's Hunger bar by interacting with the human touch sensor. He feeds off souls.");
    display.setCursor(0, 0);
  } else if (infoPage == 3) {
    display.println("How to: Joy");
    display.setCursor(0, 16);
    display.println("You can fill up Void's Joy bar by playing the different minigames. Don't try too hard, he doesn't mind losing.");
    display.setCursor(0, 0);
  } else if (infoPage == 4) {
    display.println("How to: Sleep");
    display.setCursor(0, 16);
    display.println("After you're done with everything be sure to lay him to bed. He loves sleeping..Maybe too much.");
    display.setCursor(0, 0);
  } else if (infoPage == 5) {
    display.println("How to: Climate");
    display.setCursor(0, 16);
    display.println("Be sure the temperature isn't too high or too low. He may be an alien, but he still likes comfort");
    display.setCursor(0, 0);
  }

  display.display();
}