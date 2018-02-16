// PINS
#define MOTOR_L_PWM 10 // Motor B PWM Speed
#define MOTOR_L_DIR 9 // Motor B Direction
#define MOTOR_R_PWM 6 // Motor B PWM Speed
#define MOTOR_R_DIR 5 // Motor B Direction
#define LEFT_IR A0
#define RIGHT_IR A1

// POWER LEVELS
#define PTURN 255
#define PSTRAIGHT 255

// POWER MULTIPIERS
#define LEFT_TURN 0.9
#define RIGHT_TURN 1
#define LEFT_STRAIGHT 1
#define RIGHT_STRAIGHT 1


#define IR_THRESHOLD 300
#define DEBOUNCE_COUNT 15
// one is full tank turn, 0 is no tank turn
#define TANK_TURN 0.9

typedef enum { FWD, RVS, STP, LFT, RHT } Dir;
typedef enum { WHITE, BLACK } Color;

int left_ir;
int right_ir;

Dir dir;

Color left_col;
Color left_prev_col;
Color right_col;
Color right_prev_col;

int i = 0;

void setup() {
  pinMode( MOTOR_L_DIR, OUTPUT );
  pinMode( MOTOR_L_PWM, OUTPUT );
  pinMode( MOTOR_R_DIR, OUTPUT );
  pinMode( MOTOR_R_PWM, OUTPUT );

  Serial.begin(9600);
}

void loop() {
  motor(STP);
  
  left_ir = 0;
  right_ir = 0;
  for (i = 0; i < DEBOUNCE_COUNT; i++) {
    left_ir += analogRead(LEFT_IR);
    right_ir += analogRead(RIGHT_IR);
    delay(1);
  }
  left_ir /= DEBOUNCE_COUNT;
  right_ir /= DEBOUNCE_COUNT;

  left_col = read_ir(left_ir);
  right_col = read_ir(right_ir);

  dir = navigate(left_col, right_col);

  motor(dir);
  delay(10);
}

Dir navigate(Color left, Color right) {
  if (left == WHITE && right == WHITE) {
    return LFT;
  } else if (left_col == BLACK && right_col == WHITE) {
    return FWD;
  } else if (left_col == WHITE && right_col == BLACK) {
    return RHT;
  } else {
    return RHT;
  }
}

void motor(Dir dir) {
  switch(dir) {
    case FWD:
      digitalWrite(MOTOR_L_DIR, HIGH); // forward
      analogWrite(MOTOR_L_PWM, (255-PSTRAIGHT) * LEFT_STRAIGHT); // speed
      digitalWrite(MOTOR_R_DIR, HIGH); // forward
      analogWrite(MOTOR_R_PWM, (255-PSTRAIGHT) * RIGHT_STRAIGHT); // speed
      break;
    case RVS:
      digitalWrite(MOTOR_L_DIR, LOW); // backward
      analogWrite(MOTOR_L_PWM, PSTRAIGHT * LEFT_STRAIGHT); // speed
      digitalWrite(MOTOR_R_DIR, LOW); // backward
      analogWrite(MOTOR_R_PWM, PSTRAIGHT * RIGHT_STRAIGHT); // speed
      break;
    case LFT:
      digitalWrite(MOTOR_L_DIR, LOW);
      analogWrite(MOTOR_L_PWM, PTURN * LEFT_TURN * TANK_TURN);
      digitalWrite(MOTOR_R_DIR, HIGH);
      analogWrite(MOTOR_R_PWM, (255-PTURN) * RIGHT_TURN);
      break;
    case RHT:
      digitalWrite(MOTOR_L_DIR, HIGH);
      analogWrite(MOTOR_L_PWM, (255-PTURN) * LEFT_TURN);
      digitalWrite(MOTOR_R_DIR, LOW);
      analogWrite(MOTOR_R_PWM, PTURN * RIGHT_TURN * TANK_TURN);
      break;
    case STP:
      quick_stop();
    default:
      break;
  }
}

void quick_stop() {
  digitalWrite( MOTOR_L_DIR, LOW );
  digitalWrite( MOTOR_L_PWM, LOW );
  digitalWrite( MOTOR_R_DIR, LOW );
  digitalWrite( MOTOR_R_PWM, LOW );
}

Color read_ir(int reading) {
  if (reading > IR_THRESHOLD) {
    return BLACK;
  } else {
    return WHITE;
  }
}

