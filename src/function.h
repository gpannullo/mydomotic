#include <Arduino.h>
#include "MyDomoticCore.h"
#include <EEPROM.h>

extern ArduinoSetting arduino_setting;
extern String inputSerialCommand;
extern String inputSerialData;
extern boolean inputSerialStringComplete;
extern boolean inputSerialDataFound;


void PrintBasic(String messaggio, int enter, String prefixdata, bool prefix) {
  if(!prefix) prefixdata="";
  Serial.print(prefixdata + messaggio);
  if(enter == 1) Serial.println();
}

void PrintDEBUG(String messaggio, int enter = 1, bool prefix = true){
  PrintBasic(messaggio,enter,"DEBUG: ", prefix);
}

void PrintINFO(String messaggio, int enter = 1, bool prefix = true){
  PrintBasic(messaggio,enter,"INFO: ", prefix);
}

void PrintCMD(String messaggio, int enter = 1, bool prefix = true){
  PrintBasic(messaggio,enter,"CMD: ", prefix);
}

void PrintDATA(String messaggio, int enter = 1, bool prefix = true){
  PrintBasic(messaggio,enter,"DATA: ", prefix);
}

void DPrint(String messaggio, bool prefix = true){
  if(arduino_setting.debug) PrintDEBUG(messaggio, 0, prefix);
}

void DPrintln(String messaggio, bool prefix = true){
  if(arduino_setting.debug) PrintDEBUG(messaggio, 1, prefix);
}

const String Str2Json(String a){
  a = "\"" + a + "\"";
  return a;
}

String bool2String(bool c){
  if(c){
    return "ON";
  }else{
    return "OFF";
  }
}

String int2String(int c){
  if(c == 0){
    return "ON";
  }else{
    return "OFF";
  }
}

String level2String(bool c){
  if(c){
    return "SET_OPEN = HIGH";
  }else{
    return "SET_OPEN = LOW";
  }
}
