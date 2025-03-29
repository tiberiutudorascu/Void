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

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);  // Initializează obiectul display cu parametrii specificați

DHT dht(DHTPIN, DHTTYPE);  // Inițializează obiectul DHT cu pinul și tipul senzorului


int page = 0;      // Variabilă pentru a urmări pagina curentă afișată
int infoPage = 0;  // Variabilă pentru a urmări pagina de informații

unsigned long lastButtonPress1 = 0;       // Stochează momentul ultimei apăsări a primului buton (pentru schimbarea paginii)
unsigned long lastButtonPress2 = 0;       // Stochează momentul ultimei apăsări a celui de-al doilea buton (pentru schimbarea modului de afișare)
const unsigned long debounceDelay = 200;  // Timpul de așteptare pentru debounce-ul butoanelor (200 ms)
unsigned long lastTempCheck = 0;    // Variabilă pentru a urmări momentul ultimei verificări a temperaturii
int buttonPressCount = 0;      

bool apasat = false;   // Flag pentru debounce-ul primului buton
bool apasat2 = false;  // Flag pentru debounce-ul celui de-al doilea buton

bool infoBool = false;  // Flag pentru a comuta între modurile de afișare (informații/statistici)

int totalPages = 3;      // Numărul total de pagini pentru modul principal
int totalInfoPages = 5;  // Numărul total de pagini pentru modul de informații

float temperature;  // Variabilă pentru a stoca temperatura citită de la senzorul DHT

void setup() {
  Serial.begin(9600);  // Inițializează comunicarea serială la 9600 baud
  dht.begin();         // Inițializează senzorul DHT
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("Eroare la initializarea OLED!"));  // Afișează un mesaj de eroare dacă ecranul OLED nu se inițializează corect
    for (;;)
      ;  // Buclă infinită în caz de eroare la inițializare
  }

  pinMode(4, INPUT_PULLUP);   // Configurarea pinului 4 ca intrare cu rezistență internă de pull-up
  pinMode(16, INPUT_PULLUP);  // Configurarea pinului 16 ca intrare cu rezistență internă de pull-up

  display.clearDisplay();  // Curăță ecranul OLED
  display.display();       // Actualizează ecranul pentru a reflecta modificările
}

void loop() {
  unsigned long currentMillis = millis();  // Obține timpul curent în milisecunde de la pornirea programului
  temperature = dht.readTemperature();     // Citește temperatura de la senzorul DHT

  // Verifică dacă primul buton este apăsat, flag-ul "apasat" este true și s-a respectat timpul de debounce
  if (digitalRead(4) == LOW && apasat && (currentMillis - lastButtonPress1 > debounceDelay)) {
    apasat = false;                    // Resetează flag-ul de debounce pentru primul buton
    lastButtonPress1 = currentMillis;  // Actualizează momentul ultimei apăsări a butonului
    if (infoBool) {
      infoPage++;  // Dacă suntem în modul informații, schimbă pagina de informații
    } else
      page++;  // Altfel, schimbă pagina principală
    if (page > totalPages) {
      page = 0;  // Resetează pagina dacă a depășit numărul total de pagini
    }
    if (infoPage > totalInfoPages) {
      infoPage = 0;  // Resetează pagina de informații dacă a depășit numărul total de pagini
    }
  }
  // Resetează flag-ul pentru primul buton când acesta nu este apăsat
  if (digitalRead(4) == HIGH) {
    apasat = true;
  }
  // Dacă pagina curentă este 1, verifică al doilea buton pentru schimbarea modului de afișare (informații vs. progres)
  if (page == 1 || page == 0) {
    if (digitalRead(16) == LOW && apasat2 && (currentMillis - lastButtonPress2 > debounceDelay)) {
      apasat2 = false;       // Resetează flag-ul de debounce pentru al doilea buton
      infoBool = !infoBool;  // Inversează starea lui infoBool pentru a comuta între moduri
      if (!infoBool) {
        previousTime2 = currentMillis;  // Resetează timpul pentru animația de progres dacă se iese din modul informații
      }
      lastButtonPress2 = currentMillis;  // Actualizează momentul ultimei apăsări a celui de-al doilea buton
    }
    if (digitalRead(16) == HIGH) {
      apasat2 = true;  // Setează flag-ul când butonul nu este apăsat
    }
  }

  // Afișează pagina curentă în Serial Monitor când se schimbă
  static int lastPage = -1;
  if (page != lastPage) {
    Serial.print("Pagina curenta: ");
    Serial.println(page);
    lastPage = page;
  }

  // Logica de afișare pentru fiecare pagină
  if (page == 0) {
    idle();  // Afișează animația idle
    if (infoBool) 
      cute();
  }
else if (page == 1) {
  if (infoBool)
    stats();  // Afișează pagina de statistici/informații
  else {
    progress();  // Afișează animația de progres
  }
}
else if (page == 2 || page == 3) {
  display.clearDisplay();  // Curăță ecranul pentru paginile 2 și 3 (posibil pagini goale sau rezervate)
  display.display();
}
// Actualizează temperatura în Serial Monitor la fiecare 10 secunde
if (currentMillis - lastTempCheck >= 10000) {
  Serial.println(temperature);
  lastTempCheck = currentMillis;
}
}

void idle() {
  unsigned long currentTime = millis();  // Obține timpul curent în milisecunde

  // Verifică dacă a trecut timpul necesar pentru a schimba cadrul curent
  if (currentTime - previousTime >= framestime[currentFrame]) {
    previousTime = currentTime;                                              // Actualizează timpul ultimei schimbări de cadru
    display.clearDisplay();                                                  // Curăță ecranul
    display.drawBitmap(0, 0, frames[currentFrame], 128, 64, SSD1306_WHITE);  // Desenează cadrul curent al animației
    display.display();                                                       // Actualizează ecranul OLED

    currentFrame++;  // Trece la următorul cadru
    if (currentFrame >= totalFrames) {
      currentFrame = 0;  // Resetează indexul cadrului dacă a ajuns la sfârșit
    }
  }
}
void cute() {
  unsigned long currentTime3 = millis();
  if (digitalRead(16) == LOW && apasat2 && (currentTime3 - lastButtonPress2 < 3000)) {
    buttonPressCount++;
    apasat2 = false;
    lastButtonPress2 = currentTime3;}
    if (buttonPressCount == 5) {
      Serial.println("da");
      buttonPressCount = 0;
    }
  
  if (digitalRead(16) == HIGH) 
    apasat2 = true;
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
