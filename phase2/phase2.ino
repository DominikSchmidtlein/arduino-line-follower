//#define DEBUG

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
#define STRAIGHT 0.9
#define LEFT_TURN 1
#define RIGHT_TURN 0.9



#define LEFT_STRAIGHT 0.8
#define RIGHT_STRAIGHT 1
#define LEFT_YIELD 0.6
#define RIGHT_YIELD 0.6

// DIRECTION SPECIFIC DELAYS
#define STP_DELAY 30
#define FWD_DELAY 50
#define RVS_DELAY 4
#define LFT_DELAY 7
#define RHT_DELAY 10

#define READ_DELAY 5

// one is full tank turn, 0 is no tank turn
#define TANK_TURN_LEFT 1.00
#define TANK_TURN_LEFT_DELAY 30
#define TANK_TURN_RIGHT 1.00
#define TANK_TURN_RIGHT_DELAY 30

// DISTANCES
#define THRESHOLD_BUFFER 3
#define FRONT_THRESHOLD 8
#define SIDE_THRESHOLD_MIN 9
#define SIDE_THRESHOLD_MAX 10
#define TURN_THRESHOLD 5
#define FRONT_SENSOR_OFFSET 3

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
  Dir closestWall;
  dFront = getDistance(TRIG_PIN_F, ECHO_PIN_F);
#ifdef DEBUG
  Serial.print("Front: ");
  Serial.print(dFront);
#endif
  if (dFront > FRONT_THRESHOLD) { // distance in front is high
    dLeft = getDistance(TRIG_PIN_L, ECHO_PIN_L);
    dRight = getDistance(TRIG_PIN_R, ECHO_PIN_R);
    closestWall = (dLeft > dRight ? RHT : LFT);
#ifdef DEBUG
    Serial.print(" Left: "); 
    Serial.print(dLeft);
    Serial.print(" RIght: "); 
    Serial.print(dRight);
#endif
    if(closestWall == LFT && dLeft <= SIDE_THRESHOLD_MIN) {
#ifdef DEBUG
      Serial.println(" Going Right");
#endif
      go(RHT);
      go(STP);
      delay(STP_DELAY);
      go(FWD);
    }
    else if(closestWall == LFT && dLeft >= SIDE_THRESHOLD_MAX) {

#ifdef DEBUG
      Serial.println(" Going Left");
#endif
      go(LFT);
      go(STP);
      delay(STP_DELAY);
      go(FWD);
    }
    if(closestWall == RHT && dRight <= SIDE_THRESHOLD_MIN) {
#ifdef DEBUG
      Serial.println(" Going Left");
#endif
      go(LFT);
      go(STP);
      delay(STP_DELAY);
      go(FWD);
    }
    else if(closestWall == RHT && dRight >= SIDE_THRESHOLD_MAX) {

#ifdef DEBUG
      Serial.println(" Going Left");
#endif
      go(RHT);
      go(STP);
      delay(STP_DELAY);
      go(FWD);
    }
    
    
    else {

#ifdef DEBUG
      Serial.println(" Going Straight");
#endif
      go(FWD);
    }
  } else { // distance in front is low
#ifdef DEBUG
      Serial.println(" Going Turning");
#endif
    dLeft = getDistance(TRIG_PIN_L, ECHO_PIN_L);
    dRight = getDistance(TRIG_PIN_R, ECHO_PIN_R);
    //turn(dLeft > dRight ? LFT : RHT);
    if(dLeft > dRight){
     turn(LFT, dFront, dLeft);
    }else{
      turn(RHT, dFront, dRight);
    }  
  }
  
  go(STP);
  delay(STP_DELAY);
}



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
  duration = pulseIn(echoPin, HIGH,6000);
  distance = duration*0.034/2;
#ifdef DEBUG
  Serial.print(" Actual ");
  Serial.print(duration); 
#endif
  if(distance > 40 || distance == 0) {
    return 60;
  }
  return distance;
}

