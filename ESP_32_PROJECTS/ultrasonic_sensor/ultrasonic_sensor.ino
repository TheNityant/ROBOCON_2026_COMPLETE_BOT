
// Define Pins
const int trigPin = 25;
const int echoPin = 26;

// Variables for math
long duration;
int distance;

void setup() {
  pinMode(trigPin, OUTPUT); // Sends the sound
  pinMode(echoPin, INPUT); // Receives the sound
  Serial.begin(9600); // Start serial monitor
}

void loop() {
  // Clear the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Trigger the sensor by sending a HIGH pulse for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the echoPin (returns the sound wave travel time in microseconds)
  duration = pulseIn(echoPin, HIGH);

  // Calculating the distance
  // Speed of sound is 0.034 cm/us. We divide by 2 (out and back).
  distance = duration * 0.034 / 2;

  // Print the distance to the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  delay(500); // Wait half a second before next reading
}