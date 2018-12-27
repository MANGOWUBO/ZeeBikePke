#ifndef __MECHANICALLOCK_H
#define __MECHANICALLOCK_H

#include "systemcontrol.h"

/*************************************************/
/*******************Define**********************/		
//#define	Timer_MechalMotorLockLockAction_Per100ms			35//20//2000ms
//#define	Timer_MechalMotorLockUnlockAction_Per100ms			35//20//2000ms
#define	Timer_WaitForMotorStop_Per1s						20	//20s

#define	MechanicalLock_Version_1							1
#define	MechanicalLock_Version_2							2

#define	MechanicalLock_Version								MechanicalLock_Version_2

	#if(LeCi_MechanicalLock_Manufacturer_ID == MechanicalLock_Manufacturer_ID)
		#if(MechanicalLock_Version_1 == MechanicalLock_Version)	
#define  	MotorSIF_Byte1									0XFE
#define  	MotorSIF_Byte2									0X58
#define		MotorSIF_LockCmd								0X89
#define		MotorSIF_UnlockCmd								0X86
		#elif(MechanicalLock_Version_2 == MechanicalLock_Version)
#define  	MotorSIF_LockCmd_Byte1							0X11
#define  	MotorSIF_LockCmd_Byte2							0X22
#define  	MotorSIF_LockCmd_Byte3							0X39
#define  	MotorSIF_UnlockCmd_Byte1						0X33
#define  	MotorSIF_UnlockCmd_Byte2						0X22
#define  	MotorSIF_UnlockCmd_Byte3						0X16
		#else
		#endif

	
#define		MotorSIF_Strlen									(1 + 3)		

#define		MotorSIF_Send_Cycle_Counter						5//2

	#endif



/*************************************************/
/*********************TypeDef*********************/
typedef enum
{
	Status_MotorUnlocked = 0 ,
	Status_MotorLocked, 
	Status_MotorUnlockAction ,
	Status_MotorLockAction ,
	#if(ELU_ActionSingle_Mode == ELU_BreakSingle_Mode)
	Status_WaitForBrake , 		//等待刹车信号 一旦有刹车信号就执行机械锁解锁动作
	#elif(ELU_ActionSingle_Mode == ELU_DirectlyUnlocked_Mode)
	Status_ReadyToUnlock ,		//no break required,action directly
	#else 
	#endif  
	Status_MotorWaitForStop ,
}StatusMechalMotorLock_TypeDef;
		
typedef struct
{
	StatusMechalMotorLock_TypeDef	status ;
	uint8_t timer_action_per50ms ;
	uint8_t counter_brake_filter ;
	bool	flag_breakconfirm ;
	uint8_t timer_waitformotorstop_per1s ;
}MechalMotorLock_TypeDef;





/*************************************************/
/*******************Variable**********************/
extern MechalMotorLock_TypeDef	MechalMotorLock ; 
	#if(LeCi_MechanicalLock_Manufacturer_ID == MechanicalLock_Manufacturer_ID)
extern SIFOUT_TypeDef MechalMotorLockSIF ;
	#endif	

/*************************************************/
/*******************function**********************/
extern void CheckMechalMotorLockActionResult(void) ;
extern void MechalMotorLockAction(void) ;
extern void MechalMotorUnlockAction(void) ;
extern void MechalMotorLockStopDrive(void) ;
extern void UpdateTimerWaitForMotorStop(void) ;

	
#endif
