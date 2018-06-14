/************************************************************************************************/
/************************************************************************************************/
/*******************************  Librerie                   ************************************/
/************************************************************************************************/
/************************************************************************************************/
#ifndef ETHERNETSUPPORT
 #define ETHERNETSUPPORT 0
#endif


#include "command.h"
#include "platform.h"
#include <ArduinoJson.h>

/************************************************************************************************/
/************************************************************************************************/
/***********************DEFINIZIONE DEI PIN E DEI LORO COMPORTAMENTI ****************************/
/************************************************************************************************/
/************************************************************************************************/

bool DEBUG_SERIAL                     = true;
bool ENABLE_CONFIGURE                 = false;
int eepromAddress                     = 0;
const int RESET_TIMEOUT               = 10;
const bool WaitTimeOutBeforeReset     = true;
const int RESET_PIN_MODE              = 13;
String inputSerialCommand             = "";     // a string to hold incoming data
String inputSerialData                = "";     // a string to hold incoming data
boolean inputSerialStringComplete     = false;  // whether the string is complete
boolean inputSerialDataFound          = false;  // whether the string is complete
ArduinoSetting arduino_setting;
AsyncDelay status_time;
long status_time_refresh              = (1000L * 60);

/****************************************
  Define board pins input/output count
****************************************/
const int count_digital_input = sizeof(digital_input) / sizeof(int);
const int count_digital_output = sizeof(digital_output) / sizeof(int);
const int count_analogic_input = sizeof(analogic_input) / sizeof(int);
const int count_custom_input = sizeof(custom_input) / sizeof(int);
/*VARIABILE DA ELIMINARE*/
MyDomotic mydomotic_obj [count_digital_input];



/*MyDomotic mydomotic_obj [] {
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

const int sizeof_mydomotic_obj = (int) sizeof(mydomotic_obj) / sizeof(MyDomotic);
/************************************************************************************************/
/************************************************************************************************/
/************************************************************************************************/
/************************************************************************************************/

#if ETHERNETSUPPORT == 0
  const bool NETWORK_ENABLE = false;
  String ARDUINOHOST =  "ArduinoMyDomotic";
