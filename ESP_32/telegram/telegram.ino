//https://randomnerdtutorials.com/telegram-control-esp32-esp8266-nodemcu-outputs/#demonstration
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>   // Universal Telegram Bot Library written by Brian Lough: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <ArduinoJson.h>

const char* ssid = "Movistar";
const char* password = "cielo2015";

#define BOTtoken "2107691394:AAGdY0pRld6BMR0jpNTJLtnXo8topgiA0mk"
#define CHAT_ID "2104752846"

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Checks for new messages every 1 second.
int botRequestDelay = 500;
unsigned long lastTimeBotRan;

//led
const int ledPin = 2;
bool ledState = LOW;

//variables
float a = 45.6;
int b = 15;

// La función maneja lo que sucede cuando llegan nuevos mensajes.
void handleNewMessages(int numNewMessages) {
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "usuario no autorizado", "");
      continue;
    }
    
    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String welcome = "Bienvenido, " + from_name + ".\n";
      welcome += "Informacion\n";
      welcome += "escrive un texto\n";
      welcome += "/variable Info de los comandos etc \n";
      bot.sendMessage(chat_id, welcome, "");
    }
    
    if (text == "/gpio") {
      if (ledState == HIGH){
        ledState = LOW;
        bot.sendMessage(chat_id, "LED OFF", "");
        }
      else{
        ledState = HIGH;
        bot.sendMessage(chat_id, "LED ON", "");
        }
      digitalWrite(ledPin, ledState);
    }
    
    if (text == "/estado") {
      if (digitalRead(ledPin)){
        bot.sendMessage(chat_id, "LED ON", "");
      }
      else{
        bot.sendMessage(chat_id, "LED OFF", "");
      }
    }

    if (text == "/variable") {
    String variable;
      variable += "variable a \n";
      variable += String(a);
      variable += "\n";
      variable += " variable b \n";
      variable += String(b);
      bot.sendMessage(chat_id, variable, "");
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);
  
  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  #ifdef ESP32
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  #endif
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
}

void loop() {
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}
