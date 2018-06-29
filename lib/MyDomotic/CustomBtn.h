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
#include <EEPROM.h>
#include "MyDomoticCore.h"

extern ArduinoSetting arduino_setting;

#if ETHERNETSUPPORT == 1
  #include <Ethernet.h>
  #include <PubSubClient.h>
  extern const bool NETWORK_ENABLE;
#elif ETHERNETSUPPORT == 2
  #include <ELClient.h>
  #include <ELClientCmd.h>
  #include <ELClientMqtt.h>
  #include <ELClientWebServer.h>
#endif

class CustomBtn
{
private:
    int btn_state;
    int btn_read;
    CustomPin data;
    #if ETHERNETSUPPORT == 2
      ELClientMqtt *client;
    #elif ETHERNETSUPPORT == 1
      PubSubClient *client;
    #endif
    void check_btn_state(void);



public:
    CustomBtn();
    CustomBtn(CustomPin data);

    void setup(void);
    void loop(void);
    void action(void);
    CustomPin get_setting(void);
    #if ETHERNETSUPPORT == 1
      void mqttset(PubSubClient mqtt);
    #elif ETHERNETSUPPORT == 2
      void mqttset(ELClientMqtt mqtt);
    #endif
    void set_setting(CustomPin data);
    void save (void);
};



#endif /* CustomBtn_h */
