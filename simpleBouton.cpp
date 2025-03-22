//Librairie Arduino pour la gestion de boutons poussoirs simples
#include "simpleBouton.h"

//Cuisine interne - décomposition du statut pour tout gérer sur un octet :
//bit 7 = état du dernier digitalRead : 0 <=> LOW (bouton enfoncé) et 1 <=> HIGH (bouton relâché)
//bit 6 = indicateur de changement d'état entre deux actualisations : 1 <=> changement
//bits 0 à 5 = numéro de pin, de 0 à 63

//NB : Les constantes ci-desssous sont dégagées à la compil. Leurs valeurs sont insérées directement dans le code exécutable.
const uint8_t bit_etat_simpleBouton = 7;
const uint8_t bit_chgt_simpleBouton = bit_etat_simpleBouton - 1;

const uint8_t masque_etat_simpleBouton = 1 << bit_etat_simpleBouton;
const uint8_t masque_chgt_simpleBouton = 1 << bit_chgt_simpleBouton;
const uint8_t masque_pin_simpleBouton = 0b11111111 & ~(masque_etat_simpleBouton | masque_chgt_simpleBouton);

//Méthodes constantes non développées dans le .h (pour masquer la cuisine interne)
bool simpleBouton::estEnfonce() const {return (this->_statut & masque_etat_simpleBouton) == 0;}
bool simpleBouton::estRelache() const {return (this->_statut & masque_etat_simpleBouton) != 0;}
bool simpleBouton::estStable()  const {return (this->_statut & masque_chgt_simpleBouton) == 0;}
uint8_t simpleBouton::pin() const     {return  this->_statut & masque_pin_simpleBouton;}

//Constructeur
simpleBouton::simpleBouton(uint8_t pin, uint8_t delai_debounce_ms)
{
  //Initialisation des variables internes
  pin &= masque_pin_simpleBouton;
  this->_statut = masque_etat_simpleBouton | pin;
  this->_delai_debounce_ms = delai_debounce_ms;
  this->_millis_etat_actuel = millis();

  //Initialisation hardware
  pinMode(pin, INPUT);
}

//Méthode principale
bool simpleBouton::actualiser()
{
  uint8_t etat_precedent = this->_statut & masque_etat_simpleBouton;

  //Lecture physique
  uint8_t etat_courant = (digitalRead(this->pin()) == HIGH) ? masque_etat_simpleBouton : 0;

  //Filtrage temporel
  uint32_t maintenant = millis();
  if (etat_courant != etat_precedent)
  {
    uint32_t delai = maintenant - this->_millis_etat_actuel;
    if (delai < this->_delai_debounce_ms)
    {
      etat_courant = etat_precedent;
    }
  }

  //Mise à jour des variables internes
  bool changement = (etat_courant != etat_precedent);
  if (changement)
  {
    this->_statut ^= masque_etat_simpleBouton; //inversion du bit d'état
    this->_statut |= masque_chgt_simpleBouton; //passage à 1 du bit de changement
    this->_millis_etat_actuel = maintenant;
  }
  else
  {
    this->_statut &= ~masque_chgt_simpleBouton; //passage à 0 du bit de changement
  }
  return changement;
}

uint16_t boutonAction::delai_max_interclic_ms = 500;

uint32_t boutonAction::_delaiRepetitionParDefaut(uint32_t duree)
{ //Exemple de cadence progressive
  if (duree <  1000) return 1000;
  if (duree <  3000) return  500;
  if (duree <  7000) return  250;
  if (duree < 12000) return  100;
  if (duree < 18000) return   50;
  return 25;
}

boutonAction::boutonAction(uint8_t pin, uint8_t delai_debounce_ms) : bouton(pin, delai_debounce_ms)
{
  _simpleClic = NULL;
  _doubleClic = NULL;
  _tripleClic = NULL;
  _statut = 0;
  _millis_dernier_clic = 0;
  _millis_derniere_action = 0;
  _delaiRepetition = _delaiRepetitionParDefaut;
}

void boutonAction::attacher(void (*clic)(), void (*clicClic)(), void (*clicClicClic)())
{
  _simpleClic = clic;
  _doubleClic = clicClic;
  _tripleClic = clicClicClic;
}

void boutonAction::activerRepetition(uint32_t (*delaiRepetitionPersonnalise)(uint32_t))
{
  _statut |= 0x80;
  if (delaiRepetitionPersonnalise != NULL) _delaiRepetition = delaiRepetitionPersonnalise;
}

void boutonAction::desactiverRepetition()
{
  _statut &= 0x7f;
}

void boutonAction::actualiser()
{
  bouton.actualiser();

  bool repetition_activee = !!(_statut & 0x80);
  if (repetition_activee) _statut &= 0x7f;

  //alors là bon courage pour comprendre ce code après factorisation maximale
  //un indice : le statut varie de 0 à 7
  uint32_t maintenant = millis();
  uint8_t action = 0;
  uint8_t st = _statut % 3;
  if (st == 0) {
    if (bouton.vientDEtreEnfonce()) {
       if (_statut == 6 || (_tripleClic == NULL && (_statut == 3 || _doubleClic == NULL))) {
        action = _statut + 3;
        _statut++;
      } else {
        _millis_dernier_clic = maintenant;
        _statut += 2;
      }
    } else if (_statut > 0 && maintenant - _millis_dernier_clic >= delai_max_interclic_ms) {
      action = _statut;
      _statut = 0;
    }
  } else if (st == 1) {
    if (bouton.estRelache()) _statut = 0;
    else if (repetition_activee && maintenant - _millis_derniere_action >= _delaiRepetition(bouton.dureeEnfonce())) {
      action = _statut + 2;
    }
  } else {
    if (bouton.estRelache()) _statut++;
    else if (maintenant - _millis_dernier_clic >= delai_max_interclic_ms) {
      action = _statut + 1;
      _statut--;
    }
  }

  if (repetition_activee) _statut |= 128;

  if (action) _millis_derniere_action = maintenant;
  if (action == 3) _simpleClic();
  if (action == 6) _doubleClic();
  if (action == 9) _tripleClic();
}
