#ifndef MyDomoticCore_h
#define MyDomoticCore_h

#include <Arduino.h>

struct ArduinoSetting {
  int eepromAddress;
  char hostname[50];
  char topic[50];
  boolean debug;
};

struct MyDomoticSetting {
  int id;                         //INDICE DELL'AZIONE
  int btn;                        //BOTTONE DELL'AZIONE
  int led;                        //LED PRIMARIO DI RIFERIMENTO PER ATTIVAZIONE RELE'
  int led_check;                  //LED SECONDARIO UTILIZZATO IN CASO DI PERSIANE
  int arrayled[2];                //ARRAY DI LED IN SOSTITUZIONE DEI PRECEDENTI
  long period;                    //EVENUALE TIMER
  long period_state;              //EVENUALE TIMER
  int led_state;                  //STATO DEL LED ATTUALE
  int type_object;                //TIPO DI OGGETTO [SWITCH, BLIND, BUTTON*, BLIND2*]
  char label[50];                 //STRINGA DI RICONOSCIMENTO
  int idx;                        //INDICE PER DOMOTICZ
  int eepromAddress;
};

#endif /* MyDomoticCore_h */
