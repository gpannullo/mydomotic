#include <Arduino.h>
#include <EEPROM.h>
#include "MyDomotic.h"
#include "function.h"

extern const int count_digital_input;
extern MyDomotic mydomotic_obj [];
extern const int digital_input [];
extern const int digital_output [];
extern const int RESET_TIMEOUT;
extern const bool WaitTimeOutBeforeReset;
extern const int RESET_PIN_MODE;

//COMANDI
String CMD_HELP           = "?";
String CMD_DEBUG          = "DEBUG";
String CMD_SETTINGS       = "SETTINGS";

void SaveData(ArduinoSetting settings){
  EEPROM.put(settings.eepromAddress, settings);
  PrintINFO(" ... SAVED!",1,false);
}

#if ETHERNETSUPPORT == 0
  void DCommand(String cmd, String data) {
    cmd.trim();data.trim();
    int data_tmp = -1;
    if(data.length() > 0){
      data_tmp = data.toInt();
    }
    if (cmd.equalsIgnoreCase(CMD_DEBUG)) {
      PrintCMD(CMD_DEBUG + ": ",0);
      if (data_tmp == 0){
        arduino_setting.debug = false;
        PrintINFO((String) arduino_setting.debug,0,false);
        SaveData(arduino_setting);
      }else if (data_tmp == 1){
        arduino_setting.debug = true;
        PrintINFO((String) arduino_setting.debug,0,false);
        SaveData(arduino_setting);
      }else{
        PrintINFO((String) arduino_setting.debug,1,false);
      }
    }else if(cmd.equalsIgnoreCase(CMD_SETTINGS)) {
      PrintCMD(CMD_SETTINGS + ": ArduinoSetting");
      PrintCMD("************************************************");
      PrintCMD("HOST: " + (String) arduino_setting.hostname);
      PrintCMD("DEBUG: " + (String) arduino_setting.debug);
      PrintCMD("eepromAddress: " + (String) arduino_setting.eepromAddress);
      PrintCMD("************************************************");
      PrintCMD(CMD_SETTINGS + ": MyDomotic");
      PrintCMD("Input Count: " + (String) count_digital_input);
      for (int i=0; i<count_digital_input; i++){
        PrintCMD("[" + (String) i + "]: " + mydomotic_obj[i].to_str());
      }
    }
  }
#endif


void set_initial_data(){
  PrintINFO("",1,false);
  PrintINFO("",1,false);
  PrintINFO("LOADING Initial DATA Setting... ",0);
  int localeepromAddress=0;
  //PrintDEBUG("Actual eepromAddress: " + (String) localeepromAddress);
  ArduinoSetting arduino_setting_tmp;
  ARDUINOHOST.toCharArray(arduino_setting_tmp.hostname, ARDUINOHOST.length()+1);
  arduino_setting_tmp.debug = DEBUG_SERIAL;
  arduino_setting_tmp.domoticz = true;
  arduino_setting_tmp.eepromAddress=localeepromAddress;
  strncpy(arduino_setting_tmp.topic, "arduino_t", sizeof(arduino_setting_tmp.topic));
  strncpy(arduino_setting_tmp.domoticz_in, "domoticz/in", sizeof(arduino_setting_tmp.domoticz_in));
  strncpy(arduino_setting_tmp.domoticz_out, "domoticz/out", sizeof(arduino_setting_tmp.domoticz_out));
  EEPROM.put(localeepromAddress, arduino_setting_tmp);
  PrintINFO("DONE!",1,false);
  localeepromAddress += sizeof(ArduinoSetting);

  PrintINFO("LOADING Initial MyDomotic data objects ", 0);
  for(int i=0; i<count_digital_input; i++){
    MyDomoticSetting data_tmp = {
            i,
            digital_input[i],
            digital_output[i],0,
            {digital_output[i],0},
            0,
            0,
            0,
            MYD_TYPE_SWITCH,
            "BTN LABEL",
            0,
            localeepromAddress,
          };
    EEPROM.put(localeepromAddress, data_tmp);
    localeepromAddress += sizeof(MyDomoticSetting);
    //PrintDEBUG("Actual eepromAddress: " + (String) localeepromAddress);
    PrintINFO(".",0,false);
  }
  PrintINFO("DONE!",1,false);
  PrintINFO("",1,false);
}


void verifica_reset_data(){
  pinMode(RESET_PIN_MODE, INPUT_PULLUP);
  int resetlevel = digitalRead(RESET_PIN_MODE);
  if (resetlevel == LOW){
    if (WaitTimeOutBeforeReset){
      PrintINFO("RESET MODE Request... wait " + (String) RESET_TIMEOUT + "s before remove all");
      for (int i=0;i<RESET_TIMEOUT;i++){
        PrintINFO((String) (RESET_TIMEOUT-i) + "s");
        delay(1000);
      }
    }
    int resetlevel = digitalRead(RESET_PIN_MODE);
    if (resetlevel == LOW){
      PrintINFO("RESET MODE Initialize:");
      int printvar = 0;
      for (unsigned int i = 0 ; i < EEPROM.length() ; i++) {
        if(EEPROM.read(i) != 0){                     //skip already "empty" addresses
          EEPROM.write(i, 0);                        //write 0 to address i
          PrintINFO(".",0,false);
          printvar ++;
          if (printvar == 128){
            printvar=0;
            PrintINFO("",1,false);
          }
        }
      }
      PrintINFO("",1,false);
      PrintINFO("",1,false);
      PrintINFO("Done!");
      PrintINFO("ALL DATA RESET");
      set_initial_data();
    }
  }
}

void load_stored_data(){
  int eepromAddress=0;
  EEPROM.get(eepromAddress, arduino_setting);
  eepromAddress += sizeof(ArduinoSetting);
  DPrintln("******************************************");
  DPrintln("******************************************");
  DPrintln("*********                         ********");
  DPrintln("      " + (String) arduino_setting.hostname);
  DPrintln("*********                         ********");
  DPrintln("******************************************");
  DPrintln("******************************************");
  MyDomoticSetting data[count_digital_input];
  Serial.print("INFO: Load internal data ");
  for(int i=0; i<count_digital_input; i++){
    EEPROM.get(eepromAddress, data[i]);
    mydomotic_obj[i]=MyDomotic(data[i]);
    eepromAddress += sizeof(MyDomoticSetting);
    Serial.print(".");
  }
  Serial.println(" done!");
  DPrintln("");
}

String mqtt_topic_cmd(){
  return "cmd/" + (String) arduino_setting.topic;
}

String mqtt_topic_stat(){
  return "stat/" + (String) arduino_setting.topic;
}

String mqtt_topic_status(){
    return mqtt_topic_stat() + "/STATUS";
}
