#include <string.h>
#include "defines.h"
//#include <zeebike1_profile/zeebike1_profile.h>
#include "zeebike1_profile.h"
#include "peripheral.h"
#include "simple_peripheral.h"



#include "ATControl.h"
#include "systemcontrol.h"
#include "debug.h"
#include "_ParameterCal.h"

#include "BatteryPowerCal.h"
#include "DouSupport.h"
#include "gpscontrol.h"
#include "Guard.h"
#include "KeyLess.h"
#include "MechanicalLock.h"
#include "MileageCal.h"
#include "OLC.h"
#include "SeatLock.h"
#include "T2C.h"
#include "FlashSNV.h"
#include "_ParameterCal.h"

#if VOICEPACKETPWM_COMPILE
#include "VoicePacket.h"
#endif




/*************************************************/
/*********************CmdNum*******************/
#define		CmdNum_DeviceInfoSet			0X04
#define		CmdNum_ThreeSpeedSet			0X17
#define		CmdNum_ThreeTorqueSet			0X18
#define	 	CmdNum_ConfigContrE2PROM		0X19
#define		CmdNum_HexTime					0X1B
#define		CmdNum_ResetHC08				0X1C
#define		CmdNum_HeartBeat				0X30




/*****************FreeATCmd**********************/
#if(COUNTRY_COMPILE ==  China_CountryVersion)
const uint8_t Str_ATBLECONNECT[] =		{"AT+BLEConnect"};
const uint8_t Str_ATDEADLINE[] = 		{"AT+DeadLine="};
const uint8_t Str_ATELECTLOCK[] = 		{"AT+ElectLock="};
const uint8_t Str_ATGUARDSTATUS[] = 	{"AT+GuardStatus="};


#elif(COUNTRY_COMPILE == Vietnam_CountryVersion)
const uint8_t Str_ATBLECONNECT[] =		{"VietnamZeebikeV1"};
const uint8_t Str_ATDEADLINE[] = 		{"AT+VNRTime="};
const uint8_t Str_ATELECTLOCK[] = 		{"AT+VNAuto="};
const uint8_t Str_ATGUARDSTATUS[] = 	{"AT+VNLock="};
#else
#endif
const uint8_t Str_ATDEVINFO[] = 		{"AT+DevInfo="};   
const uint8_t Str_ATERRORSTATUS[] = 	{"AT+ErrorStatus=?"};
//const uint8_t Str_ATERRORHISTORY[] = 	{"AT+ErrorHistory="};


const uint8_t Str_ATQ[] = 				{"AT+Q=?"};
const uint8_t Str_ATMILEAGE[] = 		{"AT+Mileage=?"};
const uint8_t Str_ATHALLSPEED[] = 		{"AT+HallSpeed=?"};
const uint8_t Str_ATCONTRCONNECT[] = 	{"AT+ContrConnect=?"};
const uint8_t Str_ATHARDWAREVERSION[] ={"AT+HardwareVersion="};

#if Double_LVSet
const uint8_t Str_ATUNLOCKLEVEL1LV[] =	{"AT+UnlockLevel1LV=1"};
#endif

#if OneKeyRepairSet
const uint8_t Str_ATONEKEYREPAIR[]= 	{"AT+OneKeyRepair=1"} ;
#endif

#if ThreeSpeedSet
const uint8_t Str_ATTHREESPEED[]=		{CmdNum_ThreeSpeedSet,0x00} ;	//for the string end
#endif

#if ThreeTorqueSet
const uint8_t Str_ATTHREETORQUE[]=		{CmdNum_ThreeTorqueSet,0x00} ;
#endif


#if (BUZZER_COMPILE ||   VOICEPACKETPWM_COMPILE)
const uint8_t Str_ATSEARCHEBIKE[] =		{"AT+SearchEbike="};
#endif
#if 0
const uint8_t Str_ATCYCLICSEND[] =		{"AT+CyclicSend="};
const uint8_t Str_ATCFILENUM[] =		{"AT+CFileNum="};
#endif

#if(KeyLessStart == Anti_theftDevice_Function)
	const uint8_t Str_ATKEYLESSSTART[] =	{"AT+FreeKeyStart="};
	#if MECHANICALLOCK_INTERNAL_COMPILE
		const uint8_t Str_ATMECHALLOCK[]=	{"AT+MechalLock="} ;
		#if DOUSUPPORT_INTERNAL_COMPILE
		const uint8_t Str_ATDOUSUPPORT[]=	{"AT+DouSupport="} ;
		#endif
	#endif
#endif

#if SEATLOCK_COMPILE
const uint8_t Str_ATSEATLOCK[] =	{"AT+SeatLock="};
#endif

#if TEST_EXTFLASH_COMPILE
const uint8_t Str_ATTESTEXTFLASH[]={0X1D,0X00} ;
#endif

const uint8_t Str_ATRESETBLE[]={0X1C,0X01};
const uint8_t Str_ATHEARTBEAT[]={0x30 ,0x00 ,0x00} ;


/******************************************************
* Length of string constants
*/
/*********************HC-08 ATCmd*******************/
//#define Strlen_ATNAME               	(sizeof(Str_ATNAME)/sizeof(Str_ATNAME[0]) -1)

/*****************FreeATCmd**********************/
#define Strlen_ATBLECONNECT				(sizeof(Str_ATBLECONNECT)/sizeof(Str_ATBLECONNECT[0])-1)
#define Strlen_ATDEVINFO				(sizeof(Str_ATDEVINFO)/sizeof(Str_ATDEVINFO[0])-1)
#define Strlen_ATDEADLINE				(sizeof(Str_ATDEADLINE)/sizeof(Str_ATDEADLINE[0])-1)
#define Strlen_ATGUARDSTATUS			(sizeof(Str_ATGUARDSTATUS)/sizeof(Str_ATGUARDSTATUS[0])-1)
#define Strlen_ATERRORSTATUS			(sizeof(Str_ATERRORSTATUS)/sizeof(Str_ATERRORSTATUS[0])-1)
//#define Strlen_ATERRORHISTORY			(sizeof(Str_ATERRORHISTORY)/sizeof(Str_ATERRORHISTORY[0])-1)
#define Strlen_ATELECTLOCK				(sizeof(Str_ATELECTLOCK)/sizeof(Str_ATELECTLOCK[0])-1)
#define Strlen_ATQ						(sizeof(Str_ATQ)/sizeof(Str_ATQ[0])-1)
#define Strlen_ATMILEAGE				(sizeof(Str_ATMILEAGE)/sizeof(Str_ATMILEAGE[0])-1)
#define Strlen_ATHALLSPEED				(sizeof(Str_ATHALLSPEED)/sizeof(Str_ATHALLSPEED[0])-1)
#define Strlen_ATCONTRCONNECT			(sizeof(Str_ATCONTRCONNECT)/sizeof(Str_ATCONTRCONNECT[0])-1)
#define Strlen_ATHARDWAREVERSION		(sizeof(Str_ATHARDWAREVERSION)/sizeof(Str_ATHARDWAREVERSION[0])-1)

#if Double_LVSet
#define Strlen_ATUNLOCKLEVEL1LV			(sizeof(Str_ATUNLOCKLEVEL1LV)/sizeof(Str_ATUNLOCKLEVEL1LV[0])-1)
#endif

#if OneKeyRepairSet
#define Strlen_ATONEKEYREPAIR			(sizeof(Str_ATONEKEYREPAIR)/sizeof(Str_ATONEKEYREPAIR[0])-1)
#endif

#if ThreeSpeedSet
#define Strlen_ATTHREESPEED 			(sizeof(Str_ATTHREESPEED)/sizeof(Str_ATTHREESPEED[0])-1)
#endif

#if ThreeTorqueSet
#define	Strlen_ATTHREETORQUE 			(sizeof(Str_ATTHREETORQUE)/sizeof(Str_ATTHREETORQUE[0])-1)
#endif


#if (BUZZER_COMPILE ||   VOICEPACKETPWM_COMPILE)
#define	Strlen_ATSEARCHEBIKE			(sizeof(Str_ATSEARCHEBIKE)/sizeof(Str_ATSEARCHEBIKE[0])-1)
#endif
#if 0
#define Strlen_ATCYCLICSEND				(sizeof(Str_ATCYCLICSEND)/sizeof(Str_ATCYCLICSEND[0])-1)
#define	Strlen_ATCFILENUM				(sizeof(Str_ATCFILENUM)/sizeof(Str_ATCFILENUM[0])-1)
#endif

#if(KeyLessStart == Anti_theftDevice_Function)	
#define Strlen_ATKEYLESSSTART				(sizeof(Str_ATKEYLESSSTART)/sizeof(Str_ATKEYLESSSTART[0])-1)
	#if MECHANICALLOCK_INTERNAL_COMPILE
#define	Strlen_ATMECHALLOCK				(sizeof(Str_ATMECHALLOCK)/sizeof(Str_ATMECHALLOCK[0])-1)
		#if DOUSUPPORT_INTERNAL_COMPILE
#define	Strlen_ATDOUSUPPORT				(sizeof(Str_ATDOUSUPPORT)/sizeof(Str_ATDOUSUPPORT[0])-1)
		#endif
	#endif	
#endif
	
#if SEATLOCK_COMPILE
#define	Strlen_ATSEATLOCK				(sizeof(Str_ATSEATLOCK)/sizeof(Str_ATSEATLOCK[0])-1)
#endif

#if TEST_EXTFLASH_COMPILE
#define	Strlen_ATTESTEXTFLASH				(sizeof(Str_ATTESTEXTFLASH)/sizeof(Str_ATTESTEXTFLASH[0]))
#endif

#define	Strlen_ATHeartBeat				(sizeof(Str_ATHEARTBEAT)/sizeof(Str_ATHEARTBEAT[0]))

