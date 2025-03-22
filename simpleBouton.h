//Librairie Arduino pour la gestion de boutons poussoirs
//Version 4.2
//
//Cablage : pin---BP---GND
//
//6 octets alloués en RAM pour un simpleBouton
//Les fonctions non utilisées sont dégagées à la compilation
//
//Bricoleau 2020

#ifndef simpleBouton_h
#define simpleBouton_h

#include <Arduino.h>

class simpleBouton
{
  public :
    //Constructeur
    simpleBouton(uint8_t pin, uint8_t delai_debounce_ms = 10);  // Delai_debounce_ms a 20 à l'origine

    //Lecture hardware et mise à jour des variables internes
    //Idéalement, doit être appelée à chaque début de loop()
    bool actualiser(); //retourne true si l'état du bouton a changé

    //Informations de statut
    bool estEnfonce() const;
    bool estRelache() const;
    bool estStable() const;
    uint8_t pin() const;

    bool vientDEtreEnfonceOuRelache() const               {return !estStable();}
    bool vientDEtreEnfonce() const                        {return vientDEtreEnfonceOuRelache() && estEnfonce();}
    bool vientDEtreRelache() const                        {return vientDEtreEnfonceOuRelache() && estRelache();}

    //Informations de durée
    uint32_t dureeEtatActuel() const                      {return millis() - this->_millis_etat_actuel;}
    uint32_t dureeEnfonce() const                         {return estEnfonce() ? dureeEtatActuel() : 0;}
    uint32_t dureeRelache() const                         {return estRelache() ? dureeEtatActuel() : 0;}

    bool estStableDepuisAuMoins(uint32_t delai_ms) const  {return dureeEtatActuel() >= delai_ms;}
    bool estEnfonceDepuisAuMoins(uint32_t delai_ms) const {return estStableDepuisAuMoins(delai_ms) && estEnfonce();}
    bool estRelacheDepuisAuMoins(uint32_t delai_ms) const {return estStableDepuisAuMoins(delai_ms) && estRelache();}

    bool estStableDepuisPlusDe(uint32_t delai_ms) const   {return dureeEtatActuel() > delai_ms;}
    bool estEnfonceDepuisPlusDe(uint32_t delai_ms) const  {return estStableDepuisPlusDe(delai_ms) && estEnfonce();}
    bool estRelacheDepuisPlusDe(uint32_t delai_ms) const  {return estStableDepuisPlusDe(delai_ms) && estRelache();}

    uint8_t delaiDebounceMs() const                       {return this->_delai_debounce_ms;}

    //Et pour un usage minimaliste :
    operator bool() {return actualiser() && estEnfonce();}

  private :
    uint8_t  _statut, _delai_debounce_ms;
    uint32_t _millis_etat_actuel;
};

class boutonAction
{
  public :
    boutonAction(uint8_t pin, uint8_t delai_debounce_ms = 20);

    void attacher(void (*clic)(), void (*clicClic)() = NULL, void (*clicClicClic)() = NULL);

    static uint16_t delai_max_interclic_ms;

    void activerRepetition(uint32_t (*delaiRepetitionPersonnalise)(uint32_t) = NULL);
    void desactiverRepetition();

    void actualiser();

  private :
    simpleBouton bouton;
    void (*_simpleClic)();
    void (*_doubleClic)();
    void (*_tripleClic)();
    uint8_t _statut;
    uint32_t _millis_dernier_clic, _millis_derniere_action;
    uint32_t (*_delaiRepetition)(uint32_t);
    //Prendre exemple sur la fonction ci-dessous dans le cpp pour définir une fonction personnalisée
    static uint32_t _delaiRepetitionParDefaut(uint32_t duree_enfonce);
};

#endif
