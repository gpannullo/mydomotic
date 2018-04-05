/************************************************************************************************/
/************************************************************************************************/
/*******************************  Librerie                   ************************************/
/************************************************************************************************/
/************************************************************************************************/
#include "MyDomotic.h"
const bool DEBUG_SERIAL = true;

/************************************************************************************************/
/************************************************************************************************/
/***********************DEFINIZIONE DEI PIN E DEI LORO COMPORTAMENTI ****************************/
/************************************************************************************************/
/************************************************************************************************/

MyDomotic mydomotic_obj [] {
  MyDomotic("Tapparella 1 APRI", 23, 34, 25000, 32),      //RELE 1 UP  //TAPPARELLA 1
  MyDomotic("Tapparella 1 CHIUDI", 25, 32, 25000, 34),    //RELE 2 DW  //TAPPARELLA 1
  MyDomotic("Tapparella 2 APRI", 27, 30, 33000, 28),      //RELE 3 UP  //TAPPARELLA 2
  MyDomotic("Tapparella 2 CHIUDI", 29, 28, 33000, 30),    //RELE 4 DW  //TAPPARELLA 2
  MyDomotic("Tapparella 3 APRI", 31, 26, 33000, 24),      //RELE 5 UP  //TAPPARELLA 3
  MyDomotic("Tapparella 3 CHIUDI", 33, 24, 33000, 26),    //RELE 6 DW  //TAPPARELLA 3
  MyDomotic("Tapparella 4 APRI", 35, 22, 33000, 21),      //RELE 7 UP  //TAPPARELLA 4
  MyDomotic("Tapparella 4 CHIUDI", 37, 21, 33000, 22),    //RELE 8 DW  //TAPPARELLA 4
  MyDomotic("Tapparella 5 APRI", 39, 36, 33000, 38),      //RELE 9 UP  //TAPPARELLA 5
  MyDomotic("Tapparella 5 CHIUDI", 41, 38, 33000, 36),    //RELE 10 DW  //TAPPARELLA 5
  MyDomotic("Tapparella 6 APRI", 20, 40, 25000, 42),      //RELE 11 UP  //TAPPARELLA 6
  MyDomotic("Tapparella 6 CHIUDI", 18, 42, 25000, 40),    //RELE 12 DW  //TAPPARELLA 6
  MyDomotic("Tapparella 7 APRI", 43, 44, 33000, 46),      //RELE 13 UP  //TAPPARELLA 7
  MyDomotic("Tapparella 7 CHIUDI", 45, 46, 33000, 44),    //RELE 14 DW  //TAPPARELLA 7
  MyDomotic("Tapparella 8 APRI", 47, 48, 33000, 19),      //RELE 15 UP  //TAPPARELLA 8
  MyDomotic("Tapparella 8 CHIUDI", 49, 19, 33000, 48),    //RELE 16 DW  //TAPPARELLA 8
};
/************************************************************************************************/
/************************************************************************************************/
/************************************************************************************************/
/************************************************************************************************/

//CALCOLO DEL NUMERO DI OGGETTO MYDOMOTIC AGGIUNTI
const int sizeof_mydomotic_obj = (int) sizeof(mydomotic_obj) / sizeof(MyDomotic);

#if ETHERNETSUPPORT == 0
  const String ARDUINOHOST =  "ArduinoESP";
  // Initialize a connection to esp-link using the normal hardware serial port both for
  // SLIP and for debug messages.
  ELClient esp(&Serial, &Serial);

  // Initialize CMD client (for GetTime)
  ELClientCmd cmd(&esp);

  // Initialize the MQTT client
  ELClientMqtt client_mqtt(&esp);

  // Callback made from esp-link to notify of wifi status changes
  // Here we just print something out for grins
  void wifiCb(void* response) {
    ELClientResponse *res = (ELClientResponse*)response;
    if (res->argc() == 1) {
      uint8_t status;
      res->popArg(&status, 1);

      if(status == STATION_GOT_IP) {
        Serial.println("WIFI CONNECTED");
      } else {
        Serial.print("WIFI NOT READY: ");
        Serial.println(status);
      }
    }
  }

  bool connected;

  // Callback when MQTT is connected
  void mqttConnected(void* response) {
    Serial.println("MQTT connected!");
    client_mqtt.subscribe("/esp-link/1");
    client_mqtt.subscribe("/hello/world/#");
    //mqtt.subscribe("/esp-link/2", 1);
    //mqtt.publish("/esp-link/0", "test1");
    connected = true;
  }

  // Callback when MQTT is disconnected
  void mqttDisconnected(void* response) {
    Serial.println("MQTT disconnected");
    connected = false;
  }

  // Callback when an MQTT message arrives for one of our subscriptions
  void callback(void* response) {
    ELClientResponse *res = (ELClientResponse *)response;

    Serial.print("Received: topic=");
    String topic = res->popString();
    Serial.println(topic);

    Serial.print("data=");
    String data = res->popString();
    Serial.println(data);
  }

  void mqttPublished(void* response) {
    Serial.println("MQTT published");
  }

