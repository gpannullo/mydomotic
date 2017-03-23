/*
 MyDomotic.cpp - Libreria per gestire di MyDomotic
 */

#include "MyDomotic.h"  // dichiarazione della classe

/* funzione di inizializzazione */
MyDomotic::MyDomotic(){
    
}

MyDomotic::MyDomotic(int btn, int led)
{
    this->btn = btn;
    this->led = led;
    this->period = -1;
    this->type_object = "SWITCH";
    this->status_led = "";
}

MyDomotic::MyDomotic(int btn, int led, int period)
{
    this->btn = btn;
    this->led = led;
    this->period = period;
    this->type_object = "SWITCH_TIME";
    this->status_led = "";
}

MyDomotic::MyDomotic(int btn, int led, int period, int led_check){
    this->btn = btn;
    this->led = led;
    this->led_check = led_check;
    this->period = period;
    this->type_object = "BLIND";
    this->status_led = "";
}

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

void MyDomotic::lock(void)
{
  //SETTA IL PIN COME HIGH PER EVITARE PICCHI DURANTE LA FARE DI ASSEGNAZIONE OUTPUT
  digitalWrite(this->led, HIGH);
  if(type_object == "BLIND"){
    digitalWrite(this->led_check, HIGH);  
  }
  this->status_led = "LOCK";
  Serial.println(this->status_led);
}

void MyDomotic::setup(void)
{
    lock();
    if (type_object == "SWITCH"){
        setup_switch();
    }
    else if(type_object == "BLIND"){
        setup_blind();
    }
    else if(type_object == "COMPLEX"){  //RDAM
        setup_complex();        
    }
}


//VERIFICA SE E' STATO PREMUTO IL BOTTONE DI INPUT
//L'EVENTO DI INPUT C'E' SOLO SE IL BOTTONE NORMALMENTE HIGH PASSA A LOW
void MyDomotic::check_btn_state(void)
{
  if (this->btn_state != digitalRead(this->btn) && digitalRead(this->btn) == LOW) {
    //ESEGUE LE AZIONI PREVISTE
    action();
  }
  //AGGIORNA LO STATO DEL BOTTONE
  this->btn_state = digitalRead(this->btn);
}

// METODO DI LOOP
void MyDomotic::loop(void)
{
  this->check_btn_state();
  //VERIFICA SE IL TIMER HA FINITO
  if (this->delayled.isExpired() && digitalRead(this->led) == LOW ) {
    digitalWrite(this->led, HIGH);
    status_led = "CLOSE";
    Serial.println(status_led);
  }
  /*  if (this->type_object == "SWITCH") 
    {
        
    } 
    else if (this->type_object == "BLIND") 
    {
      
    } 
    else if (this->type_object == "COMPLEX")  //RDAM Completare
    {       
       if (this->btn_state == LOW ) 
       {  
           BooleanOperation valida = BooleanOperation(digitalRead(btn_rain), temperature_ok, TypeInter::AND); 
           if (valida.get_risultato())
           {
              digitalWrite(btn_rele, HIGH);
              digitalWrite(btn_rele2, LOW);
      //  delay_UP.start(period_up, AsyncDelay::MILLIS); 
           }
       }
    }
  */
}

char* MyDomotic::getTopic(void){ 
  // OGNI INPUT E' VISTO COME UN EVENTO DI PRESSIONE BOTTONE
  // VENGONO PUBBLICATI GLI EVENTI CON IL NUMERO BOTTONE
  return "action/"+(char) this->btn;  
}

void MyDomotic::setup_switch(void){
    pinMode(this->led, OUTPUT);
    pinMode(this->btn, INPUT_PULLUP);
    this->btn_state = digitalRead(this->btn);
}

void MyDomotic::setup_blind(void){
    setup_switch();
    pinMode(this->led_check, OUTPUT);
}

//RDAM START : Implementare come si deve
void MyDomotic::setup_complex(void){  
    pinMode(btn, INPUT_PULLUP);
    //pinMode(led, OUTPUT);
    btn_state = digitalRead(btn);
    pinMode(btn_rain, INPUT);
    pinMode(btn_rele, OUTPUT);
    pinMode(btn_rele2, OUTPUT);
    
}
//RDAM END


// METODO PER APRIRE/CHIUDERE LA PERSIANA
// IL METEDO PREVEDE LA CHIUSURA DEL RELE' DI DISCESA (IN CASO DI SALITA)
// E POI ACCENDE IL RELE' DI SALITA PER xxx TEMPO. NELLA FUNZIONE loop()
// SI RICHIUDE PASSATO IL TEMPO xxx

void MyDomotic::open(void)
{
  //VERIFICA CHE IL RELE' DI SALITA/DISCESA NON SIA GIA' IN AZIONE
  if (digitalRead(this->led) == HIGH){
    // CHIUDE IL RELE' DI DISCESA/SALITA
    digitalWrite(this->led_check, HIGH);
    //ABILITA IL RELE' DI SALITA/DISCESA
    digitalWrite(this->led, LOW);
    this->delayled.start(this->period, AsyncDelay::MILLIS); 
    this->status_led = "MOVING";
    Serial.print(this->status_led);Serial.print("LED: ");Serial.println(this->led);
  } // SE GIA' ATTIVO NON FARE NULLA
}

// METODO PER APRIRE/CHIUDERE UNA LUCE ANCHE A TEMPO SE period > 0
void MyDomotic::change(void)
{
  if (digitalRead(this->led) == HIGH){
    digitalWrite(this->led, LOW);
    this->status_led = "OPEN";
  }else{
    digitalWrite(this->led, HIGH);
    this->status_led = "CLOSE";
  }
  Serial.print(this->status_led);Serial.print("LED: ");Serial.println(this->led);
}


void MyDomotic::action(void)
{
  if (this->type_object == "BLIND"){
    //VERIFICA SE IL PIN DEL RELE' CONTRARIO E' ACCESO
    if(digitalRead(this->led_check) == LOW){
      // SE GIA' ERA ACCESO, BLOCCA ENTRAMBI
      this->lock();
    }else{
      // AZIONA APERTURA RELE'
      this->open();
    }
  }else if(this->type_object == "COMPLEX"){
    // TODO
  }else if(this->type_object == "SWITCH"){
    // SE E' PREVISTO IL TIMER, IL LED DEVE PASSARE DALLO STATO ATTUALE A QUELLO INVERSO
    // PER POI RITORNARE NELLO STESSO STATO
    if(this->period > 0){
      this->change();
      this->delayled.start(this->period, AsyncDelay::MILLIS);
    }else{
      this->change();
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
