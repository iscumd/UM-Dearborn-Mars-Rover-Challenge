//Use this code to autonomously run a basic robot with arcade drive, using an Arduino UNO and Talon SR.

double MaxSpeed = 60; //Change this value to set the speed of the robot. It is a percentage of the absolute maximum. The absolute maximum is 100.
double FrontLeftMotorSpeed = (MaxSpeed)/2;
double FrontRightMotorSpeed = (MaxSpeed)/2;
double BackLeftMotorSpeed = (MaxSpeed)/2;
double BackRightMotorSpeed = (MaxSpeed)/2;
double AxisInput1 = 0; //AxisInput1 and AxisInput2 should be variables ranging from -100 to 100.
double AxisInput2 = 0;
double C = 0; //AxisInput1 + AxisInput2
double D = 0;
double E = 0;
int RobotStatus = 0;
//Robot Statuses:
// 0 = Robot disabled
// 1 = Robot in Test Mode
// 2 = Robot in wall-repelling Autonomous Mode
// 3 = Robot in Teleoperated Mode

void setup(){
  pinMode(3, OUTPUT); //Signal wire to Talon for FrontLeftMotor
  pinMode(9, OUTPUT); //Signal wire to Talon for FrontRightMotor
  pinMode(10, OUTPUT); //Signal wire to Talon for BackLeftMotor
  pinMode(11, OUTPUT); //Signal wire to Talon for BackRightMotor
  pinMode(2, INPUT); //External toggle switch
  pinMode(4, OUTPUT); //External LED power
  pinMode(5, OUTPUT); //External speaker
  pinMode(7, INPUT); //Left limit switch
  pinMode(12, INPUT); //Right limit switch
  pinMode(8, INPUT); //Deadman's switch
  pinMode(A0, INPUT); //JoystickUp/Down
  pinMode(A1, INPUT); //JoystickLeft/Right
}

void ChangeMaxSpeed(double NewSpeed){
    MaxSpeed = NewSpeed;
    FrontLeftMotorSpeed = (MaxSpeed)/2;
    FrontRightMotorSpeed = (MaxSpeed)/2;
    BackLeftMotorSpeed = (MaxSpeed)/2;
    BackRightMotorSpeed = (MaxSpeed)/2;
    delay(10);
}

