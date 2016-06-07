/*
 * Runs a dishwasher built according to instructions at 
 * http://www.instructables.com/id/Building-a-Dishwasher-From-Scratch
 * 
 * By default, this sketch assumes an LCD is connected on pins 8-13, 
 * three buttons are connected on pins 5-7, and a pump and two 
 * solenoids are connected on pins 2-4. Pin numbers can be easily 
 * changed by modifying the definitions near the top of the file.
 * 
 * Copyright (C) 2016 Nathaniel Paulus
 * Licensed with the MIT License
 */

#include <RBD_Timer.h>
#include <RBD_Button.h>
#include <elapsedMillis.h>
#include <LiquidCrystal.h>

// specify the pins the LCD is connected to
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);

//configure our button pins
RBD::Button left(7);
RBD::Button primary(6);
RBD::Button right(5);

//configure pins for solenoids and pump
const int solenoidAPin = 4, solenoidBPin = 3, pumpPin = 2;
boolean coldOn = false, hotOn = false;

boolean running = false; //stopped/waiting
boolean paused = false;

//washing cycles
String defaultInstructions = "Red btn to run";
String message = "Waiting", option1 = defaultInstructions, option2 = "";
int totalSeconds = 60*90; //seconds in a cycle
int secondsLeft = 0; //seconds left in current cycle
elapsedMillis mil; //automatically counts milliseconds

//pump and water sensor
const int minPumpTimeOn = 3000;
elapsedMillis pumpTimeOn;
boolean pumpOn = false;
const int waterSensorPin = A0, waterSensorThreshold = 750;

//thermistors
const int solenoidAThermistorPin = A1, solenoidBThermistorPin = A2;
//on a scale 0-100, which solenoid appears to be which?
//if number is less than 50, assume solenoidA is cold, B is hot
//otherwise assume the opposite
//this keeps a wildly-off reading from switching the solenoids  
byte tempScale = 49;

void setup() {
  //set the LCD's dimensions
  lcd.begin(16, 2);
  updateLCD();

  //set up solenoid and pump pins as outputs
  pinMode(solenoidAPin, OUTPUT);
  pinMode(solenoidBPin, OUTPUT);
  pinMode(pumpPin, OUTPUT);

  //set up input pins
  pinMode(waterSensorPin, INPUT_PULLUP);
  pinMode(solenoidAThermistorPin, INPUT_PULLUP);
  pinMode(solenoidBThermistorPin, INPUT_PULLUP);
  
  Serial.begin(9600);
}

void loop() {
  if(left.onPressed()) {
    //if paused, the left button means stop
    if(paused) {
      resetState();
    }
    updateLCD();
  }
  if(primary.onPressed()){
    //if stopped/waiting, start cycle
    if(!running && !paused){
      secondsLeft = totalSeconds;
      message = "Running";
      option1 = "";
      option2 = "Pause";
      running = true;
      coldOn = true;
      hotOn = true;
      mil = 0;
    }
    updateLCD();
  }
  if(right.onPressed()){
    Serial.println("Right btn pressed");
    //when running, the right button pauses
    if(running && !paused){
      Serial.println("pausing");
      message = "Paused";
      paused = true;
      option1 = "Stop";
      option2 = "Resume";
    }
    //when paused, the right button resumes
    else if(paused){
      Serial.println("resuming");
      paused = false;
      option1 = "";
      option2 = "Pause";
      message = "Running";
      mil = 0;
    }
    Serial.println("Updating the LCD");
    updateLCD();
  }

  if(running && !paused){
    //handle countdown
    if(mil >= 1000){
      mil -= 1000;
      secondsLeft--;
      updateLCD();
    }

    //handle termination of cycle (i.e., when it finishes naturally)
    if(secondsLeft == 0){
      resetState();
      option1 = "Finished";
      updateLCD();
      //TODO beep
    }    
  }

  //handle thermistors
  int a = analogRead(solenoidAThermistorPin), b = analogRead(solenoidBThermistorPin);
  //readings go lower when temperature increases
  //if a is cold and b is hot
  if(a > b){
    //decrement the tempScale, but not lower than 0
    if(tempScale > 0) tempScale--;
  }
  else if(tempScale < 100) tempScale++;
  
  //handle solenoids
  boolean aOn, bOn;
  if(tempScale < 50){
    aOn = coldOn, bOn = hotOn;
  }
  else {
    aOn = hotOn, bOn = coldOn;
  }
  if(!running || paused) {
    aOn = false, bOn = false;
  }
  solenoidA(aOn);
  solenoidB(bOn);
  
  //handle water sensor and pump
  //if the water sensor pin has been pulled low by current in the water
  if(analogRead(waterSensorPin) < waterSensorThreshold){
    if(!pumpOn){
      pumpOn = true;
      pumpTimeOn = 0;
      pump(HIGH);
    }
  }
  else if(pumpOn && pumpTimeOn >= minPumpTimeOn){
    pumpOn = false;
    pump(LOW);
  }
  
  delay(1);
}

//reset state to default (as when a cycle finishes, or is stopped by the user)
void resetState(){
  secondsLeft = 0;
  running = false;
  paused = false;
  message = "Waiting";
  option1 = defaultInstructions;
  option2 = "";
}

//the message (to be displayed on the top row)
//the number of seconds left in the cycle (0 if stopped)
//option1 is an option to be shown in the bottom left, option2 to be 
//shown on the bottom right (only if applicable)
void updateLCD(){
  lcd.clear();
  lcd.print(message);
  //go to line 2
  lcd.setCursor(0, 1);
  
  //show the time left if applicable
  if(running && !paused){
    lcd.setCursor(0, 1);
    //minutes
    lcd.print(secondsLeft / 60);
    lcd.print(":");
    //print the number of seconds
    //the % sign is used to divide and find the remainder
    byte seconds = secondsLeft % 60;
    if(seconds < 10) lcd.print("0"); // add a padding zero
    lcd.print(secondsLeft % 60);
  }
  //if no count down to show
  else {
    lcd.print(option1);
  }

  //move the cursor so option2 can be right-aligned
  lcd.setCursor(16-option2.length(), 1);
  lcd.print(option2);
}

// functions below turn the solenoids and pump on and off

void solenoidA(uint8_t state){
  digitalWrite(solenoidAPin, state);
}

void solenoidB(uint8_t state){
  digitalWrite(solenoidBPin, state);
}

void pump(uint8_t state){
  digitalWrite(pumpPin, state);
}

