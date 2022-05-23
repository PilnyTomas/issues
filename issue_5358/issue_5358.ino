#include <ArduinoJson.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

/*#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif*/

static const BaseType_t pro_cpu = 0;
static const BaseType_t app_cpu = 1;

//ADC Related Global Variables
static const uint16_t timer_divider =80;
static const uint64_t timer_max_count=1000;

static const int adc_pin=A0;

static const int BUF_SIZE=1000;

static int buf[BUF_SIZE];
int Buff_Len=0;
static int Read=0;
static int Write=0;
static int count=0;
static float avg=0;
int i=0;
int BLE_flag=0;
String cmd;
static volatile uint8_t msg_flag = 0;
int t3=0;

static hw_timer_t *timer=NULL;
static uint16_t val;
static int count1=0;

static SemaphoreHandle_t bin_sem=NULL;
static SemaphoreHandle_t bin_sem2=NULL;
static portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;
//ADC Related Global Variables

//BLE Global Variable
char Reading[1000];
BLEServer *pServer = NULL;
BLECharacteristic * pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;

TaskHandle_t task1;
TaskHandle_t task2;
TaskHandle_t task3;
//Declaration BLE necessary Classes
#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};
//BLE Global Variables

//Task Section 
void IRAM_ATTR onTimer()
{
  BaseType_t xHigherPriorityTaskWoken=pdFALSE;

  val=analogRead(adc_pin);
  count1++;
  
  vTaskNotifyGiveFromISR(task1, 
                         &xHigherPriorityTaskWoken);
  
  if (xHigherPriorityTaskWoken) {
    portYIELD_FROM_ISR();
  }
}
void Timer_Turnon(void *parameters)
{
  while(1)
  {
    if(msg_flag == 1)
    //if(t3==1)
  //xSemaphoreTake(bin_sem2,portMAX_DELAY);
   { 
   {// inner blcok 
     timerStart(timer);
     Serial.println("Timer started again"); 
     msg_flag = 0;
  } // inner blcok
  } // if
 } // while
}

void move_to_Queue (void *parameters)
{
  while(1)
  {
    static uint32_t thread_notification;
    thread_notification = ulTaskNotifyTake(pdTRUE, 
                        portMAX_DELAY);

   if(thread_notification)
   {
   //xSemaphoreTake(bin_sem2,portMAX_DELAY);
    if(Buff_Len==BUF_SIZE||count1>2000)
    {
      Serial.println("Buffer is full");
      xSemaphoreGive(bin_sem);
      //BLE_flag=1;
      Buff_Len=0;
      count1=0;
      timerStop(timer);    
    }
    else
    {
      buf[Write]=val;
      Write = (Write + 1) % BUF_SIZE;
      Buff_Len++;
    } 
  }
}
}

void BLE_Task(void *parameters){
  while(1){
    //Serial.println("Task3 on work");
    //if(BLE_flag==1)
    {
      //BLE_flag=0;
      xSemaphoreTake(bin_sem,portMAX_DELAY);
      Serial.println("BLE");
      for (i=0;i<BUF_SIZE;i++){
        if(i==0){
          sprintf(Reading,"[%d",buf[i]);
        }else if(i==BUF_SIZE-1){
          sprintf(Reading,",%d]",buf[i]);
        }else{
          sprintf(Reading,",%d",buf[i]); 
        }
        Serial.print(Reading);
        if (deviceConnected){
          pTxCharacteristic->setValue(Reading);
          pTxCharacteristic->notify();
          delay(10); // bluetooth stack will go into congestion, if too many packets are sent
        }   
      } // for
      Serial.println();
    } // anon block
    msg_flag = 1;
    //t3=1;
    //xSemaphoreGive(bin_sem2);
  } // inf loop
}

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  bin_sem = xSemaphoreCreateBinary();
  bin_sem2 = xSemaphoreCreateBinary();
  vTaskDelay(1000/portTICK_PERIOD_MS);
  
  //BLE Declarations
  BLEDevice::init("UART Service");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pTxCharacteristic = pService->createCharacteristic(
                    CHARACTERISTIC_UUID_TX,
                    BLECharacteristic::PROPERTY_NOTIFY
                  );
      
  pTxCharacteristic->addDescriptor(new BLE2902());
  pService->start();
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
  //BLE Declaration

  //ADC Semaphore and Timer Declarations
  bin_sem = xSemaphoreCreateBinary();
  bin_sem2 = xSemaphoreCreateBinary();

  if(bin_sem==NULL||bin_sem2==NULL)
  {
    Serial.println("Could not create semaphore");
    ESP.restart();
  }
    xTaskCreatePinnedToCore(move_to_Queue,
                          "move_to_Queue",
                          1024,
                          NULL,
                          2,
                          &task1,
                          app_cpu);
     xTaskCreatePinnedToCore(BLE_Task,
                          "BLE_Task",
                          2048,
                          NULL,
                          2,
                          &task2,
                          app_cpu);
                  
     timer = timerBegin(0, timer_divider, true);

  // Provide ISR to timer (timer, function, edge)
     timerAttachInterrupt(timer, &onTimer, true);

  // At what count should ISR trigger (timer, count, autoreload)
     timerAlarmWrite(timer, timer_max_count, true);

  // Allow ISR to trigger
     timerAlarmEnable(timer);
     xTaskCreatePinnedToCore(Timer_Turnon,
                    "Timer_Turnon",
                    1024,
                    NULL,
                    2,
                    &task3,
                    app_cpu);
      vTaskDelete(NULL);
}

void loop() {
  // put your main code here, to run repeatedly:
}