#elif ETHERNETSUPPORT == 1
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
  String ARDUINOHOST =  ArduinoHostName +
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

      for (int i = 0; i < count_digital_input; i++) {
        client_mqtt.subscribe((char *)mydomotic_obj[i].getTopic().c_str());
      }
    }
    return client_mqtt.connected();
  }

  void callback(char* topicRecive, byte* payload, unsigned int length) {
    // DEBUG STAMPA COSA ARRIVA SU MQTT
    PrintDEBUG("MQTT Message arrived [" + (String) topicRecive + "] ",0);
    for (int i = 0; (unsigned) i < length; i++) {
      PrintDEBUG((String) payload[i],0);
    }
    PrintDEBUG("");
    // CERCA L'OGGETTO MYDOMOTIC CON IL TOPIC RICEVUTO
    for (int i = 0; i < count_digital_input; i++) {
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
#elif ETHERNETSUPPORT == 2
    String ARDUINOHOST =  "ArduinoESP";

    ELClient esp(&Serial);

    ELClientMqtt client_mqtt(&esp);                   // Initialize the MQTT client
    bool connected;
    ELClientWebServer client_webserver(&esp);         // Initialize the Web-Server client

    void wifiCb(void* response) {
      ELClientResponse *res = (ELClientResponse*)response;
      if (res->argc() == 1) {
        uint8_t status;
        res->popArg(&status, 1);

        if(status == STATION_GOT_IP) {
          PrintINFO("WIFI CONNECTED");
        } else {
          PrintINFO("WIFI NOT READY: ",0);
          PrintINFO((String) status);
        }
      }
    }

    // Callback when MQTT is connected
    void mqttConnected(void* response) {
      client_mqtt.subscribe(mqtt_topic_cmd().c_str());
      client_mqtt.subscribe(mqtt_topic_set().c_str());

      for (int i = 0; i < count_digital_input; i++) {
        client_mqtt.subscribe(mydomotic_obj[i].getTopic().c_str());
        client_mqtt.subscribe(mydomotic_obj[i].setObj().c_str());
      }
      connected = true;
    }

    // Callback when MQTT is disconnected
    void mqttDisconnected(void* response) {
      connected = false;
    }

    // Callback when an MQTT message arrives for one of our subscriptions
    void callback(void* response) {
      ELClientResponse *res = (ELClientResponse *)response;
      String topic = res->popString();
      String data = res->popString();
      StaticJsonBuffer<200> jsonBuffer;


      PrintDEBUG("MQTT Message arrived [" + topic + "]: DATA [" + data + "] ");
      if (topic == mqtt_topic_cmd()){
        /*
        JsonObject& root = jsonBuffer.parseObject(data);
        if (!root.success()) {
          PrintINFO("Failed to load json from data");
          return;
        }
        PrintINFO("Arrived a command");
        PrintINFO(data);
        */
      }else if (topic == mqtt_topic_set()){
        /*
        JsonObject& root = jsonBuffer.parseObject(data);
        if (!root.success()) {
          PrintINFO("Failed to load json from data");
          return;
        }
        PrintINFO("Arrived a SET");
        PrintINFO(data);
        if(root["id"] != "") PrintINFO(root["id"]);
        if(root["label"] != "") PrintINFO(root["label"]);
        if(root["led"][0] != "") PrintINFO(root["led"][0]);
        if(root["led"][1] != "") PrintINFO(root["led"][1]);
        */
      }else{
        // CERCA L'OGGETTO MYDOMOTIC CON IL TOPIC RICEVUTO
        if(topic.substring(0,3) == PREFIX_SET.substring(0,3)){
          for (int i = 0; i < count_digital_input; i++) {
            if (mydomotic_obj[i].setObj() == topic) {
              JsonObject& root = jsonBuffer.parseObject(data);
              MyDomoticSetting dataobj = mydomotic_obj[i].get_setting();
              if(root["label"] != "") strncpy(dataobj.label, root["label"], sizeof(dataobj.label));
              if(root["led"] != "") dataobj.led = atoi(root["led"]);
              if(root["period"] != "") dataobj.period = 1000L * atoi(root["period"]);
              if(root["led_check"] != "") dataobj.led_check = atoi(root["led_check"]);
              if(root["type_object"] != "") dataobj.type_object = atoi(root["type_object"]);
              if(root["idx"] != "") dataobj.idx = atoi(root["idx"]);
              dataobj._period_state = 0;
              mydomotic_obj[i].set_setting(dataobj);
              mydomotic_obj[i].setup();
              i = count_digital_input;
            }
          }
        } else if (topic.substring(0,3) == PREFIX_CMD.substring(0,3)){
          for (int i = 0; i < count_digital_input; i++) {
            if (mydomotic_obj[i].getTopic() == topic) {
              if(data == "0"){
                mydomotic_obj[i].off();
                i = count_digital_input;
              }else if(data == "1"){
                mydomotic_obj[i].on();
                i = count_digital_input;
              }else if(data == "2"){
                mydomotic_obj[i].action();
                i = count_digital_input;
              }
            }
          }
        }
      }
    }

    void mqttPublished(void* response) {
      PrintDEBUG("MQTT published");
    }

    void resetCb(void) {
      bool ok = false;
      do {
        PrintINFO("EL-Client (re-)starting!");
        ok = esp.Sync();
      } while(!ok);
      PrintINFO("EL-Client synced!");
      client_webserver.setup();
      client_mqtt.setup();
    }

    void mydomoticRefreshCb(char * url) {
      String h = (String) arduino_setting.hostname;
      String t = (String) arduino_setting.topic;
      String din = (String) arduino_setting.domoticz_in;
      String dout = (String) arduino_setting.domoticz_out;
      String cmdmqtt = mqtt_topic_cmd();
      String statmqtt = mqtt_topic_stat();
      String setmqtt = mqtt_topic_set();
      String d = bool2String(arduino_setting.debug);
      String d1 = bool2String(arduino_setting.domoticz);
      String b = (String) BOARDNAMETYPE;
      String configure_enable = bool2String(ENABLE_CONFIGURE);
      client_webserver.setArgJson(F("arduinohostname"), Str2Json(h).begin());
      client_webserver.setArgJson(F("debug"), Str2Json(d).begin());
      client_webserver.setArgJson(F("domoticz"), Str2Json(d1).begin());
      client_webserver.setArgJson(F("configureenable"), Str2Json(configure_enable).begin());
      client_webserver.setArgJson(F("din"), Str2Json(din).begin());
      client_webserver.setArgJson(F("dout"), Str2Json(dout).begin());
      client_webserver.setArgJson(F("board"), Str2Json(b).begin());
      client_webserver.setArgJson(F("topic"), Str2Json(t).begin());
      client_webserver.setArgJson(F("cmdmqtt"), Str2Json(cmdmqtt).begin());
      client_webserver.setArgJson(F("statmqtt"), Str2Json(statmqtt).begin());
      client_webserver.setArgJson(F("setmqtt"), Str2Json(setmqtt).begin());
    }

    void mydomoticDebugButtonPressCb(char * button){
      String btn = button;
      if( btn == F("debugchange") ) {
        if(arduino_setting.debug){
          arduino_setting.debug = false;
          SaveData(arduino_setting);
        } else {
          arduino_setting.debug = true;
          SaveData(arduino_setting);
        }
      } else if( btn == F("domoticzchange") ){
        if (arduino_setting.domoticz) {
          arduino_setting.domoticz = false;
          SaveData(arduino_setting);
        } else {
          arduino_setting.domoticz = true;
          SaveData(arduino_setting);
        }
      } else if( btn == F("configchange") ){
        if (ENABLE_CONFIGURE) {
          ENABLE_CONFIGURE = false;
        } else {
          ENABLE_CONFIGURE = true;
        }
      }
    }

    // called at page loading
    void mydomoticLoadCb(char * url)
    {
      mydomoticRefreshCb( url );
    }

    void mydomoticSetFieldCb(char * field)
    {
      String fld = field;
      if( fld == F("topic")){
        String topic;
        topic = client_webserver.getArgString();
        topic.toCharArray(arduino_setting.topic, topic.length()+1);
        SaveData(arduino_setting);
      } else if( fld == F("arduinohostname")){
        String arduinohostname;
        arduinohostname = client_webserver.getArgString();
        arduinohostname.toCharArray(arduino_setting.hostname, arduinohostname.length()+1);
        SaveData(arduino_setting);
      } else if( fld == F("din")){
        String stringa;
        stringa = client_webserver.getArgString();
        stringa.toCharArray(arduino_setting.domoticz_in, stringa.length()+1);
        SaveData(arduino_setting);
      } else if( fld == F("dout")){
        String stringa;
        stringa = client_webserver.getArgString();
        stringa.toCharArray(arduino_setting.domoticz_out, stringa.length()+1);
        SaveData(arduino_setting);
      }
    }


#endif

void setup() {
  Serial.begin(115200);

  verifica_reset_data();
  //set_initial_data();
  load_stored_data();

  PrintINFO("",1,false);
  PrintINFO("Starting system...");
  PrintINFO("",1,false);

  //CHIAMATA AL SETUP DEGLI OGGETTI
  for (int i = 0; i < count_digital_input; i++) {
    //CHIAMATA AL SETUP DEGLI OGGETTI
    mydomotic_obj[i].setup();
  }

  #if ETHERNETSUPPORT  == 1
    //ATTENDE L'IP DAL DHCP
    DPrintln("Attempting IP...");
    if(DHCP_ENABLE){
      if (Ethernet.begin(mac) == 0) {
        DPrintln("Failed to configure Ethernet using DHCP");
        Ethernet.begin(mac, ip);
      }
    }else{
      Ethernet.begin(mac, ip);
    }
    DPrint("LocalIP: ");
    DPrintln((String) Ethernet.localIP());
    client_mqtt.setServer(server_mqtt, 1883);
    client_mqtt.setCallback(callback);
    reconnect();
  #elif ETHERNETSUPPORT  == 2
    PrintINFO("EL-Client starting!");
    esp.resetCb = resetCb;

    URLHandler *mydomotic_status = client_webserver.createURLHandler(F("/MyDStatus.html.json"));
    mydomotic_status->loadCb.attach(mydomoticLoadCb);
    mydomotic_status->refreshCb.attach(mydomoticRefreshCb);
    mydomotic_status->setFieldCb.attach(mydomoticSetFieldCb);
    mydomotic_status->buttonCb.attach(mydomoticDebugButtonPressCb);

    client_mqtt.connectedCb.attach(mqttConnected);
    client_mqtt.disconnectedCb.attach(mqttDisconnected);
    client_mqtt.publishedCb.attach(mqttPublished);
    client_mqtt.dataCb.attach(callback);

    resetCb();
  #endif
  #if ETHERNETSUPPORT == 1 or ETHERNETSUPPORT == 2
  for (int i = 0; i < count_digital_input; i++) {
    //CHIAMATA AL SETUP DEGLI OGGETTI
    mydomotic_obj[i].mqttset(client_mqtt);
  }
  #endif
  status_time.start(status_time_refresh, AsyncDelay::MILLIS);
  PrintINFO("HOSTNAME: " + (String) arduino_setting.hostname);
  PrintINFO("SYSTEM STARTED!");
}


void loop() {
  if (status_time.isExpired()) {
    client_mqtt.publish(mqtt_topic_status().c_str(),system_status().c_str());
    for (int i = 0; i < count_digital_input; i++) {
      client_mqtt.publish(mqtt_topic_status().c_str(),mydomotic_obj[i].to_json().c_str());
    }
    status_time.repeat();
  }
  for (int i = 0; i < count_digital_input; i++) {
    mydomotic_obj[i].loop();
  }
  #if ETHERNETSUPPORT == 1
    if (!client_mqtt.connected()) {
      long now = millis();
      if (now - lastReconnectAttempt > 5000) {
        lastReconnectAttempt = now;
        // Attempt to reconnect
        DPrintln("MQTT Reconnect!");
        if (reconnect()) {
          lastReconnectAttempt = 0;
        }
      }
    } else {
      // Client connected
      client_mqtt.loop();
    }
  #elif ETHERNETSUPPORT == 2
    esp.Process();
  #endif

  #if ETHERNETSUPPORT == 0
    //EVENTO DI RICEZIONE COMANDI DALLA SERIALE
    if (inputSerialStringComplete) {
      DCommand(inputSerialCommand, inputSerialData);
      if(inputSerialData != "") {
        inputSerialData = "";
      }
      inputSerialCommand = "";
      inputSerialStringComplete = false;
      inputSerialDataFound = false;
    }
  #endif
}

#if ETHERNETSUPPORT == 0
  void serialEvent() {
    while (Serial.available()) {
      char inChar = (char) Serial.read();
      bool addtostring = true;
      switch (inChar) {
        case ' ':
          inputSerialDataFound = true;
          addtostring=false;
        break;
        case '\n':
          inputSerialStringComplete = true;
          addtostring=false;
        break;
      }
      if(!inputSerialDataFound && addtostring) {
        inputSerialCommand += inChar;
      }
      if(inputSerialDataFound && addtostring) {
        inputSerialData += inChar;
      }
    }
  }
#endif
