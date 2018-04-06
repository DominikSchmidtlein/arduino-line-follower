/*
   DEBUG mode can be turned on by defining DEBUG
*/
// #define DEBUG

// PINS
// Motor pins
#define MOTOR_L_PWM 10 // Motor B PWM Speed
#define MOTOR_L_DIR 9 // Motor B Direction
#define MOTOR_R_PWM 6 // Motor B PWM Speed
#define MOTOR_R_DIR 5 // Motor B Direction
// Ultrasonic sensor pins
#define TRIG_PIN_F 11
#define TRIG_PIN_R 3
#define TRIG_PIN_L 13
#define ECHO_PIN_F 12
#define ECHO_PIN_R 2
#define ECHO_PIN_L 4

// POWER LEVELS
// PTURN defines what power level, from 0-255, both wheels operate at when turning
#define PTURN 255
// PSTRAIGHT defines the power level, from 0-255, for both wheels when moving straight
#define PSTRAIGHT 255
// STRAIGHT is a control for power of all wheels in both turns and straight
#define STRAIGHT 0.9
// LEFT_TURN is the power of the left wheel in left and right turns
#define LEFT_TURN 1
// RIGHT_TURN is the power of the right wheel in left and right turns
#define RIGHT_TURN 0.9
// LEFT_STRAIGHT is the power of the left wheel in all cases
#define LEFT_STRAIGHT 0.8
// RIGHT_STRAIGHT is the power of the right wheel in all cases
#define RIGHT_STRAIGHT 1
// LEFT_YIELD is the power of the left wheeel when making a slight bend but not turning
#define LEFT_YIELD 0.6
// RIGHT_YIELD is the power of the right wheeel when making a slight bend but not turning
#define RIGHT_YIELD 0.6

// DIRECTION SPECIFIC DELAYS
// the time in ms where the robot breaks, in each iteration
#define STP_DELAY 30
// the amount of time the wheels turn forward when given go forward command
#define FWD_DELAY 50
// the amount of time the wheels turn backwards when given go backwards command
#define RVS_DELAY 4
// the amount of time the robot turns left when given go left command
#define LFT_DELAY 7
// the amount of time the robot turns right when given go right command
#define RHT_DELAY 10

// ADJUSTABLE TANK TURN
// 1 is full tank turn, 0 is no tank turn
#define TANK_TURN_LEFT 1.00
#define TANK_TURN_RIGHT 1.00
// the amount of time the robot turns in turn(LFT)
#define TANK_TURN_LEFT_DELAY 50
// the amount of time the robot turns in turn(RIGHT)
#define TANK_TURN_RIGHT_DELAY 50

// DISTANCES
// the distance in front within which the robot decides to turn
#define FRONT_THRESHOLD 8
// the distance to the side where the robot considers itself too close from the wall
#define SIDE_THRESHOLD_MIN 9
// the distance to the side where the robot considers itself too far from the wall
#define SIDE_THRESHOLD_MAX 10
// the threshold between old and new readings of sensors, below which the turn is considered complete
#define TURN_THRESHOLD 4
// the difference in distance of the front sensor to the center of rotation than the side sensors
#define FRONT_SENSOR_OFFSET 3

typedef enum { FWD, RVS, STP, LFT, RHT } Dir;

Dir dir;
float dLeft, dFront, dRight;
Dir closestWall;

void setup() {
  pinMode(MOTOR_L_DIR, OUTPUT);
  pinMode(MOTOR_L_PWM, OUTPUT);
  pinMode(MOTOR_R_DIR, OUTPUT);
  pinMode(MOTOR_R_PWM, OUTPUT);

  pinMode(TRIG_PIN_F, OUTPUT); // Sets the trigPin as an Output
  pinMode(TRIG_PIN_L, OUTPUT); // Sets the trigPin as an Output
  pinMode(TRIG_PIN_R, OUTPUT); // Sets the trigPin as an Output
  pinMode(ECHO_PIN_F, INPUT); // Sets the echoPin as an Input
  pinMode(ECHO_PIN_L, INPUT); // Sets the echoPin as an Input
  pinMode(ECHO_PIN_R, INPUT); // Sets the echoPin as an Input

  // initialize serial for debugging
  Serial.begin(9600);
  // initial direction is forward
  dir = FWD;
}