#define Strlen_ATResetBLE               (sizeof(Str_ATRESETBLE)/sizeof(Str_ATRESETBLE[0]))
/*********************HC-08 ATCmd*******************/
#define		Strlen_Data_Deadline			( 2 + 1 + 1 + 1 )
#define     Strlen_Data_DevInfo         	( 1 + 2 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 )
#define		Strlen_Mileage_Max				4

							

#define		ThreeSpeed_QAndS_ByteNum		(2 - 1)
#define		ThreeSpeed_SetData_ByteNum		(3 - 1)
#define		ThreeTorque_QAndS_ByteNum		(2 - 1)
#define		ThreeTorque_SetData_ByteNum		(3 - 1)

							
/*****************BLE_ATCmdResponseCode**************/
#define		RCODEWithData_ErrorATCmd			0XE0
#define		RCODEWithData_BLEConnect			0X01
#define		RCODEWithData_DevInfo			    0X04  
#define		RCODEWithData_DeadLine				0X02
#define		RCODEWithData_GuardStatus			0X03					
#define		RCODEWithData_ErrorStatus			0X05
#define		RCODEWithData_Q 					0X06
#define		RCODEWithData_Mileage			    0X07  
#define		RCODEWithData_ErrorHistory			0X08
#define		RCODEWithData_CyclicSend			0X09
#define		RCODE_ReceiveNameSuccess			0X0A
#define		RCODEWithData_HallSpeed				0X0B
#define		RCODEWithData_ControllerConnect		0X0C
#define		RCODEWithData_HardwareVersion		0X0E
#define		RCODEWithData_ElectLock				0X0F
#define		RCODEWithData_UnLockLevel1LV		0X0D
#define		RCODEWithData_SearchEbike			0X10
							
#define		RCODEWithData_KeyLessStart			0X11
#define		RCODEWithData_SeatLock				0X12
#define		RCODEWithData_DouSupport			0X13
#define		RCODEWithData_MechalLock			0X14							
#define		RCODEWithData_AllParkFinish			0X15
#define		RCODEWithData_CFileNum				0X16
#define		RCODEWithData_ThreeSpeed			0X17
#define		RCODEWithData_ThreeTorque			0X18
#define		RCODEWithData_ContrE2Cfg			0X19
							
#define		RCODEWithData_OneKeyRepair			0X1A
#define		RCODEWithData_HexTime				0X1B
#define		RCODEWithData_ResetHC08				0X1C
#define		RCODEWithData_TestExtFlash			0X1D
#define		RCODEWithData_HeartBeat				0X30							
							
							
/*************BLE_ATCmdResponse Strlen******************/
#define     StrlenWithData_ErrorATCmd           (1 + 1)
#define     StrlenWithData_BLEConnect           1
#define		StrlenWithData_GuardStatus			(1 + 1)
#define     StrlenWithData_DevInfo              (1 + 1 + Strlen_Data_DevInfo )
#define     StrlenWithData_DevInfoSet			(1 + 1 + 1) 
#define		StrlenWithData_ElectLock			(1 + 1)
#define		StrlenWithData_ErrorStatus			(1 + 1)
#define		StrlenWithData_ErrorHistory			(1 + 1 + 1)
#define		StrlenWithData_ErrorHistoryDetail	(1 + 1 + RunMessageSingleEntryLen)	
#define		StrlenWithData_DeadLine				(1 + 1 + Strlen_Data_Deadline)					
#define		StrlenWithData_Q			        (1 + 4 + 4 + 4)
#define		StrlenWithData_Mileage				(1 + 4 + 4 + 4 + 4)
#define 	StrlenWithData_HallSpeed			(1 + 2)
#define     StrlenWithData_ControllerConnect	(1 + 1)
#define     StrlenWithData_BLERename           	1	
#define		StrlenWithData_ResetHC08			(1 + 1)
#define		StrlenWithData_TestExtFlash			(1 + 1 + 1)
#define 	StrlenWithData_HardwareVersion		(1 + 2 + 3)
#define		StrlenWithData_HexTime				(1 + 1 + 13)
#define 	StrlenWithData_UnLockLevel1LV		(1 + 1)
#define		StrlenWithData_OneKeyRepair			(1 + 1)
#define		StrlenWithData_SearchEbike			(1 + 1)
#define		StrlenWithData_CyclicSend			(1 + 1 + 4 + 4 + 2 + 2 + 2)
#define		StrlenWithData_HeartBeatSend		(1 + 1 )
							
#define		StrlenWithData_KeyLessStatusChanged	(1 + 1)
#define		StrlenWithData_SeatLock				(1 + 1)
#define		StrlenWithData_DouSupport			(1 + 1 + 1)
#define		StrlenWithData_MechalLock			(1 + 1)							
#define		StrlenWithData_AllParkFinish		(1 + 1)
#define		StrlenWithData_CFileNum				(1 + 1 + 5 )		
#define		StrlenWithData_ThreeSpeed			(1 + 1 + 1)
#define		StrlenWithData_ThreeTorque			(1 + 1 + 1)
							
/*************ATCmd Error Define*****************/ 
#define    	ErrorATCmd_Unrecognized             0X01
#define     ErrorATCmd_ErrorFormat              0X02
#define     ErrorATCmd_ConncetCmdNotYetSent     0X03
#define     ErrorATCmd_ConncetCmdSendRepeatedly 0X04  

/*************ATCmdByte*****************/ 
#define    	ATCmdByte_Cmd            			0X00
#define    	ATCmdByte_Action           			0X01

#define		ATCmdByte_Status					0X02
#define		ATCmdByte_BlockNum					0X02

/*************ATActionCode*****************/ 
#define    	ATActionCode_Query            		0X00
#define    	ATActionCode_Config_Defalt          0X01

#define		ATActionCode_Config_Second			0X02
						
/*************ATCmd ConnectWithController Define*****************/ 
#define         NoConnectWithController         0x00
#define         ConnectWithController           0x01
							
/*************ATCmd Set Define*****************/ 
#define			ExecutionWait					0X02

									
/*************ATCmd Inquiry Define*****************/ 
#define         ReplyInquiry     				0x00

/*************Heart Beat*****************/ 
#define		Timer_HeartBeatSendInterval_Per500ms	20	//10s
#define		Timer_HeartBeat_TimeOut_Per500ms		64//32s


/*************Variable*****************/ 
ArrInfo_TypeDef BLEResponse ;
AT_TypeDef AT ;

static uint8_t timer_heartbeat_send_per500ms = Timer_HeartBeatSendInterval_Per500ms ;
static uint8_t timer_heartbeat_outtime_per500ms = Timer_HeartBeat_TimeOut_Per500ms ;
	
/*************Function*****************/ 
static void ResponseErrorATCmd(void);						
static void ResponseErrorATCmdErrorFormat(void) ;
static void ResponseErrorATCmdConncetCmdNotYetSent(void);
static void ResponseErrorATCmdConncetCmdSendRepeatedly(uint8_t* para);
static void ResponseATBLEConnect(void);
static void FillATDevInfoDataMessageAndSend(void);
static void ResponseATDevInfo(uint8_t *para) ;
#if 0
static bool IsATDevInfoSetFormatCorrect(uint8_t *para) ;
#endif
static void ResponseATRentDeadline(uint8_t *para) ;
static void ResponseDeadLineInquire(void) ;
static void ResponseDeadLineInquire(void) ; 
static void ResponseATGuardStatus(uint8_t *para) ;

static void ResponseConntrollerConnectCheck(uint8_t* para) ;
static void ResponseATElectLock(uint8_t* para) ;
static void CheckElectLockAndResponse(void) ;
static void CheckElectLockAndResponse(void) ;

#if (BUZZER_COMPILE ||   VOICEPACKETPWM_COMPILE)
static void ResponseATSearchEbike(uint8_t* para) ;
#endif

#if(Commu_Mode_Common != Commu_Mode)
static void ResponseATErrorStatus(uint8_t* para) ;

static void ResponseATQ(uint8_t* para) ;
	#if(Commu_Mode_TwoWire == Commu_Mode)
		#if(COMPILE_ENABLE == OneKeyRepairSet)
static void ResponseOneKeyRepair(uint8_t* para) ;
		#endif
		#if(COMPILE_ENABLE == Double_LV)
static void ResponseLV1LevelUnlockSet(uint8_t* para) ;
		#endif
		#if(COMPILE_ENABLE == ThreeSpeedSet)
static void ResponseThreeSpeed(uint8_t* para) ;
static void ResponseThreeSpeedQuery(void) ;
static void ResponseThreeSpeedConfig(void) ;
		#endif
		#if(COMPILE_ENABLE == ThreeTorqueSet)
static void ResponseThreeTorque(uint8_t* para) ;
static void ResponseThreeTorqueQuery(void) ;
static void ResponseThreeTorqueConfig(void) ;
		#endif
	#endif
#endif
static void ResponseATMileage(uint8_t* para) ;
static void ResponseATHallSpeed(uint8_t* para) ;
static void ResponseHardwareVersion(uint8_t* para) ;
//static void ResponseHexTime(uint8_t* para) ;
#if 0
static void ResponseCustomerFileNum(uint8_t* para) ;
static void ResponseATCyclicSend(uint8_t* para) ;
static void CyclicSendData(uint8_t* para) ;
static void CloseCyclicSend(void) ;
#endif


#if SEATLOCK_COMPILE
static void ResponseSeatLock(uint8_t* para) ;
static void ResponseSeatLockAction(void) ;
#endif

#if TEST_EXTFLASH_COMPILE
static void ResponseTestExtFlash(uint8_t* para) ;
#endif


#if HEARTBEAT_COMPILE
static void ResponseATHeartBeat(uint8_t* para) ;
static void FillBLEHeartBeatData(void) ;
void ClearHeartBeatTimer(void) ;
#endif



/*************************************************
 * Compare string & response string function pairs
 */
