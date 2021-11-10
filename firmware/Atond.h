#define IR_L_PIN 10                                         // NASTAVENÍ PINU PRO LEVÝ IR SENZOR
#define IR_R_PIN 9                                          // NASTAVENÍ PINU PRO PRAVÝ IR SENZOR
#define US_T_PIN 11                                         // NASTAVENÍ TRIGGER PINU PRO ULTRAZVUKOVÝ SENZOR
#define US_E_PIN 12                                         // NASTAVENÍ ECHO PINU PRO ULTRAZVUKOVÝ SENZOR
#define M_LEFT 7                                            // NASTAVENÍ PINU PRO LEVÝ MOTOR
#define M_RIGHT 4                                           // NASTAVENÍ PINU PRO PRAVÝ MOTOR
#define PWM_LEFT 6                                          // NASTAVENÍ PWM PINU PRO LEVÝ MOTOR
#define PWM_RIGHT 5                                         // NASTAVENÍ PWM PINU PRO PRAVÝ MOTOR
#define BL_RED A2                                           // NASTAVENÍ PINU PRO PODSVÍCENÍ (ČERVENÁ)
#define BL_GREEN A4                                         // NASTAVENÍ PINU PRO PODSVÍCENÍ (ZELENÁ)
#define BL_BLUE A3                                          // NASTAVENÍ PINU PRO PODSVÍCENÍ (MODRÁ)
#define MAIN_BUTTON A7                                      // NASTAVENÍ PINU PRO OVLADACÍ TLAČÍTKO

bool IR_L;                                                  // POMOCNÁ PROMĚNNÁ PRO STAV LEVÉHO IR SENZORU
bool IR_R;                                                  // POMOCNÁ PROMĚNNÁ PRO STAV PRAVÉHO IR SENZORU
long US_DUR;                                                // DOBA TRVÁNÍ CESTY ULTRAZVUKOVÉ VLNY
int US_DIST;                                                // VZDÁLENOST PŘEKÁŽKY OD SENZORU
int RYCHLOST;                                               // RYCHLOST ROBOTA
bool MAIN_BUTTON_STATE;                                     // POMOCNÁ PROMĚNNÁ PRO STAV TLAČÍTKA
bool ACTIVE;                                                // POMOCNÁ PROMĚNNÁ PRO STAV ROBOTA
int STATUS_CURRENT;                                         // POMOCNÁ PROMĚNNÁ REŽÍMU "ZTRACEN" (MOMENTÁLNÍ SMĚR)
int STATUS_LAST;                                            // POMOCNÁ PROMĚNNÁ REŽÍMU "ZTRACEN" (POSLEDNÍ SMĚR)
bool DEBUG;                                                 // DEBUG MOD
bool ATTACKED;                                              // REŽIM "POD ÚTOKEM"
