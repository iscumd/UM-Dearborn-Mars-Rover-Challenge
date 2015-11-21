//MPU 6050
//short example

#include <Wire.h>

//setup constant variable for MPU address
const int MPU = 0x68;
//setup variables to read accelerometer data
int16_t AcX, AcY, AcZ;
//setup variables to read gyroscope data
int16_t GyX, GyY, GyZ;
//setup variable to read temperature
int16_t Tmp;

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
}

void loop()
{
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
  //specail equation for tempuration calculation
  Serial.print(Tmp/340.00+36.53);
  //Serial.println("");
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

  //print delay
  delay(300);
}
