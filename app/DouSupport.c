#include "defines.h"
#include "simple_peripheral.h"
#include "systemcontrol.h"
#include "DouSupport.h"
#include "T2C.h"
#include "Guard.h"
#include "ATControl.h"

#if DOUSUPPORT_INTERNAL_COMPILE
typedef struct
{
	CmdDouSupportTrans_TypeDef	ecmd ;
	
#if(DongdaLanniao_DouSupport_ManufactID == DouSupport_Manufacturer_ID)	
	bool					flag_wirebreak ;
	bool					flag_wirebreakdetection_enable ;
	uint8_t					timer_wirebreakdetection_per500ms ;
#endif
}DouSupportTrans_TypeDef;	


/*************************************************/
/*******************Variable**********************/
DouSupportReceive_TypeDef	DSR ;
DouSupport_TypeDef			DouSupport ;
//DouSupportTransMessage_Type DouSupportTransMessage ; 

//static bool  IsDouSptRaiseInPlaceAndKeyLessPowerOffFromOtherStatus(void) ;
#if(DongdaLanniao_DouSupport_ManufactID == DouSupport_Manufacturer_ID)
//level
static LevelStatus_TypeDef levelstatus_temp_DSR = Low_Level ;
static LevelStatus_TypeDef levelstatus_DSR = Low_Level ;
static uint16_t timer_levelstatus_DSR = 0 ; 
static uint16_t timer_highlevel_DSR = 0 ;
static uint16_t timer_lowlevel_DSR = 0 ;	

//level to data
static uint8_t data8bit_temp_DSR = 0 ;

static uint8_t counter_displacement_DSR = 0 ;

static uint8_t	timer_wirebreakdetection_per500ms_DSR = 0 ;
static bool 	flag_wirebreak_DSR = true ;
#endif	

static DouSupportTrans_TypeDef	DST = 
{
	.ecmd = Cmd_DST_Nop 
#if(DongdaLanniao_DouSupport_ManufactID == DouSupport_Manufacturer_ID)
,
	.flag_wirebreak = false ,
	.flag_wirebreakdetection_enable = false ,
	.timer_wirebreakdetection_per500ms = 0
#endif	
} ;		



/**********************function**********************/
static void LoadDSTSIFByte(void) ;	
#if(DongdaLanniao_DouSupport_ManufactID == DouSupport_Manufacturer_ID)
static void EnableAndInitialDSTWireBreakVariable(void) ;
static void InitialDSRWireBreakVariable(void);

#endif	





SIFOUT_TypeDef DSTSIF =
{
	.id = 2 ,
	.pin_id = Pin_DST ,
	.mode = Data_LowLevelFirst ,
	.fixtimer_head_lowlevel_per100us = 1000 ,
	.fixtimer_head_highlevel_per100us = 20 ,
	.fixtimer_data_longlevel_per100us =  40 ,
	.fixtimer_data_shortlevel_per100us =  20 ,
	.fixcounter_byte = (1 + 5) ,
#if(FuzhouPeipeng_DouSupport_ManufactID == DouSupport_Manufacturer_ID)	
	.fixcounter_cycle = 3 ,
	.index_cycle = 0 ,
#elif(DongdaLanniao_DouSupport_ManufactID == DouSupport_Manufacturer_ID)
	.fixcounter_cycle = 0XFF ,
	.index_cycle = 0XFF ,
#else
#endif	
	.status  = SIF_IDLE,
	.timer_lowlevel_per100us = 0 ,
	.timer_highlevel_per100us = 0 ,
	.index_byte = 0 ,
	.index_bit = 0 ,
		
	//.databuff[SIFOUT_BYTENUM_MAX] ={0} ,
	.data_send = 0 ,
	.LoadSIFOutByteFunc = LoadDSTSIFByte 
};
	
/****************************************************/
/**********************function**********************/
/*******************************************************************************
* 
*/
static void LoadDSTSIFByte(void)
{
	DSTSIF.databuff[1] = DST_StartCode ;
	DSTSIF.databuff[2] = DST_ValidStrlen + 3 ;
	DSTSIF.databuff[3] = (uint8_t)DST.ecmd ;  

	//Get checkCode
	DSTSIF.databuff[4] = DSTSIF.databuff[1] ;
	for(uint8_t i = 2;i < 4 ;i++)
		DSTSIF.databuff[4] ^= DSTSIF.databuff[i] ;
	
	DSTSIF.databuff[5] = DST_EndCode ;

}

