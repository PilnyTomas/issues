#define TIME_BETWEEN_OUTPUTS 1000

unsigned long loopNumber = 0;
unsigned long timeSinceLastOutput;
unsigned long currentTime;

void setup() {
  Serial.begin(115200);
  Serial.println("Speed Test Start");
  currentTime = millis();
  timeSinceLastOutput = currentTime;
}

void loop() {
  currentTime = millis();
  if (currentTime - timeSinceLastOutput > TIME_BETWEEN_OUTPUTS)
  {
    timeSinceLastOutput = currentTime;
    Serial.print(currentTime / 1000);
    Serial.print("s: ");
    Serial.print(loopNumber);
    Serial.println(" reads");

    Serial.print(1000000.0/loopNumber);
    Serial.println("us per read");

    loopNumber = 0;
  }
  analogRead(34);
  loopNumber++;
}
