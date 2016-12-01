#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 

// Stepper Motor with 1.8 degrees per step
Adafruit_StepperMotor *rotational = AFMS.getStepper(200, 1);
<<<<<<< HEAD
Adafruit_DCMotor *cardspitter = AFMS.getMotor(3);


float step_angle = 1.8;
float num_teeth_small = 18;
float num_teeth_big = 108;
=======

float step_angle = 1.8;
>>>>>>> 6d5b1778f0b6818c873cacb1858531bac8605da5
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
<<<<<<< HEAD
    Serial.println('hello');
=======
>>>>>>> 6d5b1778f0b6818c873cacb1858531bac8605da5
    rotational->step(steps, FORWARD,DOUBLE);
    stepCount += steps;
  }
}

void untangle(){
  rotational->step(stepCount, BACKWARD, DOUBLE);
  
}
