//-----------------------------------------//
//                                         //
//         TrappePoule V1.02.00            //  
//                                         //
//-----------------------------------------//
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <BH1750.h>
#include <LowPower.h>


#include <RTClib.h>
#include "Ephemeride.h"
#include "Menu.h"
#include "Eprom.h"
#include "simpleBouton.h"
#include "simpleMinuteur.h"
#include "ChargeBatterie.h"
#include "Trappe.h"

#define SIMULHEURE            false      // simulation de l'heure
#define PASSIMULHEURE          200       // saut en seconde de l'horloge simulée par tour
#define PINBOUTON1              10       // Définition de l'entrée du bouton 1 
#define PINBOUTON2              11       // Définition de l'entrée du bouton 2 
#define PINBOUTON3              12       // Définition de l'entrée du bouton 3 
#define PINBOUTON4              13       // Définition de l'entrée du bouton 4 
#define PINSERVO                6        // Définition de la sortie du servomoteur 
#define PINLUMIERE              5        // Sortie pour la commande de lumière
#define PININTERRUPTEUR         4        // entré de l'interrupteur
#define PINCOMMANDETRIAC        A0       // entré de commande de la gachette du triac
#define DUREERETROECLAIRAGE  60000       // Définition de la durée du rétroéclairage en millisecondes donc 1 minutes 
#define DUREEMENU           300000       // Définition de la durée que l'on reste dans les menus avant retour à l'écran 1, 5 minutes
#define DUREEINFO             1000       // Définition de la durée d'affichage de l'écran d'information, 5 seondes
#define DELAISINTERRUPTEUR    1000       // délais entre 2 alummage manuel pour différencier interrupteur et BP

// défintions pour le moteur de trappe
#define MOTEURIN1     7
#define MOTEURIN2     8
#define MOTEURPWM     6
#define MOTEURSTBY    9
#define MOTEURENC     3
#define MOTEURFDC     A3


/*****
 * Déclaration du menu
 */
Menu MonMenu(0x27,20,4);                        // initialisation du menu Lcd adresse 3F, 4 lignes de 20 carcatères 
uint8_t MenuCourant;                            // Numéro du menu courant
uint8_t NiveauMenu=1;                           // Niveau de menu
uint8_t ParamCourant=1;                         // numéro de paramètre courant
bool RetroEclairage=true;                       // par défaut le rétro éclairage est actif
int8_t ModeAuto = MODEINIT;                     // mode de fonctionnement courant AUTO ou MANU
//uint8_t PosTrappe = FERME;                    // Position de la trappe
uint8_t TrappeModeManu = TrappeManuFixe;        // fonctionnement de la trappe en mode Manu ?
uint16_t NiveauLux = 150;                       // niveau de lumière détecté par le luxmètre en %
DateTime DateCourante (2020, 11, 12, 07, 35);   // Date courante
DateTime DateHorlogeUTC;                        // Date UTC
uint8_t DecalageUTC;                            // Décalage en minute entre l'heure locale et UTC
bool    LumiereForcee = false;                  // gestion forcée de la lumière quelque soit le mode  
int8_t  NiveauLumiere=0;                        // Niveau de la lumière
boolean PremierMEnu0=false;                     // On n'est pas encore passé par le menu 0 

// chargement des variable de l'Eeprom
IntEeprom EepromLongDegre(EEPROMLONGDEGRE);                // Degré de la longitude
IntEeprom EepromLongMinute(EEPROMLONGMINUTE);              // Minute de la longitude
IntEeprom EepromLongSeconde(EEPROMLONGSECONDE);            // seconde de la longitude
IntEeprom EepromLatDegre(EEPROMLATDEGRE);                  // Degré de la latitude
IntEeprom EepromLatMinute(EEPROMLATMINUTE);                // Minute de la latitude
IntEeprom EepromLatSeconde(EEPROMLATSECONDE);              // seconde de la latitude
IntEeprom EepromDelaisOuverture(EEPROMDELAISOUVERTURE);    // Delais d'ouverture de la trappe
IntEeprom EepromDelaisFermeture(EEPROMDELAISFERMETURE);    // Delais de fermeture de la trappe
IntEeprom EepromHauteurOuverture(EEPROMHAUTEUROUVERTURE);  // Hauteur de la trappe en cm
IntEeprom EepromPasMoteur(EEPROMPASMOTEUR);                // Pas du moteur pour un cm de trappe
IntEeprom EepromTimeoutTrappe(EEPROMTIMEOUTTRAPPE);        // Timeout lors de l'ouverture et fermeture de la trappe
IntEeprom EepromDelaisAllumage(EEPROMDELAISALLUMAGE);      // Delais d'allumage en minutes
IntEeprom EepromTempsAllumage(EEPROMTEMPSALLUMAGE);        // Temp allumage pour passer de 0 à 100%
IntEeprom EepromDureeAllumage(EEPROMDUREEALLUMAGE);        // Durée allumage après les 100%
IntEeprom EepromHeureOuverture(EEPROMHEUREOUVERTURE);      // Heure d'ouverture en mode horaire
IntEeprom EepromMinuteOuverture(EEPROMMINUTEOUVERTURE);    // Minute d'ouverture en mode horaire
IntEeprom EepromHeureFermeture(EEPROMHEUREFERMETURE);      // Heure de fermenture en mode horaire
IntEeprom EepromMinuteFermeture(EEPROMMINUTEFERMETURE);    // Minute de fermeture en mode horaire
IntEeprom EepromLuminositeBasse(EEPROMLUMINOSITEBASSE);    // seuil de déclenchement luminosité basse en mode luminosité
IntEeprom EepromLuminositeHaute(EEPROMLUMINOSITEHAUTE);    // seuil de déclenchement luminosité haute en mode luminosité
IntEeprom EepromDelaisLux(EEPROMDELAISLUX);                // hystéresis en mode Nature
IntEeprom EepromVMinMoteur(EEPROMVMINMOTEUR);              // Vitesse minimun du moteur
IntEeprom EepromVMaxMoteur(EEPROMVMAXMOTEUR);              // Vitesse maximun du moteur
IntEeprom EepromDureeMinuterie(EEPROMDUREEMINUTERIE);      // Durée de la minuterie en minute
IntEeprom EepromPresenceFdc(EEPROMPRESENCEFDC);            // Présence du fin de course haute sur la trappe

