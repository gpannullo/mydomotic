/*
  MyDomotic.cpp - Libreria per gestire di MyDomotic
*/

#include "MyDomotic.h"  // dichiarazione della classe

/* funzione di inizializzazione */
MyDomotic::MyDomotic() {

}

MyDomotic::MyDomotic(MyDomoticSetting data) {
  this->data = data;
  this->status_led = "";
}

MyDomotic::MyDomotic(String label, int btn, int led)
{
  this->data.btn = btn;
  this->data.led = led;
  this->data.period = -1;
  this->data.type_object = MYD_TYPE_SWITCH;
  this->status_led = "";
  label.toCharArray(this->data.label, label.length()+1);
}

MyDomotic::MyDomotic(String label, int btn, int led, long period)
{
  this->data.btn = btn;
  this->data.led = led;
  this->data.period = period;
  this->data.type_object = MYD_TYPE_SWITCH;
  this->status_led = "";
  label.toCharArray(this->data.label, label.length()+1);
}

MyDomotic::MyDomotic(String label, int btn, int led, long period, int led_check) {
  this->data.btn = btn;
  this->data.led = led;
  this->data.led_check = led_check;
  this->data.period = period;
  this->data.type_object = MYD_TYPE_BLIND;
  this->status_led = "";
  label.toCharArray(this->data.label, label.length()+1);
}

MyDomoticSetting MyDomotic::get_setting(void) {
  return this->data;
}
/*
  //RDAM start
  MyDomotic::MyDomotic(int btn1, int btn_pioggia, float  lim_max_temperature, float lim_min_temperature, int btn_rele, int btn_rele2, int period1 )
  {
    this->data.btn = btn1;
    this->btn_rain = btn_pioggia;
    this->btn_rele = btn_rele;
    this->btn_rele2 = btn_rele;
    this->data.period = period1;
    TypeInter::condition_type cond1;
    //cond1 = TypeInter::MAJOR;
  //     ConditionDom(int dato, TypeInter::condition_type condition, int value);
    int temperaturaRilevata = 19;
    ConditionDom condTemperatura  = ConditionDom(temperaturaRilevata, TypeInter::MAJOR, lim_min_temperature);
    ConditionDom condTemperatura2 = ConditionDom(temperaturaRilevata, TypeInter::MINOR, lim_max_temperature);
    //Verifico se la temperatura è nei limiti
    BooleanOperation valida = BooleanOperation(condTemperatura.status(), condTemperatura2.status(), TypeInter::AND);
    this->temperature_ok = valida.get_risultato();
    this->type_object = MYD_TYPE_COMPLEX;
  }
  //RDAM end
*/

void MyDomotic::lock(void)
{
  //SETTA IL PIN COME HIGH PER EVITARE PICCHI DURANTE LA FARE DI ASSEGNAZIONE OUTPUT
  delay(10);
  this->SetLed(this->data.led, HIGH);
  if (this->data.type_object == MYD_TYPE_BLIND) {
    delay(10);
    this->SetLed(this->data.led_check, HIGH);
  }
  this->status_led = "LOCK";
  if (DEBUG_SERIAL_MYD) Serial.println(this->status_led + " LED: " + this->data.label);
}

void MyDomotic::off(void) {
  this->lock();
}

void MyDomotic::setup(void)
{
  // PREVIENE CHE QUANDO UN PIN VIENE CONFIGURATO COME OUTPUT
  // POSSA ESSERE LOW E ACCENDERE IL RELE IN MANIERA NON CONTROLLATA
  lock();
  if (this->data.type_object == MYD_TYPE_SWITCH) {
    setup_switch();
  }
  else if (this->data.type_object == MYD_TYPE_BLIND) {
    setup_blind();
  }
  else if (this->data.type_object == MYD_TYPE_COMPLEX) { //RDAM
    //setup_complex();
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
      if (DEBUG_SERIAL_MYD) Serial.println("1. BOTTONE di: " + (String) this->data.label + " (" + (String) this->data.btn + ") PREMUTO");
      this->action();
      delay(10);
    }else{
      if (DEBUG_SERIAL_MYD) Serial.println("BOTTONE di: " + (String) this->data.label + " INTERFERENZA *****************************");
    }
  }
  //AGGIORNA LO STATO DEL BOTTONE
  this->btn_state = digitalRead(this->data.btn);
}

String MyDomotic::type_to_str(void){
  switch (this->data.type_object) {
    case MYD_TYPE_SWITCH:
      return "SWITCH"; break;
    case MYD_TYPE_BLIND:
      return "BLIND"; break;
    case MYD_TYPE_BLIND2:
      return "BLIND2"; break;
    case MYD_TYPE_BUTTON:
      return "BUTTON"; break;
  }
  return "";
}


String MyDomotic::to_str(void) {
  // CONVERSIONE DETTAGLI SETTINGS OGGETTO
  return  "  ID: " + (String) this->data.id + \
          ", IDX: " + (String) this->data.idx + \
          ", Label: " + (String) this->data.label + \
          ", PIN button: " + (String) this->data.btn + \
          ", PIN action: " + (String) this->data.arrayled[0] + \
          ", PIN action check: " + (String) this->data.arrayled[1] + \
          ", TYPE: " + (String) this->type_to_str() + \
          ", STATUS: " + (String) this->status_led + \
          ", eepromAddress: " + (String) this->data.eepromAddress + \
          "";
}

