#include <Arduino.h>

#if ARDUINOTYPE == 4096     //ARDUINO MEGA
  const String BOARDNAMETYPE = "Arduino MEGA";
  const int digital_input []  =   {22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37};
  const int digital_output [] =   {38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53};
  const int analogic_input [] =   {A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15};
  const int custom_input []   =   {14,15,16,17,18,19,20,21};
#elif ARDUINOTYPE == 1024   //ARDUINO UNO
  const String BOARDNAMETYPE = "Arduino UNO";
  const int digital_input [] =  {2,3,4,5};
  const int digital_output [] = {6,7,8,9};
  const int analogic_input [] = {A0,A1,A2,A3,A4,A5};
  #if ETHERNETSUPPORT == 1
    const int custom_input [] = {10,11,12};
  #else
    const int custom_input [] = {};
  #endif
#elif ARDUINOTYPE == 512
  const String BOARDNAMETYPE = "Arduino NANO";
  const int digital_input [] = {2,3,4,5};
  const int digital_output [] = {6,7,8,9};
  const int analogic_input [] = {A0,A1,A2,A3,A4,A5};
  #if ETHERNETSUPPORT == 1
    const int custom_input [] = {10,11,12};
  #else
    const int custom_input [] = {};
  #endif
#else
  #error "Define 'ARDUINOTYPE' in your paltformio.ini"
#endif
