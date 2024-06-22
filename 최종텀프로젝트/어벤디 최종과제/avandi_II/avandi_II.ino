#include <Dynamixel2Arduino.h>

#include "avandi_II.h"

Dynamixel2Arduino dxl(DXL_SERIAL, DXL_DIR_PIN);
using namespace ControlTableItem;

int robot_target_pos = ROBOT_HOME;
int robot_current_pos = ROBOT_HOME;

bool has_book[3] = {true, true, true};
int qr_dx[3] = {0};
long qr_timestamp[3] = {0};
long timestamp = 0;

int robot_act = 0;
int robot_act_param = 0;
bool begin_act = false;
String error_code = "";

void setup() {
  //Serial Start
  DEBUG_SERIAL.begin(57600);
  while(!DEBUG_SERIAL);
  DEBUG_SERIAL.println("Starting...");

  //DXL Start
  dxl_setup();
  
  //Wifi Access Point Start
  wifi_ap_setup();
}

void read_serial() {
  if (DEBUG_SERIAL.available()) {
    if (DEBUG_SERIAL.read() != '/') {
      return;
    }
    delay(1);
    char c = DEBUG_SERIAL.read();
    switch (c) {
      case '0':
      set_target_pos(0);
      break;
      case '1':
      set_target_pos(1);
      break;
      case '2':
      set_target_pos(2);
      break;
      case '3':
      set_target_pos(3);
      break;
      case '4':
      set_target_pos(4);
      break;
      case 'g':
      grip_book();
      break;
      case 'c':
      check_book_id();
      break;
    }
  }
}

void set_target_pos(int target) {
  int near_target = robot_current_pos;
  int direction = 0;

  if (target > robot_current_pos) {
    direction ++;
  } else if (target < robot_current_pos) {
    direction --;
  }

  near_target += direction;
  robot_target_pos = target;
  robot_current_pos = near_target;

  if (direction == 1) {
    switch (near_target) {
      case BOOK_RETURN:
        move_line(185);
        break;
      case BOOK_A:
        move_angle(90);
        move_line(275);
        break;
      case BOOK_B:
        move_line(79);
        break;
      case BOOK_C:
        move_line(83);
        break;
    }
  } else if (direction == -1) {
    switch (near_target) {
      case ROBOT_HOME:
        move_line(-185);
        break;
      case BOOK_RETURN:
        move_line(-275);
        move_angle(-90);
        break;
      case BOOK_A:
        move_line(-79);
        break;
      case BOOK_B:
        move_line(-83);
        break;
    }
  }
  
  DEBUG_SERIAL.println((String) "robot_target_pos:" + robot_target_pos + " robot_current_pos:" + robot_current_pos);
  if (robot_target_pos != robot_current_pos) {
    set_target_pos(robot_target_pos);
  }
}

int check_book_id() {
  dxl_arm_run(ZERO_POS, 300, 400);
  long start_time = timestamp;
  long dt = 0;
  while (dt < 5) {
    loop();
    dt = timestamp - start_time;
  }
  dxl_arm_run(ZERO_POS, MINUS_90, ZERO_POS);
  for (int i = 0; i < 3; i ++) {
    if (timestamp - qr_timestamp[i] <= 5) {
      return i;
    }
  }
  return -1;
}

void grip_book() {
  dxl_arm_run(ZERO_POS, ZERO_POS - 40, MINUS_90 + 40);
  gripper_open(true);
  move_line(100);
  gripper_open(false);
  dxl_arm_run(ZERO_POS, ZERO_POS - 100, MINUS_90 + 100);
  move_line(-100);
  dxl_arm_run(ZERO_POS, MINUS_90, ZERO_POS);
}

#define BOOK_C_OFFSET -5

void insert_book(int offset) {
  dxl_arm_run(ZERO_POS + offset, ZERO_POS - 80, MINUS_90 + 80);
  move_line(87);
  gripper_open(true);
  dxl_arm_run(ZERO_POS + offset, ZERO_POS - 45, MINUS_90 + 45);
  dxl_arm_run(ZERO_POS + offset, ZERO_POS - 80, MINUS_90 + 80);
  move_line(-87);
  dxl_arm_run(ZERO_POS, MINUS_90, ZERO_POS);
}

void save_book() {
  dxl_arm_run(PLUS_90 - 10, MIN_POS, ZERO_POS);
  dxl_arm_run(PLUS_90 - 10, MIN_POS, PLUS_90);
  delay(500);
  gripper_open(true);
  delay(500);
  dxl_arm_run(PLUS_90 - 10, MIN_POS, ZERO_POS);
  dxl_arm_run(ZERO_POS, MINUS_90, ZERO_POS);
}

