// ESP32-S3 devkit board uses pin 48 as RGB NeoPixel LED
#define BUILTIN_RGBLED_PIN   38
// RMT peripheral is used to drive the RGB LED
rmt_obj_t* rmt_LED = NULL;
rmt_data_t led_data[8 * 3]; // 8 bits x 3 colors per LED
enum {WHITE, RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA, OFF};
int LED_Colors[8][3] =  {
  //Green, Red, Blue
  { 0xFF, 0xFF, 0xFF }, // White
  { 0x00, 0xFF, 0x00 }, // Red
  { 0xFF, 0x00, 0x00 }, // Green
  { 0x00, 0x00, 0xFF }, // Blue
  { 0xC0, 0xFF, 0x00 }, // Yellow
  { 0xB0, 0x00, 0xFF }, // Cyan
  { 0x00, 0xD0, 0xB0 }, // Magenta
  { 0x00, 0x00, 0x00 }, // Off | Black
};

void setLEDColor(int *color, rmt_obj_t *rmtObj) {
  if (rmtObj == NULL || color == NULL) return;

  // Init data with only one led ON
  int col, bit;
  int i = 0;
  for (col = 0; col < 3; col++ ) {
    for (bit = 0; bit < 8; bit++) {
      if ( color[col] & (1 << (7 - bit)) ) {
        led_data[i].level0 = 1;
        led_data[i].duration0 = 8;
        led_data[i].level1 = 0;
        led_data[i].duration1 = 4;
      } else {
        led_data[i].level0 = 1;
        led_data[i].duration0 = 4;
        led_data[i].level1 = 0;
        led_data[i].duration1 = 8;
      }
      i++;
    }
  }
  // Send the data
  rmtWrite(rmt_LED, led_data, 8 * 3); // 8 bits x 3 colors per LED
}


void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    delay(100);
  }
  delay(10000);
  Serial.println("Starting");
  log_d("Starting");
  if ((rmt_LED = rmtInit(BUILTIN_RGBLED_PIN, RMT_TX_MODE, RMT_MEM_64)) == NULL) {
    Serial.println("RGB LED init failed\n");
    rmt_LED = NULL;
  }
  if (rmt_LED) rmtSetTick(rmt_LED, 100);

  esp_sleep_enable_timer_wakeup(10e6);
  setLEDColor(LED_Colors[WHITE], rmt_LED);
#if 1
  // DEEP sleep test.
  Serial.println("\n\nEntering in DEEP Sleep...");
  log_d("Entering in DEEP Sleep...");
  Serial.flush();
  esp_deep_sleep_start();
  // From this point on it will never get executed back from deep sleep
  // Board will hard reset after returning from deep sleep.
  Serial.println("Got back from DEEP Sleep");
#else
  // LIGHT Sleep test.
  Serial.println("\n\nEntering in LIGHT Sleep...");
  log_d("Entering in LIGHT Sleep...");
  Serial.flush();
  esp_light_sleep_start();
  Serial.println("Got back from LIGHT Sleep...\n");
  log_d("Got back from LIGHT Sleep...\n");
#endif
  Serial.flush();
}

void loop() {
  static uint32_t count = 0;
  Serial.printf("Loop CDC OUTPUT#%d\n", ++count);
  setLEDColor(LED_Colors[count & 7], rmt_LED);
  delay(2000);
}
