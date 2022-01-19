#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "Movistar";
const char* password =  "cielo2015";
const String APIcall = "https://api.openweathermap.org/data/2.5/onecall?lat=-34.826220000&lon=-58.393080000&exclude=minutely,hourly,daily,alerts&appid=8d42bc67176fa21cf2a5e3e7d9d6b196&units=metric";
 
void setup() {
Serial.begin(115200);
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.printf(".");}
Serial.println("Connected to the WiFi network");
}

void loop() {
 
  //if ((WiFi.status() == WL_CONNECTED)) {
    HTTPClient http;
    http.begin(APIcall); 
    int httpCode = http.GET();
    if (httpCode > 0) {
        String input = http.getString();
        //Serial.println(httpCode);
        //Serial.println(input);
      
DynamicJsonDocument doc(768);

deserializeJson(doc, input);
JsonObject obj = doc.as<JsonObject>();

float current_temp = obj["current"]["temp"];
int current_humidity = obj["current"]["humidity"];

Serial.println(current_temp);
Serial.println(current_humidity);
   }
    else {
      Serial.println("Error on HTTP request");
    }
    //http.end();
  delay(6000);}
