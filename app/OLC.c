
#include "defines.h"
#include "OLC.h"
#include "ATControl.h"
#include "systemcontrol.h"
#include "KeyLess.h"
#include "MechanicalLock.h"
#include "MileageCal.h"
#include "BatteryPowerCal.h"
#include "T2C.h"



#define 	TimePeriod_Noconnected2Controller_Per50ms          40	//2s

#if(Commu_Mode_Common != Commu_Mode)
/*************************************************/
/*******************Variable**********************/
//level
static LevelStatus_TypeDef levelstatus_temp_OLC = Low_Level ;
static LevelStatus_TypeDef levelstatus_OLC = Low_Level ;
static uint16_t timer_levelstatus_OLC = 0 ; 
static uint16_t timer_highlevel_OLC = 0 ;
static uint16_t timer_lowlevel_OLC = 0 ;

//level to data
static uint8_t data8bit_temp_OLC = 0 ;

static uint8_t counter_displacement_OLC = 0 ;
OLCDataBuff_Typedef DataBuff = {0} ;
OLCType_TypeDef type = OLCType_Undefined ;


bool Flag_code_receive_wrong = false ;


//OLC_TypeDef OLC ;
OLCM_TypeDef OLCM ;
BLEModuleConnect_TypeDef BLEMdlCnt; 
Motor_TypeDef	Motor ;
/****************************************************/
/**********************function**********************/
/*******************************************************************************
* ������  		: 
* ��������    	: 
* �������      : ��
* ������      : ��
* ����ֵ        : ��
*******************************************************************************/
void ReadOLCLevel(void)
{
	levelstatus_temp_OLC = GetPinLevel(Pin_YXT) ;	
	if(levelstatus_OLC != levelstatus_temp_OLC)
	{
		UpdataOLCLevelStatus(); 
		if(levelstatus_OLC == Low_Level)
		{
			if(timer_lowlevel_OLC >= OLC_Counter_Start_Bit)
				DetectStartBit();
			else			
				GetOLCLogicValue();
		}	  
	} 
	else
	{
		timer_levelstatus_OLC ++ ;
	}
}

/*******************************************************************************
* ������  		: 
* ��������    	: 
* �������      : ��
* ������      : ��
* ����ֵ        : ��
*******************************************************************************/
void UpdataOLCLevelStatus(void)
{
	if(Low_Level == levelstatus_OLC)
		timer_lowlevel_OLC = timer_levelstatus_OLC ;
	else
		timer_highlevel_OLC = timer_levelstatus_OLC ;
	timer_levelstatus_OLC = 0 ;
	levelstatus_OLC = levelstatus_temp_OLC ;
	BLEMdlCnt.timer_connect2controller_per50ms = TimePeriod_Noconnected2Controller_Per50ms ;
}

/*******************************************************************************
* ������  		: 
* ��������    	: 
* �������      : ��
* ������      : ��
* ����ֵ        : ��
*******************************************************************************/
void GetOLCLogicValue(void)
{
	data8bit_temp_OLC <<= 1 ;
	if(timer_highlevel_OLC > timer_lowlevel_OLC )
	{  
		if((timer_highlevel_OLC- timer_lowlevel_OLC) > 5)  
			data8bit_temp_OLC ++ ;
		else
			Flag_code_receive_wrong = YES ;	  
	}
	else
	{	
		if((timer_lowlevel_OLC- timer_highlevel_OLC) > 5)		  
			;
		else
			Flag_code_receive_wrong = YES ;	
	} 
	counter_displacement_OLC ++ ;
	if(0 == counter_displacement_OLC % OneByteBitNum)
		Write8bitToCodeTemp(); 
}

/*******************************************************************************
* ������  		: 
* ��������    	: 
* �������      : ��
* ������      : ��
* ����ֵ        : ��
*******************************************************************************/
void Write8bitToCodeTemp(void)
{
	//stack the new byte
	DataBuff.Data[(counter_displacement_OLC / OneByteBitNum) - 1] = data8bit_temp_OLC ;

	type = GetOLCType() ;

	if(OLCType_Common == type)
		WriteToCommuCode();	
#if(Commu_Mode_TwoWire == Commu_Mode)
	else if(OLCType_ReadControllerE2PROMCfg == type)
		GetReadControllerConfigCmdReplyMessage();
	else if (OLCType_WriteControllerE2PROMCfg == type)
		GetWriteControllerConfigCmdReplyMessage();
#endif
	else if(OLCType_Unfinished == type)
		;
	else
		;
	
	data8bit_temp_OLC = 0 ;
		
}	  

