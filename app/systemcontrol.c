//#include "defines.h"
#include "systemcontrol.h"
#include "simple_peripheral.h"
#include "FlashSNV.h"
#include "MileageCal.h"




#define BUZZER(a)	if(a)	\
				GPIO_SetBits(Port_BUZZER,Pin_BUZZER);	\
					else	\
				GPIO_ResetBits(Port_BUZZER,Pin_BUZZER)
					

/*variable */
 PIN_Handle hGpioPin = NULL;
 PIN_State pinStateGPIO ;
static BLEConnectStatus_TypeDef bleconnectstatus = BLE_Unconnect  ;

static bool flag_reset_with_checkspeed_saveflash = false;
static State_ResetMCU_ETypeDef state_resetmcu = State_ResetMCU_None ;

bool flag_start_resetmcu_timer = false ;
uint8_t timer_resetmcu_remain_per2ms = 0XFF ;
 
#if BUZZER_COMPILE
BUZZER_TypeDef 	Buzzer ;
#endif



/*function*/
static void LoadSIFOutDataAndInitial(SIFOUT_TypeDef * pSIFOut) ;
static void SendSIFOutBit(SIFOUT_TypeDef * pSIFOut) ;



/*********************************************************************
* @brief  
 *
 * @param   a0, a1 - not used.
 *
 * @return  None.
 */
void Delay(void)
{
  for(uint8_t counter1_delay = 0 ;counter1_delay < UINT8_MAX ;counter1_delay ++)
    NOP ;
  
}
/*********************************************************************
 * @brief   Allows a PIN client (driver or application) to allocate a set of pins, thus ensuring that they cannot be reconfigured/controlled by anyone else. 
 *			The pins are identified by and reconfigured according to the PIN_Config entries in aPinList.
 *
 * @param   none
 * 			gpioPinState:	Pointer to a PIN_State object that will hold the state for this IO client. The object must be in persistent memory
 *
 * @return  A handle for further PIN driver calls or NULL if an error occurred
 */   
PIN_Handle GetGpioPinHandle(void)
{
	//if(!hGpioPin)			//hGpioPin is not a local variable
	//	hGpioPin = PIN_open(&pinStateGPIO, BoardGpioInitTable);			 
	return hGpioPin ;																	
}

/*********************************************************************
* @brief  
 *
 * @param   a0, a1 - not used.
 *
 * @return  None.
 */
LevelStatus_TypeDef GetPinLevel(PIN_Id pinId)
{
#if defined( CC26XX )	
	if(PIN_getInputValue(pinId))
#endif		
		return  High_Level ;
	else
		return Low_Level ;
}

/*********************************************************************
* @brief  
 *
 * @param   a0, a1 - not used.
 *
 * @return  None.
 */
void SetPinLevel(PIN_Id pinId,LevelStatus_TypeDef level)
{
#if defined( CC26XX )
	if(High_Level == level)
		PIN_setOutputValue(GetGpioPinHandle() ,pinId, 1 ) ;
	else
		PIN_setOutputValue(GetGpioPinHandle() ,pinId, 0 ) ;
#endif
}

/*********************************************************************
* @brief  
 *
 * @param   a0, a1 - not used.
 *
 * @return  None.
 */
void BlueLEDControl(void)
{
	/************************LEDIO和ICDIO冲突******************************/
	#ifndef TI_DRIVERS_LCD_MANGO 
	if(BLE_Unconnect ==  bleconnectstatus)
		PIN_setOutputValue(GetGpioPinHandle() ,Board_BLED ,0) ;
	else if(BLE_ConnectButNoneInteractive == bleconnectstatus)
	{
		if(PIN_getOutputValue(Board_BLED))
			PIN_setOutputValue(GetGpioPinHandle() ,Board_BLED ,0) ;
		else
			PIN_setOutputValue(GetGpioPinHandle() ,Board_BLED ,1) ;

	}
	else
		PIN_setOutputValue(GetGpioPinHandle() ,Board_BLED ,1) ;
   #endif
}

#if BUZZER_COMPILE
/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
bool IsBuzzerRingEnable(void)
{
    if(Buzzer.counter_targetringnum_mul2)
        return true ;
    else
        return false ;
}

