#include "defines.h"
#include "systemcontrol.h"
#include "KeyLess.h"
#include "ATControl.h"
#include "OLC.h"
#include "DouSupport.h"
#include "BatteryPowerCal.h"
#include "Guard.h"
#include "debug.h"

#include "VoicePacket.h"

/*************************************************/
/*******************Variable**********************/
//static uint16_t adc_lowest = 0;
//static uint16_t adc_highest =0 ;
//static uint16_t adc_temp = 0 ;

static ADCSample_TypeDef ADCSample_ACC =
{
	.id = 2 ,
	.value_single_sample = 0 ,
	.sum_temp = 0 ,
	.counter_sample = 16 ,
	.average_value = 0 ,
	.fix_counter_sample = 16 ,	//for 256 timers
	.fix_counter_shift = 4 
};
//static uint64_t 	sum_adc_SW48V = 0 ;
//static uint8_t 		counter_sample_SW48V = 0 ;
//static uint16_t 	avr_adc_ACC = 0 ;




KeyLess_TypeDef		KeyLess ;
LDPower_TypeDef		LDPower ;




/***********************************************/
/*****************function********************/
#if(KeyLessStart == Anti_theftDevice_Function)
/*********************************************************************
* @brief  
 *
 * @param   a0, a1 - not used.
 *
 * @return  None.
 */
bool IsACCPoweredOff(void)
{	
	//if(avr_adc_ACC < ADC_PowerOff_Thresholds_D)
	if(ADCSample_ACC.average_value < ADC_PowerOff_Thresholds_D)
		return true ;
	return false ;
 }

 /*********************************************************************
 * @brief  
  *
  * @param	 a0, a1 - not used.
  *
  * @return  None.
  */
bool IsACCPoweredOn(void)
 {	 
	// if(avr_adc_ACC > (LV.avr_adc - ADC_PowerOff_Thresholds_Differ_D))
	// if(avr_adc_ACC > (GetLVValueFlashSaveDetect() - ADC_PowerOff_Thresholds_Differ_D))
	if(ADCSample_ACC.average_value  > (GetLVValueFlashSaveDetect() - ADC_PowerOff_Thresholds_Differ_D))
		 return true ;
	 return false ;
  }


/*******************************************************************************
* 函数描述    	:
* 输入参数      : None
* 输出结果      : None
* 返回值        : None
*******************************************************************************/
void KeyLessPowerOnAction(void)
{
	#if(BLE_H == PCB_VERSION )
	GPIO_SetBits(Port_XL,Pin_XL);		//输出高电平 电源输出的PMOSFET导通
	#elif((BLE_J ==PCB_VERSION)||(BLE_M == PCB_VERSION)||(BLE_P == PCB_VERSION) || BLE_R == PCB_VERSION ||BLE_Q == PCB_VERSION)
	GPIO_SetBits(Port_CPMOS,Pin_CPMOS);		//输出高电平 电源输出的PMOSFET导通
	#elif((DLCC01 == PCB_VERSION)||(DLCC04 == PCB_VERSION ) ||(ZBJ02A == PCB_VERSION )\
	||(ZBJ02B == PCB_VERSION )||(ZBJ05A ==PCB_VERSION )||(RGZ_B == PCB_VERSION )||(ZBJ02D ==PCB_VERSION ))
	SetPinLevel(Pin_SW48V, High_Level) ;
	#else
		#error "缺少免钥匙的定义"
	#endif
	KeyLess.timer_48VSWActionOuttime_per2ms = 0 ;
}

/*******************************************************************************
* 函数描述    	:
* 输入参数      : None
* 输出结果      : None
* 返回值        : None
*******************************************************************************/
void KeyLessPowerOffAction(void)
{
	#if(BLE_H == PCB_VERSION )
	GPIO_ResetBits(Port_XL,Pin_XL);		//输出低电平 电源输出的PMOSFET断开
	#elif((BLE_J ==PCB_VERSION)||(BLE_M == PCB_VERSION)||(BLE_P == PCB_VERSION) || BLE_R == PCB_VERSION ||BLE_Q == PCB_VERSION)
	GPIO_ResetBits(Port_CPMOS,Pin_CPMOS);		//输出低电平 电源输出的PMOSFET断开
	#elif((DLCC01 ==PCB_VERSION)||(DLCC04 == PCB_VERSION )||(ZBJ02A == PCB_VERSION )||(ZBJ05A ==PCB_VERSION )\
	||(ZBJ02B == PCB_VERSION )||(RGZ_B == PCB_VERSION )|(ZBJ02D ==PCB_VERSION ))
	SetPinLevel(Pin_SW48V, Low_Level) ;
	#else
		#error "缺少免钥匙的定义"
	#endif
	KeyLess.timer_48VSWActionOuttime_per2ms = 0 ;
}

