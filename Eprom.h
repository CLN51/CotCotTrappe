//-----------------------------------------//
//                                         //
//             TrappePoule                 //  
//               Eprom.h                   //
//                                         //
//-----------------------------------------//
#define EEPROMLONGDEGRE            0x00
#define EEPROMLONGMINUTE           0x02
#define EEPROMLONGSECONDE          0x04
#define EEPROMLATDEGRE             0x06
#define EEPROMLATMINUTE            0x08
#define EEPROMLATSECONDE           0x0A
#define EEPROMDELAISOUVERTURE      0x0C
#define EEPROMDELAISFERMETURE      0x0E
#define EEPROMHAUTEUROUVERTURE     0X10
#define EEPROMPASMOTEUR            0x12
#define EEPROMTIMEOUTTRAPPE        0x14
#define EEPROMDELAISALLUMAGE       0x16
#define EEPROMTEMPSALLUMAGE        0x18
#define EEPROMDUREEALLUMAGE        0x1A
#define EEPROMHEUREOUVERTURE       0x1C
#define EEPROMMINUTEOUVERTURE      0x1E
#define EEPROMHEUREFERMETURE       0x20
#define EEPROMMINUTEFERMETURE      0x22
#define EEPROMLUMINOSITEBASSE      0x24
#define EEPROMLUMINOSITEHAUTE      0x26
#define EEPROMDELAISLUX            40
#define EEPROMVMINMOTEUR           42
#define EEPROMVMAXMOTEUR           44
#define EEPROMDUREEMINUTERIE       46
#define EEPROMPRESENCEFDC          48
#define EEPROMBoutonMenu           100
#define EEPROMBoutonVide           106
#define EEPROMBoutonMode           112
#define EEPROMBoutonPrecedent      118
#define EEPROMBoutonSuivant        124
#define EEPROMBoutonModifier       130
#define EEPROMBoutonQuitter        136
#define EEPROMBoutonAnnuler        142
#define EEPROMBoutonMoins          148
#define EEPROMBoutonPlus           154
#define EEPROMBoutonValider        160
#define EEPROMBoutonTrappe         166
#define EEPROMBoutonLumiere        172
#define EEPROMBoutonInfo           178
#define EEPROMBoutonFdc            184
#define EEPROMMenu9_L1             200
#define EEPROMMenu9_L2             222
#define EEPROMMenu10_L1            244
#define EEPROMMenu10_L2            266
#define EEPROMMenu11_L1            288
#define EEPROMMenu11_L2            310
#define EEPROMMenu12_L1            332
#define EEPROMMenu12_L2            354
#define EEPROMMenu13_L1            376
#define EEPROMMenu13_L2            398
#define EEPROMMenu14_L1            420
#define EEPROMMenu14_L2            442
#define EEPROMMenu15_L1            464
#define EEPROMMenu15_L2            486
#define EEPROMMenu16_L1            508
#define EEPROMMenu16_L2            530
#define EEPROMMenu17_L1            552
#define EEPROMMenu17_L2            574
#define EEPROMMenu18_L1            596
#define EEPROMMenu18_L2            618
#define EEPROMMenu19_L1            640
#define EEPROMMenu19_L2            662
#define EEPROMMenu20_L1            684
#define EEPROMMenu20_L2            706
#define EEPROMMenu21_L1            728
#define EEPROMMenu21_L2            750
#define EEPROMMenu22_L1            772
#define EEPROMMenu22_L2            794
#define EEPROMMenu23_L1            816
#define EEPROMMenu23_L2            838


class IntEeprom                                              // gestion d'un entier stocké en mémoire Eprom
{
public :
  IntEeprom(int Adresse);                                    // constructeur avec l'adresse comme paramètre
  int Valeur();                                              // retour la valeur entière de l'objet
  int Maj(unsigned int Val);                                 // Mise à jour de la valeur
  IntEeprom & operator=(const int & Val);                    // surcharge le l'opérateur d'affectation
  IntEeprom & operator+=(const int & Val);                   // surcharge le l'opérateur +=
  IntEeprom & operator-=(const int & Val);                   // surcharge le l'opérateur -=
 
private :
  int  _Valeur;                                             // valeur de l'entier
  int _Adresse;                                             // Adresse de la valeur;
  int  LireEepromInt(int addresse);                         // fonction de lecture d'un entier à une adresse donnée
  void EcrireEepromInt(int adresse, unsigned int valeur);   // fonction d'écriture d'un entier à une adresse donnée

};

char *LireEepromString(int adresse);                       // Permet de lire une chaine de caractères stockée à l'adresse adresse
void EcrireEepromString(int adresse, char *valeur);        // Permet d'écrire une chaine de caractère à l'adresse adresse

//long LireEepromLong(int addresse);
//int  LireEepromInt(int addresse);
//char LireEepromChar(int addresse);
//void EcrireEepromLong(int adresse, unsigned long valeur);
//void EcrireEepromInt(int adresse, unsigned int valeur);
//void EcrireEepromChar(int adresse, unsigned char valeur);
