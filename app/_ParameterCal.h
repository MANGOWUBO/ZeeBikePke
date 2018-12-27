#ifndef __PARAMETERCAL_H
#define __PARAMETERCAL_H

#include "_Parameter.h"
#include "CustomerPara.h"

#if(CumstomerNum < 5080000000)
#define		DEBUG_COMPILE					COMPILE_ENABLE
#else
#define		DEBUG_COMPILE					COMPILE_DISABLE
#endif
//#define		DEBUG_COMPILE					COMPILE_ENABLE

#if!defined SEATLOCK_COMPILE
	#error "not defined seatlock_compile!"
#endif

#if!defined ELEMGLOCK_COMPILE
	#error "not defined ELEMGLOCK_COMPILE!"
#endif

/******************** Manufacturing Config *****************************/
#define		TEST_EXTFLASH_COMPILE			COMPILE_ENABLE

/******************** WatchDogTimer Config *****************************/
#define		WATCHDOGTIMER_COMPILE			COMPILE_ENABLE

/******************** Customer type *****************************/
#if(Rent_Ebike == Customer_Type)
	#define	DeadlineCounter_COMPILE			COMPILE_ENABLE
#else
	#define	DeadlineCounter_COMPILE			COMPILE_DISABLE
#endif



/********************CyclicSendPeriod Config*****************************/
#if(Commu_Mode_Common == Commu_Mode)
	#define		CyclicSendPeriod					CyclicSendMessagePer30s
#else
	#define		CyclicSendPeriod					CyclicSendMessagePer0_678s
#endif


/********************Anti_theftDevice_Function Config*****************************/
#if(KeyLessStart == Anti_theftDevice_Function)
	/***********是否需要打开电流中断 */
	#define		EXIT_COMPILE			COMPILE_DISABLE		//打开CPMOSFET的大电流中断
#else
	#define		EXIT_COMPILE			COMPILE_DISABLE
#endif

/********************Heart Beat Config****************************/
#define			HEARTBEAT_COMPILE				COMPILE_ENABLE

/********************Commu_Mode_TwoWireSet Auto Config*****************************/
#if(Commu_Mode_Common == Commu_Mode)
	#define		Double_LVSet						COMPILE_DISABLE
	#define		OneKeyRepairSet						COMPILE_DISABLE
	#define		ThreeSpeedSet						COMPILE_DISABLE
	#define		ThreeTorqueSet						COMPILE_DISABLE
#else
	#if(Commu_Mode_TwoWire == Commu_Mode)
		#define		Double_LVSet						COMPILE_DISABLE
		#define		OneKeyRepairSet						COMPILE_DISABLE
		#if((L000001AE == CumstomerNum)||(L000002AB == CumstomerNum)||(L601001AA == CumstomerNum)\
			||(L515101AA == CumstomerNum)||(L063153AA == CumstomerNum) ||(L517101AA == CumstomerNum)||(L50810900 == CumstomerNum))
			#define		ThreeSpeedSet						COMPILE_ENABLE
			#define		ThreeTorqueSet						COMPILE_ENABLE
		#else
			#define		ThreeSpeedSet						COMPILE_DISABLE
			#define		ThreeTorqueSet						COMPILE_DISABLE
		#endif
	#else 
		#define		Double_LVSet						COMPILE_DISABLE
		#define		OneKeyRepairSet						COMPILE_DISABLE
		#define		ThreeSpeedSet						COMPILE_DISABLE
		#define		ThreeTorqueSet						COMPILE_DISABLE
	#endif
#endif


#define		Deadline_Day_InFlash				(uint16_t)0XFFFF
#define		Deadline_Hour_InFlash				(uint8_t)(23)
#define		Deadline_Minu_InFlash				(uint8_t)(59)

/********************GPSController*****************************/
#if(COMPILE_ENABLE == GPS_COMPILE)
	#define		GPS_Commu_Binding				COMPILE_DISABLE
#else
	#define		GPS_Commu_Binding				COMPILE_DISABLE
#endif


/********************Battery Num Auto Cal*****************************/
#define Lead_Acid_LV_OPENCRI_Standard_Single_Cell_A			12	
#define Lead_Acid_Battery_Section_Num						(uint8_t)(LV_Grade / Lead_Acid_LV_OPENCRI_Standard_Single_Cell_A)

#if(36 == LV_Grade) 
	#define MnNiCo_Battery_Con_Section_Num						10
	#define MnNiCo_Battery_UnCon_Section_Num					10
#elif(48 == LV_Grade) 
	#define MnNiCo_Battery_Con_Section_Num						13
	#define MnNiCo_Battery_UnCon_Section_Num					14
#elif(60 == LV_Grade)
	#define MnNiCo_Battery_Con_Section_Num						16
	#define MnNiCo_Battery_UnCon_Section_Num					17
#elif(72 == LV_Grade)
	#define MnNiCo_Battery_Con_Section_Num						20
	#define MnNiCo_Battery_UnCon_Section_Num					20		//72V用20节 60V用16节  48V用13节
#else
	#error "没有合适的"
#endif

#if(Battery_Materials_InFlash > Lead_Acid_Batteries_MaX)
	#if((MnNiCo_Battery_Section_Num != MnNiCo_Battery_Con_Section_Num)&&(MnNiCo_Battery_Section_Num != MnNiCo_Battery_UnCon_Section_Num))
		#error "battery num error!"
	#endif
#endif

#if KEYLESSSTART_EXTERNAL_COMPILE
#define	KEYLESSSTART_INTERNAL_COMPILE						COMPILE_ENABLE
#endif