/*****
 * Constantes regroupants le nom des touches des menus
 */
const char MVersion[]        = "V1.02.00";   // Numéro de version du logiciel
const char MDateVersion[]    = "03-2025";   // Date de rélisation sous la forme MM/AAAA

/*****
 * Constantes regroupants les formats d'impression des paramètres
 */
const char Format02d[]       = "%02d";  // Format décimal sur 2 caractères précédés avec des 0 si besoin 
const char Format1d[]        = "%1d";   // Format décimal sur 1 caractères
const char Format2d[]        = "%2d";   // Format décimal sur 2 caractères
const char Format3d[]        = "%3d";   // Format décimal sur 3 caractères
const char Format4d[]        = "%4d";   // Format décimal sur 4 caractères
const char FormatLux[]       = "%4dL";  // Format d'affichage des données Lux
/*****
 * Définition des boutons
 */
simpleBouton Bouton1(PINBOUTON1);            // Déclaration de l'objet bouton 1
simpleBouton Bouton2(PINBOUTON2);            // Déclaration de l'objet bouton 2
simpleBouton Bouton3(PINBOUTON3);            // Déclaration de l'objet bouton 3
simpleBouton Bouton4(PINBOUTON4);            // Déclaration de l'objet bouton 4
simpleBouton Interrupteur(PININTERRUPTEUR);  // Déclaration de l'interrupteur
/*****
 * Définition du moteur de trappe
 */
CommandeTrappe Trappe(MOTEURIN1, MOTEURIN2, MOTEURPWM, MOTEURSTBY, MOTEURENC, MOTEURFDC); 

/*****
 * Définition des minuteurs
 */
simpleMinuteur MinuteurRetroeclairage(DUREERETROECLAIRAGE);                 // minuteur de durée de fonctionnement du rétroéclairage
simpleMinuteur MinuteurMenu(DUREEMENU);                                     // Minuteur de sortie d'un mode menu avec retour sur l'écran principal
simpleMinuteur MinuteurInfo(DUREEINFO);                                     // Minuteur de durée d'affichage de l'écran d'info
simpleMinuteur MinuteurLuxBas(EepromDelaisLux.Valeur()*10000);              // Minuteur de dépassement du seuil d'ouverture de la trappe en mode Lumi
simpleMinuteur MinuteurLuxHaut(EepromDelaisLux.Valeur()*10000);             // Minuteur de dépassement du seuil de fermeture de la trappe en momde Lumi
simpleMinuteur MinuteurInterrupteur(DELAISINTERRUPTEUR);                    // Minuteur de délais entre 2 alummage manuel pour différencier interrupteur et BP
simpleMinuteur MinuteurDureeMinuterie(EepromDureeMinuterie.Valeur()*60000); // Minuteur de durée de la minuterie

/*****
 * Définition de l'objet horloge UTC
 */
RTC_DS3231 UTC;

/*****
 * Définition du luxmetre
 */
BH1750 LuxMetre;


/*****
 * Définition du caractère degrée
 */
uint8_t CharDegre[8]  = {B00100,B01010,B01010,B00100}; 

/*****
 * Allumer : Allumer la lumière avec une variation de 0 à 100%
 *           sur une durée égale au paramètre Temps d'allumage
 */
void Allumer()
  {
  static unsigned long HeureDepart;
  unsigned long HeureCourante;
  unsigned long DiffHeure = 0l;
  int Niveau;
  int NiveauSortie;

  if (NiveauLumiere == 0) 
    {
      HeureDepart = millis();
      NiveauLumiere =1;
    }

   if ((NiveauLumiere < 100) && (NiveauLumiere >=1))
     {
      DiffHeure = millis() - HeureDepart;
      Niveau = 100.0 * ((float) DiffHeure / (EepromTempsAllumage.Valeur()*60000.0));
      if (Niveau == 0)
         Niveau =1;
      if (Niveau > 100)
         Niveau = 100;
      NiveauLumiere = Niveau;
     }
 
  }



/*****
 * OffsetUTC : Calucle le décalage entre l'heure locale et l'heure UTC en minutes
 *     retour : renvoi 60 ou 120 en fonction que nous sommes en here d'hiver ou d'été
 *     
 *     Pour calculer de décalage, cette fonction détermine :
 *     - le dernier dimanche d'octobre pour le passage en heure d'hivers
 *     - le dernier dimanche de mars pour le passage en heure d'été
 */
uint8_t OffsetUTC(DateTime *Date)
  {
  int JourEte;
  int JourHiver;
  int annee = Date->year();
  
  JourEte = 25+(7002-(annee)-((annee)/4))%7;
  JourHiver = 25+(7005-(annee)-((annee)/4))%7;;

  DateTime DateEte (annee, 3, JourEte,1,0,5);
  DateTime DateHiver(annee, 10, JourHiver,1,0,5);


  if ( (*Date >= DateEte ) && (*Date < DateHiver))
    {  
    return 120;
    }
  else
   {
   return 60;  
   }
  }

/*****
 * TriacDetectionZero : Fonction d'intérruption rattachée au passage à zéro de l'alimentation du triac
 */
void TriacDetectionZero() {            // Fonction rattachée à l'intérruption détection passage par zéro
TCNT2 = NiveauLumiere ? 118 + (NiveauLumiere * 1.3): 0;    
}                                 

// Routine d'interruption
/*****
 * Fonction d'intérruption rattachée au dépassement du timer 2
 */
ISR(TIMER2_OVF_vect)
{
      digitalWrite(PINCOMMANDETRIAC, HIGH);   // on commande la gachette du triac       
      digitalWrite(PINCOMMANDETRIAC, LOW);    // on arrête la commande de la gachette       
}
 

