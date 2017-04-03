#include <SPI.h>
#include <Ethernet.h>
#include "MyDomotic.h"
#include <PubSubClient.h>

/************************************************************************************************/
/************************************************************************************************/
/*******************************  CONFIGURAZIONE PARAMETRI   ************************************/
/************************************************************************************************/
/************************************************************************************************/
// IP DEL SERVER MQTT
IPAddress server_mqtt(192, 168, 10, 92);
//DEFINIZIONE IP SE IL DHCP NON FUNZIONA
IPAddress ip(192, 168, 10, 98);
//MAC ADDRESS DELLA SCHEDA ETHERNET
byte mac[]    = {  0xDE, 0xED, 0xBA, 0x00, 0x00, 0x02 };
bool DEBUG_SERIAL = false;
/************************************************************************************************/
/************************************************************************************************/
/************************************************************************************************/
/************************************************************************************************/

/************************************************************************************************/
/************************************************************************************************/
/*******************************      PARAMETRI IFSSI        ************************************/
/************************************************************************************************/
/************************************************************************************************/
String ArduinoHostName = "Arduino";
EthernetClient ethClient;
PubSubClient client(ethClient);
long lastReconnectAttempt = 0;
const int WL_MAC_ADDR_LENGTH = 6;
const String ArduinoHost =  ArduinoHostName + 
                            String(mac[WL_MAC_ADDR_LENGTH - 3], HEX) +
                            String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                            String(mac[WL_MAC_ADDR_LENGTH - 1], HEX) ;
/************************************************************************************************/
/************************************************************************************************/
/************************************************************************************************/
/************************************************************************************************/



/************************************************************************************************/
/************************************************************************************************/
/***********************DEFINIZIONE DEI PIN E DEI LORO COMPORTAMENTI ****************************/
/************************************************************************************************/
/************************************************************************************************/

MyDomotic mydomotic_obj [] {
  MyDomotic("Tapparella 9 APRI", ArduinoHost, 23, 39, 40000, 41),      //RELE 1 UP  //TAPPARELLA 9
  MyDomotic("Tapparella 9 CHIUDI", ArduinoHost, 25, 41, 40000, 39),    //RELE 2 DW  //TAPPARELLA 9
  MyDomotic("Interruttore Luci 1", ArduinoHost, 27, 43),               //RELE 3 LUCI
  MyDomotic("Interruttore Cancello", ArduinoHost, 29, 45, 5000),       //RELE 4 CANCELLO
};
/************************************************************************************************/
/************************************************************************************************/
/************************************************************************************************/
/************************************************************************************************/

//CALCOLO DEL NUMERO DI OGGETTO MYDOMOTIC AGGIUNTI
const int sizeof_mydomotic_obj = (int) sizeof(mydomotic_obj) / sizeof(MyDomotic);

void setup() {
  if(DEBUG_SERIAL) Serial.begin(9600);
  if(DEBUG_SERIAL) Serial.println("Starting..." + ArduinoHost);
  //CHIAMATA AL SETUP DEGLI OGGETTI
  for (int i = 0; i < sizeof_mydomotic_obj; i++) {
    //CHIAMATA AL SETUP DEGLI OGGETTI
    mydomotic_obj[i].setup();
  }
  
  //ATTENDE L'IP DAL DHCP
  if(DEBUG_SERIAL) Serial.println("Attempting IP...");
  if (Ethernet.begin(mac) == 0) {
    if(DEBUG_SERIAL) Serial.println("Failed to configure Ethernet using DHCP");
    Ethernet.begin(mac, ip);
  }
  if(DEBUG_SERIAL) Serial.print("LocalIP: ");
  if(DEBUG_SERIAL) Serial.println(Ethernet.localIP());
  client.setServer(server_mqtt, 1883);
  client.setCallback(callback);
  reconnect();
  if(DEBUG_SERIAL) Serial.println("STARTED!");
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
      if(DEBUG_SERIAL) Serial.println("MQTT Reconnect!");
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
  if(DEBUG_SERIAL) Serial.print("MQTT Message arrived [");
  if(DEBUG_SERIAL) Serial.print(topicRecive);
  if(DEBUG_SERIAL) Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char) payload[i]);
  }
  if(DEBUG_SERIAL) Serial.println();
  // CERCA L'OGGETTO MYDOMOTIC CON IL TOPIC RICEVUTO 
  for (int i = 0; i < sizeof_mydomotic_obj; i++) {
    String topic = mydomotic_obj[i].getTopic();
    if (String(topicRecive) == topic) { 
      if(payload[0] == '0'){
        mydomotic_obj[i].off();
      }else if(payload[0] == '1'){
        mydomotic_obj[i].on();
      }else if(payload[0] == '2'){
        mydomotic_obj[i].action();
      }
    }
  }
}





