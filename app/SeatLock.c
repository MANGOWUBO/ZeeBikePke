#include "defines.h"
#include "SeatLock.h"
#include "systemcontrol.h"
#include "ATControl.h"
#include "MileageCal.h"
#include "MechanicalLock.h"
#include "OLC.h"
#include "ElectroMagneticLock.h"


/*************************************************/
/*******************Variable**********************/
SeatLock_TypeDef	SeatLock ;	
SW9V_TypeDef		SW9V ;

/****************************************************/
/**********************function**********************/
#if SEATLOCK_COMPILE
static void IfSeatLockCmdFlagThenClearAndNotify(void) ;
static void IncreaseSeatLockUnlockedActionFrequent(void) ;

	#if !ELEMGLOCK_COMPILE
static void Seat9VLockLockAction(void) ;
static void Seat9VLockUnlockAction(void) ;
static void Seat9VLockAndChangeStatus(void) ;
	#endif
#endif

#if SEATLOCK_COMPILE
/*******************************************************************************
* ��������    	:
* �������      : None
* ������      : None
* ����ֵ        : None
*******************************************************************************/
void CheckSeatLockActionResult(void)
{
	VariableTimerDecrease(&(SeatLock.timer_action_per50ms));
	if(Status_Locked == SeatLock.status)
	{
		if(SeatLock.flag_cmd_action)
		{
			SeatLock.status = Status_WaitForMotorStop ;
			SeatLock.flag_cmd_action = false ;
			SeatLock.flag_BLE_notify = true ;
		}
	}
	else if(Status_WaitForMotorStop == SeatLock.status)
	{
		if(true == IsHallSpeedNearlyStop())
			SeatUnlockAndChangeStatus() ;
		IfSeatLockCmdFlagThenClearAndNotify();
	}
	else if(Status_OTP_Locked == SeatLock.status)
	{
		IfSeatLockCmdFlagThenClearAndNotify();
	}
	#if ELEMGLOCK_COMPILE
	else if(Status_UnlockAction == SeatLock.status)
	{	
		if(0 == SeatLock.timer_action_per50ms)
		{
			hEleMglock0->fxnTablePtr->poweroff(hEleMglock0); 
			SeatLock.status = Status_Locked ;
			SeatLock.flag_BLE_notify = true ;
		}
		IfSeatLockCmdFlagThenClearAndNotify();
	}	
	#else
	else if(Status_UnlockAction == SeatLock.status)
	{
		if(0 == SeatLock.timer_action_per50ms)
		{
			Seat9VLockStopDrive();	//stop drive and clear timer
			SeatLock.status = Status_Unlocked ;
			SeatLock.timer_action_per50ms = Timer_SeatLockUnlockedDealy_Per50ms ;
			SeatLock.flag_BLE_notify = true ;					
		}
		IfSeatLockCmdFlagThenClearAndNotify();
	}
	else if(Status_Unlocked == SeatLock.status)	
	{
		if(0 == SeatLock.timer_action_per50ms )
			Seat9VLockAndChangeStatus() ;
		IfSeatLockCmdFlagThenClearAndNotify();
	}
	else if(Status_LockAction == SeatLock.status)	
	{
		if(0 == SeatLock.timer_action_per50ms )
		{
			Seat9VLockStopDrive();	//stop drive and clear timer
			SeatLock.status = Status_Locked ;
			SeatLock.flag_BLE_notify = true ;
		}
		IfSeatLockCmdFlagThenClearAndNotify();
	}
	#endif
	else
		;
	
	SeatLockNotifyWhenStatusChanged();

}

/*********************************************************************
* @brief  
*
* @param   
*
* @return  None.
*/
static void IfSeatLockCmdFlagThenClearAndNotify(void)
{
	if(SeatLock.flag_cmd_action)
	{
		SeatLock.flag_cmd_action = false ;
		SeatLock.flag_BLE_notify = true ;
	}
}
/*********************************************************************
* @brief  
 *
 * @param   
 *
 * @return  None.
 */
 void UpdateSeatLockUnlockedActionFrequent(void)
{
	if(true == IsVariableTimerDecreaseToZero(&(SeatLock.timer_unlockactionfrequent_per1s)))
	{
		SeatLock.counter_unlockactionfrequent = 0 ;
		if(Status_OTP_Locked == SeatLock.status)
			SeatLock.status = Status_Locked ;
	}
 }

 /*********************************************************************
* @brief  
 *
 * @param   
 *
 * @return  None.
 */
 static void IncreaseSeatLockUnlockedActionFrequent(void)
{
	SeatLock.counter_unlockactionfrequent ++ ;
	if(1 == SeatLock.counter_unlockactionfrequent)
		SeatLock.timer_unlockactionfrequent_per1s = Timer_SeatLockUnlockedActionFrequent_Per1s ;
}
	
 /*******************************************************************************
 * �������� 	 :
 * ������� 	 : None
 * ������ 	 : None
 * ����ֵ		 : None
 *******************************************************************************/
 void SeatUnlockAndChangeStatus(void)
 {
	 if(SeatLock.counter_unlockactionfrequent <= Counter_UnlockActionFrequent)
	 {
#if ELEMGLOCK_COMPILE
		hEleMglock0->fxnTablePtr->poweron(hEleMglock0,&SeatLock.timer_action_per50ms) ;
		SeatLock.status = Status_UnlockAction ;
		IncreaseSeatLockUnlockedActionFrequent();
#else
		 SW9V.targetlock.seatlockBit = 1 ;
		 SW9V.direction.seatlockdirBit = 0 ;
#endif
	 }
	 else
	 {
		 SeatLock.status = Status_OTP_Locked ;
		 //ResponseSeatLockAction(); 
		 SeatLock.flag_BLE_notify = true ;
	 }
 }
 



	#if !ELEMGLOCK_COMPILE
