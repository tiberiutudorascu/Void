#include <Wire.h>              // Bibliotecă pentru comunicația I2C
#include <Adafruit_GFX.h>      // Bibliotecă grafică Adafruit
#include <Adafruit_SSD1306.h>  // Bibliotecă pentru afișajul OLED SSD1306
#include "images.h"            // Include fișierul cu imagini predefinite
#include "DHT.h"
#include "icons.h"  // Bibliotecă pentru senzorul DHT (temperatură și umiditate)

#define SCREEN_WIDTH 128     // Lățimea ecranului OLED în pixeli
#define SCREEN_HEIGHT 64     // Înălțimea ecranului OLED în pixeli
#define OLED_RESET -1        // Pinul de reset pentru OLED (nu este utilizat, de aceea -1)
#define SCREEN_ADDRESS 0x3C  // Adresa I2C a ecranului OLED

#define DHTPIN 17      // Pinul la care este conectat senzorul DHT
#define DHTTYPE DHT11  // Tipul senzorului DHT utilizat

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
enum Mode {IDLE, CUTE};
Mode currentMode = IDLE;// idle e activ initial

bool infoBool = false;

int totalPages = 3;
int totalInfoPages = 5;

float temperature;
int healthpoints = 0;

void setup() {
  Serial.begin(9600);
  dht.begin();
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("Eroare la initializarea OLED!"));
    for (;;);
  }
  pinMode(4, INPUT_PULLUP);
  pinMode(16, INPUT_PULLUP);
  display.clearDisplay();
  display.display();
}

void loop() {
  unsigned long currentMillis = millis();
  temperature = dht.readTemperature();

  if (digitalRead(4) == LOW && apasat && (currentMillis - lastButtonPress1 > debounceDelay)) {
    apasat = false;
    lastButtonPress1 = currentMillis;
    if (infoBool) infoPage++;
    else page++;
    if (page > totalPages) page = 0;
    if (infoPage > totalInfoPages) infoPage = 0;
  }
  if (digitalRead(4) == HIGH) apasat = true;

  if (page == 1 || page == 0) {
    if (digitalRead(16) == LOW && apasat2 && (currentMillis - lastButtonPress2 > debounceDelay)) {
      apasat2 = false;
      infoBool = !infoBool;
      currentButtonCount++;
      if (!infoBool) previousTime2 = currentMillis;
      lastButtonPress2 = currentMillis;
      if(currentButtonCount == 5 && page == 0){
        currentMode= CUTE;
      buttonPressCount = 0;}
    }
    if (digitalRead(16) == HIGH) apasat2 = true;
  }

  static int lastPage = -1;
  if (page != lastPage) {
    Serial.print("Pagina curenta: ");
    Serial.println(page);
    lastPage = page;
  }

  if (page == 0) {
   if(currentMode == IDLE) 
     idle();
    else if (currentMode == CUTE) cute(&healthpoints);
  } else if (page == 1) {
    if (infoBool) stats();
    else progress();
  } else if (page == 2 || page == 3) {
    display.clearDisplay();
    display.display();
  }

  if (currentMillis - lastTempCheck >= 10000) {
    Serial.println(temperature);
    lastTempCheck = currentMillis;
  }
}

void idle() {
  unsigned long currentTime = millis();
  if (animatingidle && !animatingcute && (currentTime - previousTime >= framestime[currentFrame])) {
    previousTime = currentTime;
    display.clearDisplay();
    display.drawBitmap(0, 0, frames[currentFrame], 128, 64, SSD1306_WHITE);
    display.display();
    currentFrame++;
    if (currentFrame >= totalFrames) currentFrame = 0;
  }
}

void cute(int *health) {
  unsigned long currentTime3 = millis();
    if (currentTime3 - previousTime3 >= framestime3[currentFrame3]) {
      previousTime3 = currentTime3;
      display.clearDisplay();
      display.drawBitmap(0, 0, frames3[currentFrame3], 128, 64, SSD1306_WHITE);
      display.display();
      currentFrame3++;}
      if (currentFrame3 >= totalFrames3) {
        (*health)++;
        currentMode= IDLE:
        currentFrame3 = 0;
      }
    }
  }

  if (digitalRead(16) == HIGH) apasat2 = true;
}

