#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>  // v6.18.0

const char* SSID = "ZTE_2.4G_ExQCMa";                 // Your WiFi SSID
const char* PASSWORD = "NullReferenceException#123";  // Your WiFi Password

const int districtID = 1;          // District ID
const int minAge = 45;             // 45 or 18
const String date = "11-05-2021";  // Date for slot in DD-MM-YYYY format

const String baseURL = "http://arduino-cowin.centralindia.cloudapp.azure.com/?districtID=";
const String URL = baseURL + districtID + "&date=" + date + "&minAge=" + minAge;

int ledPin = LED_BUILTIN;

bool slotFound = false;

HTTPClient http;

void setupWiFi() {
  Serial.print("\nConnecting...");
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nConnected - ");
  Serial.println(WiFi.localIP());
}

void parseData(String jsonString) {
  DynamicJsonDocument doc(8192);

  DeserializationError error = deserializeJson(doc, jsonString);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  for (JsonObject center : doc.as<JsonArray>()) {
    Serial.print("Center Name: ");
    Serial.println(center["name"].as<String>());

    Serial.print("Center ID: ");
    Serial.println(center["center_id"].as<String>());

    Serial.print("Address: ");
    Serial.println(center["address"].as<String>());

    Serial.print("Block: ");
    Serial.println(center["block_name"].as<String>());

    Serial.print("District: ");
    Serial.println(center["district_name"].as<String>());

    Serial.print("State: ");
    Serial.println(center["state_name"].as<String>());

    Serial.print("Pincode Name: ");
    Serial.println(center["pincode"].as<int>());

    Serial.print("Center Name: ");
    Serial.println(center["name"].as<String>());

    Serial.print("From: ");
    Serial.println(center["from"].as<String>());

    Serial.print("To: ");
    Serial.println(center["to"].as<String>());

    Serial.print("Fee Type: ");
    Serial.println(center["fee_type"].as<String>());


    Serial.print("Sessions: ");
    for (JsonObject session : center["sessions"].as<JsonArray>()) {
      Serial.print("Date: ");
      Serial.println(session["date"].as<String>());

      Serial.print("Capacity: ");
      Serial.println(session["available_capacity"].as<String>());

      Serial.print("Age Limit: ");
      Serial.println(session["min_age_limit"].as<String>());

      Serial.print("Date: ");
      Serial.println(session["date"].as<String>());

      Serial.print("Vaccine: ");
      Serial.println(session["vaccine"].as<String>());

      Serial.print("First Dose Capacity: ");
      Serial.println(session["available_capacity_dose1"].as<String>());

      Serial.print("Second Dose Capacity: ");
      Serial.println(session["available_capacity_dose2"].as<String>());

      Serial.print("Slots: ");
      for (auto slot : session["slots"].as<String>()) {
        Serial.print(slot);
      }
      Serial.println("");
    }

    Serial.println("---------------------------------------------------------------");
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
    parseData(payload);
  }

  http.end();
}

void setup() {
  pinMode(ledPin, OUTPUT);

  Serial.begin(115200);
  setupWiFi();
}

void loop() {
  if (slotFound) {
    digitalWrite(ledPin, HIGH);
  } 
  pingServer();
  delay(5000); // IP Address will be blocked if you call API more than onece per 5 minute
}