#if MECHANICALLOCK_EXTERNAL_COMPILE							
#define		MECHANICALLOCK_INTERNAL_COMPILE					COMPILE_ENABLE

	#if(LeCi_MechanicalLock_Manufacturer_ID == MechanicalLock_Manufacturer_ID)
#define		ELU_ActionSingle_Mode							ELU_DirectlyUnlocked_Mode
#define		Timer_MechalMotorLockLockAction_Per50ms			70//20//2000ms
#define		Timer_MechalMotorLockUnlockAction_Per50ms		70//20//2000ms
	#elif(Leci_FaucetLock_Manufacturer_ID == MechanicalLock_Manufacturer_ID)
#define		ELU_ActionSingle_Mode							ELU_DirectlyUnlocked_Mode
#define		Timer_MechalMotorLockLockAction_Per50ms			70//20//2000ms
#define		Timer_MechalMotorLockUnlockAction_Per50ms		70//20//2000ms
	#elif(TangZe_MechanicalLock_Manufacturer_ID == MechanicalLock_Manufacturer_ID)
#define		ELU_ActionSingle_Mode							ELU_DirectlyUnlocked_Mode
#define		Timer_MechalMotorLockLockAction_Per50ms			30//35//2000ms
#define		Timer_MechalMotorLockUnlockAction_Per50ms		30//35//2000ms
	#else
	#endif

#else
	#if DOUSUPPORT_EXTERNAL_COMPILE
#define		MECHANICALLOCK_INTERNAL_COMPILE					COMPILE_ENABLE
#define		ELU_ActionSingle_Mode							ELU_BreakSingle_Mode
#define		Timer_MechalMotorLockLockAction_Per50ms			10//20//2000ms
#define		Timer_MechalMotorLockUnlockAction_Per50ms		10//20//2000ms
	#else
#define		MECHANICALLOCK_INTERNAL_COMPILE					COMPILE_ENABLE
#define		ELU_ActionSingle_Mode							ELU_BreakSingle_Mode
#define		Timer_MechalMotorLockLockAction_Per50ms			10//20//2000ms
#define		Timer_MechalMotorLockUnlockAction_Per50ms		10//20//2000ms

	#endif

#endif


#if DOUSUPPORT_EXTERNAL_COMPILE	
	#define	DOUSUPPORT_INTERNAL_COMPILE					COMPILE_ENABLE
#endif

/**************************9V output Config************************/
#if(MECHANICALLOCK_EXTERNAL_COMPILE ||(!ELEMGLOCK_COMPILE && SEATLOCK_COMPILE ))
	#define	L9110DRIVE_COMPILE							COMPILE_ENABLE
#else
	#define	L9110DRIVE_COMPILE							COMPILE_DISABLE
#endif


#if((Leci_FaucetLock_Manufacturer_ID == MechanicalLock_Manufacturer_ID) && (!ELEMGLOCK_COMPILE))
	#error "no enough 9110"
#endif 

#if(PCB_VERSION < ZBJ02A)
	#if DEBUG_COMPILE
		#define	BUZZER_COMPILE								COMPILE_ENABLE
	#else
		#define	BUZZER_COMPILE								COMPILE_DISABLE
	#endif
#endif

//set COMPILE_DISABLE to reduce the atcmd between phone,so that make sure phone get important message,like dousupport inplace cmd
#define	KEYLESS_RESPONSE_COMPILE						COMPILE_DISABLE


/********************GPS Auto Config*****************************/
#if GPS_COMPILE
	#define	HostType							HostTypeA
#endif

#if  BlePLUS_OBSERVER_COMPILE
#define  PLUS_OBSERVER                     COMPILE_ENABLE
#endif

#define	ERRORHISTORY_COMPILE				COMPILE_DISABLE


#if!defined COUNTRY_COMPILE
	#error "no country defined"
#endif

/**************timeperiod define**************/
#define TimePeriod_10ms_Per100us		100
#define TimePeriod_2ms_Per100us			20
#define TimePeriod_200ms_Per100us		2000
#define TimePeriod_50ms_Per100us		500

#define TimePeriod_10ms_Per2ms			5
#define	TimePeriod_200ms_Per2ms			100

#define TimePeriod_500ms_Per2ms			250
#define TimePeriod_2s_Per2ms			1000
#define TimePeriod_1h_Per1s   			(60 * 60)

#if TESTCODE_TIMER_Q_COMPILE
    #define TimePeriod_1min_Per1s			20
    #define TimePeriod_10min_Per1s 		    (TimePeriod_1min_Per1s*	1)
	#define TimePeriod_15min_Per1s 		    (TimePeriod_1min_Per1s*	2)
    #define TimePeriod_45min_Per1s 		    (TimePeriod_1min_Per1s*	3)
	#define	Time_Stall_Max					(1 * 10  )	
#else
    #define TimePeriod_1min_Per1s			60
    #define TimePeriod_10min_Per1s 		    (TimePeriod_1min_Per1s * 10)
	#define TimePeriod_15min_Per1s 		    (TimePeriod_1min_Per1s * 15)
    #define TimePeriod_45min_Per1s 		    (TimePeriod_1min_Per1s * 45)   
	#define	Time_Stall_Max					(TEN * TimePeriod_1min_Per1s * 2 )	//10 minutes
#endif


/*************************make sure uart is working*********针对蓝牙串口和GPS不通讯问题加看门狗************************/

#define	BLEMAKESUREUARTISWORJING_COMPILE									COMPILE_ENABLE























#endif
