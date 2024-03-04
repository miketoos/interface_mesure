#include <Arduino.h>
#include <Wire.h> // Bibliothèque I2C
#include <math.h>

// Paramètres de sortie
#define Gain_Out1 1  // -> Régime Moteur : 1V => 1000 tr/min
#define Gain_Out2 68 // -> Avance Allumage : 68 mV => 1° (pour aller de 0 à 60°)
#define Gain_Out3 1  // Gain variable de la carte -> 1 paramètre de base 1V pour 1ms
                     //                           -> 0.5 :   1ms => 0.5V
                     //                           -> 0.25 :  1ms => 0.25V
                     //                           -> 2 :     1ms => 2V
#define Gain_Out4 1  // -> Acceleration : 1V pour 1000(tour/min)−2

#define Address_dac 0x48 // Adresse du DAC
#define OUT1 0x30        // Adresse DAC sortie 1
#define OUT2 0x31        // Adresse DAC sortie 2
#define OUT3 0x32        // Adresse DAC sortie 3
#define OUT4 0x33        // Adresse DAC sortie 4

#define IN1 18 // Pin entrée 1
#define IN2 19 // Pin entrée 2
#define IN3 22 // Pin entrée 3
#define IN4 23 // Pin entrée 4

// Drapeau d'interruption
unsigned int IFLAG; // Drapeau indiquant le type d'interruption détectée

// Variables liées au signal PMH (Point Mort Haut)
unsigned int T1_PMH = 0; // Temps du front montant de PMH
unsigned int T2_PMH = 0; // Temps du front descendant de PMH
unsigned int DP_PMH = 0; // Durée de la dent précédente (previous)
unsigned int DC_PMH = 0; // Durée de la dent actuelle (current)
unsigned int FD_1;       // Temps du début de la période PMH
unsigned int FD_2;       // Temps fin de la période PMH
unsigned int N;          // Variable pour le régime moteur (à préciser si elle est utilisée)
unsigned int count = 1;  // Compteur cycle regimes

double PMH = 0;                 // Variable pour le calcul du régime moteur en tr/
unsigned int buffer_regime = 0; // Buffer pour stocker la valeur du régime moteur

// Variables liées à l'allumage
unsigned int T1_AA = 0; // Temps du front descendant de l'allumage
unsigned int TF = 0;    // Temps à préciser (non utilisé dans le code actuel)
double avance = 0;      // Temps d'avance entre le PMH et l'allumage en microsecondes
double rapport = 0;     // Rapport entre le temps d'avance et la période du PMH
double deg = 0;         // Conversion du rapport en degrés
double solus = 0.7;
double buffer_allumage = 0; // Buffer pour stocker la valeur de l'allumage

// Variables liées au temps de conduction
unsigned int T1_TC = 0;             // Temps début signal
unsigned int T2_TC = 0;             // Temps fin signal
unsigned int duree_condu;           // Duree du temps de conduction
unsigned int buffer_conduction = 0; // Buffer pour stocker la valeur du temps de conduction

// Variables liées à l'acceleration
unsigned int acc_count = 0;         // Compteur différences de régime effectué
unsigned int tmp_regime = 0;        // Variables temporaires pour calcul différence regime et temps ecoule
unsigned int tmp_temps = 0;
double tempsEcoule = 0;             // Temps ecoule entre la premier et dernier regime et la différence associé
int diff_regime;
double buffer_accel = 0;            // Buffer pour stocker la valeur de l'acceleration

void ES_Init() // Fonction initialisation PIN
{
  pinMode(IN1, INPUT); // Signal 1
  pinMode(IN2, INPUT); // Signal 2
  pinMode(IN3, INPUT); // Signal 3
  pinMode(IN4, INPUT); // Signal 4

  pinMode(15, OUTPUT);   // LED test fonctionnement SAMD21
  digitalWrite(15, LOW); // Initialisation Led éteinte
}

