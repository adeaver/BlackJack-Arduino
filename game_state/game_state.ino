#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 

// Stepper Motor with 1.8 degrees per step
Adafruit_StepperMotor *rotational = AFMS.getStepper(200, 1);
Adafruit_StepperMotor *leadscrew = AFMS.getStepper(200,2);

String gameState = "0";
int deal = 1;

int gear_ratio = 6;
float step_angle_ratio = 108/360;
float current_angle = 0;

void setup() {
  Serial.begin(9600);              //Starting serial communication
  AFMS.begin();
}

void loop() {
  
  if (Serial.available()) {
    String game_state = Serial.readString();
    
    if (game_state.startsWith("0")){

    }
    
    else if (game_state.startsWith("1")) {
      //deal
    }
    
    else if (game_state.startsWith("2")) {
      update_position();
    }
    
    else if (game_state.startsWith("3")) {
      deal_card();
    }
  
  Serial.println(game_state);   // send the data
  }
 
  
//  if (receivedState) {
//  //if player hits a button, then 
//  }
  

}


void sendCurrentState() { 
  //interpret current game state received from raspi
   
}


void update_position() {
  int steps = gear_ratio * 72;
  rotational->step(steps, FORWARD, DOUBLE);
  current_angle+=steps * step_angle_ratio;
}


void deal_card() {
  bool card_dealt = false;
  int loops = 0;
  while (!card_dealt) {
    leadscrew->step(FORWARD, DOUBLE);
    loops++;
    if (loops>10) {
      card_dealt = true;
    }
  }
}


void button_press() {
  //read in button press from system
}

