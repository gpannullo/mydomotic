//RDAM start
/* CLASS BooleanOperation */
#ifndef BooleanOperation_h
#define BooleanOperation_h
#include "TypeInter.h"
class BooleanOperation
{
private:
    /* Attributes */
    int A;
    int B;
    int C;
    int D;
    int risultato;
   /* enum boolOperation { 
        AND = 0 , 
        OR = 1,
        NOT = 2 }; 
   */
public:

    BooleanOperation(int a, TypeInter::boolOperation oper );
    BooleanOperation(int a, int b, TypeInter::boolOperation oper );
    BooleanOperation(int a, int b, TypeInter::boolOperation oper, int c, TypeInter::boolOperation oper2 );
    BooleanOperation(int a, int b, TypeInter::boolOperation oper, int c, TypeInter::boolOperation oper2, int d, TypeInter::boolOperation oper3 );
  
    int operation(int a, int b);
    
    void mynot(int a);    
    int get_risultato(void);
    
    
};
#endif
//RDAM end
