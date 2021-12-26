
#define BLYNK_PRINT Serial
#include <SPI.h>
#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>
#include "DHT.h"
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include "TRIGGER_WIFI.h"
#include "TRIGGER_GOOGLESHEETS.h"

#define DHTPIN D4
#define DHTTYPE DHT11
DHT dht11(DHTPIN, DHTTYPE);

//Configure Blynk
char auth[] = "bs8yZnDRaaEWgsDijAChXezZFuag74-O";
char ssid[]="V2040";
char pass[]="regzaa24";

// Configure Firebase
#define FIREBASE_HOST "tugasakhir-142eb-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "nVeLdqzKFX0p5e6u8RX80VY90bH7JKYwJUn4psdS"

// Configure Google Sheet
char column_name_in_sheets[ ][20] = {"value1","value2"};
String Sheets_GAS_ID = "AKfycbxFPcOBeU5oOK_LJPjoAUs6UWwhQbpw0rBQA8CzjEgkcBh5lp5caRSo-69fe7yj8mPPEA";
int No_of_Parameters = 2;

// Wifi
#define WIFI_SSID "V2040"
#define WIFI_PASSWORD "regzaa24"

FirebaseData firebaseData;

// Connect ke app blynk
SimpleTimer timer;
void sendSensor(){
  float h = dht11.readHumidity();
  float t = dht11.readTemperature();

  if (isnan(h) || isnan(t)){
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Blynk.virtualWrite(V1, h);
  Blynk.virtualWrite(V2, t);
}

void setup() {
  Serial.begin(115200);

  //Blynk
  dht11.begin();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while(WiFi.status()!= WL_CONNECTED){
    Serial.print(".");
    delay(400);
  }
  Serial.println();
  Serial.print("Connected with IP : ");
  Serial.println(WiFi.localIP());
  Serial.println();

  //Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Blynk.begin(auth, ssid, pass);
  dht11.begin();
  timer.setInterval(1000L, sendSensor);

  //Google Sheet
  while (!Serial);
  dht11.begin();
  WIFI_Connect("V2040","regzaa24");                                                    /*Provide you Wi-Fi SSID and password to connect to Wi-Fi*/
  Google_Sheets_Init(column_name_in_sheets, Sheets_GAS_ID, No_of_Parameters );
}

void loop() {
  float h = dht11.readHumidity();
  float t = dht11.readTemperature();

  //Blynk
  Blynk.run();
  timer.run();
  delay(1000);

  //Firebase
  if (isnan(h) || isnan(t)){
    Serial.println("Gagal membaca sensor DHT11");
    delay(1000);
    return;
  }

  Serial.print("Kelembaban : ");
  Serial.print(h);
  Serial.println();
  Serial.print("Suhu : ");
  Serial.print(t);
  Serial.println();
  
  if (Firebase.setFloat(firebaseData, "/Hasil_Pembacaan/kelembaban", h)){
    Serial.println("Kelembaban terkirim");
  }else{
    Serial.println("Kelembaban tidak terkirim");
    Serial.println("Karena : "+firebaseData.errorReason());
  }

  if (Firebase.setFloat(firebaseData, "/Hasil_Pembacaan/suhu", t)){
    Serial.println("Suhu terkirim");
  }else{
    Serial.println("Suhu tidak terkirim");
    Serial.println("Karena : "+firebaseData.errorReason());
  }

  //Google Sheet
  Data_to_Sheets(No_of_Parameters,  t,  h);
  Serial.println();
  delay(5000);                       
}
