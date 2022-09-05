char sStringBuffer[20];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  sprintf(&sStringBuffer[10], "123.4567"); // edited: original code was: dtostrf(123.45, 4, 1, &sStringBuffer[10]);
  sprintf(sStringBuffer, "Acc.X %s", &sStringBuffer[10]);
  Serial.println(sStringBuffer);
}

void loop() {
  // put your main code here, to run repeatedly:
}