const ResponseStrategy_TypeDef resp_strategy_table[] = {
		Str_ATBLECONNECT, Strlen_ATBLECONNECT, ResponseErrorATCmdConncetCmdSendRepeatedly,//,ResponseATBLEConnect
		Str_ATDEVINFO, Strlen_ATDEVINFO,	ResponseATDevInfo,
		Str_ATDEADLINE,	Strlen_ATDEADLINE,	ResponseATRentDeadline,
		Str_ATELECTLOCK, Strlen_ATELECTLOCK, ResponseATElectLock,
		Str_ATGUARDSTATUS, Strlen_ATGUARDSTATUS, ResponseATGuardStatus,
#if (BUZZER_COMPILE ||   VOICEPACKETPWM_COMPILE)
		Str_ATSEARCHEBIKE, Strlen_ATSEARCHEBIKE, ResponseATSearchEbike,
#endif
		
#if(KeyLessStart == Anti_theftDevice_Function)
		Str_ATKEYLESSSTART, Strlen_ATKEYLESSSTART, ResponseKeyLessStart,
	#if MECHANICALLOCK_INTERNAL_COMPILE
		Str_ATMECHALLOCK,Strlen_ATMECHALLOCK,ResponseMechalMotorLock ,
		#if DOUSUPPORT_INTERNAL_COMPILE
		Str_ATDOUSUPPORT, Strlen_ATDOUSUPPORT, ResponseDouSupport,
		#endif
	#endif
#endif
		
#if SEATLOCK_COMPILE
		Str_ATSEATLOCK, Strlen_ATSEATLOCK, ResponseSeatLock,	
#endif
		
#if(Commu_Mode_Common != Commu_Mode)
		Str_ATERRORSTATUS, Strlen_ATERRORSTATUS, ResponseATErrorStatus,
		Str_ATQ, Strlen_ATQ, ResponseATQ,
		//Str_ATERRORHISTORY, Strlen_ATERRORHISTORY, ResponseATErrorHistory,
	#if(Commu_Mode_TwoWire == Commu_Mode)
		#if(COMPILE_ENABLE == OneKeyRepair)
		Str_ATONEKEYREPAIR, Strlen_ATONEKEYREPAIR, ResponseOneKeyRepair,
		#endif
		#if(COMPILE_ENABLE == Double_LV)
		Str_ATUNLOCKLEVEL1LV, Strlen_ATUNLOCKLEVEL1LV, ResponseLV1LevelUnlockSet,
		#endif
		#if(COMPILE_ENABLE == ThreeSpeedSet)
		Str_ATTHREESPEED,	Strlen_ATTHREESPEED,		ResponseThreeSpeed,
		#endif
		#if(COMPILE_ENABLE == ThreeTorqueSet)
		Str_ATTHREETORQUE,	Strlen_ATTHREETORQUE,		ResponseThreeTorque,
		#endif
		
	#endif
#endif
		Str_ATMILEAGE, Strlen_ATMILEAGE, ResponseATMileage,
		Str_ATHALLSPEED, Strlen_ATHALLSPEED, ResponseATHallSpeed,
		Str_ATCONTRCONNECT, Strlen_ATCONTRCONNECT, ResponseConntrollerConnectCheck,
		Str_ATHARDWAREVERSION, Strlen_ATHARDWAREVERSION, ResponseHardwareVersion,
#if 0		
		Str_ATCFILENUM,Strlen_ATCFILENUM,ResponseCustomerFileNum,
		Str_ATCYCLICSEND, Strlen_ATCYCLICSEND, ResponseATCyclicSend ,
#endif	

#if TEST_EXTFLASH_COMPILE
		Str_ATTESTEXTFLASH,Strlen_ATTESTEXTFLASH,ResponseTestExtFlash,
#endif	
		Str_ATRESETBLE, Strlen_ATResetBLE,ResponseResetBle,
		Str_ATHEARTBEAT,Strlen_ATHeartBeat,ResponseATHeartBeat
		//Str_ATNAME, Strlen_ATNAME, ResponseAndSetChangeBLEModuleName
};

/*************************************************/
/*******************Variable**********************/
//AT_TypeDef AT ;
ArrInfo_TypeDef BLEResponse = {0 ,{0}};
__root const unsigned char hextimeconst[21]= __DATE__" " __TIME__;






/****************************************************/
/**********************function**********************/
/**
 * Main application is responsible for calling this function when
 * Disconnected from the BLE App
 */
//void processDisconnectWithBLEApp()
//{
//	CloseCyclicSend();
//	AT.status_BLEconnect = false;
//	/*当App连接断开时，下次上电门锁不可以直接解锁*/
//	Guard.status_unguardrecoverable_whenstayAppconnected = false ;
//}

/******************************************************
 * Current received AT command total Length
 */
static uint8_t currentCommandLength = 0;

/********************************************************
 * Calculate and return the length of the parameter end with 0x00
 */
uint8_t GetCurrentCommandLength()
{
	return currentCommandLength;
}

/********************************************************
 * 
 */
uint8_t getLengthOfAsynchronousNotify(void)
{
	return BLEResponse.length;
}

/********************************************************
 * 
 */
void * getHandleOfAsynchronousNotify(void)
{
	/*void *p ;
	p = (void *)(&BLEResponse.arr) ;*/
	return  BLEResponse.arr ;
	//return p ;	
}



/********************************************************
 *	return a pointer to the response string
 */
bool ATControl(uint8_t* in,const uint8_t len) 
{
	//ArrInfo_TypeDef arrInfo;
	bool flag = true ;
	currentCommandLength = len;

//Don't delete the code of this section
//this code is for debug	
#if DEBUG_COMPILE	
	//use test_ATRes to watch *in
	for(uint8_t i = 0 ; i< currentCommandLength; i++)
		test_ATRes[i] = *(in+ i) ;	
#endif
	
	//check if the connect command is received
	if(BLE_ConnectButNoneInteractive == GetBLEConnectStatus())
	{
		//check if the incoming string is the connect command
		if (strncmp((const char *)Str_ATBLECONNECT, (char *)in, Strlen_ATBLECONNECT) == 0)
		{
			ClearHeartBeatTimer();
			ResponseATBLEConnect();		
			return flag ;
			
		} 
		else 
		{
			 ResponseErrorATCmdConncetCmdNotYetSent();
			 return flag ;
		}
	}
	for (uint8_t i = 0; i < sizeof(resp_strategy_table) / sizeof(ResponseStrategy_TypeDef); i++) 
	{
		if (strncmp((char *)resp_strategy_table[i].compare_str, (char *)in, resp_strategy_table[i].compare_str_len) == 0)//compare matches
		{
			//arrInfo = resp_strategy_table[i].ResponseStr(in+resp_strategy_table[i].compare_str_len);	// para is data after fixed string
			//return arrInfo;
			ClearHeartBeatTimer();
			resp_strategy_table[i].ResponseStr(in+resp_strategy_table[i].compare_str_len);	// para is data after fixed string
			return flag ;
			
		}
	}
	//no match, return error code
	ResponseErrorATCmd();
	flag = false ;
	return flag ;
	//apend the NULL char to the array
}

/*******************************************************************************
* 
*/
static void  ResponseErrorATCmd(void)
{    
    BLEResponse.length = StrlenWithData_ErrorATCmd ;
	BLEResponse.arr[0] = RCODEWithData_ErrorATCmd ;
	BLEResponse.arr[1] = ErrorATCmd_Unrecognized ;
    
#if((1 + 1) !=StrlenWithData_ErrorATCmd)
	#error "StrlenWithData_ErrorATCmd is wrong"
#endif

}


/*******************************************************************************
* 
*/
static void ResponseErrorATCmdErrorFormat(void)
{
    BLEResponse.length = StrlenWithData_ErrorATCmd ;
	BLEResponse.arr[0] = RCODEWithData_ErrorATCmd ;
	BLEResponse.arr[1] = ErrorATCmd_ErrorFormat ;
	
#if((1 + 1) !=StrlenWithData_ErrorATCmd)
	#error "StrlenWithData_ErrorATCmd is wrong"
#endif
}

/*******************************************************************************
* 
*/
static void ResponseErrorATCmdConncetCmdNotYetSent(void)
{
    //uint8_t *  arr = (uint8_t*)malloc(StrlenWithData_ErrorATCmd * sizeof(uint8_t));
	BLEResponse.length = StrlenWithData_ErrorATCmd ;
	BLEResponse.arr[0] = RCODEWithData_ErrorATCmd ;
	BLEResponse.arr[1] = ErrorATCmd_ConncetCmdNotYetSent ;
#if((1 + 1) !=StrlenWithData_ErrorATCmd)
	#error "StrlenWithData_ErrorATCmd is wrong"
#endif
}

/*******************************************************************************
* 
*/
static void ResponseErrorATCmdConncetCmdSendRepeatedly(uint8_t* para)
{
//Log_info0("Calling ResponseErrorATCmdConncetCmdSendRepeatedly.");
	BLEResponse.length = StrlenWithData_ErrorATCmd ;
	BLEResponse.arr[0] = RCODEWithData_ErrorATCmd ;
	BLEResponse.arr[1] = ErrorATCmd_ConncetCmdSendRepeatedly ;


#if((1 + 1) !=StrlenWithData_ErrorATCmd)
	#error "StrlenWithData_ErrorATCmd is wrong"
#endif
}
/*******************************************************************************
*
*/
static void ResponseATBLEConnect(void)
{
	BLEResponse.length = StrlenWithData_BLEConnect ;
	BLEResponse.arr[0] = RCODEWithData_BLEConnect ;
	
#if((0 + 1) !=StrlenWithData_BLEConnect)
	#error "StrlenWithData_BLEConnect is wrong"
#endif
	//Flag it as connected before reacting to other commands
	ChangeBLEConnectStatus(BLE_ConnectWithInteractive);
	//PIN_setOutputValue(GetGpioPinHandle(), Pin_BLE_Connect , High) ;
	
	
}