/*******************************************************************************
* 函数描述    	:
* 输入参数      : None
* 输出结果      : None
* 返回值        : None
*******************************************************************************/
void KeyLessPowerOn(void)
{
	if(Status_PowerOnAllowed == IsFreePowerOnAllowWhenReceiveCmd())
	{
		KeyLess.status = Status_PowerOnAction ;
		KeyLessPowerOnAction();	
		KeyLess.counter_retry_whenfailure = 0 ;
		
		/*//Suppose BLEMdlCnt when PowerOn
		BLEMdlCnt.timer_connect2controller_100ms  = 0 ;	
		BLEMdlCnt.status_connect2controller = true ;
		*/
		
	}
	else if(Status_48SWAlreadyPowerOn == IsFreePowerOnAllowWhenReceiveCmd())
	{
		KeyLess.status = Status_PowerOn ;
		KeyLess.flag_responsestatus = true ;
		KeyLess.counter_retry_whenfailure = 0 ;
	}
	else
		;
}

/*******************************************************************************
* 函数描述    	:
* 输入参数      : None
* 输出结果      : None
* 返回值        : None
*******************************************************************************/
void KeyLessPowerOff(void)
{
	KeyLess.status = Status_PowerOffAction ;
	KeyLessPowerOffAction();	
	KeyLess.counter_retry_whenfailure = 0 ;
}

/*******************************************************************************
* 函数名  		: 
* 函数描述    	: remove the lowest and highest value and then average
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
void LV48VSWADSample(void)
{ 
	ADCSample_ACC.value_single_sample =  GetLVRawValue(Board_ADCSW48V);
/*	
	sum_adc_SW48V += GetLVRawValue(Board_ADCSW48V);
	counter_sample_SW48V ++ ;
	
	if(0 == counter_sample_SW48V)
	{
		avr_adc_ACC = (sum_adc_SW48V >> Counter_1Byte_Shift)/1000 ;
		sum_adc_SW48V = 0 ;
	}*/
	CalADCSampleAverageValue(&ADCSample_ACC) ;
	
		
	/*
	//get lowest and high adc
	//init when every first adc
	if(1 == KeyLess.counter_sample)
	{
		adc_lowest = adc_temp ;
		adc_highest = adc_temp ;
	}
	else
	{
		if(adc_temp < adc_lowest )
			adc_lowest = adc_temp ;
		if(adc_temp > adc_highest)
			adc_highest = adc_temp ;
	}
	

	if( KeyLess.counter_sample >= Counter_LV_48VSW_Sample_Max) 
	{  
		KeyLess.sum_temp_adc = KeyLess.sum_temp_adc - adc_lowest - adc_highest ;
		KeyLess.avr_adc = KeyLess.sum_temp_adc >> Counter_LV_48VSW_Sample_Shift ;
		KeyLess.sum_temp_adc = 0 ;
		KeyLess.counter_sample = 0 ;
	} */ 
}

