/*******************************************************************************
  Filename:       uart_task.c
  Revised:       
  Revision:      

  Description:   

*******************************************************************************/

/*********************************************************************
 * INCLUDES
 */



#include "Board.h"

#include "util.h"
#include "defines.h"
#include "systemcontrol.h"
#include "PeriodTask.h"
#include "InitialConfig.h"
#include "simple_peripheral.h"
#include "ATControl.h"
#include "BatteryPowerCal.h"

#include "Guard.h"
#include "gpscontrol.h"
#include "OLC.h"
#include "T2C.h"
#include "MileageCal.h"
#include "KeyLess.h"
#include "MechanicalLock.h"
#include "Dousupport.h"
#include "SeatLock.h"
#include "FlashSNV.h"
#if VOICEPACKET_COMPILE
#include "VoicePacket.h"
#endif
#include "debug.h"
	 

#include "icall_api.h"
/*********************************************************************
 * CONSTANTS
 */
// Task configuration
#define 	PERIOD_TASK_PRIORITY      	2//1

#ifndef 	PERIOD_TASK_STACK_SIZE
#define 	PERIOD_TASK_STACK_SIZE     	644
#endif

//#define UART_READ_EVT        		0x0001

#define		Timer_2ms_Per100us			20	//20*100us = 2ms
#define		Timer_50ms_Per100us			500
#define		Timer_500ms_Per2ms			250
#define		Timer_1s_Per100us			10000
#define		Timer_30s_Per1s				30
#define		Timer_10min_Per30s			20	




/*********************************************************************
 * LOCAL VARIABLES
 */
static Semaphore_Struct clock100us_semStruct;
static Semaphore_Handle clock100us_semHandle;

static Clock_Struct 	clockStruct;
 Clock_Handle 	clockHandle;
//Clock_FuncPtr EventPer100us ;
extern ButtonPressDetect_TypeDef GSButton;
// Task configuration
Task_Struct periodTask;
Char 		periodTaskStack[PERIOD_TASK_STACK_SIZE];

static uint8_t  timer_2ms_per100us = 0 ;
static uint16_t timer_50ms_per100us = 0 ;
static uint8_t 	timer_500ms_per2ms = 0 ;
static uint16_t timer_1s_per100us = 0 ;
static uint8_t 	timer_30s_per1s = 0 ;
static uint8_t 	timer_10min_per30s = 0 ;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
//static void readCallback(UART_Handle handle, void *rxBuf, size_t size);
static void PeriodTask_createClock(void) ;
static void Period_taskFxn(UArg a0, UArg a1);
static void PeriodTask_init(void);
static void EventPer100us(UArg a0) ;
static void EventPer2ms(void) ;
static void EventPer50ms(void) ;
static void EventPer500ms(void) ;
static void EventPer1s(void) ;
static void EventPer30s(void) ;
static void EventPer10Minute(void) ;

/*********************************************************************
 * @brief   
 *
 * @param   None.
 *
 * @return  None.
 */
void PeriodTask_createClock(void)
{
	Clock_Params clockParams;
    /* Construct a clock object to be use */
    Clock_Params_init(&clockParams);
	clockParams.period = 10 ;//100000;		//1s   *10us
	clockParams.startFlag = true;
	clockParams.arg = (UArg)0x5555;
    Clock_construct(&clockStruct, EventPer100us ,10 ,&clockParams);

    /* Obtain instance handle */
    clockHandle = Clock_handle(&clockStruct);
}

/*********************************************************************
 * @brief   
 *
 * @param   None.
 *
 * @return  None.
 */