/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
void DouSupportRaiseAction(void)
{
	//DouSupport.cmd = Cmd_DouSupport_Raise ;
	
#if(FuzhouPeipeng_DouSupport_ManufactID == DouSupport_Manufacturer_ID)
	DouSupport.status = Status_RaisingAction ;
	DST.ecmd = Cmd_DST_Raise;
	SetSIFSendCyclesValue(&DSTSIF) ;
	DouSupport.timer_action_per50ms = Timer_DouSupportAction_Per50ms ;
#elif(DongdaLanniao_DouSupport_ManufactID == DouSupport_Manufacturer_ID)
	DouSupport.status = Status_RaiseCmdStart ;
	EnableAndInitialDSTWireBreakVariable();
#else
#endif
	
}

/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
void DouSupportRetractAction(void)
{
	//DouSupport.cmd = Cmd_DouSupport_Retract ;
#if(FuzhouPeipeng_DouSupport_ManufactID == DouSupport_Manufacturer_ID)
	DouSupport.status = Status_RetractingAction ;
	DST.ecmd = Cmd_DST_Retract ;
	SetSIFSendCyclesValue(&DSTSIF) ;
	DouSupport.timer_action_per50ms = Timer_DouSupportAction_Per50ms ;
#elif(DongdaLanniao_DouSupport_ManufactID == DouSupport_Manufacturer_ID)
	DouSupport.status = Status_RetractCmdStart ;
	EnableAndInitialDSTWireBreakVariable();
#else
#endif
}

