#include <Wire.h>
#include <SPI.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// Stepper Motor with 1.8 degrees per step
Adafruit_StepperMotor *rotational = AFMS.getStepper(200, 1);
Adafruit_StepperMotor *cardSpitter = AFMS.getStepper(200, 2);

String token = "";

int cardsDealt = 0;
int faceStepsTaken = 0;
int playerCount = 0;
int players[20];

const int stopSteps = 1200;
const int cardDealingSteps = 30;

// SPI Variables
char buf [100];
volatile byte pos;
volatile boolean stateChanged = false;
String state = "0000";

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
}

void runState() {
  updateState();
  
  Serial.println("Running state: " + state);
  
  if(state.startsWith("1")) {
      dispenseCard();
  }
  
  if(state.startsWith("2")) {
     rotate();
  }
  
  if(state.startsWith("3")) {
     getUserInput();
  }
  
  if(state.startsWith("5")) {
    hit();
  }
  
  if(state.startsWith("7")) {
    rotateForFaceDetection();
  }
  
  if(state.startsWith("8")) {
    addPlayer();  
  }
}

void loop() {
  if(stateChanged) {
    runState();
    stateChanged = false;
    state = "";
  }
}

void dispenseCard() {
  cardsDealt++;
  cardSpitter->step(cardDealingSteps, FORWARD);
  cardSpitter->release();
  sendState(1);
}

void rotate() {
  cardSpitter->step(200, BACKWARD);
  rotational->step(300, FORWARD, DOUBLE);
  cardSpitter->step(200, FORWARD);
  
  cardSpitter->release();
  rotational->release();
  
  sendState(2);
}

void getUserInput() {
  // Could also send "4444" for negative input
  sendState(3);  
}

void hit() {
  sendState(5);
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
   sendState(9); 
 } else {
   sendState(8);
 }
}

void rotateForFaceDetection() {
 rotational->step(10, FORWARD);
 faceStepsTaken += 10;
 
 if(faceStepsTaken >= stopSteps) {
   sendState(9); 
 } else {
   sendState(8);
 }
}

// SPI interrupt routine
ISR (SPI_STC_vect) {
  byte c = SPDR;  // grab byte from SPI Data Register

  // add to buffer if room
  if (pos < sizeof buf) {
    
    if(c != 0 && c != 10) {
      buf [pos++] = char(c);
    } else {
      if(pos > 0) {
        stateChanged = true;
      }
    }
    
    if(pos > 12) {
      stateChanged = true;
    }
  }
}

void updateState() {
  String outState = "";
 
  for(int i = 0; i < pos; i++) {
    outState += String(buf[i]);
  }
  
  pos = 0;
  state = outState;
}

void sendState(int sState) {
  Serial.println("Sending state: " + String(char(sState+48)));
  for(int i = 0; i <= 12; i++) {
    SPI.transfer(sState+48);
  }
}
