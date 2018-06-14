#ifndef MyDomoticCore_h
#define MyDomoticCore_h

#include <Arduino.h>

const String PREFIX_STAT="sta";
const String PREFIX_CMD="cmd";
const String PREFIX_SET="set";

struct ArduinoSetting {
  int _eepromAddress;
  char hostname[50];
  char topic[50];
  boolean debug;
  boolean domoticz;
  char domoticz_in[100];
  char domoticz_out[100];
};

struct MyDomoticSetting {
  int   led;                        //LED PRIMARIO DI RIFERIMENTO PER ATTIVAZIONE RELE'
  int   led_check;                  //LED SECONDARIO UTILIZZATO IN CASO DI PERSIANE
  long  period;                    //EVENUALE TIMER
  int   type_object;                //TIPO DI OGGETTO [SWITCH, BLIND, BUTTON*, BLIND2*]
  char  label[50];                 //STRINGA DI RICONOSCIMENTO
  int   idx;                        //INDICE PER DOMOTICZ
  // PRIVATE VARIABLE
  int   id;                         //INDICE DELL'AZIONE
  int   btn;                        //BOTTONE DELL'AZIONE
  int   _eepromAddress;
  long  _period_state;              //EVENUALE TIMER
  int   _led_state;                  //STATO DEL LED ATTUALE
};

#endif /* MyDomoticCore_h */
