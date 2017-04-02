//RDAM start
/* CLASS ConditionDom */
#ifndef ConditionDom_h
#define ConditionDom_h
#include "TypeInter.h"
//#include <string>

class ConditionDom
{
private:
    /* Attributes */
/*    float dato;
    stringr condition_type; //=, <, >, !=
    float value;
*/

  //  float datof;
    int datoi;
    //string condition_type; //=, <, >, !=
//    enum condition_type { 
//        EQUAL = 0 , 
//        MAJOR = 1,
//        MINOR = 2 }; 
    TypeInter::condition_type mycondition;
    float valuef;
    //int valuei;   
    float temperatura;
public:
//Si può implementare anche un'altra di questo tipo: ConditionDom(int dato, String condition, int value);
    ConditionDom(float dato, TypeInter::condition_type condition, float value);
    //ConditionDom(int dato, TypeInter::condition_type condition, int value);
  //  ConditionDom(int dato, string condition, int value);
    int status(void);
};
#endif
//RDAM end
