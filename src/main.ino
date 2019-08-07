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
#include "platform_custom.h"

/************************************************************************************************/
/************************************************************************************************/
/***********************                                             ****************************/
/************************************************************************************************/
/************************************************************************************************/

bool DEBUG_SERIAL                     = true;
bool ENABLE_CONFIGURE                 = false;
const int RESET_TIMEOUT               = 5;
const bool WaitTimeOutBeforeReset     = true;
const int RESET_PIN_MODE              = 14;
const int RESET_PIN_LED               = 13;
ArduinoSetting arduino_setting;
AsyncDelay status_time;
AsyncDelay status_setup;
long status_time_refresh              = (1000L * 60);
long status_setup_refresh             = (1000L * 60);
int SET_OPEN                          = LOW;
int SET_CLOSE                         = HIGH;
bool mqtt_connect_status              = false;

/****************************************
  Define board pins input/output count
****************************************/
const int count_digital_input = sizeof(digital_input) / sizeof(int);
const int count_digital_output = sizeof(digital_output) / sizeof(int);
const int count_analogic_input = sizeof(analogic_input) / sizeof(int);
const int count_custom_input = sizeof(custom_input) / sizeof(int);
/*VARIABILE DA ELIMINARE*/
MyDomotic         mydomotic_obj         [count_digital_input];
CustomBtn         mydomotic_custom_obj  [count_custom_input];


