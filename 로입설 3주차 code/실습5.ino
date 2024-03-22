#include<Servo.h>

Servo myServo;
int servo_degree=90;


void setup()
{
  myServo.attach(8);
  Serial.begin(9600);
  myServo write(servo_degree);
}


void loop()
{
  while(Serial.available()){
    
    servo_degree=Serial.parseInt();
    Serial.print("degree");
    Serial.println(servo_degree);
    
  }
  myServo.write(servo_degree);
}