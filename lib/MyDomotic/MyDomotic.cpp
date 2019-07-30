/*
  MyDomotic.cpp - Libreria per gestire di MyDomotic
*/

#include "MyDomotic.h"  // dichiarazione della classe

/* funzione di inizializzazione */
MyDomotic::MyDomotic()
{
}

MyDomotic::MyDomotic(MyDomoticSetting data)
{
  this->data = data;
  this->status_led = "";
}

MyDomotic::MyDomotic(String label, int btn, int led)
{
  this->data.btn = btn;
  this->data.led[0] = led;
  this->data.period = -1;
  this->data.type_object = MYD_TYPE_SWITCH;
  this->status_led = "";
  label.toCharArray(this->data.label, label.length()+1);
}

MyDomotic::MyDomotic(String label, int btn, int led, long period)
{
  this->data.btn = btn;
  this->data.led[0] = led;
  this->data.period = period;
  this->data.type_object = MYD_TYPE_SWITCH;
  this->status_led = "";
  label.toCharArray(this->data.label, label.length()+1);
}

MyDomotic::MyDomotic(String label, int btn, int led, long period, int led_check)
{
  this->data.btn = btn;
  this->data.led[0] = led;
  this->data.led[1] = led_check;
  this->data.period = period;
  this->data.type_object = MYD_TYPE_BLIND;
  this->status_led = "";
  label.toCharArray(this->data.label, label.length()+1);
}

MyDomoticSetting MyDomotic::get_setting(void)
{
  return this->data;
}

void MyDomotic::set_setting(MyDomoticSetting data)
{
  this->data = data;
  this->save();
}

void MyDomotic::lock(void)
{
  //SETTA IL PIN COME HIGH PER EVITARE PICCHI DURANTE LA FARE DI ASSEGNAZIONE OUTPUT
  this->SetLed(this->data.led[0], SET_CLOSE);
  if (this->data.type_object == MYD_TYPE_BLIND or  \
    this->data.type_object == MYD_TYPE_BLIND2_UP or \
    this->data.type_object == MYD_TYPE_BLIND2_DW or \
    this->data.type_object == MYD_TYPE_SWITCH2 \
  ) {
    delay(10);
    if (this->data.led[1] != 0) {
      this->SetLed(this->data.led[1], SET_CLOSE);
    }
  }
  this->status_led = "LOCK";
  if (arduino_setting.debug) Serial.println(this->status_led + " LED: " + this->data.label);
}

void MyDomotic::off(void)
{
  this->lock();
}

void MyDomotic::setup(void)
{
  // EVITA SALVATAGGI INUTILI IN FASE DI SETUP
  this->configuration_setup = false;
  // PREVIENE CHE QUANDO UN PIN VIENE CONFIGURATO COME OUTPUT
  // POSSA ESSERE LOW E ACCENDERE IL RELE IN MANIERA NON CONTROLLATA
  int type_object = this->data.type_object;
  this->lock();
  if (type_object == MYD_TYPE_SWITCH) {
    this->setup_switch();
  }
  else if (type_object == MYD_TYPE_BLIND or \
    type_object == MYD_TYPE_BLIND2_UP or \
    type_object == MYD_TYPE_BLIND2_DW or \
    type_object == MYD_TYPE_SWITCH2
  ) {
    this->setup_blind();
  }
  else if (type_object == MYD_TYPE_COMPLEX) { //RDAM
    //setup_complex();
  }
  this->configuration_setup = true;
}

void MyDomotic::setup_switch(void)
{
  pinMode(this->data.led[0], OUTPUT);
  if(this->data.type_object == MYD_TYPE_SWITCH){
      this->SetLed(this->data.led[0],this->data._led_state);
  }
  pinMode(this->data.btn, INPUT_PULLUP);
  this->btn_state = digitalRead(this->data.btn);
  this->btn_read = this->btn_state;
}

void MyDomotic::setup_blind(void)
{
  this->setup_switch();
  if (this->data.led[1] != 0){
      pinMode(this->data.led[1], OUTPUT);
  }
}

