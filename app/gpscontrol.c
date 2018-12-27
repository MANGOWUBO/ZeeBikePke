/*******************************************************************************
Filename:       uart_task.c
Revised:       
Revision:      

Description:   

*******************************************************************************/

/*********************************************************************
* INCLUDES
*/
#include "defines.h"

#include <ti/drivers/UART.h>
#include <ti/drivers/uart/UARTCC26XX.h>

#include "Board.h"

#include "util.h"

#include "gpscontrol.h"
#include "systemcontrol.h"
#include "ATControl.h"
#include "Guard.h"
#include "BatteryPowerCal.h"
#include "KeyLess.h"
#include "OLC.h"
#include "FlashSNV.h"
#include "debug.h"

#include "icall_api.h"
/***************************TypeDef***********************************/
typedef union
{
	uint8_t status ;
	struct
	{
		uint8_t flag_filled 		:1	;
		uint8_t flag_ready			:1	;	
		uint8_t flag_sending		:1 	;
		//uint8_t flag_needresponse	:1 	;
		uint8_t reserve				:5	;
	};	
}TxBuffStatus_TypeDef;

#define MAX_NUM_RX_BYTES      		40   // Maximum RX bytes to receive in one go
#define MAX_NUM_TX_BYTES      		30   // Maximum TX bytes to send in one go

typedef struct
{
	uint8_t id ;
	TxBuffStatus_TypeDef buffstatus ;
	uint8_t	data[MAX_NUM_TX_BYTES] ;
	uint8_t length_valid ;
	uint8_t	counter_retry			;		//if equel 0 means no longer need resend 
	uint8_t	timer_resend_per50ms	;
	uint8_t timer_resend_temp_per50ms		;
		
}UartBuff_TypeDef ;

UartBuff_TypeDef UartBuff_table[] = {
	{
		.id =  0 ,//tx_buff0 ,
		.buffstatus.status = 0 ,
		.length_valid = 0 ,
		.counter_retry = 0 ,
		.timer_resend_per50ms = 0XFF ,
		.timer_resend_temp_per50ms	= 0XFF 
	},
	{
		.id = 1 ,// tx_buff1 ,
		.buffstatus.status = 0 ,
		.length_valid = 0 ,
		.counter_retry = 0 ,
		.timer_resend_per50ms = 0XFF ,
		.timer_resend_temp_per50ms	= 0XFF 
	},	
	{
		.id =  2 ,//tx_buff0 ,
		.buffstatus.status = 0 ,
		.length_valid = 0 ,
		.counter_retry = 0 ,
		.timer_resend_per50ms = 0XFF ,
		.timer_resend_temp_per50ms	= 0XFF 
	}
};

typedef void (*TxFunc_TypeDef)(uint8_t buffid,uint8_t startbyte,uint8_t data);			//ResponseStr_TypeDef is a funtion ArrInfo(*func)(uint8_t *)
typedef bool (*RxFunc_TypeDef)(uint8_t startbyte);	

typedef bool (*PTCmdFunc_TypeDef)(void);

typedef struct
{
	const uint8_t 	id ;
	const uint8_t 	source ;
	const uint8_t  	cmd;			
	const uint8_t 	rsp_cmd ;		//the string to compare with incoming string
	uint8_t 		tx_data_len	;
	const uint8_t 	tx_counter_retry ;
	const uint8_t 	tx_timer_resend_per50ms ;
	TxFunc_TypeDef	txFunc ;
	uint8_t 		rx_data_len ;
	RxFunc_TypeDef	rxFunc ;
	//RspFunc_TypeDef ResponseFunc;		//the function return the string
}GPSRXTXFunc_TypeDef;

typedef enum
{
	Rec_No_Match = 0 ,
	RecNotPT_Fail  ,
	RecNotPT_Success  ,
	RecPT_Fail ,
	RecPT_Success ,
}RecResultStatus_TypeDef ;
	
typedef struct
{
	uint8_t processid_match ;
	uint8_t buffid_match ;
	uint8_t cmd_match ;
	//uint8_t serialnum_match ;
	RecResultStatus_TypeDef status ;
}RecResult_TypeDef;

typedef struct
{
	const uint8_t pt_id ;
	const uint16_t rx_ptcmd ;
	const uint16_t tx_ptcmd ;
	const uint8_t  tx_pt_data_len ;
	const uint8_t  rx_pt_data_len ;
	PTCmdFunc_TypeDef ptfunc ;
	TxFunc_TypeDef  pttxfunc ;
}PTMatch_TypeDef ;

/*********************************************************************
* CONSTANTS
*/
// Task configuration
#define UART_TASK_PRIORITY      	2

#ifndef UART_TASK_STACK_SIZE
#define UART_TASK_STACK_SIZE     	644
#endif

#define UART_READ_EVT        		0x0001



#define	Timer_Tx_Interval_Per2ms	15	//15*2 = 30ms		

#define	UART_HEARTBEAT_EVENT			0X01
#define	UART_RXNEWMESSAGE_EVENT			0X02
#define	UART_GPS_CONTROL_EVENT			0X04
#define UART_SBP_CONTROL_EVENT			0X08
#define	UART_AFTER_INTERVAL_EVENT		0X10
#define	UART_RESEND_EVENT				0X20
#define	UART_DISCONNECT_EVENT			0X40
#define	UART_BLESTATUSCHANGED_EVENT		0X80

#define	UartBuffStatus_FlagNotBlankBit		U8_BIT0
#define	UartBuffStatus_FlagReadyBit			U8_BIT1
#define	UartBuffStatus_FlagSendingBit		U8_BIT2


#define	Timer_TX_HeartBeat_Per50ms		60	//3s
#define	Timer_Resend_Per50ms			20	//1s
#define	Timer_RX_HeartBeat_Per50ms		180	//9s

#define	RESEND_FOREVER					0xff
#define	RESEND_NONE						0
#define	RESEND_NORMAL					3


/****************************************************/
/**********************string define************************/
/**********************Package Format************************/
#define		GPS_TX_LEN_MIN						12
#define		GPS_RX_LEN_MIN						12
#define		GPS_PT_RSP_DATA_LEN					7					

#define		GPS_StartByte_1						0X24
#define		GPS_StartByte_2						0X24
#define		GPS_CheckByte_2						0XAA
#define		GPS_EndByte_1						0X0D
#define		GPS_EndByte_2						0X0A

#define		GPS_Start1_ArrNum					0
#define		GPS_Start2_ArrNum					1
#define		GPS_ValidDataLen_ArrNum				2
#define		GPS_Passway_ArrNum					3
#define		GPS_Cmd_ArrNum						4
#define		GPS_MinLen_Data_ArrNum				5
#define		GPS_MinLen_SrlNum1_ArrNum			6
#define		GPS_MinLen_SrlNum2_ArrNum			7
#define		GPS_MinLen_ChkCode1_ArrNum			8
#define		GPS_MinLen_ChkCode2_ArrNum			9
#define		GPS_MinLen_EndCode1_ArrNum			10
#define		GPS_MinLen_EndCode2_ArrNum			11



#define	GPS_FromLast_SrlNum1_ArrNum			6
#define	GPS_FromLast_SrlNum2_ArrNum			5
#define	GPS_FromLast_ChkCode1_ArrNum		4
#define	GPS_FromLast_ChkCode2_ArrNum		3
#define	GPS_FromLast_EndCode1_ArrNum		2
#define	GPS_FromLast_EndCode2_ArrNum		1


/**********************Passageway************************/
#define		CM2PT								0X01
#define		CPT2M								0X02
#define		CPT2S								0X03
#define		CS2PT								0X04
#define		MM2PT								0X05
#define		MPT2M								0X06
#define		MPT2S								0X07
#define		MS2PT								0X08

/**********************Cmd ID************************/
#define		Cmd_SetHostType						0X01
#define		Cmd_GPRSControlLock					0X05
#define		Cmd_SetDeviceNum					0X06
#define		Cmd_UpdateDeviceNum					0X07
#define		Cmd_QueryACCOffVoltage				0X0B
#define		Cmd_QueryGPSStatus					0X10
#define		Cmd_ConnectPackage					0XDF


	
/**********************ReCmd ID************************/
#define		ReCmd_SetHostType					0X01
#define		ReCmd_GPRSControlLock				0X05
#define		ReCmd_SetDeviceNum					0X06
#define		ReCmd_UpdateDeviceNum				0X07
#define		ReCmd_QueryGPSStatus				0X10
#define		ReCmd_ConnectPackage				0XDF	

