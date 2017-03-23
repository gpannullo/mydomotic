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
#include "ConditionDom.h"
#include "BooleanOperation.h"


class MyDomotic
{
private:
    /* Attributes */
    int led;
    int led_check;
    int btn;
    int btn_state;
    int period;
    String type_object;
    String status_led = "";
    AsyncDelay delayled;

//RDAM start
    int btn_rain;
    int btn_rele;
    int btn_rele2;
    int temperature_ok;
//RDAM end

    /* Methods */
    void lock(void);
    void action(void);
    void open(void);
    void close(void);
    void setup_switch(void);
    void setup_blind(void);
    void setup_complex(void); //RDAM
    void on(void);
    void off(void);
    void change (void);
    void check_btn_state(void);
    
    
public:
    MyDomotic();
    MyDomotic(int btn, int led);
    MyDomotic(int btn, int led, int period);
    MyDomotic(int btn_up, int led_up, int period, int led_dw);
    //RDAM: definisco una costruttore con tutte le informazioni di cui ho bisogno per gestire le persiane    
    MyDomotic(int btn1, int btn_pioggia, float  lim_max_temperature, float lim_min_temperature, int btn_rele, int btn_rele2, int period1 );
    
    void setup(void);
    void loop(void);
    char* getTopic(void);
};



#endif /* MyDomotic_h */
