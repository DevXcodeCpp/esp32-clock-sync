#include <WiFi.h>
#include <time.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 32
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1 );

#define butPin 1
#define disOnSec 3


const char* ssid = "x"; 
const char* password = "x";

const char* ntpServer = "time.nist.gov"; 
// Смещение в секундах. Например, для Москвы (UTC+3) это 3 * 3600 = 10800
const long  gmtOffset_sec = 10800; // Здесь: UTC+3 часа (например, Москва, Киев летом)
const int   daylightOffset_sec = 0; // Смещение для летнего времени (если не используется, оставьте 0)

byte h;
byte m;
byte s;

uint64_t tmr = 0;
uint64_t tmr1 = 0;
struct tm timeinfo;


byte disOn = 0;

void timeSync() {
  Serial.printf("Подключение к Wi-Fi сети: %s\n", ssid);
  WiFi.begin(ssid, password);
   while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Wi-Fi подключен");
  Serial.print("IP адрес: ");
  Serial.println(WiFi.localIP());
   Serial.println("Инициализация NTP...");
  // configTime(смещение_GMT, смещение_летнего_времени, сервер_NTP)
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  // Ожидание завершения синхронизации
  if(time(nullptr) < 100000) { // Проверяем, что время не равно 0
    Serial.print("Ожидание синхронизации времени");
    int i = 0;
    while (time(nullptr) < 100000 && i < 20) { // Максимум 10 секунд
      delay(500);
      Serial.print(".");
      i++;
    }
    Serial.println();
  }
  if(!getLocalTime(&timeinfo)){
    Serial.println("❌ Не удалось получить время");
    return;
  }
  h = (byte)timeinfo.tm_hour;
  m = (byte)timeinfo.tm_min;
  s = (byte)timeinfo.tm_sec;

  Serial.println("Выключаем WIFI...");
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  if (WiFi.getMode() == WIFI_MODE_NULL) {
    Serial.println("WIFI успешно выключен");
  }else{
    Serial.println(WiFi.status());
    Serial.println(WiFi.getMode());
  }
}


void timeTick() {
  s++;
  if (s >= 60) {
    s = 0;
    m++;
    if (m >= 60) {
      m = 0;
      h++;
      if (h >= 24) h = 0;
    }
  }
  return;
}

void setup() {
  pinMode(butPin, INPUT);
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.clearDisplay();
  display.display();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  //display.setBrightness(128);
  display.setCursor(0, 0);

  Serial.begin(115200);
  delay(100);
  timeSync();
}

void loop() {
  if (millis()-tmr1 >= 100) {
    tmr1 = millis();
    if (digitalRead(butPin)) {
      printLocalTime();
      disOn = disOnSec;
    }
  }
  if (millis()-tmr >= 1000) {
    tmr = millis();
    timeTick();
    if (disOn > 0) {
      disOn--;
      printLocalTime();
    }
    if (disOn <= 0) {
      display.clearDisplay();
      display.display();
    }
  }
}





void printLocalTime(){ 
  Serial.println("Текущее время: ");
  Serial.print(h);
  Serial.print(":");
  Serial.print(m);
  Serial.print(":");
  Serial.print(s);

  display.clearDisplay();
  if (h<10) {
    display.setCursor(0, 0);
    display.print(0);
    display.setCursor(12,0);
    display.print(h);
  }else {
    display.setCursor(0, 0);
    display.print(h);
  }


  display.setCursor(24, 0);
  display.print(":");


  
  if (m<10) {
    display.setCursor(36, 0);
    display.print(0);
    display.setCursor(48,0);
    display.print(m);
  }else {
    display.setCursor(36, 0);
    display.print(m);
  }


  display.setCursor(60, 0);
  display.print(":");


  if (s<10) {
    display.setCursor(72,0);
    display.print(0);
    display.setCursor(84,0);
    display.print(s);
  }else {
    display.setCursor(72,0);
    display.print(s);
  }
  display.display();
}
