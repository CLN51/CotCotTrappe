//-----------------------------------------//
//                                         //
//             TrappePoule                 //  
//              Menu.cpp                   //
//                                         //
//-----------------------------------------//
#include <arduino.h>
#include <LiquidCrystal_I2C.h>
#include "simpleBouton.h"
#include "Menu.h"
#include "Eprom.h"
#include "Trappe.h"

extern LiquidCrystal_I2C lcd;



extern simpleBouton Bouton1;             // Déclaration de l'objet bouton 1
extern simpleBouton Bouton2;             // Déclaration de l'objet bouton 2
extern simpleBouton Bouton3;             // Déclaration de l'objet bouton 3
extern simpleBouton Bouton4;             // Déclaration de l'objet bouton 4
extern uint8_t CharDegre[8];             // Définition du symbole degré




/*****
 * Menu : constructeur de la classe
 *   lcdAddr : adresse du module lcd
 *   lcdCol  : nombre de colonne du module lcd
 *   lcdRows : nombre de ligne du module lcd
 */
Menu::Menu(uint8_t lcdAddr, uint8_t lcdCol,  uint8_t lcdRows) : LiquidCrystal_I2C( lcdAddr, lcdCol,  lcdRows) 
{  
  pinMode(PINALIMLCD, OUTPUT);            // Gestion de la sortie alimentation du LCD
  digitalWrite(PINALIMLCD, LOW);
  Actif=true;
  MenuCourant = 0;
}

/*****
 * On : alimente le LCD
 */
void Menu::On()
  {
  if (Actif == false)
   { 
   digitalWrite(PINALIMLCD, LOW);
   Actif=true;
   backlight();
   begin(20,4);
   this->createChar(1, CharDegre);

   }
  }

/*****
 * Off : coupe l'alimentation du LCD
 */
void Menu::Off()
  {
  digitalWrite(PINALIMLCD, HIGH);
  noBacklight();
  Actif=false;
  }

/*****
 * clear : surcharge de la méthode clear()
 */
void Menu::clear()
  {
  if (Actif)
    {
    LiquidCrystal_I2C::clear();    
    }
  }

/*****
 * setCursor : surcharge de la méthode setCursor()
 */
void Menu::setCursor(uint8_t col, uint8_t row)
  {
  if (Actif)
    {
    LiquidCrystal_I2C::setCursor(col, row);    
    }
  }

/*****
 * print : surcharge de la méthode print()
 */
void Menu::print(char *chaine)
  {
  if (Actif)
    {
    LiquidCrystal_I2C::print(chaine);    
    }
  }
void Menu::print(char car)
  {
  if (Actif)
    {
    LiquidCrystal_I2C::print(car);    
    }
  }

/*****
 * noBacklight : surcharge de la méthode noBacklight()
 */
void Menu::noBacklight()
  {
  if (Actif)
    {
    LiquidCrystal_I2C::noBacklight();    
    }
  }

/*****
 * backlight : surcharge de la méthode backlight()
 */
void Menu::backlight()
  {
  if (Actif)
    {
    LiquidCrystal_I2C::backlight();    
    }
  }

/*****
 * Affiche : affiche un menu
 *     NMenu : numéro du menu à afficher 
 */