/*******************************************************************************
*
*/
static void FillATDevInfoDataMessageAndSend(void)
{
	//int8_t arr[StrlenWithData_DevInfo];
	BLEResponse.length = StrlenWithData_DevInfo ;
	BLEResponse.arr[0] = RCODEWithData_DevInfo ;
	BLEResponse.arr[1] = 0X00 ;
	BLEResponse.arr[2] = Commu_Mode ;
	BLEResponse.arr[3] = Gear_Ratio_Motor_InFlash ; //DevicePara.gear_ratio_motor ;//
	BLEResponse.arr[4] = Gear_Ratio_Wheel_InFlash ;//DevicePara.gear_ratio_wheel ;//
	BLEResponse.arr[5] = Number_Pole_Pairs_InFlash ;//DevicePara.number_pole_pairs ;//
	BLEResponse.arr[6] = Wheel_Diameter_Inch_Int_InFlash ;//DevicePara.wheel_diameter_inch_int ;//
	BLEResponse.arr[7] = AH_Battery_InFlash ;//DevicePara.battery_aH_battery ;//

	BLEResponse.arr[8] = 0X00 ;
	if(COMPILE_ENABLE == Double_LVSet)
		BLEResponse.arr[8] |= U8_BIT0 ;
	if(COMPILE_ENABLE == OneKeyRepairSet)
		BLEResponse.arr[8] |= U8_BIT1 ;
	if(COMPILE_ENABLE == ThreeSpeedSet)
		BLEResponse.arr[8] |= U8_BIT2 ;
	if(COMPILE_ENABLE == ThreeTorqueSet)
		BLEResponse.arr[8] |= U8_BIT3 ;
			
	BLEResponse.arr[9] = Wheel_Diameter_Inch_Dec_InFlash ;//DevicePara.wheel_diameter_inch_dec ;
	BLEResponse.arr[10] = Battery_Materials_InFlash ;//DevicePara.battery_materialsAndmanufact ;
	BLEResponse.arr[11] = MnNiCo_Battery_Section_Num ;//DevicePara.battery_section_num ;
	BLEResponse.arr[12] = Battery_Materials_InFlash ;//DevicePara.batterystandardKM ;
	BLEResponse.arr[13] = Rated_Q_Release_PerFrame_A_InFlash ;//DevicePara.rated_Q_release_perFrame_A ;

#if(KeyLessStart == Anti_theftDevice_Function) 
	#if DOUSUPPORT_EXTERNAL_COMPILE
	BLEResponse.arr[14] = 0X04 ;
	#else
		#if MECHANICALLOCK_EXTERNAL_COMPILE
	BLEResponse.arr[14] = 0X03 ;
		#else
	BLEResponse.arr[14] = 0X02 ;
		#endif
	#endif
#else	
	BLEResponse.arr[14] = Anti_theftDevice_Function ;
#endif	
		      
#if((14 + 1) !=StrlenWithData_DevInfo)
	#error "StrlenWithData_DevInfo is wrong"
#endif
   
}
/*******************************************************************************
*
*/
static void ResponseATDevInfo(uint8_t *para)
{
	if(ASCII_QuestionMark == *para)
		FillATDevInfoDataMessageAndSend();	
	/*else if(ASCII_Num_0 == *para)
	{
		if(true == IsATDevInfoSetFormatCorrect(para))
		{
			DevicePara.gear_ratio_motor = Gear_Ratio_Motor_InFlash;
			DevicePara.gear_ratio_wheel = Gear_Ratio_Wheel_InFlash;
			DevicePara.number_pole_pairs = Number_Pole_Pairs_InFlash;
			DevicePara.wheel_diameter_inch_int = Wheel_Diameter_Inch_Int_InFlash;
			DevicePara.battery_aH_battery = AH_Battery_InFlash;
			DevicePara.wheel_diameter_inch_dec = Wheel_Diameter_Inch_Dec_InFlash ;
			DevicePara.battery_materialsAndmanufact = Battery_Materials_InFlash ;
			if(Battery_Materials_InFlash == Lead_Acid_Batteries)
				DevicePara.battery_section_num = Lead_Acid_Battery_Section_Num ;
			else
				DevicePara.battery_section_num = MnNiCo_Battery_Section_Num;
			DevicePara.batterystandardKM = BatteryStandardKM_InFlash ;
			DevicePara.rated_Q_release_perFrame_A = Rated_Q_Release_PerFrame_A_InFlash ;
			Mileage.caledvalue_hallchange_1KM = CalCounterHallchange1KM();
			
			FillATDevInfoDataMessageAndSend();
		}	
	}*/
    else
        ResponseErrorATCmdErrorFormat();
	
}

#if 0
/*******************************************************************************
*
*/
static bool IsATDevInfoSetFormatCorrect(uint8_t *para)
{
	//长度是否够
    if(GetCurrentCommandLength()!= ( Strlen_Data_DevInfo + Strlen_Mileage_Max + 1 ))
        return false ;
	
	uint8_t gear_motor ;
	uint8_t gear_wheel ;
	gear_motor = *(para + 1) ;
	gear_wheel = *(para + 2) ;
	
	//如果是三轮车，极对数必须是4
	if(gear_motor > gear_wheel)
	{
		if(Pole_Pairs_Tricycle_Standard != *(para + 3))	
			return false ;
	}
	//如果是二轮车，极对数必须大于等于23 小于50
	else if(gear_motor == gear_wheel )
	{
		if(*(para + 3) < Pole_Pairs_TwoWheel_Min)
			return false ;
		if(*(para + 3) > Pole_Pairs_TwoWheel_Max)
			return false ;
		
	}
    else//电机齿轮变速比必须大于等于后轮变速比
		return false ;
	
	//轮径的整数部分应该不小于16英寸 不大于40
	if(*(para + 4) < Wheel_Diameter_Inch_Int_Min)
		return false ;
	if(*(para + 4)  > Wheel_Diameter_Inch_Int_Max)
		return false ;

	//轮径的小数部分不能大于等于100
	if(*(para + 7)  >= ONE_HUNDRED)
		return false ;
	
	//安时数必须不小于12AH
	if(*(para + 5) < AH_Battery_Min )	
        return false ;
	
	//只有带双向控制的硬件才能设置是否有双欠压、一键修复、三速控制功能
	if(Commu_Mode_TwoWire != Commu_Mode )
	{
		if(*(para + 6) > 0 )
			return false ;
		
	}
	
	//电池材料应该在铅酸和三元锂中选择
	if(*(para + 8)  > 0X01)
		return false ;

	if(0X00 == *(para + 8))	//Lead_Acid_Batteries
	{
		if((LV_Grade / 12) != *(para + 9))
			return false ;
	}
	else if(0X01 == *(para + 9))	//MnNiCo_Ternary_Battery
	{
		uint8_t sectionnum[2] ;
#if(36 == LV_Grade)
		sectionnum[0] = 10 ;		/*conventional*/
		sectionnum[1] = 10 ;		/*unconventional*/		
#elif(48 == LV_Grade)
		sectionnum[0] = 13 ;		/*conventional*/
		sectionnum[1] = 14 ;		/*unconventional*/
#elif(60 == LV_Grade)
		sectionnum[0] = 16 ;		/*conventional*/
		sectionnum[1] = 17 ;		/*unconventional*/
#elif(72 == LV_Grade)
		sectionnum[0] = 20 ;		/*conventional*/
		sectionnum[1] = 20 ;		/*unconventional*/
#endif
		for(uint8_t i = 0 ;i < 2 ;i++)
		{
			if(*(para + 9) != sectionnum[i])
				return false ;
		}
	}
	//防盗方式应该在已有的方式中选择
	//if(Reveive_Usart1ToDMA.Data[Anti_theftDevice_Function_RecArrNum] > ElectricDouSupport)
		//return false ;
	
	return true ; 
}
#endif

/*******************************************************************************
*
*/
static void ResponseATRentDeadline(uint8_t *para)
{		
    if(ASCII_QuestionMark == *para)
        ResponseDeadLineInquire();
    else if((GetCurrentCommandLength() >= (Strlen_ATDEADLINE + Strlen_Data_Deadline) ) &&\
		(true == IsDeadLineFormatCorrect( (para + 2),3)))
    {
		Guard.deadlinedata.timerleft_day.h8 = *(para);
        Guard.deadlinedata.timerleft_day.l8 = *(para + 1 );
		for(uint8_t i = 2 ; i < 5 ; i ++)
			Guard.deadlinedata.data[i] = *(para + i );
		
		BLEResponse.length = StrlenWithData_DeadLine ;
		BLEResponse.arr[0] = RCODEWithData_DeadLine ;
		BLEResponse.arr[1] = SET_SUCCESS_U8 ;

		//用0补全剩余的所有byte
		for(uint8_t i= 2 ;i < StrlenWithData_DeadLine ;i++ )
			BLEResponse.arr[i] =  0 ;		    
    }
    else
	{
#if 0
//Don't delete the code of this section
//this code is for debug		
	//use test_ATRes to watch *para
	for(uint8_t i = 0 ; i< currentCommandLength; i++)
		test_ATRes[i] = *(para+ i) ;	
#endif
		ResponseErrorATCmdErrorFormat();
	}
}

/*******************************************************************************
*
*/
static void ResponseDeadLineInquire(void)
{
	BLEResponse.length = StrlenWithData_DeadLine ;
	BLEResponse.arr[0] = RCODEWithData_DeadLine ;
	BLEResponse.arr[1] = ReplyInquiry ;
    BLEResponse.arr[2]= Guard.deadlinedata.timerleft_day.h8 ;
    BLEResponse.arr[3]= Guard.deadlinedata.timerleft_day.l8 ;
#if 0	
    BLEResponse.arr[4]= Guard.timer_hour_guard_timeleft ;
    BLEResponse.arr[5]= Guard.timer_minute_guard_timeleft ;
    BLEResponse.arr[6]= Guard.timer_second_guard_timeleft ;
#endif

	for(uint8_t i = 2 ; i < 5 ; i ++)
		BLEResponse.arr[2 + i] = Guard.deadlinedata.data[i] ;
    
#if((6 + 1) !=StrlenWithData_DeadLine)
#error "StrlenWithData_DeadLine is wrong"
#endif
}

