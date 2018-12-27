#ifndef __ATCONTROL_H
#define __ATCONTROL_H

#include "defines.h"
//#include "stdint.h"
/*********************TypeDef**********************/	
//typedef struct 
//{
//	uint8_t	length;//array length
//	void *arr;
//}ArrInfo_TypeDef;

typedef struct 
{
	uint8_t	length;//array length
	uint8_t arr[BLE_CHAR_MAX_LEN];
}ArrInfo_TypeDef;


/*********************TypeDef**********************/	
typedef void (*ResponseStr_TypeDef)( uint8_t * );			//ResponseStr_TypeDef is a funtion ArrInfo(*func)(uint8_t *)

typedef struct
{
	const uint8_t   *compare_str;			//the string to compare with incoming string
	const uint8_t 	compare_str_len;
	ResponseStr_TypeDef ResponseStr;		//the function return the string
}ResponseStrategy_TypeDef;

typedef struct
{ 
	uint8_t timer_sendheartbeat_per1s ;
	uint8_t timer_heartbeat_timeout_per1s ;				
}BLEHeartBeat_TypeDef;






typedef struct
{
	/******************status*************************/
	//bool 			status_BLEconnect ;
	BLEHeartBeat_TypeDef HeartBeat ;	
	
	bool 			flag_alreadysent ;
	bool			mode_cyclicsend ;
	//uint8_t	status_error_realtime ;
	//	uint8_t	timer_cyclicsend_per2ms ;
	/***********************LV***********************/
}AT_TypeDef;		

/*************************************************/
/*******************Variable**********************/
//extern AT_TypeDef AT ;
extern ArrInfo_TypeDef BLEResponse ;
extern AT_TypeDef AT ;

//extern volatile  uint8_t timer_heartbeat_send_per500ms ;
														
/****************************************************/
/**********************function**********************/
void processDisconnectWithBLEApp();
uint8_t GetCurrentCommandLength();
extern uint8_t getLengthOfAsynchronousNotify(void) ;
extern void* getHandleOfAsynchronousNotify(void) ;
//ArrInfo_TypeDef ATControl(uint8_t* in, const uint8_t len);
//ArrInfo_TypeDef ResponseErrorATCmd(void);						
//ArrInfo_TypeDef ResponseErrorATCmdErrorFormat(void) ;
////ArrInfo_TypeDef ResponseErrorATCmdConncetCmdNotYetSent(uint8_t* para);
//static ArrInfo_TypeDef ResponseErrorATCmdConncetCmdNotYetSent(void);
//ArrInfo_TypeDef ResponseErrorATCmdConncetCmdSendRepeatedly(uint8_t* para);
//ArrInfo_TypeDef ResponseATBLEConnect(uint8_t* para);
extern bool ATControl(uint8_t* in, const uint8_t len);	

extern void CheckGuardStatusAndResponse(uint8_t status) ;
extern void ATCmdGuardResponseWhenGuardStatusUnchange(void) ;
#if(Commu_Mode_Common != Commu_Mode)
extern uint8_t GetControllerErrorStatus(void) ;
#endif
#if(KeyLessStart == Anti_theftDevice_Function)
extern void ResponseKeyLessStart(uint8_t* para) ;
extern void ResponseKeyLessStatusChanged(void) ;
	#if MECHANICALLOCK_INTERNAL_COMPILE
extern void ResponseMechalMotorLock(uint8_t* para) ;
extern void ResponseDouSupportStatusAndErrorChanged(uint8_t* para)  ;
		#if DOUSUPPORT_INTERNAL_COMPILE
extern void ResponseDouSupport(uint8_t* para) ;
		#endif
	#endif
#endif

#if SEATLOCK_COMPILE
//extern void ResponseSeatLockAction(void) ;
extern void SeatLockNotifyWhenStatusChanged(void) ;
#endif

#if TEST_EXTFLASH_COMPILE
extern void CheckAndResponseExtFlashResult(uint8_t result);
#endif

extern void ClearHeartBeatTimer(void) ;
extern void ResponseResetBle(uint8_t* para);

#if HEARTBEAT_COMPILE
extern void HeartBeatSendEveryFixedInterval(void) ;
extern void CheckBLEHeartBeat(void) ;
#endif
#endif