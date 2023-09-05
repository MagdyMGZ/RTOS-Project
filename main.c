/*
 * This file is part of the µOS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2014 Liviu Ionescu.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include "diag/trace.h"

/*--------------------------------- Kernel includes.--------------------------------- */
#include "FreeRTOS.h"

#include "task.h"

/*Define the task handle variables
 * to manage and manipulate tasks within your FreeRTOS application more effectively */
		TaskHandle_t Transmitter_1_TaskHandle =NULL ;
		TaskHandle_t Transmitter_2_TaskHandle =NULL ;
		TaskHandle_t Transmitter_3_TaskHandle =NULL;
		TaskHandle_t ReceiverTaskHandle       =NULL;
		int xreturned1;
		int xreturned2;
		int xreturned3;
		int xreturned4;
		/*------------------------------------------------------*/

#include "queue.h"

		/*Define the  Queue handle variable*/
		QueueHandle_t MessageQueue;
		/*------------------------------------------------------*/

#include "timers.h"

		/*Define the  Timer handle variables*/
		static TimerHandle_t xTransmitter_1_TimerHandle;
		static TimerHandle_t xTransmitter_2_TimerHandle;
		static TimerHandle_t xTransmitter_3_TimerHandle;
		static TimerHandle_t xReceiverTimerHandle;
		BaseType_t xTimer_T1_Started, xTimer_T2_Started,xTimer_T3_Started,xTimer_R_Started;
		/*------------------------------------------------------*/
#include "semphr.h"

		/*Define the Semaphore handle variables*/
		SemaphoreHandle_t TransmitSemaphore_1=NULL ;
		SemaphoreHandle_t TransmitSemaphore_2=NULL ;
		SemaphoreHandle_t TransmitSemaphore_3=NULL ;
		SemaphoreHandle_t ReceiveSemaphore=NULL ;
		/*------------------------------------------------------*/

#define CCM_RAM __attribute__((section(".ccmram")))

// ----------------------------------------------------------------------------

#include "led.h"

/*-----------------------------------------------------------*/
// ----------------------------------------------------------------------------

#define TRANSMITTER_TASK_STACK_SIZE   configMINIMAL_STACK_SIZE
#define RECEIVER_TASK_STACK_SIZE	  configMINIMAL_STACK_SIZE

#define QUEUE_SIZE 						3

#define MAX_RECEIVED_MESSAGES 1000

#define Transmitter_1_TASK_PRIORITY     2
#define Transmitter_2_TASK_PRIORITY     1
#define Transmitter_3_TASK_PRIORITY     1
#define RECEIVER_TASK_PRIORITY   		3

char Message[20];
char MessageReveiver[20];

int Transmitter_1_SentMessages = 0;
int Transmitter_1_BlockedMessages = 0;
int Tsender1;

int Transmitter_2_SentMessages = 0;
int Transmitter_2_BlockedMessages = 0;
int Tsender2;

int Transmitter_3_SentMessages = 0;
int Transmitter_3_BlockedMessages = 0;
int Tsender3;

int ReceivedMessages = 0;
int Treceiver=100;

int TransmittedMessages=0;
int BlockedMessages=0;

int Reset =0;

int LowerBound[6]= {50, 80, 110, 140, 170, 200};
int UpperBound[6]= {150, 200, 250, 300, 350, 400};
/* ---------------------------Transmitter & Receiver Tasks---------------------------*/

void TransmitterTask_1(void* p_T1_Parameters) {
	while (1) {
			if (xSemaphoreTake(TransmitSemaphore_1,portMAX_DELAY) == 1) {
				sprintf(Message, "Time is %lu\n", xTaskGetTickCount());
				if (xQueueSend( MessageQueue , &Message , 0) == pdPASS)
					Transmitter_1_SentMessages++;
				else
					Transmitter_1_BlockedMessages++;
			}
		}

    }

void TransmitterTask_2(void* p_T2_Parameters) {
	while (1) {
				if (xSemaphoreTake(TransmitSemaphore_2,portMAX_DELAY) == 1) {
					sprintf(Message, "Time is %lu \n", xTaskGetTickCount());
					if (xQueueSend( MessageQueue , &Message , 0) == pdPASS)
						Transmitter_2_SentMessages++;
					else
						Transmitter_2_BlockedMessages++;
				}
			}

	    }

