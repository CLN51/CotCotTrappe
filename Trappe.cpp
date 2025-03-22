//-----------------------------------------//
//                                         //
//             TrappePoule                 //  
//             Trappe.cpp                  //
//                                         //
//-----------------------------------------//

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <arduino.h>

#include "Trappe.h"
#include "simpleMinuteur.h"
#include "Eprom.h"

#define WATCHDOGINIT   15000              // Durée maximale de recherche du point haut, lors de la phase d'initialisation

volatile long  CommandeTrappe::Pos;        // Déclaration de la varial Pos déclarée en static dans la classe, elle renseigne la position de l'encodeur
volatile   uint8_t CommandeTrappe::Etat;   // Déclaration de la variale Etat déclarée en statique dans la classe:  FERME, OUVERT, MONTE ou DESCEND
simpleMinuteur WatchDogInit(WATCHDOGINIT); // Déclaration du minuteur WatchDog utilisé lors de la phase d'initialisation

extern IntEeprom EepromHauteurOuverture;   // hauteur d'ouverture de la trappe
extern IntEeprom EepromPasMoteur;          // Pas moteur pour 1 cm
extern IntEeprom EepromTimeoutTrappe;      // Time out manoeuvre de la trappe
extern IntEeprom EepromVMinMoteur;         // Vitesse minimun de la trappe (démarrage)
extern IntEeprom EepromVMaxMoteur;         // vitesse maximun de la trappe
extern IntEeprom EepromPresenceFdc;        // indication de présence d'un fin de course en haut de la trappe
static unsigned long TopPrec;                 // top temp pour l'augmentation de la vitesse
static unsigned long TopMaintenant;           // top temp pour l'augmentation de la vitesse



/**********  
*   construteur de la classe
*      In1Pin     : Première sortie d'alimentation du moteur
*      In2Pin     : Deuxième sortie d'alimentation du moteur
*      PwmPin     : Sortie pour variation de la vitesse.
*      StbyPin    : Sortie de commande StandBy
*      EncPin     : Entrée de l'encodeur de position
*      ShuntPin   : Entrée du shunt de courant
*      VitesseMax : vitesse mas du moteur de 0 à 255
*      Courant    : Courant de blocage du moteur
*      Sens       : sens par défaut du moteur, peut prendre les valeurs -1 et 1. 1 par défaut
*/
CommandeTrappe::CommandeTrappe(uint8_t In1Pin, uint8_t In2Pin, uint8_t PwmPin, uint8_t StbyPin, uint8_t EncPin, uint8_t FdcPin, uint8_t SensDefaut = 1)
  {
  In1  = In1Pin;
  In2  = In2Pin;
  Pwm  = PwmPin;
  Stby = StbyPin;
  Enc  = EncPin;
  Fdc  = FdcPin;
  Sens = SensDefaut;  

  pinMode(In1,  OUTPUT);
  pinMode(In2,  OUTPUT);
  pinMode(Pwm,  OUTPUT);
  pinMode(Stby, OUTPUT);
  pinMode(Enc,  INPUT);
  pinMode(Fdc,  INPUT);
 
  this->Stop();
  attachInterrupt(digitalPinToInterrupt(EncPin), Comptage, RISING);
  }

/***********
*   Fonction Init, Fonction D'initialisation de la postion de la trappe
*                  A appeler avant toute utilisation.
*                  La fonction remonte la porte jusqu'a la butée
*                  puis la redescend légèrement
*                  Cette fonction est blocante.
*     Position : position de la trappe
*/
void CommandeTrappe::Init()
  {
  static uint8_t Vitesse = EepromVMinMoteur.Valeur();  // vitesse initiale
  int courant = 0;                   // moyenne des mesures du shunt 

  
  int i = 0;      
  long Somme = 0l;
  int Mesure;

    WatchDogInit.demarrer(EepromTimeoutTrappe.Valeur() * 1000);

    Vitesse = EepromVMinMoteur.Valeur();  
    Etat = MONTE;  
  while ( (digitalRead(Fdc)) && (!WatchDogInit.estTermine()))   // on remonte la trappe jusqu'a la butée.
    {
     Monte (Vitesse); 
    //accélération
    if (Vitesse < EepromVMaxMoteur.Valeur()) 
       {
       TopMaintenant = millis();
       if ((TopMaintenant - TopPrec) > 120)
         {  
         Vitesse ++;
         TopPrec=TopMaintenant;
         } 
       }   
   }
  Etat = OUVERT;
  Stop();
  Pos = 0L;
  }

/***********
*   Fonction InitPos, Fonction permettant de renseigner la position de la trappe
*     Position : position de la trappe
*/
void CommandeTrappe::InitPos(long Position)
  {
  Pos = Position;
  }


/***********
*   Fonction Ouvrir, Fonction d'ouverture de la trappe en fontion de la présence ou non d'un fin de course
*/
void CommandeTrappe::Ouvrir()
  {
    if ( EepromPresenceFdc.Valeur()==1)
      {
      this->OuvrirFdc();
      }
    else
      {
      this->OuvrirPos();  
      }
  }
