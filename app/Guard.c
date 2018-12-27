
#include "defines.h"
#include "simple_peripheral.h"

#include "systemcontrol.h"
#include "Guard.h"
#include "ATControl.h"
#include "OLC.h"
#include "T2C.h"
#include "KeyLess.h"
#include "MileageCal.h"

#include "MechanicalLock.h"
#include "DouSupport.h"
#include "gpscontrol.h"
#if VOICEPACKET_COMPILE
#include "VoicePacket.h"
#endif
#if ResetMcuAccWork_COMPILE	
#include "FlashSNV.h"
#endif

/*************************************************/
/*******************Variable**********************/
static bool flag_guard_buffcmd = false;



static ButtonPressDetect_TypeDef LockButton =
{
	Board_LockButton ,
	Button_Released ,
    Button_Released ,
	0,
	4,
	4
} ;

#if LONGRENT_COMPILE
ButtonPressDetect_TypeDef GSButton =
{
	Board_GSButton ,
	Button_Released ,
    Button_Released ,
	0,
	4,
	4
} ;
#endif

Guard_TypeDef	Guard ;

/***********************************************/
static void StageBuffCmd(bool action_status);
static bool IsThereBuffNegativeCmd(bool action_status);
static void ProcessNegativeBuffCmd(bool action_status);
static bool IsGuardActionFinished(void);
static bool IsUnguardActionFinished(void);

static void GuardDeadLineStatusMachine(void);




/*****************function********************/
/*******************************************************************************
* 函数名        : 1:1信号输入低电平有效
* 函数描述    	:
* 输入参数      : None
* 输出结果      : None
* 返回值        : None
*******************************************************************************/
void GuardControl(void)
{	
	GuardDeadLineStatusMachine();
	Guard.status_guardreason = CheckGuardReason();
	
	if(Status_Guarded == Guard.status)
	{

		if(None == Guard.status_guardreason)
			ResetGuardStatus();
		else if(AppLockCmd == Guard.status_guardreason)
		{
			if(true == Guard.status_unguardrecoverable_whenstayAppconnected)
			{
				Guard.status_unguardrecoverable_whenstayAppconnected = false ;
				Guard.cmd_lock_From_BLE = Unlock_Ebike ;
				Guard.status_guardreason = None ;
				ResetGuardStatus();
			}
		}
		else
			;
		
	}
	else if(Status_UnGuarded == Guard.status)
	{

		if(None == Guard.status_guardreason)
			;
		else if(Controller_Disconnect == Guard.status_guardreason)
		{
			Guard.cmd_lock_From_BLE = Lock_Ebike ;		//用来保证每次电门锁断电以后需要用手机解锁
			Guard.status_unguardrecoverable_whenstayAppconnected = true ;	//当App连接没有断时，下次上电门锁可以直接解锁
			SetGuardStatus();
		}
		else
			SetGuardStatus();
	}
	else if(Status_UnguardProcessActiong_BeforPowerOn == Guard.status)
	{
		StageBuffCmd(false) ;
			
		if(Status_PowerOn == KeyLess.status )
		{
#if(ELU_ActionSingle_Mode == ELU_BreakSingle_Mode)	
			Guard.status = Status_UnguardProcessActiong_BeforBreak ;
#elif(ELU_ActionSingle_Mode == ELU_DirectlyUnlocked_Mode)
			Guard.status = Status_UnguardProcessActiong_AfterBreak ;
#else	
#endif			
		}		
	}
#if(ELU_ActionSingle_Mode == ELU_BreakSingle_Mode)		
	else if(Status_UnguardProcessActiong_BeforBreak == Guard.status)
	{
		if(true == MechalMotorLock.flag_breakconfirm)
			Guard.status = Status_UnguardProcessActiong_AfterBreak ;
		else
		{
			if((Guard.status_guardreason)||(true == IsThereBuffNegativeCmd(false)))
				ProcessNegativeBuffCmd(false);
		}
	}
#endif	
	else if(Status_UnguardProcessActiong_AfterBreak == Guard.status)
	{
		StageBuffCmd(false);
		
		if(true == IsUnguardActionFinished())		
		{
			Guard.status = Status_UnGuarded ;
            if(IsBLEConnectStatusInteractive())	
            {
                ResponseDouSupportStatusAndErrorChanged(NULL);		//in order to tell the phone app that the action is finished 
                SimpleBLEPeripheral_enqueueAsySendMsg();
            }
            if(true == IsThereBuffNegativeCmd(false))
                ProcessNegativeBuffCmd(false) ;	      

        }
	}
	else if(Guard.status == Status_GuardProcessActing )
	{
		StageBuffCmd(true) ;
		if(true == IsGuardActionFinished())
		{
			Guard.status = Status_Guarded ;

            if(IsBLEConnectStatusInteractive())	
            {
                ResponseDouSupportStatusAndErrorChanged(NULL);		//in order to tell the phone app that the action is finished 
                SimpleBLEPeripheral_enqueueAsySendMsg();
            }
            if(true == IsThereBuffNegativeCmd(true))
                ProcessNegativeBuffCmd(true) ;	

        }
	}
	else
		;
	ATCmdGuardResponseWhenGuardStatusUnchange();
	
	
}

