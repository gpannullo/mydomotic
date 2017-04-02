#include <SPI.h>
#include <Ethernet.h>
#include "MyDomotic.h"
#include <PubSubClient.h>

// IP DEL SERVER MQTT
IPAddress server_mqtt(192, 168, 10, 92);
//MAC ADDRESS DELLA SCHEDA ETHERNET
byte mac[]    = {  0xDE, 0xED, 0xBA, 0x00, 0x00, 0x01 };
String ArduinoHostName = "Arduino";

EthernetClient ethClient;
PubSubClient client(ethClient);
long lastReconnectAttempt = 0;
//DEFINIZIONE IP SE IL DHCP NON FUNZIONA
IPAddress ip(192, 168, 10, 99);
const int WL_MAC_ADDR_LENGTH = 6;
const String ArduinoHost =  ArduinoHostName + 
                            String(mac[WL_MAC_ADDR_LENGTH - 3], HEX) +
                            String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                            String(mac[WL_MAC_ADDR_LENGTH - 1], HEX) ;



//DEFINIZIONE DEI PIN E DEI LORO COMPORTAMENTI
MyDomotic mydomotic_obj [] {
  MyDomotic("Tapparella 1 APRI", ArduinoHost, 23, 34, 40000, 32),      //RELE 1 UP  //TAPPARELLA 1
  MyDomotic("Tapparella 1 CHIUDI", ArduinoHost, 25, 32, 40000, 34),    //RELE 2 DW  //TAPPARELLA 1
  MyDomotic("Tapparella 2 APRI", ArduinoHost, 27, 30, 40000, 28),      //RELE 3 UP  //TAPPARELLA 2
  MyDomotic("Tapparella 2 CHIUDI", ArduinoHost, 29, 28, 40000, 30),    //RELE 4 DW  //TAPPARELLA 2
  MyDomotic("Tapparella 3 APRI", ArduinoHost, 31, 26, 40000, 24),      //RELE 5 UP  //TAPPARELLA 3
  MyDomotic("Tapparella 3 CHIUDI", ArduinoHost, 33, 24, 40000, 26),    //RELE 6 DW  //TAPPARELLA 3
  MyDomotic("Tapparella 4 APRI", ArduinoHost, 35, 22, 40000, 21),      //RELE 7 UP  //TAPPARELLA 4
  MyDomotic("Tapparella 4 CHIUDI", ArduinoHost, 37, 21, 40000, 22),    //RELE 8 DW  //TAPPARELLA 4
  MyDomotic("Tapparella 5 APRI", ArduinoHost, 39, 36, 40000, 38),      //RELE 9 UP  //TAPPARELLA 5
  MyDomotic("Tapparella 5 CHIUDI", ArduinoHost, 41, 38, 40000, 36),    //RELE 10 DW  //TAPPARELLA 5
  MyDomotic("Tapparella 6 APRI", ArduinoHost, 20, 40, 40000, 42),      //RELE 11 UP  //TAPPARELLA 6
  MyDomotic("Tapparella 6 CHIUDI", ArduinoHost, 43, 42, 40000, 40),    //RELE 12 DW  //TAPPARELLA 6
  MyDomotic("Tapparella 7 APRI", ArduinoHost, 45, 44, 40000, 46),      //RELE 13 UP  //TAPPARELLA 7
  MyDomotic("Tapparella 7 CHIUDI", ArduinoHost, 47, 46, 40000, 44),    //RELE 14 DW  //TAPPARELLA 7
  MyDomotic("Tapparella 8 APRI", ArduinoHost, 49, 48, 40000, 50),      //RELE 15 UP  //TAPPARELLA 8
  MyDomotic("Tapparella 8 CHIUDI", ArduinoHost, 51, 19, 40000, 48),    //RELE 16 DW  //TAPPARELLA 8
};

//DEFAULT
//CALCOLO DEL NUMERO DI OGGETTO MYDOMOTIC AGGIUNTI
const int sizeof_mydomotic_obj = (int) sizeof(mydomotic_obj) / sizeof(MyDomotic);

void setup() {
  Serial.begin(9600);
  Serial.println("Starting..." + ArduinoHost);
  //CHIAMATA AL SETUP DEGLI OGGETTI
  for (int i = 0; i < sizeof_mydomotic_obj; i++) {
    //CHIAMATA AL SETUP DEGLI OGGETTI
    mydomotic_obj[i].setup();
  }
  
  //ATTENDE L'IP DAL DHCP
  Serial.println("Attempting IP...");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    Ethernet.begin(mac, ip);
  }
  Serial.print("LocalIP: ");
  Serial.println(Ethernet.localIP());
  client.setServer(server_mqtt, 1883);
  client.setCallback(callback);
  reconnect();
  Serial.println("STARTED!");
}


void loop() {
  for (int i = 0; i < sizeof_mydomotic_obj; i++) {
    mydomotic_obj[i].loop();
  }
  if (!client.connected()) {
    long now = millis();
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      // Attempt to reconnect
      Serial.println("MQTT Reconnect!");
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
  if (client.connect((char*)ArduinoHost.c_str())) {
    String json = "{'host':'" + ArduinoHost + "', 'state':'connect'}";
    //PUBBLICA SU MQTT I DATI DI STATO
    client.publish("out", (char*)json.c_str());
    // RI/CREA LE subscribe PER MQTT
    for (int i = 0; i < sizeof_mydomotic_obj; i++) {
      mydomotic_obj[i].subMQTT(client);
    }
  }
  return client.connected();
}

void callback(char* topicRecive, byte* payload, unsigned int length) {
  //DEBUG STAMPA COSA ARRIVA SU MQTT
  Serial.print("MQTT Message arrived [");
  Serial.print(topicRecive);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char) payload[i]);
  }
  Serial.println();
  // CERCA L'OGGETTO MYDOMOTIC CON IL TOPIC RICEVUTO 
  for (int i = 0; i < sizeof_mydomotic_obj; i++) {
    String topic = mydomotic_obj[i].getTopic();
    if (String(topicRecive) == topic) { 
      if(payload[0] == '1'){
        mydomotic_obj[i].action();
      }else if(payload[0] == '0'){
        mydomotic_obj[i].lock();
      }
    }
  }
}





