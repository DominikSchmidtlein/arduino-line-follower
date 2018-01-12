#define LEFT_IR 8
#define RIGHT_IR 7

#define LEFT_M_FWD 9
#define LEFT_M_RVS 10
#define RIGHT_M_FWD 6 
#define RIGHT_M_RVS 5 

typedef enum { FWD, RVS, STP, LFT, RHT } Dir;

int left_ir;
int right_ir;


void setup() {
  //Setup Channel A
  pinMode(RIGHT_M_RVS, OUTPUT); //Initiates Motor Channel A pin
  pinMode(RIGHT_M_FWD, OUTPUT); //Initiates Brake Channel A pin

  pinMode(LEFT_M_FWD, OUTPUT); //Initiates Motor Channel A pin
  pinMode(LEFT_M_RVS, OUTPUT);
  Serial.begin(9600);
}

void loop() {

  left_ir = digitalRead(LEFT_IR);
  right_ir = digitalRead(RIGHT_IR);

  Serial.print(left_ir);
  Serial.print(", ");
  Serial.println(right_ir);

  if ((left_ir | right_ir) == 0) {
    motor(FWD);
  } else if (left_ir && !right_ir) {
    motor(LFT);
  } else if (!left_ir && right_ir) {
    motor(RHT);
  } else {
    motor(STP);
  }
}

void motor(Dir dir) {
  switch(dir) {
    case FWD:
      digitalWrite(LEFT_M_FWD, HIGH);
      digitalWrite(LEFT_M_RVS, LOW);
      
      digitalWrite(RIGHT_M_FWD, HIGH);
      digitalWrite(RIGHT_M_RVS, LOW);
      break;
    case RVS:
      digitalWrite(LEFT_M_FWD, LOW);
      digitalWrite(LEFT_M_RVS, HIGH);
      
      digitalWrite(RIGHT_M_FWD, LOW);
      digitalWrite(RIGHT_M_RVS, HIGH);
      break;
    case LFT:
      digitalWrite(LEFT_M_FWD, LOW);
      digitalWrite(LEFT_M_RVS, HIGH);

      digitalWrite(RIGHT_M_FWD, HIGH);
      digitalWrite(RIGHT_M_RVS, LOW);
      break;
    case RHT:
      digitalWrite(LEFT_M_FWD, HIGH);
      digitalWrite(LEFT_M_RVS, LOW);

      digitalWrite(RIGHT_M_FWD, LOW);
      digitalWrite(RIGHT_M_RVS, HIGH);
      break;
     case STP:
     default:
      digitalWrite(LEFT_M_FWD, LOW);
      digitalWrite(LEFT_M_RVS, LOW);
      
      digitalWrite(RIGHT_M_FWD, LOW);
      digitalWrite(RIGHT_M_RVS, LOW);
      break;
  }
}

