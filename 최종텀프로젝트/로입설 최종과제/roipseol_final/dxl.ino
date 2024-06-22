
void dxl_setup() {
  dxl.begin(1000000);
  dxl.setPortProtocolVersion(DXL_PROTOCOL_VERSION);
  for (int i = 0; i <= 5; i ++) {
    DEBUG_SERIAL.print((String) "Starting dxl ID: " + i);
    dxl.torqueOff(i);
    if (i <= 1) {
      dxl.setOperatingMode(i, OP_EXTENDED_POSITION);
    } else {
      dxl.setOperatingMode(i, OP_POSITION);
      dxl.writeControlTableItem(MAX_TORQUE, i, 1023);
    }
    bool c;
    c = dxl.torqueOn(i);
    if (c) {
      DEBUG_SERIAL.println("...OK");
    } else {
      DEBUG_SERIAL.println("");
    }
    /*while (!c) {
      delay(500);
      c = dxl.torqueOn(i);
    }*/
  }
  //gripper_open(true);
}

void dxl_arm_off() {
  for (int i = 2; i <= 5; i ++) {
    dxl.torqueOff(i);
  }
}

bool dxl_is_arm_up() {
  if (dxl.getPresentPosition(4) < ZERO_POS + DEGREE_45) {
    return false;
  }
  if (dxl.getPresentPosition(2) > ZERO_POS - DEGREE_45) {
    return false;
  }
  return true;
}

void dxl_arm_on() {
  for (int i = 2; i <= 5; i ++) {
    dxl.torqueOff(i);
  }
}

int dxl_arm_run(float pos1, float pos2, float pos3, float pos4) {
  // ID 위치 : 5 <- 4 <-> 2 -> 3
  int ret = 0;
  for (int i = 2; i <= 5; i ++) {
    dxl.writeControlTableItem(MOVING_SPEED, i, 256);
  }
  ret += dxl.setGoalPosition(5, pos1);
  ret += dxl.setGoalPosition(4, pos2);
  ret += dxl.setGoalPosition(2, pos3);
  ret += dxl.setGoalPosition(3, pos4);
  return ret;
}

void move_line(float distance) {
  float E_0 = degrees(distance / WHEEL_RADIUS) / XL430_DP;
  float E_1 = degrees(distance / WHEEL_RADIUS) / XL430_DP;

  move_common(-E_0, E_1);
}

void move_angle(float angle) {
  float E_0 = degrees(radians(angle) * ROBOT_RADIUS / WHEEL_RADIUS) / XL430_DP;
  float E_1 = degrees(radians(angle) * ROBOT_RADIUS / WHEEL_RADIUS) / XL430_DP;

  move_common(E_0, E_1);
}

void move_common(float E_0, float E_1) {
  change_wheel_mode(OP_VELOCITY);
  float dxl_0_pos = dxl.getPresentPosition(0) + E_0;
  float dxl_1_pos = dxl.getPresentPosition(1) + E_1;
  if (E_0 > 0) dxl.setGoalVelocity(0, MAX_SPEED, UNIT_PERCENT);
  else if (E_0 < 0) dxl.setGoalVelocity(0, -MAX_SPEED, UNIT_PERCENT);
  if (E_1 > 0) dxl.setGoalVelocity(1, MAX_SPEED, UNIT_PERCENT);
  else if (E_1 < 0) dxl.setGoalVelocity(1, -MAX_SPEED, UNIT_PERCENT);
  bool goal_0 = false;
  bool goal_1 = false;
  while (!goal_0 || !goal_1) {
    float local_E_0 = dxl_0_pos - dxl.getPresentPosition(0);
    float local_E_1 = dxl_1_pos - dxl.getPresentPosition(1);
    //DEBUG_SERIAL.println((String) "local_E_0: " + local_E_0 +  "local_E_1: " + local_E_1);
    if (E_0 < 0) {
      local_E_0 *= -1;
    }
    if (E_1 < 0) {
      local_E_1 *= -1;
    }
    if (local_E_0 <= 0) {
      dxl.torqueOff(0);
      dxl.setOperatingMode(0, OP_EXTENDED_POSITION);
      dxl.torqueOn(0);
      dxl.setGoalPosition(0, dxl_0_pos);
      goal_0 = true;
    }
    if (local_E_1 <= 0) {
      dxl.torqueOff(1);
      dxl.setOperatingMode(1, OP_EXTENDED_POSITION);
      dxl.torqueOn(1);
      dxl.setGoalPosition(1, dxl_1_pos);
      goal_1 = true;
    }
  }
  delay(500);
  DEBUG_SERIAL.println("move done!");
}

void change_wheel_mode(uint8_t mode) {
  for (int i = 0; i <= 1; i ++) {
    dxl.torqueOff(i);
    dxl.setOperatingMode(i, mode);
    dxl.torqueOn(i);
  }
}

void move_until_red(bool forward) {
  change_wheel_mode(OP_VELOCITY);
  float gain_left = 0;
  float gain_right = 0;
  if (forward) {
    dxl.setGoalVelocity(0, -MAX_SPEED, UNIT_PERCENT);
    dxl.setGoalVelocity(1, MAX_SPEED, UNIT_PERCENT);
  } else {
    dxl.setGoalVelocity(0, MAX_SPEED, UNIT_PERCENT);
    dxl.setGoalVelocity(1, -MAX_SPEED, UNIT_PERCENT);
  }
  while (color_id == COLOR_RED) {
    loop();
  }
  delay(100);
  while (color_id != COLOR_RED) {
    loop();
  }
  change_wheel_mode(OP_EXTENDED_POSITION);
}