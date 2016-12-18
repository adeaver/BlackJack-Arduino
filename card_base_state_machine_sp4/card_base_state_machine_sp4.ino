#include <Wire.h>
//#include <SPI.h>
#include <Adafruit_MotorShield.h>
//#include "utility/Adafruit_MS_PWMServoDriver.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// Stepper Motor with 1.8 degrees per step
Adafruit_StepperMotor *rotational = AFMS.getStepper(200, 1);
Adafruit_StepperMotor *cardSpitter = AFMS.getStepper(200, 2);

uint8_t cardsDealt = 0;
const int deltaSteps = 150;
const int fullRotation = 600;
int currentStep = 0;


int reflectanceVal = 1001;           // variable to store the value read

const uint8_t reflectancePin = A3;     // reflectance sensor is connected to pin 3
const uint8_t hitButtonPin = A1;      //input pin for "Hit" button
const uint8_t passButtonPin = A5;     //input pin for "Pass" button
const uint8_t startButton = 6;
//const uint8_t E_STOP = 4;

int hitPressed = LOW;
int passPressed = LOW;
int startPressed = LOW;

boolean started = false;
boolean faceScanning = true;
boolean playingGame = true;

unsigned long lastDebounce = millis();
unsigned long debounceDelay = 2000;

void setup() {
  Serial.begin(9600); 
  Serial.setTimeout(100);
  Serial.println("Hello World");
  
  pinMode(startButton, INPUT);
  pinMode(hitButtonPin, INPUT);
  pinMode(passButtonPin, INPUT);
  pinMode(reflectancePin, INPUT);

  Serial.println("Hello World");
  AFMS.begin();
  Serial.println("Hello");
  rotational->setSpeed(20);
  cardSpitter->setSpeed(0);
  
}

void loop() {
  
  Serial.println("In Loop");
  /*
  if(playingGame) {
    if(!faceScanning) {
//      if(!started) {
//        started = true;
//        zero();
//        goToNextPlayer();
//      }
//      
//      getUserInput();
      faceScanning = true;
      
    } 
    
    if(faceScanning) {
      //Serial.println("Hello");
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
    Serial.println(playingGame);
    if(playingGame) {
      lastDebounce = millis(); 
    }
  }
  */
  started = getStartStop();
  Serial.println(started);
  if(started){
    
   deal();
   
   for(int i = 0; i <= fullRotation/deltaSteps; i++) {
     getUserInput();
   }
  }
  
  
}

void deal(){
  Serial.println("Dealing");
  for(int i = 0; i <= fullRotation/deltaSteps; i++)
  {
    rotate(deltaSteps);
    dispenseCard();
    dispenseCard();
    cardsDealt += 2;
    currentStep += deltaSteps;
  }
}

void dispenseCard() {
  Serial.println("Dispensing");
  Serial.println(reflectanceVal);
  //cardsDealt++;
  delay(20);
  reflectanceVal = analogRead(reflectancePin);
  //cardDealDebounce = millis();
  delay(20);
  
  Serial.println(reflectanceVal);
  while (reflectanceVal > 900) {
    reflectanceVal = analogRead(reflectancePin);    // read the input pin
    //delay(20);
    //Serial.println(reflectanceVal);
    cardSpitter->step(1, FORWARD);
  }
  cardSpitter->release();
  Serial.println("Done");
  reflectanceVal = 1001;
  delay(200);
}

void rotate(int steps) {
  cardSpitter->step(75, BACKWARD);
  rotational->step(steps, FORWARD, DOUBLE);
  cardSpitter->step(40, FORWARD);
  cardSpitter->release();
  rotational->release();
}

void rotateBack(int steps) {
  cardSpitter->step(75, BACKWARD);
  rotational->step(steps, BACKWARD, DOUBLE);
  cardSpitter->step(40, FORWARD);
  cardSpitter->release();
  rotational->release();
}

/*void goToNextPlayer() {
  Serial.println("Rotating");
  

//  int moveSteps = players[currentPlayer] - atSteps;
  
//  if(moveSteps < 0) {
//    zero();
//    moveSteps = players[currentPlayer]; 
//  }
  atSteps += moveSteps;
  atSteps = atSteps % fullRotation;
  
 // Serial.println(moveSteps);
  
  //rotate(moveSteps);
  rotate(150);
  
  currentPlayer = (currentPlayer+1) % playerCount;
}
*/
void getUserInput() {
  hitPressed = getHit();
  passPressed = getPass();
  //passPressed = 1000;
  //startPressed = getStartStop();
  //Serial.println(passPressed);
//  delay(500);
//  hitPressed = 100;

  boolean wait = true;
  boolean hit = false;

  while(hitPressed > 1000 && passPressed > 800) {
    hitPressed = getHit();
    passPressed = getPass();
    //startPressed = getStartStop();
    
    if(hitPressed <= 1000) {
      hit = true;  
    }
    
    
    Serial.println(hitPressed);
    delay(150);
  }
  
  //Serial.println(hit);
  
  if(hit) {
    dispenseCard();
    //goToNextPlayer();
  } else if(passPressed < 900) {
    //goToNextPlayer();
    rotateBack(deltaSteps);
    
  }
  
//  if(startPressed) {
//    endGame();
//  }
  
  //dispenseCard();
  
  hitPressed = 1100;
  passPressed = 1100;
}

void endGame() {
  cardsDealt = 0;
  currentStep = 0;
  started = false;
  faceScanning = true;
  playingGame = false;
  delay(100);
}
/*
void scanForFaces(int state) {
  lastDebounce = millis();
  
  if(state == 7) {
    rotate(deltaSteps);
    currentStep += deltaSteps;
  } else if (state == 8) {
//     addPlayer();
  } else if (state == 6) {
    //zero();
  } else if (state == 5) {
    dispenseCard();  
  } else if (state == 4) {
    //goToNextPlayer();  
  }
    else if (state == 3) {
    getUserInput(); 
  }
  
  if(currentStep >= fullRotation) {
    faceScanning = false; 
  }
  
  rotational->release();
  
  sendState();
}
*/

void sendState() {
  //Serial.println("Sending state");
  if(currentStep < fullRotation) {
    Serial.write("7"); 
  } else {
    Serial.write("9");
  } 
}

/*void addPlayer() {
 players[playerCount] = faceStepsTaken;
 playerCount = playerCount + 1;
 dispenseCard();
 dispenseCard();
 cardSpitter->step(35, BACKWARD);
 cardSpitter->release();
 rotate(cardDealingSteps);
 faceStepsTaken += cardDealingSteps;
}
*/

int getHit() {
  return analogRead(hitButtonPin);
}

int getPass() {
  return analogRead(passButtonPin);
}

bool getStartStop() {
  bool button = digitalRead(startButton);
  return button;
  
}
/*
int flushSerial() {
  int val = 0;
  int rVal = 0;
  while(Serial.available()) {
    rVal = Serial.read();
    //Serial.println("Reading serial");
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
*/