/**************Strlen Data Cmd************/
#define		Strlen_ValidFixed_ExceptN			6

#define		StrlenN_Send_HostType				1
#define		StrlenN_Send_GPRSControlLock		1
#define		StrlenN_Send_SetDeviceNum			12
#define		StrlenN_Send_UpdateDeviceNum		12
#define		StrlenN_Send_QueryGPSStatus			1
#define		StrlenN_Send_QueryACCOffVoltage		1
#define		StrlenN_Send_ConnectPackage			1

/**************Strlen Data ReCmd************/
#define		StrlenN_Rec_HostType				1
#define		StrlenN_Rec_GPRSControlLock			1
#define		StrlenN_Rec_SetDeviceNum			1
#define		StrlenN_Rec_UpdateDeviceNum			1
#define		StrlenN_Rec_QueryGPSStatus			3
#define		StrlenN_Rec_ConnectPackage			1

/**************Message passthrough CmdID************/
#define		MCmd_PassthroughSend				0XAA
#define		MCmd_PassthroughResponse			0XA0


/**********************PassthroughCmd ID************************/
#define		PTCmd_Unlock						0X0001
#define		PTCmd_Lock							0X0002
#define		PTCmd_UpdateRentTime				0X0003
#define		PTCmd_ResetBLE						0X0004
#define		PTCmd_ResetMCU						0X0005
#define		PTCmd_QueryBLEStatus				0X0006
#define		PTCmd_ACCPowerOn					0X0007
#define		PTCmd_ACCPowerOff					0X0008
#define		PTCmd_QueryVersion					0X0009
#define		PTCmd_QueryRentDeadLine				0X000A


#define		PTCmd_Status						0X0101		//lock status and Q percent
#define		PTCmd_Version						0X0102
#define		PTCmd_RentDeadLine					0X0103



/**************Strlen Data PassThrough************/
#define		StrlenValidData_UpdateStatus		(2 + 1 + 1 + 1 + 1 )
#define		StrlenValidData_Test1				5

#define		StrlenValidData_Version				(2 + 2 + 3)
#define		StrlenValidData_RentDeadLine		(2 + 2 + 1 + 1 + 1 )




/*********************************************************************
* LOCAL VARIABLES
*/

// Entity ID globally used to check for source and/or destination of messages
// 线程注册到ICALL时分配的ID
//static ICall_EntityID uart_selfEntity;

// Semaphore globally used to post events to the application thread
// 信号量，app线程没有事情处理时是阻塞状态，发信号量给app线程唤醒线程
//ICall_Semaphore uart_sem;
static Semaphore_Struct uart_semStruct;
static Semaphore_Handle uart_semHandle;

static Queue_Struct uart_queueStruct;
static Queue_Handle uart_queueHandle;

UartMessage_TypeDef uartmessage_fromRx ;
UartMessage_TypeDef uartmessage_fromaftintvl ;
UartMessage_TypeDef uartmessage_fromresend ;
UartMessage_TypeDef uartmessage_fromTxHeartBeat ;
UartMessage_TypeDef uartmessage_fromRxHeartBeat ;
UartMessage_TypeDef uartmessage_fromBLEstatuschanged ;

UartMessage_TypeDef *uart_message ;

// Task configuration
Task_Struct uartTask;
Char uartTaskStack[UART_TASK_STACK_SIZE];

//static UART_Handle  uart_handle;
 UART_Handle  uart_handle=NULL;
static uint8_t   rxBuf[MAX_NUM_RX_BYTES];  // Receive buffer
static uint8_t   length_rxmsg = 0 ;

static uint8_t timer_tx_interval_per2ms = 0 ;
static uint8_t timer_tx_heartbeat_per50ms = Timer_TX_HeartBeat_Per50ms ;
static uint8_t timer_rx_heartbeat_per50ms = 0 ;


static uint8_t process_id = 0 ;

static UnionSumWithH8L8_TypeDef  num_serial ;
static RecResult_TypeDef RecResult ;


/*********************************************************************
* LOCAL FUNCTIONS
*/
static void Uart_taskFxn(UArg a0, UArg a1);
static void Uart0_init(void);

static void readCallback(UART_Handle handle, void *rxBuf, size_t size);
static void writeCallback(UART_Handle handle, void *txBuf, size_t size) ;
static bool IsConstTXLengthLessThanTxBuffLength(void) ;
static bool IsConstRXLengthLessThanRxBuffLength(void) ;
static uint8_t GetBlankUartBuffID(void) ;
//static bool SendUartBuffToTx(uint8_t *pBuffData,uint8_t len,TxBuffStatus_TypeDef * pStatus) ;
static bool SendUartBuffToTx(uint8_t buffid) ;
static bool ProcessTxAfterSendIntervalEvt(uint8_t buffid) ;
static bool IsNeedToSendProcessIDCmdWhenReceiveHeartBeat(uint8_t * process) ;
static bool ProcessRxNewMessageEvt(void);
static bool FillAndSendMsg(uint8_t processid_temp, uint8_t tx_data_len,uint8_t data) ;
static uint8_t FillCmdDataToUartBuff(uint8_t target_buffid ,uint8_t process_id_temp,uint8_t data) ;
static void RewriteSerialNum(uint8_t target_buffid) ;
static void WriteCheckCode(uint8_t target_buffid) ;
static void ConfigTxFlag(uint8_t target_buffid , uint8_t process_id_temp) ;

static bool ClearSpecifiedUartBuffTxFlagAndRetryCounter(uint8_t buffid) ;
static bool ProcessBLEStatusChangedEvt(uint8_t data) ;

static bool ProcessTxSendHeartBeatEvt(void) ;
static bool ProcessTxResendEvt(uint8_t buffid) ;
static bool ProcessDisConnectEvt(void) ;

//static bool GPSControl(void) ;
static uint8_t GetCheckCode(uint8_t * Buff ,uint8_t length)  ;
static bool IsRxMsgFormatCorrect(void) ;
//static uint8_t GetRecMatchProcessID(uint8_t startbyte) ;
//static bool IsRspInfor(void) ;
static bool IsHeartBeatRsp(void) ;
static bool IsPTSendInfor(void) ;
//static bool IsPTRsp(void) ;
static bool IsGPRSControlLockCmd(void) ;


//static uint8_t GetRxMatchTxBuffID(void) ;

static void SetRxHeartBeatTimer(void) ;
static bool GetTxBuffIDMatchWithRx(RecResult_TypeDef *result) ;
static bool IsSerialNumSameWithRx(uint8_t tx_buffid) ;
static uint8_t GetRxMatchProcessID(void)  ;

static bool IsRxHeartBeatDisconnectBefore(void) ;
static bool IsInPassThroughStatus(void) ;



//static uint8_t GetRXMatchTXBuffID(void) 


//static bool GetValidInformInRxMsg(void);

static void FillConnectPackageToUartBuff(uint8_t buffid,uint8_t startbyte ,uint8_t data);
static void FillSetHostTypeToUartBuff(uint8_t buffid,uint8_t startbyte,uint8_t data);
static void FillSetDeviceNumToUartBuff(uint8_t buffid,uint8_t startbyte,uint8_t data);
static void FillQueryGPSStatusToUartBuff(uint8_t buffid,uint8_t startbyte,uint8_t data);
//static void FillBLEStatusToUartBuff(uint8_t buffid,uint8_t startbyte);

//static bool CompareRecWithConnectPackageData(uint8_t startbyte);
static bool CompareRecWithHostTypeData(uint8_t startbyte);
static bool CompareRecWithDeviceNumData(uint8_t startbyte);
static bool CompareRecWithGPSStatusData(uint8_t startbyte);
static bool CompareRecWithPassthrough(uint8_t startbyte) ;

