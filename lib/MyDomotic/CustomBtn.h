//
//  CustomBtn.h
//
//
//  Created by Giuseppe Pannullo on 02/03/17.
//
//

#ifndef CustomBtn_h
#define CustomBtn_h

#include <Arduino.h>
#include <AsyncDelay.h>
#include <EEPROM.h>
#include "MyDomoticCore.h"

extern ArduinoSetting arduino_setting;
extern bool send_status_action;

const int MYDC_TYPE_DIGITAL_LEVEL   = 1;
const int MYDC_TYPE_MQTT_BTN        = 2;

class CustomBtn
{
private:
    int btn_state;
    int btn_read;
    CustomPin data;
    AsyncDelay delayled;
    void check_btn_state(void);
    void check_input(void);



public:
    CustomBtn();
    CustomBtn(CustomPin data);

    void setup(void);
    void loop(void);
    void action(void);
    CustomPin get_setting(void);
    void set_setting(CustomPin data);
    void save (void);
    String to_small_json(void);
};



#endif /* CustomBtn_h */