/*******************************************************************************
* ������  		: 
* ��������    	: 
* �������      : ��
* ������      : ��
* ����ֵ        : ��
*******************************************************************************/
OLCType_TypeDef GetOLCType(void)
{
	if((OLC_CommonCmd_Strlen * OneByteBitNum) == counter_displacement_OLC )
	{
		if((true == IsDeviceCodeCompliant(DeviceCode_CommonCmd))&&\
			(true == IsSeqCodeCompliant(SeqCode_Commoncmd)))
			return OLCType_Common ;
		
		else
			return OLCType_Undefined ;
			
	}
	else if(((T2C_WriteConfig_ValidStrlen + 3) * OneByteBitNum) == (int)counter_displacement_OLC)
	{
		if(true == IsDeviceCodeCompliant(DeviceCode_ConfigCmd))
			return OLCType_ReadControllerE2PROMCfg ;
			
		else
			return OLCType_Undefined ;
		
	}
	else if((5 * OneByteBitNum) == counter_displacement_OLC)
	{
		if(true == IsDeviceCodeCompliant(DeviceCode_ConfigCmd))
			return OLCType_WriteControllerE2PROMCfg;
			
		else
			return OLCType_Undefined ;
	}	
	else
		return OLCType_Unfinished ;
	
}

/*******************************************************************************
* ������  		: 
* ��������    	: 
* �������      : ��
* ������      : ��
* ����ֵ        : ��
*******************************************************************************/
void WriteToCommuCode(void)
{
	if(true == IsOLCSumCompliant(DataBuff.checksum))
	{			
		GetOneLineCommuMessage();
		CalQAndMileageReleaseAndCurrentAvr();
		CalCurrentReleaseAndHallChangeInFixedTime();
		//Guard.status_park = AnalysisStateOfParkWhenConnectedWithController();
		
	}
	else
		;
}



/*******************************************************************************
* ������  		: 
* ��������    	: 
* �������      : ��
* ������      : ��
* ����ֵ        : ��
*******************************************************************************/
void DetectStartBit(void)
{
	data8bit_temp_OLC = 0;
	counter_displacement_OLC = 0;
	Flag_code_receive_wrong = false;
}

/*******************************************************************************
* ������  		: a
* ��������    	: 
* �������      : ��
* ������      : ��
* ����ֵ        : ��
*******************************************************************************/
bool IsDeviceCodeCompliant(uint8_t devicecode)
{
	if( devicecode == DataBuff.devicecode)
		return true ;
	return false ; 
}

/*******************************************************************************
* ������  		: a
* ��������    	: 
* �������      : ��
* ������      : ��
* ����ֵ        : ��
*******************************************************************************/
bool IsSeqCodeCompliant(uint8_t seqcode)
{
	if(seqcode == DataBuff.seqcode)
		return true ;
	return false ; 
}


/*******************************************************************************
* ������  		: 
* ��������    	: 
* �������      : ��
* ������      : ��
* ����ֵ        : ��
*******************************************************************************/
bool IsOLCSumCompliant(void)
{
	uint8_t checksum_temp ;
	checksum_temp = DataBuff.Data[0] ;
	
	for(uint8_t i = 1 ; i<= (counter_displacement_OLC/OneByteBitNum- 2 );i++)
		checksum_temp= checksum_temp ^ DataBuff.Data[i] ;
	if(checksum_temp == DataBuff.Data[counter_displacement_OLC/OneByteBitNum- 1])
		return true ;
	return false;
}

