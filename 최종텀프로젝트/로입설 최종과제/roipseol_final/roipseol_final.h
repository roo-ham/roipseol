
#define DXL_SERIAL   Serial1
#define DEBUG_SERIAL Serial

#define COLOR_BLACK 0
#define COLOR_RED 1
#define COLOR_WHITE 2

#define MESSAGE 3
#define BUZZER 4
#define BUTTON 8

#define IR_LEFT A0
#define IR_RIGHT A2

//2번 ~ 5번 셀 돌릴때 쓰는 유용한 각도 목록!
#define MIN_POS 155
#define MINUS_90 202
#define ZERO_POS 512
#define PLUS_90 822
#define MAX_POS 868

#define DEGREE_45 154
#define DEGREE_90 307

const int DXL_DIR_PIN = 2; // DYNAMIXEL Shield DIR PIN
const float DXL_PROTOCOL_VERSION = 1.0;

#define WHEEL_RADIUS (65 * 0.5)
#define ROBOT_RADIUS (165 * 0.5)

// dp는 deg/pulse 라는 뜻
#define XL430_DP 0.088

// 단위 : percent
#define MAX_SPEED 13
