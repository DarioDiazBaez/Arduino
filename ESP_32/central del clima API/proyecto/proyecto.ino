/******   LIBRERIAS   *********/

#include <ThingSpeak.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <EEPROM.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

AsyncWebServer server(80);
WiFiClient client;

/******   CONSTANTES   *********/

// Pines para pulsadores de setpoints de humedad y temperatura
#define PIN_HUMEDAD_UP 32
#define PIN_HUMEDAD_LOW 33
#define PIN_TEMPERATURA_UP 34
#define PIN_TEMPERATURA_LOW 35
// Pines salida de relay
#define PIN_RELAY_C 0
#define PIN_RELAY_A 15

/******   LDC Y DHT11   *********/

//DHT11
#define PIN_DHT 25// PIN SENSOR DHT11
#define DHTTYPE DHT11//MODELO DEL SENSOR
DHT dht(PIN_DHT, DHTTYPE);
//LCD
#define COLUMNAS_LCD 16
#define FILAS_LCD 2
#define DIRECCION_LCD 0x27
LiquidCrystal_I2C lcd(DIRECCION_LCD, COLUMNAS_LCD, FILAS_LCD);

/*********   I2C   ************/

#define I2C_SCL 21
#define I2C_SDA 22

/******   VARIABLES   *********/

char respuesta[]= {""};
String input1 = "-";//Temperatura
String input2 = "-";//Humedad
const char* PARAM_INPUT_1 = "input1";
const char* PARAM_INPUT_2 = "input2";
const char* ssid = "Movistar";
const char* password = "cielo2015";
unsigned long channelID = 1553262;
const char* WriteAPIKey = "KXP87ULQAWI9WSWY"; 
const String APIcall = "https://api.openweathermap.org/data/2.5/onecall?lat=-34.826220000&lon=-58.393080000&exclude=minutely,hourly,daily,alerts&appid=8d42bc67176fa21cf2a5e3e7d9d6b196&units=metric";
float current_temp;
int current_humidity;
int ct;//control temperatura--------------------------variable a controlar
int ch;//control humedad------------------------------variable a controlar
int h;
float t;
/*
  byte grados[8] = {
  0b01110,
  0b01010,
  0b01110,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000};//Caracter "°"
*/

// Página web HTML
// <meta HTTP-EQUIV="Refresh" CONTENT="1">
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
<title>ESP 32</title>
<meta charset="utf-8">
</head>
<body>
<center>
<h1>ESP32 DHT11</h1>
<form action=/get>
Control Temperatura: <input type="number" min="0" max="100" name="input1">
<input type="submit">
</form>
<br>
<form action=/get>
Control Humedad: <input type="number" min="0" max="100" name="input2">
<input type="submit">
</form>
<br>
)rawliteral";


 const char index_html_footer[] PROGMEM = R"rawliteral(
</center>
</body>
</html>)rawliteral";

void setup() {
   Serial.begin(115200);
   
   Wifi();

   ThingSpeak.begin(client);
   
   dht.begin();
   
   lcd.begin(COLUMNAS_LCD, FILAS_LCD);
   lcd.backlight();
   //lcd.createChar(1, grados);//Caracter "°"
   
   Wire.begin(I2C_SDA, I2C_SCL);
   
   EEPROM.begin(10);//tamaño
   ct = EEPROM.read(0);
   ch = EEPROM.read(1);
   
   pinMode(PIN_HUMEDAD_UP,INPUT);//pulsador
   pinMode (PIN_HUMEDAD_LOW,INPUT);//pulsador
   pinMode(PIN_TEMPERATURA_UP,INPUT);//pulsador
   pinMode(PIN_TEMPERATURA_LOW,INPUT);//pulsador
   pinMode(PIN_RELAY_C,OUTPUT);//relay
   pinMode(PIN_RELAY_A,OUTPUT);//relay
   
   HTML();
 }  

void loop() { 

    api();

    thingspeak();
  
    eeprom();
   
    ActualizarLCD();
    
    LeerEntradas();
    
    LeerSensores();

    ActualizarSalidas(); 

delay(100);
}

void api(){
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

current_temp = obj["current"]["temp"];
current_humidity = obj["current"]["humidity"];

//Serial.println(current_temp);
//Serial.println(current_humidity);
   }
    else {
      Serial.println("Error on HTTP request");
    }
    //http.end();
}

void thingspeak(){
  ThingSpeak.setField (1,t);
  ThingSpeak.setField (2,h);
  ThingSpeak.writeFields(channelID,WriteAPIKey);
}

void eeprom(){
if(EEPROM.read(0) != ct){EEPROM.write(0,ct);EEPROM.commit();}
if(EEPROM.read(1) != ch){EEPROM.write(1,ch);EEPROM.commit();}
}

