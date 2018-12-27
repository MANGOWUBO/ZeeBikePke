#ifndef __DOUSUPPORT_H
#define __DOUSUPPORT_H

#include "systemcontrol.h"
#include "OLC.h"

/**********************DST parament************************/
/* 定义DST的一帧结构为：长时间低电平 + 开始码 + 内容长度 + 内容 + 长度和内容异或校验 +结束码   */
#if DOUSUPPORT_INTERNAL_COMPILE
	#if(FuzhouPeipeng_DouSupport_ManufactID == DouSupport_Manufacturer_ID)
		#define		DST_StartCode				0x0A
	#elif(DongdaLanniao_DouSupport_ManufactID == DouSupport_Manufacturer_ID)
		#define		DST_StartCode				0x1A
	#else
		//#error "Missing DouSupport_Manufacturer_ID"
	#endif
#endif

#define		DST_ValidStrlen						1
#define		DST_Strlen							(DST_ValidStrlen + 5 )
#define		DST_EndCode							0x0A	
#define		DST_CycleTimes						3



/**********************DSR parament************************/
#define		DSR_Strlen							6



#define		DSR_Timer_Counter_Start_Bit_Per100us				400
#define 	DSR_DeviceCode						0x07
#define		DSR_EndCode							0x0A


#define		Timer_DouSupportAction_Per50ms						110//双撑运行7s 可以放点余量	


#define		Timer_DSTWireBreakDetection_Per500ms				40//DST wire break detection is 20s	
#define		Timer_DSRWireBreakDetection_Per500ms				4//DSR wire break detection is 2s	




typedef enum
{
	Cmd_DST_Nop = 0 ,
#if(FuzhouPeipeng_DouSupport_ManufactID == DouSupport_Manufacturer_ID)		
	Cmd_DST_Raise, 
	Cmd_DST_Retract ,
#elif(DongdaLanniao_DouSupport_ManufactID == DouSupport_Manufacturer_ID)
	Cmd_DST_Retract ,
	Cmd_DST_Raise,
#else
#endif
}CmdDouSupportTrans_TypeDef; 


typedef enum
{
  Status_RetractInPlace = 0 ,
  Status_RaiseInPlace, 
  Status_RetractingAction ,
  Status_RaisingAction ,
 
#if(DongdaLanniao_DouSupport_ManufactID == DouSupport_Manufacturer_ID) 
  Status_RetractOuttime ,			
  Status_RaiseOuttime ,

  Status_RetractCmdStart ,
  Status_RaiseCmdStart ,
  Status_WaitForRetract ,		  
  Status_WaitForRaise ,

  //Status_NoneLSWPO ,		//none limit swith detect when power on
#endif
}StatusDouSupport_TypeDef;

typedef enum
{
	DouSupportActionCmdFrom_FlowControl = 0 ,
	DouSupportActionCmdFrom_ATCmd ,	
}DouSupportActionCmdFrom_TypeDef ;



typedef enum
{
  Error_None ,		
  Error_LimitSwithError ,		//not allowed to drive dousupport
  Error_DSOCP ,			//dousupport Over current 
  Error_DriveOpenCircuit ,
  Error_MOSShortCircuit ,

  Error_WrongDirection ,
}ErrorDouSupport_TypeDef;





typedef union
{
	uint8_t 	data[DST_Strlen] ;
	struct
	{
		uint8_t nullbyte ;
		uint8_t startcode ;
		uint8_t	strlencode ;
		union
		{
			uint8_t						bcmd ;
			CmdDouSupportTrans_TypeDef	ecmd ;
		} ;
		uint8_t	checkcode	;
		uint8_t endcode		;
	} ;
}DSTProtocal_TypeDef	;

/*typedef struct
{
	SIFStatus_TypeDef	status ;
	uint8_t			strlen ;
	uint8_t 		bit_index ;
	uint8_t 		byte_index ;
	uint8_t			data_buff ;
	uint8_t 		highlevel_counter ;
	uint16_t 		lowlevel_counter ;
	bool		flag_100us ;
	uint8_t			Data[DST_Strlen] ;
	uint8_t 		DataBuff[DST_Strlen];
}DSTSIF_TypeDef;*/