/*****
 * setup : Fonction setup du programme
 */
void setup() {
      int i; 

      
      pinMode(PINLUMIERE, OUTPUT);            // Gestion de la sortie lumière
      pinMode(PINCOMMANDETRIAC, OUTPUT);      // gachette du triac
      attachInterrupt(0, TriacDetectionZero, RISING);    // Déclaration d'une interruption sur patte eb détection de passage à zéro,  Entrée 2 (interuption 0)
      cli(); // Desactive l'interruption globale
      bitClear (TCCR2A, WGM20); // WGM20 = 0
      bitClear (TCCR2A, WGM21); // WGM21 = 0 
      TCCR2B = 0b00000111; // Clock / 1024 soit 64 micro-s et WGM22 = 0
      TIMSK2 = 0b00000001; // Interruption locale autoriséesr dépassement par TOIE2
      sei(); // Active l'interruption globale

      Serial.begin(115200);

      UTC.begin();

      MonMenu.init();
      MonMenu.backlight();
      MonMenu.createChar(1, CharDegre);
      MenuCourant=0;
      MonMenu.setCursor(3,3);
      MonMenu.print((char *)"Initialisation");
      LuxMetre.begin();


	    MonMenu.InitVersion((char *)MVersion);
	    MonMenu.InitDateVersion((char*)MDateVersion);
	    MonMenu.Affiche(MenuCourant);

      // Tout les paramètres du menu sont inactifs et non soulignés
     for(i=1; i<=6; i++)
       {
       MonMenu.ParamSetActif(i,false);
       MonMenu.ParamSetSouligne(i,false);
       }


     // démarrage du minuteur rétroéclairage  et menu 
     MinuteurRetroeclairage.demarrer();
     MinuteurMenu.demarrer();
     MinuteurInfo.demarrer();
     MinuteurInterrupteur.demarrer();
}     

/*****
 * loop : Fonction loop du programme
 */
