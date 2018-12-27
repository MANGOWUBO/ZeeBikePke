#ifndef __SEATLOCK_H
#define __SEATLOCK_H


/*************************************************/
/*******************Define**********************/		
#if(TangZe_SeatLock_Manufacturer_ID == SeatLock_Manufacturer_ID)
#define	Timer_SeatLockLockAction_Per50ms			18	//900ms
#define	Timer_SeatLockUnlockAction_Per50ms			18	//900ms
#define	Timer_SeatLockUnlockedDealy_Per50ms			40	//2000ms =2s

#elif(LeCi_BatteryLock_Manufacturer_ID == SeatLock_Manufacturer_ID)
#define	Timer_SeatLockLockAction_Per50ms			40	//2s
#define	Timer_SeatLockUnlockAction_Per50ms			40	//2s
#define	Timer_SeatLockUnlockedDealy_Per50ms			200//40	//2000ms =2s  //10s

#else
#endif




#if ELEMGLOCK_COMPILE
#define	Counter_UnlockActionFrequent				20
#define	Timer_SeatLockUnlockedActionFrequent_Per1s	40	//40s
#else
#define	Counter_UnlockActionFrequent				2
#define	Timer_SeatLockUnlockedActionFrequent_Per1s	40	//40s
#endif

#define	Timer_SW9V_DelayAfterPowerOn_Per50ms		20	//1s

/*************************************************/
/*********************TypeDef*********************/
typedef enum
{
  Status_Unlocked = 0 ,
  Status_Locked, 
  Status_UnlockAction ,
  Status_LockAction ,
  Status_WaitForMotorStop ,
  Status_UnlockFail_Locked ,
  Status_OTP_Locked ,			//发热保护 此时为上锁状态
}StatusSeatLock_TypeDef;

typedef struct
{
	bool	flag_cmd_action ;
	StatusSeatLock_TypeDef			status ;
	uint8_t timer_action_per50ms ;
	uint8_t counter_unlockactionfrequent ;
	uint8_t timer_unlockactionfrequent_per1s ;
	bool 	flag_BLE_notify ;
}SeatLock_TypeDef;

typedef enum
{
  	Status_SW9V_Block = 0 ,			//wait for new 9VSW cmd
  	//Status_SW9VPowerOn_Action, 	//powon in action
  	Status_SW9V_PoweredOn ,			//has been powered on			
  	Status_SW9V_WeakedUpLockAction,  	
}SW9VStatus_TypeDef;

typedef union
{
	uint8_t para ;
	struct
	{
		uint8_t seatlockBit		:	1 ;
		uint8_t mechalockBit	:	1 ;
		uint8_t reservedBit		:	6 ;
	};
}SW9VTargetLock_TypeDef ; 

typedef union
{
	uint8_t para ;
	struct
	{
		uint8_t seatlockdirBit		:	1 ;
		uint8_t mechalockdirBit		:	1 ;
		uint8_t reserveddirBit		:	6 ;
	};
}SW9VLockActionDir_TypeDef ;			//lock action direction
										//0:unlock ,1:lock


typedef struct
{
	SW9VStatus_TypeDef			status ;
	uint8_t 					timer_delay_afterpoweron_per50ms ;
	SW9VTargetLock_TypeDef 		targetlock ;
	SW9VLockActionDir_TypeDef	direction ;
}SW9V_TypeDef;






/*************************************************/
/*******************Variable**********************/
extern SeatLock_TypeDef	SeatLock ;
extern SW9V_TypeDef		SW9V ;

/*************************************************/
/*******************function**********************/
#if SEATLOCK_COMPILE
extern void CheckSeatLockActionResult(void) ;	
extern void UpdateSeatLockUnlockedActionFrequent(void) ;
extern void SeatUnlockAndChangeStatus(void);


	#if !ELEMGLOCK_COMPILE
extern void Seat9VLockStopDrive(void) ;
	#endif
#endif
extern void SW9VDelayAfterPowerOnService(void);
#endif

