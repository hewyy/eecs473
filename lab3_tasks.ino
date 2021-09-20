#include <Arduino_FreeRTOS.h>

// define two tasks for Blink & AnalogRead
void TaskBlink( void *pvParameters );
void TaskOne( void *pvParameters );
void TaskTwo( void *pvParameters );

static int MS_CONVERSION = 836;
//void TaskAnalogRead( void *pvParameters );

// the setup function runs once when you press reset or power the board
void setup() {
  
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  Serial.println(portTICK_PERIOD_MS);
  pinMode(7, OUTPUT);
  
  pinMode(8, OUTPUT);
  digitalWrite(8, LOW);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB, on LEONARDO, MICRO, YUN, and other 32u4 based boards.
  }

  xTaskCreate(
    TaskOne
    ,  "Task1"   // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );

  xTaskCreate(
    TaskTwo
    ,  "Task2"   // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  3  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );

  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
  vTaskStartScheduler();
}

void CPU_work(int time) {
  volatile int i;
  digitalWrite(7, HIGH);
  while (time > 39) {
     time -= 39;
     for(i=0;i<MS_CONVERSION*39;i++){
      
     }
  }
  for(i=0;i<MS_CONVERSION*time;i++) {
    
  }
  digitalWrite(7, LOW);
}

void loop()
{
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/
void TaskOne(void *pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  for(;;) {
    xTaskDelayUntil(&xLastWakeTime, 85 / 15);
    digitalWrite(8, HIGH);
    CPU_work(30); 
    digitalWrite(8, LOW);
  }
}

void TaskTwo(void *pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  pinMode(9, OUTPUT);
  digitalWrite(9, LOW);
  for(;;) {
    xTaskDelayUntil(&xLastWakeTime, 30 / 15);
    digitalWrite(9, HIGH);
    CPU_work(10);
    digitalWrite(9, LOW);
    }
}