//VERIFICA SE E' STATO PREMUTO IL BOTTONE DI INPUT
//L'EVENTO DI INPUT C'E' SOLO SE IL BOTTONE NORMALMENTE HIGH PASSA A LOW
void MyDomotic::check_btn_state(void)
{
  this->btn_read = digitalRead(this->data.btn);
  if (this->btn_state != this->btn_read && this->btn_read == LOW) {
    //PER EVITARE PICCHI DI CORRENTE NON PREVISTI SI MISURA UN RITADO
    //SULL?ATTIVAZIONE DEL BOTTONE
    delay(50);
    if(digitalRead(this->data.btn) == LOW){
      //ESEGUE LE AZIONI PREVISTE
      if (arduino_setting.debug) Serial.println("1. BOTTONE di: " + (String) this->data.label + " (" + (String) this->data.btn + ") PREMUTO");
      this->action();
      delay(10);
    }else{
      if (arduino_setting.debug) Serial.println("BOTTONE di: " + (String) this->data.label + " INTERFERENZA *****************************");
    }
  } else if (this->btn_state != this->btn_read && this->data.type_object == MYD_TYPE_BUTTON) {
    delay(50);
    if ( this->btn_state != digitalRead(this->data.btn)) {
      this->action();
      delay(10);
    }
  }
  //AGGIORNA LO STATO DEL BOTTONE
  this->btn_state = digitalRead(this->data.btn);
}

String MyDomotic::type_to_str(void)
{
  switch (this->data.type_object) {
    case MYD_TYPE_SWITCH:
      return "SWITCH"; break;
    case MYD_TYPE_BLIND:
      return "BLIND"; break;
    case MYD_TYPE_BLIND2_UP:
      return "BLIND2UP"; break;
    case MYD_TYPE_BLIND2_DW:
      return "BLIND2DW"; break;
    case MYD_TYPE_BUTTON:
      return "BUTTON"; break;
  }
  return "";
}


String MyDomotic::to_str(void)
{
  // CONVERSIONE DETTAGLI SETTINGS OGGETTO
  return  "  ID: " + (String) this->data.id + \
          ", IDX: " + (String) this->data.idx + \
          ", Label: " + (String) this->data.label + \
          ", BTN: " + (String) this->data.btn + \
          ", LED: " + (String) this->data.led[0] + \
          ", LEV: " + (String) digitalRead(this->data.led[0]) + \
          ", LED2: " + (String) this->data.led[1] + \
          ", TYPE: " + (String) this->type_to_str() + \
          ", STATUS: " + (String) this->status_led + \
          ", PERIOD: " + (String) this->data.period + \
          ", TOPIC: " + (String) this->getTopic() + \
          ", SET: " + (String) this->setObj() + \
          ", eepromAddress: " + (String) this->data._eepromAddress + \
          "";
}

String MyDomotic::to_json(void)
{
  // CONVERSIONE DETTAGLI SETTINGS OGGETTO IN JSON
  return  "{ \"ID\": \"" + (String) this->data.id + "\"" + \
          ", \"IDX\": \"" + (String) this->data.idx + "\"" + \
          ", \"Label\": \"" + (String) this->data.label + "\"" + \
          ", \"BTN\": \"" + (String) this->data.btn + "\"" + \
          ", \"LED\": \"" + (String) this->data.led[0] + "\"" + \
          ", \"LEV\": \"" + (String) digitalRead(this->data.led[0]) + "\"" + \
          ", \"LED2\": \"" + (String) this->data.led[1] + "\"" + \
          ", \"TYPE\": \"" + (String) this->type_to_str() + "\"" + \
          ", \"STATUS\": \"" + (String) this->status_led + "\"" + \
          ", \"TOPICSTATUS\": \"" + this->getTopicStatus() + "\"" + \
          ", \"PERIOD\": \"" + (String) this->data.period + "\"" + \
          ", \"TOPIC\": \"" + (String) this->getTopic() + "\"" + \
          ", \"SET\": \"" + (String) this->setObj() + "\"" + \
          ", \"eepromAddress\": \"" + (String) this->data._eepromAddress + "\"" + \
          "}";
}

String MyDomotic::get_status(void)
{
  return this->status_led;
}