void go(Dir dir) {
  switch(dir) {
    case FWD:
      digitalWrite(MOTOR_L_DIR, HIGH); // forward
      analogWrite(MOTOR_L_PWM, (255-PSTRAIGHT) * LEFT_STRAIGHT * STRAIGHT); // speed
      digitalWrite(MOTOR_R_DIR, HIGH); // forward
      analogWrite(MOTOR_R_PWM, (255-PSTRAIGHT) * RIGHT_STRAIGHT * STRAIGHT); // speed
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
      analogWrite(MOTOR_R_PWM, (255-PSTRAIGHT) * RIGHT_STRAIGHT * STRAIGHT * RIGHT_YIELD); // speed
//      digitalWrite(MOTOR_L_DIR, HIGH); // forward
//      analogWrite(MOTOR_L_PWM, (255-PTURN) * LEFT_STRAIGHT * STRAIGHT * LEFT_YIELD); // speed
//      digitalWrite(MOTOR_R_DIR, HIGH);
//      analogWrite(MOTOR_R_PWM, (255-PTURN) * RIGHT_STRAIGHT * STRAIGHT);
      delay(LFT_DELAY);
      break;
    case RHT:
      digitalWrite(MOTOR_L_DIR, HIGH); // forward
      analogWrite(MOTOR_L_PWM, (255-PTURN) * LEFT_STRAIGHT * STRAIGHT * LEFT_YIELD); // speed
      digitalWrite(MOTOR_R_DIR, LOW);
      digitalWrite(MOTOR_R_PWM, LOW);
    
//      digitalWrite(MOTOR_L_DIR, HIGH);
//      analogWrite(MOTOR_L_PWM, (255-PTURN) * LEFT_STRAIGHT * STRAIGHT);
//      digitalWrite(MOTOR_R_DIR, HIGH); // forward
//      analogWrite(MOTOR_R_PWM, (255-PSTRAIGHT) * RIGHT_STRAIGHT * STRAIGHT * RIGHT_YIELD); // speed
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

   //dFront = getDistance(TRIG_PIN_F, ECHO_PIN_F);
   // dLeft = getDistance(TRIG_PIN_L, ECHO_PIN_L);
   // dRight = getDistance(TRIG_PIN_R, ECHO_PIN_R);


void turn(Dir dir, float curFront, float curLongSide ) {
  float tdLeft, tdFront, tdRight;
  bool turning = true; 
  while(turning){ 
    switch(dir) {
      case LFT:
        digitalWrite(MOTOR_L_DIR, LOW);
        analogWrite(MOTOR_L_PWM, PTURN * LEFT_TURN * TANK_TURN_LEFT);
        digitalWrite(MOTOR_R_DIR, HIGH);
        analogWrite(MOTOR_R_PWM, (255-PTURN) * RIGHT_TURN);
        delay(TANK_TURN_LEFT_DELAY);
        tdFront = getDistance(TRIG_PIN_F, ECHO_PIN_F) + FRONT_SENSOR_OFFSET;        
        tdRight = getDistance(TRIG_PIN_R, ECHO_PIN_R);
        if(abs(tdFront - curLongSide) < TURN_THRESHOLD && abs(tdRight - curFront) < TURN_THRESHOLD){
          turning = false; 
        }
        break;
      case RHT:
        digitalWrite(MOTOR_L_DIR, HIGH);
        analogWrite(MOTOR_L_PWM, (255-PTURN) * LEFT_TURN);
        digitalWrite(MOTOR_R_DIR, LOW);
        analogWrite(MOTOR_R_PWM, PTURN * RIGHT_TURN * TANK_TURN_RIGHT);
        delay(TANK_TURN_RIGHT_DELAY);
        tdFront = getDistance(TRIG_PIN_F, ECHO_PIN_F) + FRONT_SENSOR_OFFSET;        
        tdLeft = getDistance(TRIG_PIN_L, ECHO_PIN_L);
        if(abs(tdFront - curLongSide) < TURN_THRESHOLD && abs(tdLeft - curFront) < TURN_THRESHOLD){
          turning = false; 
        }
        
        break;
      default:
        break;
    }
  }
}