void Menu::Affiche(uint8_t NMenu)
{
  int i;

  
  if (this->MenuCourant != NMenu)   // si le numéro de menu est different du précedent, alors on efface l'écran
    {
      this->clear();
      this->MenuCourant = NMenu;
    }
// Affichage des paramètres
for (i=0; i<6; i++)
   {
   if( _Param[i].Actif)
      {
      this->setCursor(_Param[i].x, _Param[i].y);
      this->print(_Param[i].Texte);
      if (_Param[i].Souligne)
         {
          this->CurseurOn(_Param[i].x, _Param[i].y, _Param[i].Lg);
         }
      else
         {
         this->CurseurOff(_Param[i].x, _Param[i].y, _Param[i].Lg);
        }
      }  
   }
 switch (NMenu)
       {
         case 0 :                                // menu d'information
         case 3 :
               this->setCursor(4,0);
               this->print((char *)"CotCotTrappe");
			         this->PrintVersion();
			         this->PrintDateVersion();
               this->setCursor(15,3);
               break; 
         case 1 :                                // menu d'accueil
               this->PrintDate();
               this->PrintHeure();
               this->PrintTrappe();
               this->PrintLumiere();
               this->PrintChargeBatterie();
               this->PrintLever();
               this->PrintSeparateur();
               this->PrintCoucher();
               this->PrintMode();
               this->PrintMenu();
               break;
         case 2 :                                // menu de réglage de la position haute de la trappe
               this->PrintMenu();
               break;
         case 9 :                               // menu modif de l'heure
               this->AfficheFondNew(EEPROMMenu9_L1);
               this->PrintMenu();
               break;
         case 10 :                               // menu modif coordonées
               this->AfficheFondNew(EEPROMMenu10_L1);
               this->PrintMenu();
               break;
         case 11 :                               // menu modif délais ouverture porte
               this->AfficheFondNew(EEPROMMenu11_L1);
               this->PrintMenu();
               break;
         case 12 :                               // menu modif délais fermeture porte
               this->AfficheFondNew(EEPROMMenu12_L1);
               this->PrintMenu();
               break;
         case 13 :                               // menu modif position basse trappe
               this->AfficheFondNew(EEPROMMenu13_L1);
               this->PrintMenu();
               break;
         case 14 :                               // menu modif position haute trappe
               this->AfficheFondNew(EEPROMMenu14_L1);
               this->PrintMenu();
               break;
         case 15 :                               // menu modif temps manoeuvre trappe
               this->AfficheFondNew(EEPROMMenu15_L1);
               this->PrintMenu();
               break;
         case 16 :                               // menu modif delai allumagee
               this->AfficheFondNew(EEPROMMenu16_L1);
               this->PrintMenu();
               break;
         case 17 :                               // menu modif temps pour passer l'allumage de 0 à 100%
               this->AfficheFondNew(EEPROMMenu17_L1);
               this->PrintMenu();
               break;
         case 18 :                               // menu modif durée de l'allumage
               this->AfficheFondNew(EEPROMMenu18_L1);
               this->PrintMenu();
               break;
         case 19 :                               // menu modif des heure en mode horaire
               this->AfficheFondNew(EEPROMMenu19_L1);
               this->PrintMenu();
               break;
         case 20 :                               // menu modif des luminostés en mode luminosité
               this->AfficheFondNew(EEPROMMenu20_L1);
               this->PrintMenu();
               break;
         case 21 :                               // menu reglage hystéresis en mode luminosité
               this->AfficheFondNew(EEPROMMenu21_L1);
               this->PrintMenu();
               break;
         case 22 :                               // menu reglage des vitesses du moteur
               this->AfficheFondNew(EEPROMMenu22_L1);
               this->PrintMenu();
               break;
         case 23 :                               // menu reglage de la durée de la minuterie
               this->AfficheFondNew(EEPROMMenu23_L1);
               this->PrintMenu();
               break;
         default :
               this->setCursor(2,1);
               this->print((char *)"Menu inconnu");
               break;
       }
  
}

/*****
 * InitDate : Permet de renseigner la date courante
 *     Date : Date sous la forme JJ/MM/AA 
 */
void Menu::InitDate(char *Date)
  {
  strncpy(_Date, Date,12);
  }

/*****
 * PrintDate : Permet d'afficher la date renseignée par la fonction InitDate
 *     x : position en x, valeur facultative, elle est renseignée par defaut 
 *     y : position en y, valeur facultative, elle est renseignée par defaut 
 */
 void Menu::PrintDate(uint8_t x, uint8_t y)
  {
  this->setCursor(x,y);
  this->print(_Date);
  }

/*****
 * InitHeure : Permet de renseigner l'heure courante
 *     Heure : Heure sous la forme HH:MM:SS 
 */
void Menu::InitHeure(char *Heure)
  {
  strncpy(_Heure, Heure,5);
  }

/*****
 * PrintHeure : Permet d'afficher l'heure renseignée par la fonction InitHeure
 *     x : position en x, valeur facultative, elle est renseignée par defaut 
 *     y : position en y, valeur facultative, elle est renseignée par defaut 
 */
void Menu::PrintHeure(uint8_t x, uint8_t y)
  {
  this->setCursor(x,y);
  this->print(_Heure);
  }

/*****
 * InitVersion : Permet de renseigner la version du logiciel
 *     Version : Version du logiciel sous la forme VX.yy 
 */