void TransmitterTask_3(void* p_T3_Parameters) {
	while (1) {
				if (xSemaphoreTake(TransmitSemaphore_3,portMAX_DELAY )== 1) {
					sprintf(Message, "Time is %lu\n", xTaskGetTickCount());
					if (xQueueSend( MessageQueue , &Message , 0) == pdPASS)
						Transmitter_3_SentMessages++;
					else
						Transmitter_3_BlockedMessages++;
				}
			}

	    }

void ReceiverTask(void* p_R_Parameters) {
	while (1) {
			if (xSemaphoreTake(ReceiveSemaphore,portMAX_DELAY) == 1) {
				if (MessageQueue!= 0)
				{
					if (xQueueReceive( MessageQueue , &MessageReveiver , 0) == pdPASS)
					{
						printf("Received message: %s\n", MessageReveiver);
						ReceivedMessages++;
					}
				}

			}
		}
}
void ResetCounters (void)
{
	 Transmitter_1_SentMessages = 0;
	 Transmitter_1_BlockedMessages = 0;

	 Transmitter_2_SentMessages = 0;
	 Transmitter_2_BlockedMessages = 0;

	 Transmitter_3_SentMessages = 0;
	 Transmitter_3_BlockedMessages = 0;

	 ReceivedMessages = 0;

	  TransmittedMessages=0;
	  BlockedMessages=0;
}
void UpdateRandomPeriods(void)
{
	Tsender1= (rand()%(UpperBound[Reset] - LowerBound[Reset] + 1) + LowerBound[Reset]);
	Tsender2= (rand()%(UpperBound[Reset] - LowerBound[Reset] + 1) + LowerBound[Reset]);
	Tsender3= (rand()%(UpperBound[Reset] - LowerBound[Reset] + 1) + LowerBound[Reset]);
}
void MainReset(void)
{
	ResetCounters();
	xQueueReset(MessageQueue);
	UpdateRandomPeriods();
}
void ResetFunction(void )
	{
	 TransmittedMessages= Transmitter_1_SentMessages+Transmitter_2_SentMessages+Transmitter_3_SentMessages;
	 BlockedMessages = Transmitter_1_BlockedMessages+Transmitter_2_BlockedMessages+Transmitter_3_BlockedMessages;
		printf("--------------------Statistics of Iteration %d --------------------\r\n",Reset);
		printf("The Sent Messages of Sender1 =%d & The Blocked Messages of Sender1=%d\r\n",Transmitter_1_SentMessages,Transmitter_1_BlockedMessages);
		printf("The Sent Messages of Sender2 =%d & The Blocked Messages of Sender2=%d\r\n",Transmitter_2_SentMessages,Transmitter_2_BlockedMessages);
		printf("The Sent Messages of Sender3 =%d & The Blocked Messages of Sender3=%d\r\n",Transmitter_3_SentMessages,Transmitter_3_BlockedMessages);
		printf("-----------------------------------------------------------------------\n");
		printf("The Total number of Sent Messages=%d & The total number of blocked messages=%d \r\n",TransmittedMessages,BlockedMessages);
		printf("The total number of received messages=%d \r\n",ReceivedMessages);
		printf("-----------------------------------------------------------------------\n");
		Reset ++;
		MainReset();

		if (Reset > 5 )
			{
				printf("GameOver\r\n");
				xTimerDelete( xTransmitter_1_TimerHandle , 0);
				xTimerDelete( xTransmitter_2_TimerHandle , 0);
				xTimerDelete( xTransmitter_3_TimerHandle , 0);
				xTimerDelete( xReceiverTimerHandle , 0);
				Reset=0;
				exit(0);
			}
		}
/* ---------------------------Transmitter & Receiver Timer CallBack Functions ---------------------------*/

// Transmitter Timer Callback Functions

