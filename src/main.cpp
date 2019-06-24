/*
 * Basic I/O MQTT
 * 
 * Alessandro Lanni
 * 24/06/2019
 */

#include <Arduino.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>


const char* ssid = "HOMEAUTOEXT";
const char* password = "homeautonet";

const char* clientNAme = "swcameradaletto";
const char* mqttServer = "192.168.22.20";
const char* outTopic1 = "amdomus/basicio/swcameradaletto/SW1";
const char* inTopic = "amdomus/basicio/swcameradaletto/command";
const char* infoTopic = "amdomus/basicio/swcameradaletto/info";

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
int value = 0;
char msg[75];

const int button1 = D1;
char* button1Status;
long lastBtn1 = 0;
int lastBtn1State = 0;
boolean sendCommandSw1 = false;
long lenCommandSw1 = 0;

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  // Gestione MQTT in ingresso
  Serial.print("Ricevuto messaggio [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}


void setup() {

  Serial.begin(115200);

  pinMode(button1, INPUT);
  button1Status = "ON";

  // inizializzo la parte WifiWiFi.mode(WIFI_STA);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.println("Connessione alla WIFI" );

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connesso A ");
  Serial.println(ssid);
  Serial.print("IP : ");
  Serial.println(WiFi.localIP());

  // Collegamento a MQTT
  client.setServer(mqttServer, 1883);
  client.setCallback(mqttCallback);
  

}

void reconnect() {
  // loop until reconnected
  while (!client.connected()) {
    Serial.print("Tentativo di connessione MQTT...");
    if (client.connect( clientNAme )) {  
      Serial.println("connesso");
      client.publish(infoTopic, "Hello world, I'm AM Domus Client");
      client.subscribe(inTopic);
    } else {
      Serial.print("Fallito, rc=");
      Serial.print(client.state());
      Serial.println(" tentativo in 5 secondi");
      delay(5000);
    }
  }
}

void loop() {
  // Gestione riconnessione a MQTT

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    client.publish(infoTopic, "online");
  }

  int tmpButton1 = digitalRead(button1);  

  if( tmpButton1 == 1 ){
    if( lastBtn1State == 0 ){      

      lenCommandSw1 = now;
      lastBtn1State = 1;
      Serial.println("Button 1 Pressed");
      Serial.println(lastBtn1State);
      if( button1Status == "ON" ){
        button1Status = "OFF";      
      }else{
        button1Status = "ON";
      }      
      sendCommandSw1 = true;      
    }        
  }else{

    if( sendCommandSw1 ){
      long delaySw1 = now - lenCommandSw1;
      if  ( delaySw1 < 1000 ){
        client.publish(outTopic1, "TOGGLE");
      }else{
        client.publish(outTopic1, "TOGGLE_DELAYED");
      }
      sendCommandSw1 = false;
    }

    lastBtn1State = 0;    
    delay(100);
  }

}
