//https://www.youtube.com/watch?v=_2gi9VHZ-q0&ab_channel=jadsatv
#include <WiFi.h>
#include "FirebaseESP32.h"

// Credenciales wifi
#define WIFI_SSID "Movistar"
#define WIFI_PASSWORD "cielo2015"

// Credenciales Proyecto Firebase
#define FIREBASE_HOST "https://esp32basedemo-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "URjaVukEdhrhIkaQoO2QckqXTJ2p5PRE9vPqtb9r"

// Firebase Data object
FirebaseData firebaseData;

// Si deseamos una ruta especifica String nodo = "/variables"; Firebase.getFloat(firebaseData, nodo + "/a");
bool iterar = true;


void setup() 
{
  Serial.begin(115200);
  Serial.println();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectado al Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
}


void loop() 
{
    while(iterar){
        // leer datos
        Firebase.getFloat(firebaseData, "/variables/a");
        Serial.println(firebaseData.floatData());
        delay(250);
        Firebase.getInt(firebaseData, "/variables/b");
        int b = firebaseData.intData();
        Serial.println(b);
        //Serial.println(firebaseData.intData());
        //getJSON getFloat getInt getString getBool
        
        // escribir datos
        Firebase.setBool(firebaseData, "/led1", true);       
        Firebase.setBool(firebaseData, "/led2", false);

        // push de datos
        Firebase.pushInt(firebaseData, "/variables/lecturas", 256);
        delay(100);
        Firebase.pushInt(firebaseData, "/variables/lecturas", 128);
        delay(100);
        Firebase.pushInt(firebaseData, "/variables/lecturas", 64);
        delay(100);
        Firebase.pushInt(firebaseData, "/variables/lecturas", 32);
        
        iterar = false; // no repite el while infinitamente
        Firebase.end(firebaseData);

    }
} // End Loop
