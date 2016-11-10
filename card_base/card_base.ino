#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 

// Stepper Motor with 1.8 degrees per step
Adafruit_StepperMotor *rotational = AFMS.getStepper(200, 1);

float step_angle = 1.8;
int loops = 0;
int stepCount = 0;

float steps = ((num_teeth_small/step_angle)/num_teeth_big)*72;

bool end = false;


void setup() {
  Serial.begin(9600); 
  AFMS.begin();
  
  rotational->setSpeed(10);
}

void loop() {
  rotateMotor72degrees((!end));
  delay(500);
  loops++;
  if (loops > 4){
    end = true;
  }
  if(end){
    untangle();
    rotational->setSpeed(0);

  }
}

void rotateMotor72degrees(bool go) {
  if(go){
    rotational->step(steps, FORWARD,DOUBLE);
    stepCount += steps;
  }
}

void untangle(){
  rotational->step(stepCount, BACKWARD, DOUBLE);
  
}
