#include <Arduino.h>
#include <DHT.h>

#define PIN_LDR        34  
#define PIN_PIR        27   
#define PIN_DHT        15  
#define PIN_LED_HALAMAN 26
#define PIN_LED_KM      25  
#define PIN_LED_KIPAS   33 

const int AMBANG_GELAP = 2000;            
const int HISTERESIS   = 200;            
const unsigned long DURASI_LAMPU_KM = 10000UL; 
const float AMBANG_SUHU = 23.0;          

const unsigned long INTERVAL_DHT = 2000UL; 
const unsigned long INTERVAL_LOG = 1000UL; 

DHT dht(PIN_DHT, DHT22);

bool lampuHalaman = false;
bool lampuKM      = false;
bool kipas        = false;
float suhu        = NAN;
unsigned long waktuGerakanTerakhir = 0;
unsigned long tDHT = 0, tLog = 0;

void setup() {
  Serial.begin(115200);
  pinMode(PIN_PIR, INPUT);
  pinMode(PIN_LED_HALAMAN, OUTPUT);
  pinMode(PIN_LED_KM, OUTPUT);
  pinMode(PIN_LED_KIPAS, OUTPUT);
  dht.begin();
  Serial.println("Smart Home siap...");
}

void loop() {
  unsigned long sekarang = millis();

  int nilaiLDR = analogRead(PIN_LDR);
  if (nilaiLDR > AMBANG_GELAP + HISTERESIS)      lampuHalaman = true;  
  else if (nilaiLDR < AMBANG_GELAP - HISTERESIS) lampuHalaman = false; 

  bool gerakan = (digitalRead(PIN_PIR) == HIGH);
  if (gerakan) {
    lampuKM = true;
    waktuGerakanTerakhir = sekarang;  
  } else if (lampuKM && (sekarang - waktuGerakanTerakhir >= DURASI_LAMPU_KM)) {
    lampuKM = false;                  
  }

  if (sekarang - tDHT >= INTERVAL_DHT) {
    tDHT = sekarang;
    float t = dht.readTemperature();
    if (!isnan(t)) {                  
      suhu = t;
      kipas = (suhu > AMBANG_SUHU);    
    }
  }

  digitalWrite(PIN_LED_HALAMAN, lampuHalaman);
  digitalWrite(PIN_LED_KM, lampuKM);
  digitalWrite(PIN_LED_KIPAS, kipas);

  if (sekarang - tLog >= INTERVAL_LOG) {
    tLog = sekarang;
    Serial.printf("LDR=%d | Gerakan=%s | Suhu=%.1f C | Halaman=%s | KM=%s | Kipas=%s\n",
                  nilaiLDR,
                  gerakan ? "ADA" : "TIDAK",
                  suhu,
                  lampuHalaman ? "ON" : "OFF",
                  lampuKM ? "ON" : "OFF",
                  kipas ? "ON" : "OFF");
  }
}