/*******************************************************************************
* 
*/
void GuardStatusInitial(void)
{

	
	if(Status_Auto == Guard.status_autoguard)
	{
		Guard.cmd_lock_From_BLE = Lock_Ebike ;
#if DOUSUPPORT_INTERNAL_COMPILE			
		DouSupport.status = Status_RaiseInPlace ;
#endif		
		if(NO == IsDeadLineReached())
			Guard.status_deadline = Status_UnReachDeadline ;
		else		
			Guard.status_deadline = Status_ReachedDeadline_Execute ;
		//Motor.status_motor = Status_Motor_Park ;
		Guard.status = Status_Guarded ;
		SetGuard2Controller();	 	//上锁
	}
	else
	{
		Guard.cmd_lock_From_BLE = Unlock_Ebike ;
#if DOUSUPPORT_INTERNAL_COMPILE			
		DouSupport.status = Status_RetractInPlace ;	
#endif		
		if(NO == IsDeadLineReached())
		{
			Guard.status_deadline = Status_UnReachDeadline ;
			Guard.status = Status_UnGuarded ;
			ResetGuard2Controller();		//解锁
			
			#if ResetMcuAccWork_COMPILE	
			     #if(KeyLessStart == Anti_theftDevice_Function)
				   KeyLessPowerOnAction();	
                 #endif	
		   #endif	
		}
		else		
		{
			Guard.status_deadline = Status_ReachedDeadline_Execute ;
			Guard.status = Status_Guarded ;
			SetGuard2Controller();	 	//上锁
		}
	}
	/*Inital GInPin status */ 
	Guard.GInPin.status = High_Level ;
	Guard.status_guardreason = CheckGuardReason();

}


/*********************************************************************
* @brief  
 *
 * @param   a0, a1 - not used.
 *
 * @return  None.
 */
StatusGuardReason_TypeDef	CheckGuardReason(void)
{
    if(Status_ReachedDeadline_Execute == Guard.status_deadline)
	{
		MakeSureStillLockByAppWhenReasonChanged();
		return DeadlineBecomeDue ;
	}
 
	
#if(External_Anti_theft == Anti_theftDevice_Function)
	CheckLevelUpdateAndFilter(&Guard.GInPin ,Port_Guard_Input,Pin_Guard_Input,Timer_GIn_Valid_Per2ms);
	if(Low_Level == Guard.GInPin.status)
		return RemoteLock ;
#endif
	
#if GPS_COMPILE
	if(Lock_ByForceGPRS == Guard.cmd_lock_From_FroceGPRS)
	{
		MakeSureStillLockByAppWhenReasonChanged();
		return ForceGPRSLock ;
	}
	#if GPS_Commu_Binding
	if(false == GPS.Connect.status)
	{
		MakeSureStillLockByAppWhenReasonChanged();
		return GPRSUnconncetLock ;
	}
	#endif
#endif
	
	if(Status_Auto == Guard.status_autoguard)
	{
       #if LONGRENT_COMPILE
		
       #else
  		if(false == BLEMdlCnt.status_connect2controller)
			return Controller_Disconnect ;			        //用来保证每次电门锁断电以后需要用手机解锁
       #endif

		if(Lock_Ebike == Guard.cmd_lock_From_BLE)			
			return AppLockCmd ;
		else
			return None ;
	}
	else
		return None ;
}

/*********************************************************************
* @brief  avoid when guard/unguard action ,receive the negative cmd
 *
 * @param   a0, a1 - not used.
 *
 * @return  None.
 */
static void StageBuffCmd(bool action_status)
{
	if(Guard.status_guardreason)
		flag_guard_buffcmd = true;
	else
		flag_guard_buffcmd = false;	//clear flag
}

/*********************************************************************
* @brief  avoid when guard/unguard action ,receive the negative cmd
 *
 * @param   a0, a1 - not used.
 *
 * @return  None.
 */