typedef union
{
	uint8_t Data[DSR_Strlen] ;
	struct
	{
		uint8_t devicecode ;
		uint8_t strlencode ;
		union
		{
			uint8_t statuscode ;
			struct
			{
				uint8_t status_reservedbit : 4 ;
				uint8_t flag_nonelswpo : 1 ;			//nonelimitswithwhenpoweron
				uint8_t flag_timeout : 1 ;
				uint8_t flag_limitswith : 1 ;		
				uint8_t flag_direction : 1 ;			//raise set 1
			};
		};
		union
		{
			uint8_t errorcode ;
			struct
			{
				uint8_t error_reservedbit : 4 ;
				uint8_t flag_mosshortcircuit : 1 ;		//mosfet short circuit
				uint8_t flag_driveopencircuit : 1 ;		//drive open circuit
				uint8_t flag_highcurrentinterrupt : 1 ;
				uint8_t flag_bothlimitswith : 1 ;
			};
		};
		uint8_t checksum ;
		uint8_t endcode ;
	};
}DSRDataBuff_TypeDef ;



typedef struct
{
	//Level2Data_TypeDef 	Level2Data ;
	DSRDataBuff_TypeDef	DataBuff ;
	DSRDataBuff_TypeDef Data;
	bool 			Flag_code_receive_wrong ;	
	//bool			flag_wirebreak ;
	//uint8_t				timer_wirebreakdetection_per100ms ;
}DouSupportReceive_TypeDef;		


typedef struct
{
	StatusDouSupport_TypeDef	status ;
	bool	flag_BLEResponseStatusAndError ;
	
#if(FuzhouPeipeng_DouSupport_ManufactID == DouSupport_Manufacturer_ID)		
	uint8_t timer_action_per50ms ;			
	DouSupportActionCmdFrom_TypeDef	cmdfrom ;
#elif(DongdaLanniao_DouSupport_ManufactID == DouSupport_Manufacturer_ID)
	uint8_t	  	error ;
	uint8_t 	status_byte ;
#else
#endif
}DouSupport_TypeDef;
	
//typedef struct
//{
//	bool status_guard  ;
//	StatusLVSwitch status_LVLevel ;
//}DouSupportTransMessage_Type;
	

/*************************************************/
/*******************Variable**********************/
extern SIFOUT_TypeDef 				DSTSIF ;
extern DouSupportReceive_TypeDef	DSR ;
extern DouSupport_TypeDef			DouSupport ;
//extern DouSupportTransMessage_Type DouSupportTransMessage ; 

/*******************Function**********************/
#if DOUSUPPORT_INTERNAL_COMPILE
void DouSupportRaiseAction(void) ;
void DouSupportRetractAction(void) ;
void CheckDouSupportActionResult(void) ;

#if(DongdaLanniao_DouSupport_ManufactID == DouSupport_Manufacturer_ID)
uint8_t ReturnDouSupportError(void) ;
void StopDouSupportActionWhenDouSupportError(void) ;
void SetFlagWhenDouSupportTimeOut(StatusDouSupport_TypeDef	status) ;
void ReadDSRLevel(void) ;
void UpdataDSRLevelStatus(void);
void GetDSRLogicValue(void);
void Write8bitToDSRCodeTemp(void);
void WriteToDSRCode(void);
void DetectDSRStartBit(void);
bool IsDSRDeviceCodeCompliant(void);
bool IsDSRStrlenCodeCompliant(void) ;
bool IsDSREndCodeCompliant(void) ;
bool IsDSRSumCompliant(void);
uint8_t GetDouSupportError(void) ;
extern  void CheckIsDSTWireBreakSetTruePer500ms(void) ;
extern  void CheckIsDSRWireBreakSetTruePer500ms(void) ;
#if 0
ErrorDouSupport_Type GetDouSupportError(void) ;
StatusDouSupport_Type GetDouSupportStatus(void) ;
#endif
#endif
#endif



#endif