/***********
*   Fonction Ouvrir, Fonction d'ouverture de la trappe en fontion de sa position
*/
void CommandeTrappe::OuvrirPos()
  {
static uint8_t Vitesse;      // vitesse initiale
  if (Etat == FERME)
    {
    Vitesse = EepromVMinMoteur.Valeur();  
    TopPrec = millis();
    Etat = MONTE;  
    WatchDogInit.demarrer(EepromTimeoutTrappe.Valeur() * 1000);
    }
  if (Etat == MONTE)
    {
    if ( (Pos >0) && (!WatchDogInit.estTermine() ))
       {
       Monte (Vitesse); 
       }
    else 
       {
       Stop();    
       Etat = OUVERT; 
       }
    //accélération
    if (Vitesse < EepromVMaxMoteur.Valeur()) 
       {
       TopMaintenant = millis();
       if ((TopMaintenant - TopPrec) > 120)
         {  
         Vitesse ++;
         TopPrec=TopMaintenant;
         } 
       }   
    }
  }

/***********
*   Fonction Ouvrir, Fonction d'ouverture de la trappe en utilisant de fin de course haut
*/
void CommandeTrappe::OuvrirFdc()
  {
static uint8_t Vitesse;      // vitesse initiale
  if (Etat == FERME)
    {
    Vitesse = EepromVMinMoteur.Valeur();  
    Etat = MONTE;  
    WatchDogInit.demarrer(EepromTimeoutTrappe.Valeur() * 1000);
    }
  if (Etat == MONTE)
    {
    if ( (digitalRead(Fdc)) && (!WatchDogInit.estTermine() ))
       {
       Monte (Vitesse); 
       }
    else 
       {
       Etat = OUVERT; 
       Stop();    
       Pos = 0L;
       }
    //accélération
    if (Vitesse < EepromVMaxMoteur.Valeur()) 
       {
       TopMaintenant = millis();
       if ((TopMaintenant - TopPrec) > 120)
         {  
         Vitesse ++;
         TopPrec=TopMaintenant;
         } 
       }   
    }
    
  }

/***********
*   Fonction Fermer, Fonction de fermeture de la trappe
*/
void CommandeTrappe::Fermer()
  {
  static uint8_t Vitesse;  // vitesse initiale
  int PositionBasse = EepromHauteurOuverture.Valeur()*EepromPasMoteur.Valeur();
  
  if (Etat == OUVERT)
    {
    Vitesse = EepromVMinMoteur.Valeur();  
    TopPrec = millis();
    Etat = DESCEND;  
    }
  if (Etat == DESCEND)
    {
    if ( (Pos < PositionBasse) )
       {
       Descend (Vitesse); 
       }
    else 
       {
       Stop();    
       Etat = FERME; 
       }
    //accélération
    if (Vitesse < EepromVMaxMoteur.Valeur()) 
       {
       TopMaintenant = millis();
       if ((TopMaintenant - TopPrec) > 120)
         {  
         Vitesse ++;
         TopPrec=TopMaintenant;
         } 
       }   
    }
  }
  
/***********
*   Fonction Monte, permet de monter la trappe
*     Vitesse : Vitesse de monté de la trappe de 0 à 255.
*/
void CommandeTrappe::Monte(int Vitesse)
  {
   digitalWrite(Stby, HIGH);
   digitalWrite(In1, Sens >0 ? HIGH : LOW);
   digitalWrite(In2, Sens >0 ? LOW  : HIGH);
   analogWrite(Pwm, Vitesse);
  }

/***********
*   Fonction descend, permet de descendre la trappe
*     Vitesse : Vitesse de monté de la trappe de 0 à 255.
*/
void CommandeTrappe::Descend(int Vitesse)
  {
   digitalWrite(Stby, HIGH);
   digitalWrite(In1, Sens >0 ? LOW : HIGH);
   digitalWrite(In2, Sens >0 ? HIGH  : LOW);
   analogWrite(Pwm, Vitesse);
  }

/***********
*   Fonction Stop, arrête la trappe et passe le circuit de commante en standby;
*/
void CommandeTrappe::Stop()
  {
  digitalWrite(In1, HIGH);      // Met les 2 entrées du moteur à la masse
  digitalWrite(In2, HIGH);      // créant arrêt freiné
  analogWrite(Pwm, 0);
  digitalWrite(Stby,LOW);  
  }

/***********
*   Fonction Position, renvoi la positon de la trappe;
*/
long CommandeTrappe::Position()
  {
  return Pos;
  }

/***********
*   Fonction comptage : permet de déterminer la position de la trappe par comptage du mombre d'impultions;
*/
inline  void CommandeTrappe::Comptage()
  {
   if ((Etat == MONTE) || (Etat == OUVERT))
      Pos--;   
   else
      Pos++; 
  }