void Menu::InitVersion(char *Version)
  {
  strncpy(_Version, Version,9);
  }

/*****
 * PrintVersion : Permet d'afficher la version du logiciel renseignée par la fonction InitVersion
 *     x : position en x, valeur facultative, elle est renseignée par defaut 
 *     y : position en y, valeur facultative, elle est renseignée par defaut 
 */
void Menu::PrintVersion(uint8_t x, uint8_t y)
  {
  this->setCursor(x,y);
  this->print(_Version);
  }

/*****
 * InitDateVersion : Permet de renseigner la date de réalisation du logiciel
 *     Date : Date du logiciel sous la forme MM/AAAA 
 */
void Menu::InitDateVersion(char *Date)
  {
  strncpy(_DateVersion, Date,8);
  }

/*****
 * PrintDateVersion : Permet d'afficher la date de réalisation du logiciel renseignée par la fonction InitDateVersion
 *     x : position en x, valeur facultative, elle est renseignée par defaut 
 *     y : position en y, valeur facultative, elle est renseignée par defaut 
 */
void Menu::PrintDateVersion(uint8_t x, uint8_t y)
  {
  this->setCursor(x,y);
  this->print(_DateVersion);
  }

/*****
 * InitTrappe : Permet de renseigner la position de la trappe
 *     Etat : Etat de la TrappePoule
 *       0 : Fermée
 *       1 : Ouverte
 *       2 : En cours d'ouverture
 *       3 : En cours de fermeture 
 */
void Menu::InitTrappe(uint8_t Etat)
  {
  _EtatTrappe = Etat;
  }

/*****
 * PrintTrappe : Permet d'afficher la version du logiciel renseignée par la fonction InitVersion
 *     x : position en x, valeur facultative, elle est renseignée par defaut 
 *     y : position en y, valeur facultative, elle est renseignée par defaut 
 */
void Menu::PrintTrappe(uint8_t x, uint8_t y)
  {
  this->setCursor(x,y);  

  switch (_EtatTrappe)
       {
         case FERME :
            this->print((char *)"Ferme  ");
            break;
         case OUVERT :
            this->print((char *)"Ouvert ");
            break;
         case MONTE :
            this->print((char *)"Monte  ");
            break;
         case DESCEND :
            this->print((char *)"Desc   ");
            break;
       }
  }  


/*****
 * InitLumiere : Permet de renseigner le niveau d'éclairage
 *     Pourcent : Valeur en pourcebtage de l'éclairement. 
 */
 void Menu::InitLumiere(uint8_t Pourcent)
  {
  _Lumiere = Pourcent;
  }

/*****
 * PrintLumiere : Permet d'afficher le niveau d'éclairement en % renseignée par la fonction InitVersion
 *     x : position en x, valeur facultative, elle est renseignée par defaut 
 *     y : position en y, valeur facultative, elle est renseignée par defaut 
 */
void Menu::PrintLumiere(uint8_t x, uint8_t y)
  {
  char Buffer [4];
  
  sprintf(Buffer, "%3d", _Lumiere);
  this->setCursor(x,y);
  this->print((char *)"L:");
  this->setCursor(x+5,y);
  this->print('%');
  this->setCursor(x+2,y);
  this->print(Buffer);
  }
/*****
 * InitLumiere : Permet de renseigner le niveau d'éclairage
 *     Pourcent : Valeur en pourcebtage de l'éclairement. 
 */
 void Menu::InitChargeBatterie(uint8_t Pourcent)
  {
  _ChargeBatterie = Pourcent;
  }

/*****
 * PrintLumiere : Permet d'afficher le niveau d'éclairement en % renseignée par la fonction InitVersion
 *     x : position en x, valeur facultative, elle est renseignée par defaut 
 *     y : position en y, valeur facultative, elle est renseignée par defaut 
 */
void Menu::PrintChargeBatterie(uint8_t x, uint8_t y)
  {
  char Buffer [4];
  
  sprintf(Buffer, "%3d", _ChargeBatterie);
  this->setCursor(x,y);
  this->print((char *)"B:");
  this->setCursor(x+5,y);
  this->print('%');
  this->setCursor(x+2,y);
  this->print(Buffer);
  }

/*****
 * InitLever : Permet de renseigner l'heure de lever du soleil
 *     Valeur : Valeur sous la forme HH:MM. 
 */