/*******************************************************************************
* 函数描述    	:In-place status takes precedence over time-out status
* 输入参数      : None
* 输出结果      : None
* 返回值        : None
*******************************************************************************/
void CheckDouSupportActionResult(void)
{
#if(FuzhouPeipeng_DouSupport_ManufactID == DouSupport_Manufacturer_ID)	
	VariableTimerDecrease(&(DouSupport.timer_action_per50ms));
	if(Status_RetractingAction == DouSupport.status)
	{
		if(0 == DouSupport.timer_action_per50ms)
		{
			DouSupport.status = Status_RetractInPlace ;
			if(DouSupportActionCmdFrom_FlowControl == DouSupport.cmdfrom)
				 ResetGuard2Controller();
			//DouSupport.flag_BLEResponseStatusAndError = true ;
		}
		else
			;
	}
	else if(Status_RaisingAction == DouSupport.status)
	{
		if(0 == DouSupport.timer_action_per50ms)		
		{
			DouSupport.status = Status_RaiseInPlace ;
			//DouSupport.flag_BLEResponseStatusAndError = true ;
		}
		else
			;
	}
	else
		;
#elif(DongdaLanniao_DouSupport_ManufactID == DouSupport_Manufacturer_ID)	
	uint8_t error ;
	error = ReturnDouSupportError();
	if(error)
	{		
		if(error != DouSupport.error)
		{
			//if(0 == DouSupport.error)
				StopDouSupportActionWhenDouSupportError();
			DouSupport.error = error ;
			DouSupport.flag_BLEResponseStatusAndError = true ;
		}
	}
	else
	{
		//clear dousuppor.error
		if(DouSupport.error)
		{
			DouSupport.error = error ;
			DouSupport.flag_BLEResponseStatusAndError = true ;	// optional
		}

		if(Status_RetractCmdStart == DouSupport.status)
		{
			if(true == DSR.Data.flag_limitswith)	
			{
				if(1 == DSR.Data.flag_direction)	//retract in place
				{
					DouSupport.status = Status_RetractInPlace ;		//when in place ,get wrong cmd,no action
					DST.ecmd = Cmd_DST_Nop ;
					ResetGuard2Controller();
					//DouSupport.flag_BLEResponseStatusAndError = true ;
					DST.flag_wirebreakdetection_enable = false ;
				}
				else
				{
					DouSupport.status = Status_WaitForRetract ;		//right cmd
					DST.ecmd = Cmd_DST_Retract ;
				}
			}
			else
			{
				DouSupport.status = Status_RetractingAction;		// not in place
				DST.ecmd = Cmd_DST_Retract ;						//it can't detective wheather the dousupport moved
			}
		}
		else if(Status_RaiseCmdStart == DouSupport.status)
		{
			if(true == DSR.Data.flag_limitswith)	
			{
				if(0 == DSR.Data.flag_direction)	//raise in place
				{
					DouSupport.status = Status_RaiseInPlace;		//when in place ,get wrong cmd,no action
					DST.ecmd = Cmd_DST_Nop ;
					//DouSupport.flag_BLEResponseStatusAndError = true ;
					DST.flag_wirebreakdetection_enable = false ;
				}
				else
				{
					DouSupport.status = Status_WaitForRaise;		//right cmd
					DST.ecmd = Cmd_DST_Raise;
				}
			}
			else
			{
				DouSupport.status = Status_RaisingAction;		// not in place
				DST.ecmd = Cmd_DST_Raise;						//it can't detective wheather the dousupport moved
			}
		}
		else if(Status_WaitForRetract == DouSupport.status)
		{
			if(false == DSR.Data.flag_limitswith)
			{
				DouSupport.status = Status_RetractingAction;		// not in place
				DouSupport.flag_BLEResponseStatusAndError = true ;
			}
			else
				;

			//when drive dousupport timeoutflag will been cleared
			SetFlagWhenDouSupportTimeOut(Status_RetractOuttime) ;
		}
		else if(Status_WaitForRaise == DouSupport.status)
		{
			if(false == DSR.Data.flag_limitswith)
			{
				DouSupport.status = Status_RaisingAction;		// not in place
				DouSupport.flag_BLEResponseStatusAndError = true ;
			}
			else
				;
			
			//when drive dousupport timeoutflag will been cleared
			SetFlagWhenDouSupportTimeOut(Status_RaiseOuttime) ;
		}
		else if(Status_RetractingAction == DouSupport.status)
		{
			if(true == DSR.Data.flag_limitswith)
			{
				DouSupport.status = Status_RetractInPlace ;
				DST.ecmd = Cmd_DST_Nop ;
				ResetGuard2Controller();
				//DouSupport.flag_BLEResponseStatusAndError = true ;
			}
			else
				;
			SetFlagWhenDouSupportTimeOut(Status_RetractOuttime) ;
		}
		else if(Status_RaisingAction == DouSupport.status)
		{
			if(true == DSR.Data.flag_limitswith)
			{
				DouSupport.status = Status_RaiseInPlace;
				DST.ecmd = Cmd_DST_Nop ;
				//DouSupport.flag_BLEResponseStatusAndError = true ;
			}
			else
				;
			SetFlagWhenDouSupportTimeOut(Status_RaiseOuttime) ;
		}
		else if(Status_RetractInPlace == DouSupport.status)
		{
			;
		}
		else if(Status_RaiseInPlace == DouSupport.status)
		{
			;
		}
		else if(Status_RetractOuttime == DouSupport.status)
		{
			;
		}
		else if(Status_RaiseOuttime == DouSupport.status)
		{
			;
		}
		//else if(Status_NoneLSWPO == DouSupport.status)
			//;
		
	}


	//correct the DouSupport status
	if(Status_RetractInPlace == DouSupport.status)
	{
		if(true == DSR.Data.flag_limitswith)	
		{
			if(0 == DSR.Data.flag_direction)		//raise in place
			{
				DouSupport.status = Status_RaiseInPlace;
				//DouSupport.flag_BLEResponseStatusAndError = true ;
			}
		}
		else
			DouSupport.status = Status_RaiseOuttime ;
	}
	else if(Status_RaiseInPlace == DouSupport.status)
	{
		if(true == DSR.Data.flag_limitswith)
		{
			if(1 == DSR.Data.flag_direction)	//retract in place
			{
				DouSupport.status = Status_RetractInPlace ;
				DouSupport.flag_BLEResponseStatusAndError = true ;
			}
			
		}
		else
			DouSupport.status = Status_RetractOuttime;
	}
	else
		;
	
	if(DouSupport.status_byte != DSR.Data.statuscode )
	{
		//status_byte update
		DouSupport.status_byte = DSR.Data.statuscode ;
		DST.flag_wirebreakdetection_enable = false ;
		DST.flag_wirebreak = false ;
	}
	
	

#else
#endif

	
	if(true == DouSupport.flag_BLEResponseStatusAndError)
	{
		if(IsBLEConnectStatusInteractive())
		{
			ResponseDouSupportStatusAndErrorChanged(NULL);
			SimpleBLEPeripheral_enqueueAsySendMsg();
		}
		DouSupport.flag_BLEResponseStatusAndError= false ;
	}	
}