/*******************************************************************************
* ������  		: 
* ��������    	: 
* �������      : ��
* ������      : ��
* ����ֵ        : ��
*******************************************************************************/
void GetOneLineCommuMessage(void)
{
	//get raw data
	OLCM.rawdata[0] = DataBuff.errorbyte ;    //error cruise and assist and so on 
	OLCM.rawdata[1] = DataBuff.switchbyte ;    //threespeed,charge,break,runflag and so on 
	OLCM.rawdata[2] = DataBuff.protectbyte ;    //overcurrent,protect,back,speedlimit and so on
	OLCM.rawdata[3] = DataBuff.current_lowbyte ;    //current     the value div 4 is real current
	
	//Get counter_hall_speed
	OLCM.counter_hall_speed.h8 = DataBuff.hallspeed_highbyte ;
	OLCM.counter_hall_speed.l8 = DataBuff.hallspeed_lowbyte ;
	if(OLCM.counter_hall_speed.sum < 0x15)
		OLCM.counter_hall_speed.sum = 0 ;
	GetHallSpeedFilteredValue();
	
	//Get current
	if(DataBuff.current_lowbyte <= 0x01 || DataBuff.current_lowbyte >= 0XFE  )
		OLCM.data_current = 0 ;
	else
		OLCM.data_current = DataBuff.current_lowbyte ;	//����4������ʵֵ
	
	//Get error
	//��Ϊȥ��D7��D2��D1��Ӱ�죬�����0111 1001��0x79��
	OLCM.data_error = DataBuff.errorbyte & 0x79 ;
	
	if(DataBuff.switchbyte & U8_BIT5)
	{
		OLCM.flag_brake = true ;
#if MECHANICALLOCK_INTERNAL_COMPILE			
	#if(ELU_ActionSingle_Mode == ELU_BreakSingle_Mode)	
		if((Status_PowerOn == KeyLess.status) && (MechalMotorLock.status == Status_WaitForBrake))
		{
			MechalMotorLock.counter_brake_filter ++ ;
			if(MechalMotorLock.counter_brake_filter >= Counter_Brake_Filter)
				MechalMotorLock.flag_breakconfirm = true ;
		}
		else
		{
			MechalMotorLock.counter_brake_filter = 0 ;
			MechalMotorLock.flag_breakconfirm = false ;
		}
	#endif	
#endif
	}
	else
	{
		OLCM.flag_brake = false ;
#if MECHANICALLOCK_INTERNAL_COMPILE	
		MechalMotorLock.counter_brake_filter = 0 ;
		MechalMotorLock.flag_breakconfirm = false ;
#endif
		
	}	
	
	//Get motor run status
	if(DataBuff.switchbyte & U8_BIT6)
		OLCM.flag_motor_running = true ;
	else
		OLCM.flag_motor_running = false ;

	//Updata Odometer
	

#if 0
#if(Commu_Mode_TwoWire == Commu_Mode)	
	if(1 == OLC.DataBuff.flag_clrflagwritenconfigfinished )
	{
		if(T2C_WriteConfigWait == T2C.cmd)
		{
			ResponseWriteControllerE2PROMConfigFinished() ;
			T2C.cmd = T2C_ClrWriteConfigCompleteFlag ;
		}
	}
	else
	{
		if(T2C_ClrWriteConfigCompleteFlagWait == T2C.cmd)
			T2C.cmd = T2C_SendCommonDataCmd ;
	}

#endif	
#endif
	
	
	//static uint16_t LV_last = 0 ;
	static uint8_t counter = 0 ;
	uint32_t cal_temp ;
	//Get Q release in this frame
	if(OLCM.data_current > 0X01)
	{
		/*//��������0.678sǰ���ѹ�Ĳ�ֵ������2����ֵ���ƶ���ֵ����
		cal_temp = (LV.avr_adc + LV_last ) / 2 ;
		
		//counter���������������������ĵ�����Ϣ��ΪĨȥβ�����µ����
		//���������15A���� 	���Ϊ1/4 / 15 =1.6% 
		if(counter % 2)
		cal_temp = (cal_temp>>LV_Accuracy_Reduced_ForQ) * (OLCM.data_current + 1);
		else
		cal_temp = (cal_temp>>LV_Accuracy_Reduced_ForQ) * OLCM.data_current ;*/
		
		//counter���������������������ĵ�����Ϣ��ΪĨȥβ�����µ����
		//���������15A���� 	���Ϊ1/4 / 15 =1.6% 
		if(counter % 2)
			cal_temp = OLCM.data_current + 1;
		else
			cal_temp = OLCM.data_current ;
		
		OLCM.data_Q_thisframe = cal_temp ;
		counter ++ ;
		//OLCM.data_Q_thisframe = ((LV.avr_adc >>LV_Accuracy_Reduced_ForQ) * OLCM.data_current);
		// OLCM.data_Q_thisframe =  OLCM.data_current ;
		//LowPassFilteringQPerFrame();
		//LowPassFilteringMileagePerFrame();
	}
	else
		OLCM.data_Q_thisframe = 0 ;
	//LV_last = LV.avr_adc ;
	
}

