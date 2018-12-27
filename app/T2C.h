#ifndef __T2C_H
#define __T2C_H

#include "OLC.h"




/*********************define************************/
#define  	T2C_Cmd
#define		T2C_CommonCmd_StartCode							0x3A
#define		T2C_CommonCmd_ValidStrlen						1
#define		T2C_CommonCmd_Strlen							(T2C_CommonCmd_ValidStrlen + 5 )
#define		T2C_EndCode										0X0A

#define		T2C_Config_StartCode							0x3B
#define		T2C_ReadConfig_ValidStrlen						1

#define		T2C_WriteConfig_ValidStrlen						(1 + UserConfigData_Strlen)
#define		T2C_ReadConfig_Strlen							(T2C_ReadConfig_ValidStrlen + 5)
#define		T2C_WriteConfig_Strlen							(T2C_WriteConfig_ValidStrlen + 5 )

//#if((T2C_CommonCmd_Strlen < 2 )||(T2C_ReadConfig_Strlen <2) ||(T2C_WriteConfig_Strlen < 2))
//	#error "T2C.SIF.strlen mustn't smaller than 2"
//#endif
//
//#if(T2C_WriteConfig_Strlen >= T2C_CommonCmd_Strlen)
//	#define		SIF_Strlen									T2C_WriteConfig_Strlen	
//#else
//	#define		SIF_Strlen									T2C_CommonCmd_Strlen
//#endif
#define		SIF_Strlen									T2C_WriteConfig_Strlen	

/*********************AT************************/
#define		T2C_ATConfigBlockStrlen									16


#define		Timer_T2CCmdWait_Per500ms								18	//9s  should bigger than 2s


/*********************typedef************************/
typedef struct
{
	SIFStatus_TypeDef	status ;
	uint8_t			strlen ;
	uint8_t 		bit_index ;
	uint8_t 		byte_index ;
	uint8_t			data_buff ;
	uint8_t 		highlevel_counter ;
	uint16_t 		lowlevel_counter ;
	bool		    flag_100us ;
	uint8_t			Data[SIF_Strlen] ;
	uint8_t 		DataBuff[SIF_Strlen];
}T2CSIF_TypeDef;



typedef enum
{
	T2C_SendCommonDataCmd = 0 ,
	T2C_ReadConfigCmd ,
	T2C_ReadConfigWait ,
	T2C_ReadConfigFinished ,
	//T2C_UpdateCofigFromATCmd ,
	T2C_WriteConfigCmd ,
	T2C_WriteConfigWait ,
	//T2C_ClrWriteConfigCompleteFlag ,
	//T2C_ClrWriteConfigCompleteFlagWait ,
}T2CCmd_TypeDef ;


typedef union
{
	uint8_t 	data[T2C_CommonCmd_Strlen] ;
	struct
	{
		uint8_t nullbyte ;
		uint8_t startcode ;
		uint8_t	strlencode ;
		union
		{
			uint8_t	status ;
			struct
			{
				uint8_t guardstatusBit	:	1 ;
				uint8_t LVlevelBit		:	1 ;
				uint8_t	OneKeyRepairBit :	1 ;
				uint8_t ReserveBit		:	1 ;
				uint8_t ThressTorqueBit	:	2 ;
				uint8_t ThressSpeedBit	: 	2 ;
			} ;
		} ;
		uint8_t	checkcode	;
		uint8_t endcode		;
	} ;
}T2CCommon_TypeDef	;

typedef union
{
	uint8_t 	data[T2C_ReadConfig_Strlen] ;
	struct
	{
		uint8_t nullbyte ;
		uint8_t startcode ;
		uint8_t	strlencode ;
		union
		{
			uint8_t	status ;
			struct
			{
				uint8_t cmd				:	4 ;
				uint8_t ReserveBit		:	4 ;
			} ;
		} ;
		uint8_t	checkcode	;
		uint8_t endcode		;
	} ;
}T2CReadConfig_TypeDef	;

typedef union
{
	uint8_t 	data[T2C_WriteConfig_Strlen] ;
	struct
	{
		uint8_t nullbyte ;
		uint8_t startcode ;
		uint8_t	strlencode ;
		union
		{
			uint8_t	status ;
			struct
			{
				uint8_t cmd				:	4 ;
				uint8_t ReserveBit		:	4 ;
			} ;
		} ;
		UserConfig_TypeDef UserConfig ;
		uint8_t	checkcode	;
		uint8_t endcode		;
	} ;
}T2CWriteConfig_TypeDef	;

typedef union
{
	uint8_t		flag_all ;
	struct
	{
		uint8_t block0	:	1 ;	
		uint8_t block1	:	1 ;
		uint8_t block2	:	1 ;
		uint8_t ReserveBit		:	5 ;
	} ;
	
}BlockWrittenFlag_TypeDef ;




typedef struct
{
	uint8_t				counter_onekeyrepairset_cycle ;
	T2CCommon_TypeDef	CommonData ;
	uint8_t				AT_blocknum ;
	T2CReadConfig_TypeDef ReadConfig ;
	T2CWriteConfig_TypeDef WriteConfig ;
	BlockWrittenFlag_TypeDef BlockWrittenFlag ;
	uint8_t 			timer_cmdwait_per500ms ;
	T2CCmd_TypeDef	cmd ;
	T2CSIF_TypeDef	SIF ;
}T2C_TypeDef ;

typedef union
{
	UserConfig_TypeDef 	UserConfigStruct ;
	uint8_t 			UserConfigArr[UserConfigData_Strlen] ;
}UserConfig_UnionDef;



//typedef struct
//{
//	uint8_t blocknum ;	
//	
//}ConfigController_BLE_TypeDef;
//
//typedef struct
//{
//	uint8_t 		Data[ConfigControll_Strlen];	
//	
//}ConfigController_Contr_TypeDef;



/*******************Variable**********************/
extern T2C_TypeDef T2C ;
extern UserConfig_UnionDef UserConfig ;
extern UserConfig_UnionDef UserConfigAT ;





/***************function******************/
void T2CSIFSend_Service(void) ;
void LoadT2CSIFData(void) ;

void LoadT2CCommonCmdSIFByte(void);
void LoadT2CReadConfigSIFByte(void);
void LoadT2CWriteConfigSIFByte(void);
//void LoadT2CClearWriteConfigCompleteFlagSIFByte(void) ;
void FillT2CSIFData(uint8_t *) ;

void SendT2CSIFBit(void);

UserConfig_TypeDef AdjustOrderOfUserConfig(UserConfig_TypeDef);



#endif