void loop() {
  // get the distance in front
  dFront = getDistance(TRIG_PIN_F, ECHO_PIN_F);
#ifdef DEBUG
  Serial.print("Front: ");
  Serial.print(dFront);
#endif
  // distance in front is high
  if (dFront > FRONT_THRESHOLD) {
    // read distance left
    dLeft = getDistance(TRIG_PIN_L, ECHO_PIN_L);
    // read distance right
    dRight = getDistance(TRIG_PIN_R, ECHO_PIN_R);
    // calculate closest wall
    closestWall = (dLeft > dRight ? RHT : LFT);
#ifdef DEBUG
    Serial.print(" Left: ");
    Serial.print(dLeft);
    Serial.print(" Right: ");
    Serial.print(dRight);
#endif
    // if the left wall is the closest and it is too close, then steer right slightly
    if (closestWall == LFT && dLeft <= SIDE_THRESHOLD_MIN) {
#ifdef DEBUG
      Serial.println(" Going Right");
#endif
      go(RHT);
      go(STP);
      delay(STP_DELAY);
      go(FWD);
    }
    // if the left wall is closest and it is too far away, then steer left slightly
    else if (closestWall == LFT && dLeft >= SIDE_THRESHOLD_MAX) {
#ifdef DEBUG
      Serial.println(" Going Left");
#endif
      go(LFT);
      go(STP);
      delay(STP_DELAY);
      go(FWD);
    }
    // if the right wall is closest and it is too close, then steer slightly left
    if (closestWall == RHT && dRight <= SIDE_THRESHOLD_MIN) {
#ifdef DEBUG
      Serial.println(" Going Left");
#endif
      go(LFT);
      go(STP);
      delay(STP_DELAY);
      go(FWD);
    }
    // if the right wall is closest and it is too far away, steer slightly right
    else if (closestWall == RHT && dRight >= SIDE_THRESHOLD_MAX) {
#ifdef DEBUG
      Serial.println(" Going Left");
#endif
      go(RHT);
      go(STP);
      delay(STP_DELAY);
      go(FWD);
    }
    // the robot is in the sweet spot, go straight
    else {
#ifdef DEBUG
      Serial.println(" Going Straight");
#endif
      go(FWD);
    }
  }
  // distance in front is low, need to turn
  else {
#ifdef DEBUG
    Serial.println(" Going Turning");
#endif
    // read distance left
    dLeft = getDistance(TRIG_PIN_L, ECHO_PIN_L);
    // read distance right
    dRight = getDistance(TRIG_PIN_R, ECHO_PIN_R);
    // if there is more space left, go left, otherwise go right
    if (dLeft > dRight) {
      turn(LFT, dFront, dLeft);
    } else {
      turn(RHT, dFront, dRight);
    }
  }
  // stop to slow down the robot
  go(STP);
  delay(STP_DELAY);
}

/*
   Read from the ultrasonic at the provided pins and return a distance in cm
*/
float getDistance(int trigPin, int echoPin) {
  unsigned long duration = 0;
  float distance = 0;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  // Timeout of 6000 us prevents a extreme delays in this blocking call
  duration = pulseIn(echoPin, HIGH, 6000);
  distance = duration * 0.034 / 2;
#ifdef DEBUG
  Serial.print(" Actual ");
  Serial.print(duration);
#endif
  if (distance > 40 || distance == 0) {
    return 60;
  }
  return distance;
}