void Menu::InitLever(char *Valeur)
  {
  strncpy(_Lever,Valeur,5);
  }

/*****
 * PrintLever : Permet d'afficher l'heure de lever du soleil renseigné par la fonction InitLever
 *     x : position en x, valeur facultative, elle est renseignée par defaut 
 *     y : position en y, valeur facultative, elle est renseignée par defaut 
 */
void Menu::PrintLever(uint8_t x, uint8_t y)
  {
  this->setCursor(x,y);
  this->print(_Lever);
  }

/*****
 * Printsparateur : Permet d'afficher un séparateur entre l'heure de lever et de coucher du soleil
 *     x : position en x, valeur facultative, elle est renseignée par defaut 
 *     y : position en y, valeur facultative, elle est renseignée par defaut 
 */
void Menu::PrintSeparateur(uint8_t x, uint8_t y)
  {
  this->setCursor(x,y);
  this->print((char *)"<->");
  }

/*****
 * Initcoucher : Permet de renseigner l'heure de coucher du soleil
 *     Valeur : Valeur sous la forme HH:MM. 
 */
void Menu::InitCoucher(char *Valeur)
  {
  strncpy(_Coucher,Valeur,5);
  }

/*****
 * PrintCoucher : Permet d'afficher l'heure de coucher du soleil renseigné par la fonction InitLever
 *     x : position en x, valeur facultative, elle est renseignée par defaut 
 *     y : position en y, valeur facultative, elle est renseignée par defaut 
 */
void Menu::PrintCoucher(uint8_t x, uint8_t y)
  {
  this->setCursor(x,y);
  this->print(_Coucher);
  }

 /*****
 * InitMode : Permet de renseigner le mode fonctionnement
 *     Mode : mode de fonctionnement
 *       Vrai : Auto
 *       Faux : MANU
 */
void Menu::InitMode(int8_t Mode)
  {
  _Mode = Mode;
  }

/*****
 * PrintMode : Permet d'afficher le mode de fonctionnement renseignée par la fonction InitVersion
 *     x : position en x, valeur facultative, elle est renseignée par defaut 
 *     y : position en y, valeur facultative, elle est renseignée par defaut 
 */
void Menu::PrintMode(uint8_t x, uint8_t y)
  {
  this->setCursor(x,y);  

  switch (_Mode)
      {
      case MODEMANU :
         this->print((char *)"Manuel");
         break;
      case MODEEPHE :
         this->print((char *)"Epheme");
         break;
      case MODEHORL :
         this->print((char *)"Horlog");
         break;
      case MODELUMI :
         this->print((char *)"Nature");
         break;
      };
  }      

/*****
 * InitMenu1 : Permet de renseigner le libellé du menu1
 *     Libelle : Nom du libellé. 
 */
void Menu::InitMenu1(char *Libelle)
  {
  strncpy(_Menu1,Libelle,4);
  }

/*****
 * InitMenu2 : Permet de renseigner le libellé du menu2
 *     Libelle : Nom du libellé. 
 */
void Menu::InitMenu2(char *Libelle)
  {
  strncpy(_Menu2,Libelle,4);
  }

/*****
 * InitMenu3 : Permet de renseigner le libellé du menu3
 *     Libelle : Nom du libellé. 
 */
void Menu::InitMenu3(char *Libelle)
  {
  strncpy(_Menu3,Libelle,4);
  }

/*****
 * InitMenu4 : Permet de renseigner le libellé du menu4
 *     Libelle : Nom du libellé. 
 */
void Menu::InitMenu4(char *Libelle)
  {
  strncpy(_Menu4,Libelle,5);
  }

/*****
 * PrintMenu : Permet d'afficher le menu en bas de l'écran
 *     x : position en x, valeur facultative, elle est renseignée par defaut 
 *     y : position en y, valeur facultative, elle est renseignée par defaut 
 */
void Menu::PrintMenu(uint8_t x, uint8_t y)
  {
  this->setCursor(x,y);  
  this->print(_Menu1);
  this->setCursor(x+4,y);  
  this->print('|');
  this->print(_Menu2);
  this->setCursor(x+9,y);  
  this->print('|');
  this->print(_Menu3);
  this->setCursor(x+14,y);  
  this->print('|');
  this->print(_Menu4);
  }  

