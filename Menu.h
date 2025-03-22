//-----------------------------------------//
//                                         //
//             TrappePoule                 //  
//               Menu.h                    //
//                                         //
//-----------------------------------------//

//#define FERME     0                       // la trappe est fermée
//#define OUVERT    1                       // la trappe est ouverte
//#define MONTE     2                       // la trappe est en montée, elle s'ouvre
//#define DESCENT   3                       // la trappe est en descente, elle se ferme
#define TrappeManuFixe       0            // la trappe en mode manu est fixe
#define TrappeManuOuverture  1            // la trappe s'ouvre en mode Manu
#define TrappeManuFermeture  2            // la trappe se ferme en mode manu
#define MODEMANU  0                       // fonctionnement en mode manuel
#define MODEEPHE  1                       // fonctionnement en mode éphéméride
#define MODEHORL  2                       // fonctionnement en mode horloge
#define MODELUMI  3                       // fonctionnement en mode détection du crespuscule
#define MODEINIT  4                       // fonctionnement en mode initialisation
#define PINALIMLCD A2                     // Patte de commande de l'alimentation du LCD

extern const char MMenu[];                // Intitulé du bouton Menu   
extern const char MVide[];                // Intitulé du bouton vide
extern const char MMode[];                // Intitulé du bouton Mode
extern const char MPrecedent[];           // Intitulé du bouton Précédent
extern const char MSuivant[];             // Intitulé du bouton Suivant
extern const char MModifier[];            // Intitulé du bouton Modification
extern  const char MQuitter[];             // Intitulé du bouton Quittre
extern const char MAnnuler[];             // Intitulé du bouton Annulé
extern const char MMoins[];               // Intitulé du bouton Moins
extern const char MPlus[];                // Intitulé du bouton Plus
extern const char MValider[];             // Intitulé du bouton Validation
extern const char MTrappe[];              // Intitulé du bouton Trappe
extern const char MLumiere[];             // Intitulé du bouton Lumière


struct LigneMenu {
  int8_t x;
  int8_t y;
  char *Texte;
};
struct Parametre {
  bool Actif;                                 // défini si le paramètre est actif
  bool Souligne;                              // défini si le paramètre est sousligné
  int Entier;                                 // valeur du paramétre sous forme entière
  char Texte[5];                              // valeur du paramètre sous forme Texte
  uint8_t x;                                      // Position en x du paramètre
  uint8_t y;                                      // Position en y du paramètre
  uint8_t Lg;                                     // Longeur du curseur
  int Min;                                    // valeur min du paramètre
  int Max;                                    // Valeur max du paramètre
  char Format[9];                             // format d'edition de la valeur sour forme "%02d"
};

