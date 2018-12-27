#include "defines.h"
#include "systemcontrol.h"
#include "T2C.h"

/*************************************************/
/*******************Variable**********************/
T2C_TypeDef T2C ;
UserConfig_UnionDef UserConfig ;
UserConfig_UnionDef UserConfigAT ;




#if(Commu_Mode_TwoWire == Commu_Mode)
/****************************************************/
/**********************function**********************/
/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
void T2CSIFSend_Service(void)
{
//	if(true == T2C.SIF.flag_100us)
//	{
//		T2C.SIF.flag_100us = false ;
		if(SIF_IDLE == T2C.SIF.status)
			LoadT2CSIFData();
		else if(SIF_Sendding == T2C.SIF.status)
			SendT2CSIFBit();
		else
			;
//	}
}

/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
void LoadT2CSIFData(void)
{	
	if((T2C_ReadConfigWait == T2C.cmd) ||(T2C_WriteConfigWait == T2C.cmd) )
	{
		if(T2C.timer_cmdwait_per500ms >= Timer_T2CCmdWait_Per500ms)
		{
			T2C.cmd -- ;	//from wait cmd to excute cmd
			T2C.timer_cmdwait_per500ms = 0 ;	
		}
	}
	else if(T2C_SendCommonDataCmd == T2C.cmd || T2C_ReadConfigFinished == T2C.cmd)
		T2C.timer_cmdwait_per500ms = 0 ;
	else
		;
	
	if((T2C_SendCommonDataCmd == T2C.cmd) || (T2C_ReadConfigWait == T2C.cmd) || \
		(T2C_WriteConfigWait == T2C.cmd) || (T2C_ReadConfigFinished == T2C.cmd))
	{	
		T2C.SIF.strlen = T2C_CommonCmd_Strlen ;
		LoadT2CCommonCmdSIFByte();
	}
	else if(T2C_ReadConfigCmd == T2C.cmd)
	{
		T2C.SIF.strlen = T2C_ReadConfig_Strlen ;
		LoadT2CReadConfigSIFByte();
		T2C.cmd = T2C_ReadConfigWait ;
	}
	else if(T2C_WriteConfigCmd == T2C.cmd)
	{
		T2C.SIF.strlen = T2C_WriteConfig_Strlen ;
		LoadT2CWriteConfigSIFByte();
		T2C.cmd = T2C_WriteConfigWait ;
	}
	
	#if 0
	else if(T2C_ClrWriteConfigCompleteFlag == T2C.cmd)	//read and clearflag use the same struct and address
	{
		T2C.SIF.strlen = T2C_ReadConfig_Strlen ;
		LoadT2CClearWriteConfigCompleteFlagSIFByte();
		T2C.cmd = T2C_ClrWriteConfigCompleteFlagWait;
	}
	#endif
	
	else
		;

	
	
	
	//fill databuff	
	for(uint8_t i = 0 ;i < T2C.SIF.strlen ;i++)
		T2C.SIF.DataBuff[i] = T2C.SIF.Data[i] ;	
	
	T2C.SIF.byte_index = 0 ;
	T2C.SIF.bit_index = 7 ;	
	T2C.SIF.lowlevel_counter = 100000/100 - 2 ;		//the level is low already ,so the counter = realcounter -2 ;  IDLE set used 1
	T2C.SIF.highlevel_counter = 2000/100 ;
	//GPIO_ResetBits(Port_T2C,Pin_T2C);		//输出低电平//SetT2CPinLow();
	SetPinLevel(Pin_T2C,Low_Level) ;
	T2C.SIF.status = SIF_Sendding ;
}

/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
void LoadT2CCommonCmdSIFByte(void)
{		
#if(COMPILE_ENABLE == Double_LV)
	if(true == IsLVHigherThanLV1RecoverLevel())
		T2C.CommonData.LVlevelBit = 0;			
#else
	T2C.CommonData.LVlevelBit = 1;
#endif
			
#if(COMPILE_ENABLE == OneKeyRepairSet)
	if(T2C.counter_onekeyrepairset_cycle)
	{
		T2C.CommonData.OneKeyRepairBit = 1 ;
		T2C.counter_onekeyrepairset_cycle -- ;
	}
	else
		T2C.CommonData.OneKeyRepairBit = 0 ;
#else
	T2C.CommonData.OneKeyRepairBit = 0 ;
#endif
	
#if(COMPILE_ENABLE == ThreeSpeedSet)
#else
	T2C.CommonData.ThressSpeedBit = 0 ;
#endif
	
#if(COMPILE_ENABLE == ThreeTorqueSet)
#else
	T2C.CommonData.ThressTorqueBit = 0 ;
#endif

	FillT2CSIFData(T2C.CommonData.data) ;	
}