/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
void BuzzerRingWhenEnable(void)
{
    if(true == IsBuzzerRingEnable())
    {
        if(Buzzer.timer_addcounter_per2ms >= Timer_AddCounter_Per2ms)	
        {
            Buzzer.timer_addcounter_per2ms = 0 ;
            Buzzer.counter_nowringnum_mul2 ++ ;
            if(Buzzer.counter_nowringnum_mul2 < Buzzer.counter_targetringnum_mul2)
            {
                if(Buzzer.counter_nowringnum_mul2 % 2)
                    SetPinLevel(Pin_Buzzer ,High_Level) ;
					
                else
                    SetPinLevel(Pin_Buzzer ,Low_Level) ;
            }
            else
            {
                SetPinLevel(Pin_Buzzer ,Low_Level) ;
                Buzzer.counter_nowringnum_mul2 = 0 ;
                Buzzer.counter_targetringnum_mul2 = 0 ;
            }
        }
        else 
            ;
    }
    else
        ;	
}
#endif

/*********************************************************************
* @brief  
 *
 * @param   a0, a1 - not used.
 *
 * @return  None.
 */
BLEConnectStatus_TypeDef GetBLEConnectStatus(void)
{
	return bleconnectstatus ;
}

/*********************************************************************
* @brief  
 *
 * @param   a0, a1 - not used.
 *
 * @return  None.
 */
void SetBLEConnectStatus(BLEConnectStatus_TypeDef status)
{
	bleconnectstatus = status;
}


/*********************************************************************
* @brief  
 *
 * @param   a0, a1 - not used.
 *
 * @return  None.
 */
bool IsBLEConnectStatusInteractive(void)
{
	if(BLE_ConnectWithInteractive == bleconnectstatus)
		return true ;
	return false ;
}



/*********************************************************************
* @brief  
 *
 * @param   a0, a1 - not used.
 *
 * @return  None.
 */
void ChangeBLEConnectStatus(BLEConnectStatus_TypeDef	status)
{
	bleconnectstatus = status ;
	if(BLE_Unconnect == status)
		InitTimerUninteractiveTimeroutPer1s() ;
}

/*********************************************************************
* @brief  
 *
 * @param   a0, a1 - not used.
 *
 * @return  None.
 */
bool IsButtonEventIfPressedLowLevelAndRealseEffect(ButtonPressDetect_TypeDef *buttonpress)
{
	if(buttonpress->timer_per50ms)
		buttonpress->timer_per50ms -- ;
	
	if(Low_Level == GetPinLevel(buttonpress->pin_id))	// buttonpress pressed
	{
		if(Button_Released == buttonpress->status)
		{
			buttonpress->status = Button_Press_Filtering ;
			buttonpress->timer_per50ms = buttonpress->fix_timer_press_per50ms ;
		}
		else if(Button_Press_Filtering == buttonpress->status)
		{
			if(!buttonpress->timer_per50ms)	//equal 0
				buttonpress->status = Button_Pressd ;

		}
		else if(Button_Release_Filtering == buttonpress->status)
		{
			buttonpress->status = Button_Pressd ;
			buttonpress->timer_per50ms = buttonpress->fix_timer_release_per50ms;
		}
		else //Button_Pressd
			;
	}
	else	//high level
	{
		if(Button_Pressd == buttonpress->status)
		{
			buttonpress->status = Button_Release_Filtering ;
			buttonpress->timer_per50ms = buttonpress->fix_timer_release_per50ms ;
		}
		else if(Button_Release_Filtering == buttonpress->status)
		{
			if(!buttonpress->timer_per50ms)	//equal 0
			{
				buttonpress->status = Button_Released ;
				return true ;
			}

		}
		else if(Button_Press_Filtering == buttonpress->status)
		{
			buttonpress->status = Button_Released ;
			buttonpress->timer_per50ms = buttonpress->fix_timer_press_per50ms;
		}
		else //Button_Release
			;
	}		
	return false ;
 }

