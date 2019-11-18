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


const char* ssid = "AMDomus";
const char* password = "AleMar7981";
const char* mqttServer = "192.168.1.121";

//swloginlogout
//swcomandoesternocucina
//swscaleinterne

const char* clientNAme = "gestioneevprimopriano";
char* outTopic1 = "amdomus/basicio/swloginlogout/SW1";
char* outTopic2 = "amdomus/basicio/swloginlogout/SW2";

const char* ledATopic = "amdomus/basicio/test/ledA";
const char* ledBTopic = "amdomus/basicio/test/ledB";
const char* infoTopic = "amdomus/basicio/test/info";
const char* outTopicTemplate = "amdomus/basicio/%s/SW%d";

const char* cameraDaLettoTopic = "amdomus/clima/primopiano/ev/cameradaletto";
const char* camerettaTopic = "amdomus/clima/primopiano/ev/cameretta";
const char* bagnopiccoloTopic = "amdomus/clima/primopiano/ev/bagnopiccolo";

const char* cucinaTopic = "amdomus/clima/primopiano/ev/cucina";
const char* salaTopic = "amdomus/clima/primopiano/ev/sala";

const char* zonaingressoTopic = "amdomus/clima/primopiano/ev/zonaingresso";




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

const int ledA = D2;
const int ledB = D1;


void mqttCallback(char* topic, byte* payload, unsigned int length) {
  // Gestione MQTT in ingresso
  Serial.print("Ricevuto messaggio [");
  Serial.print(topic);
  Serial.print("] ");

  int test = strcmp(topic, cameraDaLettoTopic);
  int test2 = strcmp(topic, cucinaTopic);
  Serial.println(test);
  Serial.println(test2);
  Serial.println(topic);
  Serial.print(" --- ");

  String message;

  for (int i = 0; i < length; i++) {
    char c = (char)payload[i]; 
    message += c;
  }
  Serial.println(message);

  //Camera Da letto
  if( strcmp(topic, cameraDaLettoTopic ) ==0 ) {
    Serial.println("Camera Da Letto");
    if( message == "ON" ){
      digitalWrite( D8, LOW  );
    }else if( message == "OFF" ){
      digitalWrite( D8, HIGH  );
    }
  }

  // Cameretta
  if( strcmp(topic, camerettaTopic ) == 0 ){
    Serial.println("Cameretta");
    if( message == "ON" ){
      digitalWrite( D7, LOW  );
    }else if( message == "OFF" ){
      digitalWrite( D7, HIGH  );
    }
  }

  // Bagno piccolo
  if( strcmp(topic, bagnopiccoloTopic ) == 0 ){
    Serial.println("Bagno Piccolo");
    if( message == "ON" ){
      digitalWrite( D6, LOW  );
    }else if( message == "OFF" ){
      digitalWrite( D6, HIGH  );
    }
  }

  // Cucina
  if( strcmp(topic, cucinaTopic ) == 0 ){
    Serial.println("Cucina");
    if( message == "ON" ){
      digitalWrite( D5, LOW  );
    }else if( message == "OFF" ){
      digitalWrite( D5, HIGH  );
    }
  }

  // Sala
  if( strcmp(topic, salaTopic ) == 0 ){
    Serial.println("Sala");
    if( message == "ON" ){
      digitalWrite( D0, LOW  );
    }else if( message == "OFF" ){
      digitalWrite( D0, HIGH  );
    }
  }

  // Zona ingresso
  if( strcmp(topic, zonaingressoTopic )  == 0 ){
    Serial.println("Ingresso");
    if( message == "ON" ){
      digitalWrite( D3, LOW  );
    }else if( message == "OFF" ){
      digitalWrite( D3, HIGH  );
    }
  }

}


void setup() {

  Serial.begin(115200);

  pinMode(D0, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
  pinMode(D8, OUTPUT);
  
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

  delay(2000);

}

void reconnect() {
  // loop until reconnected
  while (!client.connected()) {
    Serial.print("Tentativo di connessione MQTT...");
    if (client.connect( clientNAme )) {  
      Serial.println("connesso");
      client.subscribe(cameraDaLettoTopic);
      delay(100);
      client.subscribe(camerettaTopic);
      delay(100);
      client.subscribe(bagnopiccoloTopic);
      delay(100);
      client.subscribe(cucinaTopic);
      delay(100);
      client.subscribe(salaTopic);
      delay(100);
      client.subscribe(zonaingressoTopic);
      delay(100);
      //client.publish(infoTopic, "Hello world, I'm AM Domus Client");
        
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
  if (now - lastMsg > 30000) {
    lastMsg = now;
    ++value;
    client.publish(infoTopic, "online");
  }

}