static UnionSumWithH8L8_TypeDef GetRecPTSerialNum(void) ;
static uint16_t GetRecPTCmd(void) ;
static bool FillPTRspToUartBuff(uint8_t target_buffid,UnionSumWithH8L8_TypeDef serialnum_pt,bool status) ;
static bool SendPTRsp(bool status)  ;
static uint8_t MatchPTIDWithPTCmd(uint16_t ptcmd) ;







static bool PTFuncControl(void) ;


static bool PTFuncUpdateRentDeadline(void) ;
static bool PTFuncResetBLE(void) ;
static bool PTFuncResetMCU(void) ;
static bool PTFuncQueryBLEStatus(void) ;
static bool PTFuncACCPowerOn(void) ;
static bool PTFuncACCPowerOff(void) ;
static bool PTFuncQueryVersion(void) ;
static bool PTFuncQueryRentDeadLine(void) ;

static void PTTxFuncSendBLEStatus(uint8_t buffid, uint8_t startbyte,uint8_t data) ;
static void PTTxFuncSendVersion(uint8_t buffid, uint8_t startbyte,uint8_t data) ;
static void PTTxFuncSendRentDeadLine(uint8_t buffid, uint8_t startbyte,uint8_t data) ;

	
GPSRXTXFunc_TypeDef RXTXFunc[] ={
	{
		.id =  0 ,
		.source = CM2PT ,//01
		.cmd = Cmd_ConnectPackage ,
		.rsp_cmd = Cmd_ConnectPackage ,
		.tx_data_len = StrlenN_Send_ConnectPackage ,
		.tx_counter_retry  = RESEND_NONE ,
		.tx_timer_resend_per50ms = Timer_TX_HeartBeat_Per50ms ,
		.txFunc = FillConnectPackageToUartBuff ,
		.rx_data_len = StrlenN_Rec_ConnectPackage ,
		.rxFunc = NULL
	},
	{
		.id =  1 ,
		.source = CM2PT ,//01
		.cmd = Cmd_SetHostType ,
		.rsp_cmd = Cmd_SetHostType ,
		.tx_data_len = StrlenN_Send_HostType ,
		.tx_counter_retry  = RESEND_FOREVER ,
		.tx_timer_resend_per50ms = Timer_Resend_Per50ms ,
		.txFunc = FillSetHostTypeToUartBuff ,//TxFuncSetHostType ,
		.rx_data_len = StrlenN_Rec_HostType ,
		.rxFunc = CompareRecWithHostTypeData
	},
	{
		.id =  2 ,
		.source = CM2PT ,//01
		.cmd = Cmd_SetDeviceNum ,
		.rsp_cmd = Cmd_SetDeviceNum ,
		.tx_data_len = StrlenN_Send_SetDeviceNum ,
		.tx_counter_retry  = RESEND_FOREVER ,
		.tx_timer_resend_per50ms = Timer_Resend_Per50ms ,
		.txFunc = FillSetDeviceNumToUartBuff ,
		.rx_data_len = StrlenN_Rec_SetDeviceNum ,
		.rxFunc = CompareRecWithDeviceNumData
	},
	{
		.id =  3 ,
		.source = CM2PT ,//01
		.cmd = Cmd_QueryGPSStatus ,
		.rsp_cmd = Cmd_QueryGPSStatus ,
		.tx_data_len = StrlenN_Send_QueryGPSStatus ,
		.tx_counter_retry  = RESEND_FOREVER ,
		.tx_timer_resend_per50ms = Timer_Resend_Per50ms ,
		.txFunc = FillQueryGPSStatusToUartBuff ,	
		.rx_data_len = StrlenN_Rec_QueryGPSStatus ,
		.rxFunc = CompareRecWithGPSStatusData
	},
	{
		.id =  4 ,
		.source = MM2PT ,//05
		.cmd = MCmd_PassthroughSend ,
		.rsp_cmd = MCmd_PassthroughResponse,
		//.tx_data_len = StrlenN_Send_SetDeviceNum ,
		.tx_counter_retry  = RESEND_NORMAL ,
		.tx_timer_resend_per50ms = Timer_Resend_Per50ms ,
		.txFunc = PTTxFuncSendBLEStatus,//FillBLEStatusToUartBuff ,
		
		//.rx_data_len = StrlenN_Rec_SetDeviceNum ,
		.rxFunc = CompareRecWithPassthrough
	},
};

#define		ProcessID_MM2PT			(sizeof(RXTXFunc)/sizeof(GPSRXTXFunc_TypeDef)-1)

const PTMatch_TypeDef PT[] = {
	{
		.pt_id = 0 ,
		.rx_ptcmd = PTCmd_Unlock ,
		.tx_pt_data_len = 0 ,
		.rx_pt_data_len = 2 ,
		.ptfunc = PTFuncUnlock 
	},	
	{
		.pt_id = 1 ,
		.rx_ptcmd = PTCmd_Lock ,
		.tx_pt_data_len = 0 ,
		.rx_pt_data_len = 2 ,
		.ptfunc = PTFuncLock 
	},	
	{
		.pt_id = 2 ,
		.rx_ptcmd = PTCmd_UpdateRentTime ,
		.tx_pt_data_len = 0 ,
		.rx_pt_data_len = 5 ,
		.ptfunc = PTFuncUpdateRentDeadline
	},	
	{
		.pt_id = 3 ,
		.rx_ptcmd = PTCmd_ResetBLE ,
		.tx_pt_data_len = 0 ,
		.rx_pt_data_len = 2 ,
		.ptfunc = PTFuncResetBLE
	},	
	{
		.pt_id = 4 ,
		.rx_ptcmd = PTCmd_ResetMCU ,
		.tx_pt_data_len = 0 ,
		.rx_pt_data_len = 2 ,
		.ptfunc = PTFuncResetMCU
	},	
	{
		.pt_id = 5 ,
		.rx_ptcmd = PTCmd_QueryBLEStatus ,
		.tx_ptcmd = PTCmd_Status ,
		.tx_pt_data_len = 6 ,
		.rx_pt_data_len = 2 ,
		.ptfunc = PTFuncQueryBLEStatus,
		.pttxfunc = PTTxFuncSendBLEStatus
	},
	{
		.pt_id = 6 ,
		.rx_ptcmd = PTCmd_ACCPowerOn ,
		.tx_pt_data_len = 0 ,
		.rx_pt_data_len = 2 ,
		.ptfunc = PTFuncACCPowerOn
	},
	{
		.pt_id = 7 ,
		.rx_ptcmd = PTCmd_ACCPowerOff ,
		.tx_pt_data_len = 0 ,
		.rx_pt_data_len = 2 ,
		.ptfunc = PTFuncACCPowerOff
	},
	{
		.pt_id = 8 ,
		.rx_ptcmd = PTCmd_QueryVersion ,
		.tx_ptcmd = PTCmd_Version ,
		.tx_pt_data_len = 7 ,
		.rx_pt_data_len = 2 ,
		.ptfunc = PTFuncQueryVersion,
		.pttxfunc = PTTxFuncSendVersion
	},
	{
		.pt_id = 9 ,
		.rx_ptcmd = PTCmd_QueryRentDeadLine ,
		.tx_ptcmd = PTCmd_RentDeadLine ,
		.tx_pt_data_len = 7 ,
		.rx_pt_data_len = 2 ,
		.ptfunc = PTFuncQueryRentDeadLine,
		.pttxfunc = PTTxFuncSendRentDeadLine
	},
};

#define		PT_NUM_LAST			(sizeof(PT)/sizeof(PTMatch_TypeDef)-1)

/*********************************************************************
* @brief   Semaphore creation function for the UartTask.
*
* @param   None.
*
* @return  None.
*/
void UartTask_createSemaphore(void)
{
	Semaphore_Params uart_semParams;
      /* Construct a Semaphore object to be use as a resource lock, inital count 1 */
    Semaphore_Params_init(&uart_semParams);
	//defalt is counting mode
    Semaphore_construct(&uart_semStruct, 0, &uart_semParams);

    /* Obtain instance handle */
    uart_semHandle = Semaphore_handle(&uart_semStruct);
}
	