void DacInit() // Fonction initialisation DAC
{
  pinMode(10, OUTPUT);    // Clear en sortie du DAC
  pinMode(11, OUTPUT);    // Ldac en sortie du DAC
  digitalWrite(10, LOW);  // Clear à 0
  digitalWrite(11, LOW);  // Ldac à 0
  digitalWrite(10, HIGH); // Remise à 0 du DAC
}

void DacWrite(int data, unsigned int channel)
{
  int MSBD, LSBD; // Variable bit de start et stop

  MSBD = (data & 0xFF0) >> 4;           // 8 bits de poids fort
  LSBD = ((data & 0x00F) << 4) & 0x0FF; // 8 bits de poids faible

  Wire.beginTransmission(Address_dac); // Commence la transmission sur l'adresse du DAC
  Wire.write(channel);                 // Commence la transmission sur la sortie
  Wire.write(MSBD);                    // On envoie 8 bits de poids forts
  Wire.write(LSBD);                    // On envoie 8 bits de poids faible
  Wire.endTransmission();              // On termine la transmission
}

void dacWriteBuffer()
{
  DacWrite(buffer_regime, OUT1);     // Ecriture sur la premiere sortie du regime moteur
  DacWrite(buffer_allumage, OUT2);   // Ecriture sur la deuxieme sortie de la valeur de l'avance à l'allumage
  DacWrite(buffer_conduction, OUT3); // Ecriture sur la troisieme sortie de la valeur du temps de conduction
  DacWrite(buffer_accel, OUT4);      // Ecriture sur la quartieme sortie de la valeur de l'accélération
}

void setup() // Fonction Initialisation Microcontrôleur
{
  Wire.begin();           // Démarrage I2C
  Wire.setClock(3400000); // 180000 bauds
  ES_Init();              // Initialisation des ports d'entrée
  DacInit();              // Initialisation des ports d'activation du DAC
  Serial.begin(115200);   // Démarrage de la liaison série

  attachInterrupt(digitalPinToInterrupt(IN2), deuxieme_interrupt, RISING); // Procédure d'interruption entrée 2, Pin 19
  attachInterrupt(digitalPinToInterrupt(IN3), trois_interrupt, RISING);    // Procédure d'interruption entrée 3, Pin 22
}

// Fonction d'interruption pour l'entrée 1
void premier_interrupt()
{
  IFLAG = 1;
}

// Fonction d'interruption pour l'entrée 2
void deuxieme_interrupt()
{
  IFLAG = 2;
}

// Fonction d'interruption pour l'entrée 3
void trois_interrupt()
{
  IFLAG = 3;
}