#if ETHERNETSUPPORT == 0
  const bool NETWORK_ENABLE             = false;
  String ARDUINOHOST                    =  "ArduinoMyDomotic";
  String inputSerialCommand             = "";     // a string to hold incoming data
  String inputSerialData                = "";     // a string to hold incoming data
  boolean inputSerialStringComplete     = false;  // whether the string is complete
  boolean inputSerialDataFound          = false;  // whether the string is complete

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
  byte mac[]                            = {  0xDE, 0xED, 0xBA, 0x20, 0x30, 0x01 };
  const bool NETWORK_ENABLE             = true;
  const bool DHCP_ENABLE                = true;
  const bool MQTT_ENABLE                = NETWORK_ENABLE;
  /************************************************************************************************/
  /************************************************************************************************/
  /************************************************************************************************/
  /************************************************************************************************/



  /************************************************************************************************/
  /************************************************************************************************/
  /*******************************      PARAMETRI FISSI        ************************************/
  /************************************************************************************************/
  /************************************************************************************************/
  EthernetClient ethClient;
  PubSubClient client_mqtt(ethClient);
  long lastReconnectAttempt             = 0;
  const int WL_MAC_ADDR_LENGTH          = 6;
  String ARDUINOHOST                    = "Arduino" +
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
    mqtt_connect_status = true;
    return client_mqtt.connected();
  }

  void callback(char* topicRecive, byte* payload, unsigned int length) {
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
    bool sync_ok        = false;
    ELClientWebServer client_webserver(&esp);         // Initialize the Web-Server client

    void wifiCb(void* response)
    {
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
    void mqttConnected(void* response)
    {
      PrintINFO("EL-Client callback mqttConnected, starting..");
      for (int i = 0; i < count_digital_input; i++) {
        client_mqtt.subscribe(mydomotic_obj[i].getTopic().c_str());
        PrintINFO(mydomotic_obj[i].getTopic().c_str());
        client_mqtt.subscribe(mydomotic_obj[i].setObj().c_str());
        PrintINFO(mydomotic_obj[i].setObj().c_str());
      }
      mqtt_connect_status = true;
    }

    // Callback when MQTT is disconnected
    void mqttDisconnected(void* response)
    {
      mqtt_connect_status = false;
    }

    // Callback when an MQTT message arrives for one of our subscriptions
    void callback(void* response)
    {
      ELClientResponse *res = (ELClientResponse *)response;
      String topic  = res->popString();
      String data   = res->popString();

      PrintDEBUG("MQTT Message arrived [" + topic + "]: DATA [" + data + "] ");
      // CERCA L'OGGETTO MYDOMOTIC CON IL TOPIC RICEVUTO
      if(topic.substring(0,3) == PREFIX_SET.substring(0,3)){
        if (digitalRead(RESET_PIN_MODE) == LOW) {
          for (int i = 0; i < count_digital_input; i++) {
            if (mydomotic_obj[i].setObj() == topic) {
              mydomotic_obj[i].loadingData(data);
              client_mqtt.publish(mydomotic_obj[i].getTopicStatus().c_str(),mydomotic_obj[i].to_json().c_str());
              i = count_digital_input;
              break;
            }
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

    void mqttPublished(void* response)
    {
      PrintDEBUG("MQTT published");
    }

    void mydomoticRefreshCb(char * url)
    {
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
      String configure_enable = int2String(digitalRead(RESET_PIN_MODE));
      String work_relay_level = level2String(arduino_setting.logical_work_level);
      client_webserver.setArgJson(F("arduinohostname"), Str2Json(h).begin());
      client_webserver.setArgJson(F("debug"), Str2Json(d).begin());
      client_webserver.setArgJson(F("domoticz"), Str2Json(d1).begin());
      client_webserver.setArgJson(F("configureenable"), Str2Json(configure_enable).begin());
      client_webserver.setArgJson(F("workrelaylevel"), Str2Json(work_relay_level).begin());
      client_webserver.setArgJson(F("din"), Str2Json(din).begin());
      client_webserver.setArgJson(F("dout"), Str2Json(dout).begin());
      client_webserver.setArgJson(F("board"), Str2Json(b).begin());
      client_webserver.setArgJson(F("topic"), Str2Json(t).begin());
      client_webserver.setArgJson(F("cmdmqtt"), Str2Json(cmdmqtt).begin());
      client_webserver.setArgJson(F("statmqtt"), Str2Json(statmqtt).begin());
      client_webserver.setArgJson(F("setmqtt"), Str2Json(setmqtt).begin());
    }

    void mydomoticDebugButtonPressCb(char * button)
    {
      String btn = button;
      if (digitalRead(RESET_PIN_MODE) == LOW){
        if( btn == F("debugchange") ) {
          if(arduino_setting.debug){
            arduino_setting.debug = false;
            SaveData(arduino_setting);
          } else {
            arduino_setting.debug = true;
            SaveData(arduino_setting);
          }
        } else if( btn == F("domoticzchange") ) {
          if (arduino_setting.domoticz) {
            arduino_setting.domoticz = false;
            SaveData(arduino_setting);
          } else {
            arduino_setting.domoticz = true;
            SaveData(arduino_setting);
          }
        }  else if( btn == F("workrelaychange") ) {
          if (arduino_setting.logical_work_level) {
            arduino_setting.logical_work_level = false;
          } else {
            arduino_setting.logical_work_level = true;
          }
          SaveData(arduino_setting);
          set_logical_work_level(arduino_setting.logical_work_level);
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
      if (digitalRead(RESET_PIN_MODE) == LOW){
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
    }

    void resetCb(void)
    {
      PrintINFO("EL-Client resetCb test syncing");
      int ind=0;
      do {
        ind++;
        sync_ok = esp.Sync();
        if (!sync_ok) PrintINFO("EL-Client sync failed! Test: " + (String) ind);
      } while(!sync_ok and ind<3);
      if(sync_ok){
        PrintINFO("EL-Client synced! send setup now");

        URLHandler *mydomotic_status = client_webserver.createURLHandler(F("/MyDStatus.html.json"));
        mydomotic_status->loadCb.attach(mydomoticLoadCb);
        mydomotic_status->refreshCb.attach(mydomoticRefreshCb);
        mydomotic_status->setFieldCb.attach(mydomoticSetFieldCb);
        mydomotic_status->buttonCb.attach(mydomoticDebugButtonPressCb);

        client_mqtt.connectedCb.attach(mqttConnected);
        client_mqtt.disconnectedCb.attach(mqttDisconnected);
        client_mqtt.publishedCb.attach(mqttPublished);
        client_mqtt.dataCb.attach(callback);

        client_webserver.setup();
        client_mqtt.setup();
        for (int i = 0; i < count_digital_input; i++) {
          //CHIAMATA AL SETUP DEGLI OGGETTI
          mydomotic_obj[i].mqttset(client_mqtt);
        }
      } else {
        PrintINFO("EL-Client NOT synced!");
      }
    }
#endif

void setup()
{
  Serial.begin(115200);
  //test_first_powered();
  test_reset_request();
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
    esp.resetCb = resetCb;
  #endif
  status_time.start(status_time_refresh, AsyncDelay::MILLIS);
  status_setup.start(status_time_refresh, AsyncDelay::MILLIS);
  PrintINFO("SETUP HOSTNAME: " + (String) arduino_setting.hostname);
  PrintINFO("SETUP BOARDNAMETYPE: " + BOARDNAMETYPE);
  PrintINFO("SETUP SYSTEM STARTED!");
}


void loop()
{
  #if ETHERNETSUPPORT == 1 or ETHERNETSUPPORT == 2
  if (status_time.isExpired()) {
    if (mqtt_connect_status) {
      PrintINFO("EL-Client send Mqtt Status");
      client_mqtt.publish(mqtt_topic_status().c_str(),system_status().c_str());
      for (int i = 0; i < count_digital_input; i++) {
        client_mqtt.publish(mydomotic_obj[i].getTopicStatus().c_str(),mydomotic_obj[i].to_json().c_str());
      }
    } else {
      PrintINFO("EL-Client send serial Status");
      PrintINFO(system_status().c_str());
      for (int i = 0; i < count_digital_input; i++) {
        PrintINFO(mydomotic_obj[i].getTopicStatus().c_str(),0);
        PrintINFO(" : ",0,false);
        PrintINFO(mydomotic_obj[i].to_json().c_str(),1,false);
      }
    }
    status_time.repeat();
  }
  #endif
  for (int i = 0; i < count_digital_input; i++) {
    mydomotic_obj[i].loop();
  }
  /*
  for (int i = 0; i < count_custom_input; i++) {
    mydomotic_custom_obj[i].loop();
  }*/
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
    if(status_setup.isExpired() && !sync_ok){
      resetCb();
      if(!sync_ok) status_setup.repeat();
    }
    if(sync_ok) esp.Process();
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