/*********************************************************************
* @brief   Queue creation function for the UartTask.
*
* @param   None.
*
* @return  None.
*/
void UartTask_createQueue(void)
{
	Queue_Params uart_queueParams;
	/* Construct a Semaphore object to be use as a resource lock, inital count 1 */
	Queue_Params_init(&uart_queueParams);
	Queue_construct(&uart_queueStruct, &uart_queueParams);
	
	/* Obtain instance handle */
	uart_queueHandle = Queue_handle(&uart_queueStruct);
}

/*********************************************************************
* @fn      SimpleBLEPeripheral_createTask
*
* @brief   Task creation function for the Simple BLE Peripheral.
*
* @param   None.
*
* @return  None.
*/
void Uart_createTask(void)
{
	Task_Params taskParams;
	
	// Configure task
	Task_Params_init(&taskParams);
	taskParams.stack = uartTaskStack;
	taskParams.stackSize = UART_TASK_STACK_SIZE;
	taskParams.priority = UART_TASK_PRIORITY;
	
	Task_construct(&uartTask, Uart_taskFxn, &taskParams, NULL);
}

/*********************************************************************
* @fn      Uart_init
*
* @brief  
*
* @param   None.
*
* @return  None.
*/
static void Uart0_init(void)
{	
	UART_Params      params;
    UART_init();
	// Init UART and specify non-default parameters
	UART_Params_init(&params);
	params.baudRate      = 9600 ;
	params.writeMode	 = UART_MODE_CALLBACK;
	params.writeDataMode = UART_DATA_BINARY;
	params.writeCallback = writeCallback;
	params.readMode      = UART_MODE_CALLBACK;
	params.readDataMode  = UART_DATA_BINARY;
	params.readCallback  = readCallback;

	if((true == IsConstRXLengthLessThanRxBuffLength())&&(true == IsConstTXLengthLessThanTxBuffLength()))
    {
		// Open the UART and do the read
		uart_handle = UART_open(Board_UART, &params);
		// Enable RETURN_PARTIAL   for receive unknow len data
		UART_control(uart_handle, UARTCC26XX_CMD_RETURN_PARTIAL_ENABLE, NULL);
		//start uart read 
		UART_read(uart_handle, rxBuf, MAX_NUM_RX_BYTES);
			
		num_serial.sum = 0 ;
	}
		
}

/*********************************************************************
* @fn      Uart_taskFxn
*
* @brief  
*
* @param   a0, a1 - not used.
*
* @return  None.
*/
static void Uart_taskFxn(UArg a0, UArg a1)
{
	// Initialize application
	Uart0_init();

	// Application main loop
	for (;;)
	{
		/* Get access to resource */
		Semaphore_pend(uart_semHandle, BIOS_WAIT_FOREVER);
		if(!Queue_empty(uart_queueHandle))
		{
			uart_message = Queue_dequeue(uart_queueHandle) ;
			if(UART_AFTER_INTERVAL_EVENT == uart_message->event)
				ProcessTxAfterSendIntervalEvt(uart_message->buffid) ; 
			else if(UART_RXNEWMESSAGE_EVENT == uart_message->event)
				ProcessRxNewMessageEvt() ;
			else if(UART_RESEND_EVENT == uart_message->event)
				ProcessTxResendEvt(uart_message->buffid) ;
			else if(UART_BLESTATUSCHANGED_EVENT == uart_message->event)
				ProcessBLEStatusChangedEvt(uart_message->data) ;
			else if(UART_DISCONNECT_EVENT == uart_message->event)
				ProcessDisConnectEvt() ;                                       
			else if(UART_HEARTBEAT_EVENT == uart_message->event)
				ProcessTxSendHeartBeatEvt() ;			
			else  
				;  
			/*free(uart_message);
			uart_message = NULL;*/
		}	

	}
}

/*******************************************************************************
*
*/
static void readCallback(UART_Handle handle, void *rxBuf, size_t size)
{	
	length_rxmsg = size ;
	uartmessage_fromRx.event = 	UART_RXNEWMESSAGE_EVENT ;
	uartmessage_fromRx.buffid = 1 ;
	// Wake up the application.
	Queue_enqueue(uart_queueHandle, &(uartmessage_fromRx.elem));
	Semaphore_post(uart_semHandle);
	
	
	// Start another read, with size the same as it was during first call to
	// UART_read()
	UART_read(uart_handle, rxBuf, MAX_NUM_RX_BYTES);
}

/*******************************************************************************
*
*/
static void writeCallback(UART_Handle handle, void *txBuf, size_t size)
{
	timer_tx_interval_per2ms  = Timer_Tx_Interval_Per2ms; 
}

/*******************************************************************************
*
*/
static bool IsConstTXLengthLessThanTxBuffLength(void)
{
	for(uint8_t i =0 ; i <= ProcessID_MM2PT ; i++)
	{
		if((RXTXFunc[i].tx_data_len + 11 ) > MAX_NUM_TX_BYTES )
			return false ;
	}
	for(uint8_t i = 0 ; i <= PT_NUM_LAST;i++)
	{
		if((PT[i].tx_pt_data_len + 11 ) > MAX_NUM_TX_BYTES)
			return false ;
	}
	return true ;
}

/*******************************************************************************
*
*/
static bool IsConstRXLengthLessThanRxBuffLength(void)
{
	for(uint8_t i =0 ; i <= ProcessID_MM2PT ; i++)
	{
		if((RXTXFunc[i].rx_data_len + 11 ) > MAX_NUM_RX_BYTES )
			return false ;
	}
	for(uint8_t i = 0 ; i <= PT_NUM_LAST ;i++)
	{
		if((PT[i].rx_pt_data_len + 11 ) > MAX_NUM_RX_BYTES)
			return false ;
	}
	return true ;
}



/*******************************************************************************
*
*/
static uint8_t GetBlankUartBuffID(void)
{
	for(uint8_t i = 0 ;i < sizeof(UartBuff_table)/sizeof(UartBuff_TypeDef);i++)
	{
		if(!UartBuff_table[i].buffstatus.flag_filled)
			return i ;
	}
	return 0Xff ;
}

/*******************************************************************************
*
*/
//static bool SendUartBuffToTx(uint8_t *pBuffData,uint8_t len,TxBuffStatus_TypeDef * pStatus)
static bool SendUartBuffToTx(uint8_t buffid)
{
	for(uint8_t i = 0 ; i < sizeof(UartBuff_table)/sizeof(UartBuff_TypeDef) ; i ++)
		if(UartBuff_table[i].buffstatus.flag_sending )
			return false ;
		
	UartBuff_table[buffid].buffstatus.flag_ready = 0 ;		//clear ready flag
	UartBuff_table[buffid].buffstatus.flag_sending= 1 ;		//set sending flag
	UartBuff_table[buffid].timer_resend_temp_per50ms = UartBuff_table[buffid].timer_resend_per50ms ;

	UART_write(uart_handle, UartBuff_table[buffid].data,UartBuff_table[buffid].length_valid);	
	return true ;
}

/*******************************************************************************
*
*
*/
bool TimerTxIntervalPer2msDecrease(void)
{
	if(true == IsVariableTimerDecreaseToZero(&timer_tx_interval_per2ms))
	{
		for(uint8_t i = 0 ;i < sizeof(UartBuff_table)/sizeof(UartBuff_TypeDef);i++)
		{
			if(UartBuff_table[i].buffstatus.flag_filled && UartBuff_table[i].buffstatus.flag_sending)
			{					
				uartmessage_fromaftintvl.event =	UART_AFTER_INTERVAL_EVENT ;
				uartmessage_fromaftintvl.buffid = i ;
				
				// Wake up the application.
				Queue_enqueue(uart_queueHandle, &(uartmessage_fromaftintvl.elem));
				Semaphore_post(uart_semHandle);
				
				return true ;
			}
		}	
	}
	return false ;
}

/*******************************************************************************
*
*/
static bool ProcessTxAfterSendIntervalEvt(uint8_t buffid)
{
	UartBuff_table[buffid].buffstatus.flag_sending = 0 ;
	if(0 == UartBuff_table[buffid].counter_retry)
		UartBuff_table[buffid].buffstatus.flag_filled = 0 ;		//no response data  need to clear flag_filled 
			
	for(uint8_t i = 0 ;i < sizeof(UartBuff_table)/sizeof(UartBuff_TypeDef);i++)
	{
		if(UartBuff_table[i].buffstatus.flag_ready)
		{
			SendUartBuffToTx(i) ;
			return true ;
		}
	}
	return false ;
}

