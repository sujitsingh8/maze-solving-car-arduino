// ================= PIN DEFINITIONS =================

// Ultrasonic Sensors
#define trigFront 2
#define echoFront 3

#define trigLeft 4
#define echoLeft 7

#define trigRight 12
#define echoRight 10

// Motor Driver (L298N)
#define ENA 5
#define ENB 6

#define IN1 8
#define IN2 9
#define IN3 11
#define IN4 13

// ================= SETTINGS =================

#define BASE_SPEED 210
#define TURN_SPEED 200
#define SLOW_SPEED 170

#define WALL_DIST 17
#define IDEAL_LEFT 13

#define FRONT_LIMIT 18
#define FRONT_BRAKE 26

#define KP 1.7

// ================= DISTANCE =================

int getDistance(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);

  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  long duration = pulseIn(echo, HIGH, 15000); 
  int distance = duration * 0.034 / 2;

  if (distance == 0) return 80;
  return distance;
}

// ================= MOTOR =================

void moveForward(int leftSpeed, int rightSpeed) {
  leftSpeed = constrain(leftSpeed, 130, 255);
  rightSpeed = constrain(rightSpeed, 130, 255);

  analogWrite(ENA, leftSpeed);
  analogWrite(ENB, rightSpeed);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void stopBot() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// ================= TURNING =================

void turnLeft() {
  // --- CHANGE 1: Clear the corner before turning ---
  moveForward(BASE_SPEED, BASE_SPEED);
  delay(150); // Adjust this (100-200ms) based on your car's length
  stopBot();
  delay(50);
  // ------------------------------------------------

  analogWrite(ENA, TURN_SPEED);
  analogWrite(ENB, TURN_SPEED);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  delay(180);

  while (getDistance(trigFront, echoFront) < 20);
}

void turnRight() {
  moveForward(150,150);
  delay(50);

  analogWrite(ENA, TURN_SPEED);
  analogWrite(ENB, TURN_SPEED);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  delay(180);

  while (getDistance(trigFront, echoFront) < 20);
}

void turnBack() {
  analogWrite(ENA, TURN_SPEED);
  analogWrite(ENB, TURN_SPEED);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  delay(360);
}

// ================= SETUP =================

void setup() {
  pinMode(trigFront, OUTPUT);
  pinMode(echoFront, INPUT);
  pinMode(trigLeft, OUTPUT);
  pinMode(echoLeft, INPUT);
  pinMode(trigRight, OUTPUT);
  pinMode(echoRight, INPUT);

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  
  delay(2000); // 2-second safety delay to place the car down
}

// ================= LOOP =================

void loop() {
  int front = getDistance(trigFront, echoFront);
  int left  = getDistance(trigLeft, echoLeft);
  int right = getDistance(trigRight, echoRight);

  // LEFT HAND RULE logic
  if (left > WALL_DIST) {
    turnLeft();
  }
  else if (front > FRONT_LIMIT) {
    int speed = BASE_SPEED;
    if (front < FRONT_BRAKE) {
      speed = SLOW_SPEED;
    }

    int error = IDEAL_LEFT - left;
    int correction = error * KP;

    if (right < 10) correction -= 6;

    int leftSpeed  = speed - correction;
    int rightSpeed = speed + correction;

    moveForward(leftSpeed, rightSpeed);
  }
  else if (right > WALL_DIST) {
    turnRight();
  }
  else if (front < 12 && left < 12 && right < 12) {
    turnBack();
  }
  else {
    turnRight(); 
  }

  delay(5);
}
