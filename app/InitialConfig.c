#include "defines.h"
#include "InitialConfig.h"

#include "ATControl.h"


#include "BatteryPowerCal.h"
#include "DouSupport.h"
#include "gpscontrol.h"
#include "Guard.h"
#include "KeyLess.h"
#include "MechanicalLock.h"
#include "MileageCal.h"
#include "OLC.h"
#include "SeatLock.h"
#include "T2C.h"
#include "ElectroMagneticLock.h"


/****************************************************/
/**********************function**********************/
/*******************************************************************************
* 
*/


/*******************************************************************************
* 
*/
void ClearAllVariablesInAStruct(uint8_t *p ,uint8_t size )
{
	while(size > 0)
	{
		*p++ = 0 ;
		size -- ;
	}
}
/*******************************************************************************
* 
*/
void ClearAllVariables(void)
{
	ClearAllVariablesInAStruct((uint8_t *)&AT,sizeof(AT_TypeDef));

	ClearAllVariablesInAStruct((uint8_t *)&LV,sizeof(LV_TypeDef)); 
	ClearAllVariablesInAStruct((uint8_t *)&Q,sizeof(Q_TypeDef)); 
	ClearAllVariablesInAStruct((uint8_t *)&Current,sizeof(Current_TypeDef)); 
	ClearAllVariablesInAStruct((uint8_t *)&Motor,sizeof(Motor_TypeDef));
	
	ClearAllVariablesInAStruct((uint8_t *)&BLEMdlCnt,sizeof(BLEModuleConnect_TypeDef));
	
#if DOUSUPPORT_INTERNAL_COMPILE
	ClearAllVariablesInAStruct((uint8_t *)&DouSupport,sizeof(DouSupport_TypeDef));
	#if(DongdaLanniao_DouSupport_ManufactID == DouSupport_Manufacturer_ID) 
		ClearAllVariablesInAStruct((uint8_t *)&DSR,sizeof(DouSupportReceive_TypeDef));
	#endif
#endif

#if ERRORHISTORY_COMPILE		
	ClearAllVariablesInAStruct((uint8_t *)&ErrorHistoryMessage,sizeof(ErrorHistoryMessage_TypeDef)); 
	ClearAllVariablesInAStruct((uint8_t *)&RunMessage,sizeof(RunMessage_TypeDef)); 
#endif
	
	
	//FlashInitial is const
	//ClearAllVariablesInAStruct((uint8_t *)&FlashInitial,sizeof(FlashStruct_TypeDef)); 
	
	ClearAllVariablesInAStruct((uint8_t *)&KeyLess,sizeof(KeyLess_TypeDef)); 
	ClearAllVariablesInAStruct((uint8_t *)&LDPower,sizeof(LDPower_TypeDef)); 
	
#if SEATLOCK_COMPILE
	ClearAllVariablesInAStruct((uint8_t *)&SeatLock,sizeof(SeatLock_TypeDef)); 
#endif
	
#if L9110DRIVE_COMPILE	
	ClearAllVariablesInAStruct((uint8_t *)&SW9V,sizeof(SW9V_TypeDef));
#endif	
	
#if MECHANICALLOCK_INTERNAL_COMPILE	
	ClearAllVariablesInAStruct((uint8_t *)&MechalMotorLock,sizeof(MechalMotorLock_TypeDef)); 
#endif	

	ClearAllVariablesInAStruct((uint8_t *)&Guard,sizeof(Guard_TypeDef));
	
	ClearAllVariablesInAStruct((uint8_t *)&Mileage,sizeof(Mileage_TypeDef));
	
	ClearAllVariablesInAStruct((uint8_t *)&OLCM,sizeof(OLCM_TypeDef));

	ClearAllVariablesInAStruct((uint8_t *)&T2C,sizeof(T2C_TypeDef)); 	
}

