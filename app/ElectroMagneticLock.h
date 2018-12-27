#ifndef __ELECTROMAGNETICLOCK_H
#define __ELECTROMAGNETICLOCK_H
#include "defines.h"

/*************************************************/
/*******************Define**********************/		


/*************************************************/
/*********************TypeDef*********************/
typedef enum 
{
  EleMgLock_Status_Unlocked = 0 ,
  EleMgLock_Status_Locked, 
  EleMgLock_Status_UnlockAction ,
  EleMgLock_Status_LockAction ,
  EleMgLock_Status_WaitForMotorStop ,
  EleMgLock_Status_UnlockFail_Locked ,
  EleMgLock_Status_OTP_Locked ,			//发热保护 此时为上锁状态
}ELEMGLOCK_Status_TypeDef;

//A handle that is returned from a EleMgLock_Init() call.
typedef struct ELEMGLOCK_Config_TypeDef      *EleMgLock_Handle;

typedef struct 
{
	ELEMGLOCK_Status_TypeDef		state_init	;		/*!< EleMgLock state object */
	uint8_t 						timer_poweron_limit_per50ms ;
	LevelStatus_TypeDef			    valid_action_level	;
	LevelStatus_TypeDef			    restore_level	;
}EleMgLock_Params_TypeDef;




typedef EleMgLock_Handle 	(*EleMgLockGetHandle_Fxn)		(uint8_t index) ;
typedef void 				(*EleMgLockInit_Fxn)        	(EleMgLock_Handle hEleMgLock,EleMgLock_Params_TypeDef *params);
typedef void 				(*EleMgLockPowerOn_Fxn)         (EleMgLock_Handle hEleMgLock,uint8_t * ptimer);
typedef void 				(*EleMgLockPowerOff_Fxn)        (EleMgLock_Handle hEleMgLock);



typedef struct  
{
	EleMgLockGetHandle_Fxn  gethandle ;
	EleMgLockInit_Fxn		init;
	EleMgLockPowerOn_Fxn	poweron ;
	EleMgLockPowerOff_Fxn	poweroff ;
}EleMgLock_FxnTable_TypeDef ;


typedef struct  
{
	ELEMGLOCK_Status_TypeDef		state;				/*!< EleMgLock state object */
	uint8_t 						timer_poweron_limit_per50ms ;
	LevelStatus_TypeDef			    valid_action_level	;
	LevelStatus_TypeDef			    restore_level	;
}ELEMGLOCK_Object_TypeDef;
	
typedef const struct 
{
    PIN_Id  elemglockPin;   
}ELEMGLOCK_HwAttrs_TypeDef;

typedef struct ELEMGLOCK_Config_TypeDef
{
    /*! Pointer to a table of driver-specific implementations of UART APIs */
    EleMgLock_FxnTable_TypeDef const    *fxnTablePtr;

    /*! Pointer to a driver specific data object */
    void                   *object;

    /*! Pointer to a driver specific hardware attributes structure */
    void          const    *hwAttrs;
} ELEMGLOCK_Config_TypeDef;



/*************************************************/
/*******************Variable**********************/
/* EleMgLock function table pointer */
extern const EleMgLock_FxnTable_TypeDef elemglock_fxnTable;
extern const ELEMGLOCK_Config_TypeDef elemglock_config[] ;
extern EleMgLock_Handle	hEleMglock0 ;


/*************************************************/
/*******************function**********************/

#endif