/*******************************************************************************
*
*/
static void ResponseATGuardStatus(uint8_t *para)
{
    if(ASCII_QuestionMark == *para)
        CheckGuardStatusAndResponse(0XFF);
    else if(ASCII_Num_0 == *para )
    {	
        Guard.cmd_lock_From_BLE = Unlock_Ebike ;
        
        /*褰揂T+GuardStatus鍛戒护鍙樺寲鏃讹紝涓嬫涓婄數闂ㄩ攣涓嶅彲浠ョ洿鎺ヨВ閿?**/
		Guard.status_unguardrecoverable_whenstayAppconnected = false ;	 /*褰揂T+GuardStatus鍛戒护鍙樺寲鏃讹紝涓嬫涓婄數闂ㄩ攣涓嶅彲浠ョ洿鎺ヨВ閿?**/
        //Guard.status_ATCmd_response = true ;	// Flag AT guardCmd response Enable
		//BLEResponse.length =  0 ;
#if(KeyLessStart == Anti_theftDevice_Function)
		if(Guard.status != Status_GuardProcessActing)
			KeyLessPowerOn();
#endif

		CheckGuardStatusAndResponse(0);
		
    }
    else if( ASCII_Num_1 == *para)
    {
	//	#if PLUS_OBSERVER
	//    SimpleBLECentral_startDiscHandler(); //开启扫描蓝牙道钉
     //   #else
        Guard.cmd_lock_From_BLE = Lock_Ebike ;
		Guard.status_unguardrecoverable_whenstayAppconnected = false ;
		CheckGuardStatusAndResponse(0X01);
    //    #endif
        /*褰揂T+GuardCmd鍙樺寲鏃讹紝涓嬫涓婄數闂ㄩ攣涓嶅彲浠ョ洿鎺ヨВ閿?**/  //当App连接没有断时，下次上电门锁可以直接解锁
			
		
      
    }	
    else
    {
        ResponseErrorATCmdErrorFormat();
    }
}

/*******************************************************************************
*
*/
void CheckGuardStatusAndResponse(uint8_t status)
{
	Guard.status_guardreason = CheckGuardReason();
	BLEResponse.length = StrlenWithData_GuardStatus ;
	BLEResponse.arr[0] = RCODEWithData_GuardStatus ;
    //if(Status_UnGuarded == Guard.status)
   	if(0 == status)
        BLEResponse.arr[1]= 0X00 ;
    else
        BLEResponse.arr[1]= Guard.status_guardreason;
    
#if((1 + 1) !=StrlenWithData_GuardStatus)
    #error "StrlenWithData_GuardStatus is wrong "
#endif

}

/*******************************************************************************
* 函数名        :
* 函数描述    	:
* 输入参数      : None
* 输出结果      : None
* 返回值        : None
*******************************************************************************/
void ATCmdGuardResponseWhenGuardStatusUnchange(void)
{
	if(true == Guard.status_ATCmd_response)
	{
		if(IsBLEConnectStatusInteractive())
		{
			CheckGuardStatusAndResponse(0XFF);
			SimpleBLEPeripheral_enqueueAsySendMsg() ;
		}
		Guard.status_ATCmd_response = false ;
	}
	else
		;
}

/*******************************************************************************
*
*/
static void ResponseConntrollerConnectCheck(uint8_t* para)
{
	BLEResponse.length = StrlenWithData_ControllerConnect ;
	BLEResponse.arr[0] = RCODEWithData_ControllerConnect ;
	
	if(true == BLEMdlCnt.status_connect2controller)
		BLEResponse.arr[1]= ConnectWithController ;
    else
		BLEResponse.arr[1]= NoConnectWithController ;

#if((1 + 1) !=StrlenWithData_ControllerConnect)
    #error "StrlenWithData_ControllerConnect is wrong"
#endif    
	
}


/*******************************************************************************
*
*/
static void ResponseATElectLock(uint8_t* para)
{
    if(ASCII_QuestionMark == *para)
        CheckElectLockAndResponse();
    else if(ASCII_Num_0 == *para )
    {
        if(Status_Auto == Guard.status_autoguard )
        {
            Guard.cmd_lock_From_BLE = Unlock_Ebike ;
            Guard.status_autoguard = Status_UnAuto ;
        }
        else
            ;
        CheckElectLockAndResponse();	
    }
    else if( ASCII_Num_1 == *para)
    {
        if(Status_UnAuto == Guard.status_autoguard )
        {
            if(Status_Guarded == Guard.status )
                Guard.cmd_lock_From_BLE = Lock_Ebike ;
            Guard.status_autoguard = Status_Auto ;
        }
        else	
            ;		
        CheckElectLockAndResponse();
    }
    else
        ResponseErrorATCmdErrorFormat();
}

/*******************************************************************************
*
*/
static void CheckElectLockAndResponse(void)
{
	BLEResponse.length = StrlenWithData_ElectLock ;
	BLEResponse.arr[0] = RCODEWithData_ElectLock ;
    BLEResponse.arr[1]= (uint8_t)(Guard.status_autoguard);
    
 #if((1 + 1) != StrlenWithData_ElectLock)
    #error "StrlenWithData_ElectLock is wrong"
#endif    
}

#if (BUZZER_COMPILE || VOICEPACKETPWM_COMPILE)
/*******************************************************************************
*
*/
static void ResponseATSearchEbike(uint8_t* para)
{
	if( ASCII_Num_1 == *para)
	{
		BLEResponse.length = StrlenWithData_SearchEbike ;
		BLEResponse.arr[0] = RCODEWithData_SearchEbike ;
		BLEResponse.arr[1]= 0X01;
		
	#if((1 + 1) != StrlenWithData_ElectLock)
		#error "StrlenWithData_SearchEbike is wrong"
	#endif    
	   
		SendSomeAudio(5,1);
		/********BuzzerSet*************/
	//	Buzzer.counter_targetringnum_mul2 = Ring5_Mul2 ;
	//	Buzzer.counter_nowringnum_mul2 = 0;
	//	Buzzer.timer_addcounter_per2ms = 0 ;
	}
	else
		ResponseErrorATCmdErrorFormat();
}
#endif

#if(Commu_Mode_Common != Commu_Mode)
/*******************************************************************************
*
*/
static void ResponseATErrorStatus(uint8_t* para)
{
	BLEResponse.length = StrlenWithData_ErrorStatus ;
	BLEResponse.arr[0] = RCODEWithData_ErrorStatus ;
    BLEResponse.arr[1] = GetControllerErrorStatus() ;
    
#if((1 + 1) != StrlenWithData_ErrorStatus)
    #error "StrlenWithData_ErrorStatus is wrong"
#endif      

}

/*******************************************************************************
*
*/
uint8_t GetControllerErrorStatus(void)
{
	static uint8_t error = 0 ;
	if((error & ~U8_BIT7) != OLCM.data_error)
		error = OLCM.data_error ;
	
	if(IsACCPoweredOn() )
	{
		if(false == BLEMdlCnt.status_connect2controller)
			error |= U8_BIT7 ;
		else
			error &= ~U8_BIT7 ;
	}
	return error ;

	#if 0
	if(false == BLEMdlCnt.status_connect2controller)
		if(KeyLess.avr_adc > ADC_PowerOff_Thresholds_Differ_D )
			return 0X80 ;	//YXT is broken
	return OLCM.data_error ;
	#endif
}

/*******************************************************************************
*
*/
static void ResponseATQ(uint8_t* para)
{
	CalQInitialAndMileageRemainder();
	BLEResponse.length = StrlenWithData_Q ;
	BLEResponse.arr[0] = RCODEWithData_Q ;
#if(TESTCODE_BLE_220V)
	arr[1] = 0X05 ;//鍥犱负瀛樺偍鍣ㄤ娇鐢ㄧ殑鏄皬绔瓨鍌ㄦ柟寮忥紝鍥犳楂樹綅鏁版嵁鍦╞it[3]涓?
    arr[2] = 0X06 ;
    arr[3] = 0X07 ;
    arr[4] = 0X08 ;
    arr[5] = 0X01 ;//鍥犱负瀛樺偍鍣ㄤ娇鐢ㄧ殑鏄皬绔瓨鍌ㄦ柟寮忥紝鍥犳楂樹綅鏁版嵁鍦╞it[3]涓?
    arr[6] = 0X02 ;
    arr[7] = 0X03 ;
    arr[8] = 0X04 ;
	arr[9] = 0X04 ;
	arr[10] = 0X04 ;
	arr[11] = 0X04 ;
	arr[12] = 0X04 ;
#else	
	
			
    BLEResponse.arr[1] = Q.data_max.bit8[3];//鍥犱负瀛樺偍鍣ㄤ娇鐢ㄧ殑鏄皬绔瓨鍌ㄦ柟寮忥紝鍥犳楂樹綅鏁版嵁鍦╞it[3]涓?
    BLEResponse.arr[2] = Q.data_max.bit8[2];
    BLEResponse.arr[3] = Q.data_max.bit8[1];
    BLEResponse.arr[4] = Q.data_max.bit8[0];
    BLEResponse.arr[5] = Q.data_remainder.bit8[3];//鍥犱负瀛樺偍鍣ㄤ娇鐢ㄧ殑鏄皬绔瓨鍌ㄦ柟寮忥紝鍥犳楂樹綅鏁版嵁鍦╞it[3]涓?
    BLEResponse.arr[6] = Q.data_remainder.bit8[2];
    BLEResponse.arr[7] = Q.data_remainder.bit8[1];
    BLEResponse.arr[8] = Q.data_remainder.bit8[0];
	Union4Bytes_TypeDef temp_send ;
	temp_send.sum32 = Q.data_release_temp ;
	BLEResponse.arr[9] = temp_send.bit8[3];//鍥犱负瀛樺偍鍣ㄤ娇鐢ㄧ殑鏄皬绔瓨鍌ㄦ柟寮忥紝鍥犳楂樹綅鏁版嵁鍦╞it[3]涓?
    BLEResponse.arr[10] = temp_send.bit8[2];
    BLEResponse.arr[11] = temp_send.bit8[1];
    BLEResponse.arr[12] = temp_send.bit8[0];
#endif	
    
#if((12 + 1) != StrlenWithData_Q)
    #error "StrlenWithData_Q is wrong"
#endif       
}

	#if(Commu_Mode_TwoWire == Commu_Mode)
		#if(COMPILE_ENABLE == OneKeyRepairSet)
