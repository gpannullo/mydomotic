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

#if ETHERNETSUPPORT == 1
  #include <Ethernet.h>
  #include <PubSubClient.h>
  extern const bool NETWORK_ENABLE;
#elif ETHERNETSUPPORT == 2
  #include <ELClient.h>
  #include <ELClientCmd.h>
  #include <ELClientMqtt.h>

  extern ELClientMqtt client_mqtt;
#endif

extern const bool DEBUG_SERIAL;
extern const String ARDUINOHOST;

struct MyDomoticSetting {
  int btn;                        //BOTTONE DELL'AZIONE
  int led;                        //LED PRIMARIO DI RIFERIMENTO PER ATTIVAZIONE RELE'
  int led_check;                  //LED SECONDARIO UTILIZZATO IN CASO DI PERSIANE
  int arrayled[2];                //ARRAY DI LED IN SOSTITUZIONE DEI PRECEDENTI
  long period;                    //EVENUALE TIMER
  long period_state;              //EVENUALE TIMER
  int led_state;                  //STATO DEL LED ATTUALE
  int type_object;                //TIPO DI OGGETTO [SWITCH, BLIND, BUTTON*, BLIND2*]
  char label[50];                 //STRINGA DI RICONOSCIMENTO
};

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
    String ArduinoHost;
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
    MyDomoticSetting get_setting(void);

};



#endif /* MyDomotic_h */
