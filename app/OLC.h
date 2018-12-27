#ifndef __OLC_H
#define __OLC_H


/********************Annotate*********************************/
/*
一线通完整的一帧信息时间为0.678s
*/


/*********************typedef************************/
typedef struct
{
	bool	status_connect2controller ;
	#if(Commu_Mode_Common == Commu_Mode)
		uint8_t timer_connect2controller5V_50ms ;
		uint8_t timer_noconnect2controller5V_50ms ;
	#else
		uint8_t timer_connect2controller_per50ms  ;
	#endif
}BLEModuleConnect_TypeDef;

__packed typedef struct
{
	uint8_t bRatedVolt;			// 48/60/64/72/84/96/120;
	uint8_t bBatDieVolt;			// 32-110;
	UnionSumWithH8L8_TypeDef hAvgCrtLmt;			// 10-200;
	UnionSumWithH8L8_TypeDef hPhaseCrtLmt;		// 10-500;
	uint8_t bHallType;			// 120/60;
//7	
	uint8_t bTurboIncCrt;		// 0-33% of hAvgCrtLmt; show warning if >20%;
	uint8_t bTurboDuration;		// 0-30s
	uint8_t bTurboCooldown;		// 30s-300s
//3	
	uint8_t bBankScale0;			// %, 10 <= x <= 100
	uint8_t bBankScale1;			// %, 10 <= x <= 100
	uint8_t bBankScale2;			// %, 10 <= x <= 100
//3	
	UnionSumWithH8L8_TypeDef hBankCrtLmt0;		// <= hAvgCrtLmt
	UnionSumWithH8L8_TypeDef hBankCrtLmt1;		// <= hAvgCrtLmt
	UnionSumWithH8L8_TypeDef hBankCrtLmt2;		// <= hAvgCrtLmt
//6	
	uint8_t bBankTorqueScale0;	// %, 10 <= x <= 100
	uint8_t bBankTorqueScale1;	// %, 10 <= x <= 100
	uint8_t bBankTorqueScale2;	// %, 10 <= x <= 100
//3	
	uint8_t bBrakeForceScale;	// %, 0 <= x <= 100
	uint8_t bSpeedLimitScale;	// %, 10 <= x <= 100
//2	
	uint8_t bCruiseTimer;		// 3-15s;
//1	
	uint8_t bBikeRevSpeed;		// 0-20km/h, only available when closed-loop mode selected;
//1	
	uint8_t bFieldWeakenScale;	// 5-30%
//1	
	uint8_t bSpeedMeterScale;	// %, 5 <= x <= 100
//1	
	uint8_t bBankMode:2;			// 0 = Disabled; 1 = Button-type; 2 = Switch-type;
	uint8_t bCruiseMode:2;		// 0-2 = Disabled / Button-trigger / Timer-trigger;
	uint8_t bBikeRevMode:2;		// 0-2 = Disabled / Open-loop / Closed-loop
	uint8_t bFieldWeakenEn:1;	// 0 = Disabled; 1 = Enabled;
	uint8_t bTurboEn:1;			// 0 = Disabled; 1 = Enabled;
	uint8_t bBikeParkEn:1;		// 0 = Disabled; 1 = Enabled;
//2	
	UnionSumWithH8L8_TypeDef hFirmwareVer;		// (major ver).(junior ver)
//2	
	UnionSumWithH8L8_TypeDef hDataIntegrityID;	// used to check data integrity, must be last member of this structure.
//2
}UserConfig_TypeDef;



#define		UserConfigData_Strlen				sizeof(UserConfig_TypeDef)


typedef struct
{
	//CommunicaLevelTypeDef Level;	
	uint8_t data8bit_temp_OLC ;
	uint16_t counter_displacement_OLC ;
}Level2Data_TypeDef;

typedef struct
{
	//CommunicaLevelTypeDef Level;	
	uint8_t data8bit_temp_OLC ;
	uint8_t counter_displacement_OLC ;
}Data2Level_TypeDef;

#define		Num_HallSpeed_Filter				4	//一线通大约2.7s 通用版2s


#if(Commu_Mode_Common != Commu_Mode)
/**********************YXT parament************************/
#define		OLC_Counter_Start_Bit				400


/**********************YXT data************************/
#define		OLC_CommonCmd_ValidStrlen			9
#define		OLC_CommonCmd_Strlen				(OLC_CommonCmd_ValidStrlen + 3)
#define 	DeviceCode_CommonCmd				0x08
#define 	SeqCode_Commoncmd					0x61

/**********************Config Controller E2PROM data************************/
#define		OLC_ReadConfigCmd_ValidStrlen		(1 + UserConfigData_Strlen )
#define 	OLC_ReadConfigCmd_Strlen			(OLC_ReadConfigCmd_ValidStrlen + 3)
#define		DeviceCode_ConfigCmd				0X09
#define 	SeqCode_ConfigCmd					0x61

//#if(OLC_ReadConfigCmd_Strlen > OLC_CommonCmd_Strlen)
//	#define		OLC_Strlen		OLC_ReadConfigCmd_Strlen
//#else
//	#define		OLC_Strlen		OLC_CommonCmd_Strlen
//#endif
#define		OLC_Strlen							OLC_ReadConfigCmd_Strlen


