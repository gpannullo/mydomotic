#include <SPI.h>
#include <Ethernet.h>
#include "MyDomotic.h"
#include <PubSubClient.h>

// IP DEL SERVER MQTT
IPAddress server_mqtt(192, 168, 10, 92);
//MAC ADDRESS DELLA SCHEDA ETHERNET
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xEE };

//DEFINIZIONE DEI PIN E DEI LORO COMPORTAMENTI
MyDomotic mydomotic_obj [] {
  MyDomotic(40, 13),            //ACCENSIONE LUCI (type SWITCH)
  MyDomotic(30, 12, 5000),      //ACCENSIONE LUCI (type SWITCH) a tempo
  MyDomotic(50, 51, 5000, 21),  //SALITA PERSIANA (type BLIND)
  MyDomotic(50, 23, 5000, 25),
  MyDomotic(50, 27, 5000, 29),
  MyDomotic(50, 31, 5000, 33),
  MyDomotic(50, 35, 5000, 37),
  MyDomotic(50, 39, 5000, 41),
  MyDomotic(50, 43, 5000, 45),
  MyDomotic(50, 47, 5000, 49),
  MyDomotic(20, 21, 5000, 51),
  MyDomotic(20, 25, 5000, 23),
  MyDomotic(20, 29, 5000, 27),
  MyDomotic(20, 33, 5000, 31),
  MyDomotic(20, 37, 5000, 35),
  MyDomotic(20, 41, 5000, 39),
  MyDomotic(20, 45, 5000, 43),
  MyDomotic(20, 49, 5000, 47),
};

//DEFAULT
//CALCOLO DEL NUMERO DI OGGETTO MYDOMOTIC AGGIUNTI
const int sizeof_mydomotic_obj = (int) sizeof(mydomotic_obj) / sizeof(MyDomotic);

EthernetClient ethClient;
PubSubClient client(ethClient);
long lastReconnectAttempt = 0;
//DEFINIZIONE IP SE IL DHCP NON FUNZIONA
IPAddress ip(192, 168, 10, 99);

void setup() {
  Serial.begin(9600);
  Serial.println("Starting...");
  //CHIAMATA AL SETUP DEGLI OGGETTI
  for (int i = 0; i < sizeof_mydomotic_obj; i++) {
    mydomotic_obj[i].setup();
  }
  
  for (int i = 0; i < sizeof_mydomotic_obj; i++) {
    client.subscribe(mydomotic_obj[i].getTopic());
  }
  
  
  Serial.println("Attempting IP...");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    Ethernet.begin(mac, ip);
  }
  Serial.print("LocalIP: ");
  Serial.println(Ethernet.localIP());
  client.setServer(server_mqtt, 1883);
  client.setCallback(callback);
}


void loop() {
  for (int i = 0; i < sizeof_mydomotic_obj; i++) {
    mydomotic_obj[i].loop();
  }
  if (!client.connected()) {  //RDAM COMMENTATO
    long now = millis();
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      // Attempt to reconnect
      if (reconnect()) {
        lastReconnectAttempt = 0;
      }
    }
  } else {
    // Client connected
    client.loop();
  }
}

boolean reconnect() {
  if (client.connect("arduinoClient")) {
    // Once connected, publish an announcement...
    client.publish("outTopic", "hello world");
    // ... and resubscribe
    client.subscribe("inTopic");
    for (int i = 0; i < sizeof_mydomotic_obj; i++) {
      client.subscribe(mydomotic_obj[i].getTopic());
    }
  }
  return client.connected();
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  char value_read[length];
  for (int i = 0; i < length; i++) {
    value_read[i] = (char) payload[i];
  }
  int value = (int) value_read;
  for (int i = 0; i < sizeof_mydomotic_obj; i++) {
    if (topic == mydomotic_obj[i].getTopic()) { 
      Serial.println("TROVATO");
    }
  }
  Serial.println(value);
}