String MyDomotic::get_status(void){
  return this->status_led;
}

// METODO DI LOOP
void MyDomotic::loop(void)
{
  //VERIFICA SE IL TIMER HA FINITO
  if (this->delayled.isExpired() && digitalRead(this->data.led) == LOW && this->data.period > 0) {
    this->SetLed(this->data.led, HIGH);
    this->status_led = "CLOSE";
    if (DEBUG_SERIAL_MYD) Serial.println("END TIMER CLOSE LED: " + (String) this->data.led + " " + (String) this->data.label);
  }
  this->check_btn_state();
}

String MyDomotic::getTopic(void) {
  // OGNI INPUT E' VISTO COME UN EVENTO DI PRESSIONE BOTTONE
  // VENGONO PUBBLICATI GLI EVENTI CON IL NUMERO BOTTONE
  return "cmd/" + (String) arduino_setting.topic + "/action/" + (String) this->data.btn;
}

void MyDomotic::setup_switch(void) {
  pinMode(this->data.led, OUTPUT);
  pinMode(this->data.btn, INPUT_PULLUP);
  this->btn_state = digitalRead(this->data.btn);
  this->btn_read = this->btn_state;
}

void MyDomotic::setup_blind(void) {
  this->setup_switch();
  pinMode(this->data.led_check, OUTPUT);
}

/*
  //RDAM START : Implementare come si deve
  void MyDomotic::setup_complex(void){
    pinMode(btn, INPUT_PULLUP);
    //pinMode(led, OUTPUT);
    btn_state = digitalRead(this->data.btn);
    pinMode(btn_rain, INPUT);
    pinMode(btn_rele, OUTPUT);
    pinMode(btn_rele2, OUTPUT);

  }
  //RDAM END
*/


// METODO PER APRIRE/CHIUDERE LA PERSIANA
// IL METEDO PREVEDE LA CHIUSURA DEL RELE' DI DISCESA (IN CASO DI SALITA)
// E POI ACCENDE IL RELE' DI SALITA PER xxx TEMPO. NELLA FUNZIONE loop()
// SI RICHIUDE PASSATO IL TEMPO xxx

void MyDomotic::open(void)
{
  //VERIFICA CHE IL RELE' DI SALITA/DISCESA NON SIA GIA' IN AZIONE
  if (digitalRead(this->data.led) == HIGH) {
    if (DEBUG_SERIAL_MYD) Serial.println("3 BLOCCO DI TUTTI I RELE DI: " + (String) this->data.label + " AVVIATO");
    // CHIUDE IL RELE' DI DISCESA/SALITA
    lock();
    if (DEBUG_SERIAL_MYD) Serial.println("4 ATTESA: " + (String) this->data.label);
    //ABILITA IL RELE' DI SALITA/DISCESA
    this->SetLed(this->data.led, LOW);
    if (this->data.period > 0) {
      if (DEBUG_SERIAL_MYD) Serial.println("5 ACCENSIONE RELE DI: " + (String) this->data.label + " AVVIATO");
      this->delayled.start(this->data.period, AsyncDelay::MILLIS);
      this->status_led = "MOVING";
    }
    if (DEBUG_SERIAL_MYD) {
      Serial.print(this->status_led);
      Serial.println(" LED: " + (String) this->data.led);
    }
  } // SE GIA' ATTIVO NON FARE NULLA
}

void MyDomotic::on(void) {
  this->open();
}

// METODO PER APRIRE/CHIUDERE UNA LUCE ANCHE A TEMPO SE period > 0
void MyDomotic::change(void)
{
  if (digitalRead(this->data.led) == HIGH) {
    this->SetLed(this->data.led, LOW);
    this->status_led = "OPEN";
  } else {
    this->SetLed(this->data.led, HIGH);
    this->status_led = "CLOSE";
  }
}


void MyDomotic::action(void)
{
  if (this->data.type_object == MYD_TYPE_BLIND) {
    //VERIFICA SE IL PIN DEL RELE' CONTRARIO E' ACCESO
    if (digitalRead(this->data.led_check) == LOW /*&& this->btn_read == LOW*/) {
      // SE GIA' ERA ACCESO, BLOCCA ENTRAMBI
      if (DEBUG_SERIAL_MYD) Serial.println("2.2. BLOCCO: " + (String) this->data.label + " RICHIESTO");
      this->lock();
    } else {
      // AZIONA APERTURA RELE'
      if (DEBUG_SERIAL_MYD) Serial.println("2.1 APERTURA RELE DI: " + (String) this->data.label + " RICHIESTO");
      this->open();
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
void MyDomotic::SetLed(int LED, int level) {
  delay(10);
  digitalWrite(LED, level);
  this->save();
  String json = "{\"host\":\"" + (String) arduino_setting.hostname + "\", " +
                "\"state\":\"ready\", " +
                "\"led\":\"" + LED + "\", " +
                "\"btn\":\"" + this->data.btn + "\", " +
                "\"status\":\"" + digitalRead(LED) + "\"}";
  if (this->client_mqtt_enable && this->data.led == LED) {
    #if ETHERNETSUPPORT == 1 or ETHERNETSUPPORT == 2
    this->client->publish("out", (char*)json.c_str());
    #endif
  }
}

// METODO PER CAMBIARE DI STATO UN LED
void MyDomotic::save() {
  EEPROM.put(this->data.eepromAddress, this->data);
}
