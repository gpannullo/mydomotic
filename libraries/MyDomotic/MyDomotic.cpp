/*
  MyDomotic.cpp - Libreria per gestire di MyDomotic
*/

#include "MyDomotic.h"  // dichiarazione della classe

/* funzione di inizializzazione */
MyDomotic::MyDomotic() {

}

MyDomotic::MyDomotic(String label, int btn, int led)
{
  this->btn = btn;
  this->led = led;
  this->period = -1;
  this->type_object = "SWITCH";
  this->status_led = "";
  this->label = label;
  this->ArduinoHost = ARDUINOHOST;
}

MyDomotic::MyDomotic(String label, int btn, int led, long period)
{
  this->btn = btn;
  this->led = led;
  this->period = period;
  this->type_object = "SWITCH";
  this->status_led = "";
  this->label = label;
  this->ArduinoHost = ARDUINOHOST;
}

MyDomotic::MyDomotic(String label, int btn, int led, long period, int led_check) {
  this->btn = btn;
  this->led = led;
  this->led_check = led_check;
  this->period = period;
  this->type_object = "BLIND";
  this->status_led = "";
  this->label = label;
  this->ArduinoHost = ARDUINOHOST;
}
/*
  //RDAM start
  MyDomotic::MyDomotic(int btn1, int btn_pioggia, float  lim_max_temperature, float lim_min_temperature, int btn_rele, int btn_rele2, int period1 )
  {
    this->btn = btn1;
    this->btn_rain = btn_pioggia;
    this->btn_rele = btn_rele;
    this->btn_rele2 = btn_rele;
    this->period = period1;
    TypeInter::condition_type cond1;
    //cond1 = TypeInter::MAJOR;
  //     ConditionDom(int dato, TypeInter::condition_type condition, int value);
    int temperaturaRilevata = 19;
    ConditionDom condTemperatura  = ConditionDom(temperaturaRilevata, TypeInter::MAJOR, lim_min_temperature);
    ConditionDom condTemperatura2 = ConditionDom(temperaturaRilevata, TypeInter::MINOR, lim_max_temperature);
    //Verifico se la temperatura è nei limiti
    BooleanOperation valida = BooleanOperation(condTemperatura.status(), condTemperatura2.status(), TypeInter::AND);
    this->temperature_ok = valida.get_risultato();
    this->type_object = "COMPLEX";
  }
  //RDAM end
*/

void MyDomotic::lock(void)
{
  //SETTA IL PIN COME HIGH PER EVITARE PICCHI DURANTE LA FARE DI ASSEGNAZIONE OUTPUT
  delay(10);
  this->SetLed(this->led, HIGH);
  if (type_object == "BLIND") {
    delay(10);
    this->SetLed(this->led_check, HIGH);
  }
  this->status_led = "LOCK";
  if (DEBUG_SERIAL) Serial.println(this->status_led + " LED: " + this->label);
}

void MyDomotic::off(void) {
  this->lock();
}

void MyDomotic::setup(void)
{
  // PREVIENE CHE QUANDO UN PIN VIENE CONFIGURATO COME OUTPUT
  // POSSA ESSERE LOW E ACCENDERE IL RELE IN MANIERA NON CONTROLLATA
  lock();
  if (type_object == "SWITCH") {
    setup_switch();
  }
  else if (type_object == "BLIND") {
    setup_blind();
  }
  else if (type_object == "COMPLEX") { //RDAM
    //setup_complex();
  }
}


//VERIFICA SE E' STATO PREMUTO IL BOTTONE DI INPUT
//L'EVENTO DI INPUT C'E' SOLO SE IL BOTTONE NORMALMENTE HIGH PASSA A LOW
void MyDomotic::check_btn_state(void)
{
  this->btn_read = digitalRead(this->btn);
  if (this->btn_state != this->btn_read && this->btn_read == LOW) {
    //PER EVITARE PICCHI DI CORRENTE NON PREVISTI SI MISURA UN RITADO 
    //SULL?ATTIVAZIONE DEL BOTTONE
    delay(50);
    if(digitalRead(this->btn) == LOW){
      //ESEGUE LE AZIONI PREVISTE
      if (DEBUG_SERIAL) Serial.println("1. BOTTONE di: " + this->label + " PREMUTO");
      this->action();
      delay(10);
    }else{
      if (DEBUG_SERIAL) Serial.println("BOTTONE di: " + this->label + " INTERFERENZA *****************************");
    }
  }
  //AGGIORNA LO STATO DEL BOTTONE
  this->btn_state = digitalRead(this->btn);
}

