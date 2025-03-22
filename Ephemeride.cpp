//-----------------------------------------//
//                                         //
//             TrappePoule                 //  
//           Ephemeride.cpp                //
//                                         //
//-----------------------------------------//
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "Ephemeride.h"

/*****
 * Heure : constructeur de la classe
 *     l'heure est initialisée à 00:00 
 */
Heure::Heure()
{
  Valeur = 0;
}

/*****
 * Heure : constructeur de la classe
 *     T : paramètre d'initialisation exprimé en minutes 
 */
Heure::Heure(int T)
{
  Valeur = T;
}

/*****
 * Heure : constructeur de la classe
 *     H : Heure d'initialisation
 *     M : Minute d'initialisation 
 */
Heure::Heure(int H, int M)
{
  Valeur = H*60 + M;
}

/*****
 * Temps : renvoie la valeur sous forme minute
 */
int Heure::Temps()
  {
  return Valeur;
  }

/*****
 * H : renvoie la partie heure
 */
int Heure::H()
  {
  return (Valeur / 60);  
  }

/*****
 * M : renvoie la partie minute
 */
int Heure::M()
  {
  return (Valeur % 60);  
  }

/*****
 * Init : permet d'initialiser un objet Heure à partir d'une valeur exprimée en minutes
 *     Val : valeur d'initialisation exprimée en minutes 
 */
void Heure::Init(int Val)
  {
    Valeur = Val;
  }

/*****
 * PrintT : retourne la valeur de l'heure sous la forme HH:MM
 */
char *Heure::PrintT()
  {
    sprintf(TempsChar, "%02d:%02d", Valeur / 60, Valeur % 60);
    return TempsChar;
  }

/*****
 * += : Surcharge de l'opérateur += permetant d'ajouter des minutes
 *     Val : valeur en minute à rajouter 
 */
Heure & Heure::operator+= (const int & Val)
  {
    Valeur += Val;
    return *this;
  }

/*****
 * -= : Surcharge de l'opérateur -= permetant de soustraire des minutes
 *     Val : valeur en minute à soustraire 
 */
Heure & Heure::operator-= (const int & Val)
  {
    Valeur -= Val;
    return *this;
  }

/*****
 * FormaterHeure : permet de formater une heure exprimée au format décimal (julien) en objet Heure
 *     d : valeur de l'heure au format décimal (julien) 
 *     Time : valeur calclée au format Heure
 */
void FormaterHeure(double d, Heure *Time)
{
  int h,m,s;
  char texte[20];

  d = d + 0.5;


  h = d * 24.0;
  d = d - (double) h / 24.0;
  m = d * 1440.0;
  d = d - (double) m / 1440.0;
  s = d * 86400.0 + 0.5;

  Time->Init(h*60+m);
}

/*****
 * calculerCoordonneeDecimale : renvoi une coordonnée décimale a partir d'une coordonnée exprimée en degré-minute-seconde.
 *     degre   : valeur degrée de la coordonée a convertir 
 *     minute  : valeur minute de la coordonée a convertir 
 *     seconde : valeur seconde de la coordonée a convertir 
 */
double calculerCoordonneeDecimale(int degre, int minute, int seconde)
{
  if (degre > 0)
  {
    return (double) degre + minute / 60.0 + seconde / 3600.0;
  }
  else
  {
    return (double) degre - minute / 60.0 - seconde / 3600.0;
  }
}


/*****
 * calculerCentreEtVariation : fonction interne.
 */
void calculerCentreEtVariation(double longitude_ouest, double sinlat, double coslat, double d, double *centre, double *variation)
{
  //constantes précalculées par le compilateur
  const double M_2PI = 2.0 * M_PI;
  const double degres = 180.0 / M_PI;
  const double radians = M_PI / 180.0;
  const double radians2 = M_PI / 90.0;
  const double m0 = 357.5291;
  const double m1 = 0.98560028;
  const double l0 = 280.4665;
  const double l1 = 0.98564736;
  const double c0 = 0.01671;
  const double c1 = degres * (2.0*c0 - c0*c0*c0/4.0);
  const double c2 = degres * c0*c0 * 5.0 / 4.0;
  const double c3 = degres * c0*c0*c0 * 13.0 / 12.0;
  const double r1 = 0.207447644182976; // = tan(23.43929 / 180.0 * M_PI / 2.0)
  const double r2 = r1*r1;
  const double d0 = 0.397777138139599; // = sin(23.43929 / 180.0 * M_PI)
  const double o0 = -0.0106463073113138; // = sin(-36.6 / 60.0 * M_PI / 180.0)

  double M,C,L,R,dec,omega,x;

  //deux ou trois petites formules de calcul
  M = radians * fmod(m0 + m1 * d, 360.0);
  C = c1*sin(M) + c2*sin(2.0*M) + c3*sin(3.0*M);
  L = fmod(l0 + l1 * d + C, 360.0);
  x = radians2 * L;
  R = -degres * atan((r2*sin(x))/(1+r2*cos(x)));
  *centre = (C + R + longitude_ouest)/360.0;

  dec = asin(d0*sin(radians*L));
  omega = (o0 - sin(dec)*sinlat)/(cos(dec)*coslat);
  if ((omega > -1.0) && (omega < 1.0))
    *variation = acos(omega) / M_2PI;
  else
    *variation = 0.0;
}



