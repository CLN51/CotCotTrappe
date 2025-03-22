//-----------------------------------------//
//                                         //
//             TrappePoule                 //  
//             Ephemeride.h                //
//                                         //
//-----------------------------------------//
class Heure
{
public :
  Heure();                              // constructeur sans paramètre;
  Heure(int T);                         // constructeur avec une valeur en minute
  Heure(int H, int S);                  // constructeur avec une valeur en heure et minute
  int Temps();                          // retourne la valeur en minutes
  int H();                              // retourne la partie heure
  int M();                              // retourne la partie minute
  void Init(int Val);                   // Initialisation de la valeur en minute
  char *PrintT();                       // retourne la valeur sous forme hh:mm 
  Heure & operator+=(const int & Val);  // surcharge opérateur pour ajouter des minutes
  Heure & operator-=(const int & Val);  // surcharge opérateur pour supprimer des minutes
  
private :
  int  Valeur;                         // valeur de l'heure expriméee en seconde
  char HeureChar[3];                   // heure au format texte
  char MinuteChar[3];                  // minute au format texte
  char TempsChar[6];                   // Heure et minute au format texte

};

void calculerEphemeride(int jour, int mois, int annee, int LongDegre, int LongMinute,int LongSeconde, int LatDegre, int LatMinute, int LatSeconde, Heure *Hlever, Heure *Hcoucher); // calcul de l'heure de lever et coucher en coordonées degré-minute-seconde
void calculerEphemeride(int jour, int mois, int annee, double longitude_ouest, double latitude_nord, Heure *Hlever, Heure *Hcoucher);                                               // calcul de l'heure de lever et coucher en coordonées décimales
