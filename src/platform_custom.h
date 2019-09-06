#include <Arduino.h>

// #if ARDUINOTYPE == 1     //ARDUINO MEGA
//   const String BOARDNAMETYPE    =   "Arduino MEGA";
//   const char* MQTTTOPICPREFIX   =   "arduino_t";
//   const bool LOGICAL_LEVEL_WORK =   true;
//   const bool DOMOTICZ_WORK      =   true;
//   // PORTE                          { 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16};
//   const int digital_input []    =   {22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37};
//   const int digital_output []   =   {38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53};
//   const int digital_check []    =   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
//   const int digital_period []   =   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
//   const int digital_command []  =   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
//   const int analogic_input []   =   {A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15};
//   const int custom_input []     =   {15,16,17,18,19,20,21};
// #elif ARDUINOTYPE == 2   //ARDUINO UNO
//   const String BOARDNAMETYPE    =   "Arduino UNO";
//   const char* MQTTTOPICPREFIX   =   "arduino_t";
//   const bool LOGICAL_LEVEL_WORK =   true;
//   const bool DOMOTICZ_WORK      =   true;
//   const int digital_input []    =   {2,3,4,5};
//   const int digital_output []   =   {6,7,8,9};
//   const int digital_check []    =   {0,0,0,0};
//   const int digital_period []   =   {0,0,0,0};
//   const int digital_command[]   =   {0,0,0,0};
//   const int analogic_input []   =   {A0,A1,A2,A3,A4,A5};
//   #if ETHERNETSUPPORT == 2
//     const int custom_input []   =   {10,11,12};
//   #else
//     const int custom_input []   =   {};
//   #endif
// #endif

#if ARDUINOTYPE == 1     //ARDUINO MEGA A
  const String BOARDNAMETYPE    =   "Arduino MEGA A";
  const char* MQTTTOPICPREFIX  =   "arduino_a";
  const bool LOGICAL_LEVEL_WORK =   true;
  const bool DOMOTICZ_WORK      =   true;
  // PORTE                          { 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16};
  const int digital_input []    =   {22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37};
  const int digital_output []   =   {38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53};
  const int digital_check []    =   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  const float digital_period [] =   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.2, 0.2};
  const int digital_command []  =   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  const int analogic_input []   =   {};
  const int custom_input []     =   {};
#elif ARDUINOTYPE == 2     //ARDUINO MEGA B
  const String BOARDNAMETYPE    =   "Arduino MEGA B";
  const char* MQTTTOPICPREFIX  =   "arduino_b";
  const bool LOGICAL_LEVEL_WORK =   true;
  const bool DOMOTICZ_WORK      =   true;
  // PORTE                          { 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16};
  const int digital_input []    =   {22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37};
  const int digital_output []   =   {38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53};
  const int digital_check []    =   { 0, 0, 0, 0, 0,44,43, 0, 0, 0, 0, 0,51,50,53,52};
  const float digital_period [] =   { 0, 0, 0, 0, 0,14,14, 0, 0, 0, 0, 0,16,16,22,22};
  const int digital_command []  =   { 0, 0, 0, 0, 0,41,42, 0, 0, 0, 0, 0,40,40,40,40};
  const int analogic_input []   =   {};
  const int custom_input []     =   {};
#elif ARDUINOTYPE == 3     //ARDUINO MEGA C
  const String BOARDNAMETYPE    =   "Arduino MEGA C";
  const char* MQTTTOPICPREFIX  =   "arduino_c";
  const bool LOGICAL_LEVEL_WORK =   true;
  const bool DOMOTICZ_WORK      =   true;
  // PORTE                          { 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16};
  const int digital_input []    =   {22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37};
  const int digital_output []   =   {46,39,40,41,42,47,48,45,38,43,44,49,50,51,52,53};
  const int digital_check []    =   { 0, 0, 0, 0, 0, 0, 0, 0, 0,44,43, 0, 0, 0,53,52};
  const float digital_period [] =   { 0, 0, 0, 0, 0, 0, 0, 0, 0,14,14, 0, 0, 0, 7, 7};
  const int digital_command []  =   { 0, 0, 0, 0, 0, 0, 0, 0, 0,41,42, 0, 0, 0,41,42};
  const int analogic_input []   =   {};
  const int custom_input []     =   {};
#elif ARDUINOTYPE == 4     //ARDUINO MEGA D
  const String BOARDNAMETYPE    =   "Arduino MEGA D";
  const char* MQTTTOPICPREFIX  =   "arduino_d";
  const bool LOGICAL_LEVEL_WORK =   true;
  const bool DOMOTICZ_WORK      =   true;
  // PORTE                          { 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16};
  const int digital_input []    =   {22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37};
  const int digital_output []   =   {38,39,50,51,40,41,48,49,42,43,46,47,45,46,52,53};
  const int digital_check []    =   {39,38,51,50,41,40,49,48,43,42,47,46, 0, 0, 0, 0};
  const float digital_period [] =   {18,18,32,32,28,28,64,64,31,31,55,55, 0, 0, 0, 0};
  const int digital_command []  =   {40,40,40,40,40,40,40,40,40,40,40,40, 0, 0, 0, 0};
  const int analogic_input []   =   {};
  const int custom_input []     =   {};
#elif ARDUINOTYPE == 5     //ARDUINO MEGA E
  const String BOARDNAMETYPE    =   "Arduino MEGA E";
  const char* MQTTTOPICPREFIX  =   "arduino_e";
  const bool LOGICAL_LEVEL_WORK =   true;
  const bool DOMOTICZ_WORK      =   true;
  // PORTE                          { 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16};
  const int digital_input []    =   {22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37};
  const int digital_output []   =   {38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53};
  const int digital_check []    =   { 0, 0, 0, 0, 0, 0,45,44, 0, 0, 0,50,49, 0, 0, 0};
  const float digital_period [] =   { 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0};
  const int digital_command []  =   { 0, 0, 0, 0, 0, 0,40,40, 0, 0, 0,40,40, 0, 0, 0};
  const int analogic_input []   =   {A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15};
  const int custom_input []     =   {15,16,17,18,19,20,21};
#elif ARDUINOTYPE == 6     //ARDUINO MEGA G
  const String BOARDNAMETYPE    =   "Arduino MEGA G";
  const char* MQTTTOPICPREFIX  =   "arduino_g";
  const bool LOGICAL_LEVEL_WORK =   true;
  const bool DOMOTICZ_WORK      =   true;
  // PORTE                          { 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16};
  const int digital_input []    =   {22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37};
  const int digital_output []   =   {38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53};
  const int digital_check []    =   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  const float digital_period [] =   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  const int digital_command []  =   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  const int analogic_input []   =   {A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15};
  const int custom_input []     =   {15,16,17,18,19,20,21};
#endif
