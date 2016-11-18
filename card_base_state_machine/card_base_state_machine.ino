#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// Stepper Motor with 1.8 degrees per step
Adafruit_StepperMotor *rotational = AFMS.getStepper(200, 1);
Adafruit_StepperMotor *cardSpitter = AFMS.getStepper(200, 2);

String token = "";

int cardsDealt = 0;
int faceStepsTaken = 0;
int playerCount = 0;
int players[20];

const int stopSteps = 1200;
const int cardDealingSteps = 30;

void setup() {
  Serial.begin(9600);
  
  AFMS.begin();
  
  rotational->setSpeed(20);
  cardSpitter->setSpeed(15);
}

void loop() {
  if(Serial.available()) {
    token = Serial.readString();
    
    if(token.startsWith("1")) {
        dispenseCard();
    }
    
    if(token.startsWith("2")) {
       rotate();
    }
    
    if(token.startsWith("3")) {
       getUserInput();
    }
    
    if(token.startsWith("5")) {
      hit();
    }
    
    if(token.startsWith("7")) {
      rotateForFaceDetection();
    }
    
    if(token.startsWith("8")) {
      addPlayer();  
    }
    
    if(token.startsWith("e")) {
     reset();
    }
  
  }
}

void dispenseCard() {
  cardsDealt++;
  cardSpitter->step(cardDealingSteps, FORWARD);
  cardSpitter->release();
  Serial.write("1111"); 
}

void rotate() {
  cardSpitter->step(200, BACKWARD);
  rotational->step(300, FORWARD, DOUBLE);
  cardSpitter->step(200, FORWARD);
  
  cardSpitter->release();
  rotational->release();
  
  Serial.write("2222");
}

void getUserInput() {
  // Could also send "4444" for negative input
  Serial.write("3333");  
}

void hit() {
  Serial.write("5555");
}

void reset() {
  cardSpitter->step(cardDealingSteps*cardsDealt, BACKWARD);
  cardSpitter->release();
  cardsDealt = 0;
}

void addPlayer() {
 players[playerCount] = faceStepsTaken;
 playerCount++;
 //rotational->step(25, FORWARD);
 faceStepsTaken += 25;
 if(faceStepsTaken >= stopSteps) {
  Serial.write("9999"); 
 } else {
  Serial.write("8888"); 
 }
}

void rotateForFaceDetection() {
 //rotational->step(10, FORWARD);
 faceStepsTaken += 10;
 
 if(faceStepsTaken >= stopSteps) {
  Serial.write("9999"); 
 } else {
  Serial.write("7777"); 
 }
}
