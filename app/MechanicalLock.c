#include "defines.h"
#include "simple_peripheral.h"
#include "systemcontrol.h"
#include "ATControl.h"
#include "MechanicalLock.h"
#include "DouSupport.h"
#include "SeatLock.h"
#include "OLC.h"
#include "KeyLess.h"
#include "Guard.h"
#include "MileageCal.h"

/*************************************************/
/*******************Variable**********************/
MechalMotorLock_TypeDef	MechalMotorLock ;

#if(LeCi_MechanicalLock_Manufacturer_ID == MechanicalLock_Manufacturer_ID)
static uint8_t cmd = 0 ; 
//MotorSIF_TypeDef MotorSIF ;
static void LoadMechanicallMotorLockSIFByte(void) ;

SIFOUT_TypeDef MechalMotorLockSIF =
{
	.id = 1 ,
	.pin_id = Pin_MotorD1 ,
	.mode = Data_HighLevelFirst ,
	.fixtimer_head_lowlevel_per100us = 120 ,
	.fixtimer_head_highlevel_per100us = 2 ,
	.fixtimer_data_longlevel_per100us =  12,//12 ,
	.fixtimer_data_shortlevel_per100us = 4,//4 ,
	.fixcounter_byte = 4 ,
	.fixcounter_cycle = 6 ,
	.status  = SIF_IDLE,
	.timer_lowlevel_per100us = 0 ,
	.timer_highlevel_per100us = 0 ,
	.index_byte = 0 ,
	.index_bit = 0 ,
	.index_cycle = 0 ,
	//.databuff[SIFOUT_BYTENUM_MAX] ={0} ,
	.data_send = 0 ,
	.LoadSIFOutByteFunc = LoadMechanicallMotorLockSIFByte 
};



#endif

/****************************************************/
/**********************function**********************/
/*******************************************************************************
* 函数描述    	:
* 输入参数      : None
* 输出结果      : None
* 返回值        : None
*******************************************************************************/
void CheckMechalMotorLockActionResult(void)
{ 
	VariableTimerDecrease(&(MechalMotorLock.timer_action_per50ms)) ;
	
#if(ELU_ActionSingle_Mode == ELU_BreakSingle_Mode)	
	if(Status_WaitForBrake == MechalMotorLock.status)
	{
		if(true == MechalMotorLock.flag_breakconfirm)	
		{
			SW9V.targetlock.mechalockBit = 1 ;
			SW9V.direction.mechalockdirBit = 0 ;
		}
		else
			;
	}
#elif(ELU_ActionSingle_Mode == ELU_DirectlyUnlocked_Mode)
	if(Status_ReadyToUnlock == MechalMotorLock.status)
	{
		SW9V.targetlock.mechalockBit = 1 ;
		SW9V.direction.mechalockdirBit = 0 ;
	}
#else	
#endif	

	else if(Status_MotorUnlockAction == MechalMotorLock.status)
	{
		if( 0 == MechalMotorLock.timer_action_per50ms)
		{
			MechalMotorLockStopDrive();	//stop drive and clear timer
			MechalMotorLock.status = Status_MotorUnlocked ;
			#if(ELU_ActionSingle_Mode == ELU_BreakSingle_Mode)
			if(IsBLEConnectStatusInteractive())
				ResponseMechalMotorLock(NULL);
			
				#if DOUSUPPORT_INTERNAL_COMPILE
					#if(FuzhouPeipeng_DouSupport_ManufactID == DouSupport_Manufacturer_ID)
			DouSupport.cmdfrom = DouSupportActionCmdFrom_FlowControl;
					#endif
			DouSupportRetractAction() ;
			//DouSupport.status = Status_RetractingAction ;
				#else
			ResetGuard2Controller();
				#endif
			#elif(ELU_ActionSingle_Mode == ELU_DirectlyUnlocked_Mode)
			ResetGuard2Controller();
//			if(IsBLEConnectStatusInteractive())
//			{
//				//ResponseMechalMotorLock();
//				ResponseDouSupportStatusAndErrorChanged(NULL);		//in order to tell the phone app that the action is finished 
//				SimpleBLEPeripheral_enqueueAsySendMsg();
//			}
			#else
			#endif
			
		}
		else
			;
	}
	else if(Status_MotorWaitForStop == MechalMotorLock.status)	
	{				
		if(true == IsHallSpeedNearlyStop())
		{
			SW9V.targetlock.mechalockBit = 1 ;
			SW9V.direction.mechalockdirBit = 1 ;
		}
		else	
		{
			if(HallCounter_WhenYXTBroken == GetHallSpeed())	//48VSW PowerOn,YXT broken
			{
				if(0 == MechalMotorLock.timer_waitformotorstop_per1s)	
				{
					SW9V.targetlock.mechalockBit = 1 ;
					SW9V.direction.mechalockdirBit = 1 ;
				}
				else
					;
			}
		}
	}
	else if(Status_MotorLockAction == MechalMotorLock.status)	
	{
		if(0 == MechalMotorLock.timer_action_per50ms)
		{
			MechalMotorLockStopDrive();	//stop drive and clear timer
			MechalMotorLock.status = Status_MotorLocked ;
			
			//关闭电源输出
			KeyLessPowerOff();

			#if DOUSUPPORT_INTERNAL_COMPILE	
			//DouSupport.status = Status_WaitForPowerOff;
			KeyLess.flag_enableDSraise_afterpoweroff = true ;
			#else
				//#if( !DOUSUPPORT_EXTERNAL_COMPILE)
			//if(BLE_ConnectWithInteractive == GetBLEConnectStatus())	
			//{
				////ResponseMechalMotorLock();
				//ResponseDouSupportStatusAndErrorChanged(NULL);		//in order to tell the phone app that the action is finished 
				////#endif
				//SimpleBLEPeripheral_enqueueAsySendMsg();
			//}
			#endif
		}
		else
			;
	}	
	else
		;
}