static bool IsThereBuffNegativeCmd(bool action_status)
{
	if(action_status == flag_guard_buffcmd)
		return false ;
	return true ;
		
}

/*********************************************************************
* @brief  avoid when guard/unguard action ,receive the negative cmd
 *
 * @param   a0, a1 - not used.
 *
 * @return  None.
 */
static void ProcessNegativeBuffCmd(bool action_status)
{
	if(true == action_status)
		ResetGuardStatus() ;
	else
		SetGuardStatus() ;
}

/*********************************************************************
* @brief  avoid when guard/unguard action ,receive the negative cmd
 *
 * @param   a0, a1 - not used.
 *
 * @return  None.
 */
static bool IsGuardActionFinished(void)
{
#if DOUSUPPORT_INTERNAL_COMPILE	 
	if(DouSupport.status != Status_RaiseInPlace )
		return false ;
	#if(DongdaLanniao_DouSupport_ManufactID == DouSupport_Manufacturer_ID)
	if(DouSupport.error)
		return false ;
	#endif		
#else
	if(Status_MotorLocked != MechalMotorLock.status)
		return false ;
#endif	

	//avoid can't power on when guard_action +unguard(only mechical version)
	if(false == IsACCPoweredOff())
		return false ;
	return true ;
}

/*********************************************************************
* @brief  avoid when guard/unguard action ,receive the negative cmd
 *
 * @param   a0, a1 - not used.
 *
 * @return  None.
 */
static bool IsUnguardActionFinished(void)
{
#if DOUSUPPORT_INTERNAL_COMPILE	
	if(Status_RetractInPlace != DouSupport.status)
		return false ;
	#if(DongdaLanniao_DouSupport_ManufactID == DouSupport_Manufacturer_ID)
	if(DouSupport.error)
		return false ;
	#endif		
#else
	if(Status_MotorUnlocked != MechalMotorLock.status)
		return false ;
#endif	

	if(false == IsACCPoweredOn())
		return false ;
	return true ;
}

/*********************************************************************
* @brief 
 *
 * @param   a0, a1 - not used.
 *
 * @return  None.
 */
void SetGuardStatus(void)
{	
#if MECHANICALLOCK_INTERNAL_COMPILE	
	if(Status_MotorUnlockAction == MechalMotorLock.status)
		MechalMotorLockStopDrive();	

	MechalMotorLock.status = Status_MotorWaitForStop ;		//等待电机停止转动
	SetGuard2Controller();		//Stop motor
	MechalMotorLock.timer_waitformotorstop_per1s = Timer_WaitForMotorStop_Per1s ;
		
	//if(true == AT.status_BLEconnect)	//if cmd from gprs	
		//ResponseMechalMotorLock();
	/*在下面 有response*/
#else
	SetGuard2Controller();
#endif
	
	//	/********SendGuardStatusChange*************/
	//	CloseCyclicSendWhenReceiveNewATCmd();
	//	SendATCmdResponseCode(RCODE_Guarded);
	//	//AT.flag_alreadysent = true ;

#if BUZZER_COMPILE	
	/********BuzzerSet*************/
	Buzzer.counter_targetringnum_mul2 = Ring2_Mul2 ;
	Buzzer.counter_nowringnum_mul2 = 0;
	Buzzer.timer_addcounter_per2ms = 0 ;
#endif	

#if VOICEPACKET_COMPILE
	SetVoicePacketCmd(VOICE_LOCK) ;
	SetSIFSendCyclesValue(&VoicePacketSIF);
#endif

	
	flag_guard_buffcmd = true;
	Guard.status = Status_GuardProcessActing;
	
	//CheckGuardStatusAndResponse((uint8_t)Status_Guarded);
	//SimpleBLEPeripheral_enqueueAsySendMsg() ;
	
	//CheckGuardStatusAndResponse();
	//Guard.status_ATCmd_response = false ;	// Flag AT guardCmd response clear
	//Guard.status_ATCmd_response = true ;

#if GPS_COMPILE	
	BLEStatusChangeMsg_EnQueue((uint8_t)Status_Guarded);
#endif
	
#if ResetMcuAccWork_COMPILE	
	NotifyFlashAction(Flash_Protection);
#endif

}

/*********************************************************************
* @brief  
 *
 * @param   a0, a1 - not used.
 *
 * @return  None.
 */