/*****
 * calculerEphemeride : calcul de heure de levre et de coucher du soleil pour une date et une ccordonnée.
 *       jour            : jour de la date
 *       mois            : mois de la date
 *       annee           : année de la date
 *       longitude-ouest : longitude ouest de la coordonnée, une longitude est s'exprime en négatif
 *       latitude_nour   : latitude nord de la coordonée
 *       Hlever          : heure de lever calculée sous forme objet HEURE
 *       Hcoucher        : heure de coucher calculée sous forme objjet HEURE
 */
void calculerEphemeride(int jour, int mois, int annee, double longitude_ouest, double latitude_nord, Heure *Hlever, Heure *Hcoucher)
{
  int nbjours;
  const double radians = M_PI / 180.0;
  double d, x, sinlat, coslat;
  double lever;
  double meridien;
  double coucher;

  //calcul nb jours écoulés depuis le 01/01/2000
  if (annee > 2000) annee -= 2000;
  nbjours = (annee*365) + ((annee+3)>>2) + jour - 1;
  switch (mois)
  {
    case  2 : nbjours +=  31; break;
    case  3 : nbjours +=  59; break;
    case  4 : nbjours +=  90; break;
    case  5 : nbjours += 120; break;
    case  6 : nbjours += 151; break;
    case  7 : nbjours += 181; break;
    case  8 : nbjours += 212; break;
    case  9 : nbjours += 243; break;
    case 10 : nbjours += 273; break;
    case 11 : nbjours += 304; break;
    case 12 : nbjours += 334; break;
  }
  if ((mois > 2) && (annee&3 == 0)) nbjours++;
  d = nbjours;

  //calcul initial meridien & lever & coucher
  x = radians * latitude_nord;
  sinlat = sin(x);
  coslat = cos(x);
  calculerCentreEtVariation(longitude_ouest, sinlat, coslat, d + longitude_ouest/360.0, &meridien, &x);
  lever = meridien - x;
  coucher = meridien + x;

  //seconde itération pour une meilleure précision de calcul du lever
  calculerCentreEtVariation(longitude_ouest, sinlat, coslat, d + lever, &lever, &x);
  lever = lever - x;

  //seconde itération pour une meilleure précision de calcul du coucher
  calculerCentreEtVariation(longitude_ouest, sinlat, coslat, d + coucher, &coucher, &x);
  coucher = coucher + x;

  FormaterHeure(lever, Hlever);
  FormaterHeure(coucher, Hcoucher);
}

/*****
 * calculerEphemeride : calcul de heure de levre et de coucher du soleil pour une date et une ccordonnée.
 *       jour        : jour de la date
 *       mois        : mois de la date
 *       annee       : année de la date
 *       LongDegre   : degre de la longitude est
 *       LongMinute  : minute de la longitude est
 *       LongSeconde : seconde de la longitude est
 *       LatDegre    : degré de la latitude nord de la coordonée
 *       LatMinute   : minute de la latitude nord de la coordonée
 *       LatSeconde  : seconde de la latitude nord de la coordonée
 *       Hlever      : heure de lever calculée sous forme objet HEURE
 *       Hcoucher    : heure de coucher calculée sous forme objjet HEURE
 */
void calculerEphemeride(int jour, int mois, int annee, int LongDegre, int LongMinute,int LongSeconde, int LatDegre, int LatMinute, int LatSeconde, Heure *Hlever, Heure *Hcoucher)
{
  double Longitude;
  double Latitude;

  Longitude = calculerCoordonneeDecimale( LongDegre, LongMinute, LongSeconde);
  Latitude = calculerCoordonneeDecimale(LatDegre, LatMinute, LatSeconde);
  calculerEphemeride(jour , mois, annee, Longitude, Latitude, Hlever, Hcoucher);
}
