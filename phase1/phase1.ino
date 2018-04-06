// PINS
// motor pins
#define MOTOR_L_PWM 10 // Motor B PWM Speed
#define MOTOR_L_DIR 9 // Motor B Direction
#define MOTOR_R_PWM 6 // Motor B PWM Speed
#define MOTOR_R_DIR 5 // Motor B Direction
// infrared sensor pins
#define LEFT_IR A0
#define RIGHT_IR A1

// POWER LEVELS
// the power of both wheels in turns
#define PTURN 255
// the power of both wheels when going straight
#define PSTRAIGHT 255
// the power of the left wheel when turning
#define LEFT_TURN 1
// the power of the right wheel when turning
#define RIGHT_TURN 0.9
// the power of the left wheel when going straight
#define LEFT_STRAIGHT 1
// the power of the right wheel when going straight
#define RIGHT_STRAIGHT 1

// DIRECTION SPECIFIC DELAYS
// delay after moving in every direction
#define ALL_DELAY 5
// delay after going forwards only
#define FWD_DELAY 0.9
// delay after going backwards only
#define RVS_DELAY 4
// delay after going left only
#define LFT_DELAY 2
// delay after going right only
#define RHT_DELAY 6

// the threshold for determining black vs white under IR sensor
#define IR_THRESHOLD 500
// the number of reads to IR sensors when debouncing
#define DEBOUNCE_COUNT 10

// TANK TURN
// one is full tank turn, 0 is no tank turn
#define TANK_TURN_LEFT 1
// when turning right there is a slight bias for the forward wheel so we always make some progress forwards
#define TANK_TURN_RIGHT 0.9

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
  Serial.println("Hello!");
}

void loop() {
  Serial.println("Start of loop!");
  motor(STP);
  
  left_ir = 0;
  right_ir = 0;
  // make a series o readings to the IR sensors
  for (i = 0; i < DEBOUNCE_COUNT; i++) {
    left_ir += analogRead(LEFT_IR);
    right_ir += analogRead(RIGHT_IR);
    delay(1);
  }
  // take average of readings
  left_ir /= DEBOUNCE_COUNT;
  right_ir /= DEBOUNCE_COUNT;
  
  // interpret ir readings as black or white
  left_col = read_ir(left_ir);
  right_col = read_ir(right_ir);

  // determine next direction based on left and right colors
  dir = navigateLeft(left_col, right_col);
  printDir(dir);
  // engage motors in correct direction
  motor(dir);
  delay(ALL_DELAY);
}

/*
 * Make decision on how to move based on left and right sensor readings. Left hand rule implemented here.
 */
Dir navigateLeft(Color left, Color right) {
  if (left == WHITE && right == WHITE) {
    return RHT;
  } else {
    return LFT;
  }
}

/*
 * Makes motors move in a direction for a specific amount of time
 */
void motor(Dir dir) {
  switch(dir) {
    case FWD:
      digitalWrite(MOTOR_L_DIR, HIGH); // forward
      analogWrite(MOTOR_L_PWM, (255-PSTRAIGHT) * LEFT_STRAIGHT); // speed
      digitalWrite(MOTOR_R_DIR, HIGH); // forward
      analogWrite(MOTOR_R_PWM, (255-PSTRAIGHT) * RIGHT_STRAIGHT); // speed
      delay(FWD_DELAY);
      break;
    case RVS:
      digitalWrite(MOTOR_L_DIR, LOW); // backward
      analogWrite(MOTOR_L_PWM, PSTRAIGHT * LEFT_STRAIGHT); // speed
      digitalWrite(MOTOR_R_DIR, LOW); // backward
      analogWrite(MOTOR_R_PWM, PSTRAIGHT * RIGHT_STRAIGHT); // speed
      delay(RVS_DELAY);
      break;
    case LFT:
      digitalWrite(MOTOR_L_DIR, LOW);
      analogWrite(MOTOR_L_PWM, PTURN * LEFT_TURN * TANK_TURN_LEFT);
      digitalWrite(MOTOR_R_DIR, HIGH);
      analogWrite(MOTOR_R_PWM, (255-PTURN) * RIGHT_TURN);
      delay(LFT_DELAY);
      break;
    case RHT:
      digitalWrite(MOTOR_L_DIR, HIGH);
      analogWrite(MOTOR_L_PWM, (255-PTURN) * LEFT_TURN);
      digitalWrite(MOTOR_R_DIR, LOW);
      analogWrite(MOTOR_R_PWM, PTURN * RIGHT_TURN * TANK_TURN_RIGHT);
      delay(RHT_DELAY);
      break;
    case STP:
      digitalWrite( MOTOR_L_DIR, LOW );
      digitalWrite( MOTOR_L_PWM, LOW );
      digitalWrite( MOTOR_R_DIR, LOW );
      digitalWrite( MOTOR_R_PWM, LOW );
    default:
      break;
  }
}

/* 
 *  Interpret IR reading as black or white depending on threshold
 */
Color read_ir(int reading) {
  if (reading > IR_THRESHOLD) {
    return BLACK;
  } else {
    return WHITE;
  }
}

void printDir(Dir dir) {
  switch(dir) {
    case FWD:
      Serial.println("FWD");
      break;
    case RVS:
      Serial.println("RVS");
      break;
    case STP:
      Serial.println("STP");
      break;
    case LFT:
      Serial.println("LFT");
      break;
    case RHT:
      Serial.println("RHT");
      break;
  }
}

