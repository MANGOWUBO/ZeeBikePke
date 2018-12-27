#ifndef PERIOD_TASK_H
#define PERIOD_TASK_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Queue.h>

//#include <ICall.h>

/*********************************************************************
 * FUNCTIONS
 */

/*
 * Task creation function for the UART.
 */
extern void PeriodTask_createSemaphore(void) ;	 
extern void PeriodTask_createTask(void);


/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* UART_TASK_H */
