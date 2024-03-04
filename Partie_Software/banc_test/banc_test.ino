#include <Arduino.h>

#define PMH 12
#define ALLUMAGE 11
#define POT_A0 A0
#define POT_A1 A1

int potValue1;
int potValue2;
int period; // Période initiale du signal en microsecondes
int multi;
int base;
int toothCount = 0;      // Compteur de dents
int allumageTrigger = 0; // Compteur pour déclencher ALLUMAGE

void setup()
{
    pinMode(PMH, OUTPUT);
    pinMode(ALLUMAGE, OUTPUT);
    pinMode(POT_A0, INPUT);
    pinMode(POT_A1, INPUT);
}

void loop()
{
    // Lecture de la valeur du potentiomètre pour ajuster la période du signal
    potValue1 = analogRead(POT_A0);
    potValue2 = analogRead(POT_A1);
    // La valeur lue du potentiomètre varie de 0 à 1023, donc nous ajustons la période entre 1000 et 10000 microsecondes
    multi = map(potValue2, 0, 1023, 10, 100);
    base = map(potValue1, 0, 1023, 50, 16);
    period = base * multi;
    // Génération du signal PMH
    generatePMHSignal();

    // Génération du signal ALLUMAGE un peu après chaque période de PMH
    if (toothCount == 20 && allumageTrigger == 2)
    {
        digitalWrite(ALLUMAGE, HIGH);
        delayMicroseconds(100); // Réduire ce délai pour retarder le signal ALLUMAGE
        digitalWrite(ALLUMAGE, LOW);
        allumageTrigger = 0; // Réinitialise le compteur pour le prochain déclenchement
    }
}

void generatePMHSignal()
{
    if (toothCount < 58)
    { // Génère les 58 dents
        digitalWrite(PMH, LOW);
        delayMicroseconds(period / 2); // On maintient le signal HIGH pendant la moitié de la période
        digitalWrite(PMH, HIGH);
        delayMicroseconds(period / 2); // Puis le signal LOW pendant l'autre moitié de la période
        toothCount++;
    }
    else
    { // Génère les 2 dents manquantes
        digitalWrite(PMH, LOW);
        delayMicroseconds(period * 2); // On maintient le signal HIGH pendant la moitié de la période
        digitalWrite(PMH, HIGH);
        delayMicroseconds(period * 2);
        toothCount = 0;    // Réinitialise le compteur pour recommencer le cycle
        allumageTrigger++; // Marque le déclenchement pour le signal ALLUMAGE
    }
}