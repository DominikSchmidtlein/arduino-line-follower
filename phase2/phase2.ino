// PINS
#define MOTOR_L_PWM 10 // Motor B PWM Speed
#define MOTOR_L_DIR 9 // Motor B Direction
#define MOTOR_R_PWM 6 // Motor B PWM Speed
#define MOTOR_R_DIR 5 // Motor B Direction

#define TRIG_PIN_F 11
#define TRIG_PIN_R 3
#define TRIG_PIN_L 13
#define ECHO_PIN_F 12
#define ECHO_PIN_R 2
#define ECHO_PIN_L 4
#define ULTRASONIC_DEBOUNDING 10

// POWER LEVELS
#define PTURN 255
#define PSTRAIGHT 255

// POWER 

#define LEFT_TURN 1
#define RIGHT_TURN 0.9
#define LEFT_STRAIGHT 1
#define RIGHT_STRAIGHT 1

// DIRECTION SPECIFIC DELAYS
#define ALL_DELAY 5
#define FWD_DELAY 0.9
#define RVS_DELAY 4
#define LFT_DELAY 2
#define RHT_DELAY 6

// one is full tank turn, 0 is no tank turn
#define TANK_TURN_LEFT 0.85
#define TANK_TURN_RIGHT 0.85

typedef enum { FWD, RVS, STP, LFT, RHT } Dir;

Dir dir;

long duration;
int distance1;
float dLeft, dFront, dRight;

int i = 0;

void setup() {
  pinMode( MOTOR_L_DIR, OUTPUT );
  pinMode( MOTOR_L_PWM, OUTPUT );
  pinMode( MOTOR_R_DIR, OUTPUT );
  pinMode( MOTOR_R_PWM, OUTPUT );

  pinMode(TRIG_PIN_F, OUTPUT); // Sets the trigPin as an Output
  pinMode(TRIG_PIN_L, OUTPUT); // Sets the trigPin as an Output
  pinMode(TRIG_PIN_R, OUTPUT); // Sets the trigPin as an Output
  pinMode(ECHO_PIN_F, INPUT); // Sets the echoPin as an Input
  pinMode(ECHO_PIN_L, INPUT); // Sets the echoPin as an Input
  pinMode(ECHO_PIN_R, INPUT); // Sets the echoPin as an Input

  Serial.begin(9600);
  dir = FWD;
}

void loop() {
//  dFront = getDistance();
//  if (dFront > 4) { // distance in front is high
//    dLeft = getDistance();
//    dRight = getDistance();
//    if (dLeft > dRight) {
//      motor(LFT);
//    } else {
//      motor(RHT);
//    }
//  } else { // distance in front is low
//    
//  }
  dFront = getDistance(TRIG_PIN_F, ECHO_PIN_F);
  dLeft = getDistance(TRIG_PIN_L, ECHO_PIN_L);
  dRight = getDistance(TRIG_PIN_R, ECHO_PIN_R);


  Serial.print("Front: ");
  Serial.print(dFront);
  Serial.print(" Left: ");
  Serial.print(dLeft);
  Serial.print(" Right: ");
  Serial.println(dRight);
  

  // Prints the distance on the Serial Monitor
//  Serial.print("Distance: ");
//  Serial.print(distance1);
//  Serial.print("     ");
  
  //motor(STP);
//  motor(dir);
  //delay(ALL_DELAY);
}



float getDistance(int trigPin, int echoPin) {
  unsigned long duration = 0;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration= pulseIn(echoPin, HIGH);
  return duration*0.034/2;
  }

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
    default:
      digitalWrite(MOTOR_L_DIR, LOW);
      digitalWrite(MOTOR_L_PWM, LOW);
      digitalWrite(MOTOR_R_DIR, LOW);
      digitalWrite(MOTOR_R_PWM, LOW);
      break;
  }
}

