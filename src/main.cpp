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
const char* mqttServer = "192.168.22.20";

const char* clientNAme = "swcameradaletto";
char* outTopic1 = "amdomus/basicio/swcameradaletto/SW1";
char* outTopic2 = "amdomus/basicio/swcameradaletto/SW2";

const char* infoTopic = "amdomus/basicio/swcameradaletto/info";
const char* outTopicTemplate = "amdomus/basicio/%s/SW%d";

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
int value = 0;
char msg[75];

const int button1 = D4;
char* button1Status;
long lastBtn1 = 0;
int lastBtn1State = 0;
boolean sendCommandSw1 = false;
long lenCommandSw1 = 0;

const int button2 = D3;
char* button2Status;
long lastBtn2 = 0;
int lastBtn2State = 0;
boolean sendCommandSw2 = false;
long lenCommandSw2 = 0;

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

  pinMode(button2, INPUT);
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

  Serial.println(outTopic1);
  delay(2000);
  Serial.println(outTopic2);
}

void reconnect() {
  // loop until reconnected
  while (!client.connected()) {
    Serial.print("Tentativo di connessione MQTT...");
    if (client.connect( clientNAme )) {  
      Serial.println("connesso");
      client.publish(infoTopic, "Hello world, I'm AM Domus Client");      
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
  int tmpButton2 = digitalRead(button2);

  if( tmpButton1 == 0 ){
    if( lastBtn1State == 0 ){      

      lenCommandSw1 = now;
      lastBtn1State = 1;
      Serial.println("Button 1 Pressed");
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

    
  if( tmpButton2 == 0 ){
    if( lastBtn2State == 0 ){      

      lenCommandSw2 = now;
      lastBtn2State = 1;
      Serial.println("Button 2 Pressed");
      sendCommandSw2 = true;      
    }        
  }else{
    if( sendCommandSw2 ){
      long delaySw2 = now - lenCommandSw2;
      if  ( delaySw2 < 1000 ){
        client.publish(outTopic2, "TOGGLE");
      }else{
        client.publish(outTopic2, "TOGGLE_DELAYED");
      }
      sendCommandSw2 = false;
    }
    lastBtn2State = 0;
    delay(100);
  }

}
