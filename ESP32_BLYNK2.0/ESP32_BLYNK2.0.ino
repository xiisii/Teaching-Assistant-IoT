/* --- WIFI --- */
#include <WiFi.h>
#include <WiFiClient.h>
#define WIFI_SSID "XiiSii"
#define WIFI_PASSWORD "hehehehe"

/* --- BLYNK --- */
#define BLYNK_FIRMWARE_VERSION "0.1.0"
#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL6wAAVX-WN"
#define BLYNK_TEMPLATE_NAME "Mai Thy"
#define BLYNK_AUTH_TOKEN "lk4ABIoZY7601P88CsuTYnN3jsUiMu6k"
#include <BlynkSimpleEsp32.h>
BlynkTimer timer;

/* --- DHT11 --- */
#include <DHT.h>
#define DHTPIN 15    
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);

/* --- BUTTON --- */
#define BUTTON 12    

/* --- LED --- */
#define LED 5 
WidgetLED LED_CONNECT(V5);      

/* --- SENSOR --- */
#define SENSOR 13  

/* --- SERVO --- */
#include <Servo.h>
#define SERVO_PIN 14
Servo servo;

BLYNK_WRITE(V0) {
  int value = param.asInt();
  value ? digitalWrite(LED, HIGH) : digitalWrite(LED, LOW);
}

BLYNK_WRITE(V4) {
  int s0 = param.asInt(); 
  servo.write(s0);
}

void SendData() {
  float h = dht.readHumidity();
  float t = dht.readTemperature(); 
  int b = digitalRead(BUTTON);
  int s = digitalRead(SENSOR);

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  if (isnan(b)) {
    Serial.println("Failed to read from BUTTON!");
    return;  
  }
  if (isnan(s)) {
    Serial.println("Failed to read from SENSOR!");
    return; 
  }

  if( s == LOW ) {
    Blynk.virtualWrite(V1, " Co vat can ");
  }
  else {
    Blynk.virtualWrite(V1, " Khong co vat can ");
  }

  if( b == HIGH ) {
    LED_CONNECT.on();
  }
  else {
    LED_CONNECT.off();
  }

  Blynk.virtualWrite(V2, t);
  Blynk.virtualWrite(V3, h);
}

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(BUTTON, INPUT);
  pinMode(SENSOR, INPUT);
  dht.begin();
  servo.attach(SERVO_PIN);

  Serial.begin(9600);
  delay(1000);

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
  
  Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASSWORD);
  timer.setInterval(100L, SendData);
}

void loop() {
  Blynk.run(); 
  timer.run();
}
