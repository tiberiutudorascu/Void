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
    for (;;)
      ;
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
    if (page > totalPages)
      page = 0;
    if (infoPage > totalInfoPages)
      infoPage = 0;
  }
  if (digitalRead(4) == HIGH)
    apasat = true;

  if (digitalRead(5) == LOW && apasat3 && (currentMillis - lastButtonPress3 > debounceDelay)) {
    apasat3 = false;
    lastButtonPress3 = currentMillis;
    if (infoBool)
      infoBool = !infoBool;
    page = 0;
  }
  if (digitalRead(5) == HIGH)
    apasat3 = true;


  if (digitalRead(16) == LOW && apasat2 && (currentMillis - lastButtonPress2 > debounceDelay)) {
    apasat2 = false;
    lastButtonPress2 = currentMillis;

    if (page == 1) {
      infoBool = !infoBool;
      if (!infoBool)
        previousTime2 = currentMillis;
    }

    else if (page == 0) {
      buttonPressCount++;
      Serial.println(buttonPressCount);
      Serial.print("AngryPoints: ");
      Serial.println(angrypoints);

      if (buttonPressCount == 15 && angrypoints == 3) {
        currentMode = ANGRY;
        buttonPressCount = 0;
      }

      else if (buttonPressCount == 15 && angrypoints < 3) {
        currentMode = CUTE;
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