// METODO DI LOOP
void MyDomotic::loop(void)
{
  //VERIFICA SE IL TIMER HA FINITO
  if (this->data.type_object == MYD_TYPE_BLIND2_DW)
  {
    if (this->delayled.isExpired() && digitalRead(this->data.led[0]) == SET_OPEN && this->data.period > 0 && digitalRead(this->data.led[1]) == SET_OPEN) {
      this->SetLed(this->data.led[1], SET_CLOSE);
      delay(10);
      this->SetLed(this->data.led[0], SET_CLOSE);
      this->status_led = "CLOSE";
      if (arduino_setting.debug) Serial.println("END TIMER CLOSE LED: " + \
                                      (String) this->data.led[1] + " E " + \
                                      (String) this->data.led[0] + " " + \
                                      (String) this->data.label);
    }
  } else if (this->data.type_object == MYD_TYPE_BLIND2_UP)
  {
    if (this->delayled.isExpired() && digitalRead(this->data.led[0]) == SET_OPEN && this->data.period > 0 && digitalRead(this->data.led[1]) == SET_CLOSE) {
      this->SetLed(this->data.led[0], SET_CLOSE);
      delay(10);
      this->SetLed(this->data.led[1], SET_CLOSE);
      this->status_led = "CLOSE";
      if (arduino_setting.debug) Serial.println("END TIMER CLOSE LED: " + \
                                      (String) this->data.led[0] + " E " + \
                                      (String) this->data.led[1] + " " + \
                                      (String) this->data.label);
    }
  } else if (this->delayled.isExpired() && digitalRead(this->data.led[0]) == SET_OPEN && this->data.period > 0) {
    this->SetLed(this->data.led[0], SET_CLOSE);
    this->status_led = "CLOSE";
    if (arduino_setting.debug) Serial.println("END TIMER CLOSE LED: " + (String) this->data.led[0] + " " + (String) this->data.label);
  }
  this->check_btn_state();
}

// METODO PER APRIRE/CHIUDERE LA PERSIANA
// IL METEDO PREVEDE LA CHIUSURA DEL RELE' DI DISCESA (IN CASO DI SALITA)
// E POI ACCENDE IL RELE' DI SALITA PER xxx TEMPO. NELLA FUNZIONE loop()
// SI RICHIUDE PASSATO IL TEMPO xxx

void MyDomotic::open(void)
{
  //VERIFICA CHE IL RELE' DI SALITA/DISCESA NON SIA GIA' IN AZIONE
  if (digitalRead(this->data.led[0]) == SET_CLOSE) {
    if (arduino_setting.debug) Serial.println("3 BLOCCO DI TUTTI I RELE DI: " + (String) this->data.label + " AVVIATO");
    // CHIUDE IL RELE' DI DISCESA/SALITA
    lock();
    if (arduino_setting.debug) Serial.println("4 ATTESA: " + (String) this->data.label);
    //ABILITA IL RELE' DI SALITA/DISCESA
    this->SetLed(this->data.led[0], SET_OPEN);
    if (this->data.period > 0) {
      if (arduino_setting.debug) Serial.println("5 ACCENSIONE RELE DI: " + (String) this->data.label + " AVVIATO");
      this->delayled.start(this->data.period, AsyncDelay::MILLIS);
      this->status_led = "MOVING";
    }
    if (arduino_setting.debug) {
      Serial.print(this->status_led);
      Serial.println(" LED: " + (String) this->data.led[0]);
    }
  } // SE GIA' ATTIVO NON FARE NULLA
}

void MyDomotic::on(void)
{
  this->open();
}

// METODO PER APRIRE/CHIUDERE UNA LUCE ANCHE A TEMPO SE period > 0
void MyDomotic::change(void)
{
  if (digitalRead(this->data.led[0]) == SET_CLOSE) {
    this->status_led = "OPEN";
    this->SetLed(this->data.led[0], SET_OPEN);
  } else {
    this->status_led = "CLOSE";
    this->SetLed(this->data.led[0], SET_CLOSE);
  }
}

