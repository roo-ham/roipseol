#include <R4Timer.h>
#include "HUSKYLENS.h"

#define PWM_L 10  //M1
#define PWM_R 9   //M2
#define DIR_L1 8
#define DIR_L2 12
#define DIR_R1 6
#define DIR_R2 7

#define IR_USAGE A1
#define IR_1 A2
#define IR_2 A3

HUSKYLENS huskylens;
HUSKYLENSResult result;

byte temp_button = 0;
int forward = 0;
int rotation = 0;
volatile int t2_flag = 0;

void setup() {
  Serial.begin(115200);

  Timer1.initialize(50);
  Timer1.attachInterrupt(T2ISR);
  Timer1.start();

  pinMode(DIR_L1, OUTPUT);
  pinMode(DIR_L2, OUTPUT);
  pinMode(DIR_R1, OUTPUT);
  pinMode(DIR_R2, OUTPUT);
  pinMode(IR_USAGE, INPUT_PULLUP);
  pinMode(IR_1, INPUT);
  pinMode(IR_2, INPUT);
  analogWrite(PWM_L, 0);
  analogWrite(PWM_R, 0);

  Wire.begin();
  while (!huskylens.begin(Wire))
  {
      Serial.println(F("Begin failed!"));
      Serial.println(F("1.Please recheck the \"Protocol Type\" in HUSKYLENS (General Settings>>Protocol Type>>I2C)"));
      Serial.println(F("2.Please recheck the connection."));
      delay(100);
  }
}

#define FORWARD_SPEED 50

void loop_ir() {
  int x0 = analogRead(IR_1);
  int x1 = analogRead(IR_2) / 10;
  int combination = (x0 - x1) * 1.5;
  if (analogRead(IR_1) > 40 && analogRead(IR_2) > 400) {
    forward = 0;
    rotation = 0;
  } else {
    forward = (30 - abs(combination)) * 1.3;
    rotation = combination * 1.0;
  }
}

void loop_cam() {
  int x0 = result.xOrigin;
  int x1 = result.xTarget;
  int y0 = result.yOrigin;
  int y1 = result.yTarget;
  if (y1 > 120) { // 비정상적인 값?
    return;
  } else if (y1 > 90) {
    forward = 0;
    rotation=0;
    return;
  }
  int average = (x0 + x1 - 320) / 2;
  int minus = x1 - x0;
  rotation = -average / 2;
  forward = FORWARD_SPEED - abs(rotation);
}

void loop() {
  static bool has_result = 0;
  static int want_stop = 0;
  if (t2_flag) {
    t2_flag = 0;
    //Serial.println(String() + forward + ", " + rotation);
    if (want_stop > 0) {
      want_stop --;
    }
    Serial.println(String() + analogRead(IR_1) + ", " + analogRead(IR_2));
  }
  leftWheel(forward + rotation);
  rightWheel(forward - rotation);
  if (!huskylens.request() || !huskylens.isLearned() || !huskylens.available());
  else {
      while (huskylens.available())
      {
        result = huskylens.read();
        has_result = 1;
      }    
  }
  if (analogRead(IR_2) > 150 && analogRead(IR_1) > 80) {
    //want_stop = 20;
  }
  /*if (want_stop) {
    forward = 0;
    rotation = 0;
    return;
  }*/
  if (digitalRead(IR_USAGE)) {
    loop_ir();
  } else {
    if (has_result) {
      loop_cam();
    }
  }
}

void T2ISR() {
  t2_flag = 1;
}

#define MINIMUM_WORK 1
#define MIN_SPEED 35

void leftWheel(int speed) {
  if (abs(speed) < MINIMUM_WORK) {
    speed = 0;
  } else if (abs(speed) < MIN_SPEED) {
    speed = speed > 0 ? MIN_SPEED : -MIN_SPEED;
  }
  int magnitude = abs(speed);
  bool direction = speed > 0;
  bool bi_direction = speed < 0;
  analogWrite(PWM_L, magnitude);
  digitalWrite(DIR_L1, direction);
  digitalWrite(DIR_L2, bi_direction);
}

void rightWheel(int speed) {
  if (abs(speed) < MINIMUM_WORK) {
    speed = 0;
  } else if (abs(speed) < MIN_SPEED) {
    speed = speed > 0 ? MIN_SPEED : -MIN_SPEED;
  }
  int magnitude = abs(speed);
  bool direction = speed < 0;
  bool bi_direction = speed > 0;
  analogWrite(PWM_R, magnitude);
  digitalWrite(DIR_R1, direction);
  digitalWrite(DIR_R2, bi_direction);
}

// 라인트레이싱 하는 무언가...