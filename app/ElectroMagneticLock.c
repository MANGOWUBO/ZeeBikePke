#include "ATControl.h"
#if ELEMGLOCK_COMPILE
#include "systemcontrol.h"
#include "ElectroMagneticLock.h"

#include "MileageCal.h"

/*************************************************/
/*******************Variable**********************/
/* Externs */
//const ELEMGLOCK_Config_TypeDef elemglock_config[];

EleMgLock_Handle	hEleMglock0 = NULL ;

/****************************************************/
/**********************function**********************/
static EleMgLock_Handle EleMgLock_gethandle(uint8_t index) ;
static void EleMgLock_init(EleMgLock_Handle hEleMgLock,EleMgLock_Params_TypeDef *params) ;
static void EleMgLock_poweron(EleMgLock_Handle hEleMgLock,uint8_t *ptimer) ;
static void EleMgLock_poweroff(EleMgLock_Handle hEleMgLock) ; 


const EleMgLock_FxnTable_TypeDef elemglock_fxnTable =
{
	EleMgLock_gethandle ,
	EleMgLock_init,
	EleMgLock_poweron ,
	EleMgLock_poweroff 
};

/* Also used to check status for initialization */
//static int EleMgLock_count = -1;

/* Default Watchdog parameters structure */
const EleMgLock_Params_TypeDef EleMgLock_defaultParams = 
{
	EleMgLock_Status_Locked,
	4,			//200ms
	High_Level,
	Low_Level
};
	
/*******************************************************************************
*
*/
static EleMgLock_Handle EleMgLock_gethandle(uint8_t index)
{
	EleMgLock_Handle handle ;
	if(index >= ZBJ02B_ELEMGLOCKCOUNT)
		return (NULL) ;
	handle = (EleMgLock_Handle)&(elemglock_config[index]) ;
	return handle ;
}	
	

/*******************************************************************************
*
*/
static void EleMgLock_init(EleMgLock_Handle hEleMgLock,
						   EleMgLock_Params_TypeDef *params)
{
    ELEMGLOCK_Object_TypeDef  *object  = (ELEMGLOCK_Object_TypeDef  *)hEleMgLock->object;
	
	 /* If params are NULL use defaults */
    if (params == NULL) 
	{
        params = (EleMgLock_Params_TypeDef *)&EleMgLock_defaultParams;
    }

	//set params
	object->state = params->state_init ;
	object->timer_poweron_limit_per50ms = params->timer_poweron_limit_per50ms ;
	object->valid_action_level = params->valid_action_level ;
	object->restore_level = params->restore_level ;
}


/*******************************************************************************
*
*/
static void EleMgLock_poweron(EleMgLock_Handle hEleMgLock,
								uint8_t *ptimer)
{
	//hEleMglock->hwAttrs and object are both  void pointer in config array
	ELEMGLOCK_HwAttrs_TypeDef *hwAttrs = (ELEMGLOCK_HwAttrs_TypeDef *)hEleMgLock->hwAttrs;
	ELEMGLOCK_Object_TypeDef  *object  = (ELEMGLOCK_Object_TypeDef  *)hEleMgLock->object;

	SetPinLevel(hwAttrs->elemglockPin,object->valid_action_level) ; 
	object->state = EleMgLock_Status_UnlockAction ;
	*ptimer = object->timer_poweron_limit_per50ms ;
	
}

/*******************************************************************************
*
*/
static void EleMgLock_poweroff(EleMgLock_Handle hEleMgLock)
{
	//hEleMglock->hwAttrs and object are both  void pointer in config array
	ELEMGLOCK_HwAttrs_TypeDef *hwAttrs = (ELEMGLOCK_HwAttrs_TypeDef *)hEleMgLock->hwAttrs;
	ELEMGLOCK_Object_TypeDef  *object  = (ELEMGLOCK_Object_TypeDef  *)hEleMgLock->object;
	
	//object->timer_poweron_per10ms = object->timer_poweron_limit_per10ms ;	
	SetPinLevel(hwAttrs->elemglockPin,object->restore_level) ; 
	object->state = EleMgLock_Status_Locked ;

}
#endif