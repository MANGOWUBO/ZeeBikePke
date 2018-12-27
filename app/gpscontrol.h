#ifndef UART_TASK_H
#define UART_TASK_H

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


/* This structure can be added to a Queue because the first field is a Queue_Elem. */
typedef struct  
{
	Queue_Elem elem;
	//uint8_t	source ;	//timer	/main/simplepherial 
	uint8_t event ;		//tx event
	uint8_t buffid ;
	uint8_t data;
}UartMessage_TypeDef;

/*********************************************************************
 * FUNCTIONS
 */

/*
 * Task creation function for the UART.
 */
extern void	UartTask_createSemaphore(void);
extern void UartTask_createQueue(void) ;	 
extern void Uart_createTask(void);
extern bool TimerTxIntervalPer2msDecrease(void) ;
extern bool TimerTxSendHeartBeatPer50ms(void) ;
extern bool TimerRxHeartBeatDecreasePer50ms(void);

extern UART_Handle  uart_handle;


extern bool TimerTxResendPer50ms(void) ;

extern void uart_SendData(uint8_t *txBuf, size_t size);
extern void BLEStatusChangeMsg_EnQueue(uint8_t status) ;

extern bool PTFuncLock(void) ;
extern bool PTFuncUnlock(void) ;

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* UART_TASK_H */