#if(DongdaLanniao_DouSupport_ManufactID == DouSupport_Manufacturer_ID)
/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
void ReadDSRLevel(void)
{
#if((BLE_J == PCB_VERSION)||(BLE_M == PCB_VERSION))		
	levelstatus_temp_DSR = Port_SCJD->IDR & Pin_SCJD ;
#elif(BLE_P == PCB_VERSION || BLE_R == PCB_VERSION ||BLE_Q == PCB_VERSION)
	levelstatus_temp_DSR = Port_DSR->IDR & Pin_DSR	;	
#elif((DLCC01 == PCB_VERSION) ||(DLCC04 == PCB_VERSION )||(DLCC05 == PCB_VERSION )\
	||(CC2650_LAUNCHXL ==PCB_VERSION )||(RGZ_B ==PCB_VERSION )||(ZBJ05A ==PCB_VERSION ))
	levelstatus_temp_DSR = GetPinLevel(Pin_DSR) ;
#else
	error " NO This PCB!"
#endif

	if(levelstatus_DSR != levelstatus_temp_DSR)
	{
		UpdataDSRLevelStatus(); 
		if(levelstatus_DSR == Low_Level)
		{
			if(timer_lowlevel_DSR >= DSR_Timer_Counter_Start_Bit_Per100us)
				DetectDSRStartBit();
			else			
				GetDSRLogicValue();
		}	  
	} 
	else
	{
		timer_levelstatus_DSR ++ ;
	}	
}

/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
void UpdataDSRLevelStatus(void)
{
	if(Low_Level == levelstatus_DSR)
		timer_lowlevel_DSR = timer_levelstatus_DSR	;	
	else
		timer_highlevel_DSR = timer_levelstatus_DSR	;
	timer_levelstatus_DSR = 0 ;
	levelstatus_DSR = levelstatus_temp_DSR ;
}

/*******************************************************************************
* 函数描述    	:
* 输入参数      :
* 输出结果      : None
* 返回值        : None
*******************************************************************************/
uint8_t ReturnDouSupportError(void)
{
	uint8_t error ;
	error = DSR.Data.errorcode ;
	
	if(true == flag_wirebreak_DSR)
		error |= U8_BIT1 ;
	return error ;
}

/*******************************************************************************
* 函数描述    	:
* 输入参数      :
* 输出结果      : None
* 返回值        : None
*******************************************************************************/
void StopDouSupportActionWhenDouSupportError(void)
{
	DST.ecmd = Cmd_DST_Nop ;
	if(Status_RetractCmdStart == DouSupport.status)
		DouSupport.status = Status_RetractOuttime;
	else if(Status_RaiseCmdStart == DouSupport.status)
		DouSupport.status = Status_RaiseOuttime;
	else
		;
	DST.flag_wirebreakdetection_enable = false ;
}

/*******************************************************************************
* 函数描述    	:
* 输入参数      ://when drive dousupport timeoutflag will been cleared
* 输出结果      : None
* 返回值        : None
*******************************************************************************/
void SetFlagWhenDouSupportTimeOut(StatusDouSupport_TypeDef	status)
{
	if(0 == (DouSupport.status_byte & U8_BIT5)) 	//time out flag
	{
		if(DSR.Data.flag_timeout)
		{
			DouSupport.status = status ;
			DST.ecmd = Cmd_DST_Nop ;
			DouSupport.flag_BLEResponseStatusAndError = true ;
		}
	}
}

/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
void GetDSRLogicValue(void)
{
	data8bit_temp_DSR <<= 1 ;
	if(timer_highlevel_DSR > timer_lowlevel_DSR )
	{  
		if((timer_highlevel_DSR- timer_lowlevel_DSR) > 5)	  
			data8bit_temp_DSR ++ ;
		else
			DSR.Flag_code_receive_wrong = true ;	  
	}
	else
	{	
		if((timer_lowlevel_DSR-timer_highlevel_DSR) > 5)
			;
		else
			DSR.Flag_code_receive_wrong = true ;	
	} 
	counter_displacement_DSR ++ ;
	if(0 == counter_displacement_DSR % OneByteBitNum)
		Write8bitToDSRCodeTemp(); 
}

/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
void Write8bitToDSRCodeTemp(void)
{
	DSR.DataBuff.Data[(counter_displacement_DSR/OneByteBitNum)-1] = data8bit_temp_DSR ;
	WriteToDSRCode();	
	data8bit_temp_DSR = 0 ;
}	  

