#define MOTOR_L_PWM 10 // Motor B PWM Speed
#define MOTOR_L_DIR 9 // Motor B Direction
#define MOTOR_R_PWM 6 // Motor B PWM Speed
#define MOTOR_R_DIR 5 // Motor B Direction

#define DIR_DELAY 1000

#define LEFT_IR A0
#define RIGHT_IR A1

#define P100 250
#define P50 127
#define P0 0

#define IR_THRESHOLD 300

#define TANK_TURN false

typedef enum { FWD, RVS, STP, LFT, RHT } Dir;
typedef enum { WHITE, BLACK } Color;

int left_ir;
int right_ir;

Color left_col;
Color right_col;


void setup() {
  pinMode( MOTOR_L_DIR, OUTPUT );
  pinMode( MOTOR_L_PWM, OUTPUT );
  pinMode( MOTOR_R_DIR, OUTPUT );
  pinMode( MOTOR_R_PWM, OUTPUT );

  Serial.begin(9600);
}

void loop() {

  motor(STP, TANK_TURN);
  left_ir = analogRead(LEFT_IR);
  right_ir = analogRead(RIGHT_IR);

  Serial.println(left_ir);
//  Serial.print(", ");
//  Serial.println(right_ir);

  left_col = read_ir(left_ir);
  right_col = read_ir(right_ir);

  if (left_col == WHITE && right_col == WHITE) {
    motor(FWD, TANK_TURN);
  } else if (left_col == BLACK && right_col == WHITE) {
    motor(LFT, TANK_TURN);
  } else if (left_col == WHITE && right_col == BLACK) {
    motor(RHT, TANK_TURN);
  } else {
    motor(STP, TANK_TURN);
  }
  delay(40);
  quick_stop();
  delay(80);
}

void motor(Dir dir, boolean tankTurn) {
  switch(dir) {
    case FWD:
      // set the motor speed and direction
      digitalWrite( MOTOR_L_DIR, HIGH ); // direction = forward
      analogWrite( MOTOR_L_PWM, 255-P100 ); // PWM speed = fast
      digitalWrite( MOTOR_R_DIR, HIGH ); // direction = forward
      analogWrite( MOTOR_R_PWM, 255-P100 ); // PWM speed = fast
      break;
    case RVS:
      // set the motor speed and direction
      digitalWrite( MOTOR_L_DIR, LOW ); // direction = forward
      analogWrite( MOTOR_L_PWM, P100 ); // PWM speed = fast
      digitalWrite( MOTOR_R_DIR, LOW ); // direction = forward
      analogWrite( MOTOR_R_PWM, P100 ); // PWM speed = fast
      break;
    case LFT:
      // set the motor speed and direction
      if (tankTurn) {
        digitalWrite( MOTOR_L_DIR, LOW ); // direction = forward
        analogWrite( MOTOR_L_PWM, P100 ); // PWM speed = fast
      }
      digitalWrite( MOTOR_R_DIR, HIGH ); // direction = forward
      analogWrite( MOTOR_R_PWM, 255-P100 ); // PWM speed = fast
      break;
    case RHT:
      // set the motor speed and direction
      digitalWrite( MOTOR_L_DIR, HIGH ); // direction = forward
      analogWrite( MOTOR_L_PWM, 255-P100 ); // PWM speed = fast
      if (tankTurn) {
        digitalWrite( MOTOR_R_DIR, LOW ); // direction = forward
        analogWrite( MOTOR_R_PWM, P100 ); // PWM speed = fast
      }
      break;
     case STP:
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

