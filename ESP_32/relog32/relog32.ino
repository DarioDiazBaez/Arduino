#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Wire.h>

#define I2C_SCL 22
#define I2C_SDA 23
#define COLUMNAS_LCD 16
#define FILAS_LCD 2
#define DIRECCION_LCD 0x27
LiquidCrystal_I2C lcd(DIRECCION_LCD, COLUMNAS_LCD, FILAS_LCD);

const char* ssid     = "Movistar";
const char* password = "cielo2015";

// Define NTP Client
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Variables
String formato;
String fecha;
String hora;

void setup() {
  Serial.begin(115200);
//WIFI  
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
// Iniciar NTPClient
  timeClient.begin();
//Ajustar zona horaria
  // GMT +8 = 28800
  // GMT +1 = 3600
  // GMT 0 = 0
  // GMT -1 = -3600
  // GMT -3 = -10800 argentina
  timeClient.setTimeOffset(-10800);
lcd.begin(I2C_SDA, I2C_SCL);
lcd.backlight();
//lcd.noBacklight();
}
void loop() {

fechayhora();

pantalla();

delay(100);
}


void fechayhora(){  
//verificar la obtencion de datos  
while(!timeClient.update()) {
timeClient.forceUpdate();}
//el formato es el siguiente 2021-11-10T16:00:13Z
formato = timeClient.getFormattedDate();
Serial.println(formato);

//se necesita extraer por separado fecha y hora
// Extraer fecha
int splitT = formato.indexOf("T");
fecha = formato.substring(0, splitT);
Serial.print("Fecha: ");
Serial.println(fecha);
// Extraer hora
hora = formato.substring(splitT+1, formato.length()-1);
Serial.print("Hora: ");
Serial.println(hora);}

void pantalla(){
lcd.setCursor(0,0);
lcd.print("Fecha");
lcd.setCursor(6,0);
lcd.print(fecha);

lcd.setCursor(0,1);
lcd.print("Hora ");
lcd.setCursor(6,1);
lcd.print(hora);
}