void ResetGuardStatus(void)
{	
#if MECHANICALLOCK_INTERNAL_COMPILE
	if(Status_MotorLockAction == MechalMotorLock.status)
		MechalMotorLockStopDrive();	
	#if(ELU_ActionSingle_Mode == ELU_BreakSingle_Mode)
	MechalMotorLock.status = Status_WaitForBrake;		//wait for poweron success	 
	#elif(ELU_ActionSingle_Mode == ELU_DirectlyUnlocked_Mode)
	MechalMotorLock.status = Status_ReadyToUnlock ;
	#else
	#endif
	//if(true == AT.status_BLEconnect)	//if cmd from gprs	
		//ResponseMechalMotorLock();
#else
	ResetGuard2Controller();	
#endif

	
	//	/********SendGuardStatusChange*************/
	//	CloseCyclicSendWhenReceiveNewATCmd();
	//	SendATCmdResponseCode(RCODE_UnGuarded);
	//	//AT.flag_alreadysent = true ;
#if BUZZER_COMPILE	
	/********BuzzerSet*************/
	Buzzer.counter_targetringnum_mul2 = Ring1_Mul2 ;
	Buzzer.counter_nowringnum_mul2 = 0;
	Buzzer.timer_addcounter_per2ms = 0 ;
#endif	

#if VOICEPACKET_COMPILE
	SetVoicePacketCmd(VOICE_UNLOCK) ;
	SetSIFSendCyclesValue(&VoicePacketSIF);
#endif

#if(KeyLessStart == Anti_theftDevice_Function)
	KeyLessPowerOn();
#endif	
	flag_guard_buffcmd = false ;
	Guard.status = Status_UnguardProcessActiong_BeforPowerOn;	
	
	//in order to power off acc after 10min
	Motor.status_motor = Status_Motor_Stall ;
	Motor.timer_stall_500ms = 0 ;
	
	//CheckGuardStatusAndResponse((uint8_t)Status_UnGuarded);	
	//SimpleBLEPeripheral_enqueueAsySendMsg() ;
	
	//CheckGuardStatusAndResponse();
	//Guard.status_ATCmd_response = false ;	// Flag AT guardCmd response clear
	//Guard.status_ATCmd_response = true ;
#if GPS_COMPILE	
	BLEStatusChangeMsg_EnQueue((uint8_t)Status_UnGuarded) ;
#endif
	
#if HistoryMileage_COMPILE   
	Mileage.Meter=0;
	Mileage.counter_hallchange_accumu_under1KM.sum32=0;
#endif
	
#if ResetMcuAccWork_COMPILE	
	NotifyFlashAction(Flash_Protection);
#endif
}


/*******************************************************************************
* 函数名        :
* 函数描述    	:
* 输入参数      : None
* 输出结果      : None
* 返回值        : None
*******************************************************************************/
void SetGuard2Controller(void)
{
#if(Commu_Mode_TwoWire == Commu_Mode)
	//CustomSerialInterfaceMessage.status_guard = true ;
	T2C.CommonData.guardstatusBit = 1 ;
#else
	//GPIO_ResetBits(Port_Guard_Output,Pin_Guard_Output);		//输出低电平 （低电平有效）
	SetPinLevel(Pin_Gout, Low_Level) ;
#endif
}

/*******************************************************************************
* 函数名        :
* 函数描述    	:
* 输入参数      : None
* 输出结果      : None
* 返回值        : None
*******************************************************************************/
void ResetGuard2Controller(void)
{
#if(Commu_Mode_TwoWire == Commu_Mode)
	//CustomSerialInterfaceMessage.status_guard = false	;
	T2C.CommonData.guardstatusBit = 0 ;
#else
	//GPIO_SetBits(Port_Guard_Output,Pin_Guard_Output);		//输出高电平 （低电平有效）
	SetPinLevel(Pin_Gout, High_Level) ;
#endif
}

/*******************************************************************************
* 函数名        :
* 函数描述    	:
* 输入参数      : None
* 输出结果      : None
* 返回值        : None
*******************************************************************************/
static void GuardDeadLineStatusMachine(void)
{
	if(Status_UnReachDeadline == Guard.status_deadline )
	{
		if(IsDeadLineReached())
			Guard.status_deadline = Status_ReachedDeadline_NoExecute ;
	}		//per 1s  Excuse GuardDeadLineApproachPer1s()
	else if(Status_ReachedDeadline_NoExecute == Guard.status_deadline)
	{	 
		if(IsDeadLineReached())
		{
			if(Status_Motor_Park == Motor.status_motor)
			 	Guard.status_deadline = Status_ReachedDeadline_Execute ;
			else
				;
		}
		else
			Guard.status_deadline = Status_UnReachDeadline ;	
	}
	else	//Status_ReachedDeadline_Execute
	{
		if(false == IsDeadLineReached())
			Guard.status_deadline = Status_UnReachDeadline ;	
	}
}

