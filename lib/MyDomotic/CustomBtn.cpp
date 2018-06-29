/*
  CustomBtn.cpp - Libreria per gestire di MyDomotic
*/

#include "CustomBtn.h"  // dichiarazione della classe

/* funzione di inizializzazione */
CustomBtn::CustomBtn() {

}

CustomBtn::CustomBtn(CustomPin data) {
  this->data = data;
}

void CustomBtn::set_setting(CustomPin data) {
  this->data = data;
  this->save();
}

void CustomBtn::setup(void)
{
  pinMode(this->data.btn, INPUT_PULLUP);
  this->btn_state = digitalRead(this->data.btn);
  this->btn_read = this->btn_state;
}


//VERIFICA SE E' STATO PREMUTO IL BOTTONE DI INPUT
//L'EVENTO DI INPUT C'E' SOLO SE IL BOTTONE NORMALMENTE HIGH PASSA A LOW
void CustomBtn::check_btn_state(void)
{
  this->btn_read = digitalRead(this->data.btn);
  if (this->btn_state != this->btn_read && this->btn_read == LOW) {
    //PER EVITARE PICCHI DI CORRENTE NON PREVISTI SI MISURA UN RITADO
    //SULL?ATTIVAZIONE DEL BOTTONE
    delay(50);
    if(digitalRead(this->data.btn) == LOW){
      //ESEGUE LE AZIONI PREVISTE
      this->action();
      delay(10);
    }
  }
  //AGGIORNA LO STATO DEL BOTTONE
  this->btn_state = digitalRead(this->data.btn);
}

// METODO DI LOOP
void CustomBtn::loop(void)
{
  this->check_btn_state();
}

void CustomBtn::action(void)
{
  #if ETHERNETSUPPORT == 1 or ETHERNETSUPPORT == 2
  this->client->publish(this->data.topic, this->data.mqtt);
  #endif
}

#if ETHERNETSUPPORT == 1
  void CustomBtn::mqttset(PubSubClient mqtt){
    this->client = &mqtt;
  }
#elif ETHERNETSUPPORT == 2
  void CustomBtn::mqttset(ELClientMqtt mqtt){
    this->client = &mqtt;
  }
#endif

// METODO PER CAMBIARE DI STATO UN LED
void CustomBtn::save() {
  EEPROM.put(this->data._eepromAddress, this->data);
}