/*******************************************************************************
* ������  		: 
* ��������    	: ��Ϊ���ǵ�����ֱ��True��False�жϣ�������������м�״̬����Ҫ����֮ǰ��״̬��
				��˻�����ҪGuard.status_connectwithcontroller����ת���ݴ档
				ָ��������YXT�ϵĵ�ƽ�Ƿ�Ƶ���仯�ж�  ��ע��δ���Ӽ��ź��Ŷ�������
				ͨ�ð������ж�YXT��ƽ�Ƿ�һֱΪ�ߵ�ƽ�жϡ�
* �������      : ��
* ������      : ��
* ����ֵ        : ��
*******************************************************************************/
void UpdateStatusConnectToController(void)
{
#if(Commu_Mode_Common == Commu_Mode)
	if(False == BLEMdlCnt.status_connect2controller)
	{
		if(BLEMdlCnt.timer_connect2controller5V_50ms > TimePeriod_Noconnected2Controller_Per50ms)   // > 2s
			BLEMdlCnt.status_connect2controller = true ;
	}
	else		
	{
		if(BLEMdlCnt.timer_noconnect2controller5V_50ms > TimePeriod_Noconnected2Controller_Per50ms)   // > 2s
			BLEMdlCnt.status_connect2controller = false ;
	}
#else
	if(BLEMdlCnt.timer_connect2controller_per50ms)
		BLEMdlCnt.timer_connect2controller_per50ms -- ;
	if(false == BLEMdlCnt.status_connect2controller)
	{
		if(BLEMdlCnt.timer_connect2controller_per50ms)   // <2s
			BLEMdlCnt.status_connect2controller = true ;	
	}
	else
	{
		if(0 == BLEMdlCnt.timer_connect2controller_per50ms)   // >2s
			BLEMdlCnt.status_connect2controller = false ;
	}
#endif
}




#if(Commu_Mode_TwoWire == Commu_Mode)
/*******************************************************************************
* ������  		: 
* ��������    	: 
* �������      : ��
* ������      : ��
* ����ֵ        : ��
*******************************************************************************/
void GetReadControllerConfigCmdReplyMessage(void)
{
	if(true == IsOLCSumCompliant())
	{
		if((T2C_ReadConfigCmd == DataBuff.reply_cmd)&&\
			((UserConfigData_Strlen + 1)== DataBuff.strlen_valid))
			GetReadConfigReply();
		else
			;
	}
	else
		;
}

/*******************************************************************************
* ������  		: 
* ��������    	: 
* �������      : ��
* ������      : ��
* ����ֵ        : ��
*******************************************************************************/
void GetWriteControllerConfigCmdReplyMessage(void)
{
	if(true == IsOLCSumCompliant())
	{
		if((T2C_WriteConfigCmd == DataBuff.reply_cmd)&&\
			((1 + 1)== DataBuff.strlen_valid))
			GetWriteConfigReply();
		else
			;
	}
	else
		;
}





/*******************************************************************************
* ������  		: 
* ��������    	: 
* �������      : ��
* ������      : ��
* ����ֵ        : ��
*******************************************************************************/
void GetReadConfigReply(void)
{
	UserConfig.UserConfigStruct = DataBuff.UserConfig ;
	UserConfigAT.UserConfigStruct = AdjustOrderOfUserConfig(UserConfig.UserConfigStruct) ;
	//ResponseReadControllerE2PROMConfigBlock();		
	T2C.cmd = T2C_ReadConfigFinished ;				
}

/*******************************************************************************
* ������  		: 
* ��������    	: 
* �������      : ��
* ������      : ��
* ����ֵ        : ��
*******************************************************************************/
void GetWriteConfigReply(void)
{
	if(T2C_WriteConfigWait == T2C.cmd)
	{
		if(0 == DataBuff.result_writecfgrply)
			T2C.cmd = T2C_SendCommonDataCmd ;
		else 
			T2C.cmd = T2C_WriteConfigCmd ;
	}
	else
		;
}



/*******************************************************************************
* ������  		: 
* ��������    	: 
* �������      : ��
* ������      : ��
* ����ֵ        : ��
*******************************************************************************/
//void GetControllerE2PROMConfig(void)
//{
	//for(uint8_t i= 0 ;i < T2C_ConfigData_Strlen ;i++)
		//T2C.WriteConfig.configdata[i] = OLC.DataBuff.Data[i + 2] ;
//}

#endif
#endif



