//-----------------------------------------//
//                                         //
//             TrappePoule                 //  
//               Trappe.h                  //
//                                         //
//-----------------------------------------//
#define FERME     0                       // la trappe est fermée
#define OUVERT    1                       // la trappe est ouverte
#define MONTE     2                       // la trappe est en montée, elle s'ouvre
#define DESCEND   3                       // la trappe est en descente, elle se ferme
 
class CommandeTrappe
  {
  public:
     CommandeTrappe(uint8_t In1Pin, uint8_t In2Pin, uint8_t PwmPin, uint8_t StbyPin, uint8_t EncPin, uint8_t FdcPin, uint8_t SensDefaut = 1);
     void InitPos(long Position);  // Fonction permettant de renseigner la position de la trappe
     void Init();                  // Fonction d'initialisation de la position de la trappe
     void Ouvrir();                // Fonction d'ouverture de la trappe en fonction de la présence ou non d'un fin de course
     void OuvrirPos();             // Fonction d'ouverture de la trappe en fonction de la position initiale
     void OuvrirFdc();             // Fonction d'ouverture de la trappe en utilisant le fin de copurse haut
     void Fermer();                // Fonction de fermeture de la trappe;
     void Monte(int Vitesse);      // permet de monter la trappe
     void Descend(int Vitesse);    // permet de descendre la trappe
     void Stop();                  // arrête la trappe et passe le circuit de commante en standby
     long Position();              // renvoi la positon de la trappe
     
     static volatile   uint8_t Etat;   
  
  private:
     uint8_t In1;         // Première sortie d'alimentation du moteur
     uint8_t In2;        // Deuxième sortie d'alimentation du moteur
     uint8_t Pwm;        // Sortie pour variation de la vitesse, peux varier de -255 a +255
     uint8_t Stby;       // Sortie de commande StandBy
     uint8_t Enc;        // Entrée de l'encodeur de position
     uint8_t Fdc;        // Fin de course haut
     uint8_t Sens;       // sens par défaut du moteur, peut prendre les valeurs -1 et 1. 1 par défaut
     static volatile long  Pos; // Position de la trappe renseignée par l'encodeur
     static inline void Comptage(); // permet de déterminer la position de la trappe par comptage du mombre d'impultions

  };
