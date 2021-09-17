#include <Arduino_FreeRTOS.h>

// define two tasks for Blink & AnalogRead
void TaskBlink( void *pvParameters );
void TaskOne( void *pvParameters );
void TaskTwo( void *pvParameters );
//void TaskAnalogRead( void *pvParameters );

// the setup function runs once when you press reset or power the board
void setup() {
  
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB, on LEONARDO, MICRO, YUN, and other 32u4 based boards.
  }

  // Now set up two tasks to run independently.
//  xTaskCreate(
//    TaskBlink
//    ,  "Blink"   // A name just for humans
//    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
//    ,  NULL
//    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
//    ,  NULL );

  xTaskCreate(
    TaskOne
    ,  "Task1"   // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );

  xTaskCreate(
    TaskTwo
    ,  "Task2"   // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );

//  xTaskCreate(
//    TaskAnalogRead
//    ,  "AnalogRead"
//    ,  128  // Stack size
//    ,  NULL
//    ,  1  // Priority
//    ,  NULL );

  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
  vTaskStartScheduler();
}

void CPU_work(int time) {
  static const int MS_CONVERSION = 836;
  volatile int i;
  while (time > 39) {
     time -= 39;
     for(i=0;i<MS_CONVERSION*39;i++);
  }
  for(i=0;i<MS_CONVERSION*time;i++);
}

void loop()
{
  // Empty. Things are done in Tasks.
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/
void TaskOne(void *pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  pinMode(A2, OUTPUT);
  const TickType_t xPeriod = pdMS_TO_TICKS(85);
  for(;;) {
    digitalWrite(A2, HIGH);
    CPU_work(30); 
    digitalWrite(A2, LOW);
    xTaskDelayUntil(&xLastWakeTime, xPeriod);
  }
}

void TaskTwo(void *pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  pinMode(12, OUTPUT);
  const TickType_t xPeriod = pdMS_TO_TICKS(30);
  for(;;) {
    digitalWrite(12, HIGH);
    CPU_work(10);
    digitalWrite(12, LOW);
    xTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
}

//void TaskBlink(void *pvParameters) {
// (void) pvParameters;
// pinMode(LED_BUILTIN, OUTPUT);
// //volatile int i = 0;
// for (;;) // A Task shall never return or exit.
// {
// digitalWrite(LED_BUILTIN, HIGH);
// digitalWrite(A3, HIGH);
// CPU_work(100);
// digitalWrite(LED_BUILTIN, LOW);
// digitalWrite(A3, LOW);
// CPU_work(900);
// }
//} 

//void TaskAnalogRead(void *pvParameters)  // This is a task.
//{
//  (void) pvParameters;
//  
///*
//  AnalogReadSerial
//  Reads an analog input on pin 0, prints the result to the serial monitor.
//  Graphical representation is available using serial plotter (Tools > Serial Plotter menu)
//  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.
//
//  This example code is in the public domain.
//*/
//
//  for (;;)
//  {
//    // read the input on analog pin 0:
//    int sensorValue = analogRead(A0);
//    // print out the value you read:
//    Serial.println(sensorValue);
//    vTaskDelay(1);  // one tick delay (15ms) in between reads for stability
//  }
//}
