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

const uint8_t hitButtonPin = A4;      //input pin for "Hit" button
const uint8_t passButtonPin = A2;     //input pin for "Pass" button
const uint8_t rotSwitch = 8;
const uint8_t startButton = 6;
const uint8_t E_STOP = 5;

int hitPressed = LOW;
int passPressed = LOW;
int startPressed = LOW;

const int fullRotation = 1200;
const int cardDealingSteps = 30;

boolean started = false;
boolean faceScanning = true;
boolean playingGame = false;

unsigned long lastDebounce = millis();
unsigned long debounceDelay = 700;

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(100);
  
  pinMode(startButton, INPUT);
  pinMode(hitButtonPin, INPUT);
  
  AFMS.begin();
  
  rotational->setSpeed(20);
  cardSpitter->setSpeed(20);
//  allSystemsTest();
//  zero();
//  reset();
}

void loop() {
  if(playingGame) {
    if(!faceScanning) {
      //Serial.println("IS NOT SCANNING");
      if(!started) {
        started = true; 
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
    
    //Serial.println("IS NOT PLAYING");
    
    if(playingGame) {
      lastDebounce = millis(); 
    }
  }
}

void dispenseCard() {
  cardsDealt++;
  reflectanceVal = analogRead(reflectancePin);
  Serial.println(reflectanceVal);
  while (reflectanceVal>1000) {
    reflectanceVal = analogRead(reflectancePin);    // read the input pin
    delay(5);
    Serial.println(reflectanceVal);
    cardSpitter->step(1, FORWARD);
  }
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

//  ***** USE WITH FACE DETECTION *****
  int moveSteps = players[currentPlayer] - atSteps;
  
  if(moveSteps < 0) {
    moveSteps = (fullRotation-atSteps) + players[currentPlayer]; 
  }
  
  atSteps += moveSteps;
  atSteps = atSteps % fullRotation;
  
  Serial.println(moveSteps);
  
  //rotate(moveSteps);
  rotate(240);
  
  currentPlayer = (currentPlayer+1) % playerCount;
  
  cardSpitter->step(35, FORWARD);

//  currentPlayer++;
  cardSpitter->release();
}

void getUserInput() {
  hitPressed = getHit();
  //passPressed = getPass();
  passPressed = 1000;
  startPressed = getStartStop();
  
  Serial.println(hitPressed);
  Serial.println(passPressed);
  Serial.println("Getting User Input");

  while(hitPressed > 900 && passPressed > 900 && !startPressed) {
    hitPressed = getHit();
    //passPressed = getPass();
    startPressed = getStartStop();
    delay(5);
  }
  
  if(startPressed) {
    endGame();
  }
  
  if(hitPressed < 900){
    Serial.println("Hit Pressed");
    dispenseCard();
    goToNextPlayer();
  }
  else if(passPressed < 900) {
    goToNextPlayer();
  }
  
  hitPressed = 1000;
  passPressed = 1000;
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

void reset() {
  cardSpitter->step(cardDealingSteps*cardsDealt, BACKWARD);
  cardSpitter->release();
  cardsDealt = 0;
}

void scanForFaces(int state) {
  lastDebounce = millis();
  
  //Serial.println("STATE");
  //Serial.println(state);
  
  if(state != 8) {
    rotate(25);
    faceStepsTaken += 25;
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
 rotate(25);
 faceStepsTaken += 25;
}

bool getRotLimit() {
  return digitalRead(rotSwitch);
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
 bool limit = false;

 while(!limit) {
  rotate(1);
  if(getRotLimit()) {
    limit = true;
  }
 }
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