/*******************************************************************************
*
*/
static bool IsNeedToSendProcessIDCmdWhenReceiveHeartBeat(uint8_t * process)
{
	if(0 == (*process))
	{	
		(*process) ++ ;
		return true ;
	}
	if(true == IsRxHeartBeatDisconnectBefore())
		return true ;

	return false ;
}

/*******************************************************************************
*
*/
static bool ProcessRxNewMessageEvt()
{
	if(false == IsRxMsgFormatCorrect() )
	{
		if(false == IsPTSendInfor())
			return false ;
		if(false == IsInPassThroughStatus())
			return false ;
		if(false == SendPTRsp(false) )
			return false ;
		return true ;
	}

	if(IsGPRSControlLockCmd())
		return false ;

	//Heart beat rsp process
	if(IsHeartBeatRsp())
	{				
		
		if(true == IsNeedToSendProcessIDCmdWhenReceiveHeartBeat(&process_id))	//this function will check rx_heartbeat
		{	
			SetRxHeartBeatTimer() ;		//clear heart beat
			FillAndSendMsg(process_id, 1,0XFF);
			return true ;
		}
		SetRxHeartBeatTimer() ;			//clear heart beat
		return false ;
	}

	if(IsPTSendInfor())
	{
		if(true == PTFuncControl())
			return true ;
		return false ;
	}
		
	//other rsp process
	//get rxmessage cmd -> proceeid
	RecResult.processid_match = GetRxMatchProcessID() ;
	if(RecResult.processid_match > ProcessID_MM2PT)
		return false ;
	
	GetTxBuffIDMatchWithRx(&RecResult) ;
	
	if(Rec_No_Match == RecResult.status)
		return false ;

	ClearSpecifiedUartBuffTxFlagAndRetryCounter(RecResult.buffid_match ) ;
	if(RecPT_Success == RecResult.status)	//do nothing
		return false;
	
	if(RecNotPT_Success== RecResult.status) 
	{	
		if(process_id == ProcessID_MM2PT)
			return false ;
		if(process_id != RecResult.processid_match)
			return false ;
		
		process_id ++ ;
		FillAndSendMsg(process_id, 5 ,0XFF) ;	//when pt ,send first ptsend = queryBLEstatus	
		return true ;
	}
	

	// if(RecNotPT_Fail== RecResult.status) ||(RecPT_Fail == RecResult.status)
	RewriteSerialNum(RecResult.buffid_match) ;
	WriteCheckCode(RecResult.buffid_match) ;
	ConfigTxFlag(RecResult.buffid_match, RecResult.processid_match) ;
	//if	other buff sending ,will send next time after 30ms interval 	else	 send buff
	SendUartBuffToTx(RecResult.buffid_match) ;		
	return true ;
}

/*******************************************************************************
*pt_id : just for pt send ,useless for other cmd.
*			
*/
static bool FillAndSendMsg(uint8_t processid_temp ,uint8_t pt_id,uint8_t data)
{
	uint8_t blank_buff = 0xFF ;
	blank_buff = GetBlankUartBuffID() ;
	if(0XFF == blank_buff)
		return false ;

	if(processid_temp == ProcessID_MM2PT)
	{
		RXTXFunc[processid_temp].tx_data_len = PT[pt_id].tx_pt_data_len + 1 ;
		UartBuff_table[blank_buff].data[GPS_MinLen_Data_ArrNum] = PT[pt_id].tx_pt_data_len;
		RXTXFunc[processid_temp].txFunc = PT[pt_id].pttxfunc ;
	}
	FillCmdDataToUartBuff(blank_buff,processid_temp,data);
	WriteCheckCode(blank_buff) ;
	ConfigTxFlag(blank_buff, processid_temp) ;
	//if	other buff sending ,will send next time after 30ms interval 	else	 send buff
	SendUartBuffToTx(blank_buff) ;

	return true ;
}

/*******************************************************************************
*input arg:if 		passthrough,arg = data len
*			else				arg = 0 
*/
static uint8_t FillCmdDataToUartBuff(uint8_t target_buffid ,uint8_t process_id_temp,uint8_t data)
{
	if(target_buffid < sizeof(UartBuff_table)/sizeof(UartBuff_TypeDef))
	{
		num_serial.sum ++ ;
		//PT Data len is custpmer
		UartBuff_table[target_buffid].length_valid = GPS_TX_LEN_MIN + RXTXFunc[process_id_temp].tx_data_len - 1 ;
		
		UartBuff_table[target_buffid].data[GPS_Start1_ArrNum] = GPS_StartByte_1 ;
		UartBuff_table[target_buffid].data[GPS_Start2_ArrNum] = GPS_StartByte_2 ;
		UartBuff_table[target_buffid].data[GPS_ValidDataLen_ArrNum] = 6 + RXTXFunc[process_id_temp].tx_data_len  ;
		UartBuff_table[target_buffid].data[GPS_Passway_ArrNum] = RXTXFunc[process_id_temp].source ;
		UartBuff_table[target_buffid].data[GPS_Cmd_ArrNum] = RXTXFunc[process_id_temp].cmd  ;
		UartBuff_table[target_buffid].data[UartBuff_table[target_buffid].length_valid - GPS_FromLast_SrlNum1_ArrNum] = num_serial.h8	;
		UartBuff_table[target_buffid].data[UartBuff_table[target_buffid].length_valid - GPS_FromLast_SrlNum2_ArrNum] = num_serial.l8	;
		UartBuff_table[target_buffid].data[UartBuff_table[target_buffid].length_valid - GPS_FromLast_ChkCode2_ArrNum] = GPS_CheckByte_2	;
		UartBuff_table[target_buffid].data[UartBuff_table[target_buffid].length_valid - GPS_FromLast_EndCode1_ArrNum] = GPS_EndByte_1  ;
		UartBuff_table[target_buffid].data[UartBuff_table[target_buffid].length_valid - GPS_FromLast_EndCode2_ArrNum] = GPS_EndByte_2  ;
		RXTXFunc[process_id_temp].txFunc(target_buffid,GPS_MinLen_Data_ArrNum, data) ;
	}
	return target_buffid ;
}

/*******************************************************************************
*
*			
*/
static void RewriteSerialNum(uint8_t target_buffid)
{
	num_serial.sum ++ ;	
	UartBuff_table[target_buffid].data[UartBuff_table[target_buffid].length_valid - GPS_FromLast_SrlNum1_ArrNum] = num_serial.h8 ;
	UartBuff_table[target_buffid].data[UartBuff_table[target_buffid].length_valid - GPS_FromLast_SrlNum2_ArrNum] = num_serial.l8 ;
}

/*******************************************************************************
*
*			
*/
static void WriteCheckCode(uint8_t target_buffid)
{
	//check code
	UartBuff_table[target_buffid].data[UartBuff_table[target_buffid].length_valid - GPS_FromLast_ChkCode1_ArrNum] = \
				GetCheckCode(UartBuff_table[target_buffid].data, UartBuff_table[target_buffid].length_valid)	;
		

}
/*******************************************************************************
*
*			
*/
static void ConfigTxFlag(uint8_t target_buffid,uint8_t process_id_temp)
{
	//resend message
	UartBuff_table[target_buffid].timer_resend_per50ms = RXTXFunc[process_id_temp].tx_timer_resend_per50ms ;
	UartBuff_table[target_buffid].counter_retry = 	RXTXFunc[process_id_temp].tx_counter_retry ;
	
	UartBuff_table[target_buffid].buffstatus.flag_filled = 1 ;
	UartBuff_table[target_buffid].buffstatus.flag_ready = 1 ;		
}


/*******************************************************************************
*
*/
bool TimerTxSendHeartBeatPer50ms(void)
{
	if(true == IsVariableTimerDecreaseToZero(&timer_tx_heartbeat_per50ms))
	{
		timer_tx_heartbeat_per50ms = Timer_TX_HeartBeat_Per50ms ;
		uartmessage_fromTxHeartBeat.event =	UART_HEARTBEAT_EVENT ;
					
		// Wake up the application.
		Queue_enqueue(uart_queueHandle, &(uartmessage_fromTxHeartBeat.elem));
		Semaphore_post(uart_semHandle);
		return true ;
	}
	return false ;
}

