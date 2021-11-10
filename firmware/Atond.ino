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

void setup()                                                // FUNKCE SETUP, KTERÁ SE SPUSTÍ PŘI PRVOTNÍM ZAPNUTÍ ROBOTA
{
  DEBUG = false;                                            // NASTAVENÍ DEBUG MODU
  MAIN_BUTTON_STATE = 0;                                    // NASTAVENÍ ZÁKLADNÍHO STAVU TLAČÍTKA (0)
  ACTIVE = false;                                           // NASTAVENÍ STAVU ROBOTA
  RYCHLOST = 0;                                             // NASTAVENÍ ZÁKLADNÍ RYCHLOSTI

  if (DEBUG)                                                // POKUD JE AKTIVNÍ DEBUG MODE
  {
    Serial.begin(9600);                                     // ZAPOČETÍ SÉRIOVÉ KOMUNIKACE
  }

  pinMode(US_T_PIN, OUTPUT);                                // NASTAVENÍ PINOVÉHO REŽIMU PRO TRIGGER ULTRAZVUKOVÉHO SENZORU NA VSTUP
  pinMode(US_E_PIN, INPUT);                                 // NASTAVENÍ PINOVÉHO REŽIMU PRO ECHO ULTRAZVUKOVÉHO SENZORU NA VÝSTUP
  pinMode(M_LEFT, OUTPUT);                                  // NASTAVENÍ PINOVÉHO REŽIMU PRO LEVÝ MOTOR
  pinMode(M_RIGHT, OUTPUT);                                 // NASTAVENÍ PINOVÉHO REŽIMU PRO PRAVÝ MOTOR
  pinMode(PWM_LEFT, OUTPUT);                                // NASTAVENÍ PINOVÉHO REŽIMU PRO PWM LEVÉHO MOTORU
  pinMode(PWM_RIGHT, OUTPUT);                               // NASTAVENÍ PINOVÉHO REŽIMU PRO PWM PRAVÉHO MOTORU
  pinMode(BL_RED, OUTPUT);                                  // NASTAVENÍ PINOVÉHO REŽIMU PRO ČERVENÝ KANÁL PODSVÍCENÍ
  pinMode(BL_GREEN, OUTPUT);                                // NASTAVENÍ PINOVÉHO REŽIMU PRO ZELENÝ KANÁL PODSVÍCENÍ
  pinMode(BL_BLUE, OUTPUT);                                 // NASTAVENÍ PINOVÉHO REŽIMU PRO MODRÝ KANÁL PODSVÍCENÍ

  analogWrite(BL_RED, 0);                                   // ZÁPIS ANALOGOVÉ HODNOTY PRO ČERVENÝ KANÁL PODSVÍCENÍ
  analogWrite(BL_GREEN, 0);                                 // ZÁPIS ANALOGOVÉ HODNOTY PRO ZELENÝ KANÁL PODSVÍCENÍ
  analogWrite(BL_BLUE, 1024);                               // ZÁPIS ANALOGOVÉ HODNOTY PRO MODRÝ KANÁL PODSVÍCENÍ
}