/*******************************************************************************
*
*/
static void ResponseOneKeyRepair(uint8_t* para)
{
	BLEResponse.length = StrlenWithData_OneKeyRepair ;
	BLEResponse.arr[0] = RCODEWithData_OneKeyRepair ;
	//T2C.flag_onekeyrepair = true ;
	T2C.counter_onekeyrepairset_cycle = Counter_OneKeyRepairSet_Cycle ;
	BLEResponse.arr[1]= SetSuccess;
	
#if((1 + 1) != StrlenWithData_OneKeyRepair)
    #error "StrlenWithData_OneKeyRepair is wrong"
#endif   
}
		#endif

		#if(COMPILE_ENABLE == Double_LV)
/*******************************************************************************
*
*/
static void ResponseLV1LevelUnlockSet(uint8_t* para)
{	
	BLEResponse.length = StrlenWithData_UnLockLevel1LV ;
	BLEResponse.arr[0] = RCODEWithData_UnLockLevel1LV ;
	if(true == IsLVLowerThanLV1Level())	
	{
		CustomSerialInterfaceMessage.status_LVLevel = Status_LowerLVlevel ;
		BLEResponse.arr[1]= SetSuccess;
	}
	else
		BLEResponse.arr[1]= SetFailure;				//鍥犱负鐢靛帇瓒冲楂橈紝璁剧疆鏃犵數缁埅澶辫触
    
#if((1 + 1) != StrlenWithData_UnLockLevel1LV)
    #error "StrlenWithData_UnLockLevel1LV is wrong"
#endif   
    
}
		#endif

		#if(COMPILE_ENABLE == ThreeSpeedSet)
/*******************************************************************************
*
*/
static void ResponseThreeSpeed(uint8_t* para)
	{
		if(2 == GetCurrentCommandLength()) 
		{
			if(ATActionCode_Query == *(para))
				ResponseThreeSpeedQuery();
			else
				ResponseErrorATCmd();
		}
		else if(3 == GetCurrentCommandLength())
		{
			if((ATActionCode_Config_Defalt == *(para))&&(*(para + 1) <= Status_HighSpeed )) 
			{
				T2C.CommonData.ThressSpeedBit = *(para + 1);
				ResponseThreeSpeedConfig();
			}
			else
				ResponseErrorATCmd();
		}
		else
			ResponseErrorATCmd();
	}

/*******************************************************************************
*
*/
static void ResponseThreeSpeedQuery(void)
{
	BLEResponse.length = StrlenWithData_ThreeSpeed ;
	BLEResponse.arr[0] = RCODEWithData_ThreeSpeed ;
	BLEResponse.arr[1]= ATActionCode_Query ;
	BLEResponse.arr[2]= T2C.CommonData.ThressSpeedBit ;
#if((1 + 2) != StrlenWithData_ThreeSpeed)
    #error "StrlenWithData_ThreeSpeed is wrong"
#endif   
}

/*******************************************************************************
*
*/
static void ResponseThreeSpeedConfig(void)
{
	BLEResponse.length = StrlenWithData_ThreeSpeed ;
	BLEResponse.arr[0] = RCODEWithData_ThreeSpeed ;
	BLEResponse.arr[1] = ATActionCode_Config_Defalt ;
	BLEResponse.arr[2] = SET_SUCCESS_U8;
		
#if((1 + 2) != StrlenWithData_ThreeSpeed)
    #error "StrlenWithData_ThreeSpeed is wrong"
#endif   
}
		#endif
		
		#if(COMPILE_ENABLE == ThreeTorqueSet)
/*******************************************************************************
*
*/
static void ResponseThreeTorque(uint8_t* para)
{
	if(2 == GetCurrentCommandLength()) 
	{
		if(ATActionCode_Query == *(para))
			ResponseThreeTorqueQuery();
		else
			ResponseErrorATCmd();
	}
	else if(3 == GetCurrentCommandLength())
	{
		if((ATActionCode_Config_Defalt == *(para))&&(*(para + 1) <= Status_HighTorque)) 
		{
			T2C.CommonData.ThressTorqueBit = *(para + 1) ;
			ResponseThreeTorqueConfig();
		}
		else
			ResponseErrorATCmd();
	}
	else
			ResponseErrorATCmd();
}

/*******************************************************************************
*
*/
static void ResponseThreeTorqueQuery(void)
{
	BLEResponse.length = StrlenWithData_ThreeTorque ;
	BLEResponse.arr[0] = RCODEWithData_ThreeTorque ;
	BLEResponse.arr[1]= ATActionCode_Query ;
	BLEResponse.arr[2] = T2C.CommonData.ThressTorqueBit ;
			
#if((1 + 2) != StrlenWithData_ThreeTorque)
    #error "StrlenWithData_ThreeTorque闀垮害涓嶅"
#endif   
}

/*******************************************************************************
*
*/
static void ResponseThreeTorqueConfig(void)
{
	BLEResponse.length = StrlenWithData_ThreeTorque ;
	BLEResponse.arr[0] = RCODEWithData_ThreeTorque ;
	BLEResponse.arr[1] = ATActionCode_Config_Defalt;
	BLEResponse.arr[2] = SET_SUCCESS_U8;
			
#if((1 + 2) != StrlenWithData_ThreeTorque)
    #error "StrlenWithData_ThreeTorque is wrong"
#endif   
}
		#endif

	#endif
#endif

/*******************************************************************************
*
*/
static void ResponseATMileage(uint8_t* para)
{
	BLEResponse.length = StrlenWithData_Mileage ;
	BLEResponse.arr[0] = RCODEWithData_Mileage ;
	
#if(TESTCODE_BLE_220V)
	BLEResponse.arr[1] = 0X06 ; //鍥犱负瀛樺偍鍣ㄤ娇鐢ㄧ殑鏄皬绔瓨鍌ㄦ柟寮忥紝鍥犳楂樹綅鏁版嵁鍦╞it[3]涓?
    BLEResponse.arr[2] = 0X08 ;
    BLEResponse.arr[3] = 0X0A ;
    BLEResponse.arr[4] = 0X0C ;
    BLEResponse.arr[5] = 0X05 ;//鍥犱负瀛樺偍鍣ㄤ娇鐢ㄧ殑鏄皬绔瓨鍌ㄦ柟寮忥紝鍥犳楂樹綅鏁版嵁鍦╞it[3]涓?
    BLEResponse.arr[6] = 0X06 ;
    BLEResponse.arr[7] = 0X07 ;
    BLEResponse.arr[8] = 0X08 ;
    BLEResponse.arr[9] = 0X01 ;//鍥犱负瀛樺偍鍣ㄤ娇鐢ㄧ殑鏄皬绔瓨鍌ㄦ柟寮忥紝鍥犳楂樹綅鏁版嵁鍦╞it[3]涓?
    BLEResponse.arr[10] = 0X02 ;
    BLEResponse.arr[11] = 0X03 ;
    BLEResponse.arr[12] = 0X04 ;
	BLEResponse.arr[13] = 0X01 ;//鍥犱负瀛樺偍鍣ㄤ娇鐢ㄧ殑鏄皬绔瓨鍌ㄦ柟寮忥紝鍥犳楂樹綅鏁版嵁鍦╞it[3]涓?
    BLEResponse.arr[14] = 0X02 ;
    BLEResponse.arr[15] = 0X03 ;
#else
    BLEResponse.arr[1] = Mileage.counter_hallchange_max.bit8[3];//鍥犱负瀛樺偍鍣ㄤ娇鐢ㄧ殑鏄皬绔瓨鍌ㄦ柟寮忥紝鍥犳楂樹綅鏁版嵁鍦╞it[3]涓?
    BLEResponse.arr[2] = Mileage.counter_hallchange_max.bit8[2];
    BLEResponse.arr[3] = Mileage.counter_hallchange_max.bit8[1];
    BLEResponse.arr[4] = Mileage.counter_hallchange_max.bit8[0];
    BLEResponse.arr[5] = Mileage.counter_hallchange_remainder.bit8[3];//鍥犱负瀛樺偍鍣ㄤ娇鐢ㄧ殑鏄皬绔瓨鍌ㄦ柟寮忥紝鍥犳楂樹綅鏁版嵁鍦╞it[3]涓?
    BLEResponse.arr[6] = Mileage.counter_hallchange_remainder.bit8[2];
    BLEResponse.arr[7] = Mileage.counter_hallchange_remainder.bit8[1];
    BLEResponse.arr[8] = Mileage.counter_hallchange_remainder.bit8[0];
    BLEResponse.arr[9] = Mileage.counter_hallchange_accumu_under1KM.bit8[3];//鍥犱负瀛樺偍鍣ㄤ娇鐢ㄧ殑鏄皬绔瓨鍌ㄦ柟寮忥紝鍥犳楂樹綅鏁版嵁鍦╞it[3]涓?
    BLEResponse.arr[10] = Mileage.counter_hallchange_accumu_under1KM.bit8[2];
    BLEResponse.arr[11] = Mileage.counter_hallchange_accumu_under1KM.bit8[1];
    BLEResponse.arr[12] = Mileage.counter_hallchange_accumu_under1KM.bit8[0];
	BLEResponse.arr[13] = Mileage.odometer_perKM.bit8[3] ;
    BLEResponse.arr[14] = Mileage.odometer_perKM.bit8[2] ;
    BLEResponse.arr[15] = Mileage.odometer_perKM.bit8[1] ;
	BLEResponse.arr[16] = Mileage.odometer_perKM.bit8[0] ;
#endif
    
#if((16 + 1) != StrlenWithData_Mileage)
    #error "StrlenWithData_Mileage is wrong"
#endif     

}

