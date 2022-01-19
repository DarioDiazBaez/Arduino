  #include <WiFi.h>
  #include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);

String input1 = "Vacio";
String input2 = "Vacio";

const char* ssid = "Movistar";
const char* password = "cielo2015";

const char* PARAM_INPUT_1 = "input1";
const char* PARAM_INPUT_2 = "input2";

// Página web HTML
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>ESP Input Form</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <form action="/get">
    Control Temperatura: <input type="text" name="input1">
    <input type="submit" value="Enviar">
  </form><br>
  <form action="/get">
    Control Humedad: <input type="text" name="input2">
    <input type="submit" value="Enviar">
  </form><br>
    </form>
</body></html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void setup() {
  Serial.begin(115200);
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!");
    return;
  }
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Enviar codigo HTML
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
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
    }
    //GET input2
    else if (request->hasParam(PARAM_INPUT_2)) {
      Serial.println("input2 actualizado");
      inputMessage = request->getParam(PARAM_INPUT_2)->value();
      input2 = inputMessage;
    }
    else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    
  Serial.println(input1);
  Serial.println(input2);

//Volver al inicio  
  request->send(200, "text/html", "HTTP GET request sent to your ESP on input field (" 
                                     + inputParam + ") with value: " + inputMessage +
                                     "<br><a href=\"/\">Return to Home Page</a>");
  });
  server.onNotFound(notFound);
  server.begin();
}

void loop() {
}
