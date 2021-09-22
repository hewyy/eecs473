#include <Arduino_FreeRTOS.h>
#include <semphr.h>

// define two tasks for Blink & AnalogRead
void TaskOne( void *pvParameters );
void TaskTwo( void *pvParameters );
void TaskThree( void *pvParameters );

static int MS_CONVERSION = 836;
SemaphoreHandle_t xSemaphore = NULL;

// the setup function runs once when you press reset or power the board
void setup() {
  
  // initialize serial communication at 9600 bits per second: 
  pinMode(6, OUTPUT);
  digitalWrite(6, LOW);

  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);

  pinMode(8, OUTPUT);
  digitalWrite(8, LOW);

  pinMode(9, OUTPUT);
  digitalWrite(9, LOW);

  Serial.begin(9600);
  if ( xSemaphore == NULL ) {
    xSemaphore = xSemaphoreCreateBinary();  // Create semaphore
    if ( xSemaphore == NULL )
      Serial.println("something broke!");
  }

  xTaskCreate(TaskOne, "T1", 128, NULL, 1, NULL);
  xTaskCreate(TaskTwo, "T2", 128, NULL, 2, NULL);
  xTaskCreate(TaskThree, "T3", 128, NULL, 3, NULL);


  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
  vTaskStartScheduler();
}

void CPU_work(int time) {
  volatile int i;
  while (time > 39) {
     time -= 39;
     for(i=0;i<MS_CONVERSION*39;i++){
      
     }
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
  for(;;) {
    xSemaphoreTake( xSemaphore, portMAX_DELAY );
    CPU_work(20);
    digitalWrite(8, LOW);
    
  }
}

void TaskTwo(void *pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  for(;;) {
    digitalWrite(9, HIGH);
    CPU_work(10);
    digitalWrite(9, LOW);
    xTaskDelayUntil(&xLastWakeTime, 30 / 15);
    }
}

void TaskThree(void *pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  for(;;) {
    digitalWrite(8, HIGH);
    xSemaphoreGive(xSemaphore);
    vTaskDelay(85 / 15);
  }
}