void MyDomotic::action(void)
{
  if (this->data.type_object == MYD_TYPE_BLIND) {
    //VERIFICA SE IL PIN DEL RELE' CONTRARIO E' ACCESO
    if (digitalRead(this->data.led[1]) == SET_OPEN /*&& this->btn_read == LOW*/) {
      // SE GIA' ERA ACCESO, BLOCCA ENTRAMBI
      if (arduino_setting.debug) Serial.println("2.2. BLOCCO: " + (String) this->data.label + " RICHIESTO");
      this->lock();
    } else {
      // AZIONA APERTURA RELE'
      if (arduino_setting.debug) Serial.println("2.1 APERTURA RELE DI: " + (String) this->data.label + " RICHIESTO");
      this->open();
    }
  } else if (this->data.type_object == MYD_TYPE_BLIND2_UP) {
    //RELE A e B (A acceso fase B spento ritorno)
    if (digitalRead(this->data.led[0]) == SET_OPEN and digitalRead(this->data.led[1]) == SET_OPEN)
    {
      // CONDIZIONE DI MYD_TYPE_BLIND2_UP ATTIVO QUINDI BLOCCA
      if (arduino_setting.debug) Serial.println("2.2. BLOCCO: " + (String) this->data.label + " RICHIESTO");
      this->SetLed(this->data.led[0], SET_CLOSE);
      this->SetLed(this->data.led[1], SET_CLOSE);
    }
    else if (digitalRead(this->data.led[0]) == SET_CLOSE)
    {
      this->SetLed(this->data.led[0], SET_CLOSE);
      this->SetLed(this->data.led[1], SET_CLOSE);
      this->SetLed(this->data.led[0], SET_OPEN);
      if (this->data.period > 0) {
        if (arduino_setting.debug) Serial.println("5 ACCENSIONE RELE DI: " + (String) this->data.label + " AVVIATO UP");
        this->delayled.start(this->data.period, AsyncDelay::MILLIS);
        this->status_led = "MOVING UP";
      }
    }
  } else if (this->data.type_object == MYD_TYPE_BLIND2_DW) {
    //RELE B e A (B acceso fase A acceso ritorno)
    if (digitalRead(this->data.led[1]) == SET_OPEN and digitalRead(this->data.led[0]) == SET_CLOSE)
    {
      // CONDIZIONE DI MYD_TYPE_BLIND2_DW ATTIVO QUINDI BLOCCA
      this->SetLed(this->data.led[1], SET_CLOSE); //BLOCCA SUBITO L'ALIMENTAZIONE
      this->SetLed(this->data.led[0], SET_CLOSE);
    }
    else if (digitalRead(this->data.led[0]) == SET_CLOSE and digitalRead(this->data.led[1]) == SET_CLOSE)
    {
      this->SetLed(this->data.led[0], SET_OPEN);
      delay(10);
      this->SetLed(this->data.led[1], SET_OPEN);
      if (this->data.period > 0) {
        if (arduino_setting.debug) Serial.println("5 ACCENSIONE RELE DI: " + (String) this->data.label + " AVVIATO DW");
        this->delayled.start(this->data.period, AsyncDelay::MILLIS);
        this->status_led = "MOVING DW";
      }
    }
  } else if (this->data.type_object == MYD_TYPE_COMPLEX) {
    // TODO
  } else if (this->data.type_object == MYD_TYPE_SWITCH) {
    // SE E' PREVISTO IL TIMER, IL LED DEVE PASSARE DALLO STATO ATTUALE A QUELLO INVERSO
    // PER POI RITORNARE NELLO STESSO STATO
    this->change();
    if (this->data.period > 0) {
      this->delayled.start(this->data.period, AsyncDelay::MILLIS);
    }
  } else if (this->data.type_object == MYD_TYPE_SWITCH2) {
    // ACCENDE CON LO STESSO BOTTONE PIU' LED
    this->switch2();
  } else if (this->data.type_object == MYD_TYPE_BUTTON) {
      /*if(digitalRead(this->data.btn) == LOW){
        this->on();
      } else {
        this->off();
      }*/
      this->change();
  }
}

void MyDomotic::switch2(void)
{
  if (digitalRead(this->data.led[0]) == SET_CLOSE && digitalRead(this->data.led[1]) == SET_CLOSE ) {
    this->SetLed(this->data.led[0], SET_OPEN);
    this->SetLed(this->data.led[1], SET_CLOSE);
    this->status_led = "0 OPEN, 1 CLOSE";
  } else if (digitalRead(this->data.led[0]) == SET_OPEN && digitalRead(this->data.led[1]) == SET_CLOSE ) {
    this->SetLed(this->data.led[0], SET_OPEN);
    this->SetLed(this->data.led[1], SET_OPEN);
    this->status_led = "0 OPEN, 1 OPEN";
  } else if (digitalRead(this->data.led[0]) == SET_OPEN && digitalRead(this->data.led[1]) == SET_OPEN ) {
    this->SetLed(this->data.led[0], SET_CLOSE);
    this->SetLed(this->data.led[1], SET_OPEN);
    this->status_led = "0 CLOSE, 1 OPEN";
  } else if (digitalRead(this->data.led[0]) == SET_CLOSE && digitalRead(this->data.led[1]) == SET_OPEN ) {
    this->SetLed(this->data.led[0], SET_CLOSE);
    this->SetLed(this->data.led[1], SET_CLOSE);
    this->status_led = "0 CLOSE, 1 CLOSE";
  }
}

