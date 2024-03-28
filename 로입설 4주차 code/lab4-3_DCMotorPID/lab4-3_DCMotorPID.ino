//AIRO DC Motor Driver V2.0

//*pin layout
//------------------------------------------------------------
//           |   dir1    dir2    ENCA    ENCB    PWM    STBY
//------------------------------------------------------------
//  Motor1   |   PB12    PB13    PB06    PB07    PB0    PA8
//------------------------------------------------------------
#include "BlackpillTimer.h"


#define ENC1_CHA  PB6
#define ENC1_CHB  PB7
#define M1_PWM    PB0
#define M1_DIR1   PB12
#define M1_DIR2   PB13
#define STBY      PA8

BlackpillTimer myTimer(TIM1);

float Kp = 1;
float Ki = 0;
float Kd = 0;

int e1cnt=0;
int e1cnt_k=0, e1cnt_k_1=0, d_e1cnt=0;

float m1speed=0;
float m1_ref_spd = 0;
float m1_err_spd = 0;
float m1_err_spd_k_1 = 0;
float m1_derr_spd = 0;
float m1_err_sum = 0;
float ctrl_up = 0;
float ctrl_ui = 0;
float ctrl_ud = 0;

int ctrl_u = 0;
int ipwm_u = 0;

bool t2_flag = 0;
char sec = NULL;


void setup(){
  pinMode(ENC1_CHA, INPUT_PULLUP);
  pinMode(ENC1_CHB, INPUT_PULLUP);
  pinMode(M1_DIR1, OUTPUT);
  pinMode(M1_DIR2, OUTPUT);
  pinMode(M1_PWM, OUTPUT);
  pinMode(STBY, OUTPUT);
  
  attachInterrupt(digitalPinToInterrupt(ENC1_CHA), Enc1chA_ISR,CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC1_CHB), Enc1chB_ISR,CHANGE);
  
  digitalWrite(M1_DIR1, LOW);
  digitalWrite(M1_DIR2, HIGH);
  
  analogWrite(M1_PWM, 0);
  
  Serial.begin(115200);
  Serial.setTimeout(50);
  Serial.println("m1_ref_spd, m1speed");
  
  myTimer.setIntervalMs(50);
  myTimer.attachUpdateInterrupt(t2_ISR);
  myTimer.start();

}

void loop(){
  if(Serial.available()){
    sec = (char)Serial.read();
    
    if(sec == 'S')
      m1_ref_spd = Serial.parseFloat();
    else if(sec == 'P')
      Kp = Serial.parseFloat();
    else if(sec == 'I')
      Ki = Serial.parseFloat();
    else if(sec == 'D')
      Kd = Serial.parseFloat();
      
    sec = NULL;
  }
  
  if(t2_flag){
    t2_flag = 0;
    m1speed = d_e1cnt * 10/11; // *500/11/50
    //Error
    m1_err_spd = m1_ref_spd - m1speed;
    m1_derr_spd = m1_err_spd - m1_err_spd_k_1;
    m1_err_sum = m1_err_sum + m1_err_spd;
    m1_err_spd_k_1 = m1_err_spd;
    //PID-Controller
    ctrl_up = Kp * m1_err_spd;
    ctrl_ui = Ki * m1_err_sum;
    ctrl_ud = Kd * m1_derr_spd;
    ctrl_u = (int)(ctrl_up + ctrl_ud + ctrl_ui);
    
    if(ctrl_u >= 0){
      digitalWrite(M1_DIR1, HIGH);
      digitalWrite(M1_DIR2, LOW); //ccw
      digitalWrite(STBY, HIGH);
      
      if(ctrl_u > 255) 
        ipwm_u = 255;
      else 
        ipwm_u = (int)ctrl_u;
    }
    else{
      digitalWrite(M1_DIR1, LOW);
      digitalWrite(M1_DIR2, HIGH); //cw
      digitalWrite(STBY, HIGH);
      
      if(ctrl_u < -255) 
        ipwm_u = 255;
      else 
        ipwm_u = (int)ctrl_u * (-1);
    } 
    analogWrite(M1_PWM, ipwm_u);
    Serial.print(m1_ref_spd);
    Serial.print(",");
    Serial.println(m1speed);
  }
}

void t2_ISR(){
  t2_flag = 1;
  e1cnt_k = e1cnt;
  d_e1cnt = e1cnt_k - e1cnt_k_1; //delta_error
  e1cnt_k_1 = e1cnt_k;
}

void Enc1chA_ISR(){
  if(digitalRead(ENC1_CHA) == HIGH){
    if(digitalRead(ENC1_CHB) == LOW) 
      e1cnt--;
    else 
      e1cnt++;
    }
  else{
  if(digitalRead(ENC1_CHB) == HIGH) 
    e1cnt--;
  else 
    e1cnt++;
  }
}

void Enc1chB_ISR(){
  if(digitalRead(ENC1_CHB) == HIGH){
    if(digitalRead(ENC1_CHA) == HIGH) 
      e1cnt--;
    else 
      e1cnt++;
    }
  else{
    if(digitalRead(ENC1_CHA) == LOW) 
      e1cnt--;
    else 
      e1cnt++;
    }
}