/*****
 * CurseurOn : affiche un curseur sous la forme ^^ pour matérialiser le parametre en modification
 *     x  : position en x du curseur 
 *     y  : position en y du curseur 
 *     Nb : nombre de caactères du curseur
 */
void Menu::CurseurOn(uint8_t x, uint8_t y, uint8_t Nb)
  {
  uint8_t i;

  for (i=0; i<Nb; i++)
     {
      this->setCursor(x+i, y+1);
      this->print('^');
     }
  }

/*****
 * CurseurOff : Efface le curseur de modification
 *     x  : position en x du curseur 
 *     y  : position en y du curseur 
 *     Nb : nombre de caactères du curseur
 */
void Menu::CurseurOff(uint8_t x, uint8_t y, uint8_t Nb)
  {
  uint8_t i;

  for (i=0; i<Nb; i++)
     {
      this->setCursor(x+i, y+1);
      this->print(' ');
     }
   
  }

/*****
 * AfficheFond : affiche le fond d'un menu
 *      LigneMenu : pointeur vers un tableau conportant les lignes de fond d'un menu
 */
void Menu::AfficheFond(LigneMenu MaLigne[]) 
  {
    int8_t i = 0; 
    while (MaLigne[i].x != -1)
       {
       this->setCursor(MaLigne[i].x, MaLigne[i].y);
       this->print(MaLigne[i].Texte);
       i++;
       }
  }

/*****
 * AfficheFondNew : affiche le fond d'un menu stocké en Eeprom
 *         AdresseL1 : Addresse en mémoire Eeprom de la première ligne du fonc de menu
 *                     La deuxième est stockée 22 octets plus loin
 */
void Menu::AfficheFondNew(int AdresseL1) 
  {
    int8_t x;
    int8_t y=0;
    char *Ligne;

    for (y = 0 ; y<2; y++)
      {
      x=0;  
      Ligne = LireEepromString(AdresseL1 + (y * 22));
      while ( (Ligne[x] !=0) && (x<20))
        {
        this->setCursor(x, y);
        if (Ligne[x] != ' ')
           {
            this->print(Ligne[x]);  
           }
        x++;   
        }
     }
  }
/*****
 * ParamActif : permet de savoir si un paramètre est actif
 *      N      : Numéro du paramètre de 1 à 6
 *      Retour : Vrai ou Faux
 */
bool Menu::ParamActif(uint8_t N)
  { 
    N-=1;
    if ((N >=0) && (N<=5))
      {
       return _Param[N].Actif; 
      }
  }
/*****
 * ParamSetActif : permet de modifier activité d'un paramètre
 *      N    : Numéro du paramètre de 1 à 6
 *      Etat : Etat d'activité
 */
void Menu::ParamSetActif(uint8_t N, bool Etat)
  { 
    N-=1;
    if ((N >=0) && (N<=5))
      {
       _Param[N].Actif=Etat; 
      }
  }

 /*****
 * ParamSetSouligne : permet de modifier l'état souligné des paramètre
 *      N    : Numéro du paramètre de 1 à 6
 *      Etat : Etat de soulignement
 */
void Menu::ParamSetSouligne(uint8_t N, bool Etat)
  { 
    N-=1;
    if ((N >=0) && (N<=5))
      {
       _Param[N].Souligne=Etat; 
      }
  }

/*****
* InitParam : permet d'initialiser les champs Param
*      N      : Numéro du paramètre de 1 à 6
*      Entier : valeur du paramètre
*      x      : position en x
*      y      : positon en y
*      Lg     : longueur du champ cursuer
*      Min    : valeur mini du champ
*      Max    : valeur max du champ
*/
void Menu::InitParam(uint8_t N, int Entier, int x, int y, char *Form, int Lg, int Min, int Max)  
  {
    N-=1;
    if ((N >=0) && (N<=5))
      {
      _Param[N].Entier = Entier;
      _Param[N].x      = x;
      _Param[N].y      = y;
      _Param[N].Lg     = Lg;
      _Param[N].Min    = Min;
      _Param[N].Max    = Max;
      strncpy(_Param[N].Format, Form, 8);
      sprintf(_Param[N].Texte, _Param[N].Format, Entier);
      }
  }

