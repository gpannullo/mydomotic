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
  if(this->data.type_object == MYDC_TYPE_MQTT_BTN){
    pinMode(this->data.btn, INPUT_PULLUP);
    this->btn_state = digitalRead(this->data.btn);
    this->btn_read = this->btn_state;
  }else if(this->data.type_object == MYDC_TYPE_DIGITAL_LEVEL){
    pinMode(this->data.btn, INPUT_PULLUP);
    this->btn_state = digitalRead(this->data.btn);
    this->btn_read = this->btn_state;
  }
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

void CustomBtn::check_input(void)
{
  this->btn_read = digitalRead(this->data.btn);
  if (this->btn_state != this->btn_read && this->delayled.isExpired()) {
    send_status_action=true;
    this->delayled.start(30000, AsyncDelay::MILLIS); //TIMER PER 10s
  }
}

// METODO DI LOOP
void CustomBtn::loop(void)
{
  if(this->data.type_object == MYDC_TYPE_MQTT_BTN)
  {
    this->check_btn_state();
  } else if (this->data.type_object == MYDC_TYPE_DIGITAL_LEVEL)
  {
    this->check_input();
  }
}

String CustomBtn::to_small_json(void)
{
  if (this->data.type_object == MYDC_TYPE_DIGITAL_LEVEL){
    // CONVERSIONE DETTAGLI SETTINGS OGGETTO IN JSON
    String state = "CLOSE";
    if (digitalRead(this->data.btn) == LOW ){
      state = "OPEN";
    }
    return  "\"ST" + (String) this->data.btn + "\":\"" + state + "\"";
  }else{
    return "";
  }
}

void CustomBtn::action(void)
{
}

// METODO PER CAMBIARE DI STATO UN LED
void CustomBtn::save() {
  EEPROM.put(this->data._eepromAddress, this->data);
}
