//main.c
//authored by Jared Hull
//
//tasks 1 and 2 blink the ACT LED
//main initialises the devices and IP tasks

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include "interrupts.h"
#include "gpio.h"
#include "video.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"

/*
 * Input commands and pin mounts
 */

#define FORWARD 'F'
#define LEFT    'L'
#define BACK    'B'
#define RIGHT   'R'
#define STOP    'S'

//Adjust to add intermediate speeds
#define IDLE    0
#define SPEED   1

//Adjust to reverse motor polarity
int LEFT_MOTOR  = 1;
int RIGHT_MOTOR = 0;

//These pins are for the RPI3 B, adjust if using a different board

//GPIO pins for the motor
#define EN1 20  //pwm pin, left motor
#define EN2 21  //pwm pin, right motor
#define A_1 6   //Y1, left motor positive
#define A_2 13  //Y2, left motor negative
#define A_3 19  //Y3, right motor positive
#define A_4 26  //Y4, right motor negative

//Pins for your distance sensor
#define TRIG 9
#define ECHO 11

//Trigger pins tied to each task
#define T1_PIN 14
#define T2_PIN 23
#define T3_PIN 18

//LCD pins. Feel free to change.
#define RS 2
#define RW 3
#define E_ 4
#define D4 17
#define D5 27
#define D6 22
#define D7 10


#define LED 5 //TODO: CHANGE THIS

//moveRobot as used in lab 1
void moveRobot(char command);

//Helper function for moveRobot as used in lab 1
void motorControl(int ifLeftMotor, char command);



volatile int DISTANCE_IN_TICKS = 0;
const int MIN_DISTANCE = 2;
const int GO_AGAIN = 5;
const int BLINKY_CONST = 5;
int times_close = 0;
int times_far = 0;



xSemaphoreHandle mutex = NULL;


//Task 1 is implemented for you. It interfaces with the distance sensor and 
//calculates the number of ticks the echo line is high after trigger, storing that value in
//the global variable DISTANCE_IN_TICKS

//You may need to alter some values as described in the lab documentation.

void task1() {
	portTickType xLastWakeTime;
	const portTickType xFrequency = 50;
		
	
	xLastWakeTime = xTaskGetTickCount();

	while(1) {
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
		portTickType start = xTaskGetTickCount();
		//IN TASK
		SetGpio(T1_PIN, 1);
		
		// take semaphore
		if(xSemaphoreTake(mutex, xFrequency) == pdTRUE ) {
			
			SetGpio(TRIG, 1);
			vTaskDelay(1);
			SetGpio(TRIG, 0);
			
			while(ReadGpio(ECHO) == 0);
			
			portTickType curr = xTaskGetTickCount();
			
			checker_1 = xTaskGetTickCount();
			while(ReadGpio(ECHO) == 1);
			portTickType traveltime_in_ticks = xTaskGetTickCount() - curr;
			
			// give it up
			xSemaphoreGive(mutex);
		}

		//END TASK
		SetGpio(T1_PIN, 0);
	}
}


// CHECK THE DISTANCE_IN_TICKS VARIABLE
//	- stop if needed
//	- resume when the obstacle is moved away
void task2() {
	// Note that semaphores may be valuable here to protect your global variable
	portTickType xLastWakeTime;
	const portTickType xFrequency = 100;
	
	xLastWakeTime = xTaskGetTickCount();
	int temp_distance = 0;

	while(1) {

		vTaskDelayUntil(&xLastWakeTime, xFrequency);
		SetGpio(T2_PIN, 1);

		// take semaphore
		if(xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE ) {
			temp_distance = DISTANCE_IN_TICKS;
			// give it up
			xSemaphoreGive(mutex);
		}
		
		if (temp_distance < MIN_DISTANCE) {
			moveRobot(STOP);
		}
		else if (temp_distance > GO_AGAIN) {
			moveRobot(FORWARD);
		}
		
		SetGpio(T2_PIN, 0);
	}
}

