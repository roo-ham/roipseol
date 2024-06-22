
#include <Wire.h>
#include <Dynamixel2Arduino.h>

#include "Adafruit_TCS34725.h"
#include "roipseol_final.h"

Dynamixel2Arduino dxl(DXL_SERIAL, DXL_DIR_PIN);
using namespace ControlTableItem;

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
int color_id = COLOR_WHITE;

void setup() {
  //Serial Start
  DEBUG_SERIAL.begin(57600);
  while(!DEBUG_SERIAL);
  DEBUG_SERIAL.println("Starting...");

  //Digital Pin start
  pinMode(MESSAGE, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
  analogWrite(MESSAGE, 675);

  //Color Sensor Start
  analogWrite(A4, 1023); // 통신시작 실패 방지
  analogWrite(A5, 1023);
  Wire.begin();
  if (tcs.begin()) {
    //DEBUG_SERIAL.println("Found sensor");
  } else {
    DEBUG_SERIAL.println("No TCS34725 found ... check your connections");
    while (1); // halt!
  }

  //DXL Start
  dxl_setup();
  arm_reset();

  //Create Access Point
  create_ap();

  int rule = 0;

  while (true) {
    request_pdf("A0");
    digitalWrite(BUZZER, 1);
    delay(30);
    digitalWrite(BUZZER, 0);
    if (rule == 0) {
      beep();
      request_pdf("A1");

      beep();
      request_pdf("A2");

      ring_bell(); // START
      grip_ext();
      request_pdf("A3");

      beep(); // 소화기 미션 1
      move_line(575);

      beep(); // 소화기 미션 2
      arm_reset();
      request_pdf("A4");

      beep(); // 손수건 미션
      hold_nose();
      move_angle(90);
      move_line(925);
      request_pdf("A5");

      beep(); // 엘리베이터 미션
      move_until_red(true);
      move_angle(90);
      move_until_red(true);
      arm_reset();
      request_pdf("A6");

      beep();
      request_pdf("A7"); // 퀴즈

      beep();
      request_pdf("A14"); // 종료

      beep();

      rule = 1;
    } else if (rule == 1) {
      beep();
      request_pdf("A8");

      beep();
      request_pdf("A9");

      ring_bell(); // START
      request_pdf("A10");
      shrink_arm();
      move_line(300);

      beep(); // 책상 미션
      move_angle(180);
      move_line(300);

      move_angle(-90);
      move_line(250);

      move_angle(-90);
      move_line(300);
      move_angle(90);

      beep(); // 엘리베이터 미션
      move_line(300);
      move_angle(-90);
      arm_reset();
      move_line(250);
      move_angle(-90);
      move_line(250);
      request_pdf("A11");

      beep(); // 머리 가리기, 운동장 돌기 미션
      dxl_arm_off();
      while (!dxl_is_arm_up()) {
        tone(BUZZER, 350, 200);
        delay(400);
        tone(BUZZER, 350, 200);
        beep();
      }
      dxl_arm_on();
      shield_head();
      request_pdf("A12");

      beep();
      move_angle(90);
      move_line(150);
      move_angle(180);
      move_line(150);
      move_angle(180);
      move_line(50);
      move_angle(-90);
      arm_reset();
      request_pdf("A13");

      beep();
      request_pdf("A14");
      rule = 0;
      
      beep();
    }
  }
}

int guess_color(uint16_t r, uint16_t g, uint16_t b) {
  int mean = (r + g + b) / 3;
  int diff = (pow(r - mean, 2) + pow(g - mean, 2) + pow(b - mean, 2)) / 3;
  diff = pow(diff, 0.5);
  Serial.println((String) "mean:" + mean + " diff:" + diff);
  if (mean > 700 && diff < 90) {
    return COLOR_WHITE;
  }
  if (mean < 600 && diff < 90) {
    return COLOR_BLACK;
  }
  if (mean > 700 && diff > 100 && r > 900) {
    return COLOR_RED;
  }
  return color_id;
}

void beep() {
  while (digitalRead(BUTTON));
  digitalWrite(BUZZER, 1);
  delay(30);
  digitalWrite(BUZZER, 0);
  delay(1000);
}

void ring_bell() {
  while (digitalRead(BUTTON));
  for (int i = 0; i < 3; i ++) {
    digitalWrite(BUZZER, 1);
    delay(30);
    digitalWrite(BUZZER, 0);
    delay(200);
  }
}

void arm_reset() {
  dxl_arm_run(ZERO_POS + 120, ZERO_POS - 100, ZERO_POS + 100, ZERO_POS - 120);
  delay(500);
}

void grip_ext() {
  dxl_arm_run(ZERO_POS + 140, ZERO_POS, ZERO_POS, ZERO_POS - 140);
  delay(500);
  dxl_arm_run(ZERO_POS + 140, ZERO_POS, ZERO_POS - 10, ZERO_POS - 200);
  delay(200);
  dxl_arm_run(ZERO_POS + DEGREE_45, ZERO_POS, ZERO_POS - 10, ZERO_POS - 200);
  delay(200);
}

void hold_nose() {
  dxl_arm_run(ZERO_POS + 140, ZERO_POS + 20, ZERO_POS - 20, ZERO_POS - 140);
  delay(200);
  dxl_arm_run(ZERO_POS + 150, ZERO_POS + 20, ZERO_POS - 20, ZERO_POS - 150);
  delay(200);
}

void shield_head() {
  dxl_arm_run(ZERO_POS + 140, ZERO_POS + DEGREE_90, ZERO_POS - DEGREE_90, ZERO_POS - 140);
  delay(1000);
}

void shrink_arm() {
  dxl_arm_run(ZERO_POS + 140, MAX_POS, MIN_POS, ZERO_POS - 140);
  delay(1000);
}

void loop() {
  static uint16_t red, green, blue, clear;
  tcs.getRawData(&red, &green, &blue, &clear);
  color_id = guess_color(red, green, blue);
  Serial.println((String) "r:" + red + " g:" + green + " b:" + blue);
  Serial.println((String) "IR_LEFT:" + analogRead(IR_LEFT));
  delay(10);
}