/*********************************************************************
* @brief  
 *
 * @param   a0, a1 - not used.
 *
 * @return  None.
 */
bool IsButtonEventIfPressedLowLevelAndPressEffect(ButtonPressDetect_TypeDef *buttonpress)
{
	VariableTimerDecrease(&(buttonpress->timer_per50ms)) ;
	
	if(Low_Level == GetPinLevel(buttonpress->pin_id))	// buttonpress pressed
	{
		if(Button_Released == buttonpress->status)
		{
			buttonpress->status = Button_Press_Filtering ;
			buttonpress->timer_per50ms = buttonpress->fix_timer_press_per50ms ;
		}
		else if(Button_Press_Filtering == buttonpress->status)
		{
			if(!buttonpress->timer_per50ms)	//equal 0
			{
				buttonpress->status = Button_Pressd ;
				return true ;
			}

		}
		else if(Button_Release_Filtering == buttonpress->status)
		{
			buttonpress->status = Button_Pressd ;
			buttonpress->timer_per50ms = buttonpress->fix_timer_release_per50ms;
		}
		else //Button_Pressd
			;
	}
	else	//high level
	{
		if(Button_Pressd == buttonpress->status)
		{
			buttonpress->status = Button_Release_Filtering ;
			buttonpress->timer_per50ms = buttonpress->fix_timer_release_per50ms ;
		}
		else if(Button_Release_Filtering == buttonpress->status)
		{
			if(!buttonpress->timer_per50ms)	//equal 0
			{
				buttonpress->status = Button_Released ;
				
			}

		}
		else if(Button_Press_Filtering == buttonpress->status)
		{
			buttonpress->status = Button_Released ;
			buttonpress->timer_per50ms = buttonpress->fix_timer_press_per50ms;
		}
		else //Button_Release
			;
	}		
	return false ;
}


/*********************************************************************
* @brief  
 *
 * @param   a0, a1 - not used.
 *
 * @return  None.
 */
 void SetSIFSendCyclesValue(SIFOUT_TypeDef * pSIFOut)
{
	pSIFOut->index_cycle = pSIFOut->fixcounter_cycle ;
 }


/*********************************************************************
* @brief  
 *
 * @param   a0, a1 - not used.
 *
 * @return  None.
 */
void SIFSend_Service(SIFOUT_TypeDef * pSIFOut)
{
	if(pSIFOut->index_cycle)
	{
		if(SIF_IDLE == pSIFOut->status)
			LoadSIFOutDataAndInitial(pSIFOut);
		else if(SIF_Sendding == pSIFOut->status)
			SendSIFOutBit(pSIFOut);
		else
			;
	}
	else
		;
}

/*********************************************************************
* @brief  
 *
 * @param   a0, a1 - not used.
 *
 * @return  None.
 */
static void LoadSIFOutDataAndInitial(SIFOUT_TypeDef * pSIFOut)
{
	//head is 2ms highlevel + long time lowlevel
	if(Data_HighLevelFirst == pSIFOut->mode)
		SetPinLevel(pSIFOut->pin_id, High_Level);
	//head is long time lowlevel + 2ms highlevel
	else	//Data_LowLevelFirst
		SetPinLevel(pSIFOut->pin_id, Low_Level) ;
	
	pSIFOut->LoadSIFOutByteFunc();

	pSIFOut->index_byte = 0 ;
	pSIFOut->index_bit = 1 ;		//startbyte only one bit
	pSIFOut->timer_lowlevel_per100us = pSIFOut->fixtimer_head_lowlevel_per100us ;
	pSIFOut->timer_highlevel_per100us = pSIFOut->fixtimer_head_highlevel_per100us  ;
	pSIFOut->status = SIF_Sendding ;
			
	
}

/*********************************************************************
* @brief  
 *
 * @param   a0, a1 - not used.
 *
 * @return  None.
 */