/*******************************************************************************
* 函数名  	 	:
* 函数描述    	:
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
bool IsDeadLineFormatCorrect(const uint8_t *p, uint8_t strlen)
{
    if(*p >= Hours_PerDay)
        return false ;
    if(*(p + 1) >= Minutes_PerHour)
        return false ;
	if(2 == strlen)
		return true ;
	else	//strlen ==3
    {
		if(*(p + 2) >= Seconds_PerMinute)
        	return false ;
    	return true ;
	}
}



/*******************************************************************************
* 函数名  	:
* 函数描述    	:
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
bool IsDeadLineReached(void)
{
#if 0	
	if(Guard.timer_day_guard_timeleft.sum)
		return NO ;
	if(Guard.timer_hour_guard_timeleft)
		return NO ;
	if(Guard.timer_minute_guard_timeleft)
		return NO ;
	if(Guard.timer_second_guard_timeleft)
		return NO ;
	return YES ;
#endif
	for(uint8_t i = 0 ; i < 5 ; i ++ )
		if(Guard.deadlinedata.data[i])
			return false ;
	return true ;	
}

/*******************************************************************************
* 函数名        :
* 函数描述    	:
* 输入参数      : None
* 输出结果      : None
* 返回值        : None
*******************************************************************************/
void GuardDeadLineApproachPer1s(void)
{	
	if(false == IsDeadLineReached())
	{
		if(true == IsVariableZeroIfNotDecrease(&(Guard.deadlinedata.timerleft_second)))
		{
			if(true == IsVariableZeroIfNotDecrease(&(Guard.deadlinedata.timerleft_minute)))
			{
				if(true == IsVariableZeroIfNotDecrease(&(Guard.deadlinedata.timerleft_hour)))
				{
					Guard.deadlinedata.timerleft_day.sum -- ;
					Guard.deadlinedata.timerleft_hour = 23 ;
				}
				Guard.deadlinedata.timerleft_minute = 59 ;
			}
			Guard.deadlinedata.timerleft_second = 59 ;
		}
	}	
}

/*******************************************************************************
* 函数名        :
* 函数描述    	:
* 输入参数      : None
* 输出结果      : None
* 返回值        : None
*******************************************************************************/
void MakeSureStillLockByAppWhenReasonChanged(void)
{
	Guard.status_autoguard = Status_Auto ;
	Guard.cmd_lock_From_BLE = Lock_Ebike ;
}

 /*********************************************************************
* @brief  
 *
 * @param   a0, a1 - not used.
 *
 * @return  None.
 */
 void CheckLockButtonEvent(void)
 {
	if(IsButtonEventIfPressedLowLevelAndPressEffect(&LockButton) )
	{
	   //if(( Guard.cmd_lock_From_BLE == Unlock_Ebike)&&(Guard.status_autoguard == Status_UnAuto))
		 if( Status_UnAuto== Guard.status_autoguard )
		  {
		       PTFuncLock(); 
			   if(true == IsBLEConnectStatusInteractive())
		       {
			     CheckGuardStatusAndResponse(0X01);
			     SimpleBLEPeripheral_enqueueAsySendMsg();
		       }
		   }
	   else
	      {
	         if(PkeINDistance==true)
	         {
		     //  if((Guard.cmd_lock_From_BLE == Lock_Ebike)&&(Guard.status_autoguard == Status_Auto))
			   if( Status_Auto== Guard.status_autoguard )
		       {
			     PTFuncUnlock();
				 if(true == IsBLEConnectStatusInteractive())
		         {
			       CheckGuardStatusAndResponse(0X01);
			       SimpleBLEPeripheral_enqueueAsySendMsg();
		         }
		       }
	         }
	       }
	  
	   

	 }
	
 }

#if LONGRENT_COMPILE
void CheckGSButtonEvent(void)
{
    IsButtonEventIfPressedLowLevelAndPressEffect(&GSButton);
	 if(Button_Pressd == GSButton.status)
		{
			 if(Button_Released == GSButton.last_status)
            {
				if(Status_UnReachDeadline == Guard.status_deadline)
				{
                   Guard.cmd_lock_From_BLE = Unlock_Ebike ; 
		           Guard.status_autoguard = Status_UnAuto ;
				}
            }
			 GSButton.last_status = Button_Pressd;
		} 
        else if(Button_Released == GSButton.status)
        {
            if(Button_Pressd == GSButton.last_status)
            {
                Guard.status_autoguard = Status_Auto ;
                Guard.cmd_lock_From_BLE = Lock_Ebike ;
            }
            GSButton.last_status = Button_Released;
        }
}
#endif