void Forward(int Speed, double Time){
    analogWrite(3,190 + FrontLeftMotorSpeed * Speed * 0.01);
    analogWrite(9,190 + FrontRightMotorSpeed * Speed * 0.01);
    analogWrite(10,190 + BackLeftMotorSpeed * Speed * 0.01);
    analogWrite(11,190 + BackRightMotorSpeed * Speed * 0.01);
    digitalWrite(4,HIGH);
    delay(1000*Time);
}
void Backward(int Speed, double Time){
    analogWrite(3,190 - FrontLeftMotorSpeed * Speed * 0.01);
    analogWrite(9,190 - FrontRightMotorSpeed * Speed * 0.01);
    analogWrite(10,190 - BackLeftMotorSpeed * Speed * 0.01);
    analogWrite(11,190 - BackRightMotorSpeed * Speed * 0.01);
    digitalWrite(4,HIGH);
    delay(1000*Time);
}
void Left(int Speed, double Time){
    analogWrite(3,190 - FrontLeftMotorSpeed * Speed * 0.01);
    analogWrite(9,190 + FrontRightMotorSpeed * Speed * 0.01);
    analogWrite(10,190 - BackLeftMotorSpeed * Speed * 0.01);
    analogWrite(11,190 + BackRightMotorSpeed * Speed * 0.01);
    digitalWrite(4,HIGH);
    delay(1000*Time);
}
void Right(int Speed, double Time){
    analogWrite(3,190 + FrontLeftMotorSpeed * Speed * 0.01);
    analogWrite(9,190 - FrontRightMotorSpeed * Speed * 0.01);
    analogWrite(10,190 + BackLeftMotorSpeed * Speed * 0.01);
    analogWrite(11,190 - BackRightMotorSpeed * Speed * 0.01);
    digitalWrite(4,HIGH);
    delay(1000*Time);
}
void Stop(double Time){
    analogWrite(3,190);
    analogWrite(9,190);
    analogWrite(10,190);
    analogWrite(11,190);
    digitalWrite(4,HIGH);
    delay(1000*Time);
}
void ZigZag(int Speed, double Time){
    analogWrite(3,190 + (FrontLeftMotorSpeed/1.5) * Speed * 0.01);
    analogWrite(9,190 + FrontRightMotorSpeed * Speed * 0.01);
    analogWrite(10,190 + (BackLeftMotorSpeed/1.5) * Speed * 0.01);
    analogWrite(11,190 + BackRightMotorSpeed * Speed * 0.01);
    digitalWrite(4,HIGH);
    delay(500*Time);
    analogWrite(3,190 + FrontLeftMotorSpeed * Speed * 0.01);
    analogWrite(9,190 + (FrontRightMotorSpeed/1.5) * Speed * 0.01);
    analogWrite(10,190 + BackLeftMotorSpeed * Speed * 0.01);
    analogWrite(11,190 + (BackRightMotorSpeed/1.5) * Speed * 0.01);
    digitalWrite(4,HIGH);
    delay(500*Time);
}
void CurveLeft(int Speed, double Curvature, double Time){ //Higher Curvature values make the robot turn more sharply
    analogWrite(3,190 + (FrontLeftMotorSpeed/Curvature) * Speed * 0.01);
    analogWrite(9,190 + FrontRightMotorSpeed * Speed * 0.01);
    analogWrite(10,190 + (BackLeftMotorSpeed/Curvature) * Speed * 0.01);
    analogWrite(11,190 + BackRightMotorSpeed * Speed * 0.01);
    digitalWrite(4,HIGH);
    delay(1000*Time);
}
void CurveRight(int Speed, double Curvature, double Time){
    analogWrite(3,190 + FrontLeftMotorSpeed * Speed * 0.01);
    analogWrite(9,190 + (FrontRightMotorSpeed/Curvature) * Speed * 0.01);
    analogWrite(10,190 + BackLeftMotorSpeed * Speed * 0.01);
    analogWrite(11,190 + (BackRightMotorSpeed/Curvature) * Speed * 0.01);
    digitalWrite(4,HIGH);
    delay(1000*Time);
}
void Accelerate(double StartSpeed, double EndSpeed, double Time){
    analogWrite(3,190 + FrontLeftMotorSpeed * (StartSpeed + ((EndSpeed-StartSpeed)/10))* 0.01);
    analogWrite(9,190 + FrontRightMotorSpeed * (StartSpeed + ((EndSpeed-StartSpeed)/10))* 0.01);
    analogWrite(10,190 + BackLeftMotorSpeed * (StartSpeed + ((EndSpeed-StartSpeed)/10))* 0.01);
    analogWrite(11,190 + BackRightMotorSpeed * (StartSpeed + ((EndSpeed-StartSpeed)/10))* 0.01);
    digitalWrite(4,HIGH);
    delay(100*Time);
    analogWrite(3,190 + FrontLeftMotorSpeed * (StartSpeed + 2*((EndSpeed-StartSpeed)/10))* 0.01);
    analogWrite(9,190 + FrontRightMotorSpeed * (StartSpeed + 2*((EndSpeed-StartSpeed)/10))* 0.01);
    analogWrite(10,190 + BackLeftMotorSpeed * (StartSpeed + 2*((EndSpeed-StartSpeed)/10))* 0.01);
    analogWrite(11,190 + BackRightMotorSpeed * (StartSpeed + 2*((EndSpeed-StartSpeed)/10))* 0.01);
    digitalWrite(4,HIGH);
    delay(100*Time);
    analogWrite(3,190 + FrontLeftMotorSpeed * (StartSpeed + 3*((EndSpeed-StartSpeed)/10))* 0.01);
    analogWrite(9,190 + FrontRightMotorSpeed * (StartSpeed + 3*((EndSpeed-StartSpeed)/10))* 0.01);
    analogWrite(10,190 + BackLeftMotorSpeed * (StartSpeed + 3*((EndSpeed-StartSpeed)/10))* 0.01);
    analogWrite(11,190 + BackRightMotorSpeed * (StartSpeed + 3*((EndSpeed-StartSpeed)/10))* 0.01);
    digitalWrite(4,HIGH);
    delay(100*Time);
    analogWrite(3,190 + FrontLeftMotorSpeed * (StartSpeed + 4*((EndSpeed-StartSpeed)/10))* 0.01);
    analogWrite(9,190 + FrontRightMotorSpeed * (StartSpeed + 4*((EndSpeed-StartSpeed)/10))* 0.01);
    analogWrite(10,190 + BackLeftMotorSpeed * (StartSpeed + 4*((EndSpeed-StartSpeed)/10))* 0.01);
    analogWrite(11,190 + BackRightMotorSpeed * (StartSpeed + 4*((EndSpeed-StartSpeed)/10))* 0.01);
    digitalWrite(4,HIGH);
    delay(100*Time);
    analogWrite(3,190 + FrontLeftMotorSpeed * (StartSpeed + 5*((EndSpeed-StartSpeed)/10))* 0.01);
    analogWrite(9,190 + FrontRightMotorSpeed * (StartSpeed + 5*((EndSpeed-StartSpeed)/10))* 0.01);
    analogWrite(10,190 + BackLeftMotorSpeed * (StartSpeed + 5*((EndSpeed-StartSpeed)/10))* 0.01);
    analogWrite(11,190 + BackRightMotorSpeed * (StartSpeed + 5*((EndSpeed-StartSpeed)/10))* 0.01);
    digitalWrite(4,HIGH);
    delay(100*Time);
    analogWrite(3,190 + FrontLeftMotorSpeed * (StartSpeed + 6*((EndSpeed-StartSpeed)/10))* 0.01);
    analogWrite(9,190 + FrontRightMotorSpeed * (StartSpeed + 6*((EndSpeed-StartSpeed)/10))* 0.01);
    analogWrite(10,190 + BackLeftMotorSpeed * (StartSpeed + 6*((EndSpeed-StartSpeed)/10))* 0.01);
    analogWrite(11,190 + BackRightMotorSpeed * (StartSpeed + 6*((EndSpeed-StartSpeed)/10))* 0.01);
    digitalWrite(4,HIGH);
    delay(100*Time);
    analogWrite(3,190 + FrontLeftMotorSpeed * (StartSpeed + 7*((EndSpeed-StartSpeed)/10))* 0.01);
    analogWrite(9,190 + FrontRightMotorSpeed * (StartSpeed + 7*((EndSpeed-StartSpeed)/10))* 0.01);
    analogWrite(10,190 + BackLeftMotorSpeed * (StartSpeed + 7*((EndSpeed-StartSpeed)/10))* 0.01);
    analogWrite(11,190 + BackRightMotorSpeed * (StartSpeed + 7*((EndSpeed-StartSpeed)/10))* 0.01);
    digitalWrite(4,HIGH);
    delay(100*Time);
    analogWrite(3,190 + FrontLeftMotorSpeed * (StartSpeed + 8*((EndSpeed-StartSpeed)/10))* 0.01);
    analogWrite(9,190 + FrontRightMotorSpeed * (StartSpeed + 8*((EndSpeed-StartSpeed)/10))* 0.01);
    analogWrite(10,190 + BackLeftMotorSpeed * (StartSpeed + 8*((EndSpeed-StartSpeed)/10))* 0.01);
    analogWrite(11,190 + BackRightMotorSpeed * (StartSpeed + 8*((EndSpeed-StartSpeed)/10))* 0.01);
    digitalWrite(4,HIGH);
    delay(100*Time);
    analogWrite(3,190 + FrontLeftMotorSpeed * (StartSpeed + 9*((EndSpeed-StartSpeed)/10))* 0.01);
    analogWrite(9,190 + FrontRightMotorSpeed * (StartSpeed + 9*((EndSpeed-StartSpeed)/10))* 0.01);
    analogWrite(10,190 + BackLeftMotorSpeed * (StartSpeed + 9*((EndSpeed-StartSpeed)/10))* 0.01);
    analogWrite(11,190 + BackRightMotorSpeed * (StartSpeed + 9*((EndSpeed-StartSpeed)/10))* 0.01);
    digitalWrite(4,HIGH);
    delay(100*Time);
    analogWrite(3,190 + FrontLeftMotorSpeed * EndSpeed * 0.01);
    analogWrite(9,190 + FrontRightMotorSpeed * EndSpeed * 0.01);
    analogWrite(10,190 + BackLeftMotorSpeed * EndSpeed * 0.01);
    analogWrite(11,190 + BackRightMotorSpeed * EndSpeed * 0.01);
    digitalWrite(4,HIGH);
    delay(100*Time);
}
void Autonomous1(){
    Stop(5); //Don't do anything for 5 seconds
    Forward(100,5); //Go forward at 100% (of whatever MaxSpeed is) for 5 seconds.
    Stop(5);
    Backward(100,5);
    Stop(5);
    CurveLeft(100,1.5,3);
    Stop(5);
    Backward(100,3);
    Stop(5);
    CurveRight(100,1.5,3);
    Stop(5);
    RobotStatus = 0;
}
void ManualControl(double Speed){
  while((RobotStatus == 3) && (digitalRead(2) == LOW) && (digitalRead(8) == LOW)){ //Set digitalRead(8) to HIGH if using pin for Deadman's switch
    AxisInput1 = map(analogRead(A0),0,1023,-100,100);
    AxisInput2 = map(analogRead(A1),0,1023,-100,100);
    C = AxisInput1 + AxisInput2;
    D = map(C,0,200,1,2);
    E = map(C,-200,0,2,1);
    if(C <= -100){
      analogWrite(3,190 + FrontLeftMotorSpeed * Speed * 0.01 * ((AxisInput1 - AxisInput2) / E) * 0.01);
      analogWrite(9,190 + FrontRightMotorSpeed * Speed * 0.01 * ((AxisInput1 + AxisInput2) / E) * 0.01);
      analogWrite(10,190 + BackLeftMotorSpeed * Speed * 0.01 * ((AxisInput1 - AxisInput2) / E) * 0.01);
      analogWrite(11,190 + BackRightMotorSpeed * Speed * 0.01 * ((AxisInput1 + AxisInput2) / E) * 0.01);
    }
    if(C >= 100){
      analogWrite(3,190 + FrontLeftMotorSpeed * Speed * 0.01 * ((AxisInput1 - AxisInput2) / D) * 0.01);
      analogWrite(9,190 + FrontRightMotorSpeed * Speed * 0.01 * ((AxisInput1 + AxisInput2) / D) * 0.01);
      analogWrite(10,190 + BackLeftMotorSpeed * Speed * 0.01 * ((AxisInput1 - AxisInput2) / D) * 0.01);
      analogWrite(11,190 + BackRightMotorSpeed * Speed * 0.01 * ((AxisInput1 + AxisInput2) / D) * 0.01);
    }
    if(C > -100 && C < 100){
      analogWrite(3,190 + FrontLeftMotorSpeed * Speed * 0.01 * ((AxisInput1 - AxisInput2) * 0.01));
      analogWrite(9,190 + FrontRightMotorSpeed * Speed * 0.01 * ((AxisInput1 + AxisInput2) * 0.01));
      analogWrite(10,190 + BackLeftMotorSpeed * Speed * 0.01 * ((AxisInput1 - AxisInput2) * 0.01));
      analogWrite(11,190 + BackRightMotorSpeed * Speed * 0.01 * ((AxisInput1 + AxisInput2) * 0.01));
    }
    delay(10);
  }
}

