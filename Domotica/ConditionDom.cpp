/*
 ConditionDom.cpp - 
 */

#include "ConditionDom.h"  // dichiarazione della classe

/* funzione di inizializzazione */

ConditionDom::ConditionDom(float dato, TypeInter::condition_type condition , float value)
{
   datoi = (int) dato;
   mycondition = condition;
   valuef= value;
   temperatura= (dato); 
}

/*
ConditionDom::ConditionDom(int dato, TypeInter::condition_type condition, int value)
{
   datoi = dato;
   mycondition = condition;
   valuef= (float) value;
   temperatura=(float) (datoi); 
}*/

int ConditionDom::status(void)
{
          switch(mycondition) {
          case TypeInter::EQUAL : if (temperatura = valuef)
                    {
                     return 1;
                    } else {
                      return 0;
                    }
                   break;       // and exits the switch
          case TypeInter::MAJOR :if (temperatura > valuef)
                    {
                     return 1;
                    } else {
                      return 0;
                    }
                   break;   
          case TypeInter::MINOR :if (temperatura < valuef)
                    {
                     return 1;
                    } else {
                      return 0;
                    }
                   break;   
          default :  return 0;            
        }
}


