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
//#include "TypeInter.h"
//#include "ConditionDom.h"
//#include "BooleanOperation.h"
#include <PubSubClient.h>


class MyDomotic
{
private:
    /* Attributes */
    int led;
    int led_check;
    int btn;
    int btn_state;
    int btn_read;
    long period;
    String type_object;
    String status_led = "";
    String label = "";
    AsyncDelay delayled;
    PubSubClient client;
    bool client_mqtt_enable = false;
    String ArduinoHost;

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
    MyDomotic(String label, String ArduinoHost, int btn, int led);
    MyDomotic(String label, String ArduinoHost, int btn, int led, long period);
    MyDomotic(String label, String ArduinoHost, int btn_up, int led_up, long period, int led_dw);
    
    void setup(void);
    void loop(void);
    void action(void);
    void on(void);
    void off(void);
    void lock(void);
    void subMQTT(PubSubClient client);
    String getTopic(void);
};



#endif /* MyDomotic_h */
