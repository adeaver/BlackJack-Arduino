#include <Wire.h>
#include <SPI.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// Stepper Motor with 1.8 degrees per step
Adafruit_StepperMotor *rotational = AFMS.getStepper(200, 1);
Adafruit_StepperMotor *cardSpitter = AFMS.getStepper(200, 2);

String token = "";

uint8_t cardsDealt = 0;
int faceStepsTaken = 0;
uint8_t playerCount = 0;
uint8_t currentPlayer = 0;
int players[50];

uint8_t reflectancePin = A3;     // reflectance sensor is connected to pin 3
int reflectanceVal = 1001;           // variable to store the value read

const uint8_t hitButtonPin = 10;      //input pin for "Hit" button
const uint8_t passButtonPin = 9;     //input pin for "Pass" button
const uint8_t rotSwitch = 8;

int hitPressed = LOW;
int passPressed = LOW;

const int stopSteps = 1200;
const int cardDealingSteps = 30;


// SPI Variables
char buf [100];
volatile byte pos;


unsigned long lastDebounce = millis();
unsigned long debounceDelay = 350;

void setup() {
  Serial.begin(19200);
  
  AFMS.begin();
  
  // turn on SPI in slave mode
  SPCR |= bit (SPE);

  // have to send on master in, *slave out*
  pinMode(MISO, OUTPUT);

  // get ready for an interrupt 
  pos = 0;   // buffer empty

  // now turn on interrupts
  SPI.attachInterrupt();
  
  rotational->setSpeed(20);
  cardSpitter->setSpeed(15);
  
  zero();
  reset();
  
  
}



void loop() {
  getUserInput();
}

void startNewGame() {
  pos = 0;
}

void dispenseCard() {
  cardsDealt++;
  while (reflectanceVal>1000) {
    reflectanceVal = analogRead(reflectancePin);    // read the input pin
    cardSpitter->step(4, FORWARD, DOUBLE);
  }
  cardSpitter->release();
  reflectanceVal = 1001;
  
}

void rotate(int steps) {
rotational->step(steps, FORWARD, DOUBLE);
}

void goToNextPlayer() {
  cardSpitter->step(100, BACKWARD);

  int moveSteps = players[currentPlayer + 1] - players[currentPlayer];
  rotate(moveSteps);
  
  cardSpitter->step(85, FORWARD);

  currentPlayer++;
  cardSpitter->release();
  rotational->release();
  
}

void getUserInput() {

  while(!getHit() && !getPass()) {
    hitPressed = getHit();
    passPressed = getPass();
  }
  if(hitPressed){
    dispenseCard();
  }
  else(passPressed){
    goToNextPlayer();
  }
  
  hitPressed = LOW;
  passPressed = LOW;
}

void reset() {
  cardSpitter->step(cardDealingSteps*cardsDealt, BACKWARD);
  cardSpitter->release();
  cardsDealt = 0;
}


void addPlayer() {
 players[playerCount] = faceStepsTaken;
 playerCount++;
 rotate(25);
 faceStepsTaken += 25;
}

bool getRotLimit(){
  return digitalRead(rotSwitch);
}
bool getHit(){
  return digitalRead(hitButtonPin);
}
bool getPass(){
  return digitalRead(passButtonPin);
}
void zero() {
 bool limit = false;

 while(!limit) {
  rotate(10);
  if(getRotLimit()) {
    limit = true;
  }
 }
}
}