void PeriodTask_createSemaphore(void)
{
	Semaphore_Params period_semParams;
      /* Construct a Semaphore object to be use as a resource lock, inital count 1 */
    Semaphore_Params_init(&period_semParams);
    Semaphore_construct(&clock100us_semStruct, 0 ,&period_semParams);

    /* Obtain instance handle */
    clock100us_semHandle = Semaphore_handle(&clock100us_semStruct);
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
void PeriodTask_createTask(void)
{
	Task_Params taskParams;

	  // Configure task
	  Task_Params_init(&taskParams);
	  taskParams.stack = periodTaskStack;
	  taskParams.stackSize = PERIOD_TASK_STACK_SIZE;
	  taskParams.priority = PERIOD_TASK_PRIORITY;

	  Task_construct(&periodTask, Period_taskFxn, &taskParams, NULL);
}

/*********************************************************************
 * @brief  
 *
 * @param   None.
 *
 * @return  None.
 */
static void PeriodTask_init(void)
{
	PeriodTask_createClock();	
	ADCInitial();
	
	VariableInitial();	
}

/*********************************************************************
 * @brief  
 *
 * @param   a0, a1 - not used.
 *
 * @return  None.
 */
static void Period_taskFxn(UArg a0, UArg a1)
{
  // Initialize application
  	PeriodTask_init();
  // Application main loop
  	for (;;)
	{		
     	Semaphore_pend(clock100us_semHandle, BIOS_WAIT_FOREVER);
#if(Commu_Mode_Common != Commu_Mode)		
		ReadOLCLevel();
    #if(Commu_Mode_TwoWire == Commu_Mode)
		T2CSIFSend_Service();
	#endif
#endif		
				
#if MECHANICALLOCK_INTERNAL_COMPILE					
	#if(LeCi_MechanicalLock_Manufacturer_ID == MechanicalLock_Manufacturer_ID)
		SIFSend_Service(&MechalMotorLockSIF) ;
	#endif
#endif
		
#if DOUSUPPORT_INTERNAL_COMPILE
		SIFSend_Service(&DSTSIF) ;
		#if(DongdaLanniao_DouSupport_ManufactID == DouSupport_Manufacturer_ID)
		ReadDSRLevel();
		#endif			
#endif		

#if VOICEPACKET_COMPILE
	SIFSend_Service(&VoicePacketSIF) ;
#endif
		
		EventPer2ms() ;
		EventPer50ms() ;
		EventPer500ms() ;
		EventPer1s() ;
		EventPer30s() ;
		EventPer10Minute() ;	
		
  }
}

/*********************************************************************
* @brief  
 *
 * @param   a0, a1 - not used.
 *
 * @return  None.
 */
void EventPer100us(UArg a0)
{	
	timer_2ms_per100us ++ ;
	timer_50ms_per100us ++ ;
	timer_1s_per100us ++ ;
	
 	Semaphore_post(clock100us_semHandle);

}

/*********************************************************************
* @brief  
 *
 * @param   
 *
 * @return  None.
 */
void EventPer2ms(void)
{
	if(timer_2ms_per100us >= Timer_2ms_Per100us)
	{
#if BUZZER_COMPILE
		if(Buzzer.timer_addcounter_per2ms < UINT8_MAX)
			Buzzer.timer_addcounter_per2ms ++ ;
#endif		

		LVADSample();
#if(KeyLessStart == Anti_theftDevice_Function)	
		LV48VSWADSample();
#endif
		
		
#if GPS_COMPILE
	TimerTxIntervalPer2msDecrease();
#endif		

	ResetWithCheckSpeedAndSaveFlashMechanism();

		timer_500ms_per2ms ++ ;
		timer_2ms_per100us = 0 ;
		
	}	
}

/*********************************************************************
* @brief  
 *
 * @param   
 *
 * @return  None.
 */
void EventPer50ms(void)
{
	if(timer_50ms_per100us >= Timer_50ms_Per100us)
	{		
#if(Commu_Mode_Common == Commu_Mode)
		if(Port_YXT->IDR & Pin_YXT )
		{
			BLEMdlCnt.timer_noconnect2controller5V_50ms = 0 ;
			if(BLEMdlCnt.timer_connect2controller5V_50ms  < UINT8_MAX)
				BLEMdlCnt.timer_connect2controller5V_50ms  ++ ;
		}
		else
		{
			BLEMdlCnt.timer_connect2controller5V_50ms  = 0 ;
			if(BLEMdlCnt.timer_noconnect2controller5V_50ms < UINT8_MAX)
				BLEMdlCnt.timer_noconnect2controller5V_50ms ++ ;
		}
#endif	

		FlashProtectWhenLVLow();
		UpdateStatusConnectToController() ;
#if LOCKBUTTON_COMPILE		
		CheckLockButtonEvent();
#endif
#if LONGRENT_COMPILE
        CheckGSButtonEvent();
        
#endif
		GuardControl();
		
#if BUZZER_COMPILE		
		BuzzerRingWhenEnable();	
#endif
		
#if(KeyLessStart == Anti_theftDevice_Function)
		CheckKeyLessStartActionResult();
		DetectLD();
	#if MECHANICALLOCK_INTERNAL_COMPILE			
		CheckMechalMotorLockActionResult();
	#endif			
#endif

			
#if SEATLOCK_COMPILE
		CheckSeatLockActionResult();
#endif	
		
#if(MECHANICALLOCK_INTERNAL_COMPILE||L9110DRIVE_COMPILE)			
		SW9VDelayAfterPowerOnService();
#endif

#if DOUSUPPORT_INTERNAL_COMPILE		
		CheckDouSupportActionResult();
#endif		

#if GPS_COMPILE
		TimerTxResendPer50ms() ;
		TimerTxSendHeartBeatPer50ms() ;	
		TimerRxHeartBeatDecreasePer50ms();
#endif
		
#if TEST_EXTFLASH_COMPILE		
		CheckExtFlashIfFlagSet();
#endif

#if WATCHDOGTIMER_COMPILE
		ClearWatchDogTimer();
#endif

#if HistoryMileage_COMPILE 	
	if(Mileage.Meter >= 1)
	   {
		  Mileage.Meter-=1;
		  Mileage.counter_hallchange_accumu_under1KM.sum32 += 1 ;
		  Mileage.odometer_perKM.sum32 += 1 ;
	   }
#endif
		timer_50ms_per100us = 0 ;
	}	
}

/*********************************************************************
* @brief  
 *
 * @param   
 *
 * @return  None.
 */
void EventPer500ms(void)
{
	if(timer_500ms_per2ms >= Timer_500ms_Per2ms)
	{
		if(Motor.timer_stall_500ms < UINT16_MAX)		//stall 计时
			Motor.timer_stall_500ms ++ ;
		
#if(Commu_Mode_Common == Commu_Mode)		
		UpdateHallSpeedPer500ms();
#elif(Commu_Mode_TwoWire == Commu_Mode)
		T2C.timer_cmdwait_per500ms ++ ;
#else
#endif
		StatusMotor_TypeDef status_motor_temp ;
		status_motor_temp = GetMotorStatus(Motor.status_motor);		//run or stall or park	
		if(status_motor_temp != Motor.status_motor)
		{
			//first to park status
			if(Status_Motor_Park == status_motor_temp)
#if(KeyLessStart == Anti_theftDevice_Function)			
				//if(Status_UnGuarded == Guard.status)
				if(IsACCPoweredOn())
					KeyLessPowerOff();
#endif					

		}
		else
		{
			if(Status_Motor_Park == status_motor_temp)
			{
				if(true == IsChargeStatus())
					LV.flag_batterycharged = true ;	// 当充电停止时，LV.flag_batterycharged保存了曾经充电的记录
					
				if(true == LV.flag_batterycharged)
					//重新计算Qremainder和Mileage
					CalQInitialAndMileageRemainder();		
				else
				{
					//if(Motor.timer_stall_500ms != (TimePeriod_45min_Per1s * 2))
					if(Motor.timer_stall_500ms >= ((TimePeriod_45min_Per1s + TimePeriod_15min_Per1s)  * 2))
					{
						Q.soc_release = Q.soc_initial - GetBatterySoc() ;
						#if(Commu_Mode_Common != Commu_Mode)
							//Q.data_max.bit32 = CalQmax();
							UpdataQInitalAndRemainder();
							Mileage.ratio_releasedivmileage_mul256 = GetRatioReleaseDivMileageMul256();
						#endif
	#if 0	
						Mileage.counter_hallchange_max.sum32 = CalMileageMax() ;	
						Mileage.caledvalue_hallchange_1KM = CalCounterHallchange1KM();
	#endif
						UpdateMileageInitialAndRemainder();	
	
						//in order to recal Q soc after 1h+50min
						Motor.timer_stall_500ms = Time_Stall_Max ;
					}
				}
			}
		   /* else if(Status_Motor_Run == Motor.status_motor)
			{
				LV.flag_batterycharged = false ;	//电机运行后，清除LV.flag_batterycharged
			}*/
			else	//Status_Motor_Stall
				 LV.flag_batterycharged = false ;	//电机运行后，清除LV.flag_batterycharged
		}
		
		Motor.status_motor = status_motor_temp;

#if DOUSUPPORT_INTERNAL_COMPILE
	#if(DongdaLanniao_DouSupport_ManufactID == DouSupport_Manufacturer_ID)
		CheckIsDSTWireBreakSetTruePer500ms();
		CheckIsDSRWireBreakSetTruePer500ms();
	#endif
#endif	


#if HEARTBEAT_COMPILE	
		if(IsBLEConnectStatusInteractive() &&(false == flag_oading))		
		{
			HeartBeatSendEveryFixedInterval();
			CheckBLEHeartBeat() ;
		}
#endif
		
		BlueLEDControl();
	
#if HistoryMileage_COMPILE  
	   Mileage.Meter += OLCM.counter_hall_speed.sum*(float)(SpeedM_PerSecond) ;	   
#endif			
		timer_500ms_per2ms = 0 ;
	}	
}

/*********************************************************************
* @brief  
 *
 * @param   
 *
 * @return  None.
 */
void EventPer1s(void)
{
	if(timer_1s_per100us >= Timer_1s_Per100us)
	{		
#if DeadlineCounter_COMPILE
		GuardDeadLineApproachPer1s();	
#endif
//		CalQCurrentRemainder();

		CheckUnInteractiveTimeout();

		SimpleBLECentral_startDiscHandler();
		

#if SEATLOCK_COMPILE
		UpdateSeatLockUnlockedActionFrequent();
#endif		
		
#if MECHANICALLOCK_INTERNAL_COMPILE
		UpdateTimerWaitForMotorStop();	
#endif
				
		timer_30s_per1s ++ ;
		timer_1s_per100us = 0 ;
		

		
	}	
}

/*********************************************************************
* @brief  
 *
 * @param   
 *
 * @return  None.
 */
void EventPer30s(void)
{
	if(timer_30s_per1s >= Timer_30s_Per1s)
	{
#if(CyclicSendMessagePer30s == CyclicSendPeriod)
		if(True == AT.mode_cyclicsend)
			CyclicSendData();
#endif

#if GPS_COMPILE
	UpdateQPercentAndSend();
#endif	
			
		
		timer_10min_per30s ++ ;
		timer_30s_per1s = 0 ;
	}	
}

/*********************************************************************
* @brief  
 *
 * @param   
 *
 * @return  None.
 */
void EventPer10Minute(void)
{
	if(timer_10min_per30s >= Timer_10min_Per30s)
	{
		
#if GPS_COMPILE
		Q.soc_percent = ReturnCurrentQPercent() ; 
		BLEStatusChangeMsg_EnQueue((uint8_t)Guard.status);
#endif			
		timer_10min_per30s = 0 ;
	}	
}




