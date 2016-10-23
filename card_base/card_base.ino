#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 

// Stepper Motor with 1.8 degrees per step
Adafruit_StepperMotor *rotational = AFMS.getStepper(200, 1);

void setup() {
  Serial.begin(9600); 
  AFMS.begin();
  
  rotational->setSpeed(10);
}

void loop() {
  rotateMotor72degrees();
  delay(500);
}

void rotateMotor72degrees() {
  rotational->step(40, FORWARD, DOUBLE);
}
