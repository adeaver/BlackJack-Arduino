#include <Wire.h>
//#include <SPI.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// Stepper Motor with 1.8 degrees per step
Adafruit_StepperMotor *rotational = AFMS.getStepper(200, 2);
Adafruit_StepperMotor *cardSpitter = AFMS.getStepper(200, 1);

uint8_t cardsDealt = 0;
int faceStepsTaken = 0;
int atSteps = 0;
uint8_t playerCount = 0;
uint8_t currentPlayer = 0;
int players[50];

uint8_t reflectancePin = A3;     // reflectance sensor is connected to pin 3
int reflectanceVal = 1001;           // variable to store the value read

const int hitButtonPin = A4;      //input pin for "Hit" button
const uint8_t passButtonPin = A2;     //input pin for "Pass" button
const uint8_t rotSwitch = 5;
const uint8_t startButton = 6;
//const uint8_t E_STOP = 4;

int hitPressed = LOW;
int passPressed = LOW;
int startPressed = LOW;

const int fullRotation = 600;
const int cardDealingSteps = 25;

boolean started = true;
boolean faceScanning = false;
boolean playingGame = true;

unsigned long lastDebounce = millis();
unsigned long cardDealDebounce;
unsigned long debounceDelay = 2000;
unsigned long cardDealDelay = 500;

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(100);
  
  pinMode(startButton, INPUT);
  pinMode(hitButtonPin, INPUT);
  
  AFMS.begin();
  
  rotational->setSpeed(20);
  cardSpitter->setSpeed(10);
}

void loop() {
  //Serial.println("In Loop");
  if(playingGame) {
    if(!faceScanning) {
      if(!started) {
        started = true;
        zero();
        goToNextPlayer();
      }
      
      getUserInput();
    } 
    if(faceScanning) {
      if(Serial.available()) {
        lastDebounce = millis();
        int val = flushSerial();
        scanForFaces(val);
      }
      
      if(millis() - lastDebounce >= debounceDelay) {
        lastDebounce = millis();
        sendState(); 
      }
    }
  } else {
    playingGame = getStartStop();
    
    if(playingGame) {
      lastDebounce = millis(); 
    }
  }
}

void dispenseCard() {
  Serial.println("Dispensing");
  //cardsDealt++;
  reflectanceVal = analogRead(reflectancePin);
  //cardDealDebounce = millis();
  //delay(20);
  
  //Serial.println(reflectanceVal);
  while (reflectanceVal > 1000) {
    reflectanceVal = analogRead(reflectancePin);    // read the input pin
    delay(20);
    //Serial.println(reflectanceVal);
    //Serial.println("In Loop");
    cardSpitter->step(1, FORWARD);
  }
  Serial.println("Done");
  cardSpitter->release();
  reflectanceVal = 1001;
  delay(200);
}

void rotate(int steps) {
  rotational->step(steps, FORWARD, DOUBLE);
  rotational->release();
}

void goToNextPlayer() {
  Serial.println("Rotating");
  cardSpitter->step(50, BACKWARD);

  int moveSteps = players[currentPlayer] - atSteps;
  
  if(moveSteps < 0) {
    zero();
    moveSteps = players[currentPlayer]; 
  }
  
  atSteps += moveSteps;
  atSteps = atSteps % fullRotation;
  
 // Serial.println(moveSteps);
  
  //rotate(moveSteps);
  rotate(240);
  
  currentPlayer = (currentPlayer+1) % playerCount;
  
  cardSpitter->step(35, FORWARD);

//  currentPlayer++;
  cardSpitter->release();
}

void getUserInput() {
  hitPressed = getHit();
  passPressed = getPass();
  passPressed = 1000;
  //startPressed = getStartStop();
  
//  delay(500);
//  hitPressed = 100;

  boolean wait = true;
  boolean hit = false;
  boolean pass = false;

  while(hitPressed > 1000 && passPressed > 1000) {
    hitPressed = getHit();
    passPressed = getPass();
    //startPressed = getStartStop();
    
    if(hitPressed <= 1000) {
      hit = true;  
    }
    
    if(passPressed <= 1000) {
      pass = true;  
    }
    
    Serial.println(hitPressed);
    delay(150);
  }
  
  //Serial.println(hit);
  
  if(hit) {
    dispenseCard();
  }
  
  if(pass) {
    goToNextPlayer();
  }
  
//  if(startPressed) {
//    endGame();
//  }
  
  hitPressed = 1100;
  passPressed = 1100;
}

void endGame() {
  cardsDealt = 0;
  faceStepsTaken = 0;
  playerCount = 0;
  currentPlayer = 0;
  started = false;
  faceScanning = true;
  playingGame = false;
  delay(100);
}

void scanForFaces(int state) {
  lastDebounce = millis();
  
  if(state != 8) {
    rotate(cardDealingSteps);
    faceStepsTaken += cardDealingSteps;
  } else {
     addPlayer();
  }
  
  if(faceStepsTaken >= fullRotation) {
    faceScanning = false; 
  }
  
  rotational->release();
  
  sendState();
}

void sendState() {
  //Serial.println("Sending state");
  if(faceStepsTaken < fullRotation) {
    Serial.write("7"); 
  } else {
    Serial.write("9");
  } 
}

void addPlayer() {
 players[playerCount] = faceStepsTaken;
 playerCount = playerCount + 1;
 dispenseCard();
 dispenseCard();
 cardSpitter->step(35, BACKWARD);
 cardSpitter->release();
 rotate(cardDealingSteps);
 faceStepsTaken += cardDealingSteps;
}

bool getRotLimit() {
  return !digitalRead(rotSwitch);
}

int getHit() {
  return analogRead(hitButtonPin);
}

int getPass() {
  return analogRead(passButtonPin);
}

bool getStartStop() {
  return digitalRead(startButton); 
}

int flushSerial() {
  int val = 0;
  int rVal = 0;
  while(Serial.available()) {
    rVal = Serial.read();
    if(val == 0 && rVal > 0) {
      val = rVal; 
    }
  }
  
  return val-48;
}

void zero() {
  rotational->step(fullRotation, BACKWARD);
  rotational->release();
// bool limit = getRotLimit();
// Serial.println("Zeroing");
// while(!limit) {
//  rotational->step(1, BACKWARD);
//  rotational->release();
//  limit = getRotLimit();
// }
}
 void allSystemsTest() {

   while(!getHit()){
  
   }
   while(!getPass()){
    
   }
   while(!getStartStop()){
    
   }
   dispenseCard();
 }
