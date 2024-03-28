//AIRO DC Motor Driver V2.0

//*pin layout
//------------------------------------------------------------
//           |   dir1    dir2    ENCA    ENCB    PWM    STBY
//------------------------------------------------------------
//  Motor1   |   PB12    PB13    PB06    PB07    PB0    PA8
//------------------------------------------------------------




#define ENC1_CHA  PB6
#define ENC1_CHB  PB7
#define M1_PWM    PB0
#define M1_DIR1   PB12
#define M1_DIR2   PB13
#define STBY      PA8


int enAPos =0;
int in_mval= 0;
float angle =0.0;


void setup(){
  pinMode(M1_DIR1,OUTPUT);
  pinMode(M1_DIR2,OUTPUT);
  pinMode(M1_PWM,OUTPUT);
  pinMode(ENC1_CHA,INPUT_PULLUP);
  pinMode(ENC1_CHB,INPUT_PULLUP);
  pinMode(STBY, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(ENC1_CHA),enAchA_ISR,CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC1_CHB),enAchB_ISR,CHANGE);
  Serial.begin(115200);
  
  digitalWrite(STBY, LOW);
}


void loop(){
  if(Serial.available()){
    in_mval = Serial.parseInt();
    enAPos=0 ; 
  }
  motor_go(in_mval);
  angle =enAPos % 1320;
  
  Serial.print("pwm : ");
  Serial.print(in_mval);
  
  Serial.print(", pulses : ");
  Serial.print(enAPos);
  
  Serial.print(" rotation : ");
  Serial.print(enAPos/1320);
  
  Serial.print(" angle : ");
  Serial.print(angle/1320.0*360.0);
  
  Serial.println(" ");
  
}


void motor_go(int spd_val){
  if(spd_val >=0){
    analogWrite(M1_PWM,spd_val);
    digitalWrite(M1_DIR1,HIGH);
    digitalWrite(M1_DIR2,LOW);
    digitalWrite(STBY, HIGH);
  }
  else{
    analogWrite(M1_PWM, -spd_val);
    digitalWrite(M1_DIR1,LOW);
    digitalWrite(M1_DIR2,HIGH); 
    digitalWrite(STBY, HIGH);
  }
}


void enAchA_ISR(){ //chA에서 핀 변화(0 또는 1로 변했을 때) 발생
  // a low-to-high edge on channel A
  if(digitalRead(ENC1_CHA) == HIGH) { //상태 00->10 또는 01->11
    if(digitalRead(ENC1_CHB) == LOW) enAPos = enAPos - 1; //상태 00->10(cw)
      else enAPos = enAPos + 1; //상태 01->11(ccw)
      }
      // must be a high-to-low edge on channel A  
    else {
      if(digitalRead(ENC1_CHB) == HIGH) enAPos = enAPos - 1;
      else enAPos = enAPos + 1;
    }
}

void enAchB_ISR(){ //chB에서 핀 변화(0 또는 1로 변했을 때) 발생
  // a low-to-high edge on channel B
  if(digitalRead(ENC1_CHB) == HIGH) {
    if(digitalRead(ENC1_CHA) == HIGH) enAPos = enAPos - 1;
      else enAPos = enAPos +1;
      }
      // must be a high-to-low edge on channel B
    else {
      if(digitalRead(ENC1_CHA) == LOW) enAPos = enAPos - 1;
      else enAPos = enAPos + 1;
      }
}