/*******************************************************************************
*
*/
static void ResponseATHallSpeed(uint8_t* para)
{
	UnionSumWithH8L8_TypeDef hallspeed ;
	hallspeed.sum = GetHallSpeed();
	
	BLEResponse.length = StrlenWithData_HallSpeed ;
	BLEResponse.arr[0] = RCODEWithData_HallSpeed ;
	
#if(TESTCODE_BLE_220V)
	BLEResponse.arr[1]= 0x08;
    BLEResponse.arr[2]= 0XA0;
#else
    BLEResponse.arr[1]= hallspeed.h8;
    BLEResponse.arr[2]= hallspeed.l8;
#endif
    
	
	
#if((2 + 1) != StrlenWithData_HallSpeed)
    #error "StrlenWithData_HallSpeed is wrong"
#endif   
}

/*******************************************************************************
*
*/
static void ResponseHardwareVersion(uint8_t* para)
{
	BLEResponse.length = StrlenWithData_HardwareVersion ;
	BLEResponse.arr[0] = RCODEWithData_HardwareVersion ;

    BLEResponse.arr[1] = (uint8_t)(PCB_VERSION >> 8);
    BLEResponse.arr[2] = (uint8_t)(PCB_VERSION);
    BLEResponse.arr[3] = (uint8_t)(FirmwareVersion >> 16);
    BLEResponse.arr[4] = (uint8_t)(FirmwareVersion >> 8);
    BLEResponse.arr[5] = (uint8_t)(FirmwareVersion);
    
#if((5 + 1) != StrlenWithData_HardwareVersion)
    #error "StrlenWithData_HardwareVersion is wrong"
#endif   
}

/*******************************************************************************
*
*/
/*
static void ResponseHexTime(uint8_t* para)
{
	HexTime_UnionTypeDef hextemp ;
	for(uint8_t i= 0 ;i < sizeof(HexTime_UnionTypeDef) ; i ++)
		hextemp.data[i] = hextimeconst[i] ;

	BLEResponse.length = StrlenWithData_HexTime ;
	BLEResponse.arr[0] = RCODEWithData_HexTime ;
    BLEResponse.arr[1] = ATActionCode_Query ;
    BLEResponse.arr[2] = hextemp.year_arr1 ;
    BLEResponse.arr[3] = hextemp.year_arr2 ;
    BLEResponse.arr[4] = hextemp.year_arr3 ;
    BLEResponse.arr[5] = hextemp.year_arr4 ;
	BLEResponse.arr[6] = hextemp.month_arr1 ;
	BLEResponse.arr[7] = hextemp.month_arr2 ;
	BLEResponse.arr[8] = hextemp.month_arr3 ;
	BLEResponse.arr[9] = hextemp.date_arr1 ;
	BLEResponse.arr[10] = hextemp.date_arr2 ;
	BLEResponse.arr[11] = hextemp.hour_arr1 ;
	BLEResponse.arr[12] = hextemp.hour_arr2 ;
	BLEResponse.arr[13] = hextemp.minute_arr1 ;
	BLEResponse.arr[14] = hextemp.minute_arr2 ;

#if((14 + 1) != StrlenWithData_HexTime)
    #error "StrlenWithData_HexTime is wrong"
#endif   
}
*/

#if 0
/*******************************************************************************
*
*/
static void ResponseCustomerFileNum(uint8_t* para)
{
	BLEResponse.length = StrlenWithData_CFileNum ;
	BLEResponse.arr[0] = RCODEWithData_CFileNum ;
    BLEResponse.arr[1] = 0x00;
    BLEResponse.arr[2] = (uint8_t)(CumstomerNum >> 32) ;
    BLEResponse.arr[3] = (uint8_t)(CumstomerNum >> 24) ;
    BLEResponse.arr[4] = (uint8_t)(CumstomerNum >> 16) ; 	
    BLEResponse.arr[5] = (uint8_t)(CumstomerNum >> 8) ;		
	BLEResponse.arr[6] = (uint8_t)(CumstomerNum) ;		
    
#if((6 + 1) != StrlenWithData_CFileNum)
    #error "StrlenWithData_CFileNum is wrong"
#endif   
}

/*******************************************************************************
*
*/
static void ResponseATCyclicSend(uint8_t* para)
{
	if(ASCII_Num_0 == * para )
	{
		AT.mode_cyclicsend = false ;
	}
	else if(ASCII_Num_1 == * para )
	{
		AT.mode_cyclicsend = true ;
		CyclicSendData(para);
	}
	else
		ResponseErrorATCmdErrorFormat();	
}

/*******************************************************************************
*
*/
static void CyclicSendData(uint8_t* para)
{
	BLEResponse.length = StrlenWithData_CyclicSend ;
	BLEResponse.arr[0] = RCODEWithData_CyclicSend ;
	
#if(COMPILE_ENABLE == TESTCODE_BLE_220V)
	static uint8_t temp = 0 ;
	//鑾峰彇鏁呴殰浠ｇ爜
	BLEResponse.arr[1] = temp;	
	
	//鑾峰彇鍓╀綑鐢甸噺
	if(temp%3)
		BLEResponse.arr[2] = 0X01 ;
	else
		BLEResponse.arr[2] = 0X00 ;
	BLEResponse.arr[3] = 0X02;
	BLEResponse.arr[4] = 0X03;
	BLEResponse.arr[5] = 0X04; ;
	
	//鑾峰彇閲岀▼
	BLEResponse.arr[6] = 0X05; 
	BLEResponse.arr[7] = 0X06; 
	BLEResponse.arr[8] = 0X07; 
	BLEResponse.arr[9] = 0X08; 
	BLEResponse.arr[10] = 0X0A ;
	BLEResponse.arr[11] = 0X0B ;
	BLEResponse.arr[12] = 0X0C ;
	BLEResponse.arr[13] = 0X0D ;
	BLEResponse.arr[14] = 0X0E ;
	temp ++ ;
	if(temp >= 15)
		temp = 0 ;

#else
	#if(Commu_Mode_Common == Commu_Mode)
		//鑾峰彇鏁呴殰浠ｇ爜
		BLEResponse.arr[1] = 0;		
		//鑾峰彇鍓╀綑鐢甸噺
		BLEResponse.arr[2] = 0 ;	
		BLEResponse.arr[3] = 0 ;
		BLEResponse.arr[4] = 0 ;
		BLEResponse.arr[5] = 0 ;
		
		BLEResponse.arr[10] = 0 ;		//娌℃硶鑾峰彇灏卞啓鏈?澶у??
		BLEResponse.arr[11]= 0 ;		
		BLEResponse.arr[14] = 0 ;		//娌℃硶鑾峰彇閫熷害	灏嗘潵鑰冭檻浣跨敤闇嶅皵閲囨牱
		BLEResponse.arr[15] = 0 ;
		
	#else
		//鑾峰彇鏁呴殰浠ｇ爜
		BLEResponse.arr[1] = GetControllerErrorStatus();		
		//鑾峰彇鍓╀綑鐢甸噺
		BLEResponse.arr[2] = Q.data_remainder.bit8[3];//鍥犱负瀛樺偍鍣ㄤ娇鐢ㄧ殑鏄皬绔瓨鍌ㄦ柟寮忥紝鍥犳楂樹綅鏁版嵁鍦╞it[3]涓?					
		BLEResponse.arr[3] = Q.data_remainder.bit8[2];
		BLEResponse.arr[4] = Q.data_remainder.bit8[1];
		BLEResponse.arr[5] = Q.data_remainder.bit8[0];
		
        BLEResponse.arr[10] = 0 ;   //涓哄皢鏉ユ洿澶х數娴侀鐣?
		BLEResponse.arr[11] = OLCM.data_current	;		//闄や互4灏辨槸鐪熷疄鍊?
		BLEResponse.arr[14] = OLCM.counter_hall_speed.h8 ;	//0.5s鍐呯殑闇嶅皵閫熷害 楂樹綅
		BLEResponse.arr[15]	= OLCM.counter_hall_speed.l8 ;
	#endif
		
		//鑾峰彇鍓╀綑閲岀▼
		BLEResponse.arr[6] = Mileage.counter_hallchange_remainder.bit8[3];//鍥犱负瀛樺偍鍣ㄤ娇鐢ㄧ殑鏄皬绔瓨鍌ㄦ柟寮忥紝鍥犳楂樹綅鏁版嵁鍦╞it[3]涓?
		BLEResponse.arr[7] = Mileage.counter_hallchange_remainder.bit8[2];
		BLEResponse.arr[8] = Mileage.counter_hallchange_remainder.bit8[1];
		BLEResponse.arr[9] = Mileage.counter_hallchange_remainder.bit8[0];
		
		Union4Bytes_TypeDef lv_temp ;
		lv_temp.sum32 = LV.avr_adc ;
		
		#if(36 == LV_Grade)	
			#warning "unfinished"
		#elif(48 == LV_Grade)		//LVDigital2Analog 涓?0.021257136   *16 =	0.340114176		*17/50
		lv_temp.sum32 = lv_temp.sum32 * 17 / 50 ;
		#elif(60 == LV_Grade)	//LVDigital2Analog 涓?0.026586914   *16= 0.425390624		*17/40
		lv_temp.sum32 = lv_temp.sum32 * 17 / 40 ;	
		#elif(72 == LV_Grade)	//LVDigital2Analog 涓?0.032025146 *16 =0.512402336			*41/80
		lv_temp.sum32 = lv_temp.sum32 * 41 / 80 ;	
		#else
		#endif
		
        UnionSumWithH8L8_TypeDef temp ;
        temp.sum = lv_temp.StructH16L16.l16 ;
		BLEResponse.arr[12] = temp.h8;
		BLEResponse.arr[13] = temp.l8;			
#endif
		
#if((15 + 1) != StrlenWithData_CyclicSend)
    #error "StrlenWithData_CyclicSend is wrong"
#endif   
}


