#ifndef __KEYLESS_H
#define __KEYLESS_H

/*********************Annotate********************************/
/*该部分用来控制免钥匙启动、断开及相关的执行部件失败的处理以及机械锁的控制部分 */
/*********************typedef************************/
typedef enum
{
  Status_PowerOff = 0 ,
  Status_PowerOn, 
  Status_PowerOffAction ,
  Status_PowerOnAction ,
  Status_PowerOffFailure ,
  Status_PowerOnFailure ,
  Status_OCP ,
  //Status_PowerOnNotAllowed,
}StatusKeyLess_TypeDef;

typedef enum
{
	Status_PowerOnAllowed = 0 ,
	Status_PowerOnNotAllowed ,
	Status_48SWAlreadyPowerOn ,
}StatusPowerOnAllow_TypeDef ;



typedef struct
{
	StatusKeyLess_TypeDef status ;
	uint8_t counter_retry_whenfailure ;
	uint8_t counter_OCP_warning	;
	
	//uint16_t avr_adc ;
	uint8_t timer_48VSWActionOuttime_per2ms ;
	bool	flag_responsestatus ;

#if DOUSUPPORT_INTERNAL_COMPILE
	bool	flag_enableDSraise_afterpoweroff ;
#endif
	
}KeyLess_TypeDef;

typedef struct
{
	bool flag ;
	uint8_t timer_LDturnoff_per50ms ;
}LDPower_TypeDef;








/*****************************************************/
/**********************define************************/
#define	Counter_Retry_WhenPowerActionFailure			3
#define	Counter_OCP_Warning								3

#define	Timer_48VSWPowerOnOuttime_Per2ms			50	//100ms		实测21~30左右
#define	Timer_48VSWPowerOffOuttime_Per2ms			200	//400ms		实测110~130左右
#define	ADC_PowerOff_Thresholds_A					8.0		//8V
#define	ADC_PowerOff_Thresholds_Differ_A			2.0	//1V

#define	Timer_LDTurnOff_Per50ms						70	//3.5s		//控制器醒3s 这里放点余量为3.5s		

#if defined(STM32) 
#define	ADC_PowerOff_Thresholds_D					(uint16_t)(ADC_PowerOff_Thresholds_A * LVAnalog2Digital)
#define	ADC_PowerOff_Thresholds_Differ_D			(uint16_t)(ADC_PowerOff_Thresholds_Differ_A * LVAnalog2Digital)
#endif

#if defined(CC26XX)
#define	ADC_PowerOff_Thresholds_D					(uint16_t)(ADC_PowerOff_Thresholds_A * R9*1000/ADC_RSum)
#define	ADC_PowerOff_Thresholds_Differ_D			(uint16_t)(ADC_PowerOff_Thresholds_Differ_A * R9*1000/ADC_RSum)
#endif

#define Counter_LV_48VSW_Sample_Max					(8 + 2 )
#define Counter_LV_48VSW_Sample_Shift				3	//移位
	




#define	Counter_Brake_Filter						2


/*************************************************/
/*******************Variable**********************/
extern KeyLess_TypeDef	KeyLess ;
extern LDPower_TypeDef	LDPower ;



/*************************************************/
/*******************function**********************/
#if(KeyLessStart == Anti_theftDevice_Function)
extern  bool IsACCPoweredOff(void) ;
extern  bool IsACCPoweredOn(void) ;
void KeyLessPowerOnAction(void) ;
void KeyLessPowerOffAction(void) ;
void KeyLessPowerOn(void) ;
void KeyLessPowerOff(void) ;
void LV48VSWADSample(void) ;
void CheckKeyLessStartActionResult(void) ;
StatusPowerOnAllow_TypeDef IsFreePowerOnAllowWhenReceiveCmd(void);
void DetectLD(void) ;

#endif
























#endif