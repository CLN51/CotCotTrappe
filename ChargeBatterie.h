//-----------------------------------------//
//                                         //
//             TrappePoule                 //  
//          ChargeBatterie.h               //
//                                         //
//-----------------------------------------//
struct Capacite {
  uint8_t Tension;
  uint8_t Pourcent;
};

uint8_t ChargeBatterie();    // Calcul de la capacitée restante de la batterie
