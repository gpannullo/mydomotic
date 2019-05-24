#include <Arduino.h>
#include <ArduinoJson.h>
#include "MyDomotic.h"
#include "CustomBtn.h"
#include "function.h"

extern const int count_digital_input;
extern const int count_custom_input;
extern const bool LOGICAL_LEVEL_WORK;
extern const char* MQTTTOPICPREFIX;
extern const bool DOMOTICZ_WORK;
extern MyDomotic mydomotic_obj [];
extern CustomBtn mydomotic_custom_obj [];
extern const int digital_input [];
extern const int digital_output [];
extern const int digital_check [];
extern const int digital_period [];
extern const int digital_command [];
extern const int custom_input [];
extern const int RESET_TIMEOUT;
extern const bool WaitTimeOutBeforeReset;
extern const int RESET_PIN_MODE;
extern const int RESET_PIN_LED;
extern const String BOARDNAMETYPE;
extern String ARDUINOHOST;
extern bool DEBUG_SERIAL;
extern int SET_OPEN;
extern int SET_CLOSE;

//COMANDI
String CMD_HELP           = "?";
String CMD_DEBUG          = "DEBUG";
String CMD_SETTINGS       = "SETTINGS";

void SaveData(ArduinoSetting settings){
  if (digitalRead(RESET_PIN_MODE) == LOW){
    EEPROM.put(settings._eepromAddress, settings);
    PrintINFO(" ... SAVED!",1,false);
  } else {
    PrintINFO(" ... SAVED BLOCKED!",1,false);
  }
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
      PrintCMD("eepromAddress: " + (String) arduino_setting._eepromAddress);
      PrintCMD("************************************************");
      PrintCMD(CMD_SETTINGS + ": MyDomotic");
      PrintCMD("Input Count: " + (String) count_digital_input);
      for (int i=0; i<count_digital_input; i++){
        PrintCMD("[" + (String) i + "]: " + mydomotic_obj[i].to_str());
      }
    }
  }
#endif


void set_initial_data()
{
  PrintINFO("",1,false);
  PrintINFO("",1,false);
  PrintINFO("LOADING Initial DATA Setting... ",0);
  int localeepromAddress=sizeof(int);
  ArduinoSetting arduino_setting_tmp;
  strncpy(arduino_setting_tmp.hostname, ARDUINOHOST.c_str(), sizeof(arduino_setting_tmp.hostname));
  arduino_setting_tmp.debug = DEBUG_SERIAL;
  arduino_setting_tmp.domoticz = DOMOTICZ_WORK;
  arduino_setting_tmp.logical_work_level = LOGICAL_LEVEL_WORK;
  arduino_setting_tmp._eepromAddress=localeepromAddress;
  strncpy(arduino_setting_tmp.topic, MQTTTOPICPREFIX, sizeof(arduino_setting_tmp.topic));
  strncpy(arduino_setting_tmp.domoticz_in, "domoticz/in", sizeof(arduino_setting_tmp.domoticz_in));
  strncpy(arduino_setting_tmp.domoticz_out, "domoticz/out", sizeof(arduino_setting_tmp.domoticz_out));
  EEPROM.put(localeepromAddress, arduino_setting_tmp);
  set_logical_work_level(LOGICAL_LEVEL_WORK);
  PrintINFO("DONE!",1,false);
  localeepromAddress += sizeof(ArduinoSetting);

  PrintINFO("LOADING Initial MyDomotic data objects ", 0);
  for(int i=0; i<count_digital_input; i++){
    digitalWrite(RESET_PIN_LED, HIGH);
    delay(10);
    digitalWrite(RESET_PIN_LED, LOW);
    delay(10);
    MyDomoticSetting data_tmp = {
            {digital_output[i],digital_check[i]},     //led e led_check
            (1000L * digital_period[i]),              //period timer in second
            digital_command[i],                       //type_object
            "BTN LABEL",                              //LABEL
            0,                                        //IDX
            (i + 1),                                  //ID
            digital_input[i],                         //BTN
            localeepromAddress,                       //_eepromAddress
            0,                                        //_period_state
            SET_CLOSE                                 //_led_state
          };
    EEPROM.put(localeepromAddress, data_tmp);
    localeepromAddress += sizeof(MyDomoticSetting);
    PrintINFO(".",0,false);
  }
  for(int i=0; i<count_custom_input; i++){
    CustomPin data_tmp = {
            custom_input[i],
            "","",
            localeepromAddress,     //_eepromAddress
          };
    EEPROM.put(localeepromAddress, data_tmp);
    localeepromAddress += sizeof(CustomPin);
    PrintINFO(".",0,false);
  }
  PrintINFO("DONE!",1,false);
  PrintINFO("",1,false);
}


