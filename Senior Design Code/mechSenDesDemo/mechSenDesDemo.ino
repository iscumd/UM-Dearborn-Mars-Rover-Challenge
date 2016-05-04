#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

Adafruit_MotorShield ArmShield(0x62);       //addresses for motor shields (check physical connections on jumpers) 
Adafruit_MotorShield ContainShield(0x61);
Adafruit_MotorShield DoorShield(0x60);

Adafruit_StepperMotor *lArmStepper = ArmShield.getStepper(200, 2);   //left arm motor while looking from end with servos
//Adafruit_StepperMotor *rArmStepper = ArmShield.getStepper(200, 1);   //right arm motor from same orientation as above
Adafruit_StepperMotor *mContainStepper = ContainShield.getStepper(200, 1); //mobile servo motor for container
Adafruit_StepperMotor *sContainStepper = ContainShield.getStepper(200, 2); //stationary servo motor for container
Adafruit_StepperMotor *lDoorStepper = DoorShield.getStepper(200, 1); //left door with respect to looking from back of robot
Adafruit_StepperMotor *rDoorStepper = DoorShield.getStepper(200, 2); //right door with respect to looking from back of robot

int incomingByte=0;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600); //initialize serial communication with baud rate of 9600 bits/sec
  ArmShield.begin();  //initialize motor shields
  ContainShield.begin();
  DoorShield.begin();
   int stepSize=50;
  int rpm =450;
  int drpm=150;   //door's rpm
  lArmStepper->setSpeed(rpm);   //set motors' rpms
  //rArmStepper->setSpeed(rpm);
  mContainStepper->setSpeed(rpm);
  sContainStepper->setSpeed(rpm);
  lDoorStepper->setSpeed(drpm);
  rDoorStepper->setSpeed(drpm);

  //digitalWrite(13, HIGH);
  //delay(1000);
   
} 

void loop() {
  /*for(int i=0; i<500; i++){
    sContainStepper->step(1, FORWARD, SINGLE);
  }
  for(int j=0; j<500; j++){
    sContainStepper->step(1, BACKWARD, SINGLE);
  }*/
  //sContainStepper->step(500, FORWARD, SINGLE);
  //sContainStepper->step(500, BACKWARD, SINGLE);
  // put your main code here, to run repeatedly:
  //lDoorStepper->step(300, FORWARD, SINGLE);
  //lDoorStepper->step(300,BACKWARD, SINGLE);
  //Serial.println(Serial.available());
  if(Serial.available()>0){                   //if Serial port is open
   // digitalWrite(13, LOW);
    incomingByte = Serial.read();             //read incoming byte
    //println(incomingByte);
    if(incomingByte=='1'){                 
      //mContainStepper->step(stepSize, FORWARD, DOUBLE);    //UP
      mContainStepper->onestep(FORWARD, DOUBLE);
    } else if(incomingByte=='2'){
      sContainStepper->onestep(BACKWARD, DOUBLE);    //RIGHT
    } else if(incomingByte=='3'){
      mContainStepper->onestep(BACKWARD, DOUBLE);   //LEFT
    } else if(incomingByte=='4'){
      sContainStepper->onestep(FORWARD, DOUBLE);   //DOWN
    } else if(incomingByte=='5'){                   //close arm
     lArmStepper->onestep(FORWARD, DOUBLE);     //A
     //rArmStepper->onestep(FORWARD, DOUBLE);
     //mContainStepper->onestep(FORWARD, DOUBLE);
    } else if(incomingByte=='6'){                 //open arm
      lArmStepper->onestep(BACKWARD, DOUBLE);    //B
      //rArmStepper->onestep(BACKWARD, DOUBLE);
      //0.mContainStepper->onestep(BACKWARD, DOUBLE);
    } else if(incomingByte=='7'){               //open doors
      lDoorStepper->onestep(FORWARD, DOUBLE);   //X
      rDoorStepper->onestep(BACKWARD, DOUBLE);
    } else if(incomingByte=='8'){                 //close doors
      lDoorStepper->onestep(BACKWARD, DOUBLE);    //Y
      rDoorStepper->onestep(FORWARD, DOUBLE);
    } else if(incomingByte=='0'){               //NO BUTTON IS PRESSED; do nothing
      delay(1);
    } //else {
      //digitalWrite(13, HIGH);     //not recognized char
   // }
  }
}
