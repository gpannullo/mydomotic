//
//  MyDomotic.h
//
//
//  Created by Giuseppe Pannullo on 02/03/17.
//
//

#ifndef MyDomotic_h
#define MyDomotic_h

#include <Arduino.h>
#include <AsyncDelay.h>
#include <EEPROM.h>
#include <ArduinoJson.h>
#include "MyDomoticCore.h"

#if ETHERNETSUPPORT == 1
  #include <Ethernet.h>
  #include <PubSubClient.h>
  extern const bool NETWORK_ENABLE;
#elif ETHERNETSUPPORT == 2
  #include <ELClient.h>
  //#include <ELClientCmd.h>
  #include <ELClientMqtt.h>
  #include <ELClientWebServer.h>
#endif

extern bool DEBUG_SERIAL_MYD;
extern bool DEBUG_SERIAL;
extern bool ENABLE_CONFIGURE;
extern String ARDUINOHOST;
extern ArduinoSetting arduino_setting;

const int MYD_TYPE_SWITCH     = 0;
const int MYD_TYPE_BUTTON     = 1;
const int MYD_TYPE_BLIND      = 2;
const int MYD_TYPE_BLIND2     = 3;
const int MYD_TYPE_COMPLEX    = 99;

class MyDomotic
{
private:
    int btn_state;
    int btn_read;
    MyDomoticSetting data;
    AsyncDelay delayled;
    #if ETHERNETSUPPORT == 2
      ELClientMqtt *client;
    #elif ETHERNETSUPPORT == 1
      PubSubClient *client;
    #endif
    bool client_mqtt_enable = false;
    bool configuration_setup = false;
    String status_led;


    /* Methods */
    //void lock(void);
    void open(void);
    void close(void);
    void setup_switch(void);
    void setup_blind(void);
    //void setup_complex(void); //RDAM
    void change (void);
    void check_btn_state(void);
    void SetLed(int led, int level);


public:
    MyDomotic();
    MyDomotic(MyDomoticSetting data);
    MyDomotic(String label, int btn, int led);
    MyDomotic(String label, int btn, int led, long period);
    MyDomotic(String label, int btn_up, int led_up, long period, int led_dw);

    void setup(void);
    void loop(void);
    void action(void);
    void on(void);
    void off(void);
    void lock(void);
    String getTopic(void);
    String setObj(void);
    String get_status(void);
    String to_str(void);
    String type_to_str(void);
    String to_json(void);
    #if ETHERNETSUPPORT == 1
      void mqttset(PubSubClient mqtt);
    #elif ETHERNETSUPPORT == 2
      void mqttset(ELClientMqtt mqtt);
    #endif
    void set_setting(MyDomoticSetting data);
    MyDomoticSetting get_setting(void);
    void loadingData(String data);
    void save (void);
};





#endif /* MyDomotic_h */