#endif
#if ETHERNETSUPPORT == 1
  /************************************************************************************************/
  /************************************************************************************************/
  /*******************************  CONFIGURAZIONE PARAMETRI   ************************************/
  /************************************************************************************************/
  /************************************************************************************************/
  // IP DEL SERVER MQTT
  IPAddress server_mqtt(192, 168, 88, 250);
  //DEFINIZIONE IP SE IL DHCP NON FUNZIONA
  IPAddress ip(192, 168, 88, 220);
  //MAC ADDRESS DELLA SCHEDA ETHERNET
  byte mac[]    = {  0xDE, 0xED, 0xBA, 0x20, 0x30, 0x01 };
  const bool NETWORK_ENABLE = true;
  const bool DHCP_ENABLE = true;
  const bool MQTT_ENABLE = NETWORK_ENABLE;
  /************************************************************************************************/
  /************************************************************************************************/
  /************************************************************************************************/
  /************************************************************************************************/



  /************************************************************************************************/
  /************************************************************************************************/
  /*******************************      PARAMETRI FISSI        ************************************/
  /************************************************************************************************/
  /************************************************************************************************/
  String ArduinoHostName = "Arduino";
  EthernetClient ethClient;
  PubSubClient client_mqtt(ethClient);
  long lastReconnectAttempt = 0;
  const int WL_MAC_ADDR_LENGTH = 6;
  const String ARDUINOHOST =  ArduinoHostName +
                              String(mac[WL_MAC_ADDR_LENGTH - 3], HEX) +
                              String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                              String(mac[WL_MAC_ADDR_LENGTH - 1], HEX) ;
  /************************************************************************************************/
  /************************************************************************************************/
  /************************************************************************************************/
  /************************************************************************************************/


  boolean reconnect() {
    if (client_mqtt.connect((char*)ARDUINOHOST.c_str())) {
      String json = "{\"host\":\"" + ARDUINOHOST + "\", \"state\":\"connect\",\"ip\": \"" + Ethernet.localIP() + "\"}";
      //PUBBLICA SU MQTT I DATI DI STATO
      client_mqtt.publish("out", (char*)json.c_str());
      // RI/CREA LE subscribe PER MQTT

      for (int i = 0; i < sizeof_mydomotic_obj; i++) {
        client_mqtt.subscribe((char *)mydomotic_obj[i].getTopic().c_str());
      }
    }
    return client_mqtt.connected();
  }

  void callback(char* topicRecive, byte* payload, unsigned int length) {
    //DEBUG STAMPA COSA ARRIVA SU MQTT
    if(DEBUG_SERIAL) Serial.print("MQTT Message arrived [");
    if(DEBUG_SERIAL) Serial.print(topicRecive);
    if(DEBUG_SERIAL) Serial.print("] ");
    for (int i = 0; (unsigned) i < length; i++) {
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

#endif


void setup() {
  if(DEBUG_SERIAL) Serial.begin(115200);
  if(DEBUG_SERIAL) Serial.println("Starting..." + ARDUINOHOST);
  //CHIAMATA AL SETUP DEGLI OGGETTI
  for (int i = 0; i < sizeof_mydomotic_obj; i++) {
    //CHIAMATA AL SETUP DEGLI OGGETTI
    mydomotic_obj[i].setup();
  }

  #if ETHERNETSUPPORT  == 1
    //ATTENDE L'IP DAL DHCP
    if(DEBUG_SERIAL) Serial.println("Attempting IP...");
    if(NETWORK_ENABLE){
      if(DHCP_ENABLE){
        if (Ethernet.begin(mac) == 0) {
          if(DEBUG_SERIAL) Serial.println("Failed to configure Ethernet using DHCP");
          Ethernet.begin(mac, ip);
        }
      }else{
        Ethernet.begin(mac, ip);
      }
      if(DEBUG_SERIAL) Serial.print("LocalIP: ");
      if(DEBUG_SERIAL) Serial.println(Ethernet.localIP());
      client_mqtt.setServer(server_mqtt, 1883);
      client_mqtt.setCallback(callback);
      reconnect();
    }else{
      if(DEBUG_SERIAL) Serial.println("NO NETWORK ENABLE....");
    }
  #endif
  if(DEBUG_SERIAL) Serial.println("STARTED!");
}


void loop() {
  for (int i = 0; i < sizeof_mydomotic_obj; i++) {
    mydomotic_obj[i].loop();
  }
  #if ETHERNETSUPPORT == 1
    if(NETWORK_ENABLE){
      if (!client_mqtt.connected()) {
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
        client_mqtt.loop();
      }
    }
  #endif
}