/*******************************************************************************
*
*/
bool TimerRxHeartBeatDecreasePer50ms(void)
{		
	if(true == IsVariableTimerDecreaseToZero(&timer_rx_heartbeat_per50ms))		
	{
		uartmessage_fromRxHeartBeat.event =	UART_DISCONNECT_EVENT ;
				
		// Wake up the application.
		Queue_enqueue(uart_queueHandle, &(uartmessage_fromRxHeartBeat.elem));
		Semaphore_post(uart_semHandle);
					
		return true ;
	}
	return false ;
}

/*******************************************************************************
*
*/
bool TimerTxResendPer50ms(void)
{
	for(uint8_t i = 0 ;i < sizeof(UartBuff_table)/sizeof(UartBuff_TypeDef);i++)
	{
		if(UartBuff_table[i].counter_retry)
		{
			if(true == IsVariableTimerDecreaseToZero(&(UartBuff_table[i].timer_resend_temp_per50ms)))
			{
				uartmessage_fromresend.event =	UART_RESEND_EVENT ;
				uartmessage_fromresend.buffid =  i ;
					
				// Wake up the application.
				Queue_enqueue(uart_queueHandle, &(uartmessage_fromresend.elem));
				Semaphore_post(uart_semHandle);
				return true ;
			}
		}
	}
	return false ;		
}


/*******************************************************************************
*
*/
static bool ClearSpecifiedUartBuffTxFlagAndRetryCounter(uint8_t buffid)
{
	UartBuff_table[buffid].buffstatus.flag_filled = 0 ;
	UartBuff_table[buffid].buffstatus.flag_ready = 0 ;
	UartBuff_table[buffid].buffstatus.flag_sending = 0 ;
	UartBuff_table[buffid].counter_retry = 0 ;
	return true ;
}

/*******************************************************************************
*
*/
static bool ProcessTxSendHeartBeatEvt(void)
{
	//timer_tx_heartbeat_per50ms = Timer_TX_HeartBeat_Per50ms ;
	FillAndSendMsg(0, 1,0XFF);
	return false ;
}

/*******************************************************************************
*
*/
static bool ProcessTxResendEvt(uint8_t buffid)
{
	if(1 == UartBuff_table[buffid].counter_retry)
	{
		//stop resend when counter_try turn to zero
		ClearSpecifiedUartBuffTxFlagAndRetryCounter(buffid) ;
		return true ;
	}
	if(RESEND_FOREVER  == UartBuff_table[buffid].counter_retry)
	{		
		UartBuff_table[buffid].buffstatus.flag_filled = 1 ;
		UartBuff_table[buffid].buffstatus.flag_ready = 1 ;
		SendUartBuffToTx(buffid) ;
		return true ;
	}
	if(RESEND_NONE == UartBuff_table[buffid].counter_retry)
		return false ;
	else		//normal resend
	{
		UartBuff_table[buffid].counter_retry -- ;
		UartBuff_table[buffid].buffstatus.flag_filled = 1 ;
		UartBuff_table[buffid].buffstatus.flag_ready = 1 ;
		SendUartBuffToTx(buffid) ;
		return true ;
	}
}

/*******************************************************************************
*
*/
void BLEStatusChangeMsg_EnQueue(uint8_t status)
{
	if(IsInPassThroughStatus())
	{
		uartmessage_fromBLEstatuschanged.event = UART_BLESTATUSCHANGED_EVENT ;
		uartmessage_fromBLEstatuschanged.data = status ;

		// Wake up the application.
		Queue_enqueue(uart_queueHandle, &(uartmessage_fromBLEstatuschanged.elem));
		Semaphore_post(uart_semHandle);
	}
}

/*******************************************************************************
* input:data: guardstatus ,when nospecified ,is 0XFF.
*/
static bool ProcessBLEStatusChangedEvt(uint8_t data) 
{	
	FillAndSendMsg(ProcessID_MM2PT , 5,data);	//ptid = queryblestatus 
	return true ;
}


/*******************************************************************************
*
*/
static bool ProcessDisConnectEvt(void)
{
	if(process_id >= 3)
		process_id = 3;	//qurey gps status
	else
		process_id = 1 ;//set hosttype
	for(uint8_t i = 0 ;i < sizeof(UartBuff_table)/sizeof(UartBuff_TypeDef);i++)
	{
		ClearSpecifiedUartBuffTxFlagAndRetryCounter(i) ;
	}
	return true ;
}

/*******************************************************************************
*
*/
static uint8_t GetCheckCode(uint8_t * Buff ,uint8_t length) 
{
	//check code
	uint8_t checkcode = *(Buff + GPS_ValidDataLen_ArrNum) ;
	for(uint8_t i = GPS_Passway_ArrNum ;i < (length - 4 ) ; i ++ )
		checkcode ^= *(Buff +  i) ;
	return checkcode ;
}

/*******************************************************************************
*
*/
static bool IsRxMsgFormatCorrect(void)
{
	//length check
	if(length_rxmsg < GPS_RX_LEN_MIN)
		return false ;
	if(length_rxmsg > MAX_NUM_RX_BYTES)
		return false ;

	//byte content check 
	if(GPS_StartByte_1 != *(rxBuf + GPS_Start1_ArrNum))
		return false ;
	if(GPS_StartByte_2 != *(rxBuf + GPS_Start2_ArrNum))
		return false ;
	if(GPS_EndByte_2 != *(rxBuf + length_rxmsg - 1 ))
		return false ;
	if(GPS_EndByte_1 != *(rxBuf + length_rxmsg - 2 ))
		return false ;
	if(GPS_CheckByte_2 != *(rxBuf + length_rxmsg - 3 ))
		return false ;

	//check length byte
	if((length_rxmsg - 5 ) != *(rxBuf + GPS_ValidDataLen_ArrNum))
		return false ;

	//check source byte
	if((CPT2M != *(rxBuf + GPS_Passway_ArrNum)) && (MPT2M != *(rxBuf + GPS_Passway_ArrNum)) )
		return false ;

	if(MCmd_PassthroughSend == *(rxBuf + GPS_Cmd_ArrNum))
	{
		if((length_rxmsg - GPS_TX_LEN_MIN ) != *(rxBuf + GPS_MinLen_Data_ArrNum))
			return false ;
	}
	
	//check code
	if(GetCheckCode(rxBuf, length_rxmsg) !=  *(rxBuf +  length_rxmsg - 4))
		return false ;
	
	return true ;
}

/*******************************************************************************
*
*/
/*static uint8_t GetRecMatchProcessID(uint8_t startbyte)
{
	for(uint8_t i = 0 ;i < sizeof(RXTXFunc)/sizeof(GPSRXTXFunc_TypeDef) ; i ++)
	{
		if(RXTXFunc[i].cmd == *(rxBuf + startbyte))
			return i ;
	}
	return 0XFF ;
}*/

/*******************************************************************************
*
*/
static bool IsHeartBeatRsp(void)
{
	if(ReCmd_ConnectPackage  != *(rxBuf + GPS_Cmd_ArrNum) )
		return false ;
	return true ;
}

/*******************************************************************************
*
*/
static bool IsPTSendInfor(void)
{
	if(MCmd_PassthroughSend  != *(rxBuf + GPS_Cmd_ArrNum) )
		return false ;
	return true ;
}

/*******************************************************************************
*
*/
/*static bool IsPTRsp(void)
{
	if(MCmd_PassthroughResponse  == *(rxBuf + GPS_Cmd_ArrNum) )
		return true ;
	return false ;
}*/



/*******************************************************************************
*
*/
static bool IsGPRSControlLockCmd(void)
{
	if(Cmd_GPRSControlLock  != *(rxBuf + GPS_Cmd_ArrNum) )
		return false ;
	return true ;
}