#if 0
/*******************************************************************************
* 
*/
void ReadFlashInitial(void)
{
	ClearAllVariables();	
	//ReadFlash();

	//read q_max and mileage_max
	if(Flash_STM32.data.bit32[0]== DATA_FLASH_BLANK)
	{
		Q.data_max.sum32 = Q_Max_InFlash;
		Mileage.counter_hallchange_max.sum32 = Mileage_Max_InFlash ;
	}
	else
	{
		Q.data_max.sum32 = Flash_STM32.data.FlashStruct.q_max;
		Mileage.counter_hallchange_max.sum32  = Flash_STM32.data.FlashStruct.mileage_max;
	}

	//read odometer in flash
	Mileage.odometer_perKM.bit8[3] = 0 ;
	Mileage.odometer_perKM.bit8[2] = Flash_STM32.data.FlashStruct.odometer_2 ;
	Mileage.odometer_perKM.bit8[1] = Flash_STM32.data.FlashStruct.odometer_1 ;
	Mileage.odometer_perKM.bit8[0] = Flash_STM32.data.FlashStruct.odometer_0 ;
	
	//read deadline
	//correct deadline if wrong
	if(false== IsDeadLineFormatCorrect(&Flash_STM32.data.FlashStruct.deadline_hour,2))
	{	
		if(Flash_STM32.data.FlashStruct.deadline_hour>= Hours_PerDay)
			Flash_STM32.data.FlashStruct.deadline_hour= Hours_PerDay - 1 ;
		if(Flash_STM32.data.FlashStruct.deadline_minute>= Minutes_PerHour)
			Flash_STM32.data.FlashStruct.deadline_minute= Minutes_PerHour - 1 ;
	}
	for(uint8_t i = 0 ; i < 4 ; i++)
		Guard.deadlinedata.data[i] = Flash_STM32.data.FlashStruct.deadline_withoutsecond_data[i] ;
#if 0	
	Guard.timer_day_guard_timeleft.sum= Flash_STM32.data.FlashStruct.timer_day_guard_timeleft;
	Guard.timer_hour_guard_timeleft = Flash_STM32.data.FlashStruct.timer_hour_guard_timeleft ;
	Guard.timer_minute_guard_timeleft = Flash_STM32.data.FlashStruct.timer_minute_guard_timeleft ;
#endif	

	//read status
	//if(Flash_STM32.data.FlashStruct.status & BIT_AUTO_Guard)
	if(Flash_STM32.data.FlashStruct.bit_autoguard)
		Guard.status_autoguard = Status_Auto ;
	else
		Guard.status_autoguard = Status_UnAuto ;

	if(Flash_STM32.data.FlashStruct.bit_GPSguard)
		Guard.cmd_lock_From_FroceGPRS = Lock_ByForceGPRS ;
	else
		Guard.cmd_lock_From_FroceGPRS = Unlock_ByForceGPRS ;

# if 0	
	//if(Flash_STM32.data.FlashStruct.status & BIT_DouSupport)
	if(Flash_STM32.data.FlashStruct.bit_dousupport)
		DouSupport.status = Status_RaiseInPlace ;
	else
		DouSupport.status = Status_RetractInPlace ;
#endif
	
	DevicePara.battery_materialsAndmanufact= 		Flash_STM32.data.FlashStruct.battery_materialsAndmanufact;
	DevicePara.battery_section_num = 	Flash_STM32.data.FlashStruct.battery_section_num ;
	DevicePara.battery_aH_battery = 	Flash_STM32.data.FlashStruct.battery_aH_battery ;

	DevicePara.bit_autoguard = Flash_STM32.data.FlashStruct.bit_autoguard ;
	DevicePara.bit_GPSguard = Flash_STM32.data.FlashStruct.bit_GPSguard;
	//DevicePara.bit_dousupport= Flash_STM32.data.FlashStruct.bit_dousupport ;

	DevicePara.wheel_diameter_inch_int = Flash_STM32.data.FlashStruct.wheel_diameter_inch_int;
	DevicePara.wheel_diameter_inch_dec = Flash_STM32.data.FlashStruct.wheel_diameter_inch_dec;	
	DevicePara.gear_ratio_motor = 		Flash_STM32.data.FlashStruct.gear_ratio_motor ;
	DevicePara.gear_ratio_wheel = 		Flash_STM32.data.FlashStruct.gear_ratio_wheel ;
	DevicePara.number_pole_pairs = 		Flash_STM32.data.FlashStruct.number_pole_pairs ;
	DevicePara.batterystandardKM = 		Flash_STM32.data.FlashStruct.batterystandardKM ;
	DevicePara.rated_Q_release_perFrame_A = Flash_STM32.data.FlashStruct.rated_Q_release_perFrame_A ;	
}
#endif
/*******************************************************************************
* 
*/
void VariableInitial(void)
{
	
	
#if(Commu_Mode_Common == Commu_Mode)
	//BLEMdlCnt.timer_connect2controller5V_50ms = 0 ;
	BLEMdlCnt.timer_noconnect2controller5V_50ms = UINT8_MAX ;
	//Mileage.counter_hallchange_accumu.bit32 = 0 ;
#else
	/****初始一线通断开连接*********/
	BLEMdlCnt.timer_connect2controller_per50ms = 0 ;
	
	#if(Commu_Mode_TwoWire == Commu_Mode)
		//T2C.cmd = T2C_SendCommonDataCmd ;
		T2C.CommonData.startcode = T2C_CommonCmd_StartCode;		
		T2C.CommonData.strlencode = T2C_CommonCmd_ValidStrlen;

		T2C.ReadConfig.startcode = T2C_Config_StartCode ;
		T2C.ReadConfig.strlencode = T2C_ReadConfig_ValidStrlen ;

		T2C.WriteConfig.startcode = T2C_Config_StartCode ;
		T2C.WriteConfig.strlencode = T2C_WriteConfig_ValidStrlen ;


		if(None == Guard.status_guardreason)
			T2C.CommonData.guardstatusBit = 0 ;
		else
			T2C.CommonData.guardstatusBit = 1 ;
		
		#if(ThreeSpeedSet == COMPILE_ENABLE)
		T2C.CommonData.ThressSpeedBit = 2 ;
		#endif
		#if(ThreeTorqueSet== COMPILE_ENABLE)
		T2C.CommonData.ThressTorqueBit = 2 ;
		#endif
	#endif
#endif
		
	CalQInitialAndMileageRemainder();	//load Q and Mileage Initial Remainder And Max
	Mileage.caledvalue_hallchange_1KM = CalCounterHallchange1KM();

#if(Commu_Mode_Common != Commu_Mode)
	Mileage.ratio_releasedivmileage_mul256 = ReturnRatioReleaseDivMileageMul256InitialValue();
	//InitialLowPassFilteringPerFrame();
#endif
	
	#if DOUSUPPORT_INTERNAL_COMPILE
		#if(FuzhouPeipeng_DouSupport_ManufactID == DouSupport_Manufacturer_ID)
		#elif(DongdaLanniao_DouSupport_ManufactID == DouSupport_Manufacturer_ID)
	KeyLess.flag_enableDSraise_afterpoweroff = false ;
		#endif
	#endif
		
#if SEATLOCK_COMPILE
	SeatLock.status = Status_Locked ;
	#if ELEMGLOCK_COMPILE
	hEleMglock0 = elemglock_config[0].fxnTablePtr->gethandle(0) ;
	hEleMglock0->fxnTablePtr->init(hEleMglock0,NULL);
	#endif	
#endif	

	
}