void test_first_powered()
{
  int localeepromAddress=0;
  int val = 0;
  EEPROM.get(localeepromAddress, val);
  if (val == 0){
    for(int i=0; i<20; i++){
      digitalWrite(RESET_PIN_LED, HIGH);
      delay(5);
      digitalWrite(RESET_PIN_LED, LOW);
      delay(5);
    }
    EEPROM.put(localeepromAddress, 1);
    set_initial_data();
  }
}


void test_reset_request()
{
  pinMode(RESET_PIN_MODE, INPUT_PULLUP);
  pinMode(RESET_PIN_LED, OUTPUT);
  int resetlevel = digitalRead(RESET_PIN_MODE);
  if (resetlevel == LOW){
    if (WaitTimeOutBeforeReset){
      PrintINFO("RESET MODE Request... wait " + (String) RESET_TIMEOUT + "s before remove all");
      for (int i=0;i<RESET_TIMEOUT;i++){
        PrintINFO((String) (RESET_TIMEOUT-i) + "s");
        digitalWrite(RESET_PIN_LED, HIGH);
        delay(500);
        digitalWrite(RESET_PIN_LED, LOW);
        delay(500);
      }
    }
    int resetlevel = digitalRead(RESET_PIN_MODE);
    if (resetlevel == LOW){
      digitalWrite(RESET_PIN_LED, HIGH);
      PrintINFO("RESET MODE Initialize:");
      int printvar = 0;
      for (unsigned int i = 0 ; i < EEPROM.length() ; i++) {
        if(EEPROM.read(i) != 0){                     //skip already "empty" addresses
          EEPROM.write(i, 0);                        //write 0 to address i
          PrintINFO(".",0,false);
          digitalWrite(RESET_PIN_LED, HIGH);
          delay(10);
          digitalWrite(RESET_PIN_LED, LOW);
          delay(10);
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


void load_stored_data()
{
  int eepromAddress=sizeof(int);
  EEPROM.get(eepromAddress, arduino_setting);
  set_logical_work_level(arduino_setting.logical_work_level);
  eepromAddress += sizeof(ArduinoSetting);
  /*
  Serial.println(arduino_setting.hostname);
  Serial.println(arduino_setting.debug);
  Serial.println(arduino_setting.domoticz);
  Serial.println(arduino_setting._eepromAddress);
  Serial.println(arduino_setting.topic);
  Serial.println(arduino_setting.domoticz_in);
  Serial.println(arduino_setting.domoticz_out);
  */
  DPrintln("******************************************");
  DPrintln("******************************************");
  DPrintln("*********                         ********");
  DPrintln("      " + (String) arduino_setting.hostname);
  DPrintln("*********                         ********");
  DPrintln("******************************************");
  DPrintln("******************************************");
  Serial.print("INFO: Load internal data ");
  for(int i=0; i<count_digital_input; i++){
    MyDomoticSetting data_tmp;
    EEPROM.get(eepromAddress, data_tmp);
    mydomotic_obj[i]=MyDomotic(data_tmp);
    eepromAddress += sizeof(MyDomoticSetting);
    Serial.print(".");
  }
  for(int i=0; i<count_custom_input; i++){
    CustomPin data_custom_tmp;
    EEPROM.get(eepromAddress, data_custom_tmp);
    mydomotic_custom_obj[i]=CustomBtn(data_custom_tmp);
    eepromAddress += sizeof(CustomPin);
    Serial.print(".");
  }
  Serial.println(" done!");
  DPrintln("");
}

String mqtt_topic_cmd()
{
  return PREFIX_CMD + "/" + (String) arduino_setting.topic;
}

String mqtt_topic_set()
{
  return PREFIX_SET + "/" + (String) arduino_setting.topic;
}

String mqtt_topic_stat()
{
  return PREFIX_STAT + "/" +  (String) arduino_setting.topic;
}

String mqtt_topic_status()
{
    return mqtt_topic_stat() + "/STATUS";
}

String system_status()
{
    return  "{\"hostname\":\"" + (String) arduino_setting.hostname + "\"" +
            ",\"board\":\"" + (String) BOARDNAMETYPE + "\"" +
            ",\"topic\":\"" + (String) arduino_setting.topic + "\"" +
            ",\"domoticz\":\"" + bool2String(arduino_setting.domoticz) + "\"" +
            ",\"debug\":\"" + bool2String(arduino_setting.debug) + "\"" +
            ",\"SET_OPEN\":\"" + (String) SET_OPEN + "\"" +
            ",\"SET_CLOSE\":\"" + (String) SET_CLOSE + "\"" +
            "}";
}
