//-----------------------------------------//
//                                         //
//             TrappePoule                 //  
//          ChargeBatterie.cpp             //
//                                         //
//-----------------------------------------//
#include <arduino.h>
#include "ChargeBatterie.h"

#define EntreeTension A1

static uint8_t Boucle=0;
static int SommeTension=0;

/*****
 * TableauCapa : Tableau récapitulant les charges théorique en fonction de la tension de la batterie
 *               - La tension est exprimée en centième de volt - 300 afin de tenir sur 8 bits
 *               - La capacitée est exprimes en pourcentage
 */

Capacite TableauCapa[] = {
                   {0,0},
                   {30,5},
                   {60,10},
                   {70,20},
                   {75,30},
                   {79,40},
                   {83,50},
                   {87,60},
                   {92,70},
                   {97,80},
                   {110,90},
                   {120,100}
                   };

/*****
 * ChargeBatterie : Fonction renvoyant le taux de charge de la batterie
 *     Tension : tension aux bornes de la batterie exprimée en millivolt. 
 *     renvoi le pourcentage de charge restante
 */
uint8_t ChargeBatterie()
{
  uint8_t i;
  int     Lue= analogRead(EntreeTension);
  int     Tension=Lue/2;

  if (Boucle <50)
    {
      SommeTension+=Tension;
      Boucle++;
      return;  
    }
    
  Tension = SommeTension/Boucle;
  Boucle=0;
  SommeTension=0;

  
  if (Tension < 300)
      return 0;
  if (Tension >= 420)
      return 100;
  Tension -= 300;
  while ( i < 11)
    {
    if ((Tension >= TableauCapa[i].Tension) && (Tension < TableauCapa[i+1].Tension))
      {
      float DiffTension     = (TableauCapa[i+1].Tension-TableauCapa[i].Tension); 
      float DiffPourcent    = (TableauCapa[i+1].Pourcent-TableauCapa[i].Pourcent); 
      float Volt = Tension - TableauCapa[i].Tension;
      return TableauCapa[i].Pourcent+ (DiffPourcent*Volt / DiffTension);
      }          
    i++;
    }
   
}
