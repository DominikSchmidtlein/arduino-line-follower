#define LEFT_IR A0
#define RIGHT_IR A1

#define LEFT_M_FWD 9
#define LEFT_M_RVS 10
#define RIGHT_M_FWD 6 
#define RIGHT_M_RVS 5 

#define P100 160
#define P50 127
#define P0 0

#define IR_THRESHOLD 100

typedef enum { FWD, RVS, STP, LFT, RHT } Dir;
typedef enum { WHITE, BLACK } Color;

int left_ir;
int right_ir;

Color left_col;
Color right_col;


void setup() {
  //Setup Channel A
//  pinMode(RIGHT_M_RVS, OUTPUT); //Initiates Motor Channel A pin
//  pinMode(RIGHT_M_FWD, OUTPUT); //Initiates Brake Channel A pin
//
//  pinMode(LEFT_M_FWD, OUTPUT); //Initiates Motor Channel A pin
//  pinMode(LEFT_M_RVS, OUTPUT);
  Serial.begin(9600);
}

void loop() {

  motor(STP);
  left_ir = analogRead(LEFT_IR);
  right_ir = analogRead(RIGHT_IR);

  Serial.print(left_ir);
  Serial.print(", ");
  Serial.println(right_ir);

  left_col = read_ir(left_ir);
  right_col = read_ir(right_ir);

  if (left_col == WHITE && right_col == WHITE) {
    motor(LFT);
  } else if (left_col == BLACK && right_col == WHITE) {
    motor(FWD);
  } else if (left_col == WHITE && right_col == BLACK) {
    motor(RHT);
  } else {
    motor(RHT);
  }
  delay(50);
}

void motor(Dir dir) {
  switch(dir) {
    case FWD:
      analogWrite(LEFT_M_FWD, P100);
      analogWrite(LEFT_M_RVS, P0);
      
      analogWrite(RIGHT_M_FWD, P100);
      analogWrite(RIGHT_M_RVS, P0);
      break;
    case RVS:
      analogWrite(LEFT_M_FWD, P0);
      analogWrite(LEFT_M_RVS, P100);
      
      analogWrite(RIGHT_M_FWD, P0);
      analogWrite(RIGHT_M_RVS, P100);
      break;
    case LFT:
      analogWrite(LEFT_M_FWD, P0);
      analogWrite(LEFT_M_RVS, P50);

      analogWrite(RIGHT_M_FWD, P50);
      analogWrite(RIGHT_M_RVS, P0);
      break;
    case RHT:
      analogWrite(LEFT_M_FWD, P50);
      analogWrite(LEFT_M_RVS, P0);

      analogWrite(RIGHT_M_FWD, P0);
      analogWrite(RIGHT_M_RVS, P50);
      break;
     case STP:
     default:
      analogWrite(LEFT_M_FWD, P0);
      analogWrite(LEFT_M_RVS, P0);
      
      analogWrite(RIGHT_M_FWD, P0);
      analogWrite(RIGHT_M_RVS, P0);
      break;
  }
}

Color read_ir(int reading) {
  if (reading > IR_THRESHOLD) {
    return BLACK;
  } else {
    return WHITE;
  }
}