// FLASH LED - external led
// flash faster when the object is closer
void task3() {

	int temp_delay = 0;
	
	while(1) {
		SetGpio(T3_PIN, 1);

		// TURN ON!
		SetGpio(LED, 1);

		// take semephore
		temp_delay = DISTANCE_IN_TICKS * BLINKY_CONST;
			// give it up	
		vTaskDelay(temp_delay);
		
		// TURN OFF!
		SetGpio(LED, 0);

		//take semephore
		temp_delay = DISTANCE_IN_TICKS * BLINKY_CONST;
		// give it up
		
		vTaskDelay(temp_delay);
		SetGpio(T3_PIN, 0);
	}
}



int main(void) {
	SetGpioFunction(T1_PIN, 1);
	SetGpioFunction(T2_PIN, 1);
	SetGpioFunction(T3_PIN, 1);
	SetGpio(T1_PIN, 0);
	SetGpio(T2_PIN, 0);
	SetGpio(T3_PIN, 0);

	SetGpioFunction(EN1, 1);
	SetGpioFunction(EN2, 1);
	SetGpioFunction(A_1, 1);
	SetGpioFunction(A_2, 1);
	SetGpioFunction(A_3, 1);
	SetGpioFunction(A_4, 1);

	SetGpioFunction(TRIG, 1);
	SetGpioFunction(ECHO, 0);
	SetGpio(TRIG, 0);


	// INIT LED LIGHT
	SetGpioFunction(LED, 1);
	SetGpio(LED, 0);



	initFB();

	DisableInterrupts();
	InitInterruptController();

	xTaskCreate(task1, "t1", 128, NULL, 2, NULL);
	xTaskCreate(task2, "t2", 128, NULL, 1, NULL);
	xTaskCreate(task3, "t3", 128, NULL, 0, NULL);


	// create semaphore
	mutex = xSemaphoreCreateMutex();


	//set to 0 for no debug, 1 for debug, or 2 for GCC instrumentation (if enabled in config)
	loaded = 1;

	vTaskStartScheduler();

	/*
	 *	We should never get here, but just in case something goes wrong,
	 *	we'll place the CPU into a safe loop.
	 */
	while(1) {
		;
	}
}

/****
		TODO: Add LCD functions here.






	****/

void moveRobot(char command) {
    switch(command) {
        case FORWARD:
            motorControl(LEFT_MOTOR, FORWARD);
            motorControl(RIGHT_MOTOR, FORWARD);
            break;
        case LEFT:
            motorControl(LEFT_MOTOR, STOP);
            motorControl(RIGHT_MOTOR, FORWARD);
            break; 
        case BACK:
            motorControl(LEFT_MOTOR, BACK);
            motorControl(RIGHT_MOTOR, BACK);
            break; 
        case RIGHT:
            motorControl(LEFT_MOTOR, FORWARD);
            motorControl(RIGHT_MOTOR, STOP);
            break;
        case STOP:
            motorControl(LEFT_MOTOR, STOP);
            motorControl(RIGHT_MOTOR, STOP);
            break;
        default:
            break;
    }   
}

void motorControl(int ifLeftMotor, char command) {
    int enable      = ifLeftMotor ? EN1 : EN2;
    int motorPos    = ifLeftMotor ? A_1 : A_3;
    int motorNeg    = ifLeftMotor ? A_2 : A_4;

    switch (command) {
        case FORWARD:
            SetGpio(enable, SPEED);
            SetGpio(motorPos, 1);
            SetGpio(motorNeg, 0);
            break;
        case BACK:
            SetGpio(enable, SPEED);
            SetGpio(motorPos, 0);
            SetGpio(motorNeg, 1);
            break;    
        case STOP:
            SetGpio(motorPos, 0);
            SetGpio(motorNeg, 0);
            break;
        default:
            break;           
    }
}
