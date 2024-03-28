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


unsigned long t_k = 0, t_k_1 = 0, dt = 0; //시간저장변수
int enAPos = 0; //encoder A - pulse count value
int eAcnt_k = 0, eAcnt_k_1 = 0, d_eAcnt = 0; //encoder 현재값, 과거값, 증감값 저장변수
signed long mAspeed = 0; //motor rpm

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
  analogWrite(M1_PWM,50);
  digitalWrite(M1_DIR1,HIGH);
  digitalWrite(M1_DIR2,LOW);
  digitalWrite(STBY, HIGH);
  
  t_k = millis(); //현재시간 받기
  dt = t_k - t_k_1; //현재시간-과거시간
  
  if(dt >= 50){ //dt 가 50msec 이상일때
    eAcnt_k = enAPos;
    d_eAcnt = eAcnt_k - eAcnt_k_1;
    mAspeed = d_eAcnt * 45.45; // *500/11
    mAspeed /= (signed long)dt;
    t_k_1 = t_k;
    eAcnt_k_1 = eAcnt_k;
    
    Serial.println(mAspeed);
  }
}

void enAchA_ISR(){
  if(digitalRead(ENC1_CHA) == HIGH){
    if(digitalRead(ENC1_CHB) == LOW) enAPos--;
    else enAPos++;
    }
    
  else{
    if(digitalRead(ENC1_CHB) == HIGH) enAPos--;
    else enAPos++;
    }
}


void enAchB_ISR(){
  if(digitalRead(ENC1_CHB) == HIGH){
    if(digitalRead(ENC1_CHA) == HIGH) enAPos--;
    else enAPos++;
    }
  else{
    if(digitalRead(ENC1_CHA) == LOW) enAPos--;
    else enAPos++;
    }
}
