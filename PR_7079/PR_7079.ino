void setup() {
    uint8_t channel = 0;
    
    uint8_t bits = 1;
    uint8_t pin = 19;
    uint32_t freq = 40 * 1000 * 1000; // 40 MHz
    uint8_t duty = 1; // 50% duty

    ledcSetup(channel, freq, bits);
    ledcAttachPin(pin, channel);
    ledcWrite(channel, duty);

    delay(1);

    ledcDetachPin(pin);
    ledcSetup(channel, freq/2, bits+1);
    ledcAttachPin(pin, channel);
    ledcWrite(channel, duty);
    delay(1);
    ledcWrite(channel, duty+1);
    delay(1);
    ledcWrite(channel, duty+2);
    delay(1);
    ledcDetachPin(pin);

}

void loop(){}