/*****
* InitParam : permet d'initialiser les champs Param
*      N      : Numéro du paramètre de 1 à 6
*      Entier : valeur du paramètre
*/
void Menu::InitParam(uint8_t N, int Entier)  
  {
    N-=1;
    if ((N >=0) && (N<=5))
      {
      _Param[N].Entier = Entier;
      sprintf(_Param[N].Texte,  _Param[N].Format, Entier);
      }
  }


/*****
* RazParam : rend les paramètres inactifs
*/
void Menu::RazParam()
{
int8_t i; 
  
for (i=0; i<6; i++)
   {
    _Param[i].Actif = false;
   }  
}

/*****
* ValeurParam : Renvoi la valeur d'un paramètre sous forme entière
*      N      : Numéro du paramètre de 1 à 6
*/
int  Menu::ValeurParam(uint8_t N)
   {
    N-=1;
    if ((N >=0) && (N<=5))
      {
      return _Param[N].Entier;
      }
    else
      {
      return 0;  
      }
   }

/*****
* TexteParam : Renvoi la valeur d'un paramètre sous texte
*      N      : Numéro du paramètre de 1 à 6
*/
char *Menu::TexteParam(uint8_t N)
  {
    N-=1;
    if ((N >=0) && (N<=5))
      {
      return _Param[N].Texte;
      }    
  }

/*****
* IncParam : Incrémente la valeur la valeur d'un paramètre
*      N      : Numéro du paramètre de 1 à 6
*      retour : nouvelle valeur
*/
int Menu::IncParam(uint8_t N)
  {
    N-=1;
    if ((N >=0) && (N<=5))
      {
      if (_Param[N].Entier < _Param[N].Max)
        {
         // augmentation du pas d'incrémentation en fonction du temp appuyé sur le bouton 
        _Param[N].Entier =  _Param[N].Entier + 1 + (Bouton3.dureeEnfonce()/1000);
        if(_Param[N].Entier > _Param[N].Max)
           _Param[N].Entier = _Param[N].Max;     
        sprintf(_Param[N].Texte, _Param[N].Format, _Param[N].Entier);
        }
      return _Param[N].Entier;
      }    
  }

/*****
* IncParam : décrémente la valeur la valeur d'un paramètre
*      N      : Numéro du paramètre de 1 à 6
*      retour : nouvelle valeur
*/
int Menu::DecParam(uint8_t N)
  {
    N-=1;
    if ((N >=0) && (N<=5))
      {
      if (_Param[N].Entier > _Param[N].Min)
        {
        // augmentation du pas de décrémentation en fonction du temp appuyé sur le bouton 
          _Param[N].Entier = _Param[N].Entier - 1 - (Bouton2.dureeEnfonce()/1000);    
        if(_Param[N].Entier < _Param[N].Min)
           _Param[N].Entier = _Param[N].Min;     
        sprintf(_Param[N].Texte, _Param[N].Format, _Param[N].Entier);
        }
      return _Param[N].Entier;
      }    
  }
