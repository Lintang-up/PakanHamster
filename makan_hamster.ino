#include <Servo.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

//ambil tempalte_id, device_ide dan token dari aplikasi blynk
#define BLYNK_TEMPLATE_ID "TMPL6ZA53JY4c"
#define BLYNK_TEMPLATE_NAME "anu"
#define BLYNK_AUTH_TOKEN "TMQEwnvB2QuhryCksLJVQtkz0fpavRqQ"

//libary ultrasonic
#define echoPin D7
#define trigPin D6
#ifndef STASSID
#define STASSID "sarahh"
#define STAPSK "12341234"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

long duration;
int distance; 
int jarak;

//include libary Blynk
#include <BlynkSimpleEsp8266.h>

//buat sebuah object servo
Servo myservo ;
void JarakPakan()
{
    long duration, distance, jarak;
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = (duration/2) / 29.1; //rumus menghitung sensor ultrasonic
    jarak = (15-distance);
    Serial.print("Distance: ");
    Serial.println(jarak);
    Serial.println(" %");
    Blynk.virtualWrite(V1, jarak);
    delay(500);
}

//variable menampung datastream statuspakan
int StatusPakan;

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {  // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  //pin yang digunakan untuk servo = D2 = 4
  myservo.attach(4);
  //posisi servo awal
  myservo.write(0);

  Serial.begin(115200);
  //Ultra
  pinMode(34, INPUT);
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT); 

  //koneksi ke Wifi
  WiFi.begin("Test","12345612");
  //uji konek atau tidak
  while(WiFi.status() !=WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }

  Serial.println("WiFi Terkoneksi");

  //apabila terkoneksi, koneksikan perangkat ke blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, "Test","12345612");

  Serial.println("Blynk Terkoneksi");
}

void BeriPakan()
{
      //proses perputaran pakan
  for(int posisi=0; posisi<=90; posisi++)
  {
    // set posisi servo
    myservo.write(posisi);
    delay(10);
  }
  delay(10);
  //proses perputaran t4 pakan ke posisi awal
  for(int posisi=90; posisi>=0; posisi--)
  {
    myservo.write(posisi);
    delay(10);
  }
}
void loop() {

  ArduinoOTA.handle();
  //jalankan aplikasi Blynk dari nodeMCU
  Blynk.run();
  JarakPakan();
  //tampilkan StatusPakan diserial monitor
  Serial.println("Status Pakan : " + String(StatusPakan));

  //jika StatusPakan = 1 maka diberi pakan
  if(StatusPakan == 1)
  {
    //panggil void beripakan, 1 Siklus perputaran Sampai kembali keposisi awal
    BeriPakan();
    //kembalikan StatusPakan menjadi 0 setelah diberipakan
    Blynk.virtualWrite(V0, 0);
    StatusPakan = 0;
    delay(10);
  }

}

//baca datastream StatusPakan
BLYNK_WRITE(V0)
{
  //baca datastream kemudian tampung kedalam statuspakan
  StatusPakan = param.asInt();
}
//baca datastream ultrasonic
BLYNK_WRITE(V1)
{
  jarak = param.asInt();
}