static void SendSIFOutBit(SIFOUT_TypeDef * pSIFOut)
{
	if(Data_HighLevelFirst == pSIFOut->mode)
	{
		if(pSIFOut->timer_highlevel_per100us)
		{
			SetPinLevel(pSIFOut->pin_id,High_Level);			//output highlevel
			pSIFOut->timer_highlevel_per100us -- ;
		}
		else		
		{
			if(pSIFOut->timer_lowlevel_per100us)	
			{
				SetPinLevel(pSIFOut->pin_id,Low_Level); 			//output lowlevel			
				pSIFOut->timer_lowlevel_per100us -- ;
			}
			else										//next bit
			{
				SetPinLevel(pSIFOut->pin_id,High_Level);			//output highlevel
				pSIFOut->index_bit -- ;
				if(0 == pSIFOut->index_bit)				//next byte
				{
					pSIFOut->index_bit = 8 ;
					pSIFOut->index_byte ++ ;
					if(pSIFOut->index_byte >= pSIFOut->fixcounter_byte )		//next frame data
					{
						pSIFOut->status = SIF_IDLE ;
						pSIFOut->index_cycle -- ;
						if(0 == pSIFOut->index_cycle)
							SetPinLevel(pSIFOut->pin_id,Low_Level); 	//output lowlevel
						else if(0XFE == pSIFOut->index_cycle)			//cycle forever
							pSIFOut->index_cycle = 0XFF ;
						else
							;
					}
					else
						pSIFOut->data_send = pSIFOut->databuff[pSIFOut->index_byte] ;	
				}

				if(pSIFOut->data_send & U8_BIT7)			//bit 1
				{
					pSIFOut->timer_lowlevel_per100us = pSIFOut->fixtimer_data_shortlevel_per100us ;	
					pSIFOut->timer_highlevel_per100us = pSIFOut->fixtimer_data_longlevel_per100us - 1;//the level is high already ,so the counter = realcounter -1 ;
				}
				else									//bit 0
				{
					pSIFOut->timer_lowlevel_per100us  = pSIFOut->fixtimer_data_longlevel_per100us ;
					pSIFOut->timer_highlevel_per100us = pSIFOut->fixtimer_data_shortlevel_per100us - 1 ;
				}	
				pSIFOut->data_send <<= 1 ;			//left shift
			}
		}
	}
	else	//Data_LowLevelFirst
	{
		if(pSIFOut->timer_lowlevel_per100us)
		{
			SetPinLevel(pSIFOut->pin_id,Low_Level); 			//output lowlevel	
			pSIFOut->timer_lowlevel_per100us -- ;
		}
		else		
		{
			if(pSIFOut->timer_highlevel_per100us)	
			{
				SetPinLevel(pSIFOut->pin_id,High_Level);			//output highlevel
				pSIFOut->timer_highlevel_per100us -- ;
			}
			else										//next bit
			{
				SetPinLevel(pSIFOut->pin_id,Low_Level); 			//output lowlevel				
				pSIFOut->index_bit -- ;
				if(0 == pSIFOut->index_bit)				//next byte
				{
					pSIFOut->index_bit = 8 ;
					pSIFOut->index_byte ++ ;
					if(pSIFOut->index_byte >= pSIFOut->fixcounter_byte )		//next frame data
					{
						pSIFOut->status = SIF_IDLE ;
						pSIFOut->index_cycle -- ;
						if(0 == pSIFOut->index_cycle)
							SetPinLevel(pSIFOut->pin_id,Low_Level); 	//output lowlevel
						else if(0XFE == pSIFOut->index_cycle)			//cycle forever
							pSIFOut->index_cycle = 0XFF ;
						else
							;
					}
					else
						pSIFOut->data_send = pSIFOut->databuff[pSIFOut->index_byte] ;	
				}
				
				if(pSIFOut->data_send & U8_BIT7)			//bit 1
				{
					pSIFOut->timer_lowlevel_per100us = pSIFOut->fixtimer_data_shortlevel_per100us  - 1 ;	//the level is low already ,so the counter = realcounter -1 ;
					pSIFOut->timer_highlevel_per100us = pSIFOut->fixtimer_data_longlevel_per100us ;
				}
				else									//bit 0
				{
					pSIFOut->timer_lowlevel_per100us= pSIFOut->fixtimer_data_longlevel_per100us - 1 ;
					pSIFOut->timer_highlevel_per100us =  pSIFOut->fixtimer_data_shortlevel_per100us ;
				}	
				pSIFOut->data_send <<= 1 ;			//left shift
			}
		}		
	}
}

