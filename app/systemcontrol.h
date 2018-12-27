#ifndef __SYSTEMCONTROL_H
#define __SYSTEMCONTROL_H

#include <ti/drivers/pin/PINCC26xx.h>
#include "defines.h"
#include "hal_board.h"


/*typedef*/
/*typedef enum
{
	SIF_IDLE = 0 ,
	SIF_Sendding ,
}SIFStatus_TypeDef ;*/

typedef enum
{
  BLE_Unconnect = 0 ,
  BLE_ConnectButNoneInteractive ,
  BLE_ConnectWithInteractive ,
} BLEConnectStatus_TypeDef;

#if BUZZER_COMPILE
typedef	struct
{
	uint8_t timer_addcounter_per2ms ;
	uint8_t counter_targetringnum_mul2 ;
	uint8_t counter_nowringnum_mul2 ;
}BUZZER_TypeDef;
#endif

typedef enum
{
	Button_Released = 0,
	Button_Press_Filtering ,	
	Button_Pressd ,
	Button_Release_Filtering,	
}ButtonPressStatus_TypeDef;
	
//typedef void (*buttonpressfunc)(ButtonPressDetect_TypeDef) ;	

typedef struct
{
	PIN_Id pin_id ;
	ButtonPressStatus_TypeDef status ;
    ButtonPressStatus_TypeDef last_status ;
	//LevelStatus_TypeDef last_level ;
	uint8_t timer_per50ms ;
	const uint8_t fix_timer_press_per50ms ;
	const uint8_t fix_timer_release_per50ms ;	
}ButtonPressDetect_TypeDef ;


typedef		void (*LoadSIFOutByteFunc_TypeDef)(void);	
#define		SIFOUT_BYTENUM_MAX			10	//except T2C 
typedef enum
{
	Data_LowLevelFirst = 0 ,
	Data_HighLevelFirst 
}SIFOUTMODE_TypeDef ;

typedef struct
{
	uint8_t id ;
	PIN_Id pin_id ;
	const SIFOUTMODE_TypeDef mode ;
	const uint16_t fixtimer_head_lowlevel_per100us ;
	const uint8_t fixtimer_head_highlevel_per100us ;
	const uint8_t fixtimer_data_longlevel_per100us ;
	const uint8_t fixtimer_data_shortlevel_per100us ;
	const uint8_t fixcounter_byte ;
	const uint8_t fixcounter_cycle ;
	SIFStatus_TypeDef status ;
	uint16_t timer_lowlevel_per100us ;
	uint8_t timer_highlevel_per100us ;
	uint8_t index_byte ;
	uint8_t index_bit ;
	uint8_t index_cycle ;							//if equel 0xff ,cycle forever
	uint8_t databuff[SIFOUT_BYTENUM_MAX] ;
	uint8_t data_send ;
	LoadSIFOutByteFunc_TypeDef LoadSIFOutByteFunc ;
}SIFOUT_TypeDef ;

typedef enum tagState_ResetMCU_ETypeDef
{
	State_ResetMCU_None= 0 ,
	State_ResetMCU_WaitForMotorStop ,
	State_ResetMCU_SaveFlash,
	State_ResetMCU_Aciton
}State_ResetMCU_ETypeDef;



#define 	Ring1_Mul2							(1 * 2 )
#define		Ring2_Mul2							(2 * 2 ) 
#define		Ring5_Mul2							(5 * 2 )
#define		Timer_AddCounter_Per2ms				75			//75*2ms =150ms

#define		SystemReset()						HAL_SYSTEM_RESET()

/*extern variable */
extern  PIN_Handle hGpioPin ;
extern  PIN_State pinStateGPIO ;
extern bool flag_start_resetmcu_timer ;
extern uint8_t timer_resetmcu_remain_per2ms ;
#if BUZZER_COMPILE
extern BUZZER_TypeDef 	Buzzer ;
#endif

/*extern function*/
extern void Delay(void) ;
extern PIN_Handle GetGpioPinHandle(void) ;
extern LevelStatus_TypeDef GetPinLevel(PIN_Id pinId) ;
extern void SetPinLevel(PIN_Id pinId,LevelStatus_TypeDef level) ;
extern void BlueLEDControl(void) ;

#if BUZZER_COMPILE
extern bool IsBuzzerRingEnable(void) ;
extern void BuzzerRingWhenEnable(void) ;
#endif

extern BLEConnectStatus_TypeDef GetBLEConnectStatus(void) ;
extern void SetBLEConnectStatus(BLEConnectStatus_TypeDef status);
extern bool IsBLEConnectStatusInteractive(void) ;
extern void ChangeBLEConnectStatus(BLEConnectStatus_TypeDef	status) ;
extern bool IsButtonEventIfPressedLowLevelAndRealseEffect(ButtonPressDetect_TypeDef *buttonpress) ;
extern bool IsButtonEventIfPressedLowLevelAndPressEffect(ButtonPressDetect_TypeDef *buttonpress) ;
extern void SetSIFSendCyclesValue(SIFOUT_TypeDef * pSIFOut) ;
extern void SIFSend_Service(SIFOUT_TypeDef * pSIFOut) ;

extern bool IsVariableTimerZero(uint8_t * ptimer_8bit);
extern void VariableTimerDecrease(uint8_t * ptimer_8bit) ;
extern bool IsVariableTimerDecreaseToZero(uint8_t *	ptimer_8bit);
extern bool IsVariableZeroIfNotDecrease(uint8_t  * pValue_8bit) ;

extern void SetFlagResetMCUWithCheckSpeedAndSaveFlash(void) ;
extern void ResetWithCheckSpeedAndSaveFlashMechanism(void);

extern bool IsInAbsoluteValueRang(uint8_t targetvalue ,uint8_t standartvalue,uint8_t range) ;
extern UnionSumWithH8L8_TypeDef SwapH8L8(UnionSumWithH8L8_TypeDef In) ;
extern bool IsAllBytesZero(const uint8_t *p ,uint8_t strlen) ;


#endif

