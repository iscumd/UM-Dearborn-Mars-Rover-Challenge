#include <Wire.h>
#include "I2Cdev.h"
#include "MPU6050.h"




//setup constant variable for MPU address
const int MPU = 0x68;
//setup variables to read accelerometer data
int16_t AcX, AcY, AcZ;
//setup variables to read gyroscope data
int16_t GyX, GyY, GyZ;
//setup variable to read temperature
int16_t Tmp;

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage 

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

// not sure if this is needed
volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() 
{
    mpuInterrupt = true;
}

void setup()
{
  //I2C bus
  Wire.begin();
  Wire.beginTransmission(MPU);
  //PWR_MGMT_1 register
  Wire.write(0x6B);
  //set to 0 to wake up MPU 6050
  Wire.write(0);
  Wire.endTransmission(true);

  //Serial communication
  Serial.begin(9600);
  //Serial.println("It starts!");

// Setup interrupt routine
  cli();
  // Set the registers
  TCCR1A = 0; //Timer Counter Control register
  // Set mode
  TCCR1B = (1 << WGM12); // turn on CTC mode
  // Set prescale values (1024).
  TCCR1B |= (1 << CS12) | (1 << CS10);
  //Enable timer compare interrupt===> TIMSK1
  TIMSK1 |= (1 << OCIE1A);
  // Set OCR1A
  OCR1A = 624;
  // Enable global interrupts
  sei();
}



// use timer interrupt instead of loop for more constant time percision
ISR (TIMER1_COMPA_vect) 
{
  // echo any serial data recieved to allow communication
   if (Serial.available() > 0) 
   {
      incomingByte = Serial.read();
      Serial.print("echo: ");
      Serial.println(incomingByte, DEC);
   }
        
  Wire.beginTransmission(MPU);
  //start with register 0x3B ---- ACCEL_XOUT_H
  Wire.write(0x3B);
  //request 14 registers
  Wire.requestFrom(MPU,14,true);

  //sampling rate
  //delay(100);
  
  // << stands for bitwise shift left
  // |  stands for bitwise or
  //read accelerometer data
  //0x3B(ACCEL_XOUT_H)&0x3C(ACCEL_XOUT_L)
  AcX = Wire.read()<< 8|Wire.read();
  //0x3D(ACCEL_YOUT_H)&0x3E(ACCEL_YOUT_L)  
  AcY = Wire.read()<< 8|Wire.read();
  //0x3F(ACCEL_ZOUT_H)&0x40(ACCEL_ZOUT_L)
  AcZ = Wire.read()<< 8|Wire.read();

  //read temperature data
  //0x41(TEMP_OUT_H)&0x42(TEMP_OUT_L)
  Tmp = Wire.read() << 8|Wire.read();

  //0x43(GYRO_XOUT_H)&0x44(GYRO_XOUT_L)
  GyX = Wire.read()<< 8|Wire.read();
  //0x45(GYRO_YOUT_H)&0x46(GYRO_YOUT_L)  
  GyY = Wire.read()<< 8|Wire.read();
  //0x47(GYRO_ZOUT_H)&0x48(GYRO_ZOUT_L)
  GyZ = Wire.read()<< 8|Wire.read();

  //print data on the screen
  Serial.print("AcX=");
  Serial.print(AcX);
  Serial.print(" | ");
  Serial.print("AcY=");
  Serial.print(AcY);
  Serial.print(" | ");
  Serial.print("AcZ=");
  Serial.print(AcZ);
  //Serial.println("");
  Serial.print(" | ");
  Serial.print("Tmp=");
  //temperature calibration
  Serial.print(Tmp/340.00+36.53);
  Serial.print(" | ");
  Serial.print("GyX=");
  Serial.print(GyX);
  Serial.print(" | ");
  Serial.print("GyY=");
  Serial.print(GyY);
  Serial.print(" | ");
  Serial.print("GyZ=");
  Serial.print(GyZ);
  Serial.println("");
}

// needed to keep the IDE happy
void loop(){}
