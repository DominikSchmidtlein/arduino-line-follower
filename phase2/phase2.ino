// PINS
#define MOTOR_L_PWM 10 // Motor B PWM Speed
#define MOTOR_L_DIR 9 // Motor B Direction
#define MOTOR_R_PWM 6 // Motor B PWM Speed
#define MOTOR_R_DIR 5 // Motor B Direction

#define TRIG_PIN1 11
#define TRIG_PIN2 13
#define ECHO_PIN1 12
#define ECHO_PIN2 2
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
int distance2;

int i = 0;

void setup() {
  pinMode( MOTOR_L_DIR, OUTPUT );
  pinMode( MOTOR_L_PWM, OUTPUT );
  pinMode( MOTOR_R_DIR, OUTPUT );
  pinMode( MOTOR_R_PWM, OUTPUT );

  pinMode(TRIG_PIN1, OUTPUT); // Sets the trigPin as an Output
  pinMode(TRIG_PIN2, OUTPUT); // Sets the trigPin as an Output
  pinMode(ECHO_PIN1, INPUT); // Sets the echoPin as an Input
  pinMode(ECHO_PIN2, INPUT); // Sets the echoPin as an Input

  Serial.begin(9600);
  dir = FWD;
}

void loop() {

  
  // Calculating the distance
  distance1= getDistance(TRIG_PIN1,ECHO_PIN1);
  distance2= getDistance(TRIG_PIN2,ECHO_PIN2);

  // Prints the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance1);
  Serial.print("     ");
  Serial.println(distance2);
  
  //motor(STP);
//  motor(dir);
  //delay(ALL_DELAY);
}



int getDistance(int trigPin, int echoPin) {
  int duration = 0;
  for(int i = 0; i < ULTRASONIC_DEBOUNDING; i++) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration += pulseIn(echoPin, HIGH);
  }
  // Calculating the distance
  return duration/ULTRASONIC_DEBOUNDING*0.034/2;

  
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

