#include <Arduino.h>

#if ARDUINOTYPE == 1     //ARDUINO MEGA A
  const String BOARDNAMETYPE    =   "Arduino MEGA A";
  const bool LOGICAL_LEVEL_WORK =   true;
  const bool DOMOTICZ_WORK      =   true;
  const int digital_input []    =   {22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37};
  const int digital_output []   =   {38,39,46,47,40,41,48,49,42,43,50,51,44,45,52,53};
  const int digital_check []    =   {39,38,47,46,41,40,49,48,43,42,51,50,0,0,0,0};
  const int digital_period []   =   {10,10,10,10,10,10,10,10,10,10,10,10,0,0,0,0};
  const int digital_command []  =   {40,40,40,40,40,40,40,40,40,40,40,40,0,0,0,0};
  const int analogic_input []   =   {A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15};
  const int custom_input []     =   {15,16,17,18,19,20,21};
#elif ARDUINOTYPE == 2     //ARDUINO MEGA B
  const String BOARDNAMETYPE    =   "Arduino MEGA B";
  const bool LOGICAL_LEVEL_WORK =   true;
  const bool DOMOTICZ_WORK      =   true;
  const int digital_input []    =   {22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37};
  const int digital_output []   =   {38,39,46,47,40,41,48,49,42,43,50,51,44,45,52,53};
  const int digital_check []    =   {39,38,47,46,41,40,49,48,43,42,51,50,0,0,0,0};
  const int digital_period []   =   {10,10,10,10,10,10,10,10,10,10,10,10,0,0,0,0};
  const int digital_command []  =   {40,40,40,40,40,40,40,40,40,40,40,40,0,0,0,0};
  const int analogic_input []   =   {A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15};
  const int custom_input []     =   {15,16,17,18,19,20,21};
#elif ARDUINOTYPE == 3     //ARDUINO MEGA C
  const String BOARDNAMETYPE    =   "Arduino MEGA C";
  const bool LOGICAL_LEVEL_WORK =   true;
  const bool DOMOTICZ_WORK      =   true;
  const int digital_input []    =   {22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37};
  const int digital_output []   =   {38,39,46,47,40,41,48,49,42,43,50,51,44,45,52,53};
  const int digital_check []    =   {39,38,47,46,41,40,49,48,43,42,51,50,0,0,0,0};
  const int digital_period []   =   {10,10,10,10,10,10,10,10,10,10,10,10,0,0,0,0};
  const int digital_command []  =   {40,40,40,40,40,40,40,40,40,40,40,40,0,0,0,0};
  const int analogic_input []   =   {A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15};
  const int custom_input []     =   {15,16,17,18,19,20,21};
#elif ARDUINOTYPE == 4     //ARDUINO MEGA D
  const String BOARDNAMETYPE    =   "Arduino MEGA D";
  const bool LOGICAL_LEVEL_WORK =   true;
  const bool DOMOTICZ_WORK      =   true;
  const int digital_input []    =   {22,23,24,25,26,27,28,29,30,31,32,33};
  const int digital_output []   =   {38,39,50,51,40,41,48,49,42,43,46,47};
  const int digital_check []    =   {39,38,51,50,41,40,49,48,43,42,47,46};
  const int digital_period []   =   {18,18,32,32,38,38,30,30,31,31,30,30};
  const int digital_command []  =   {40,40,40,40,40,40,40,40,40,40,40,40};
  const int analogic_input []   =   {A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15};
  const int custom_input []     =   {15,16,17,18,19,20,21};
#elif ARDUINOTYPE == 5     //ARDUINO MEGA E
  const String BOARDNAMETYPE    =   "Arduino MEGA E";
  const bool LOGICAL_LEVEL_WORK =   true;
  const bool DOMOTICZ_WORK      =   true;
  const int digital_input []    =   {22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37};
  const int digital_output []   =   {38,39,46,47,40,41,48,49,42,43,50,51,44,45,52,53};
  const int digital_check []    =   {39,38,47,46,41,40,49,48,43,42,51,50,0,0,0,0};
  const int digital_period []   =   {10,10,10,10,10,10,10,10,10,10,10,10,0,0,0,0};
  const int digital_command []  =   {40,40,40,40,40,40,40,40,40,40,40,40,0,0,0,0};
  const int analogic_input []   =   {A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15};
  const int custom_input []     =   {15,16,17,18,19,20,21};
#elif ARDUINOTYPE == 6     //ARDUINO MEGA G
  const String BOARDNAMETYPE    =   "Arduino MEGA G";
  const bool LOGICAL_LEVEL_WORK =   true;
  const bool DOMOTICZ_WORK      =   true;
  const int digital_input []    =   {22,23,24,25,26,27,28,29,30,31};
  const int digital_output []   =   {38,39,40,41,42,43,44,45,46,47};
  const int digital_check []    =   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  const int digital_period []   =   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  const int digital_command []  =   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  const int analogic_input []   =   {A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15};
  const int custom_input []     =   {15,16,17,18,19,20,21};
#endif