void loop()
{
  if (IFLAG == 1)
  {

    DP_PMH = DC_PMH; // Sauvegarde de la durée de la dent précédente pour la comparaison

    T1_PMH = micros(); // Enregistrement du temps du front montant actuel

    DC_PMH = T1_PMH - T2_PMH; // Calcul de la durée de la dent actuelle

    T2_PMH = T1_PMH; // Mise à jour du temps du front montant précédent

    // Si la durée de la dent actuelle est plus de deux fois grande que la durée de la dent précédente : On est sur une fausse dent
    if (DC_PMH > DP_PMH * 2)
    {
      if (count == 1) // On calcule le regime moteur sur une periode, on vérifie donc si on en est au premier ou pas
      {
        FD_1 = T1_PMH; // On enregistre le temps de la dent actuel dans une autre variable

        if (acc_count == 2) // De la meme manière on calcule l'acceleration en faisaint la difference entre 2 regime moteur, donc un autre compteur est nécéssaire
        {
          diff_regime = buffer_regime - tmp_regime; // Calcul de la difference de regime

          tempsEcoule = FD_1 - tmp_temps;               // Calcul du temps écoulé (nécessaire pour un calcul d'accélération)
          tempsEcoule = tempsEcoule / (60 * 1000000.0); // Conversion en minutes

          tmp_temps = FD_1;           // Enregistrement du temps actuel pour le calcul suivant
          tmp_regime = buffer_regime; // Enregistrement du regime moteur actuel pour le calcul suivant
          acc_count = 0;              // Remise à 0 du compteur d'accélération
        }
        else
        {
          acc_count++;
        }
        count++;
      }
      else if (count == 2) // Deuxieme tour de boucle, on peut calculer tout les buffer
      {
        FD_2 = T1_PMH;

        // Calcul de la période du PMH et mise à jour du buffer de régime moteur
        PMH = (FD_2 - FD_1);
        buffer_regime = ((60000000 / PMH)) * Gain_Out1;
        buffer_regime = (buffer_regime > 4095) ? 4095 : (buffer_regime < 0) ? 0
                                                                            : buffer_regime;

        // Calcul de l'accélération et mise à jour du buffer d'accélération
        buffer_accel = (((static_cast<double>(diff_regime) / tempsEcoule) / 100) + 1000) * Gain_Out4;
        buffer_accel = (buffer_accel > 4095) ? 4095 : (buffer_accel < 0) ? 0
                                                                         : buffer_accel;

        FD_1 = T1_PMH;
        count = 1;

        detachInterrupt(IN1); // Arret de l'interruption, tout les calculs ont étés faits

        // Mise en route des autres interruptions
        attachInterrupt(digitalPinToInterrupt(IN2), deuxieme_interrupt, RISING); // Procédure d'interruption entrée 2, Pin 19
        attachInterrupt(digitalPinToInterrupt(IN3), trois_interrupt, RISING);    // Procédure d'interruption entrée 3, Pin 22
      }
    }
    IFLAG = 0;
  }
  // Section d'interruption pour le signal 2
  if (IFLAG == 2)
  {
    // Enregistrement du temps actuel
    T1_AA = micros();

    // Calcul de l'avance en microsecondes
    avance = T1_AA - FD_2;

    // Calcul du rapport entre la durée du PMH et l'avance
    rapport = PMH / (avance);

    // Calcul de l'angle d'allumage en degrés
    deg = (((rapport * 360) - 240) / 2) * solus; // == 70

    // Conversion de l'angle d'allumage en tension pour le DAC
    buffer_allumage = (deg)*Gain_Out2;
    buffer_allumage = (buffer_allumage > 4095) ? 4095 : (buffer_allumage < 0) ? 0
                                                                              : buffer_allumage;

    // Détachement de l'interruption pour le signal 2
    detachInterrupt(IN2);

    // Écriture des valeurs dans le DAC
    dacWriteBuffer();

    // Réattachement de l'interruption pour le signal 1 sur le front descendant
    attachInterrupt(digitalPinToInterrupt(IN1), premier_interrupt, FALLING); // Procédure d'interruption entrée 2, Pin 18

    // Réinitialisation du drapeau d'interruption
    IFLAG = 0;
  }

  // Section d'interruption pour le signal 3
  if (IFLAG == 3)
  {
    // Enregistrement du temps actuel
    T1_TC = micros();

    // Attente de la fin du signal pour éviter une mesure erronée
    while (digitalRead(IN3))
      ;

    // Enregistrement du temps de fin du signal
    T2_TC = micros();

    // Calcul de la durée de conduction en microsecondes
    duree_condu = (T2_TC - T1_TC) * Gain_Out3;

    // Conversion de la durée de conduction en tension pour le DAC
    buffer_conduction = (duree_condu > 4095) ? 4095 : (duree_condu < 0) ? 0
                                                                        : duree_condu;

    // Détachement de l'interruption pour le signal 2
    detachInterrupt(IN2);

    // Écriture des valeurs dans le DAC
    dacWriteBuffer();

    // Réattachement de l'interruption pour le signal 1 sur le front descendant
    attachInterrupt(digitalPinToInterrupt(IN1), premier_interrupt, FALLING);

    // Réinitialisation du drapeau d'interruption
    IFLAG = 0;
  }
}