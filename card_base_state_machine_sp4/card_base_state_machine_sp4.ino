#include <Wire.h>
#include <SPI.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// Stepper Motor with 1.8 degrees per step
Adafruit_StepperMotor *rotational = AFMS.getStepper(200, 2);
Adafruit_StepperMotor *cardSpitter = AFMS.getStepper(200, 1);

uint8_t cardsDealt = 0;
int faceStepsTaken = 0;
uint8_t playerCount = 5;
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

boolean started = false;
volatile boolean faceScanning = true;


// SPI Variables
char buf [100];
volatile byte pos;

unsigned long lastDebounce = millis();
unsigned long debounceDelay = 1000;

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
  
//  zero();
//  reset();
}

void loop() {
  if(!faceScanning) {
    if(!started) {
      dealCards();
      started = true; 
    }
    
    getUserInput();
  } 
  else {
    Serial.println("Debouncing");    
//    if(millis() - lastDebounce ) {
//      sendState();
//    }
  }
}

void startNewGame() {
  pos = 0;
}

void dealCards() {
  Serial.println("Dealing");
  for(int i = 0; i < playerCount; i++) {
    dispenseCard();
    dispenseCard();
    goToNextPlayer();
  }  
  
  Serial.println("Done Dealing"); 
}

void dispenseCard() {
  cardsDealt++;
  while (reflectanceVal>900) {
    reflectanceVal = analogRead(reflectancePin);    // read the input pin
    Serial.println(reflectanceVal);
    cardSpitter->step(4, FORWARD, DOUBLE);
  }
  cardSpitter->release();
  reflectanceVal = 1001;
  delay(100);
}

void rotate(int steps) {
  rotational->step(steps, FORWARD, DOUBLE);
}

void goToNextPlayer() {
  Serial.println("Rotating");
  cardSpitter->step(250, BACKWARD);

//  ***** USE WITH FACE DETECTION *****
  int comparePlayer = currentPlayer < playerCount - 1 ? currentPlayer + 1 : 0; 
  int moveSteps = players[comparePlayer] - players[currentPlayer];
  
  if(moveSteps < 0) {
    moveSteps *= -1;  
  }
  
  rotate(moveSteps);
  //rotate(240);
  
  currentPlayer = (currentPlayer+1) % playerCount;
  
  cardSpitter->step(225, FORWARD);

//  currentPlayer++;
  cardSpitter->release();
  rotational->release();
  
}

void getUserInput() {
  hitPressed = getHit();
  passPressed = getPass();

  while(!hitPressed && !passPressed) {
    hitPressed = getHit();
    passPressed = getPass();
  }
  if(hitPressed){
    dispenseCard();
  }
  else if(passPressed) {
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

void scanForFaces(int state) {
  lastDebounce = millis();
  
  if(state != 8) {
    rotate(25); 
    faceStepsTaken += 25;
  } else {
     addPlayer();
  }
  
  sendState();
}

void sendState() {
  Serial.println("Sending state");
  if(faceStepsTaken < 1200) {
    SPI.transfer(7); 
    faceScanning = false;
  } else {
    SPI.transfer(9); 
  } 
}

void addPlayer() {
 players[playerCount] = faceStepsTaken;
 playerCount++;
 rotate(25);
 faceStepsTaken += 25;
}

bool getRotLimit() {
  return digitalRead(rotSwitch);
}

bool getHit() {
  return digitalRead(hitButtonPin);
}

bool getPass() {
  return digitalRead(passButtonPin);
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

// SPI interrupt routine
ISR (SPI_STC_vect) {
  byte c = SPDR;  // grab byte from SPI Data Register
  Serial.println("Connected");

  if(faceScanning && c != 0 && c != 10) {
    Serial.println(char(c+48));
    scanForFaces(c);
  }
}