/*******************************************************************************
* ��������    	:MA+���� MA-���� ����	MA+:���������� MA-������������
* �������      : None
* ������      : None
* ����ֵ        : None
*******************************************************************************/
static void Seat9VLockLockAction(void)
{
	SetPinLevel(Pin_MotorA1,Low_Level) ;
	SetPinLevel(Pin_MotorA2,High_Level) ;

	SeatLock.timer_action_per50ms = Timer_SeatLockLockAction_Per50ms ;
	SeatLock.status = Status_LockAction ;
	//if(BLE_ConnectWithInteractive == GetBLEConnectStatus())
		//ResponseSeatLockAction();
}

/*******************************************************************************
* ��������    	:MA+���� MA-���� ����
* �������      : None
* ������      : None
* ����ֵ        : None
*******************************************************************************/
static void Seat9VLockUnlockAction(void)
{
	SetPinLevel(Pin_MotorA1,High_Level) ;
	SetPinLevel(Pin_MotorA2,Low_Level) ;
	
	SeatLock.timer_action_per50ms = Timer_SeatLockUnlockAction_Per50ms ;
	SeatLock.status = Status_UnlockAction ;
	
	IncreaseSeatLockUnlockedActionFrequent();
	
	//if(BLE_ConnectWithInteractive == GetBLEConnectStatus())
		//ResponseSeatLockAction();
		
}

/*******************************************************************************
*
*/
static void Seat9VLockAndChangeStatus(void)
{
	SW9V.targetlock.seatlockBit = 1 ;
	SW9V.direction.seatlockdirBit = 1 ;
}

/*******************************************************************************
* ��������    	:
* �������      : None
* ������      : None
* ����ֵ        : None
*******************************************************************************/
void Seat9VLockStopDrive(void)
{
	SetPinLevel(Pin_SW9V,Low_Level) ;
	SetPinLevel(Pin_MotorA1,Low_Level) ;
	SetPinLevel(Pin_MotorA2,Low_Level) ;
	//SeatLock.timer_action_per50ms = 0 ;
}





	#endif
#endif

/*******************************************************************************
* ��������    	:
* �������      : None
* ������      : None
* ����ֵ        : None
*******************************************************************************/
void SW9VDelayAfterPowerOnService(void)
{
	VariableTimerDecrease(&(SW9V.timer_delay_afterpoweron_per50ms)) ;
	if(Status_SW9V_Block == SW9V.status)
	{
			if(SW9V.targetlock.para)
			{
				//GPIO_SetBits(Port_9VSW,Pin_9VSW);					//9V�����ͨ
				SetPinLevel(Pin_SW9V, High_Level) ;
				SW9V.timer_delay_afterpoweron_per50ms = Timer_SW9V_DelayAfterPowerOn_Per50ms ;
				SW9V.status = Status_SW9V_PoweredOn;
			}
	}
	else if(Status_SW9V_PoweredOn == SW9V.status)
	{
			
			if(0 == SW9V.timer_delay_afterpoweron_per50ms )
			{
				if(SW9V.targetlock.mechalockBit)
				{
					if(SW9V.direction.mechalockdirBit)
						MechalMotorLockAction();
					else
						MechalMotorUnlockAction();
				}
				else if(SW9V.targetlock.seatlockBit)
				{
#if (!ELEMGLOCK_COMPILE	&& SEATLOCK_COMPILE)		
					if(SW9V.direction.seatlockdirBit)
						Seat9VLockLockAction();
					else
						Seat9VLockUnlockAction();
#endif					
				}
				else
					;
				SW9V.status = Status_SW9V_WeakedUpLockAction ;
			}
	}
	else if(Status_SW9V_WeakedUpLockAction == SW9V.status)
	{
			SW9V.targetlock.para = 0 ;		//clear flag
			SW9V.status = Status_SW9V_Block ;
	}
	else
			;	
	
}