/*******************************************************************************
* 函数描述    	:
* 输入参数      : None
* 输出结果      : None
* 返回值        : None
*******************************************************************************/
void MechalMotorLockAction(void)
{
#if(TangZe_MechanicalLock_Manufacturer_ID== MechanicalLock_Manufacturer_ID)		
	SetPinLevel(Pin_MotorD1,High_Level) ;
	SetPinLevel(Pin_MotorD2,Low_Level) ;
#elif(LeCi_MechanicalLock_Manufacturer_ID == MechanicalLock_Manufacturer_ID)
	SetSIFSendCyclesValue(&MechalMotorLockSIF) ;
	cmd = 1 ;		//lock
#elif(Leci_FaucetLock_Manufacturer_ID == MechanicalLock_Manufacturer_ID)
	SetPinLevel(Pin_MotorA1,High_Level) ;
	SetPinLevel(Pin_MotorA2,Low_Level) ;
#else
#endif

	MechalMotorLock.timer_action_per50ms = Timer_MechalMotorLockLockAction_Per50ms ;
	MechalMotorLock.status = Status_MotorLockAction ;
	if(IsBLEConnectStatusInteractive())
		ResponseMechalMotorLock(NULL);
}

/*******************************************************************************
* 函数描述    	: D1线：棕线 D2线：蓝线
* 输入参数      : None
* 输出结果      : None
* 返回值        : None
*******************************************************************************/
void MechalMotorUnlockAction(void)
{
#if(TangZe_MechanicalLock_Manufacturer_ID== MechanicalLock_Manufacturer_ID)	
	SetPinLevel(Pin_MotorD1,Low_Level) ;
	SetPinLevel(Pin_MotorD2,High_Level) ;
#elif(LeCi_MechanicalLock_Manufacturer_ID == MechanicalLock_Manufacturer_ID)
	SetSIFSendCyclesValue(&MechalMotorLockSIF) ;
	cmd = 0 ;		//unlock	
#elif(Leci_FaucetLock_Manufacturer_ID == MechanicalLock_Manufacturer_ID)
	SetPinLevel(Pin_MotorA1,Low_Level) ;
	SetPinLevel(Pin_MotorA2,High_Level) ;	
#else
#endif
	MechalMotorLock.timer_action_per50ms = Timer_MechalMotorLockUnlockAction_Per50ms ;
	MechalMotorLock.status = Status_MotorUnlockAction ;
	if(IsBLEConnectStatusInteractive())
		ResponseMechalMotorLock(NULL);

}

/*******************************************************************************
* 函数描述    	:
* 输入参数      : None
* 输出结果      : None
* 返回值        : None
*******************************************************************************/
void MechalMotorLockStopDrive(void)
{
#if(TangZe_MechanicalLock_Manufacturer_ID== MechanicalLock_Manufacturer_ID)		
	SetPinLevel(Pin_MotorD1, Low_Level) ;
	SetPinLevel(Pin_MotorD2, Low_Level) ;
#elif(Leci_FaucetLock_Manufacturer_ID == MechanicalLock_Manufacturer_ID)
	SetPinLevel(Pin_MotorA1,Low_Level) ;
	SetPinLevel(Pin_MotorA2,Low_Level) ;
#else	
#endif	
	//GPIO_ResetBits(Port_9VSW,Pin_9VSW);						//9V输出断开
	SetPinLevel(Pin_SW9V, Low_Level) ;
	
}


/********************************************************
* 
*/
void UpdateTimerWaitForMotorStop(void)
{
	VariableTimerDecrease(&(MechalMotorLock.timer_waitformotorstop_per1s)) ;
}


#if(LeCi_MechanicalLock_Manufacturer_ID == MechanicalLock_Manufacturer_ID)
/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
static void LoadMechanicallMotorLockSIFByte(void)
{	
	#if(MechanicalLock_Version_1 == MechanicalLock_Version)	
	MechalMotorLockSIF.databuff[1] = MotorSIF_Byte1 ;
	MechalMotorLockSIF.databuff[2] = MotorSIF_Byte2 ;
	if(cmd )
		MechalMotorLockSIF.databuff[3] = MotorSIF_LockCmd ;
	else
		MechalMotorLockSIF.databuff[3] = MotorSIF_UnlockCmd ;
	#elif(MechanicalLock_Version_2 == MechanicalLock_Version)	
	if(cmd )
	{
		MechalMotorLockSIF.databuff[1] = MotorSIF_LockCmd_Byte1 ;
		MechalMotorLockSIF.databuff[2] = MotorSIF_LockCmd_Byte2 ;
		MechalMotorLockSIF.databuff[3] = MotorSIF_LockCmd_Byte3 ;
	}
	else
	{
		MechalMotorLockSIF.databuff[1] = MotorSIF_UnlockCmd_Byte1 ;
		MechalMotorLockSIF.databuff[2] = MotorSIF_UnlockCmd_Byte2 ;
		MechalMotorLockSIF.databuff[3] = MotorSIF_UnlockCmd_Byte3 ;	
	}
	#else
	#endif
}
#endif
