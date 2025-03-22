//Librairie Arduino pour la gestion de délais
//En utilisant la notion de minuteur
//Bricoleau 2016
#include "simpleMinuteur.h"

simpleMinuteur::simpleMinuteur(uint32_t duree_ms)
{
  this->_millis_depart = 0;
  this->_duree = duree_ms;
}

void simpleMinuteur::demarrer()
{
  this->_millis_depart = millis();
}

void simpleMinuteur::demarrer(uint32_t duree_ms)
{
  this->_duree = duree_ms;
  this->demarrer();
}

void simpleMinuteur::redemarrer()
{
  this->_millis_depart += this->_duree;
}

void simpleMinuteur::redemarrer(uint32_t duree_ms)
{
  this->redemarrer();
  this->_duree = duree_ms;
}

void simpleMinuteur::repousser(uint32_t duree_ms)
{
  this->_duree += duree_ms;
}

simpleMinuteur::operator bool()
{
  bool fini = this->estTermine();
  if (fini) this->redemarrer();
  return fini;
}