/*******************************************************************************
*
*/
static bool GetTxBuffIDMatchWithRx(RecResult_TypeDef * result) 
{ 	
	//get match buffid
	if(ProcessID_MM2PT == result->processid_match)
	{
		for(uint8_t i = 0 ; i < sizeof(UartBuff_table)/sizeof(UartBuff_TypeDef) ; i ++)
		{		
			if(false == UartBuff_table[i].buffstatus.flag_filled)
				continue ;
			if(true == IsSerialNumSameWithRx(i))	
			{
				//check data 01 or 00
				if(RXTXFunc[result->processid_match].rxFunc(GPS_MinLen_Data_ArrNum))
					result->status = RecPT_Success ;
				else
					result->status= RecPT_Fail ;
				result->buffid_match = i ;
			}
			else
				result->status = Rec_No_Match  ;
		}
		return false ;		//this is a response message ,it doesn't need to send back any message
	}
	
	//if(ProcessID_MM2PT != result->processid_match)
	for(uint8_t i = 0 ; i < sizeof(UartBuff_table)/sizeof(UartBuff_TypeDef) ; i ++)
	{
		if(false == UartBuff_table[i].buffstatus.flag_filled)
			continue ;
			
		if(UartBuff_table[i].data[GPS_Cmd_ArrNum] == RXTXFunc[result->processid_match].cmd)
		{
			if(RXTXFunc[result->processid_match].rxFunc(GPS_MinLen_Data_ArrNum))
				result->status = RecNotPT_Success ;
			else
				result->status= RecNotPT_Fail ;
			result->buffid_match = i ;				
		}
		else
			result->status = Rec_No_Match;
	}
	return false ;
}

/*******************************************************************************
*
*/
static bool IsSerialNumSameWithRx(uint8_t tx_buffid)
{
	if( (*(rxBuf + GPS_MinLen_SrlNum2_ArrNum)) != UartBuff_table[tx_buffid].data[UartBuff_table[tx_buffid].length_valid - GPS_FromLast_SrlNum2_ArrNum]) 
		return false ;
	if((*(rxBuf + GPS_MinLen_SrlNum1_ArrNum)) != UartBuff_table[tx_buffid].data[UartBuff_table[tx_buffid].length_valid - GPS_FromLast_SrlNum1_ArrNum])
		return false ;
	return true ;
}
/*******************************************************************************
*
*/
static uint8_t GetRxMatchProcessID(void) 
{
	//get match process id 
	for(uint8_t i = 0 ; i < sizeof(RXTXFunc)/sizeof(GPSRXTXFunc_TypeDef) ; i ++)
	{
		if(RXTXFunc[i].rsp_cmd ==*(rxBuf + GPS_Cmd_ArrNum))
			return i ;
	}
	return 0XFF ;
}

/*******************************************************************************
*
*/
static void SetRxHeartBeatTimer(void) 
{
	timer_rx_heartbeat_per50ms = Timer_RX_HeartBeat_Per50ms ;
}

/*******************************************************************************
*
*/
static bool IsRxHeartBeatDisconnectBefore(void) 
{
	if(timer_rx_heartbeat_per50ms)
		return false ;
	return true ;
}

/*******************************************************************************
*
*/
static bool IsInPassThroughStatus(void) 
{
	if(IsRxHeartBeatDisconnectBefore())
		return false ;
	if(process_id != ProcessID_MM2PT)
		return false ;
	return true ;
}



/*******************************************************************************
*
*/
static void FillConnectPackageToUartBuff(uint8_t buffid ,uint8_t startbyte,uint8_t data) 
{
	UartBuff_table[buffid].data[startbyte] = 0 ;//SET_SUCCESS_U8 ;
}

/*******************************************************************************
*
*/
static void FillSetHostTypeToUartBuff(uint8_t buffid,uint8_t startbyte,uint8_t data) 
{
	UartBuff_table[buffid].data[startbyte] = SET_SUCCESS_U8 ;
}

/*******************************************************************************
*
*/
static void FillSetDeviceNumToUartBuff(uint8_t buffid,uint8_t startbyte,uint8_t data) 
{
	for(uint8_t i = 0 ; i < RXTXFunc[2].tx_data_len ; i ++)
		UartBuff_table[buffid].data[startbyte + i] = i + 1 ;
}

/*******************************************************************************
*
*/
static void FillQueryGPSStatusToUartBuff(uint8_t buffid,uint8_t startbyte,uint8_t data) 
{
	UartBuff_table[buffid].data[startbyte] = Query_U8 ;
}

/*******************************************************************************
*
*/
/*static bool CompareRecWithConnectPackageData(uint8_t startbyte) 
{
	if(rxBuf[startbyte] == SET_SUCCESS_U8)
		return true ;
	return false ;
}*/

/*******************************************************************************
*
*/
static bool CompareRecWithHostTypeData(uint8_t startbyte) 
{
	if(rxBuf[startbyte] == SET_SUCCESS_U8)
		return true ;
	return false ;
}

/*******************************************************************************
*
*/
static bool CompareRecWithDeviceNumData(uint8_t startbyte) 
{
	if(rxBuf[startbyte] == SET_SUCCESS_U8)
		return true ;
	return false ;
}

/*******************************************************************************
*
*/
static bool CompareRecWithGPSStatusData(uint8_t startbyte) 
{
	if(rxBuf[startbyte] & U8_BIT1)	//if TCP/IP status is ok?
		return true ;
	return false ;
}

/*******************************************************************************
*
*/
static bool CompareRecWithPassthrough(uint8_t startbyte) 
{
	if(rxBuf[startbyte] == SET_SUCCESS_U8)	
		return true ;
	return false ;
}

/*******************************************************************************
*
*/
static UnionSumWithH8L8_TypeDef GetRecPTSerialNum(void)
{
	UnionSumWithH8L8_TypeDef serialnum_pt ;
	serialnum_pt.h8 = *(rxBuf + length_rxmsg - GPS_FromLast_SrlNum1_ArrNum) ;
	serialnum_pt.l8 = *(rxBuf + length_rxmsg - GPS_FromLast_SrlNum2_ArrNum) ;
	return serialnum_pt ;
}

/*******************************************************************************
*
*/
static uint16_t GetRecPTCmd(void)
{
	UnionSumWithH8L8_TypeDef cmd_pt ;
	// GPS_MinLen_Data_ArrNum : data_len		+1 :cmd.h8		+2: cmd.l8
	cmd_pt.h8 = *(rxBuf + GPS_MinLen_Data_ArrNum + 1) ;
	cmd_pt.l8 = *(rxBuf + GPS_MinLen_Data_ArrNum + 2) ;
	return cmd_pt.sum ;
}


/*******************************************************************************
*
*/
static bool FillPTRspToUartBuff(uint8_t target_buffid,UnionSumWithH8L8_TypeDef serialnum_pt,bool status)
{
	//PT Data len is custpmer
	UartBuff_table[target_buffid].length_valid = GPS_TX_LEN_MIN  ;
		
	UartBuff_table[target_buffid].data[GPS_Start1_ArrNum] = GPS_StartByte_1 ;
	UartBuff_table[target_buffid].data[GPS_Start2_ArrNum] = GPS_StartByte_2 ;
	UartBuff_table[target_buffid].data[GPS_ValidDataLen_ArrNum] = GPS_PT_RSP_DATA_LEN  ;
	UartBuff_table[target_buffid].data[GPS_Passway_ArrNum] = MM2PT ;
	UartBuff_table[target_buffid].data[GPS_Cmd_ArrNum] = MCmd_PassthroughResponse  ;
	if(status)
		UartBuff_table[target_buffid].data[GPS_MinLen_Data_ArrNum] = SET_SUCCESS_U8  ;
	else
		UartBuff_table[target_buffid].data[GPS_MinLen_Data_ArrNum] = SET_FAIL_U8  ;
	UartBuff_table[target_buffid].data[GPS_MinLen_SrlNum1_ArrNum] = serialnum_pt.h8	;
	UartBuff_table[target_buffid].data[GPS_MinLen_SrlNum2_ArrNum] = serialnum_pt.l8	;
	UartBuff_table[target_buffid].data[GPS_MinLen_ChkCode2_ArrNum] = GPS_CheckByte_2	;
	UartBuff_table[target_buffid].data[GPS_MinLen_EndCode1_ArrNum] = GPS_EndByte_1  ;
	UartBuff_table[target_buffid].data[GPS_MinLen_EndCode2_ArrNum] = GPS_EndByte_2  ;
	return true ;
}