/*******************************************************************************
* 函数描述    	: 循环执行
* 输入参数      : None
* 输出结果      : None
* 返回值        : None
*******************************************************************************/
void CheckKeyLessStartActionResult(void)
{
	if(Status_PowerOffAction == KeyLess.status)
	{
		if(IsACCPoweredOff())
		{
			KeyLess.status = Status_PowerOff ;
#if DOUSUPPORT_INTERNAL_COMPILE		
			if(true == KeyLess.flag_enableDSraise_afterpoweroff)
			{
				KeyLess.flag_enableDSraise_afterpoweroff = false ;
	#if(FuzhouPeipeng_DouSupport_ManufactID == DouSupport_Manufacturer_ID)
				DouSupport.cmdfrom = DouSupportActionCmdFrom_FlowControl;
	#endif
				DouSupportRaiseAction();
			}
#endif		
			
#if KEYLESS_RESPONSE_COMPILE			
			KeyLess.flag_responsestatus = true ;
#endif
		}	
		else 
		{	
			if(KeyLess.timer_48VSWActionOuttime_per2ms >= Timer_48VSWPowerOffOuttime_Per2ms)
			{	
				KeyLess.counter_retry_whenfailure ++ ;
				if(KeyLess.counter_retry_whenfailure <= Counter_Retry_WhenPowerActionFailure)
					KeyLessPowerOffAction();	
				else
				{
					if(Status_PowerOffFailure != KeyLess.status)
					{
						KeyLess.status = Status_PowerOffFailure ;
#if DOUSUPPORT_INTERNAL_COMPILE				
						if(true == KeyLess.flag_enableDSraise_afterpoweroff)
							KeyLess.flag_enableDSraise_afterpoweroff = false ;
#endif					
		
#if KEYLESS_RESPONSE_COMPILE							
						KeyLess.flag_responsestatus = true ;
#endif						
					}	
				}
			}
			else
				;
		}
	}
	else if(Status_PowerOnAction == KeyLess.status)	
	{
			if(IsACCPoweredOn())
			{
				
				KeyLess.status = Status_PowerOn ;
#if KEYLESS_RESPONSE_COMPILE					
				KeyLess.flag_responsestatus = true ;
#endif				
			}
			else
			{
				if(KeyLess.timer_48VSWActionOuttime_per2ms >= Timer_48VSWPowerOnOuttime_Per2ms)
				{
					KeyLess.counter_retry_whenfailure ++ ;
					if(KeyLess.counter_retry_whenfailure <= Counter_Retry_WhenPowerActionFailure)
						KeyLessPowerOnAction();	
					else
					{
						if(Status_PowerOnFailure != KeyLess.status)
						{
							KeyLess.status = Status_PowerOnFailure  ;	
#if KEYLESS_RESPONSE_COMPILE								
							KeyLess.flag_responsestatus = true ;
#endif							
						}
					}
				}
				else
					;
			}
	}
	else if(Status_OCP == KeyLess.status)
	{
		if(KeyLess.counter_OCP_warning > 0)
		{
			KeyLess.flag_responsestatus = true ;
			KeyLess.counter_OCP_warning -- ;
		}
		else
			;
	}
	else if(Status_PowerOff == KeyLess.status)
		;
	else
		;

#if KEYLESS_RESPONSE_COMPILE	
	if(true == KeyLess.flag_responsestatus )
	{
		if(IsBLEConnectStatusInteractive())
			ResponseKeyLessStatusChanged();
		KeyLess.flag_responsestatus = false ;
	}
#endif	
}

/*******************************************************************************
* 函数描述    	:
* 输入参数      : None
* 输出结果      : None
* 返回值        : None
*******************************************************************************/
StatusPowerOnAllow_TypeDef IsFreePowerOnAllowWhenReceiveCmd(void)
{
	Guard.status_guardreason = CheckGuardReason();
	if(DeadlineBecomeDue == Guard.status_guardreason)
		return Status_PowerOnNotAllowed ;
#if GPS_COMPILE 	
	if(ForceGPRSLock == Guard.status_guardreason)
		return Status_PowerOnNotAllowed ;
#endif	
	if(IsACCPoweredOn())
		return Status_48SWAlreadyPowerOn ;
	else
		return Status_PowerOnAllowed;
}




/*******************************************************************************
* 函数描述    	:LD平时为高电平 有轮动信号时变成低电平
* 输入参数      : None
* 输出结果      : None
* 返回值        : None
*******************************************************************************/
void DetectLD(void)
{
	VariableTimerDecrease(&(LDPower.timer_LDturnoff_per50ms));
	
	//if there is no Guard.status_autoguard judgement , the motor can run after unlock guard and reset BLE module when push the motor 
	if((Status_PowerOff == KeyLess.status) && (Status_Auto == Guard.status_autoguard))
	{
		//if(Port_LD->IDR & Pin_LD)
		if(High_Level == GetPinLevel(Pin_LD))
		{
			if(true  == LDPower.flag)
			{
				if(0 == LDPower.timer_LDturnoff_per50ms)
				{
					LDPower.flag = false ;
					//ResetGuard2Controller();
					KeyLessPowerOffAction();
				}
				else
					;
			}
		}
		else
		{
			if(false  == LDPower.flag)
			{
				LDPower.flag = true ;
				//SetGuard2Controller();
				KeyLessPowerOnAction();
			}
			LDPower.timer_LDturnoff_per50ms = Timer_LDTurnOff_Per50ms ;
		}
	}
}




#endif