/*********************************************************************
* @brief  
 *
 * @param   
 *
 * @return   
 */
bool IsVariableTimerZero(uint8_t * ptimer_8bit)
{
	if(*ptimer_8bit)
		return false ;
	return true ;
}


/*********************************************************************
* @brief  
 *
 * @param   
 *
 * @return   
 */
void VariableTimerDecrease(uint8_t * ptimer_8bit)
{
	if(*ptimer_8bit)
		(*ptimer_8bit)-- ;
}


/*********************************************************************
* @brief  
 *
 * @param   
 *
 * @return  if the timer become 0 from 1,return true 
 */
 bool IsVariableTimerDecreaseToZero(uint8_t  * ptimer_8bit)
{
	if(*ptimer_8bit)
	{	
		(*ptimer_8bit)-- ;
		if(0 == (*ptimer_8bit))
			return true ;
	}
	return false ;
	
 }

/*********************************************************************
* @brief  
 *
 * @param   
 *
 * @return  if the timer become 0 from 1,return true 
 */
 bool IsVariableZeroIfNotDecrease(uint8_t  * pValue_8bit)
{
	if(*pValue_8bit)
	{	
		(*pValue_8bit)-- ;
		return false ;
	}
	return true ;
 }

 /*********************************************************************
 * @brief	  
 *
 * @param		 a0, a1 - not used.
 *
 * @return	 None.
 */
 void SetFlagResetMCUWithCheckSpeedAndSaveFlash(void)
 {
	flag_reset_with_checkspeed_saveflash = true ;
 }

/*********************************************************************
* @brief	 
*
* @param		a0, a1 - not used.
*
* @return	None.
*/
void ResetWithCheckSpeedAndSaveFlashMechanism(void)
{
	if(State_ResetMCU_None == state_resetmcu)
	{
		if(flag_reset_with_checkspeed_saveflash)
		{
			flag_reset_with_checkspeed_saveflash = false ;
			state_resetmcu = State_ResetMCU_WaitForMotorStop;
		}
	}
	else if(State_ResetMCU_WaitForMotorStop == state_resetmcu)
	{
		if(IsHallSpeedNearlyStop())
		{
			SaveFlashWithResetMCU();
			state_resetmcu = State_ResetMCU_SaveFlash ;
		}
	}
	else if(State_ResetMCU_SaveFlash == state_resetmcu)
	{
		if(IsVariableTimerZero(&timer_resetmcu_remain_per2ms))
		{
			SystemReset();
			state_resetmcu = State_ResetMCU_Aciton ;
		}
		else
			VariableTimerDecrease(&timer_resetmcu_remain_per2ms) ;
	}
	else	//State_ResetMCU_Action
		;
}


/*********************************************************************
* @brief  
*
* @param	 a0, a1 - not used.
*
* @return  None.
*/
bool IsInAbsoluteValueRang(uint8_t targetvalue ,uint8_t standartvalue,uint8_t range)
{
	if(targetvalue > standartvalue)
	{
		if((targetvalue- standartvalue) < range)
			return true ;
		else
			return false ;
	}
	else
	{
		if((standartvalue - targetvalue) < range)
			return true ;
		else
			return false ;

	}
}


/*********************************************************************
* @brief  
 *
 * @param   a0, a1 - not used.
 *
 * @return  None.
 */
UnionSumWithH8L8_TypeDef SwapH8L8(UnionSumWithH8L8_TypeDef In)
{
	UnionSumWithH8L8_TypeDef temp ;
	temp.h8 = In.l8 ;
	temp.l8 = In.h8 ;
	return temp ;
}

/*********************************************************************
* @brief  
 *
 * @param   a0, a1 - not used.
 *
 * @return  None.
 */
bool IsAllBytesZero(const uint8_t *p ,uint8_t strlen) 
{
	for(uint8_t i = 0 ; i< strlen ; i++)
	{
		if(*(p + i))
			return false;
	}
	return true ;
}