/*******************************************************************************
*
*/
static bool SendPTRsp(bool status) 
{
	uint8_t blank_buff = 0xFF ;
	blank_buff = GetBlankUartBuffID() ;
	if(0XFF == blank_buff)
		return false ;
	
	FillPTRspToUartBuff(blank_buff,GetRecPTSerialNum(),status);
	WriteCheckCode(blank_buff) ;
	
	UartBuff_table[blank_buff].timer_resend_per50ms = 0 ;
	UartBuff_table[blank_buff].counter_retry = 0 ;

	UartBuff_table[blank_buff].buffstatus.flag_filled = 1 ;
	UartBuff_table[blank_buff].buffstatus.flag_ready = 1 ;
		
	//if	other buff sending ,will send next time after 30ms interval 	else	 send buff
	SendUartBuffToTx(blank_buff) ;
	return true ;
}

/*******************************************************************************
*
*/
static uint8_t MatchPTIDWithPTCmd(uint16_t ptcmd)
{
	for(uint8_t i = 0 ; i < sizeof(PT)/sizeof(PTMatch_TypeDef) ; i++)
	{
		if(PT[i].rx_ptcmd == ptcmd)
			return i ;
	}
	return 0XFF ;		//no match
}





/*******************************************************************************
*
*/
static bool PTFuncControl(void) 
{
	if(false == IsInPassThroughStatus())
		return false ;
	if(false == SendPTRsp(true))
		return false ;
	uint8_t ptid = 0XFF;
	ptid = MatchPTIDWithPTCmd(GetRecPTCmd()) ;
	if(0XFF == ptid)
		return false ;
	(PT[ptid].ptfunc)();

	//send pttxfunc
	if(PT[ptid].tx_pt_data_len)
		FillAndSendMsg(ProcessID_MM2PT, ptid,0XFF);	//processid == pt 
	return true ;
}

/*******************************************************************************
*
*/
 bool PTFuncUnlock(void) 
{
	//if(BLE_Unconnect == bleconnectstatus)
	//{
	
	//	Guard.cmd_lock_From_reservedGPRS = Unlock_ByReservedGPRS ;
		Guard.cmd_lock_From_BLE = Unlock_Ebike ; 
		Guard.status_autoguard = Status_UnAuto ;
#if(KeyLessStart == Anti_theftDevice_Function)	
		if(Guard.status != Status_GuardProcessActing)
			KeyLessPowerOn();
#endif
	//}
	return true ;
}

/*******************************************************************************
*
*/
bool PTFuncLock(void) 
{
	//if((BLE_Unconnect == bleconnectstatus)&&(Status_Motor_Run != Motor.status_motor))
	if(Status_Motor_Run != Motor.status_motor)
	{
			//Guard.cmd_lock_From_reservedGPRS = Lock_ByReservedGPRS ;
		Guard.cmd_lock_From_BLE = Lock_Ebike ; 
		Guard.status_autoguard = Status_Auto ;
	}
	return true ;
}

/*******************************************************************************
*
*/
static bool PTFuncUpdateRentDeadline(void) 
{		
	if(true == IsDeadLineFormatCorrect((rxBuf + GPS_MinLen_Data_ArrNum + 5),2))	
	{
		Guard.deadlinedata.timerleft_day.h8 = *(rxBuf + GPS_MinLen_Data_ArrNum + 3) ;
		Guard.deadlinedata.timerleft_day.l8 = *(rxBuf + GPS_MinLen_Data_ArrNum + 4) ;
		Guard.deadlinedata.timerleft_hour   = *(rxBuf + GPS_MinLen_Data_ArrNum + 5);
		Guard.deadlinedata.timerleft_minute = *(rxBuf + GPS_MinLen_Data_ArrNum + 6);
		//for(uint8_t i = GPS_MinLen_Data_ArrNum + 5 ; i < GPS_MinLen_Data_ArrNum + 5+2 ; i ++)
			//Guard.deadlinedata.data[i] = *(rxBuf + i) ;
				
		if(true == IsAllBytesZero( ( rxBuf + GPS_MinLen_Data_ArrNum + 3 ) ,4))
		{
			Guard.deadlinedata.timerleft_second = 0 ;
			Guard.status_deadline = Status_ReachedDeadline_NoExecute ;
		}
	}
	return true ;
}
		
/*******************************************************************************
*
*/
static bool PTFuncResetBLE(void) 
{
	SetFlagResetMCUWithCheckSpeedAndSaveFlash();
	return true ;
}

/*******************************************************************************
*
*/
static bool PTFuncResetMCU(void) 
{
	SetFlagResetMCUWithCheckSpeedAndSaveFlash();
	return true ;
}


/*******************************************************************************
*
*/
static bool PTFuncQueryBLEStatus(void) 
{
	return true ;
}

/*******************************************************************************
*
*/
static bool PTFuncACCPowerOn(void) 
{
#if(KeyLessStart == Anti_theftDevice_Function)	
	KeyLessPowerOn();
#endif
	return true ;
}

/*******************************************************************************
*
*/
static bool PTFuncACCPowerOff(void) 
{
#if(KeyLessStart == Anti_theftDevice_Function)	
	KeyLessPowerOff();
#endif
	return true ;
}

/*******************************************************************************
*
*/
static bool PTFuncQueryVersion(void) 
{
	return true ;
}

/*******************************************************************************
*
*/
static bool PTFuncQueryRentDeadLine(void) 
{
	return true ;
}

/*******************************************************************************
*
*/
static void PTTxFuncSendBLEStatus(uint8_t buffid ,uint8_t startbyte,uint8_t data) 
{
	UnionSumWithH8L8_TypeDef cmd ;
	cmd.sum = PTCmd_Status ;
	UartBuff_table[buffid].data[startbyte + 1] = cmd.h8;
	UartBuff_table[buffid].data[startbyte + 2] = cmd.l8;
	if(0XFF == data)
		UartBuff_table[buffid].data[startbyte + 3] = Guard.status;	
	else
		UartBuff_table[buffid].data[startbyte + 3] = data ;
	UartBuff_table[buffid].data[startbyte + 4] = Q.soc_percent;
	UartBuff_table[buffid].data[startbyte + 5]=  GetControllerErrorStatus() ;
	UartBuff_table[buffid].data[startbyte + 6] = 0 ;
}
	

/*******************************************************************************
*
*/
static void PTTxFuncSendVersion(uint8_t buffid ,uint8_t startbyte,uint8_t data) 
{
	UnionSumWithH8L8_TypeDef cmd ;
	cmd.sum = PTCmd_Version ;
	UartBuff_table[buffid].data[startbyte + 1] = cmd.h8;
	UartBuff_table[buffid].data[startbyte + 2] = cmd.l8;
	UartBuff_table[buffid].data[startbyte + 3] = (uint8_t)(PCB_VERSION >> 8);		
	UartBuff_table[buffid].data[startbyte + 4] = (uint8_t)(PCB_VERSION);
	UartBuff_table[buffid].data[startbyte + 5]=  (uint8_t)(FirmwareVersion >> 16);
	UartBuff_table[buffid].data[startbyte + 6] = (uint8_t)(FirmwareVersion >> 8);
	UartBuff_table[buffid].data[startbyte + 7] = (uint8_t)(FirmwareVersion);	
}

/*******************************************************************************
*
*/
static void PTTxFuncSendRentDeadLine(uint8_t buffid ,uint8_t startbyte,uint8_t data) 
{
	UnionSumWithH8L8_TypeDef cmd ;
	cmd.sum = PTCmd_RentDeadLine ;
	UartBuff_table[buffid].data[startbyte + 1] = cmd.h8;
	UartBuff_table[buffid].data[startbyte + 2] = cmd.l8;
	UartBuff_table[buffid].data[startbyte + 3] = Guard.deadlinedata.timerleft_day.h8;		
	UartBuff_table[buffid].data[startbyte + 4] = Guard.deadlinedata.timerleft_day.l8;
	for(uint8_t i = 2 ; i < 5 ; i ++)
		UartBuff_table[buffid].data[startbyte + i + 3] = Guard.deadlinedata.data[i] ;
			
}





