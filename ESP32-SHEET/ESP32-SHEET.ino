/* --- WIFI --- */
#include <WiFi.h>
#include <WiFiClient.h>
#define WIFI_SSID "XiiSii"
#define WIFI_PASSWORD "hehehehe"

#include <SPI.h>
#include <MFRC522.h>
#include <HTTPClient.h>

const String sheetURL = "https://script.google.com/macros/s/AKfycbwbqGHVBzgf7rWxyyz3NVLc2KtrXRflwV-QrGEZjAifBioRKGZ7rMZofWMDw2_52JXG/exec";

int blocks[] = {4,5,6,8,9,10};
#define blocks_len  (sizeof(blocks) / sizeof(blocks[0]))

#define RST_PIN  22
#define SS_PIN   21

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;  
MFRC522::StatusCode status;      


int blockNum = 2;  


byte bufferLen = 18;
byte readBlockData[18];

void setup()
{
  Serial.begin(115200);

  Serial.println();
  Serial.print("Connecting to AP");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(200);
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  SPI.begin();
}

 void loop()
{
  mfrc522.PCD_Init();
  if ( ! mfrc522.PICC_IsNewCardPresent()) {return;}
  if ( ! mfrc522.PICC_ReadCardSerial()) {return;}
  Serial.println(F("Reading last data from RFID..."));  

  String fullURL = "", temp;
  for (byte i = 0; i < blocks_len; i++) {
    ReadDataFromBlock(blocks[i], readBlockData);
    if(i == 0){
      temp = String((char*)readBlockData);
      temp.trim();
      fullURL = "name" + String(i) + "=" + temp;
    }
    else{
      temp = String((char*)readBlockData);
      temp.trim();
      fullURL += "&name" + String(i) + "=" + temp;
    }
  }
  
  fullURL.trim();
  fullURL = sheetURL + "?" + fullURL;
  fullURL.trim();
  Serial.println(fullURL);
  delay(200);

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient https;
    Serial.print(F("[HTTPS] begin...\n"));

    if (https.begin(fullURL)){
      Serial.print(F("[HTTPS] GET...\n"));
      int httpCode = https.GET();
      if (httpCode > 0) {
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
      }

      else 
      {Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());}

      https.end();
      delay(1000);
    }
    else {
      Serial.printf("[HTTPS} Unable to connect\n");
    }
  }
}


void ReadDataFromBlock(int blockNum, byte readBlockData[]) 
{ 
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));

  if (status != MFRC522::STATUS_OK){
     Serial.print("Authentication failed for Read: ");
     Serial.println(mfrc522.GetStatusCodeName(status));
     return;
  }

  else {
    Serial.println("Authentication success");
  }
 
  status = mfrc522.MIFARE_Read(blockNum, readBlockData, &bufferLen);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Reading failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  else {
    readBlockData[16] = ' ';
    readBlockData[17] = ' ';
    Serial.println("Block was read successfully");  
  }
}