void loop(){
  delay(10);
  if(RobotStatus == 0){
    digitalWrite(4,LOW);
    analogWrite(3,0);
    analogWrite(9,0);
    analogWrite(10,0);
    analogWrite(11,0);
    delay(1000);
  }
  else if(RobotStatus == 1){
    Stop(5); //Don't do anything for 5 seconds
    Accelerate(0,100,5);
    Forward(100,3); //Go forward at 100% (of whatever MaxSpeed is) for 3 seconds.
    Accelerate(100,0,5);
    Stop(5);
    Accelerate(0,-100,5);
    Backward(100,3);
    Stop(5);
    CurveLeft(100,1.5,3);
    Stop(3);
    Backward(80,3);
    Stop(3);
    CurveRight(100,1.5,3);
    Stop(5);
    tone(5,200,500);
    delay(200);
    tone(5,0,500);
    delay(200);
    RobotStatus = 0;
  }
  else if(RobotStatus == 2){
    if(digitalRead(7)==LOW && digitalRead(12)==LOW){
      Forward(100,0.01);
    }
    else if(digitalRead(7)==HIGH && digitalRead(12)==LOW){
      Right(100,0.1);
    }
    else if(digitalRead(7)==LOW && digitalRead(12)==HIGH){
      Left(100,0.1);
    }
    else if(digitalRead(7)==HIGH && digitalRead(12)==HIGH){
      Backward(100,2);
      Right(100,1);
    }
  }
  else if(RobotStatus == 3){
    Stop(1); //Don't do anything for 1 second.
    ManualControl(100); //Allow manual control (at 100% of the MaxSpeed value) for 1 hour.
    tone(5,300,500);
    delay(200);
    tone(5,0,500);
    Stop(2);
    RobotStatus = 0;
  }
  
  if(digitalRead(2) == HIGH){
    RobotStatus = 0;
    tone(5,100,500);
    delay(1000);
    if(digitalRead(2) == HIGH){
      RobotStatus = 3;
      ChangeMaxSpeed(60);
      tone(5,200,500);
      delay(200);
      tone(5,0,500);
      delay(200);
      tone(5,200,500);
      delay(1000);
      if(digitalRead(2) == HIGH){
        RobotStatus = 3;
        ChangeMaxSpeed(80);
        tone(5,300,500);
        delay(200);
        tone(5,0,500);
        delay(200);
        tone(5,300,500);
        delay(200);
        tone(5,0,500);
        delay(200);
        tone(5,300,500);
        delay(1000);
        if(digitalRead(2) == HIGH){
          RobotStatus = 0;
          tone(5,100,500);
          delay(1000);
        }
      }
    }
  }
  else if(digitalRead(2) == LOW){
    delay(10);
  }
}
