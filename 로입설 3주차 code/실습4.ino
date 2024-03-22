#define PWM_L 10 //M1 
#define DIR_L1 8
#define DIR_L2 12
#define PWM_R 9 //M2 
#define DIR_R1 6
#define DIR_R2 7

void setup() {
  analogWriteResolution(12);
  analogReadResolution(12);
  
  pinMode(PWM_L, OUTPUT);
  pinMode(DIR_L1, OUTPUT);
  pinMode(DIR_L2, OUTPUT);
  pinMode(PWM_R, OUTPUT);
  pinMode(DIR_R1, OUTPUT);
  pinMode(DIR_R2, OUTPUT);
}

void loop() {
  int pwm = analogRead(A0);
  motor_Control(pwm, 10);
 /*for(int pwm = 300; pwm <= 4000; pwm +=10) {
 }
 for(int pwm = 4000; pwm >= 0; pwm -=5) {
 }*/
}

void motor_Control(int pwm_1, int m_time)
{
 if(pwm_1>=0){
  analogWrite(PWM_L, pwm_1);
  digitalWrite(DIR_L1, LOW);
  digitalWrite(DIR_L2, HIGH);
  analogWrite(PWM_R, pwm_1);
  digitalWrite(DIR_R1, HIGH);
  digitalWrite(DIR_R2, LOW);
 }
  else{
  analogWrite(PWM_L, abs(pwm_1));
  digitalWrite(DIR_L1, HIGH);
  digitalWrite(DIR_L2, LOW);
  analogWrite(PWM_R, abs(pwm_1));
  digitalWrite(DIR_R1, LOW);
  digitalWrite(DIR_R2, HIGH);
 }
 delay(m_time);
}