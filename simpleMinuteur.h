//Librairie Arduino pour la gestion de délais
//En utilisant la notion de minuteur
//Version 2.0
//
//8 octets alloués en RAM par minuteur
//
//Bricoleau 2016

#ifndef simpleMinuteur_h
#define simpleMinuteur_h

#include <Arduino.h>

class simpleMinuteur
{
  public :
    //constructeur
    simpleMinuteur(uint32_t duree_ms = 0); //si le minuteur a toujours la même durée, la renseigner dès la construction

    //actions essentielles
    void demarrer();                       //lance le minuteur à partir de maintenant et pour la durée pré-enregistrée
    void demarrer(uint32_t duree_ms);      //lance le minuteur à partir de maintenant pour une durée différente
    void redemarrer();                     //relance le minuteur à la suite du précédent (utile pour cadence régulière)
    void redemarrer(uint32_t duree_ms);    //relance le minuteur à la suite du précédent, mais sur une durée différente
    operator bool();                       //Retourne true si le minuteur est terminé, puis le redémarre

    //état du minuteur
    uint32_t duree() const                 {return this->_duree;}
    uint32_t tempsEcoule() const           {return millis() - this->_millis_depart;}

    bool     estActif() const              {return tempsEcoule() <  duree();}
    bool     estTermine() const            {return tempsEcoule() >= duree();}
    uint32_t tempsRestant() const          {return estTermine() ? 0 : duree() - tempsEcoule();}

    //actions secondaires
    void repousser(uint32_t duree_ms);     //ajoute une duree supplémentaire à celle pré-enregistrée

  private :
    uint32_t _millis_depart, _duree;
};

#endif
