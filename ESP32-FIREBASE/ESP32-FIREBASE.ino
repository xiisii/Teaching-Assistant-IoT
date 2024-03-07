/* --- WIFI --- */
#include <WiFi.h>
#include <WiFiClient.h>
#define WIFI_SSID "XiiSii"
#define WIFI_PASSWORD "hehehehe"

/* --- FIREBASE --- */
#include <FirebaseESP32.h>
#define FIREBASE_HOST "https://esp32-43232-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "AIzaSyBrOflZYKSxALvWY1eUKMZpoJRExjGld7o"
FirebaseData firebaseData;

/* --- SERVO --- */
#include <Servo.h>
#define SERVO_PIN 14
Servo servo;

/* --- PIR --- */
#include <Adafruit_Sensor.h>
#define PIR_PIN 2
int lastPirState = LOW;

void sendDataToFirebase(int s, int p) {
  Firebase.setInt(firebaseData, "/input", s);
  if (p != lastPirState) {
    if (p == HIGH) {
      Firebase.setBool(firebaseData, "/motion", true);
      Serial.println("true");
    } 
    else if( p == LOW ) {
      Firebase.setBool(firebaseData, "/motion", false);
      Serial.println("false");
    }
    lastPirState = p;
  }
}



void setup() {
  Serial.begin(9600);
  servo.attach(SERVO_PIN);
  pinMode(PIR_PIN, INPUT);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
}

void loop() {
  int s = servo.read();
  int p = digitalRead(PIR_PIN);
  sendDataToFirebase(s,p);
  Firebase.setBool(firebaseData, "/motion", false);
  }