/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
void WriteToDSRCode(void)
{
	if(counter_displacement_DSR >= (DSR_Strlen * OneByteBitNum))
	{
		if(true == IsDSRDeviceCodeCompliant() && true == IsDSRSumCompliant()\
		&& true == IsDSRStrlenCodeCompliant() && true == IsDSREndCodeCompliant())
		{
			DSR.Data = DSR.DataBuff ;
			InitialDSRWireBreakVariable();
			//DSR.timer_wirebreakdetection_per100ms = 0 ;
			//DSR.flag_wirebreak = false ;
			//DouSupport.error = GetDouSupportError();
			//DouSupport.status = GetDouSupportStatus();
			
		}
		counter_displacement_DSR = 0;  
	} 
	else
		;
}
/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
void DetectDSRStartBit(void)
{
	data8bit_temp_DSR = 0;
	counter_displacement_DSR = 0;
	DSR.Flag_code_receive_wrong= NO ;
}
/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
bool IsDSRDeviceCodeCompliant(void)
{
	if( DSR_DeviceCode == DSR.DataBuff.devicecode)
		return true ;
	return false ; 
}

/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
bool IsDSRStrlenCodeCompliant(void)
{
	if( (DSR_Strlen - 1) == DSR.DataBuff.strlencode )
		return true ;
	return false ; 
}

/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
bool IsDSREndCodeCompliant(void)
{
	if( DSR_EndCode == DSR.DataBuff.endcode)
		return true ;
	return false ; 
}


/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
bool IsDSRSumCompliant(void)
{
	uint8_t checksum_temp ;
	checksum_temp = DSR.DataBuff.Data[0];
	for(uint8_t i = 1 ; i< (DSR_Strlen - 2) ;i++)
		checksum_temp= checksum_temp ^ DSR.DataBuff.Data[i] ;
	if(checksum_temp == DSR.DataBuff.checksum)
		return true ;
	return false ; 
}

/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
uint8_t GetDouSupportError(void)
{
	return DSR.Data.errorcode;
}


/*******************************************************************************
* 
*/
static void EnableAndInitialDSTWireBreakVariable(void)
{
	DouSupport.error &= ~U8_BIT0 ;	//clear DST wire break error
	DST.flag_wirebreak = false ;
	DST.flag_wirebreakdetection_enable = true ;
	DST.timer_wirebreakdetection_per500ms = Timer_DSTWireBreakDetection_Per500ms ;
}

/*******************************************************************************
 *
 */
void CheckIsDSTWireBreakSetTruePer500ms(void)
{
	if(true == IsVariableTimerDecreaseToZero(&(DST.timer_wirebreakdetection_per500ms)))
	{	
		if(true == DST.flag_wirebreakdetection_enable)
		{	
			DST.flag_wirebreak = true ;
		}
	}
 }
 

/*******************************************************************************
 *
 */
 static void InitialDSRWireBreakVariable(void)
{
	timer_wirebreakdetection_per500ms_DSR = Timer_DSRWireBreakDetection_Per500ms ;
	flag_wirebreak_DSR = false ;
}

/*******************************************************************************
 *
 */
void CheckIsDSRWireBreakSetTruePer500ms(void)
{
	//DSR wire break detection 
	if(true == IsVariableTimerDecreaseToZero(&timer_wirebreakdetection_per500ms_DSR) )
	{
		flag_wirebreak_DSR = true ;
	}
}

#if 0
/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
ErrorDouSupport_Type GetDouSupportError(void)
{
	if( true == DSR.Data.flag_bothlimitswith)
		return Error_LimitSwithError ;
	if( true == DSR.Data.flag_highcurrentinterrupt )
		return Error_DSOCP ;
	if( true == DSR.Data.flag_driveopencircuit)
		return Error_DriveOpenCircuit;
	if( true == DSR.Data.flag_mosshortcircuit)
		return Error_MOSShortCircuit ;
	return Error_None ;
}





/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
StatusDouSupport_Type GetDouSupportStatus(void)
{
	if( true == DSR.Data.flag_nonelswpo)
		return Status_NoneLSWPO ;
	if( true == DSR.Data.flag_timeout)
		if(true == DSR.Data.flag_direction)
			return Status_RetractOuttime;
		else
			return Status_RaiseOuttime ;
	if(true == DSR.Data.flag_limitswith)
		if(true == DSR.Data.flag_direction)
			return Status_RetractInPlace;
		else
			return Status_RaiseInPlace ;
	if(true == DSR.Data.flag_direction)
		return Status_RetractingAction ;
	else
		return Status_RaisingAction ;
}
#endif

#endif
#endif