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
bool DEBUG_SERIAL = true;

/************************************************************************************************/
/************************************************************************************************/
/***********************DEFINIZIONE DEI PIN E DEI LORO COMPORTAMENTI ****************************/
/************************************************************************************************/
/************************************************************************************************/

int eepromAddress                     = 0;
const int RESET_TIMEOUT               = 10;
const bool WaitTimeOutBeforeReset     = true;
bool DEBUG_SERIAL_MYD                 = false;
const int RESET_PIN_MODE              = 13;
String inputSerialCommand             = "";     // a string to hold incoming data
String inputSerialData                = "";     // a string to hold incoming data
boolean inputSerialStringComplete     = false;  // whether the string is complete
boolean inputSerialDataFound          = false;  // whether the string is complete
ArduinoSetting arduino_setting;
AsyncDelay status_time;
long status_time_refresh              = (1000L * 5);

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

    ELClient esp(&Serial, &Serial);
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

      for (int i = 0; i < count_digital_input; i++) {
        client_mqtt.subscribe(mydomotic_obj[i].getTopic().c_str());
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

      PrintDEBUG("Received: topic=");
      String topic = res->popString();
      PrintDEBUG(topic);

      PrintDEBUG("data=");
      String data = res->popString();
      PrintDEBUG(data);
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
      String d;
      if (arduino_setting.debug) {
        d = "ON";
      } else {
        d = "OFF";
      }
      String d1;
      if (arduino_setting.domoticz) {
        d1 = "ON";
      } else {
        d1 = "OFF";
      }
      String b = (String) BOARDNAMETYPE;
      client_webserver.setArgJson(F("arduinohostname"), Str2Json(h).begin());
      client_webserver.setArgJson(F("debug"), Str2Json(d).begin());
      client_webserver.setArgJson(F("domoticz"), Str2Json(d1).begin());
      client_webserver.setArgJson(F("din"), Str2Json(din).begin());
      client_webserver.setArgJson(F("dout"), Str2Json(dout).begin());
      client_webserver.setArgJson(F("board"), Str2Json(b).begin());
      client_webserver.setArgJson(F("topic"), Str2Json(t).begin());
      client_webserver.setArgJson(F("cmdmqtt"), Str2Json(cmdmqtt).begin());
      client_webserver.setArgJson(F("statmqtt"), Str2Json(statmqtt).begin());
      /*
      String table = "[";
      table = table + "[" + \
          "  \"Indice\"" + \
          ", \"IDX\"" + \
          ", \"Label\"" + \
          ", \"Address\"" + \
          ", \"Button\"" + \
          ", \"Type\"" + \
          ", \"Relay\"" + \
          ", \"Time\"" + \
          ", \"Status\"" + \
          "]";
      //for(int i=0; i<count_digital_input; i++){
      for(int i=0; i<8; i++){
        MyDomoticSetting settings_data = mydomotic_obj[i].get_setting();
        table = table + ",[" + \
          "  \"" + (String) i + "\"" + \
          ", \"" + (String) settings_data.idx + "\"" + \
          ", \"" + (String) settings_data.label + "\"" + \
          ", \"" + (String) settings_data.eepromAddress + "\"" + \
          ", \"" + (String) settings_data.btn + "\"" + \
          ", \"" + (String) mydomotic_obj[i].type_to_str() + "\"" + \
          ", \"" + (String) settings_data.arrayled[0] + "\"" + \
          ", \"" + (String) settings_data.period + "\"" + \
          ", \"" + (String) mydomotic_obj[i].get_status() + "\"" + \
          "]";
        String label_id             = "idELC" + (String) i;
        String label_idx            = "idxELC" + (String) i;
        String label_label          = "labelELC" + (String) i;
        String label_eepromAddress  = "eepromAddressELC" + (String) i;
        String label_btn            = "btnELC" + (String) i;
        String label_type           = "typeELC" + (String) i;
        String label_arrayled0      = "arrayled0ELC" + (String) i;
        String label_arrayled1      = "arrayled1ELC" + (String) i;
        String label_period         = "periodELC" + (String) i;
        String label_status         = "statusELC" + (String) i;
        client_webserver.setArgJson((const char*) label_id.c_str(), Str2Json((String) i).begin());
        client_webserver.setArgJson((const char*) label_idx.c_str(), Str2Json((String) settings_data.idx).begin());
        client_webserver.setArgJson((const char*) label_label.c_str(), Str2Json((String) settings_data.label).begin());
        client_webserver.setArgJson((const char*) label_eepromAddress.c_str(), Str2Json((String) settings_data.eepromAddress).begin());
        client_webserver.setArgJson((const char*) label_btn.c_str(), Str2Json((String) settings_data.btn).begin());
        client_webserver.setArgJson((const char*) label_type.c_str(), Str2Json((String) mydomotic_obj[i].type_to_str()).begin());
        client_webserver.setArgJson((const char*) label_arrayled0.c_str(), Str2Json((String) settings_data.arrayled[0]).begin());
        client_webserver.setArgJson((const char*) label_arrayled1.c_str(), Str2Json((String) settings_data.arrayled[1]).begin());
        client_webserver.setArgJson((const char*) label_period.c_str(), Str2Json((String) settings_data.period).begin());
        client_webserver.setArgJson((const char*) label_status.c_str(), Str2Json((String) mydomotic_obj[i].get_status()).begin());
      }
      table = table + "]";
      //PrintINFO(table);
      table = Str2Json(table);
      client_webserver.setArgJson(F("tableobj"), table.begin());
      */
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
      /*
      else if( fld == F("last_name"))
        userWriteStr(client_webserver.getArgString(), EEPROM_POS_LAST_NAME);
      else if( fld == F("age"))
        EEPROM.update(EEPROM_POS_AGE, (uint8_t)client_webserver.getArgInt());
      else if( fld == F("gender"))
      {
        String gender = client_webserver.getArgString();
        EEPROM.update(EEPROM_POS_GENDER, (gender == F("male")) ? 'm' : 'f');
      }
      else if( fld == F("notifications"))
        EEPROM.update(EEPROM_POS_NOTIFICATIONS, webServer.getArgBoolean());
        */
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
  status_time.start(status_time_refresh, AsyncDelay::MILLIS);
  PrintINFO("HOSTNAME: " + (String) arduino_setting.hostname);
  PrintINFO("SYSTEM STARTED!");
}


void loop() {
  if (status_time.isExpired()) {
    client_mqtt.publish(mqtt_topic_status().c_str(),"11");
    PrintINFO(mqtt_topic_status());
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
