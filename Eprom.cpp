//-----------------------------------------//
//                                         //
//             TrappePoule                 //  
//              Eprom.cpp                  //
//                                         //
//-----------------------------------------//
#include <avr/eeprom.h>
#include <EEPROM.h>
#include "Eprom.h"

/*****
 * LireEepromInt : Fonction de lecture d'un entier à une adresse donnée en Eeprom
 *     retour : valeur de l'entier lue 
 */
int IntEeprom::LireEepromInt(int adresse)
  {
  unsigned int retour = 0;
  char i;

  for (i=0; i<2; i++)
    {
    retour <<=8;
    retour +=eeprom_read_byte((unsigned char *)adresse+i);
    }
  return retour;
  }


/*****
 * EcrireEepromInt : Fonction de lecture d'un entier à une adresse donnée en Eeprom
 *     retour : valeur de l'entier lue 
 */
void IntEeprom::EcrireEepromInt(int adresse, unsigned int valeur)
    {
    char c;
    char i;
    for (i=0; i<2;i++)
      {
      c=valeur & 0xFF;
      eeprom_write_byte((unsigned char *)adresse + 1 - i,c);
      valeur >>=8;
      }
    }

/*****
 * IntEeprom : constructeur de la classe
 *     Adresse : adresse de l'entier en mémoire Eprom 
 */
IntEeprom::IntEeprom(int Adresse)
  {
  _Valeur = LireEepromInt(Adresse);
  _Adresse = Adresse;
  }

/*****
 * Valeur : retourne la veleur entière de l'objet
 */
int IntEeprom::Valeur()
   {
   return _Valeur;
   }

/*****
 * Maj : retourne la veleur entière de l'objet
 */
int IntEeprom::Maj(unsigned int Val)
   {
   this->EcrireEepromInt(_Adresse, Val);
   _Valeur = Val;
   return Val;
   }

/*****
 * = : surcharge de l'opérateur =
 *     Val : valeur a affecter 
 */
IntEeprom & IntEeprom::operator=(const int & Val)
{
  _Valeur = Val;
  EcrireEepromInt(_Adresse, _Valeur);
  return *this;
}

/*****
 * = : surcharge de l'opérateur +=
 *     Val : valeur a additionner 
 */
IntEeprom & IntEeprom::operator+=(const int & Val)
{
  _Valeur += Val;
  EcrireEepromInt(_Adresse, _Valeur);
  return *this;
}

/*****
 * = : surcharge de l'opérateur -=
 *     Val : valeur a additionner 
 */
IntEeprom & IntEeprom::operator-=(const int & Val)
{
  _Valeur -= Val;
  EcrireEepromInt(_Adresse, _Valeur);
  return *this;
}


/*****
 * LireEepromString : permet de lire une chaine de caractères stokée en Eeprom.
 *     adresse : adresse du début de la chaine 
 *     la fonction renvoi un pointeur vers cette chaine
 *     la longueur de la chaine est limitée à 20 caractères
 */
char *LireEepromString(int adresse)
  {
  static char Retour[21];
  int8_t i =0;
  
  while ((EEPROM.read(adresse) != 0) && (i<20))
     {
    Retour[i] = EEPROM.read(adresse);
     i++;
     adresse++;
     }
  Retour[i]=0;
  
  return Retour;
  }
  

/*****
 * EcrireEepromString : permet d'écriree une chaine de caractères en Eeprom.
 *     adresse : adresse du début de la chaine en Eeprom
 *     valeur  : contenu de la chaine de caractère
 *     la longueur de la chaine est limitée à 20 caractères
 */
void EcrireEepromString(int adresse, char *valeur)
    {
  int i=adresse;
  
    while ( (*valeur != 0) && ( (adresse-i) < 20) )
      {
      EEPROM.write(adresse,*valeur);
      adresse+=1;
      valeur++;
      }
    //adresse++;
//    eeprom_write_byte(adresse,0);
    EEPROM.write(adresse,0);
    }
    