void ActualizarLCD(){
  lcd.setCursor(0,0);
  lcd.print("T:");
  lcd.setCursor(2,0);
  lcd.print(t,1);
  lcd.print((char)223);
  //lcd.write(byte(1));
  lcd.print("C ");
  lcd.setCursor(9,0);
  lcd.print("CT:");
  lcd.setCursor(12,0);
  lcd.print(ct);
  lcd.print((char)223);
  //lcd.write(byte(1));
  lcd.print("C         ");
  
  lcd.setCursor(0,1);
  lcd.print("H:");
  lcd.setCursor(2,1);
  lcd.print(h);
  lcd.print("%    ");
  lcd.setCursor(9,1);
  lcd.print("CH:");
  lcd.setCursor(12,1);
  lcd.print(ch);
  lcd.print("%  ");
}

void LeerEntradas() {
  // Lectura de pulsadores para set points
    if (digitalRead(PIN_TEMPERATURA_UP) == HIGH){ct++; delay(100);}
    if (digitalRead(PIN_TEMPERATURA_LOW) == HIGH){ct--; delay(100);}
    if (digitalRead(PIN_HUMEDAD_UP) == HIGH){ch++; delay(100);}
    if (digitalRead(PIN_HUMEDAD_LOW) == HIGH){ch--; delay(100);}
    if (ct >= 101){ct = 0;}
    if (ct <= -1){ct = 100;}
    if (ch >= 101){ch = 0;}
    if (ch <= -1){ch = 100;}

}

void LeerSensores() {
    t = dht.readTemperature();//temperatura en grados centígrados
    h = dht.readHumidity();//humedad relativa
  
}

void ActualizarSalidas() {
   if (h>=ch){digitalWrite(PIN_RELAY_C,LOW);}
    else{digitalWrite(PIN_RELAY_C,HIGH);}

   if (t>=ct){digitalWrite(PIN_RELAY_A,LOW);}
    else{digitalWrite(PIN_RELAY_A,HIGH);}

   if (isnan(h) || isnan(t)){//error en la lectura
          digitalWrite(PIN_RELAY_C,LOW);
          digitalWrite(PIN_RELAY_A,LOW);
  }
}

/******   WIFI   *********/

void Wifi() {

   WiFi.mode(WIFI_STA);
   WiFi.begin(ssid, password);

   while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
   Serial.println("");
   //ip fija
//   IPAddress ip(192,168,10,50); 
//   IPAddress gateway(192,168,1,33); 
//   IPAddress subnet(255,255,255,0); 
//   WiFi.config(ip, gateway, subnet); 
   Serial.print("Connected to ");
   Serial.println(ssid);
   Serial.print("IP address: ");
   Serial.println(WiFi.localIP());
   server.begin();
   server.onNotFound(notFound);   
}

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found No encontrado");
}
void HTML() {
// Enviar codigo HTML
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){

   strcat (respuesta,index_html);
   strcat (respuesta,"<table>");
   strcat (respuesta,"<tr>");
   strcat (respuesta,"<td>Temperatura</td>");
   strcat (respuesta,"<td>");
   strcat (respuesta,String(t).c_str());
   strcat (respuesta," °C");
   strcat (respuesta,"</td>");
   strcat (respuesta,"</tr>");

   strcat (respuesta,"<tr>");
   strcat (respuesta,"<td>Humedad</td>");
   strcat (respuesta,"<td>");
   strcat (respuesta,String(h).c_str());
   strcat (respuesta," %");
   strcat (respuesta,"</td>");
   strcat (respuesta,"</tr>");

   strcat (respuesta,"<tr>");
   strcat (respuesta,"<td>Temperatura Actual Bs.As </td>");
   strcat (respuesta,"<td>");
   strcat (respuesta,String(current_temp).c_str());
   strcat (respuesta," °C");
   strcat (respuesta,"</td>");
   strcat (respuesta,"</tr>");

   strcat (respuesta,"<tr>");
   strcat (respuesta,"<td>Humedad Actual Bs.As </td>");
   strcat (respuesta,"<td>");
   strcat (respuesta,String(current_humidity).c_str());
   strcat (respuesta," %");
   strcat (respuesta,"</td>");
   strcat (respuesta,"</tr>");
   
   strcat (respuesta,"</table>");

   strcat (respuesta,index_html_footer);

   request->send(200, "text/html", respuesta);
});

//Envíar una solicitud GET
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;
    Serial.println(inputMessage);
   
//GET input1
    if (request->hasParam(PARAM_INPUT_1)) {
      Serial.println("input1 actualizado");
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      input1 = inputMessage;
      ct = input1.toInt(); 
    }
//GET input2
    if (request->hasParam(PARAM_INPUT_2)) {
      Serial.println("input2 actualizado");
      inputMessage = request->getParam(PARAM_INPUT_2)->value();
      input2 = inputMessage;
      ch = input2.toInt();
    }
    
  Serial.println(input1);
  Serial.println(input2);

//Volver al inicio  
  request->send(200, "text/html", "Solicitud HTTP GET enviada con valor:" + inputMessage + "<br><a href=\"/\">Volver</a>");
  });}
