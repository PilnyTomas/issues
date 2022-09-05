#define BUZZERPIN 25
#define FREQUENCY 440
#define LEDC_CHANNEL 0

void setup() {
  Serial.begin(115200);
  while(!Serial){;}
  ledcSetup(LEDC_CHANNEL, FREQUENCY, 8);
  ledcAttachPin(BUZZERPIN, LEDC_CHANNEL); 
}

void loop() {
  Serial.println("50%");
  ledcWrite(LEDC_CHANNEL, 50);
  delay(1000);
  
  Serial.printf("f=%dhz\n",FREQUENCY);
  ledcWriteTone(LEDC_CHANNEL, FREQUENCY);
  delay(1000);
  
  Serial.printf("tone\n");
  ledcWriteNote(LEDC_CHANNEL, NOTE_C, 4);
  delay(1000);
  tone(BUZZERPIN, 440, 2000);
  
  ledcWriteTone(0, 440, 8);
  delay(1000);
}
