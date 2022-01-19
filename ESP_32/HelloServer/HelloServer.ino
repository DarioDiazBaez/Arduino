#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

const char* ssid = "Movistar";
const char* password = "cielo2015";
int h = 15;
float t = 25.4;

WebServer server(80);

void handleRoot() {
  server.send(200, "text/plain", "Temperatura: " + String(t) + " Humedad: " + String(h));
}

void setup(void) {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // CONECTAR
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.begin();
  }

void loop(void) {
  server.handleClient();
  delay(2);//allow the cpu to switch to other tasks
}
