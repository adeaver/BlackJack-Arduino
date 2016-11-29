#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

#define SLAVE_ADDRESS 0x14

Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// Stepper Motor with 1.8 degrees per step
Adafruit_StepperMotor *rotational = AFMS.getStepper(200, 1);
Adafruit_StepperMotor *cardSpitter = AFMS.getStepper(200, 2);

int sendState = 0;

int cardsDealt = 0;
int faceStepsTaken = 0;
int playerCount = 0;
int players[20];

const int stopSteps = 1200;
const int cardDealingSteps = 30;

void setup() {    
  Serial.begin(9600);
  
  AFMS.begin();
  
  // initialize i2c as slave
  Wire.begin(SLAVE_ADDRESS);

  // define callbacks for i2c communication
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);
  
  rotational->setSpeed(20);
  cardSpitter->setSpeed(15);
}

void loop() {
  delay(100);
}

void receiveData(int byteCount) {
  char state = 0;
  
  Serial.println("Reached");
  delay(100);

  state = char(Wire.read());
 
  switch(state) {
    case '1':
      dispenseCard();
      sendState = 1;
      break;
    case '2':
      rotate();
      sendState = 2;
      break;
    case '3':
      getUserInput();
      break;
    case '5':
      hit();
      break;
    case '7':
      rotateForFaceDetection();
      break;
    case '8':
      addPlayer();
      break;
    case 'e':
      reset();
      break;
    } 
}

void sendData() {
  Serial.println(sendState);
  Wire.write(sendState); 
}

void dispenseCard() {
  cardsDealt++;
  cardSpitter->step(cardDealingSteps, FORWARD);
  cardSpitter->release();
}

void rotate() {
  cardSpitter->step(200, BACKWARD);
  rotational->step(300, FORWARD, DOUBLE);
  cardSpitter->step(200, FORWARD);
  
  cardSpitter->release();
  rotational->release();
}

void getUserInput() {
  // Could also send "4444" for negative input
  sendState = 3;  
}

void hit() {
  sendState = 5;
}

void reset() {
  cardSpitter->step(cardDealingSteps*cardsDealt, BACKWARD);
  cardSpitter->release();
  cardsDealt = 0;
}

void addPlayer() {
 players[playerCount] = faceStepsTaken;
 playerCount++;
 rotational->step(25, FORWARD);
 faceStepsTaken += 25;
 if(faceStepsTaken >= stopSteps) {
  sendState = 9; 
 } else {
  sendState = 8;
 }
}

void rotateForFaceDetection() {
 rotational->step(10, FORWARD);
 faceStepsTaken += 10;
 
 if(faceStepsTaken >= stopSteps) {
  sendState = 9; 
 } else {
  sendState = 7;
 }
}