/*******************************************************************************
*
*/
static void CloseCyclicSend(void)
{
    if(true == AT.mode_cyclicsend )
        AT.mode_cyclicsend = false ;
}
#endif


#if(KeyLessStart == Anti_theftDevice_Function)
/*******************************************************************************
*
*/
void ResponseKeyLessStart(uint8_t* para)
{
	if(ASCII_Num_0 == * para)
		KeyLessPowerOff();
    else if(ASCII_Num_1 == * para )
		KeyLessPowerOn();
	else if(ASCII_QuestionMark == * para) 
		ResponseKeyLessStatusChanged();
	else
        ResponseErrorATCmdErrorFormat();
#if (!KEYLESS_RESPONSE_COMPILE)
	BLEResponse.length = 0 ;
#endif	
}

/*******************************************************************************
*
*/
 void ResponseKeyLessStatusChanged(void)
{
	BLEResponse.length = StrlenWithData_KeyLessStatusChanged ;
	BLEResponse.arr[0] = RCODEWithData_KeyLessStart ;
	BLEResponse.arr[1]= (uint8_t)KeyLess.status ;
    
#if((1 + 1) !=StrlenWithData_KeyLessStatusChanged)
    #error "StrlenWithData_KeyLessStatusChanged is wrong"
#endif
}

	#if MECHANICALLOCK_INTERNAL_COMPILE
/*******************************************************************************
*
*/
void ResponseMechalMotorLock(uint8_t* para)
{
	BLEResponse.length = StrlenWithData_MechalLock ;
	BLEResponse.arr[0] = RCODEWithData_MechalLock ;
	BLEResponse.arr[1]= (uint8_t)MechalMotorLock.status;
    
#if((1 + 1) !=StrlenWithData_MechalLock)
    #error "StrlenWithData_MechalLock is wrong"
#endif
}

/*******************************************************************************
*
*/
 void ResponseDouSupportStatusAndErrorChanged(uint8_t* para) 
{
	BLEResponse.length = StrlenWithData_DouSupport ;
	BLEResponse.arr[0] = RCODEWithData_DouSupport ;
	
#if(ELU_ActionSingle_Mode == ELU_BreakSingle_Mode)	
	BLEResponse.arr[1]= (uint8_t)DouSupport.status ;	
	#if(FuzhouPeipeng_DouSupport_ManufactID == DouSupport_Manufacturer_ID)
	BLEResponse.arr[2]= 0 ;
	#elif(DongdaLanniao_DouSupport_ManufactID == DouSupport_Manufacturer_ID)
	BLEResponse.arr[2]= DouSupport.error;
	#else
	#endif
#elif(ELU_ActionSingle_Mode == ELU_DirectlyUnlocked_Mode)
	BLEResponse.arr[1]= (uint8_t)MechalMotorLock.status ;
	BLEResponse.arr[2] = 0 ;
#else	
#endif
    
#if((1 + 2) != StrlenWithData_DouSupport)
    #error "StrlenWithData_DouSupport is wrong"
#endif   
}

		#if DOUSUPPORT_INTERNAL_COMPILE
/*******************************************************************************
*
*/
 void ResponseDouSupport(uint8_t* para)
{
	if(ASCII_Num_0 == *para)		//鍙屾拺鏀惰捣
	{
#if(FuzhouPeipeng_DouSupport_ManufactID == DouSupport_Manufacturer_ID)		
		DouSupport.cmdfrom = DouSupportActionCmdFrom_ATCmd ;
#endif
		DouSupportRetractAction();
		ResponseDouSupportStatusAndErrorChanged(para);
	}
	else if(ASCII_Num_1 == *para)		//鍙屾拺钀戒笅
	{
#if(FuzhouPeipeng_DouSupport_ManufactID == DouSupport_Manufacturer_ID)			
		DouSupport.cmdfrom = DouSupportActionCmdFrom_ATCmd ;
#endif
		DouSupportRaiseAction();
		ResponseDouSupportStatusAndErrorChanged(para);
	}
	else if(ASCII_QuestionMark == *para)
		ResponseDouSupportStatusAndErrorChanged(para);
	else
        ResponseErrorATCmdErrorFormat();
}

		#endif
	#endif
#endif

#if SEATLOCK_COMPILE
/*******************************************************************************
*
*/
void ResponseSeatLock(uint8_t* para)
{	
	if(ASCII_Num_0 == * para)
	{	
		SeatLock.flag_cmd_action = true ;
		BLEResponse.length =  0 ;			
	}
	else if(ASCII_QuestionMark == * para)
		ResponseSeatLockAction();
	else
    {
        ResponseErrorATCmdErrorFormat();
    }
}

/*******************************************************************************
*
*/
 void ResponseSeatLockAction(void)
{
	BLEResponse.length = StrlenWithData_SeatLock ;
	BLEResponse.arr[0] = RCODEWithData_SeatLock ;
	if(Status_WaitForMotorStop == SeatLock.status)
		BLEResponse.arr[1]= (uint8_t)Status_UnlockAction ;
	else
		BLEResponse.arr[1]= (uint8_t)SeatLock.status ;
    
#if((1 + 1) !=StrlenWithData_SeatLock)
    #error "StrlenWithData_SeatLock闀垮害涓嶅"
#endif
}

 /*******************************************************************************
 *
 */
void SeatLockNotifyWhenStatusChanged(void)
{
	if(true == SeatLock.flag_BLE_notify)
	{
		if(IsBLEConnectStatusInteractive())
		{
			ResponseSeatLockAction();
			SimpleBLEPeripheral_enqueueAsySendMsg() ;	
		}
		SeatLock.flag_BLE_notify = false ;
	}
}
#endif

#if TEST_EXTFLASH_COMPILE
/*******************************************************************************
*
*/
void ResponseTestExtFlash(uint8_t* para)
{	
	BLEResponse.length = 0 ;	
	SetFlagCheckExtFlash();
}

/*******************************************************************************
*
*/
void CheckAndResponseExtFlashResult(uint8_t result)
{
	BLEResponse.length = StrlenWithData_TestExtFlash ;
	BLEResponse.arr[0] = RCODEWithData_TestExtFlash ;
	BLEResponse.arr[1] = 0X00 ;
	BLEResponse.arr[2]=  result ;
    
#if((1 + 2) !=StrlenWithData_TestExtFlash)
    #error "StrlenWithData_TestExtFlash is wrong"
#endif
}

#endif

void ResponseResetBle(uint8_t* para)
{
	BLEResponse.length = Strlen_ATResetBLE ;
	BLEResponse.arr[0] = RCODEWithData_ResetHC08 ;
	BLEResponse.arr[1] = 0X01 ;
    SystemReset();;
}




#if HEARTBEAT_COMPILE
/*******************************************************************************
*
*/
static void ResponseATHeartBeat(uint8_t* para)
{
	BLEResponse.length = 0 ;	
}


/*******************************************************************************
*
*/
void HeartBeatSendEveryFixedInterval(void)
{
//	if(IsBLEConnectStatusInteractive())
//	{
//		if(timer_heartbeat_send_per500ms)
//		{
//			timer_heartbeat_send_per500ms -- ;
//			if(0 == timer_heartbeat_send_per500ms)
//			{
//				FillBLEHeartBeatData();
//				SimpleBLEPeripheral_enqueueAsySendMsg();
//				timer_heartbeat_send_per500ms = Timer_HeartBeatSendInterval_Per500ms ;
//			}
//		}
		/*if(timer_heartbeat_send_per500ms > 1)
			timer_heartbeat_send_per500ms -- ;
		else
		{
				FillBLEHeartBeatData();
				SimpleBLEPeripheral_enqueueAsySendMsg();
				timer_heartbeat_send_per500ms = Timer_HeartBeatSendInterval_Per500ms ;
		}	*/
//	}	
	if(true == IsVariableTimerDecreaseToZero(&timer_heartbeat_send_per500ms))
	{
		FillBLEHeartBeatData();
		SimpleBLEPeripheral_enqueueAsySendMsg();
		timer_heartbeat_send_per500ms = Timer_HeartBeatSendInterval_Per500ms ;
	}	
		
}

/*******************************************************************************
*
*/
static void FillBLEHeartBeatData(void)
{
	BLEResponse.length = StrlenWithData_HeartBeatSend ;
	BLEResponse.arr[0] = RCODEWithData_HeartBeat ;
    BLEResponse.arr[1] = 0X00;

#if((1 + 1) != StrlenWithData_HeartBeatSend)
    #error "StrlenWithData_HeartBeatSend is wrong"
#endif   
}

/*******************************************************************************
*
*/
void CheckBLEHeartBeat(void)
{
	/*{
		if(timer_heartbeat_outtime_per500ms)
		{
			timer_heartbeat_outtime_per500ms -- ;
			if(0 == timer_heartbeat_outtime_per500ms)
			{
				SimpleBLEPeripheral_enqueueInitiativeDisConnectMsg();
//				GAPRole_TerminateConnection(); 
//				bleconnectstatus = BLE_Unconnect ;
			}
		}
	}*/
	if(true == IsVariableTimerDecreaseToZero(&timer_heartbeat_outtime_per500ms))
	{
		SimpleBLEPeripheral_enqueueInitiativeDisConnectMsg();
	}	
}

/*******************************************************************************
*
*/
void ClearHeartBeatTimer(void)
{
	timer_heartbeat_outtime_per500ms = Timer_HeartBeat_TimeOut_Per500ms ;
	timer_heartbeat_send_per500ms = Timer_HeartBeatSendInterval_Per500ms ;
}


#endif