static void Transmitter_1_TimerCallback(TimerHandle_t xTimer_T1_Started) {
	Tsender1= (rand()%(UpperBound[Reset] - LowerBound[Reset] + 1) + LowerBound[Reset]);
	xTimerChangePeriod( xTransmitter_1_TimerHandle ,pdMS_TO_TICKS(Tsender1) , 0) ;
	xTimerReset( xTransmitter_1_TimerHandle , 0);
	// Release the Transmitter semaphore
    xSemaphoreGive(TransmitSemaphore_1);
}
static void Transmitter_2_TimerCallback(TimerHandle_t xTimer_T2_Started) {
	Tsender2= (rand()%(UpperBound[Reset] - LowerBound[Reset] + 1) + LowerBound[Reset]);
	xTimerChangePeriod( xTransmitter_2_TimerHandle ,pdMS_TO_TICKS(Tsender2) , 0) ;
	xTimerReset( xTransmitter_2_TimerHandle , 0);
	// Release the Transmitter semaphore
    xSemaphoreGive(TransmitSemaphore_2);
}
static void Transmitter_3_TimerCallback(TimerHandle_t xTimer_T3_Started) {
	Tsender3= (rand()%(UpperBound[Reset] - LowerBound[Reset] + 1) + LowerBound[Reset]);
	xTimerChangePeriod( xTransmitter_3_TimerHandle ,pdMS_TO_TICKS(Tsender3) , 0) ;
	xTimerReset( xTransmitter_3_TimerHandle , 0);
	// Release the Transmitter semaphore
    xSemaphoreGive(TransmitSemaphore_3);
}

// Receiver Timer Callback Function
static void ReceiverTimerCallback(TimerHandle_t xTimer_R_Started) {

	if (ReceivedMessages>=MAX_RECEIVED_MESSAGES)
		ResetFunction();
	// Release the receiver semaphore

    xSemaphoreGive(ReceiveSemaphore);
}


/*-----------------------------------------------------------*/

// ----------------------------------------------------------------------------
//
// Semihosting STM32F4 empty sample (trace via DEBUG).
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the DEBUG output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace-impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

