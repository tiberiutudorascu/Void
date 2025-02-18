#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "images.h"
#include "DHT.h"  // Declararea Bibliotecilor

// Definirea dimensiunilor ecranului OLED si a adreselor necesare
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64  // Specificarea datelor pentru initializarea ecranului OLED
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

#define DHTPIN 17
#define DHTTYPE DHT11

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);  // Crearea obiectului display

DHT dht(DHTPIN, DHTTYPE);

// Array-uri care contin frame-urile pentru animatia "Void" si iconitele pentru barele de progres
const unsigned char *frames[] = { happyidle1, happyidle2, happyidle3, happyidle4, happyidle5, happyidle2, happyidle3 };  // Frame-urile pentru Void
const unsigned char *progiconsSun[] = { infosun1, infosun2, infosun3 };
const unsigned char *progiconsCold[] = { infocold1, infocold2, infocold3 };
const unsigned char *progiconsHot[] = { infohot1, infohot2, infohot3 };
const unsigned char *progiconsFreeze[] = { infofreeze1, infofreeze2, infofreeze3 };  // Frame-urile pentru iconitele de la barele de progres

// Timpul de afisare (in milisecunde) pentru fiecare frame al animatiei "Void"
const unsigned framestime[] = { 7000, 2000, 1000, 2000, 800, 2500, 3800 };
const int totalFrames = sizeof(frames) / sizeof(frames[0]);  // Numarul total de frame-uri pentru animatie

// Variabile pentru masurarea timpului (folosite cu millis())
unsigned long previousTime = 0;
unsigned long previousTime2 = 0;  // Pentru sincronizarea altor animatii sau elemente grafice

// Indici pentru frame-uri in animatii
int currentFrame = 0;
int currentFrame2 = 0;  // Pentru animatia de pe pagina 1 (progress)

// Variabile pentru gestionarea paginilor de afisare
int page = 0;
int infoPage = 0;  // Pagina curenta a ecranului de informatii

// Variabile pentru debounce la apasarea butoanelor
unsigned long lastButtonPress1 = 0;
unsigned long lastButtonPress2 = 0;
const unsigned long debounceDelay = 200;
unsigned long lastTempCheck = 0;

// Flaguri pentru starea butoanelor (folosite pentru debounce)
bool apasat = false;
bool apasat2 = false;

// Flag pentru comutarea intre ecranul de animatie si cel de informatii
bool infoBool = false;

// Numarul total de pagini pentru navigare
int totalPages = 3;
int totalInfoPages = 5;
// Numarul total de pagini pentru ecranul de informatii

float temperature;

void setup() {
  Serial.begin(9600);  // Initializare comunicatie seriala pentru debug
  dht.begin();
  // Initializarea ecranului OLED; daca esueaza, se intra intr-un loop infinit
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("Eroare la initializarea OLED!"));
    for (;;)
      ;
  }

  // Configurarea pinilor pentru butoane (folosind rezistente interne de pull-up)
  pinMode(4, INPUT_PULLUP);   // Butonul de parcurgere a paginilor
  pinMode(16, INPUT_PULLUP);  // Butonul pentru interactiuni (toggle info)

  display.clearDisplay();  // Curatarea ecranului la initializare
  display.display();       // Actualizarea ecranului
}

void loop() {
  unsigned long currentMillis = millis();  // Obtinem timpul curent in milisecunde
  temperature = dht.readTemperature();

  // Gestionarea butonului de pe pin 4 pentru schimbarea paginilor
  if (digitalRead(4) == LOW && apasat && (currentMillis - lastButtonPress1 > debounceDelay)) {
    apasat = false;
    lastButtonPress1 = currentMillis;
    if (infoBool) {
      infoPage++;  // Daca suntem in modul informatii, incrementam infoPage
    } else
      page++;  // Altfel, incrementam pagina principala
    if (page > totalPages) {
      page = 0;  // Resetam pagina daca a depasit numarul maxim
    }
    if (infoPage > totalInfoPages) {
      infoPage = 0;  // Resetam pagina de informatii daca a depasit numarul maxim
    }
  }
  if (digitalRead(4) == HIGH) {
    apasat = true;
  }  // Logica de debounce pentru butonul 1

  // Pentru pagina 1: gestionarea butonului de pe pin 16 pentru a comuta intre informatii si animatie
  if (page == 1) {
    if (digitalRead(16) == LOW && apasat2 && (currentMillis - lastButtonPress2 > debounceDelay)) {
      apasat2 = false;
      infoBool = !infoBool;  // Inverseaza starea infoBool
      if (!infoBool) {
        previousTime2 = currentMillis;  // Reseteaza timerul cand se revine la animatie
      }
      lastButtonPress2 = currentMillis;
    }
    if (digitalRead(16) == HIGH) {
      apasat2 = true;
    }
  }  // Flagurile pentru butonul 2 sunt actualizate in functie de apasare

  // Afisarea in Serial Monitor a paginii curente, doar daca aceasta se schimba
  static int lastPage = -1;
  if (page != lastPage) {
    Serial.print("Pagina curenta: ");
    Serial.println(page);
    lastPage = page;
  }

  // Selectarea continutului de afisat in functie de pagina curenta
  if (page == 0) {
    idle();  // Daca pagina este 0, se afiseaza animatia "Void"
  } else if (page == 1) {
    if (infoBool)
      stats();
    else {  // Daca infoBool este false, afisam animatia de progres
      progress();
    }
  } else if (page == 2 || page == 3) {
    display.clearDisplay();  // Pentru paginile 2 si 3, momentan nu se afiseaza nimic
    display.display();
  }
  if (currentMillis - lastTempCheck >= 10000) {
    Serial.println(temperature);
    lastTempCheck = currentMillis;
  }
}

