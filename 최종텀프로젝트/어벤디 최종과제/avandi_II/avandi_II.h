
#define DXL_SERIAL   Serial1
#define DEBUG_SERIAL Serial

// Set these to your desired credentials.
#define AVANDI_SSID "avandi4"
#define AVANDI_PASS "123456789"

#define COLOR_BLACK 0
#define COLOR_RED 1
#define COLOR_WHITE 2

#define IR_A A0
#define IR_B A2

//2번 ~ 5번 셀 돌릴때 쓰는 유용한 각도 목록!
#define MIN_POS 155
#define MINUS_90 202
#define ZERO_POS 512
#define PLUS_90 822
#define MAX_POS 868

const int DXL_DIR_PIN = 2; // DYNAMIXEL Shield DIR PIN
const float DXL_PROTOCOL_VERSION = 1.0;

#define ROBOT_HOME 0
#define BOOK_RETURN 1
#define BOOK_A 2
#define BOOK_B 3
#define BOOK_C 4
#define SORT 10

#define WHEEL_RADIUS (65 * 0.5)
#define ROBOT_RADIUS (162 * 0.5)

// dp는 deg/pulse 라는 뜻
#define XL430_DP 0.088
