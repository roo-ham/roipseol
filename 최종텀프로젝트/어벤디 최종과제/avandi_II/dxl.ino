#define USE_DXL 1

void dxl_setup() {
  #ifdef USE_DXL
  dxl.begin(1000000);
  dxl.setPortProtocolVersion(DXL_PROTOCOL_VERSION);
  for (int i = 0; i <= 5; i ++) {
    DEBUG_SERIAL.println((String) "Starting dxl ID: " + i);
    dxl.torqueOff(i);
    if (i <= 1) {
      dxl.setOperatingMode(i, OP_EXTENDED_POSITION);
    } else {
      dxl.setOperatingMode(i, OP_POSITION);
      dxl.writeControlTableItem(MAX_TORQUE, i, 1023);
    }
    bool c;
    c = dxl.torqueOn(i);
    while (!c) {
      delay(500);
      c = dxl.torqueOn(i);
    }
  }
  dxl_arm_run(ZERO_POS, MINUS_90, ZERO_POS);
  gripper_open(true);
  #endif
}

int dxl_arm_run(float pos1, float pos2, float pos3) {
  int ret = 0;
  #ifdef USE_DXL
  for (int i = 2; i <= 4; i ++) {
    dxl.writeControlTableItem(MOVING_SPEED, i, 128);
  }
  ret += dxl.setGoalPosition(2, pos1);
  ret += dxl.setGoalPosition(3, pos2);
  ret += dxl.setGoalPosition(4, pos3);
  while (abs(dxl.getPresentPosition(2) - pos1) > 16);
  while (abs(dxl.getPresentPosition(3) - pos2) > 16);
  while (abs(dxl.getPresentPosition(4) - pos3) > 16);
  #endif
  return ret;
}

int gripper_open(bool open) {
  int ret = 0;
  float pos = open ? 860 : 300;
  dxl.writeControlTableItem(TORQUE_LIMIT, 5, 300);
  ret += dxl.setGoalPosition(5, pos);
  delay(3000);
  return ret;
}

void move_line(float distance) {
  float E_0 = degrees(distance / WHEEL_RADIUS) / XL430_DP;
  float E_1 = degrees(distance / WHEEL_RADIUS) / XL430_DP;

  move_common(E_0, -E_1);
}

void move_angle(float angle) {
  float E_0 = degrees(radians(angle) * ROBOT_RADIUS / WHEEL_RADIUS) / XL430_DP;
  float E_1 = degrees(radians(angle) * ROBOT_RADIUS / WHEEL_RADIUS) / XL430_DP;

  move_common(E_0, E_1);
}

// percent
#define MAX_SPEED 8

void move_common(float E_0, float E_1) {
  dxl.torqueOff(0);
  dxl.torqueOff(1);
  dxl.setOperatingMode(0, OP_VELOCITY);
  dxl.setOperatingMode(1, OP_VELOCITY);
  dxl.torqueOn(0);
  dxl.torqueOn(1);
  float dxl_0_pos = dxl.getPresentPosition(0) + E_0;
  float dxl_1_pos = dxl.getPresentPosition(1) + E_1;
  if (E_0 > 0) dxl.setGoalVelocity(0, MAX_SPEED, UNIT_PERCENT);
  else if (E_0 < 0) dxl.setGoalVelocity(0, -MAX_SPEED, UNIT_PERCENT);
  if (E_1 > 0) dxl.setGoalVelocity(1, MAX_SPEED, UNIT_PERCENT);
  else if (E_1 < 0) dxl.setGoalVelocity(1, -MAX_SPEED, UNIT_PERCENT);
  bool goal_0 = false;
  bool goal_1 = false;
  while (!goal_0 || !goal_1) {
    if (!goal_0 && !goal_1) loop();
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

void dxl_print_position() {
  #ifdef USE_DXL
  DEBUG_SERIAL.print("dxl_position: ");
  for (int i = 0; i <= 5; i ++) {
    DEBUG_SERIAL.print(dxl.getPresentPosition(i));
    if (i == 5) {
      DEBUG_SERIAL.print("\n");
    } else {
      DEBUG_SERIAL.print(", ");
    }
  }
  #endif
}