// Functia idle: afiseaza animatia "Void" prin ciclarea frame-urilor
void idle() {
  unsigned long currentTime = millis();

  // Verifica daca timpul scurs este mai mare decat durata frame-ului curent
  if (currentTime - previousTime >= framestime[currentFrame]) {
    previousTime = currentTime;
    display.clearDisplay();
    // Deseneaza frame-ul curent al animatiei "Void"
    display.drawBitmap(0, 0, frames[currentFrame], 128, 64, SSD1306_WHITE);
    display.display();

    currentFrame++;  // Trecem la urmatorul frame
    if (currentFrame >= totalFrames) {
      currentFrame = 0;  // Resetam indexul daca am atins capatul array-ului
    }
  }
}

// Functia progress: afiseaza animatia cu iconitele si barele de progres
void progress() {
  unsigned long currentTime2 = millis();
  display.clearDisplay();  // Curata ecranul inainte de a desena noile elemente

  // Daca 300 ms au trecut, actualizam iconita animata si elementele grafice
  if (currentTime2 - previousTime2 >= 300) {
    previousTime2 = currentTime2;
    if (temperature >= 19 && temperature <= 27)
      display.drawBitmap(0, 0, progiconsSun[currentFrame2], 128, 64, SSD1306_WHITE);
    if (temperature >= 15 && temperature <= 18)
      display.drawBitmap(0, 0, progiconsCold[currentFrame2], 128, 64, SSD1306_WHITE);
   if (temperature >= -5 && temperature <= 14)
      display.drawBitmap(0, 0, progiconsFreeze[currentFrame2], 128, 64, SSD1306_WHITE);
    if (temperature >= 28 && temperature <= 40)
      display.drawBitmap(0, 0, progiconsHot[currentFrame2], 128, 64, SSD1306_WHITE);


    // Deseneaza barele de progres folosind forme rotunjite
    display.drawRoundRect(5, 16, 10, 40, 10, SSD1306_WHITE);
    display.fillRoundRect(5, 46, 10, 10, 10, SSD1306_WHITE);

    display.drawRoundRect(25, 16, 10, 40, 10, SSD1306_WHITE);
    display.drawRoundRect(25, 16, 10, 40, 10, SSD1306_WHITE);

    display.drawRoundRect(45, 16, 10, 40, 10, SSD1306_WHITE);
    display.drawRoundRect(65, 16, 10, 40, 10, SSD1306_WHITE);

    currentFrame2++;  // Trecem la urmatorul frame pentru iconita
    if (currentFrame2 >= 3)
      currentFrame2 = 0;  // Resetam indexul daca a ajuns la finalul array-ului
  } else {
    // Daca 300 ms nu au trecut, afisam in continuare ultima iconita si elementele grafice statice
    display.drawRoundRect(5, 16, 10, 40, 10, SSD1306_WHITE);  // Iconita afisata ca ultima din rotatie
    display.fillRoundRect(5, 46, 10, 10, 10, SSD1306_WHITE);  // Barele afisate normal

    display.drawRoundRect(25, 16, 10, 40, 10, SSD1306_WHITE);
    display.drawRoundRect(25, 16, 10, 40, 10, SSD1306_WHITE);

    display.drawRoundRect(45, 16, 10, 40, 10, SSD1306_WHITE);
    display.drawRoundRect(65, 16, 10, 40, 10, SSD1306_WHITE);

    if (temperature >= 19 && temperature <= 27)
      display.drawBitmap(0, 0, progiconsSun[currentFrame2], 128, 64, SSD1306_WHITE);
    if (temperature >= 15 && temperature <= 18)
      display.drawBitmap(0, 0, progiconsCold[currentFrame2], 128, 64, SSD1306_WHITE);
   if (temperature >= -5 && temperature <= 14)
      display.drawBitmap(0, 0, progiconsFreeze[currentFrame2], 128, 64, SSD1306_WHITE);
    if (temperature >= 28 && temperature <= 40)
      display.drawBitmap(0, 0, progiconsHot[currentFrame2], 128, 64, SSD1306_WHITE);
  }
  display.display();  // Actualizeaza ecranul cu tot ce a fost desenat
}
void stats() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);

  // Selectam mesajul in functie de infoPage
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
