#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *----------------------------------------------------------*/

#define configUSE_PREEMPTION		1
#define configUSE_IDLE_HOOK			0
#define configMAX_PRIORITIES		( ( unsigned portBASE_TYPE ) 5 )
#define configUSE_TICK_HOOK			0
#define configCPU_CLOCK_HZ			( 12000000UL )
#define configTICK_RATE_HZ			( ( portTickType ) 10000 )
#define configMINIMAL_STACK_SIZE	( ( unsigned short ) 100 )
#define configTOTAL_HEAP_SIZE		( ( size_t ) ( 4 * 1024 ) )
#define configMAX_TASK_NAME_LEN		( 12 )
#define configUSE_TRACE_FACILITY	1
#define configUSE_16_BIT_TICKS		0
#define configIDLE_SHOULD_YIELD		0
#define configUSE_CO_ROUTINES 		0
#define configUSE_MUTEXES			0

#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

#define configUSE_COUNTING_SEMAPHORES 	0
#define configUSE_ALTERNATIVE_API 		0
#define configCHECK_FOR_STACK_OVERFLOW	0
#define configUSE_RECURSIVE_MUTEXES		0
#define configQUEUE_REGISTRY_SIZE		0
#define configGENERATE_RUN_TIME_STATS	0
#define configUSE_MALLOC_FAILED_HOOK	0

/* Software timer related definitions. */
#define configUSE_TIMERS                        1
#define configTIMER_TASK_PRIORITY               3
#define configTIMER_QUEUE_LENGTH                10
#define configTIMER_TASK_STACK_DEPTH            configMINIMAL_STACK_SIZE

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet			0
#define INCLUDE_uxTaskPriorityGet			0
#define INCLUDE_vTaskDelete					0
#define INCLUDE_vTaskCleanUpResources		0
#define INCLUDE_vTaskSuspend				1
#define INCLUDE_vTaskDelayUntil				1
#define INCLUDE_vTaskDelay					1
#define INCLUDE_uxTaskGetStackHighWaterMark	0

/* Use the system definition, if there is one */
#ifdef __NVIC_PRIO_BITS
	#define configPRIO_BITS       __NVIC_PRIO_BITS
#else
	#define configPRIO_BITS       3        /* 7 priority levels */
#endif

/* The lowest priority. */
#define configKERNEL_INTERRUPT_PRIORITY 		( 7 << (8 - configPRIO_BITS) )
/* Priority 5, or 160 as only the top three bits are implemented. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	( 5 << (8 - configPRIO_BITS) )


#define configEXPECTED_NO_RUNNING_TASKS		( 3 )
extern unsigned long ulTaskNumber[];
#define traceTASK_SWITCHED_IN() 	ulTaskNumber[ pxCurrentTCB->uxTCBNumber ] = 1
#define traceTASK_SWITCHED_OUT() 	ulTaskNumber[ pxCurrentTCB->uxTCBNumber ] = 0



#endif /* FREERTOS_CONFIG_H */