/*****
* ModifierParam : Appel le menu de modification d'un paramètre
*                 avant d'appeler la fonction il faut inialiser la struture param
*                 la valeur de retour étant récupérée dans la stucture si celle-ci n'a pas été effacée
*                 dans ce cas la valeur n'est pas modifiée
*             Val      : valeur initial du paramètre à modifier    
*             MenuC    : Pointeur sur menu courant
*             ParamC   : pointeur sur paramètre courant
*             NiveauM  : Pointeur sur niveau Menu
*             MenuP    : numéro du menu précédent
*             MenuS    : numméro du menu suivant
*             Multi    : si vrai, menu avec multiparamètres
*      retour : true si le paramètre à été modifié
*/

 bool Menu::ModifierParam(uint8_t &MenuC, uint8_t &ParamC, uint8_t &NiveauM ,uint8_t MenuP, uint8_t MenuS, bool Multi=false)
   {
   if ( NiveauM == 1)
      {
      this->InitMenu1(LireEepromString(EEPROMBoutonQuitter));
      this->InitMenu2(LireEepromString(EEPROMBoutonPrecedent));
      this->InitMenu3(LireEepromString(EEPROMBoutonSuivant));
      this->InitMenu4(LireEepromString(EEPROMBoutonModifier));
      }
   if ( NiveauM == 2)
      {
      this->InitMenu1(LireEepromString(EEPROMBoutonAnnuler));
      this->InitMenu2(LireEepromString(EEPROMBoutonMoins));
      this->InitMenu3(LireEepromString(EEPROMBoutonPlus));
      this->InitMenu4(LireEepromString(EEPROMBoutonValider));
     }
   if ( NiveauM == 3)
     {
     this->InitMenu1(LireEepromString(EEPROMBoutonPrecedent));
     this->InitMenu2(LireEepromString(EEPROMBoutonMoins));
     this->InitMenu3(LireEepromString(EEPROMBoutonPlus));
     this->InitMenu4(LireEepromString(EEPROMBoutonSuivant));
     }
   if ( NiveauM == 4)
     {
     this->InitMenu1(LireEepromString(EEPROMBoutonAnnuler));
     this->InitMenu2(LireEepromString(EEPROMBoutonModifier));
     this->InitMenu3(LireEepromString(EEPROMBoutonVide));
     this->InitMenu4(LireEepromString(EEPROMBoutonValider));
     }
   
             if (Bouton1.vientDEtreEnfonce() && (NiveauM == 1))  // Quitter sur niveau de menu 1
                {
                this->RazParam();
                MenuC=1;
                return true;
                }
             if (Bouton2.vientDEtreEnfonce() && (NiveauM == 1))  // Précédent sur niveau de menu 1
                {
                this->RazParam();
                MenuC = MenuP;
                }
             if (Bouton3.vientDEtreEnfonce() && (NiveauM == 1))  // Suivant sur niveau de menu 1
                {
                this->RazParam();
                MenuC = MenuS;
                }
             if (Bouton4.vientDEtreEnfonce() && (NiveauM == 1))  
                {
                Bouton4.actualiser();              
                NiveauM =  Multi ? 3 :2 ;   // gestion du mode Multi
                ParamC=1;
                this->ParamSetSouligne(ParamC, true); 
                }
             if (Bouton1.vientDEtreEnfonce() && (NiveauM == 2))  // Annulation
                {
                this->ParamSetSouligne(ParamC, false); 
                this->RazParam();
                ParamC    = 1;
                NiveauM   = 1;
                }
             if (Bouton2.estEnfonce() && ((NiveauM == 2) || (NiveauM == 3)))  // Moins
                {
                this->DecParam(ParamC);  
                }
             if (Bouton3.estEnfonce() && ((NiveauM == 2) || (NiveauM == 3)))  // Plus
                {
                this->IncParam(ParamC);  
                }
             if (Bouton4.vientDEtreEnfonce() && (NiveauM == 2))  // valider
                {
                this->ParamSetSouligne(ParamC, false);
                NiveauM=1;
                return true;
                }
              if (Bouton1.vientDEtreEnfonce() && (NiveauM == 3))  // Précedent en mode Multi
                {
                Bouton1.actualiser(); 
                this->ParamSetSouligne(ParamC, false); 
                ParamC-=1;
                if (ParamC < 1)
                   {
                    NiveauM = 4;
                   }
                else
                  {   
                  this->ParamSetSouligne(ParamC, true);
                  }
                }  
            if (Bouton4.vientDEtreEnfonce() && (NiveauM == 3))  // suivant en mode Multi
                {
                int8_t NbParam=0;

                while ( (this->ParamActif(NbParam)) && (NbParam <7))
                    NbParam++; 
                Bouton4.actualiser(); 
                this->ParamSetSouligne(ParamC, false); 
                ParamC+=1;
                if (ParamC >= NbParam)
                   {
                    NiveauM = 4;
                   }
                else
                  {   
                  this->ParamSetSouligne(ParamC, true);
                  }
                }  
             if (Bouton1.vientDEtreEnfonce() && (NiveauM == 4))  // Annuler niveau 4
                {
                this->RazParam();
                ParamC    = 1;
                NiveauM   = 1;
                }
             if (Bouton2.vientDEtreEnfonce() && (NiveauM == 4))  // Modifier niveau 4
                {
                ParamC = 1;
                this->ParamSetSouligne(ParamC, true); 
                NiveauM  = 3;
                }
             if (Bouton4.vientDEtreEnfonce() && (NiveauM == 4))  // validation niveau 4
                {
                ParamC = 1;
                NiveauM  = 1;
                return true;
                }
  
    return false;            
   }
  
