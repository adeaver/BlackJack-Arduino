#include <Wire.h>
#include <SPI.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// Stepper Motor with 1.8 degrees per step
Adafruit_StepperMotor *rotational = AFMS.getStepper(200, 2);
Adafruit_StepperMotor *cardSpitter = AFMS.getStepper(200, 1);

String token = "";

int cardsDealt = 0;
int faceStepsTaken = 0;
int playerCount = 0;
int players[20];

int reflectancePin = A3;     // reflectance sensor is connected to pin 3
int reflectanceVal = 1001;           // variable to store the value read

int hitButtonPin = 10;      //input pin for "Hit" button
int passButtonPin = 9;     //input pin for "Pass" button

int hitPressed = LOW;
int passPressed = LOW;

const int stopSteps = 1200;
const int cardDealingSteps = 30;


// SPI Variables
char buf [100];
volatile byte pos;
volatile boolean stateChanged = true;
String state = "3333";
int lastState = 0;

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
}

void runState() {
  updateState();
  
  Serial.println("Running state: " + state);
//  if(state.startsWith("0")) {
//     startNewGame();  
//  }
  
  if(state.startsWith("1")) {
     dispenseCard();
     sendState(1);
  }
  
  if(state.startsWith("2")) {
     rotate();
  }
  
  if(state.startsWith("3")) {
     getUserInput();
  }
  
  if(state.startsWith("5")) {
    Serial.println("in this");
    dispenseCard();
    sendState(5);
  }
  
  if(state.startsWith("7")) {
    rotateForFaceDetection();
  }
  
  if(state.startsWith("8")) {
    addPlayer();  
  }
  
  lastDebounce = millis();
}

void loop() {
  if(stateChanged) {
    runState();
    stateChanged = false;
    pos = 0;
  } else {
    if(millis() - lastDebounce >= debounceDelay) {
      sendState(lastState); 
    }
  }
}

void startNewGame() {
  pos = 0;
  state = "0000";
  lastState = 1;
}

void dispenseCard() {
  cardsDealt++;
  Serial.println("DISPENSING");
  reflectanceVal = HIGH;
  while (reflectanceVal > 1000) {
    Serial.println(reflectanceVal);
    reflectanceVal = analogRead(reflectancePin);    // read the input pin
    cardSpitter->step(4, FORWARD, DOUBLE);
  }
  cardSpitter->release();
  reflectanceVal = HIGH;
}

void rotate() {
  cardSpitter->step(100, BACKWARD);
  rotational->step(300, FORWARD, DOUBLE);
  cardSpitter->step(85, FORWARD);
  
  cardSpitter->release();
  rotational->release();
  
  sendState(2);
}

void getUserInput() {
  // Could also send "4444" for negative input
  //sendState(3);  
  
  while(!hitPressed && !passPressed) {
    hitPressed = digitalRead(hitButtonPin);
    passPressed = digitalRead(passButtonPin);
  }
  
  if(hitPressed) {
    sendState(3);  
  } else {
    sendState(4);
  }
  
  hitPressed = LOW;
  passPressed = LOW;
}

void hit() {
  sendState(5);
}

void resendState() {
  sendState(lastState);
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
 rotational->step(30, FORWARD, DOUBLE);
 faceStepsTaken += 30;
 
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
  if (!stateChanged && pos < sizeof buf) {
    
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
  
  state = outState;
}

void sendState(int sState) {
  delay(150);
  
  SPI.transfer(sState);
  
  lastState = sState;
}
