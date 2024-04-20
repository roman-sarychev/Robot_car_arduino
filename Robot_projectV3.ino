#define TRIGGER_TIME 10
#define MAX_DISTANCE 30 // Distance threshold for obstacle detection in centimeters
#define MAX_SPEED 255
#define LOW_SPEED 100

// H-Bridge pins for DC motors
const int eNA = 6;
const int In1 = 7; //left motor back
const int In2 = 8; // left motor fwd
const int In3 = 5; // right motor fwd
const int In4 = 4; // right motor back
const int eNB = 3;

// Ultrasonic sensors
#define RTRIGGER 9
#define RECHO 2
#define LTRIGGER 13
#define LECHO 10

// IR sensor
const int irSensorPin = A5;

// Speed sensors
const int speedLeft = 12;
const int speedRight = 11;

void setup() {
  Serial.begin(9600);
  // Motor pin setup
  pinMode(eNA, OUTPUT);
  pinMode(In1, OUTPUT);
  pinMode(In2, OUTPUT);
  pinMode(In3, OUTPUT);
  pinMode(In4, OUTPUT);
  pinMode(eNB, OUTPUT);
  // Ultrasonic sensors setup
  pinMode(RTRIGGER, OUTPUT);
  pinMode(RECHO, INPUT);
  pinMode(LTRIGGER, OUTPUT);
  pinMode(LECHO, INPUT);
  // IR sensor setup
  pinMode(irSensorPin, INPUT);
  // Speed sensor setup
  pinMode(speedLeft, INPUT);
  pinMode(speedRight, INPUT);
  // Start with motors off
 stopCar();
}
void loop() {
  long rightDistance = measureDistance(RTRIGGER, RECHO);
  long leftDistance = measureDistance(LTRIGGER, LECHO);

  // Debug Ultrasonic sensors
  Serial.print("Left Distance: ");
  Serial.print(leftDistance);
  Serial.print(" cm, Right Distance: ");
  Serial.println(rightDistance);
  Serial.println(" cm");

  int irValue = analogRead(irSensorPin); // Read the IR sensor value

  // the threshold value is set to 100 as per IR sensor's response to the black line, can be adjusted
  if (irValue > 100) { // Black line is detected
    stopCar(); // Stop if a black line is detected
    return; // Skip the rest of the loop
  }

  if (rightDistance < MAX_DISTANCE || leftDistance < MAX_DISTANCE) {
    // Obstacle detected, slow down and decide on action
    setMotorSpeed(LOW_SPEED);
    avoidObstacle(rightDistance, leftDistance);
  } else {
    // No obstacle or black line, move at maximum speed
    setMotorSpeed(MAX_SPEED);
    moveForward();
  }
}
void setMotorSpeed(int speed) {
  analogWrite(eNA, speed);
  analogWrite(eNB, speed);
}

void moveForward() {
  // Moving forward
  digitalWrite(In2, HIGH);
  digitalWrite(In1, LOW);
  analogWrite(eNA, 200);

  digitalWrite(In3, HIGH);
  digitalWrite(In4, LOW);
  analogWrite(eNB, 200);
}

void stopCar() {
  // Stopping the car
  digitalWrite(In1, LOW);
  digitalWrite(In2, LOW);
  digitalWrite(In3, LOW);
  digitalWrite(In4, LOW);
}

void avoidObstacle(long rightDist, long leftDist) {
  stopCar(); // Stop the car first
  delay(500); // Pause for half a second

  // Reverse a little
  digitalWrite(In2, LOW);
  digitalWrite(In1, HIGH);
  analogWrite(eNA, 150);

  digitalWrite(In3, LOW);
  digitalWrite(In4, HIGH);
  analogWrite(eNB, 150);
  delay(500); // Reverse for half a second

  stopCar(); // Stop the car before turning
  delay(500); // Pause for half a second

  // Determine the direction to turn
  if (rightDist < leftDist) {
    // If the right distance is less than the left, turn left
    turnLeft();
  } else {
    // If the left distance is less than or equal to the right, turn right
    turnRight();
  }

  delay(500); // Turn for half a second
  stopCar(); // Stop the car after turning
}

void turnLeft() {
  // Turn left
  digitalWrite(In1, HIGH);
  digitalWrite(In2, LOW);
  analogWrite(eNA, 130);

  digitalWrite(In3, HIGH);
  digitalWrite(In4, LOW);
  analogWrite(eNB, 50);
}

void turnRight() {
  // Turn right
  digitalWrite(In1, LOW);
  digitalWrite(In2, HIGH);
  analogWrite(eNA, 130);

  digitalWrite(In3, LOW);
  digitalWrite(In4, HIGH);
  analogWrite(eNB, 50);
}

long measureDistance(int triggerPin, int echoPin) {
  // Send a pulse
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(TRIGGER_TIME);
  digitalWrite(triggerPin, LOW);

  // Read the echo
  long duration = pulseIn(echoPin, HIGH);
  long distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 
  return distance;
}