/*
   Go is for gradual movements or adjustments. Wheels will not be moving in opposite direction.
*/
void go(Dir dir) {
  switch (dir) {
    case FWD:
      digitalWrite(MOTOR_L_DIR, HIGH); // forward
      analogWrite(MOTOR_L_PWM, (255 - PSTRAIGHT) * LEFT_STRAIGHT * STRAIGHT); // speed
      digitalWrite(MOTOR_R_DIR, HIGH); // forward
      analogWrite(MOTOR_R_PWM, (255 - PSTRAIGHT) * RIGHT_STRAIGHT * STRAIGHT); // speed
      delay(FWD_DELAY);
      break;
    case RVS:
      digitalWrite(MOTOR_L_DIR, LOW); // backward
      analogWrite(MOTOR_L_PWM, PSTRAIGHT * LEFT_STRAIGHT * STRAIGHT); // speed
      digitalWrite(MOTOR_R_DIR, LOW); // backward
      analogWrite(MOTOR_R_PWM, PSTRAIGHT * RIGHT_STRAIGHT * STRAIGHT); // speed
      delay(RVS_DELAY);
      break;
    case LFT:
      digitalWrite(MOTOR_L_DIR, LOW);
      digitalWrite(MOTOR_L_PWM, LOW);
      digitalWrite(MOTOR_R_DIR, HIGH); // forward
      analogWrite(MOTOR_R_PWM, (255 - PSTRAIGHT) * RIGHT_STRAIGHT * STRAIGHT * RIGHT_YIELD); // speed
      delay(LFT_DELAY);
      break;
    case RHT:
      digitalWrite(MOTOR_L_DIR, HIGH); // forward
      analogWrite(MOTOR_L_PWM, (255 - PTURN) * LEFT_STRAIGHT * STRAIGHT * LEFT_YIELD); // speed
      digitalWrite(MOTOR_R_DIR, LOW);
      digitalWrite(MOTOR_R_PWM, LOW);
      delay(RHT_DELAY);
      break;
    case STP:
    default:
      digitalWrite(MOTOR_L_DIR, LOW);
      digitalWrite(MOTOR_L_PWM, LOW);
      delay(5);
      digitalWrite(MOTOR_R_DIR, LOW);
      digitalWrite(MOTOR_R_PWM, LOW);
      break;
  }
}
/*
   Turn is for sharp turns. Wheels rotate in opposite direction for tank turn effect.
   Turn until the front sensor matches the previous long side and the short side matches the previous front.
   This help make more accurate 90 degree turns than timed turns.
*/
void turn(Dir dir, float curFront, float curLongSide ) {
  float tdLeft, tdFront, tdRight;
  bool turning = true;
  int maxCount = 0;
  while (turning && maxCount++ < 50) {
    switch (dir) {
      case LFT:
        digitalWrite(MOTOR_L_DIR, LOW);
        analogWrite(MOTOR_L_PWM, PTURN * LEFT_TURN * TANK_TURN_LEFT);
        digitalWrite(MOTOR_R_DIR, HIGH);
        analogWrite(MOTOR_R_PWM, (255 - PTURN) * RIGHT_TURN);
        delay(TANK_TURN_LEFT_DELAY);
        tdFront = getDistance(TRIG_PIN_F, ECHO_PIN_F) + FRONT_SENSOR_OFFSET;
        tdRight = getDistance(TRIG_PIN_R, ECHO_PIN_R);
        if (abs(tdFront - curLongSide) <= TURN_THRESHOLD && abs(tdRight - curFront) <= TURN_THRESHOLD) {
          turning = false;
        }
        break;
      case RHT:
        digitalWrite(MOTOR_L_DIR, HIGH);
        analogWrite(MOTOR_L_PWM, (255 - PTURN) * LEFT_TURN);
        digitalWrite(MOTOR_R_DIR, LOW);
        analogWrite(MOTOR_R_PWM, PTURN * RIGHT_TURN * TANK_TURN_RIGHT);
        delay(TANK_TURN_RIGHT_DELAY);
        tdFront = getDistance(TRIG_PIN_F, ECHO_PIN_F) + FRONT_SENSOR_OFFSET;
        tdLeft = getDistance(TRIG_PIN_L, ECHO_PIN_L);
        if (abs(tdFront - curLongSide) <= TURN_THRESHOLD && abs(tdLeft - curFront) <= TURN_THRESHOLD) {
          turning = false;
        }
        break;
      default:
        break;
    }
  }
}

