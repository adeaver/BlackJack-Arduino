#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 

// Stepper Motor with 1.8 degrees per step
Adafruit_StepperMotor *rotational = AFMS.getStepper(200, 1);
Adafruit_DCMotor *cardspitter = AFMS.getMotor(3);
int loops = 0;
int cardSpitter = 9;
int steps = 40;
int stepCount = 0;
bool end = false;
void setup() {
  Serial.begin(9600); 
  AFMS.begin();
  
  rotational->setSpeed(10);
  cardspitter->setSpeed(100);
}

void loop() {
  rotateMotor72degrees((!end));
  spitCard();
  delay(500);
  loops++;
  if (loops > 4){
    end = true;
  }
  if(end){
    untangle();
    rotational->setSpeed(0);
    cardspitter->setSpeed(0);
    stepCount = 0;
  }
  
}

void rotateMotor72degrees(bool go) {
  if(go){
  rotational->step(steps, FORWARD, DOUBLE);
  stepCount += steps;
  }
}
void untangle(){
  rotational->step(stepCount, BACKWARD, DOUBLE);
  
}
void spitCards(int n){
  for(int i = 0; i <n; i++){
    spitCard();
    delay(100);
  }
}

void spitCard() {

  cardspitter->run(FORWARD);
  delay(75);
  cardspitter->run(RELEASE);
 
}
