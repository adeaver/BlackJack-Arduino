#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// Stepper Motor with 1.8 degrees per step
Adafruit_StepperMotor *rotational = AFMS.getStepper(200, 1);
Adafruit_StepperMotor *cardSpitter = AFMS.getStepper(200, 2);

String token = "";

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
  
  }
}

void dispenseCard() {
  cardSpitter->step(200, FORWARD);
  Serial.write("1111"); 
}

void rotate() {
  rotational->step(300, FORWARD);
  Serial.write("2222");
}

void getUserInput() {
  // Could also send "4444" for negative input
  Serial.write("3333");  
}

void hit() {
  Serial.write("5555");
}