void loop()                                                 // FUNKCE LOOP, KTERÁ SE OPAKUJE AŽ DO ODPOJENÍ ROBOTA OD ZDROJE NAPĚTÍ
{ 
  MAIN_BUTTON_STATE = !(analogRead(MAIN_BUTTON) >= 512);    // AKTUALIZACE STAVU TLAČÍTKA

  if (MAIN_BUTTON_STATE)                                    // KONTROLA STAVU TLAČÍTKA (POKUD JE ZTISKNUTÉ)
  {
    if (ACTIVE)                                             // KONTROLA STAVU ROBOTA (POKUD JE ROBOT AKTIVNÍ)
    {
      analogWrite(BL_RED, 0);                               // NASTAVENÍ PINOVÉHO REŽIMU PRO ČERVENÝ KANÁL PODSVÍCENÍ
      analogWrite(BL_GREEN, 0);                             // NASTAVENÍ PINOVÉHO REŽIMU PRO ZELENÝ KANÁL PODSVÍCENÍ
      analogWrite(BL_BLUE, 1024);                           // NASTAVENÍ PINOVÉHO REŽIMU PRO MODRÝ KANÁL PODSVÍCENÍ
      ACTIVE = false;                                       // NASTAVENÍ STAVU ROBOTA DO NEAKTIVNÍHO
      RYCHLOST = 0;                                         // NASTAVENÍ RYCHLOSTI NA 0 (STŮJ)
    }
    else                                                    // POKUD JE ROBOT NEAKTIVNÍ
    {
      analogWrite(BL_RED, 0);                               // NASTAVENÍ PINOVÉHO REŽIMU PRO ČERVENÝ KANÁL PODSVÍCENÍ
      analogWrite(BL_GREEN, 1024);                          // NASTAVENÍ PINOVÉHO REŽIMU PRO ZELENÝ KANÁL PODSVÍCENÍ
      analogWrite(BL_BLUE, 0);                              // NASTAVENÍ PINOVÉHO REŽIMU PRO MODRÝ KANÁL PODSVÍCENÍ
      delay(1000);                                          // POČKEJ 1 SEKUNDU
      ACTIVE = true;                                        // NASTAVENÍ STAVU ROBOTA NA AKTIVNÍ
      RYCHLOST = 75;                                        // NASTAVENÍ RYCHLOSTI NA 75
    }
    delay(500);                                             // POČKEJ PŮL SEKUNDY
  }

  if (ACTIVE)                                               // POKUD JE ROBOT AKTIVNÍ (MOD JÍZDY)
  {
    IR_L = !digitalRead(IR_L_PIN);                          // KONTROLA STAVU LEVÉHO IR SENZORU
    IR_R = !digitalRead(IR_R_PIN);                          // KONTROLA STAVU PRAVÉHO IR SENZORU

    if (US_DIST <= 10)                                      // POKUD JE NALEZENA PŘEKÁŽKA V ROZMEZÍ 10 CM
    {
      if (US_DIST < 4)                                      // POKUD JE NALEZENA PŘEKÁŽKA BLÍŽE JAK 4 CM
      {
        RYCHLOST = 75;                                      // NASTAVENÍ RYCHLOSTI NA 75
        ATTACKED = true;                                    // NASTAVENÍ REŽIMU NA "ATTACKED"
        analogWrite(PWM_LEFT, RYCHLOST);                    // ZÁPIS PWM RYCHLOSTI PRO LEVÝ MOTOR
        analogWrite(PWM_RIGHT, RYCHLOST);                   // ZÁPIS PWM RYCHLOSTI PRO PRAVÝ MOTOR
        digitalWrite(M_LEFT, LOW);                          // NASTAVENÍ SMĚRU LEVÉHO MOTORU (LOW = DOZADU)
        digitalWrite(M_RIGHT, LOW);                         // NASTAVENÍ SMĚRU PRAVÉHO MOTORU (LOW = DOZADU)
        
        analogWrite(BL_RED, 1024);                          // NASTAVENÍ PINOVÉHO REŽIMU PRO ČERVENÝ KANÁL PODSVÍCENÍ
        analogWrite(BL_GREEN, 1024);                        // NASTAVENÍ PINOVÉHO REŽIMU PRO ZELENÝ KANÁL PODSVÍCENÍ
        analogWrite(BL_BLUE, 0);                            // NASTAVENÍ PINOVÉHO REŽIMU PRO MODRÝ KANÁL PODSVÍCENÍ
      }
      else                                                  // POKUD JE PŘEKÁŽKA STATICKÁ
      {
      ATTACKED = false;                                     // VYPNUTÍ REŽIMU "ATTACKED"
      RYCHLOST = 0;                                         // NASTAVENÍ RYCHLOSTI NA 0
      analogWrite(BL_RED, 1024);                            // NASTAVENÍ PINOVÉHO REŽIMU PRO ČERVENÝ KANÁL PODSVÍCENÍ
      analogWrite(BL_GREEN, 0);                             // NASTAVENÍ PINOVÉHO REŽIMU PRO ZELENÝ KANÁL PODSVÍCENÍ
      analogWrite(BL_BLUE, 0);                              // NASTAVENÍ PINOVÉHO REŽIMU PRO MODRÝ KANÁL PODSVÍCENÍ
      }
    }
    else                                                    // POKUD JE CESTA VOLNÁ
    {
      ATTACKED = false;                                     // VYPNUTÍ REŽIMU "ATTACKED"
      RYCHLOST = 75;                                        // NASTAVENÍ RYCHLOSTI NA 75
      analogWrite(BL_RED, 0);                               // NASTAVENÍ PINOVÉHO REŽIMU PRO ČERVENÝ KANÁL PODSVÍCENÍ
      analogWrite(BL_GREEN, 1024);                          // NASTAVENÍ PINOVÉHO REŽIMU PRO ZELENÝ KANÁL PODSVÍCENÍ
      analogWrite(BL_BLUE, 0);                              // NASTAVENÍ PINOVÉHO REŽIMU PRO MODRÝ KANÁL PODSVÍCENÍ
    } 

    if (IR_L && IR_R)                                       // POKUD JE ROBOT NA TRASE
    {
      if (!ATTACKED)                                        // POKUD NENÍ V REŽIMU "ATTACKED" 
      {
        analogWrite(PWM_LEFT, RYCHLOST);                    // ZÁPIS PWM RYCHLOSTI PRO LEVÝ MOTOR
        analogWrite(PWM_RIGHT, RYCHLOST);                   // ZÁPIS PWM RYCHLOSTI PRO PRAVÝ MOTOR
        digitalWrite(M_LEFT, HIGH);                         // NASTAVENÍ SMĚRU LEVÉHO MOTORU (HIGH = DOPŘEDU)
        digitalWrite(M_RIGHT, HIGH);                        // NASTAVENÍ SMĚRU PRAVÝ MOTORU (HIGH = DOPŘEDU)
        STATUS_CURRENT = 1;                                 // NASTAVENÍ SMĚRU DO MEZIPAMĚTI
      }
    }
    else if (!IR_L && IR_R)                                 // POKUD ROBOT VYJEDE Z TRASY (DO LEVA)
    {
      analogWrite(PWM_LEFT, 0);                             // ZÁPIS PWM RYCHLOSTI PRO LEVÝ MOTOR
      analogWrite(PWM_RIGHT, RYCHLOST);                     // ZÁPIS PWM RYCHLOSTI PRO PRAVÝ MOTOR
      digitalWrite(M_LEFT, LOW);                            // NASTAVENÍ SMĚRU LEVÉHO MOTORU
      digitalWrite(M_RIGHT, HIGH);                          // NASTAVENÍ SMĚRU PRAVÝ MOTORU
      STATUS_CURRENT = 2;                                   // NASTAVENÍ SMĚRU DO MEZIPAMĚTI
    }
    else if (IR_L && !IR_R)                                 // POKUD ROBOT VYJEDE Z TRASY (DO PRAVA)
    {
      analogWrite(PWM_LEFT, RYCHLOST);                      // ZÁPIS PWM RYCHLOSTI PRO LEVÝ MOTOR
      analogWrite(PWM_RIGHT, 0);                            // ZÁPIS PWM RYCHLOSTI PRO PRAVÝ MOTOR
      digitalWrite(M_LEFT, HIGH);                           // NASTAVENÍ SMĚRU LEVÉHO MOTORU
      digitalWrite(M_RIGHT, LOW);                           // NASTAVENÍ SMĚRU PRAVÝ MOTORU
    }
    else if (!IR_L && !IR_R)                                // ROBOT PŘEJTE DO REŽIMU "ZTRACEN"
    {
      if (STATUS_LAST == 1)                                 // POKUD JE V MEZIPAMĚTI HODNOTA 1 
      {
        analogWrite(PWM_LEFT, RYCHLOST);                    // ZÁPIS PWM RYCHLOSTI PRO LEVÝ MOTOR
        analogWrite(PWM_RIGHT, 0);                          // ZÁPIS PWM RYCHLOSTI PRO PRAVÝ MOTOR
        digitalWrite(M_LEFT, HIGH);                         // NASTAVENÍ SMĚRU LEVÉHO MOTORU
        digitalWrite(M_RIGHT, LOW);                         // NASTAVENÍ SMĚRU PRAVÝ MOTORU
      }
      else if (STATUS_LAST == 2)                            // POKUD JE V MEZIPAMĚTI HODNOTA 2
      {
        analogWrite(PWM_LEFT, 0);                           // ZÁPIS PWM RYCHLOSTI PRO LEVÝ MOTOR
        analogWrite(PWM_RIGHT, RYCHLOST);                   // ZÁPIS PWM RYCHLOSTI PRO PRAVÝ MOTOR
        digitalWrite(M_LEFT, LOW);                          // NASTAVENÍ SMĚRU LEVÉHO MOTORU
        digitalWrite(M_RIGHT, HIGH);                        // NASTAVENÍ SMĚRU PRAVÝ MOTORU
      } 
      else                                                  // POKUD JE MEZIPAMĚŤ PRÁZDNÁ
      {
        analogWrite(PWM_LEFT, 0);                           // ZÁPIS PWM RYCHLOSTI PRO LEVÝ MOTOR
        analogWrite(PWM_RIGHT, 0);                          // ZÁPIS PWM RYCHLOSTI PRO PRAVÝ MOTOR
        digitalWrite(M_LEFT, LOW);                          // NASTAVENÍ SMĚRU LEVÉHO MOTORU
        digitalWrite(M_RIGHT, LOW);                         // NASTAVENÍ SMĚRU PRAVÝ MOTORU
        
        analogWrite(BL_RED, 1024);                          // NASTAVENÍ PINOVÉHO REŽIMU PRO ČERVENÝ KANÁL PODSVÍCENÍ
        analogWrite(BL_GREEN, 1024);                        // NASTAVENÍ PINOVÉHO REŽIMU PRO ZELENÝ KANÁL PODSVÍCENÍ
        analogWrite(BL_BLUE, 0);                            // NASTAVENÍ PINOVÉHO REŽIMU PRO MODRÝ KANÁL PODSVÍCENÍ
      }
    }

    if (STATUS_LAST != STATUS_CURRENT)                      // POKUD JE AKTUÁLNÍ HODNOTA JINÁ NEŽ HODNOTA V MEZIPAMĚTI
    {
      STATUS_LAST = STATUS_CURRENT;                         // PŘEPISOVÁNÍ MEZIPAMĚTI NA NOVOU HODNOTU
    }
    
    digitalWrite(US_T_PIN, LOW);                            // VYPNUTÍ SIGNÁLU V TRIGGER PINU
    delayMicroseconds(2);                                   // POČKAT 2 MIKROSEKUNDY
    digitalWrite(US_T_PIN, HIGH);                           // ODESLÁNÍ SIGNÁLU DO TRIGGER PINU
    delayMicroseconds(10);                                  // POČKAT 10 MIKROSEKUND
    digitalWrite(US_T_PIN, LOW);                            // VYPNUTÍ SIGNÁLU V TRIGGER PINU
    US_DUR = pulseIn(US_E_PIN, HIGH);                       // POČKAT NA ODRAZ ULTRAZVUKU
    US_DIST = US_DUR * 0.034 / 2;                           // VÝPOČET VZDÁLENOSTI
  }
  else                                                      // POKUD JE ROBOT NEAKTIVNÍ
  {
    analogWrite(PWM_LEFT, 0);                               // ZÁPIS PWM RYCHLOSTI PRO LEVÝ MOTOR
    analogWrite(PWM_RIGHT, 0);                              // ZÁPIS PWM RYCHLOSTI PRO PRAVÝ MOTOR
    digitalWrite(M_LEFT, LOW);                              // NASTAVENÍ SMĚRU LEVÉHO MOTORU
    digitalWrite(M_RIGHT, LOW);                             // NASTAVENÍ SMĚRU PRAVÝ MOTORU
    STATUS_CURRENT = 0;                                     // NASTAVENÍ MEZIPAMĚTI NA 0
  }
}
