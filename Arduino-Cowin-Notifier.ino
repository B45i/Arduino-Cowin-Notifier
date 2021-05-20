#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>  // v6.18.0
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <Fonts/FreeMono9pt7b.h>

const char* SSID = "ZTE_2.4G_ExQCMa";                 // Your WiFi SSID
const char* PASSWORD = "NullReferenceException#123";  // Your WiFi Password

const int districtID = 1;          // District ID
const int minAge = 45;             // 45 or 18
const String date = "11-05-2021";  // Date for slot in DD-MM-YYYY format

const String baseURL = "http://arduino-cowin.centralindia.cloudapp.azure.com/?districtID=";
const String URL = baseURL + districtID + "&date=" + date + "&minAge=" + minAge;

#define TFT_CS D2
#define TFT_RST D3
#define TFT_DC D4

HTTPClient http;
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);


int ledPin = LED_BUILTIN;

bool slotFound = false;

void setupWiFi() {
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(1);
  Serial.print("\nConnecting...");
  tft.print("\nConnecting...");

  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    tft.print(".");
    delay(500);
  }
  Serial.print("\nConnected - ");
  Serial.println(WiFi.localIP());
  tft.print("\nConnected - ");
  tft.println(WiFi.localIP());
}

void setupTFT() {
  tft.begin();
  tft.setCursor(0, 0);
  tft.setRotation(3);
  tft.setFont(&FreeMono9pt7b);
  tft.fillScreen(ILI9341_BLACK);
}

void printLabel(const String &s) {
    tft.setTextColor(ILI9341_YELLOW);
    tft.print(s);
    tft.setTextColor(ILI9341_WHITE);
}

void displayData(String jsonString) {
  DynamicJsonDocument doc(8192);

  DeserializationError error = deserializeJson(doc, jsonString);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  for (JsonObject center : doc.as<JsonArray>()) {
    tft.fillScreen(ILI9341_BLACK);
    tft.setTextColor(ILI9341_YELLOW);
    tft.setCursor(0, 0);

    tft.println("");
    tft.println(center["name"].as<String>());
    tft.setTextColor(ILI9341_WHITE);

    tft.println("");
    printLabel("Address: ");
    tft.println(center["address"].as<String>() + ",");
    tft.println(center["block_name"].as<String>() + ",");
    tft.println(center["pincode"].as<String>());

    tft.println("");
    printLabel("From: ");
    tft.println(center["from"].as<String>());

    tft.println("");
    printLabel("To: ");
    tft.println(center["to"].as<String>());

    tft.println("");
    printLabel("Fee Type: ");
    tft.println(center["fee_type"].as<String>());

    // Un-comment if you want to show these info on screen
    // tft.println("Sessions: ");
    // for (JsonObject session : center["sessions"].as<JsonArray>()) {
      // tft.print("Capacity: ");
      // tft.println(session["available_capacity"].as<String>());

      // tft.print("Vaccine: ");
      // tft.println(session["vaccine"].as<String>());

      // tft.print("Date: ");
      // tft.println(session["date"].as<String>());



      // tft.print("Age Limit: ");
      // tft.println(session["min_age_limit"].as<String>());



      // tft.print("First Dose Capacity: ");
      // tft.println(session["available_capacity_dose1"].as<String>());

      // tft.print("Second Dose Capacity: ");
      // tft.println(session["available_capacity_dose2"].as<String>());

      // tft.print("Slots: ");
      // for (auto slot : session["slots"].as<String>()) {
      //   tft.print(slot);
      // }
    // }
    delay(5000);
  }
}

void pingServer() {
  Serial.println(URL);
  Serial.println("Sending HTTP request to " + URL);
  http.begin(URL);
  int httpCode = http.GET();

  if (httpCode == 200) {
    Serial.println("Slot is avilable !!");
    slotFound = true;
    String payload = http.getString();
    displayData(payload);
  }

  http.end();
}

void setup() {
  pinMode(ledPin, OUTPUT);

  Serial.begin(115200);
  setupTFT();
  setupWiFi();
}

void loop() {
  if (slotFound) {
    digitalWrite(ledPin, HIGH);
  }
  pingServer();
  delay(5000);  // IP Address will be blocked if you call API more than onece per 5 minute
}