/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
void LoadT2CReadConfigSIFByte(void)
{
	T2C.ReadConfig.cmd = T2C_ReadConfigCmd ;
	FillT2CSIFData(T2C.ReadConfig.data) ;
}

/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
void LoadT2CWriteConfigSIFByte(void)
{
	T2C.WriteConfig.cmd = T2C_WriteConfigCmd ;
	T2C.WriteConfig.UserConfig = UserConfig.UserConfigStruct ;
	FillT2CSIFData(T2C.WriteConfig.data) ;
}


/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
//void LoadT2CClearWriteConfigCompleteFlagSIFByte(void)
//{
//	T2C.ReadConfig.cmd = T2C_ClrWriteConfigCompleteFlag ;
//	FillT2CSIFData(T2C.ReadConfig.data) ;
//	
//}


/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无	
*******************************************************************************/
void FillT2CSIFData(uint8_t arr[])
{
	for(uint8_t i = 1 ; i< (T2C.SIF.strlen - 2) ; i++ )
		T2C.SIF.Data[i]= arr[i] ;

	//Get checkCode
	T2C.SIF.Data[T2C.SIF.strlen - 2] = T2C.SIF.Data[2] ;
	for(uint8_t i = 3 ; i <(T2C.SIF.strlen - 2) ;i++  )
		T2C.SIF.Data[T2C.SIF.strlen - 2] ^= T2C.SIF.Data[i] ;
	
	//set Endcode
	T2C.SIF.Data[T2C.SIF.strlen - 1] = T2C_EndCode ;
}


/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
void SendT2CSIFBit(void)
{	
	if(T2C.SIF.lowlevel_counter)
	{
		//GPIO_ResetBits(Port_T2C,Pin_T2C);			//output lowlevel
		SetPinLevel(Pin_T2C,Low_Level) ;
		T2C.SIF.lowlevel_counter -- ;
	}
	else		
	{
		if(T2C.SIF.highlevel_counter)	
		{
			//GPIO_SetBits(Port_T2C,Pin_T2C); 			//output highlevel	
			SetPinLevel(Pin_T2C,High_Level) ;
			T2C.SIF.highlevel_counter -- ;
		}
		else										//next bit
		{
			//GPIO_ResetBits(Port_T2C,Pin_T2C);		//output lowlevel
			SetPinLevel(Pin_T2C,Low_Level) ;
			T2C.SIF.bit_index ++ ;
			if(T2C.SIF.bit_index >= 8)				//next byte
			{
				T2C.SIF.bit_index = 0 ;
				T2C.SIF.byte_index ++ ;
				if(T2C.SIF.byte_index >= T2C.SIF.strlen)		//next frame data
					T2C.SIF.status = SIF_IDLE ;
				else
					T2C.SIF.data_buff = T2C.SIF.Data[T2C.SIF.byte_index] ;	
			}

			if(T2C.SIF.data_buff & U8_BIT7)			//bit 1
			{
				T2C.SIF.lowlevel_counter = (2000/100 - 1) ;		//the level is low already ,so the counter = realcounter -1 ;
				T2C.SIF.highlevel_counter = 4000/100 ;
			}
			else									//bit 0
			{
				T2C.SIF.lowlevel_counter = (4000/100 - 1) ;
				T2C.SIF.highlevel_counter = 2000/100 ;
			}	
			T2C.SIF.data_buff <<= 1 ;			//left shift
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
UserConfig_TypeDef AdjustOrderOfUserConfig(UserConfig_TypeDef In)
{
	UserConfig_TypeDef temp ;
	temp = In ;
	temp.hAvgCrtLmt = SwapH8L8(temp.hAvgCrtLmt) ;
	temp.hPhaseCrtLmt = SwapH8L8(temp.hPhaseCrtLmt) ;
	temp.hBankCrtLmt0 = SwapH8L8(temp.hBankCrtLmt0) ;
	temp.hBankCrtLmt1 = SwapH8L8(temp.hBankCrtLmt1) ;
	temp.hBankCrtLmt2 = SwapH8L8(temp.hBankCrtLmt2) ;
	temp.hFirmwareVer = SwapH8L8(temp.hFirmwareVer) ;
	temp.hDataIntegrityID = SwapH8L8(temp.hDataIntegrityID) ;
	return temp ;	
}



#endif