class Menu : public LiquidCrystal_I2C
{
public :
Menu(uint8_t lcdAddr, uint8_t lcdCol,  uint8_t lcdRows);
void On();                                         // Alimente le LCD
void Off();                                        // Coupe l'alimentataion du LCD
void clear();                                      // surcharge de la méthode clear()
void setCursor(uint8_t col, uint8_t row);          // surcharge de la méthode setCursor()
void print(char *chaine);                        // Surcharge de la méthode print()
void print(char car);                        // Surcharge de la méthode print()
void noBacklight();                                // Surcharge de la méthode noBacklight() 
void backlight();                                  // Surcharge de la méthode bacjklight()
void Affiche(uint8_t NMenu);                       // affiche un menu par son numero;
void InitDate(char *Date);                         // renseignement de la valeur date
void PrintDate(uint8_t x=0, uint8_t y=0);          // impression de la date
void InitHeure(char *Heure);                       // renseignement de la valeur heure
void PrintHeure(uint8_t x=15, uint8_t y=0);        // impression de l'heure
void InitVersion(char *Version);                   // renseignement de la version du logiciel
void PrintVersion(uint8_t x=8, uint8_t y=1);       // impression de la version du logiciel
void InitDateVersion(char *Date);                  // renseignement de la version du logiciel
void PrintDateVersion(uint8_t x=6, uint8_t y=2);  // impression de la version du logiciel
void InitTrappe(uint8_t Etat);                     // renseignement de l'état de la trappe
void PrintTrappe(uint8_t x=0, uint8_t y=2);        // impression de l'état de la trappe
void InitLumiere(uint8_t Pourcent);                // renseignement de l'état de la lumière
void PrintLumiere(uint8_t x=7, uint8_t y=2);       // impression de l'état de la lumière
void InitChargeBatterie(uint8_t Pourcent);         // renseignement de l'état de la chage de la batterrie
void PrintChargeBatterie(uint8_t x=14, uint8_t y=2);// impression de l'état de charge de la batterie
void InitLever(char *Valeur);                      // renseignement de l'heure de lever du soleil
void PrintLever(uint8_t x=0, uint8_t y=1);         // impression de l'heure de lever du soleil
void PrintSeparateur(uint8_t x=5, uint8_t y=1);    // impression du séparateur entre l'heure de lever et de coucher
void InitCoucher(char *Valeur);                    // renseignement de l'heure de lever du soleil
void PrintCoucher(uint8_t x=8, uint8_t y=1);       // impression de l'heure de lever du soleil
void InitMode(int8_t Mode);                        // renseignement du mode de fonctionnement MODEAUTO ou MODEMANU
void PrintMode(uint8_t x=14, uint8_t y=1);         // impression du mode de fonctionnment
void InitMenu1(char *Libelle);                     // renseignement du libellé du menu1
void InitMenu2(char *Libelle);                     // renseignement du libellé du menu2
void InitMenu3(char *Libelle);                     // renseignement du libellé du menu3
void InitMenu4(char *Libelle);                     // renseignement du libellé du menu4
void PrintMenu(uint8_t x=0, uint8_t y=3);          // impression du menu
bool ParamActif(uint8_t N);                        // permet de savoir si un paramètre est actif
void ParamSetActif(uint8_t N, bool Etat);          // permet de modifier activité d'un paramètre
void ParamSetSouligne(uint8_t N, bool Etat);       // permet de modifier l'état souligné des paramètre
void InitParam(uint8_t N, int Entier);             // permet d'inialiser le champ Param
void InitParam(uint8_t N, int Entier, int x, int y, char *Form, int Lg, int Min, int Max);
void RazParam();                                   // rend tout les paramètres inactifs
int  ValeurParam(uint8_t N);                       // renvoi la valeur d'un paramètre
char *TexteParam(uint8_t N);                       // renvoi la valeur du paramètre sous forme de texte
int  IncParam(uint8_t N);                          // Incrémente la valeur d'un paramètre
int  DecParam(uint8_t N);                          // Décrémente la valeur d'un paramètre
bool ModifierParam(uint8_t &MenuC, uint8_t &ParamC, uint8_t &NiveauM,uint8_t MenuP, uint8_t MenuS, bool Multi=false); // appel le menu de modification d'un paramètre
 
 private :
  boolean Actif;                              // L'écran LCD est alimenté ou pas
  uint8_t MenuCourant;                        // numéro du menu courant
  char _Date[13];                             // Date courante
  char _Version[9];                           // Version du logiciel
  char _DateVersion[9];                       // Date de la réalisation du logiciel
  char _Heure[6];                             // Heure courante
  uint8_t  _EtatTrappe;                       // Etat de la trappe : FERME, OUVERT, MONTE, DESCENT
  uint8_t  _Lumiere;                          // Valeur de la lumière en %
  uint8_t  _ChargeBatterie;                   // Valeur de la charge batterie en %
  char _Lever[6];                             // heure de lever du soleil
  char _Coucher[6];                           // heure de coucher du soleil
  int8_t  _Mode;                                 // Mode de fonctionnement MODEAUTO ou MODEMANU
  char _Menu1[5];                             // libéllé du menu 1
  char _Menu2[5];                             // libéllé du menu 2
  char _Menu3[5];                             // libéllé du menu 3
  char _Menu4[6];                             // libéllé du menu 4
  Parametre _Param[6];                        // Paramétre optionnel
  void CurseurOn(uint8_t x, uint8_t y, uint8_t Nb);          // affiche un curseur sous la forme ^^ pour matérialiser le parametre en modification
  void CurseurOff(uint8_t x, uint8_t y, uint8_t Nb);         // remplace le curseur par des espaces 
  void AfficheFond(LigneMenu LigneMenu[]);   // affiche le fond d'un menu
  void AfficheFondNew(int AdresseL1);        // affiche le fond d'un menu défini en Eeprom
};