typedef enum
{	
	OLCType_Undefined = 0 ,
	OLCType_Unfinished ,
	OLCType_Common ,
	OLCType_ReadControllerE2PROMCfg ,
	OLCType_WriteControllerE2PROMCfg
}OLCType_TypeDef ;


typedef union
{
	uint8_t Data[OLC_Strlen] ;
	struct
	{
		uint8_t devicecode ;
		uint8_t seqcode ;
		
		union
		{
			uint8_t status1 ;
			struct
			{
				uint8_t flag_clrflagwritenconfigfinished : 1 ;
				uint8_t reserved : 7 ;
			}	;
		} ;
		union
		{
			uint8_t status2 ;
			uint8_t errorbyte ;
		} ;
		union
		{
			uint8_t status3 ;
			uint8_t switchbyte ;
		} ;
		union
		{
			uint8_t status4 ;
			uint8_t protectbyte ;
		} ;
		union
		{
			uint8_t status5 ;
			uint8_t current_highbyte ;
		} ;
		union
		{
			uint8_t status6 ;
			uint8_t hallspeed_highbyte ;
		};
		union
		{
			uint8_t status7 ;
			uint8_t	hallspeed_lowbyte ;
		} ;
		union
		{
			uint8_t status8 ;
			uint8_t q_percentagebyte ;
		} ;
		union
		{
			uint8_t status9 ;
			uint8_t current_lowbyte ;
		};
		uint8_t checksum ;
	} ;
	struct
	{
		uint8_t devicecode_readconfig ;
		uint8_t strlen_valid ; 
		uint8_t reply_cmd ; 
		union
		{
			struct
			{
				UserConfig_TypeDef UserConfig ;
				uint8_t checksum_readcfgrply ;
			};
			struct
			{
				uint8_t result_writecfgrply ;
				uint8_t	checksum_writecfgrply ;	
			};
		};
	};

	
}OLCDataBuff_Typedef ;

/*typedef struct
{
	//Level2Data_TypeDef Level2Data ;
	//OLCDataBuff_Typedef DataBuff ;
	//uint8_t datacode_temp[OLC_Strlen];
	//uint8_t datacode[OLC_Strlen];
	//bool Flag_code_receive_wrong ;
	OLCType_TypeDef type ;
}OLC_TypeDef;*/

typedef struct
{
	uint8_t		rawdata[4] ;
	UnionSumWithH8L8_TypeDef	counter_hall_speed ;
	uint8_t		data_current ;
	uint8_t		data_error ;
	bool	flag_motor_running ;
	bool	flag_brake ;
	
	uint32_t	data_Q_thisframe ;

}OLCM_TypeDef;

typedef enum
{ 
	Status_Motor_Park  = 0 ,
	Status_Motor_Stall  ,
	Status_Motor_Run,
}StatusMotor_TypeDef;

typedef struct
{
	StatusMotor_TypeDef status_motor ;
	
	//bool	status_park ;
	uint16_t timer_stall_500ms  ;
}Motor_TypeDef;


/*******************Variable**********************/
//extern OLC_TypeDef OLC ;
extern OLCM_TypeDef OLCM  ;
extern BLEModuleConnect_TypeDef BLEMdlCnt; 
extern Motor_TypeDef		Motor;



#if(Commu_Mode_TwoWire == Commu_Mode)
/*****************************************************/
/*****************************************************/
/**********************define************************/
/**********************CustomSerialInterface parament************************/
/* 定义CSI的一帧结构为：长时间低电平 + 开始码 + 内容长度 + 内容 + 长度和内容异或校验 +结束码   */

#define		CSI_StartCode				0x3A
#define		CSI_Strlen_Status			1
#define		CSI_EndCode					0x0A


#define		CSI_Strlen_Code					(4 + CSI_Strlen_Status)		//Attention:必须小于16 否则timer_target会溢出
#define		Counter_OneKeyRepairSet_Cycle	3



#define		Status_LowSpeed				1
#define		Status_MiddleSpeed			2
#define		Status_HighSpeed			3

#define		Status_LowTorque			1
#define		Status_MiddleTorque			2
#define		Status_HighTorque			3

#endif

#define		MinHall_WhenMotorRunning					3
#define		HallCounter_WhenYXTBroken					0XFFFF

/***************function******************/
void ReadOLCLevel(void) ;
void UpdataOLCLevelStatus(void);
void GetOLCLogicValue(void);
void Write8bitToCodeTemp(void);
OLCType_TypeDef GetOLCType(void);
void WriteToCommuCode(void);

void DetectStartBit(void);
bool IsDeviceCodeCompliant(uint8_t);
bool IsSeqCodeCompliant(uint8_t);

bool IsOLCSumCompliant();
void GetOneLineCommuMessage(void) ;
void UpdateStatusConnectToController(void) ;
//void InitialLowPassFilteringPerFrame(void) ;

//uint32_t CalQReleaseInThisFrame(void) ;

#if(Commu_Mode_TwoWire == Commu_Mode)
void GetReadControllerConfigCmdReplyMessage(void);
void GetWriteControllerConfigCmdReplyMessage(void);
void GetReadConfigReply(void) ;
void GetWriteConfigReply(void) ;
//void GetControllerE2PROMConfig(void);
#endif
#endif
#endif
