#ifndef __GUARD_H
#define __GUARD_H

/*********************Annotate********************************/
/*≤‚ ‘128min ŒÛ≤Ó‘⁄2min */
/*********************typedef************************/
typedef enum
{
  Status_UnGuarded  = 0 ,
  Status_Guarded ,
  Status_GuardProcessActing ,
  Status_UnguardProcessActiong_BeforPowerOn ,
  Status_UnguardProcessActiong_BeforBreak ,
  Status_UnguardProcessActiong_AfterBreak ,
}StatusGuard_TypeDef;
  
typedef enum
{
  Status_UnAuto  = 0 ,
  Status_Auto ,
}AutoGuard_TypeDef;
  
typedef enum
{
	None = 0 ,
   	Controller_Disconnect ,
  	DeadlineBecomeDue ,
	AppLockCmd,
	RemoteLock,
	ForceGPRSLock,
	GPRSUnconncetLock ,
    KeyUnLock ,
	//ReservedGPRSLock,
}StatusGuardReason_TypeDef;

typedef enum
{
  Status_UnReachDeadline  = 0 ,
  Status_ReachedDeadline_NoExecute,
  Status_ReachedDeadline_Execute,
}StatusReachDeadline_TypeDef;

typedef enum
{
  Unlock_Ebike = 0 ,
  Lock_Ebike ,
}LockCmdFromBle_TypeDef ;

typedef enum
{
  Unlock_ByForceGPRS = 0 ,
  Lock_ByForceGPRS ,
}LockCmdFromForceGPRS_TypeDef ;

//typedef enum
//{
//  Unlock_ByReservedGPRS = 0 ,
//  Lock_ByReservedGPRS ,
//}LockCmdFromReservedGPRS_Type ;

typedef union
{
	uint8_t data[5] ;
	struct
	{
		UnionSumWithH8L8_TypeDef 		timerleft_day ;
		uint8_t 				timerleft_hour ;
		uint8_t 				timerleft_minute ;
		uint8_t 				timerleft_second ;
	};
}DeadLineData_TypeDef ;


typedef struct
{
	StatusGuard_TypeDef		status ;
	StatusGuardReason_TypeDef	status_guardreason ;
	AutoGuard_TypeDef		status_autoguard ;
	bool				status_ATCmd_response ;
	//bool			status_connectwithcontroller ;
	bool				status_unguardrecoverable_whenstayAppconnected ;
	SwitchLevelTypeDef		GInPin ; 
	LockCmdFromBle_TypeDef	cmd_lock_From_BLE ;
/****************deadline***********************/
	StatusReachDeadline_TypeDef	status_deadline ;
	DeadLineData_TypeDef	deadlinedata ;
	
//#if GPS_COMPILE
	LockCmdFromForceGPRS_TypeDef	cmd_lock_From_FroceGPRS ;	
//	LockCmdFromReservedGPRS_Type	cmd_lock_From_reservedGPRS ;
//#endif

}Guard_TypeDef;


/*****************************************************/
/**********************define************************/


/**********************Guard data************************/
#define		Counter_Receive_Data_Max	6


/*	when anti-theft device is guarded ,press the lock button ,
*	GIn will give a 300ms high level and then become low level*/
#define		Timer_GIn_Valid_Per2ms				200		//400ms		//30ms

/*************************************************/
/*******************Variable**********************/
extern Guard_TypeDef	Guard ;

/*************************************************/
/*******************function**********************/
void GuardControl(void);
extern void GuardStatusInitial(void) ;
StatusGuardReason_TypeDef CheckGuardReason(void);
void SetGuardStatus(void);
void ResetGuardStatus(void);
void SetGuard2Controller(void) ;
void ResetGuard2Controller(void) ;


//void ReadGuardDeadLine(void);

bool IsDeadLineFormatCorrect(const uint8_t *,uint8_t) ;
bool IsDeadLineReached(void) ;
void GuardDeadLineApproachPer1s(void);
void MakeSureStillLockByAppWhenReasonChanged(void) ;

extern void CheckLockButtonEvent(void) ;
#if LONGRENT_COMPILE
extern void CheckGSButtonEvent(void) ;
#endif



















#endif