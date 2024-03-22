#define PWM_L 10 //M1 
#define DIR_L1 8
#define DIR_L2 12

void setup() {
  pinMode(PWM_L, OUTPUT);
  pinMode(DIR_L1, OUTPUT);
  pinMode(DIR_L2, OUTPUT);
}

void loop() {
  motor_Control(50, 2000); //ccw
  motor_Control(-50, 2000); //cw
}

void motor_Control(int pwm_1, int m_time)
{
 if(pwm_1>=0){
  analogWrite(PWM_L, pwm_1);
  digitalWrite(DIR_L1, LOW);
  digitalWrite(DIR_L2, HIGH);
 }
  else{
  analogWrite(PWM_L, abs(pwm_1));
  digitalWrite(DIR_L1, HIGH);
  digitalWrite(DIR_L2, LOW);
 }
 delay(m_time);
}