void unload_book() {
  gripper_open(true);
  dxl_arm_run(PLUS_90 - 10, MINUS_90, ZERO_POS);
  dxl_arm_run(PLUS_90 - 10, MINUS_90, PLUS_90);
  delay(500);
  gripper_open(false);
  delay(500);
  dxl_arm_run(PLUS_90 - 10, MINUS_90, ZERO_POS);
  dxl_arm_run(ZERO_POS, MINUS_90, ZERO_POS);
}

#define DEGREE_PER_PIXEL 0.05

void loop() {
  static unsigned long m2 = 0;
  if (millis() > m2 + 10) {
    //0.01초마다 할 명령들
    read_serial();
    m2 = millis();
  }
  //거의 실시간으로 할 명령들
  wifi_ap_client_event();

  if (robot_act == 0) {
    begin_act = false;
    error_code = "";
  }
  if (!begin_act) {
    if (robot_act != 0) begin_act = true;
    if (robot_act == 1) {
      set_target_pos(BOOK_RETURN);
      int found_index = check_book_id();
      if (found_index == -1) {
        error_code = "오류 : 반납함에 책을 넣어 주십시오.<br>";
        set_target_pos(ROBOT_HOME);
        robot_act = 0;
        return;
      }
      float correct_angle = qr_dx[found_index] * DEGREE_PER_PIXEL;
      move_angle(correct_angle);
      delay(1000);
      grip_book();
      move_angle(-correct_angle);
      int location_id = found_index + BOOK_A;
      set_target_pos(location_id);
      move_angle(-90);
      int offset = found_index == 2 ? BOOK_C_OFFSET : 0;
      insert_book(offset);
      move_angle(90);
      set_target_pos(ROBOT_HOME);
      has_book[found_index] = true;
      robot_act = 0;
    } else if (robot_act == 2) {
      int book_id = robot_act_param - BOOK_A;
      if (!has_book[book_id]) {
        robot_act = 0;
        return;
      }
      set_target_pos(robot_act_param);
      move_angle(-90);
      int found_index = check_book_id();
      if (found_index != book_id) {
        has_book[book_id] = false;
        error_code = "오류 : 책장에서 책을 찾을 수 없었습니다.<br>";
      } else {
        float correct_angle = qr_dx[book_id] * DEGREE_PER_PIXEL;
        move_angle(correct_angle);
        grip_book();
        save_book();
        move_angle(-correct_angle);
      }
      move_angle(90);
      set_target_pos(ROBOT_HOME);
      has_book[book_id] = false;
      robot_act = 0;
    } else if (robot_act == 3) {
      for (int i = 0; i < 3; i ++) {
        has_book[i] = false;
      }
      int book_id = 0;
      int saved_book_id = -1;
      int saved_book_loc = -1;
      int correctness = 0;
      bool keep_pos = false;
      for (int i = 0;; i ++) {
        if (i >= 3) {
          i = 0;
        }
        if (!keep_pos) {
          set_target_pos(BOOK_A + i);
          move_angle(-90);
        } else {
          keep_pos = false;
        }
        int found_index = check_book_id();
        while (found_index == -1) {
          error_code = "오류 : QR 코드가 인식되지 않았습니다. 책을 넣거나 QR코드를 점검하십시오.<br>";
          found_index = check_book_id();
        }
        error_code = "";
        if (found_index != i) {
          float correct_angle = qr_dx[found_index] * DEGREE_PER_PIXEL;
          move_angle(correct_angle);
          grip_book();
          move_angle(-correct_angle);
          if (saved_book_id == -1) {
            saved_book_id = found_index;
            saved_book_loc = i;
            save_book();
          } else {
            move_angle(90);
            set_target_pos(BOOK_A + saved_book_loc);
            move_angle(-90);
            int offset = found_index == 2 ? BOOK_C_OFFSET : 0;
            insert_book(offset);
            move_angle(90);
            set_target_pos(BOOK_A + i);
            move_angle(-90);
            unload_book();
            offset = saved_book_id == 2 ? BOOK_C_OFFSET : 0;
            insert_book(offset);
            saved_book_id = -1;
            saved_book_loc = -1;
            i--;
            keep_pos = true;
            continue;
          }
        } else {
          has_book[i] = true;
        }
        move_angle(90);
        if (has_book[0] && has_book[1] && has_book[2]) {
          break;
        }
      }
      set_target_pos(ROBOT_HOME);
      robot_act = 0;
    }
  }
}