void loop() {
  
Heure Lever;                                           // heure de Lever en heure locale
Heure Coucher;                                         // heure de coucher en heure locale
Heure Ouverture;                                       // heure d'ouverture de la trappe
Heure Fermeture;                                       // heure de fermeture de la trappe
char FormatDate[] = "DDD DD/MM/YYY";
char FormatHeure[]= "hh:mm";
int LongitudeDegre;
int LongituteMinute;
int LongitudeSeconde; 
char BufferTmp[11];                                   // Buffer temporaire de 10 caractères 





//----------------------------
// actualisation de niveau d'éclairement
NiveauLux = (int) LuxMetre.readLightLevel();

//----------------------------
// actualisation des boutons
Bouton1.actualiser();
Bouton2.actualiser();
Bouton3.actualiser();
Bouton4.actualiser();
Interrupteur.actualiser();

// Gestion de l'interrupteu ou bouton poussoir
if (Interrupteur.vientDEtreEnfonceOuRelache() && MinuteurInterrupteur.estTermine())
   {
   LumiereForcee = NiveauLumiere == 100 ? false : true;  
   NiveauLumiere = LumiereForcee ? 100 : 0;
   MinuteurInterrupteur.demarrer();
   if (NiveauLumiere == 100)                // si on vient d'allumer la lumière
     {
      MinuteurDureeMinuterie.demarrer();    // on démarrage le minuteur    
     }
   }
// Gestion de la fin de la minuterie
if (LumiereForcee)
  {
    if ( MinuteurDureeMinuterie.tempsRestant() < 60000)  // si le temps restant est inférieur à une minute  
      {
      NiveauLumiere = map( MinuteurDureeMinuterie.tempsRestant(), 0 , 60000, 0, 100);  
      }
   if (NiveauLumiere <1)
     {
      LumiereForcee = false;
     }
  }

// acès aux menucaché
if ( Bouton1.estEnfonceDepuisAuMoins(5000))
  {
    MonMenu.RazParam();
    MenuCourant=14;
  }
if ( Bouton4.estEnfonceDepuisAuMoins(5000))
  {
    MonMenu.RazParam();
    ModeAuto = MODEINIT;
    MenuCourant=2;
  }

//---------------------------
// si au moin un bouton est effoncé alors on relance le minuteur rétroéclairage
if ( Bouton1.vientDEtreEnfonce() || Bouton2.vientDEtreEnfonce() || Bouton3.vientDEtreEnfonce() || Bouton4.vientDEtreEnfonce() )
   {
    if (MinuteurRetroeclairage.estTermine())   // cas du réveille, dans ce cas on réinitialise la lecture des boutons
      {
      Bouton1.actualiser();
      Bouton2.actualiser();
      Bouton3.actualiser();
      Bouton4.actualiser();
      }
   MinuteurRetroeclairage.demarrer();
   MinuteurMenu.demarrer();
   }

//---------------------------
// si le minuteur rétroéclairage est terminée alors on éteint le rétroéclairage
   RetroEclairage =  MinuteurRetroeclairage.estTermine() ? false : true;

//---------------------------
// gestion du rétro éclairage
if (RetroEclairage)
  {
  MonMenu.On();
  }
else
  {
  MonMenu.Off();
  }

// ---------------------------  
// Commande de la sortie lumière
analogWrite(PINLUMIERE, map(NiveauLumiere, 0 , 100, 0, 255)); 
/*if (NiveauLumiere==100)
  {
  digitalWrite(PINCOMMANDETRIAC, HIGH);   // on commande la gachette du triac     
  }
else
  {
  digitalWrite(PINCOMMANDETRIAC, LOW);   // on commande la gachette du triac     
  }
*/

// ---------------------------  
// si le minuteur Menu est terminé alors on repasse sur le menu 1
if (MinuteurMenu.estTermine())
  {
  for (uint8_t i=0; i<6; i++) 
       MonMenu.ParamSetSouligne(i, false);
  MonMenu.RazParam();
  MenuCourant = 1;
  NiveauMenu = 1;    
  }

  
// ---------------------------  
// récupération du décalage UTC par rapport à l'heure locale UTC
DateHorlogeUTC = UTC.now();
DecalageUTC=OffsetUTC( &DateHorlogeUTC);

// ---------------------------  
// recopie de l'heure UTC dans l'heure courant
// si l'on est en mode SIMULHEURE, alors la DateCourante est simuulé et incrémentée de la
// valeur PASSIMULHEURE à chaque passage ici
if (!SIMULHEURE)
  {
  DateCourante = DateHorlogeUTC + (DecalageUTC * 60); //on rajoute le décalage horaire heure été / hivers
  }
else
  {  
  DateCourante = DateCourante + PASSIMULHEURE;
  }

// ---------------------------  
//Recupération des heures de lever et de coucher pour la date courante et les cordonnées courantes;
LongitudeDegre  = EepromLongDegre.Valeur();                   // Permet de reporter le signe moins des degré ur les minutes et seconde
LongituteMinute = EepromLongMinute.Valeur();                  // 
LongitudeSeconde = EepromLongSeconde.Valeur();                // Ensuite pour la fonction calulerEphemeride on prend l'opposé de la longitude
if (LongitudeDegre <0)                                        // en effet les coordonnée sont saisie avec une valeur positif pour Est et negative pour ouest
  {                                                           // alors que la fonction dement une valeur négative pour estet positive pour ouest
   LongituteMinute *= -1;
   LongitudeSeconde *=-1; 
  }
calculerEphemeride(DateCourante.day(), DateCourante.month(), DateCourante.year(), -LongitudeDegre, -LongituteMinute,  -LongitudeSeconde, EepromLatDegre.Valeur(), EepromLatMinute.Valeur(), EepromLatSeconde.Valeur(), &Lever, &Coucher);
Lever+=DecalageUTC;                          // la fonction calculerEphemeride renvoyant une heure en UTC, nous rajoutons le décalage UTC
Coucher+=DecalageUTC;                        // la fonction calculerEphemeride renvoyant une heure en UTC, nous rajoutons le décalage UTC
Ouverture=Lever;                             // calcul de l'heure locale d'ouverture de la trappe, égale à l'heure de lever du soleil    
Ouverture+= EepromDelaisOuverture.Valeur();  // à laquelle on rajoute le paramètre délais d'ouverture
Fermeture=Coucher;                           // calcul de l'heure locale de fermeture de la trappe, égale à l'heure de coucher du soleil
Fermeture+= EepromDelaisFermeture.Valeur();  // à laquelle on rajoute le paramètre délais de fermeture

// ---------------------------  
// initialisation des difsérent champs de l'écran d'accueil
MonMenu.InitDate(DateCourante.toString(FormatDate));     // date courante
MonMenu.InitTrappe(Trappe.Etat);                           // Position de la trappe
MonMenu.InitMode(ModeAuto);                              // Mode de fonctionnement
MonMenu.InitLumiere(NiveauLumiere);                      // niveau d'éclairage de la lumière
MonMenu.InitChargeBatterie(ChargeBatterie());              // niveau de charge de la batterie
if ( ModeAuto == MODELUMI)                               // Si mode détection niveau lumière, on affiche la valeur de Lux
   {                                                     // sinon on affiche l'heure
   sprintf(BufferTmp, FormatLux, NiveauLux);     
   MonMenu.InitHeure(BufferTmp);
   }
else
   {
   MonMenu.InitHeure(DateCourante.toString(FormatHeure));
   }

// ---------------------------  
// gestion de la trappe en mode manu
if ( TrappeModeManu == TrappeManuOuverture )
    {
     Trappe.Ouvrir();
    }
if ( TrappeModeManu == TrappeManuFermeture )
    {
     Trappe.Fermer();
    }
if ( (Trappe.Etat == OUVERT) || (Trappe.Etat == FERME) )
    {
    TrappeModeManu = TrappeManuFixe;
    }

// ---------------------------  
// en fonction du mode de fonctionnement, initialisation des champs heures d'ouverture et fermeture   
switch (ModeAuto)
   {
      case MODEEPHE :
         MonMenu.InitLever(Ouverture.PrintT());
         MonMenu.InitCoucher(Fermeture.PrintT());
         break;
      case MODEMANU :
         MonMenu.InitLever((char *)"--:--");
         MonMenu.InitCoucher((char *)"--:--");
         break; 
      case MODEHORL :
         sprintf(BufferTmp, "%02d:%02d", EepromHeureOuverture.Valeur(), EepromMinuteOuverture.Valeur() );     
         MonMenu.InitLever(BufferTmp);
         sprintf(BufferTmp, "%02d:%02d", EepromHeureFermeture.Valeur(), EepromMinuteFermeture.Valeur() );     
         MonMenu.InitCoucher(BufferTmp);
         break;
      case MODELUMI :
         sprintf(BufferTmp, FormatLux, EepromLuminositeHaute.Valeur());     
         MonMenu.InitLever(BufferTmp);
         sprintf(BufferTmp, FormatLux, EepromLuminositeBasse.Valeur());     
         MonMenu.InitCoucher(BufferTmp);
         break;
   };      

// ---------------------------  
// Gestion des différents Menus   
switch (MenuCourant)
       {
         case 0 :   // Menu d'initialisation         
              if (MinuteurInfo.estTermine())
                  {
                    if ( EepromPresenceFdc.Valeur()==1)  // présence du fin de course
                      {
                      Trappe.Init();
                      MenuCourant = 1;
                      ModeAuto = MODEEPHE;
                      }
                    else
                      {  
                      MenuCourant = 2;
                      }
                  }
            break;
         case 1 :   // menu de base mode MANU
             MonMenu.InitMenu1(LireEepromString(EEPROMBoutonMenu));
             MonMenu.InitMenu2(LireEepromString(EEPROMBoutonInfo));
             MonMenu.InitMenu3(LireEepromString(EEPROMBoutonLumiere));
             MonMenu.InitMenu4(LireEepromString(EEPROMBoutonMode));
             if (ModeAuto == MODEMANU)
               {
               MonMenu.InitMenu2(LireEepromString(EEPROMBoutonTrappe));
               }
             if (Bouton1.vientDEtreEnfonce())  // Menu
                {
                MenuCourant=9;
                }
             if ( Bouton2.vientDEtreEnfonce() && (ModeAuto == MODEEPHE))   // affichage de la version
                {
                 MinuteurInfo.demarrer();
                 MenuCourant=3;
                }
             if ( Bouton2.vientDEtreEnfonce() && (ModeAuto == MODEMANU))  // commande trappe
                {
                if ( (Trappe.Etat == OUVERT) || (Trappe.Etat==DESCEND))
                   {
                    Trappe.Etat = OUVERT;                   // règle le problème quand la trappe est entrouverte
                    TrappeModeManu = TrappeManuFermeture;
                   }
                 else
                   {
                   Trappe.Etat = FERME;                   // règle le problème quand la trappe est entrouverte
                   TrappeModeManu = TrappeManuOuverture;
                   }
                }

             if ( Bouton3.vientDEtreEnfonce() )  // commande Lumière
                {
                LumiereForcee = NiveauLumiere == 100 ? false : true;  
                NiveauLumiere = LumiereForcee ? 100 : 0;
               if (NiveauLumiere == 100)                // si on vient d'allumer la lumière
                  {
                  MinuteurDureeMinuterie.demarrer();    // on démarrage le minuteur    
                  }
                }
             if (Bouton4.vientDEtreEnfonce())  // Mode auto ou Manu
                {
                if ( (Trappe.Etat == OUVERT) || (Trappe.Etat==FERME))
                   {
                   ModeAuto += 1;
                   if (ModeAuto >3)
                      {
                      ModeAuto=0;
                      }
                   }
                }
             break;
         case 2 :   // menu de réglage de la position haute de la trappe
             MonMenu.setCursor(7,0);
             MonMenu.print((char *)"Reglage");
             MonMenu.setCursor(3,1);
             MonMenu.print((char *)"position haute");
             MonMenu.setCursor(7,2);
             MonMenu.print((char *)"trappe");
             MonMenu.InitMenu1(LireEepromString(EEPROMBoutonFdc));
             MonMenu.InitMenu2(LireEepromString(EEPROMBoutonMoins));
             MonMenu.InitMenu3(LireEepromString(EEPROMBoutonPlus));
             MonMenu.InitMenu4(LireEepromString(EEPROMBoutonValider));
             
             if (Bouton1.estEnfonce())  // Menu
                {
                Trappe.Init();
                }
             if (Bouton2.estEnfonce())  // Descendre la trappe
                {
                Trappe.Etat == DESCEND;
                Trappe.Descend( EepromVMinMoteur.Valeur());
                }
             else if (Bouton3.estEnfonce())  // Monter la trappe
                {
                Trappe.Etat == MONTE;
                Trappe.Monte( EepromVMinMoteur.Valeur());
                }
             else
               {
                Trappe.Stop();   
               }
             if (Bouton4.vientDEtreEnfonce())  // validation
                {
                if  (digitalRead(MOTEURFDC) == LOW)    // fin de course actionné
                  { 
                  EepromPresenceFdc.Maj(1);
                  }
                else
                  { 
                  EepromPresenceFdc.Maj(0);
                  }
                Trappe.Etat = OUVERT;
                Trappe.InitPos(0);
                MenuCourant=1;
                ModeAuto = MODEEPHE;
                }
             MinuteurMenu.demarrer();   // on reste sur le menu en réactivant le tempo   
             MinuteurRetroeclairage.demarrer(); 
            break;
         case 3 :   // Menu d'initialisation         
              if (MinuteurInfo.estTermine())
                  {
                  MenuCourant = 1;
                  }
            break;
          /********
           *      menu de réglage de l'heure
           */
         case 9 :   
             // Initialisation des paramètres
             if (!MonMenu.ParamActif(1))
               {
               MonMenu.ParamSetActif(1, true);             // jour
               MonMenu.InitParam(1, DateCourante.day(), 1,1, (char *)Format2d,2,1,31); 
               MonMenu.ParamSetActif(2, true);             // mois
               MonMenu.InitParam(2, DateCourante.month(), 4,1, (char *)Format02d,2, 1,12); 
               MonMenu.ParamSetActif(3, true);             // année
               MonMenu.InitParam(3, DateCourante.year(), 7,1, (char *)Format4d,4, 2020,2050); 
               MonMenu.ParamSetActif(4, true);             // heure
               MonMenu.InitParam(4, DateCourante.hour(), 13,1, (char *)Format2d,2, 0,23); 
               MonMenu.ParamSetActif(5, true);             // minute 
               MonMenu.InitParam(5, DateCourante.minute(), 16,1, (char *)Format02d,2, 0,59); 
               }  
             if ( MonMenu.ModifierParam(MenuCourant, ParamCourant, NiveauMenu,23, 10, true))
               {
                if (MonMenu.ParamActif(1))
                  {
                  DateCourante = DateTime(MonMenu.ValeurParam(3), MonMenu.ValeurParam(2), MonMenu.ValeurParam(1), MonMenu.ValeurParam(4), MonMenu.ValeurParam(5));  // mise à jour de la date courante
                  DecalageUTC=OffsetUTC( &DateCourante);         // calcul du nouveau décalage horaire
                  DateTime DateUTC;                              // variable temporaire
                  DateUTC = DateCourante+(DecalageUTC* -60);       // on soustrait le décalage horaire
                  UTC.adjust(DateUTC);                            // mise à jour le l'horloge UTC
                  MonMenu.RazParam();  
                  }
               }
             break;
          /********
           *      menu de réglage des coordonnées
           */
         case 10 :   // menu de modification des coordonée
             // Initialisation des paramètres
             if (!MonMenu.ParamActif(1))
               {
               MonMenu.ParamSetActif(1, true);             // degré longitude
               MonMenu.InitParam(1, EepromLongDegre.Valeur(), 1,1, (char *)Format2d,2,-5,8); 
               MonMenu.ParamSetActif(2, true);             // minute longitude
               MonMenu.InitParam(2, EepromLongMinute.Valeur(), 4,1, (char *)Format02d,2, 0,59); 
               MonMenu.ParamSetActif(3, true);             // seconde longitude
               MonMenu.InitParam(3, EepromLongSeconde.Valeur(), 7,1, (char *)Format02d,2, 0,59); 
               MonMenu.ParamSetActif(4, true);             // degré latitude
               MonMenu.InitParam(4, EepromLatDegre.Valeur(), 11,1, (char *)Format2d,2, 40,50); 
               MonMenu.ParamSetActif(5, true);             // minute latitude
               MonMenu.InitParam(5, EepromLatMinute.Valeur(), 14,1, (char *)Format02d,2, 0,59); 
               MonMenu.ParamSetActif(6, true);             // seconde latitude
               MonMenu.InitParam(6, EepromLatSeconde.Valeur(), 17,1, (char *)Format02d,2, 0,59); 
               }  
             if ( MonMenu.ModifierParam(MenuCourant, ParamCourant, NiveauMenu,9, 11, true))
               {
                if (MonMenu.ParamActif(1))
                  {
                  EepromLongDegre.Maj(MonMenu.ValeurParam(1)); 
                  EepromLongMinute.Maj(MonMenu.ValeurParam(2)); 
                  EepromLongSeconde.Maj(MonMenu.ValeurParam(3)); 
                  EepromLatDegre.Maj(MonMenu.ValeurParam(4)); 
                  EepromLatMinute.Maj(MonMenu.ValeurParam(5)); 
                  EepromLatSeconde.Maj(MonMenu.ValeurParam(6)); 
                  }
               }
             break;
          /*******
           *     menu de réglage du délais d'ouverture de la trappe
           */
         case 11 :   
             // Initialisation des paramètres
             if (!MonMenu.ParamActif(1))
               {
               MonMenu.ParamSetActif(1, true);             
               MonMenu.InitParam(1, EepromDelaisOuverture.Valeur(), 5,1, (char *)Format3d,3,-20,40); 
               }  
             if ( MonMenu.ModifierParam(MenuCourant, ParamCourant, NiveauMenu,10, 12))
               {
                if (MonMenu.ParamActif(1))
                  {
                   EepromDelaisOuverture.Maj(MonMenu.ValeurParam(1));
                   MonMenu.RazParam();  
                  }
               }
             break;

          /********
           *      menu de réglage du délais de fermeture de la trappe
           */
         case 12 :  
             // Initialisation des paramètres
             if (!MonMenu.ParamActif(1))
               {
               MonMenu.ParamSetActif(1, true);             
               MonMenu.InitParam(1, EepromDelaisFermeture.Valeur(), 5,1, (char *)Format3d,3,-20,40); 
               }  
             if ( MonMenu.ModifierParam(MenuCourant, ParamCourant, NiveauMenu,11, 13))
               {
                if (MonMenu.ParamActif(1))
                  {
                   EepromDelaisFermeture.Maj(MonMenu.ValeurParam(1));
                   MonMenu.RazParam();  
                  }
               }
             break;
          /********
           *      menu de réglage de la position basse de la trappe
           */
         case 13 :   
             // Initialisation des paramètres
             if (!MonMenu.ParamActif(1))
               {
               MonMenu.ParamSetActif(1, true);             
               MonMenu.InitParam(1, EepromHauteurOuverture.Valeur(), 8,1, (char *)Format2d,2,15,60); 
               }  
               
             if ( MonMenu.ModifierParam(MenuCourant, ParamCourant, NiveauMenu,12, 16))
               {
                if (MonMenu.ParamActif(1))
                  {
                   EepromHauteurOuverture.Maj(MonMenu.ValeurParam(1));
                   MonMenu.RazParam();  
                  }
               }
             break;
          /********
           *      menu de réglage du pas moteur par cm   MENU CACHE
           */
         case 14 :   
             // Initialisation des paramètres
             if (!MonMenu.ParamActif(1))
               {
               MonMenu.ParamSetActif(1, true);             
               MonMenu.InitParam(1, EepromPasMoteur.Valeur(), 1,1, (char *)Format3d,3,1,999); 
               }  
             if ( MonMenu.ModifierParam(MenuCourant, ParamCourant, NiveauMenu,22, 15))
               {
                if (MonMenu.ParamActif(1))
                  {
                   EepromPasMoteur.Maj(MonMenu.ValeurParam(1));
                   MonMenu.RazParam();  
                  }
               }
             break;
         /********
          *      menu de réglage ddu time outTrappe
          */
         case 15 :   
             // Initialisation des paramètres
             if (!MonMenu.ParamActif(1))
               {
               MonMenu.ParamSetActif(1, true);             
               MonMenu.InitParam(1, EepromTimeoutTrappe.Valeur(), 4,1, (char *)Format2d,2,5,20); 
               }  
             if ( MonMenu.ModifierParam(MenuCourant, ParamCourant, NiveauMenu,14, 22))
               {
                if (MonMenu.ParamActif(1))
                  {
                   EepromTimeoutTrappe.Maj(MonMenu.ValeurParam(1));
                   MonMenu.RazParam();  
                  }
               }
             break;
         /********
          *      menu de réglage du délais d'allumage
          */
         case 16 :   
             // Initialisation des paramètres
             if (!MonMenu.ParamActif(1))
               {
               MonMenu.ParamSetActif(1, true);             
               MonMenu.InitParam(1, EepromDelaisAllumage.Valeur(), 4,1, (char *)Format3d,3,-60,60); 
               }  
             if ( MonMenu.ModifierParam(MenuCourant, ParamCourant, NiveauMenu,13, 17))
               {
                if (MonMenu.ParamActif(1))
                  {
                   EepromDelaisAllumage.Maj(MonMenu.ValeurParam(1));
                   MonMenu.RazParam();  
                  }
               }
             break;
         /********
          *      menu de réglage du temps d'allumage pour passer de 0 à 100%
          */
         case 17 :   
             // Initialisation des paramètres
             if (!MonMenu.ParamActif(1))
               {
               MonMenu.ParamSetActif(1, true);             
               MonMenu.InitParam(1, EepromTempsAllumage.Valeur(), 5,1, (char *)Format2d,2,0,60); 
               }  
             if ( MonMenu.ModifierParam(MenuCourant, ParamCourant, NiveauMenu,16, 18))
               {
                if (MonMenu.ParamActif(1))
                  {
                   EepromTempsAllumage.Maj(MonMenu.ValeurParam(1));
                   MonMenu.RazParam();  
                  }
               }
             break;
          /********
          *      menu de réglage de la durée d'allumage
          */
         case 18 :   
             // Initialisation des paramètres
             if (!MonMenu.ParamActif(1))
               {
               MonMenu.ParamSetActif(1, true);             
               MonMenu.InitParam(1, EepromDureeAllumage.Valeur(), 5,1, (char *)Format2d,2,0,60); 
               }  
             if ( MonMenu.ModifierParam(MenuCourant, ParamCourant, NiveauMenu,17, 19))
               {
                if (MonMenu.ParamActif(1))
                  {
                   EepromDureeAllumage.Maj(MonMenu.ValeurParam(1));
                   MonMenu.RazParam();  
                  }
               }
             break;
          /********
          *      Heure de fin et de debut en mode horaire
          */
         case 19 :   
             // Initialisation des paramètres
             if (!MonMenu.ParamActif(1))
               {
               MonMenu.ParamSetActif(1, true);             // heure ouverture
               MonMenu.InitParam(1, EepromHeureOuverture.Valeur(), 4,1, (char *)Format02d,2,0,23); 
               MonMenu.ParamSetActif(2, true);             // minute ouverture
               MonMenu.InitParam(2, EepromMinuteOuverture.Valeur(), 7,1, (char *)Format02d,2, 0,59); 
               MonMenu.ParamSetActif(3, true);             // heure fermeture
               MonMenu.InitParam(3, EepromHeureFermeture.Valeur(), 15,1, (char *)Format02d,2, 0,23); 
               MonMenu.ParamSetActif(4, true);             // minute fermertue
               MonMenu.InitParam(4, EepromMinuteFermeture.Valeur(), 18,1, (char *)Format02d,2, 0,59); 
               }  
             if ( MonMenu.ModifierParam(MenuCourant, ParamCourant, NiveauMenu,18, 20, true))
               {
                if (MonMenu.ParamActif(1))
                  {
                   EepromHeureOuverture.Maj(MonMenu.ValeurParam(1));
                   EepromMinuteOuverture.Maj(MonMenu.ValeurParam(2));
                   EepromHeureFermeture.Maj(MonMenu.ValeurParam(3));
                   EepromMinuteFermeture.Maj(MonMenu.ValeurParam(4));
                   MonMenu.RazParam();  
                  }
               }
             break;
          /********
          *      Réglage seuils de luminosités en mode luminosité
          */
         case 20 :   
             // Initialisation des paramètres
             if (!MonMenu.ParamActif(1))
               {
               MonMenu.ParamSetActif(1, true);             // luninosité basse
               MonMenu.InitParam(1, EepromLuminositeHaute.Valeur(), 4,1, (char *)Format4d, 4, 0, 1500); 
               MonMenu.ParamSetActif(2, true);             // luminosité haute
               MonMenu.InitParam(2, EepromLuminositeBasse.Valeur(), 15,1, (char *)Format4d, 4, 0, 1500); 
               }  
             if ( MonMenu.ModifierParam(MenuCourant, ParamCourant, NiveauMenu,19, 21, true))
               {
                if (MonMenu.ParamActif(1))
                  {
                   EepromLuminositeHaute.Maj(MonMenu.ValeurParam(1));
                   EepromLuminositeBasse.Maj(MonMenu.ValeurParam(2));
                   MonMenu.RazParam();  
                  }
               }
             break;
          /********
          *      menu de réglage de l'hystérésis en mode nature
          */
         case 21 :   
             // Initialisation des paramètres
             if (!MonMenu.ParamActif(1))
               {
               MonMenu.ParamSetActif(1, true);             
               MonMenu.InitParam(1, EepromDelaisLux.Valeur(), 3,1, (char *)Format3d,3,1,270); 
               }  
             if ( MonMenu.ModifierParam(MenuCourant, ParamCourant, NiveauMenu,20, 23))
               {
                if (MonMenu.ParamActif(1))
                  {
                   EepromDelaisLux.Maj(MonMenu.ValeurParam(1));
                   MonMenu.RazParam();  
                  }
               }
             break;
          /********
          *      Réglage vitesses du moteur
          */
         case 22 :   
             // Initialisation des paramètres
             if (!MonMenu.ParamActif(1))
               {
               MonMenu.ParamSetActif(1, true);             // vitesse minimun du moteur
               MonMenu.InitParam(1, EepromVMinMoteur.Valeur(), 6,1, (char *)Format3d, 3, 25, 100); 
               MonMenu.ParamSetActif(2, true);             // vitesse maximun du moteur
               MonMenu.InitParam(2, EepromVMaxMoteur.Valeur(), 15,1, (char *)Format3d, 3, 100, 255); 
               }  
             if ( MonMenu.ModifierParam(MenuCourant, ParamCourant, NiveauMenu,15, 14, true))
               {
                if (MonMenu.ParamActif(1))
                  {
                   EepromVMinMoteur.Maj(MonMenu.ValeurParam(1));
                   EepromVMaxMoteur.Maj(MonMenu.ValeurParam(2));
                   MonMenu.RazParam();  
                  }
               }
             break;
          /********
          *      menu de réglage de l'hystérésis en mode nature
          */
         case 23 :   
             // Initialisation des paramètres
             if (!MonMenu.ParamActif(1))
               {
               MonMenu.ParamSetActif(1, true);             
               MonMenu.InitParam(1, EepromDureeMinuterie.Valeur(), 2,1, (char *)Format2d,2,2,60); 
               }  
             if ( MonMenu.ModifierParam(MenuCourant, ParamCourant, NiveauMenu,21, 9))
               {
                if (MonMenu.ParamActif(1))
                  {
                   EepromDureeMinuterie.Maj(MonMenu.ValeurParam(1));
                   MonMenu.RazParam();  
                  }
               }
             break;
      }
MonMenu.Affiche(MenuCourant);  

// ---------------------------  
// détermintion de l'heure d'ouverture et de fermeture de la trape
DateTime DateOuverture(DateCourante.year(), DateCourante.month(), DateCourante.day(), Lever.H(), Lever.M());                                              // ouverture égale à la date courante avec heure de lever, le tout en heure locale
DateTime DateFermeture(DateCourante.year(), DateCourante.month(), DateCourante.day(), Coucher.H(), Coucher.M());                                          // fermeture égale à la date courante avec heure de coucher, le tout en heure locale
DateOuverture = DateOuverture + (EepromDelaisOuverture.Valeur()*60);                                                                                      // on rajoute à l'heure précédemment calculée le délais d'ouverture  
DateFermeture = DateFermeture + (EepromDelaisFermeture.Valeur()*60);                                                                                      // on rajoute à l'heure précédemment calculée le délais de fermeture
DateTime DateOuvertureLumi(DateCourante.year(), DateCourante.month(), DateCourante.day(), EepromHeureOuverture.Valeur(), EepromMinuteOuverture.Valeur());
DateTime DateFermetureLumi(DateCourante.year(), DateCourante.month(), DateCourante.day(),  EepromHeureFermeture.Valeur(), EepromMinuteFermeture.Valeur());


// ---------------------------  
// détermintion de l'ouverture et de la fermeture de la trappe en mode éphéméride
if (ModeAuto == MODEEPHE)
   {
   if ((DateCourante >= DateOuverture) && ( DateCourante <= DateFermeture))
      {
      Trappe.Ouvrir();
      }
    else
      {
      Trappe.Fermer();
      }
   }

// ---------------------------  
// détermintion de l'ouverture et de la fermeture de la trappe en mode horaire
if (ModeAuto == MODEHORL)
   {
   if ((DateCourante >= DateOuvertureLumi) && ( DateCourante <= DateFermetureLumi))
      {
      Trappe.Ouvrir();
      }
    else
      {
      Trappe.Fermer();
      }
   }

// ---------------------------  
// détermintion de l'ouverture et de la fermeture de la trappe en mode luxmètre
if (ModeAuto != MODELUMI)
   {
   MinuteurLuxHaut.demarrer(EepromDelaisLux.Valeur()*10000);        // En mode non Lumi on redemarre à chaque tour les minuteurs
   MinuteurLuxBas.demarrer(EepromDelaisLux.Valeur()*10000);         // ceci afin qu'il ne soit pas terminer au passage en mode LUMI
   }
if (ModeAuto == MODELUMI)
   {
   if ((NiveauLux >= EepromLuminositeHaute.Valeur()))
      {
      if ( MinuteurLuxHaut.estTermine())
         {
         Trappe.Ouvrir();
         }
      }

   if ((NiveauLux <= EepromLuminositeBasse.Valeur()) )
      {
      if ( MinuteurLuxBas.estTermine())
         {
         Trappe.Fermer();
         }
      }
    if ( NiveauLux < EepromLuminositeHaute.Valeur())
       {
        MinuteurLuxHaut.demarrer(EepromDelaisLux.Valeur()*10000);
       }
    if ( NiveauLux > EepromLuminositeBasse.Valeur())
       {
        MinuteurLuxBas.demarrer(EepromDelaisLux.Valeur()*10000);
       }
   }

 
// ---------------------------  
// gestion de l'allumage et l'extinction de la lumière
if (ModeAuto == MODEEPHE)                                                                                               // en mode éphéméride
  {
  DateTime  DateAllumage(DateCourante.year(), DateCourante.month(), DateCourante.day(), Lever.H(), Lever.M());
  DateTime  DateExtinction(DateCourante.year(), DateCourante.month(), DateCourante.day(), Coucher.H(), Coucher.M());

  DateAllumage = DateAllumage + (EepromDelaisAllumage.Valeur()*60);
  DateExtinction = DateAllumage  + ((EepromTempsAllumage.Valeur() + EepromDureeAllumage.Valeur())*60);
     
  if ( !LumiereForcee) 
    {
    if ((DateCourante >= DateAllumage) && ( DateCourante <= DateExtinction))
       {
       Allumer();
       }
    else
      {
      NiveauLumiere=0;
      }
    }
  }
if (ModeAuto == MODEHORL)                                                                                               // en mode horloge
  {
  DateTime  DateAllumage(DateCourante.year(), DateCourante.month(), DateCourante.day(),  EepromHeureOuverture.Valeur(), EepromMinuteOuverture.Valeur());
  DateTime  DateExtinction(DateCourante.year(), DateCourante.month(), DateCourante.day(),  EepromHeureFermeture.Valeur(), EepromMinuteFermeture.Valeur());

  DateAllumage = DateAllumage + (EepromDelaisAllumage.Valeur()*60);
  DateExtinction = DateAllumage  + ((EepromTempsAllumage.Valeur() + EepromDureeAllumage.Valeur())*60);
     
  if ( !LumiereForcee) 
    {
    if ((DateCourante >= DateAllumage) && ( DateCourante <= DateExtinction))
       {
       Allumer();
       }
    else
      {
      NiveauLumiere=0;
      }
    }
  }

// ---------------------------  
// gestion du passage en veille
if ( !RetroEclairage && ( (Trappe.Etat == FERME) || (Trappe.Etat == OUVERT)) && (NiveauLumiere == 0) &&  MinuteurInterrupteur.estTermine())
  {
//    Serial.println("Passage en veille");
//    delay(100);
    LowPower.powerDown(SLEEP_500MS, ADC_OFF, BOD_OFF);  

//    Serial.println("Sortie de veille");
  }
}
