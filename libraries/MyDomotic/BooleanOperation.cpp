/*
 BooleanOperation.cpp - 
 */

#include "BooleanOperation.h"  // dichiarazione della classe

/* funzione di inizializzazione */

BooleanOperation::BooleanOperation(int a, TypeInter::boolOperation oper)
{
//Utilizzata solo per il NOT
  if (oper == TypeInter::NOT)
  {
     mynot(a);
  }
  else
  {//ERRORE restitituisce sempre FALSE
    risultato= 0;   
  }
}


BooleanOperation::BooleanOperation(int a, int b, TypeInter::boolOperation oper)
{
      switch(oper) {
      case TypeInter::AND : risultato = (a && b);
               break;       // and exits the switch
      case TypeInter::OR : risultato = (a || b);
               break;   
      case TypeInter::NOT: //ERRORE restitituisce sempre FALSE
                risultato = 0;
               break;   
      default :   risultato = 0;            
    }
}

 
BooleanOperation::BooleanOperation(int a, int b, TypeInter::boolOperation oper, int c, TypeInter::boolOperation oper2)
{
     BooleanOperation(a,b,oper);     
     BooleanOperation(risultato,c,oper2);      
}
 
BooleanOperation::BooleanOperation(int a, int b, TypeInter::boolOperation oper, int c, TypeInter::boolOperation oper2, int d, TypeInter::boolOperation oper3)
{
   BooleanOperation(a,b,oper,c,oper2);
   BooleanOperation(risultato,d,oper3);
}

void BooleanOperation::mynot(int a)
 {
   risultato = !a;
 }
 
int BooleanOperation::get_risultato(void)
{
   return risultato;
}