void progress() {
  unsigned long currentTime2 = millis();  // Obține timpul curent pentru animația de progres
  display.clearDisplay();                 // Curăță ecranul

  // Dacă s-a trecut suficient timp, actualizează animația de progres
  if (currentTime2 - previousTime2 >= 300) {
    previousTime2 = currentTime2;  // Actualizează timpul ultimei actualizări
    // Selectează pictograma corespunzătoare în funcție de intervalul de temperatură
    if (temperature >= 19 && temperature <= 27)
      display.drawBitmap(0, 0, progiconsSun[currentFrame2], 128, 64, SSD1306_WHITE);
    if (temperature >= 15 && temperature <= 18)
      display.drawBitmap(0, 0, progiconsCold[currentFrame2], 128, 64, SSD1306_WHITE);
    if (temperature >= -5 && temperature <= 14)
      display.drawBitmap(0, 0, progiconsFreeze[currentFrame2], 128, 64, SSD1306_WHITE);
    if (temperature >= 28 && temperature <= 40)
      display.drawBitmap(0, 0, progiconsHot[currentFrame2], 128, 64, SSD1306_WHITE);

    // Desenează diverse elemente grafice suplimentare pe ecran (probabil reprezentând bare sau indicatori)
    display.drawRoundRect(5, 16, 10, 40, 10, SSD1306_WHITE);
    display.fillRoundRect(5, 46, 10, 10, 10, SSD1306_WHITE);

    display.drawRoundRect(25, 16, 10, 40, 10, SSD1306_WHITE);
    display.drawRoundRect(25, 16, 10, 40, 10, SSD1306_WHITE);

    display.drawRoundRect(45, 16, 10, 40, 10, SSD1306_WHITE);
    display.drawRoundRect(65, 16, 10, 40, 10, SSD1306_WHITE);

    currentFrame2++;  // Trecerea la următorul cadru pentru animația de progres
    if (currentFrame2 >= 3)
      currentFrame2 = 0;  // Resetarea cadrului după ce s-au parcurs toate opțiunile de pictograme
  } else {
    // Desenează elementele grafice fără a actualiza cadrul animației
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
  display.display();  // Actualizează ecranul OLED cu toate modificările
}

void stats() {
  display.clearDisplay();               // Curăță ecranul înainte de afișarea informațiilor
  display.setTextColor(SSD1306_WHITE);  // Setează culoarea textului la alb
  display.setTextSize(1);               // Setează dimensiunea textului
  display.setCursor(0, 0);              // Setează cursorul în colțul din stânga sus

  // Afișează diferite pagini de informații/statistici în funcție de valoarea lui infoPage
  if (infoPage == 0) {
    display.println("INFORMATION - Stats");  // Titlu pentru pagina de informații
    display.setCursor(0, 16);
    display.println("On the next pages you will find general informations and how to keep your Void pet healthy, safe and active.");
    display.setCursor(0, 0);
  } else if (infoPage == 1) {
    display.println("How to: Health");  // Informații despre sănătate
    display.setCursor(0, 16);
    display.println("You can fill up Void's Health bar by giving him a shower and keeping him well fed.");
    display.setCursor(0, 0);
  } else if (infoPage == 2) {
    display.println("How to: Hunger");  // Informații despre foame
    display.setCursor(0, 16);
    display.println("You can fill up Void's Hunger bar by interacting with the human touch sensor. He feeds off souls.");
    display.setCursor(0, 0);
  } else if (infoPage == 3) {
    display.println("How to: Joy");  // Informații despre bucurie
    display.setCursor(0, 16);
    display.println("You can fill up Void's Joy bar by playing the different minigames. Don't try too hard, he doesn't mind losing.");
    display.setCursor(0, 0);
  } else if (infoPage == 4) {
    display.println("How to: Sleep");  // Informații despre somn
    display.setCursor(0, 16);
    display.println("After you're done with everything be sure to lay him to bed. He loves sleeping..Maybe too much.");
    display.setCursor(0, 0);
  } else if (infoPage == 5) {
    display.println("How to: Climate");  // Informații despre climat
    display.setCursor(0, 16);
    display.println("Be sure the temperature isn't too high or too low. He may be an alien, but he still likes comfort");
    display.setCursor(0, 0);
  }

  display.display();  // Actualizează ecranul OLED pentru a afișa textul
}
