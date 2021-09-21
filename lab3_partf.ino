#include <Arduino_FreeRTOS.h>
#include <semphr.h>

// define two tasks for Blink & AnalogRead
void task1( void *pvParameters );
void task2( void *pvParameters );

static int MS_CONVERSION = 836;
SemaphoreHandle_t xSemaphore = NULL;

//void TaskAnalogRead( void *pvParameters );

// the setup function runs once when you press reset or power the board
void setup() {
  
  // initialize pins

  // pin 5: toggled when task 1 is doing work
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);

  // pin 6: toggled when the ISR is doing work
  pinMode(6, OUTPUT);
  digitalWrite(6, LOW);

  // high task2 (for the second part), is running
  pinMode(8, OUTPUT);
  digitalWrite(8, LOW);

  // high when task 1 is running
  pinMode(9, OUTPUT);
  digitalWrite(9, LOW);

  // initialize interrupt
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), ISR1, CHANGE);


  Serial.begin(9600);
  if ( xSemaphore == NULL ) {
    xSemaphore = xSemaphoreCreateBinary();  // Create semaphore
    if ( xSemaphore == NULL )
      Serial.println("something broke!");
  }
  
  xTaskCreate(task1, "task1", 128, NULL, 2, NULL);  
  //xTaskCreate(task2, "task2", 128, NULL, 1, NULL);

  vTaskStartScheduler();
}

void loop() {
  // Empty. Things are done in Tasks.
}

void CPU_work(int time,int pin) {
 volatile int i,j,k;
 int mode=0;
 for(i=0;i<time;i++)
  for(j=0;j<8;j++){
    mode=!mode;
    digitalWrite(pin,mode);
    for(k=0;k<100;k++);
    }
}

void ISR1() {
  CPU_work(100, 6);

  //xSemaphoreGive(xSemaphore); //for defered interrupts
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/
void task1(void *pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  for(;;) {
    digitalWrite(9, HIGH);
    CPU_work(20, 5);
    digitalWrite(9, LOW);
    xTaskDelayUntil(&xLastWakeTime, 30 / 15);
    }
}

void task2(void *pvParameters) {
  for(;;) {
    xSemaphoreTake( xSemaphore, portMAX_DELAY );
    digitalWrite(8, HIGH);
    CPU_work(100, 6);
    digitalWrite(8, LOW);
    
  }
}
