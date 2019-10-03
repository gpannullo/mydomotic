#ifndef MyDomoticCore_h
#define MyDomoticCore_h

#include <Arduino.h>

const String PREFIX_STAT  = "sta";
const String PREFIX_CMD   = "cmd";
const String PREFIX_SET   = "set";

struct ArduinoSetting {
  int _eepromAddress;
  char hostname[20];
  char topic[20];
  boolean debug;
  boolean domoticz;
  boolean logical_work_level;
  char domoticz_in[15];
  char domoticz_out[15];
};

struct MyDomoticSetting {
  int   led[3];                       //LED PRIMARIO DI RIFERIMENTO PER ATTIVAZIONE RELE'
  long  period;                       //EVENUALE TIMER
  int   type_object;                  //TIPO DI OGGETTO [SWITCH, BLIND, BUTTON*, BLIND2*]
  int   idx;                          //INDICE PER DOMOTICZ
  // PRIVATE VARIABLE
  int   id;                           //INDICE DELL'AZIONE
  int   btn;                          //BOTTONE DELL'AZIONE
  int   _eepromAddress;
  long  _period_state;                //EVENUALE TIMER
  int   _led_state;                   //STATO DEL LED ATTUALE
};

struct CustomPin {
  int   btn;                          //BOTTONE DELL'AZIONE
  int   type_object;                  //TIPO DI OGGETTO [SWITCH, BLIND, BUTTON*, BLIND2*]
  char  mqtt[5];
  char  topic[30];
  int   _eepromAddress;
  long  _period_state;
};
#endif /* MyDomoticCore_h */