// METODO DI LOOP
void MyDomotic::loop(void)
{
  //VERIFICA SE IL TIMER HA FINITO
  if (this->delayled.isExpired() && digitalRead(this->led) == LOW && this->period > 0) {
    this->SetLed(this->led, HIGH);
    this->status_led = "CLOSE";
    if (DEBUG_SERIAL) Serial.println("END TIMER CLOSE LED: " + this->led + ' ' + this->label);

  }
  this->check_btn_state();
}

String MyDomotic::getTopic(void) {
  // OGNI INPUT E' VISTO COME UN EVENTO DI PRESSIONE BOTTONE
  // VENGONO PUBBLICATI GLI EVENTI CON IL NUMERO BOTTONE
  return this->ArduinoHost + "/action/" + (String) this->btn;
}

void MyDomotic::setup_switch(void) {
  pinMode(this->led, OUTPUT);
  pinMode(this->btn, INPUT_PULLUP);
  this->btn_state = digitalRead(this->btn);
  this->btn_read = this->btn_state;
}

void MyDomotic::setup_blind(void) {
  setup_switch();
  pinMode(this->led_check, OUTPUT);
}

/*
  //RDAM START : Implementare come si deve
  void MyDomotic::setup_complex(void){
    pinMode(btn, INPUT_PULLUP);
    //pinMode(led, OUTPUT);
    btn_state = digitalRead(this->btn);
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
  if (digitalRead(this->led) == HIGH) {
    if (DEBUG_SERIAL) Serial.println("3 BLOCCO DI TUTTI I RELE DI: " + this->label + " AVVIATO");
    // CHIUDE IL RELE' DI DISCESA/SALITA
    lock();
    if (DEBUG_SERIAL) Serial.println("4 ATTESA: " + this->label);
    //ABILITA IL RELE' DI SALITA/DISCESA
    this->SetLed(this->led, LOW);
    if (this->period > 0) {
      if (DEBUG_SERIAL) Serial.println("5 ACCENSIONE RELE DI: " + this->label + " AVVIATO");
      this->delayled.start(this->period, AsyncDelay::MILLIS);
      this->status_led = "MOVING";
    }
    if (DEBUG_SERIAL) {
      Serial.print(this->status_led);
      Serial.print(" LED: ");
      Serial.println(this->led);
    }
  } // SE GIA' ATTIVO NON FARE NULLA
}

void MyDomotic::on(void) {
  this->open();
}

// METODO PER APRIRE/CHIUDERE UNA LUCE ANCHE A TEMPO SE period > 0
void MyDomotic::change(void)
{
  if (digitalRead(this->led) == HIGH) {
    this->SetLed(this->led, LOW);
    this->status_led = "OPEN";
  } else {
    this->SetLed(this->led, HIGH);
    this->status_led = "CLOSE";
  }
}


void MyDomotic::action(void)
{
  if (this->type_object == "BLIND") {
    //VERIFICA SE IL PIN DEL RELE' CONTRARIO E' ACCESO
    if (digitalRead(this->led_check) == LOW /*&& this->btn_read == LOW*/) {
      // SE GIA' ERA ACCESO, BLOCCA ENTRAMBI
      if (DEBUG_SERIAL) Serial.println("2.2. BLOCCO: " + this->label + " RICHIESTO");
      this->lock();
    } else {
      // AZIONA APERTURA RELE'
      if (DEBUG_SERIAL) Serial.println("2.1 APERTURA RELE DI: " + this->label + " RICHIESTO");
      this->open();
    }
  } else if (this->type_object == "COMPLEX") {
    // TODO
  } else if (this->type_object == "SWITCH") {
    // SE E' PREVISTO IL TIMER, IL LED DEVE PASSARE DALLO STATO ATTUALE A QUELLO INVERSO
    // PER POI RITORNARE NELLO STESSO STATO
    this->change();
    if (this->period > 0) {
      this->delayled.start(this->period, AsyncDelay::MILLIS);
    }
  }
}

void MyDomotic::subMQTT(PubSubClient client) {
  this->client = &client;
  this->client_mqtt_enable = true;
  client.subscribe((char *)this->getTopic().c_str());
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
  String json = "{\"host\":\"" + this->ArduinoHost + "\", " + 
                "\"state\":\"ready\", " +
                "\"led\":\"" + LED + "\", " +
                "\"btn\":\"" + this->btn + "\", " +
                "\"status\":\"" + digitalRead(LED) + "\"}";
  if (this->client_mqtt_enable && this->led == LED) {
    this->client->publish("out", (char*)json.c_str());
  }
}