int main(int argc, char* argv[])
{
	/*Create 4 Tasks : 3 Transmitter & 1 Receiver */
		   /* xTaskCreate(TransmitterTask_1, "MessageTransmitter_1",TRANSMITTER_TASK_STACK_SIZE, NULL, Transmitter_1_TASK_PRIORITY, Transmitter_1_TaskHandle);
		    xTaskCreate(TransmitterTask_2,"MessageTransmitter_2",TRANSMITTER_TASK_STACK_SIZE, NULL, Transmitter_2_TASK_PRIORITY, Transmitter_2_TaskHandle);
		    xTaskCreate(TransmitterTask_3,"MessageTransmitter_3", TRANSMITTER_TASK_STACK_SIZE, NULL, Transmitter_3_TASK_PRIORITY, Transmitter_3_TaskHandle);
		    xTaskCreate(ReceiverTask, "MessageReceiver", RECEIVER_TASK_STACK_SIZE, NULL, RECEIVER_TASK_PRIORITY, ReceiverTaskHandle);*/

			if (xTaskCreate(TransmitterTask_1, "MessageTransmitter_1",TRANSMITTER_TASK_STACK_SIZE, NULL, Transmitter_1_TASK_PRIORITY, Transmitter_1_TaskHandle) != pdPASS) {
			        printf("Failed to create Transmitter_1 task\n");
			        return -1;
			    }
			if (xTaskCreate(TransmitterTask_2,"MessageTransmitter_2",TRANSMITTER_TASK_STACK_SIZE, NULL, Transmitter_2_TASK_PRIORITY, Transmitter_2_TaskHandle) != pdPASS) {
					        printf("Failed to create Transmitter_2 task\n");
					        return -1;
					    }
			if (xTaskCreate(TransmitterTask_3,"MessageTransmitter_3", TRANSMITTER_TASK_STACK_SIZE, NULL, Transmitter_3_TASK_PRIORITY, Transmitter_3_TaskHandle) != pdPASS) {
					        printf("Failed to create Transmitter_3 task\n");
					        return -1;
					    }
			    // Create the receiver task
			    if (xTaskCreate(ReceiverTask, "MessageReceiver", RECEIVER_TASK_STACK_SIZE, NULL, RECEIVER_TASK_PRIORITY, NULL) != pdPASS) {
			        printf("Failed to create Receiver task\n");
			        return -1;
			    }
			    /*-----------------------------------------------------------*/
		    /*Create the Queues to communicate between tasks*/

		    	    MessageQueue = xQueueCreate(QUEUE_SIZE, sizeof(Message));

		    	    if (MessageQueue == NULL) {
		    	        printf("Failed to create MessageQueue \n");
		    	        return -1;
		    	    }
		       /*-----------------------------------------------------------*/
	MainReset();
	/*-----------------------------------------------------------*/
		/* Create Transmit&Receive Semaphores*/

		TransmitSemaphore_1=xSemaphoreCreateBinary();
		if (TransmitSemaphore_1 == NULL) {
			 printf(" TransmitSemaphore_1 creation failed\n");
			   return -1;
		}
		TransmitSemaphore_2=xSemaphoreCreateBinary();
		if (TransmitSemaphore_2 == NULL) {
			 printf(" TransmitSemaphore_2 creation failed\n");
			   return -1;
		}
		TransmitSemaphore_3=xSemaphoreCreateBinary();
		if (TransmitSemaphore_3 == NULL) {
			 printf(" TransmitSemaphore_3 creation failed\n");
			   return -1;
		}
		ReceiveSemaphore=xSemaphoreCreateBinary();
		if (ReceiveSemaphore == NULL) {
			 printf(" ReceiveSemaphore creation failed\n");
			   return -1;
		}
		/*-----------------------------------------------------------*/
	/* Create Transmitter&Receiver Timer */
	xTransmitter_1_TimerHandle=xTimerCreate("SenderTimer1", pdMS_TO_TICKS(Tsender1), pdTRUE, NULL, Transmitter_1_TimerCallback);
	xTransmitter_2_TimerHandle=xTimerCreate("SenderTimer2", pdMS_TO_TICKS(Tsender2), pdTRUE, NULL, Transmitter_2_TimerCallback);
	xTransmitter_3_TimerHandle=xTimerCreate("SenderTimer3", pdMS_TO_TICKS(Tsender3), pdTRUE, NULL, Transmitter_3_TimerCallback);
	xReceiverTimerHandle=xTimerCreate("ReceiverTimer", pdMS_TO_TICKS(Treceiver), pdTRUE, NULL, ReceiverTimerCallback);

	if (xTransmitter_1_TimerHandle == NULL || xTransmitter_2_TimerHandle == NULL
			|| xTransmitter_3_TimerHandle == NULL || xReceiverTimerHandle == NULL) {
		 printf(" Timer creation failed\n");
	}
	/* Start Transmitter&Receiver Timer */
		 xTimer_T1_Started=xTimerStart(xTransmitter_1_TimerHandle, 0);
		 xTimer_T2_Started=xTimerStart(xTransmitter_2_TimerHandle, 0);
		 xTimer_T3_Started=xTimerStart(xTransmitter_3_TimerHandle, 0);
		 xTimer_R_Started=xTimerStart(xReceiverTimerHandle, 0);

		if (xTimer_T1_Started != pdPASS ||xTimer_T2_Started != pdPASS
				 || xTimer_T3_Started != pdPASS || xTimer_R_Started != pdPASS) {
			 printf(" Timer start failed\n");
		 }
		else
			vTaskStartScheduler();
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------


void vApplicationMallocFailedHook( void )
{
	/* Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
volatile size_t xFreeStackSpace;

	/* This function is called on each cycle of the idle task.  In this case it
	does nothing useful, other than report the amout of FreeRTOS heap that
	remains unallocated. */
	xFreeStackSpace = xPortGetFreeHeapSize();

	if( xFreeStackSpace > 100 )
	{
		/* By now, the kernel has allocated everything it is going to, so
		if there is a lot of heap remaining unallocated then
		the value of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h can be
		reduced accordingly. */
	}
}

void vApplicationTickHook(void) {
}

StaticTask_t xIdleTaskTCB CCM_RAM;
StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE] CCM_RAM;

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize) {
  /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
  state will be stored. */
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

  /* Pass out the array that will be used as the Idle task's stack. */
  *ppxIdleTaskStackBuffer = uxIdleTaskStack;

  /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
  Note that, as the array is necessarily of type StackType_t,
  configMINIMAL_STACK_SIZE is specified in words, not bytes. */
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

static StaticTask_t xTimerTaskTCB CCM_RAM;
static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH] CCM_RAM;

/* configUSE_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize) {
  *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
  *ppxTimerTaskStackBuffer = uxTimerTaskStack;
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

