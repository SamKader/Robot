// Robot éviteur d'obstacles pour Arduino Uno
// Compatible avec shield moteur L298N ou similaire

#include <Ultrasonic.h>
#include <Servo.h>
// Définition des pins moteurs
#define vitesse_gauche 6    // Pin PWM pour vitesse moteur gauche
#define avant_gauche 8      // Pin direction avant moteur gauche
#define arriere_gauche 12   // Pin direction arrière moteur gauche (corrigé)
#define vitesse_droite 11   // Pin PWM pour vitesse moteur droite
#define avant_droite 2      // Pin direction avant moteur droite
#define arriere_droite 7    // Pin direction arrière moteur droite (corrigé)

// Définition des pins capteur ultrason
#define pin_trig 4          // Pin trigger du capteur HC-SR04
#define pin_echo 3          // Pin echo du capteur HC-SR04

// Définition du pin servo moteur
#define pin_servo 9         // Pin PWM pour le servo moteur

Servo moteur;
// Initialisation du capteur ultrason
Ultrasonic ultrasonic(pin_trig, pin_echo);

// Variables globales
int vitesse = 200;          // Vitesse des moteurs (0-255 pour Arduino Uno)
int vitesse_gauche_val = 200;  // Vitesse spécifique moteur gauche
int vitesse_droite_val = 220;  // Vitesse compensée moteur droit (+20 pour compenser)
int vitesse_rotation = 255;    // Vitesse maximale pour les rotations

void setup() {
  // Configuration des pins moteurs en sortie
  pinMode(vitesse_gauche, OUTPUT);
  pinMode(avant_gauche, OUTPUT);
  pinMode(avant_droite, OUTPUT);
  pinMode(arriere_gauche, OUTPUT);
  pinMode(arriere_droite, OUTPUT);
  pinMode(vitesse_droite, OUTPUT);

  moteur.attach(pin_servo);
  moteur.write(85); // Position centrale (regarder devant)

  // Initialisation de la communication série
  Serial.begin(9600);
  Serial.println("Robot éviteur d'obstacles démarré !");
  
  // Petit délai de démarrage
  delay(1000);
}

void avancer() {
  Serial.println("Le robot avance");
  
  // Moteur gauche en avant
  digitalWrite(avant_gauche, HIGH);
  digitalWrite(arriere_gauche, LOW);
  analogWrite(vitesse_gauche, vitesse_gauche_val);

  // Moteur droite en avant (vitesse compensée)
  digitalWrite(avant_droite, HIGH);
  digitalWrite(arriere_droite, LOW);
  analogWrite(vitesse_droite, vitesse_droite_val);
}

void reculer() {
  Serial.println("Le robot recule");
  
  // Moteur gauche en arrière
  digitalWrite(avant_gauche, LOW);
  digitalWrite(arriere_gauche, HIGH);
  analogWrite(vitesse_gauche, vitesse_gauche_val);

  // Moteur droite en arrière (vitesse compensée)
  digitalWrite(avant_droite, LOW);
  digitalWrite(arriere_droite, HIGH);
  analogWrite(vitesse_droite, vitesse_droite_val);
}

void arreter() {
  Serial.println("Le robot s'arrête");
  
  // Arrêt des moteurs
  analogWrite(vitesse_gauche, 0);
  analogWrite(vitesse_droite, 0);

  // Mise à LOW de tous les pins de direction
  digitalWrite(avant_gauche, LOW);
  digitalWrite(arriere_gauche, LOW);
  digitalWrite(avant_droite, LOW);
  digitalWrite(arriere_droite, LOW);
}

void tourner_a_droite() {
  Serial.println("Le robot tourne à droite");
  
  // Arrêt complet avant rotation
  arreter();
  delay(100);
  
  // Moteur gauche en avant (vitesse maximale)
  digitalWrite(avant_gauche, HIGH);
  digitalWrite(arriere_gauche, LOW);
  analogWrite(vitesse_gauche, vitesse_rotation);

  // Moteur droite en arrière (vitesse maximale)
  digitalWrite(avant_droite, LOW);
  digitalWrite(arriere_droite, HIGH);
  analogWrite(vitesse_droite, vitesse_rotation);
}

void tourner_a_gauche() {
  Serial.println("Le robot tourne à gauche");
  
  // Arrêt complet avant rotation
  arreter();
  delay(100);
  
  // Moteur gauche en arrière (vitesse maximale)
  digitalWrite(avant_gauche, LOW);
  digitalWrite(arriere_gauche, HIGH);
  analogWrite(vitesse_gauche, vitesse_rotation);

  // Moteur droite en avant (vitesse maximale)
  digitalWrite(avant_droite, HIGH);
  digitalWrite(arriere_droite, LOW);
  analogWrite(vitesse_droite, vitesse_rotation);
}

// ----------- FONCTIONS SERVO -----------
long regarder_a_droite() {
  moteur.write(30); // Tourner le servo à droite (30° au lieu de 0°)
  delay(600); // Délai pour stabilisation
  long distance = ultrasonic.read();
  moteur.write(90); // Ramener le servo au centre (90° au lieu de 85°)
  delay(400);
  return distance;
}

long regarder_a_gauche() {
  moteur.write(150); // Tourner le servo à gauche
  delay(600); // Délai pour stabilisation
  long distance =ultrasonic.read() ;
  moteur.write(90); // Ramener le servo au centre (90° au lieu de 85°)
  delay(400);
  return distance;
}

void loop() {
  // Mesure de la distance avec la librairie Ultrasonic
  long distance = ultrasonic.read();
  
  // Affichage de la distance mesurée
  Serial.print("Distance mesurée : ");
  Serial.print(distance);
  Serial.println(" cm");

  // Logique d'évitement d'obstacles
  if (distance > 20) {
    // Chemin libre - avancer
    avancer();
  } 
  else {
    // Obstacle détecté
    Serial.println("Obstacle détecté !");
    
    // Arrêt
    arreter();
    delay(500);

    // Reculer un peu
    reculer();
    delay(800);
    
    // Arrêt
    arreter();
    delay(300);

    // Tourner dans une direction aléatoire
    long distance_droite=regarder_a_droite();
    long distance_gauche= regarder_a_gauche();
    if (distance_droite >20 && distance_gauche>20) {
      if (distance_droite>distance_gauche){
        tourner_a_droite();
        delay(700);
      }
      else{
        tourner_a_gauche();
        delay(700);
      }
    } else {
      // faire demi tour
      if (distance_droite>distance_gauche){
        // faire demi tour a droite
        tourner_a_droite();
        delay(1300);
      }
      else{
        // faire demi tour a gauche
        tourner_a_gauche();
        delay(1300);
      }

    }
  }
  
  // Petit délai pour stabiliser les lectures
  delay(100);
}