// METODO PER CHIUDERE LA PERSIANA
// DA RIVEDERE COME SETTARE LO STATO
void MyDomotic::close(void)
{
  this->open();
  this->status_led = "CLOSING";
}

// METODO PER CAMBIARE DI STATO UN LED
void MyDomotic::SetLed(int LED, int level)
{
  delay(10);
  digitalWrite(LED, level);
  if(this->configuration_setup) {
    if(this->data.type_object == MYD_TYPE_SWITCH){
        this->data._led_state = level;
        this->save();
    }

    #if ETHERNETSUPPORT == 1 or ETHERNETSUPPORT == 2
    if (this->client_mqtt_enable && this->data.led[0] == LED && mqtt_connect_status)
    {
      if (arduino_setting.domoticz)
      {
        String json = "{\"host\":\"" + (String) arduino_setting.hostname + "\", " +
                      "\"state\":\"ready\", " +
                      "\"led\":\"" + (String) LED + "\", " +
                      "\"btn\":\"" + (String) this->data.btn + "\", " +
                      "\"period\":\"" + (String) this->data.period + "\", " +
                      "\"status\":\"" + (String) digitalRead(LED) + "\"}";
        this->client->publish(arduino_setting.domoticz_out, json.c_str());
      }
      String topic1 = PREFIX_STAT + "/" + (String) arduino_setting.topic + "/STATUS";
      this->client->publish(topic1.c_str(), this->to_json().c_str());
      String topic2 = PREFIX_STAT + "/" + (String) arduino_setting.topic + "/LED";
      this->client->publish(topic2.c_str(), this->to_json().c_str());
    }
    #endif
  }
}

#if ETHERNETSUPPORT == 1
  void MyDomotic::mqttset(PubSubClient mqtt)
  {
    this->client = &mqtt;
    this->client_mqtt_enable = true;
  }
#elif ETHERNETSUPPORT == 2
  void MyDomotic::mqttset(ELClientMqtt mqtt)
  {
    this->client = &mqtt;
    this->client_mqtt_enable = true;
  }
#endif

// METODO PER CAMBIARE DI STATO UN LED
void MyDomotic::save()
{
  EEPROM.put(this->data._eepromAddress, this->data);
}


String MyDomotic::getTopic(void)
{
  // OGNI INPUT E' VISTO COME UN EVENTO DI PRESSIONE BOTTONE
  // VENGONO PUBBLICATI GLI EVENTI CON IL NUMERO BOTTONE
  return PREFIX_CMD + "/" + (String) arduino_setting.topic + "/action/" + (String) this->data.id;
}


String MyDomotic::getTopicStatus(void)
{
  // OGNI INPUT E' VISTO COME UN EVENTO DI PRESSIONE BOTTONE
  // VENGONO PUBBLICATI GLI EVENTI CON IL NUMERO BOTTONE
  return PREFIX_STAT + "/" + (String) arduino_setting.topic + "/status/" + (String) this->data.id;
}


String MyDomotic::setObj(void)
{
  // OGNI INPUT E' VISTO COME UN EVENTO DI PRESSIONE BOTTONE
  // VENGONO PUBBLICATI GLI EVENTI CON IL NUMERO BOTTONE
  return PREFIX_SET + "/" + (String) arduino_setting.topic + "/" + (String) this->data.id;
}


void MyDomotic::loadingData(String data)
{
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(data);
  if(root["label"]        != "") strncpy(this->data.label, root["label"], sizeof(this->data.label));
  if(root["led"]          != "") this->data.led[0] = atoi(root["led"]);
  if(root["period"]       != "") this->data.period = 1000L * atoi(root["period"]);
  if(root["led2"]         != "") this->data.led[1] = atoi(root["led2"]);
  if(root["type_object"]  != "") this->data.type_object = atoi(root["type_object"]);
  if(root["idx"]          != "") this->data.idx = atoi(root["idx"]);
  this->data._period_state = 0;

  this->save();
}
