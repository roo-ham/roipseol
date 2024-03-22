#define PWM_L 10 //M1 
#define DIR_L1 8
#define DIR_L2 12

void setup() {
  analogWriteResolution(12);
  
  pinMode(PWM_L, OUTPUT);
  pinMode(DIR_L1, OUTPUT);
  pinMode(DIR_L2, OUTPUT);
}

void loop() {
 for(int pwm = 300; pwm <= 4000; pwm +=10) {
  motor_Control(pwm, 10);
 }
 for(int pwm = 4000; pwm >= 0; pwm -=5) {
  motor_Control(pwm, 10);
 }
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