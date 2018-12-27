#include "defines.h"
#include "bcomdef.h"

#include "simple_peripheral.h"


#include "osal_snv.h"
#include "FlashSNV.h"
#include "systemcontrol.h"
#include "ATControl.h"
#include "BatteryPowerCal.h"
#include "Guard.h"
#include "gpscontrol.h"
#include "debug.h"

#include "icall_api.h"

#include "MileageCal.h"

#if TEST_EXTFLASH_COMPILE
#include "oad_target.h"
#endif


//Blank Data
//#define 	DATA_FLASH_BLANK				0xffffffff

#define		BIT_AUTO_Guard							U8_BIT0
#define		BIT_DouSupport							U8_BIT4
	 
#define		Commu_Mode_RecArrNum					1
#define		Gear_Ratio_Motor_RecArrNum				2
#define		Gear_Ratio_Wheel_RecArrNum				3
#define		Number_Pole_Pairs_RecArrNum				4
#define		Wheel_Diameter_Inch_Int_RecArrNum		5
#define		AH_Battery_RecArrNum					6
#define		T2CFuncition_RecArrNum					7
#define		Wheel_Diameter_Inch_Dec_RecArrNum		8
#define		Battery_Materials_RecArrNum				9
#define		Battery_SectionNum_RecArrNum			10
#define		BatteryStandardKM_RecArrNum				11
#define		Rated_Q_Release_PerFrame_A_RecArrNum	12
#define		Anti_theftDevice_Function_RecArrNum		13
#define		Mileage_Max_Byte1_RecArrNum				14
#define		Mileage_Max_Byte2_RecArrNum				15
#define		Mileage_Max_Byte3_RecArrNum				16
#define		Mileage_Max_Byte4_RecArrNum				17	 

#define 	BLE_NVID_CUST_BLE 						BLE_NVID_CUST_START
/*********************************************************************
 * TYPEDEFS
 */
typedef enum
{
  Status_FlashProtect_Non  =0 ,
  Status_FlashProtected
}StatusFlashProtect_TypeDef;


typedef struct
{	
	union
	{
		uint8_t status ;
		struct
		{
			uint8_t bit_autoguard : 	1 ;
			uint8_t bit_GPSguard : 		1 ;
			uint8_t	bit_reserved1 :		2 ;
			uint8_t bit_dousupport :	1 ;
			uint8_t	bit_reserved2 :		3 ;
		};
	};
	uint16_t timer_day_guard_timeleft;
	uint8_t timer_hour_guard_timeleft ;
	uint8_t timer_minute_guard_timeleft ;
	#if HistoryMileage_COMPILE   
	uint32_t HistoryMileageINFlash;
    #endif
	//uint8_t checksum ;
	uint8_t fixvalue ;
	
}FlashStruct_TypeDef ;



static FlashStruct_TypeDef data_flash_read;
static FlashStruct_TypeDef data_flash_write;
static StatusFlashProtect_TypeDef status_flash_protect =Status_FlashProtected ;// Status_FlashProtect_Non;
static flashOperationMessageCBs_t *pAppCBs = NULL;

#if TEST_EXTFLASH_COMPILE
static bool flag_testExtFlash = false ;

#endif

 /*********************************************************************
  * FUNCTIONS
  */
/*
* Read SNV Flash memory data and assign value to variables
* The function need to be called in a ICall_registerApp Task
*/
static uint8_t ReadFlash(void);
/**
 * Write variables data to SNV Flash memory
 * The function need to be called in a ICall_registerApp Task
 */
static uint8_t WriteFlash(void);

static bool IsLVadcHighThanLVFlashProtectSchmidtValue(void);
static bool IsLVadcLowerThanLVFlashProtect(void) ;


static bool IsFlashDataDiffer(void) ;

static bool IsUpdateDeadLineInFlash(void);
static bool IsUpdateElectLockInFlash(void) ;

#if GPS_COMPILE
static bool IsUpdateGPSLockStatusInFlash(void);
#endif
static bool IsReadFlashSuccess(void) ;

static void UpdateWriteData(void) ;

#if HistoryMileage_COMPILE   
static bool IsUpdateHistoryMileage(void);
#endif

/*#if(ElectricDouSupport == Anti_theftDevice_Function)
static bool IsUpdateDouSupportStatusInFlash(void) ;
#endif*/



/********************************************************
 * 
 */
static uint8_t ReadFlash(void)
{
	
//	if(SUCCESS != osal_snv_read(BLE_NVID_CUST_BLE, sizeof(FlashStruct_TypeDef)/sizeof(uint8_t), (void *)&data_flash_read) )
//		return NV_OPER_FAILED ;
//	return SUCCESS;	
	return osal_snv_read(BLE_NVID_CUST_BLE, sizeof(FlashStruct_TypeDef)/sizeof(uint8_t), (void *)&data_flash_read) ;
}

/********************************************************
 * 
 */
uint8_t WriteFlash(void)
{
	return osal_snv_write(BLE_NVID_CUST_BLE, sizeof(FlashStruct_TypeDef)/sizeof(uint8_t), (void *)&data_flash_write);
}




/********************************************************
 * 
 */
static bool IsLVadcHighThanLVFlashProtectSchmidtValue(void)
{
#if(Lead_Acid_Batteries == Battery_Materials_InFlash)	
	//if(LV.avr_adc > Lead_Acid_LV_FlashProtect_Schmidt_D)
	if(GetLVValueFlashSaveDetect()> Lead_Acid_LV_FlashProtect_Schmidt_D)
		return true ;
	return false ;
#else
	uint16_t temp ;
	if(true == IsBatteryNumConventional())
		temp = MnNiCo_LV_FlashProtect_Schmidt_Con_D ;
	else
		temp = MnNiCo_LV_FlashProtect_Schmidt_UnCon_D ;
	//if(LV.avr_adc > temp)
	if(GetLVValueFlashSaveDetect() > temp)
		return true ;
	return false ;
#endif
}

/********************************************************
 * 
 */
static bool IsLVadcLowerThanLVFlashProtect(void)
{
#if(Lead_Acid_Batteries == Battery_Materials_InFlash)	
	if(false == IsFlagLVInitSet())
		return false ;
	if(GetLVValueFlashSaveDetect() >= Lead_Acid_LV_FlashProtect_D)
		return false ;
	return true ;
#else
	uint16_t temp ;
	if(true == IsBatteryNumConventional())
		temp = MnNiCo_LV_FlashProtect_Con_D ;
	else
		temp = MnNiCo_LV_FlashProtect_UnCon_D ;
	if(false == IsFlagLVInitSet())
		return false ;
	if(GetLVValueFlashSaveDetect() >= temp)
		return false ;
	return true ;
#endif
}

/********************************************************
 * 
 */
void FlashProtectWhenLVLow(void)
{
	if(Status_FlashProtect_Non == status_flash_protect)
	{
		if(IsLVadcLowerThanLVFlashProtect())
		{
			//FlashProtectionAction() ;//
			NotifyFlashAction(Flash_Protection);
			status_flash_protect = Status_FlashProtected ;
		}
	}
	else
	{
		if(IsLVadcHighThanLVFlashProtectSchmidtValue()) 
			status_flash_protect = Status_FlashProtect_Non ;
	}
}

/********************************************************
 * 
 */
void FlashProtectionAction(void)
{
	ReadFlash();
	if(true == IsFlashDataDiffer())
	{
		UpdateWriteData();
		WriteFlash() ;//NotifyFlashAction(Flash_Protection);//
	}
}


/********************************************************
 * 
 */
static bool IsFlashDataDiffer(void)
{
#if(Commu_Mode_Common == Commu_Mode)
	if(IsUpdateDeadLineInFlash() )
		return true ;
	if(IsUpdateElectLockInFlash())
		return true ;
#else 
	if(IsUpdateDeadLineInFlash() )
		return true ;
	if(IsUpdateElectLockInFlash())
		return true ;

	#if GPS_COMPILE	
	if(IsUpdateGPSLockStatusInFlash())
		return true ;
	#endif
	
	#if HistoryMileage_COMPILE	
	if(IsUpdateHistoryMileage())
		return true ;
	#endif
	/*#if(ElectricDouSupport == Anti_theftDevice_Function)
	if(YES == IsUpdateDouSupportStatusInFlash() )
		return YES ;
	#endif		*/	
#endif

	return false ;
}



/********************************************************
 * 
 */
/*
FlagStatus IsUpdateOdometerInFlash(void)
{
	if(Flash_CC.data_flash_read.FlashStruct.odometer_0 != Mileage.odometer_perKM.bit8[0])
	{
		Flash_CC.data_flash_write.FlashStruct.odometer_0 = Mileage.odometer_perKM.bit8[0] ;
		return YES ;
	}
	if(Flash_CC.data_flash_read.FlashStruct.odometer_1 != Mileage.odometer_perKM.bit8[1])
	{
		Flash_CC.data_flash_write.FlashStruct.odometer_1 = Mileage.odometer_perKM.bit8[1] ;
		return YES ;
	}
	if(Flash_CC.data_flash_read.FlashStruct.odometer_2 != Mileage.odometer_perKM.bit8[2])
	{
		Flash_CC.data_flash_write.FlashStruct.odometer_2 = Mileage.odometer_perKM.bit8[2] ;
		return YES ;
	}
	return NO ; 
}
*/
/********************************************************
 * 
 */
static bool IsUpdateDeadLineInFlash(void)
{
	if(data_flash_read.timer_day_guard_timeleft != Guard.deadlinedata.timerleft_day.sum)
		return true ;
	if(data_flash_read.timer_hour_guard_timeleft != Guard.deadlinedata.timerleft_hour)
		return true ;
	if(data_flash_read.timer_minute_guard_timeleft != Guard.deadlinedata.timerleft_minute)				
		return true ;
	return false ;
}

/********************************************************
* 
*/
static bool IsUpdateElectLockInFlash(void)
{
	if(data_flash_read.bit_autoguard)		
	{
		if(Status_Auto != Guard.status_autoguard)
			return true ;
	}	
	else
	{
		if(Status_Auto == Guard.status_autoguard)
			return true ;
	}
	return false ;
}

#if HistoryMileage_COMPILE   
static bool IsUpdateHistoryMileage(void)
{
	if(data_flash_read.HistoryMileageINFlash!=Mileage.odometer_perKM.sum32)		
	{
		return true ;
	}	
	else
	;
	return false ;
}	
#endif

#if GPS_COMPILE
/*******************************************************************************
* 函数名        : 
* 函数描述    	:
* 输入参数      : None
* 输出结果      : None

* 返回值        : None
*******************************************************************************/
static bool IsUpdateGPSLockStatusInFlash(void)
{
	if(data_flash_read.bit_GPSguard)
	{
		if(Lock_ByForceGPRS != Guard.cmd_lock_From_FroceGPRS)
			return true ;		
	}
	else
	{
		if(Lock_ByForceGPRS == Guard.cmd_lock_From_FroceGPRS)
			return true ; 
	}
	return false ;
}
#endif

/********************************************************
 * 
 */
 static bool IsReadFlashSuccess(void)
{
	if(SUCCESS != ReadFlash())
		return false ;
	if(data_flash_read.fixvalue != 0XAA)
		return false ;
	return true ;
 }


/********************************************************
 * 
 */
void ReadGuardContextFromFlashWhenInitial(void)
{
	if(true == IsReadFlashSuccess())	
	{
		//read flash status
		if(data_flash_read.bit_autoguard)
			Guard.status_autoguard = Status_Auto ;
		else
			Guard.status_autoguard = Status_UnAuto ;

		if(data_flash_read.bit_GPSguard)
			Guard.cmd_lock_From_FroceGPRS = Lock_ByForceGPRS ;
		else
			Guard.cmd_lock_From_FroceGPRS = Unlock_ByForceGPRS ;

		//read flash guard deadline
		Guard.deadlinedata.timerleft_day.sum = data_flash_read.timer_day_guard_timeleft ;
		Guard.deadlinedata.timerleft_hour = data_flash_read.timer_hour_guard_timeleft ;
		Guard.deadlinedata.timerleft_minute = data_flash_read.timer_minute_guard_timeleft ;
        #if HistoryMileage_COMPILE	
	    Mileage.odometer_perKM.sum32= data_flash_read.HistoryMileageINFlash ;
        #endif
	}
	else
	{		
		//intial status
		Guard.status_autoguard = Status_UnAuto ;
		Guard.cmd_lock_From_FroceGPRS = Unlock_ByForceGPRS ;	
		
		//deadline initial
		Guard.deadlinedata.timerleft_day.sum = 255 ;
		Guard.deadlinedata.timerleft_hour = Hours_PerDay - 1 ;
		Guard.deadlinedata.timerleft_minute = Minutes_PerHour - 1 ;
	}

	//guard deadline second initial
	Guard.deadlinedata.timerleft_second = Seconds_PerMinute - 1 ;	
 }

/********************************************************
 * 
 */
 static void UpdateWriteData(void)
{
	data_flash_write.bit_autoguard = (uint8_t)Guard.status_autoguard  ;
	data_flash_write.bit_GPSguard = (uint8_t)Guard.cmd_lock_From_FroceGPRS ;
	data_flash_write.timer_day_guard_timeleft = Guard.deadlinedata.timerleft_day.sum ;
	data_flash_write.timer_hour_guard_timeleft = Guard.deadlinedata.timerleft_hour ;
	data_flash_write.timer_minute_guard_timeleft = Guard.deadlinedata.timerleft_minute ;
	#if HistoryMileage_COMPILE
	data_flash_write.HistoryMileageINFlash= Mileage.odometer_perKM.sum32 ;
	#endif
	data_flash_write.fixvalue = 0XAA ;

 }

#if 0
/********************************************************
 * 
 */
void FlashTest(void)
{
	uint8 buf[10] = {0,};
	
	uint8 status = SUCCESS;
	//Read from SNV flash
	status = osal_snv_read(0x80, 10, (uint8 *)buf);
	if(status != SUCCESS)
	{
	//Write first time to initialize SNV ID
	osal_snv_write(0x80, 10, (uint8 *)buf);
	}
	//Increment value and write to SNV flash
	uint8 x;
	for(x = 0; x < 10; x++)
	buf[x] = x + 1;
	status = osal_snv_write(0x80, 10, (uint8 *)buf);
}
#endif

/*********************************************************
 * FlashOperationMessage_RegisterAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
bStatus_t FlashOperationMessage_RegisterAppCBs( flashOperationMessageCBs_t *appCallbacks )
{
	  if ( appCallbacks )
	  {
	    pAppCBs = appCallbacks;

	    return ( SUCCESS );
	  }
	  else
	  {
	    return ( bleAlreadyInRequestedMode );
	  }
}
/*******************************************************************************
* 函数名  	:
* 函数描述    	:
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
void NotifyFlashAction(const uint8_t action)
{
    if ( pAppCBs && pAppCBs->pfnNotifyCb)
      pAppCBs->pfnNotifyCb(action); // Call app function from stack task context.
}

/*******************************************************************************
*
*/
void SaveFlashWithResetMCU(void)
{
	NotifyFlashAction(Flash_Protection);

	timer_resetmcu_remain_per2ms = 5 ;		//at lest 9ms to erser and rewrite flash
}



#if TEST_EXTFLASH_COMPILE
/*******************************************************************************
*
*/
void SetFlagCheckExtFlash(void)
{
	flag_testExtFlash = true ;
}


/*******************************************************************************
*
*/
void CheckExtFlashIfFlagSet(void)
{
	if(flag_testExtFlash)
	{
		uint8_t result_testExtFlash = 0X01 ;
		flag_testExtFlash = false ;
		if(OADTarget_open())
		{
			result_testExtFlash = 0X01 ;
			OADTarget_close();
			
		}
		else
			result_testExtFlash = 0X00 ;
		if(IsBLEConnectStatusInteractive())
		{
			CheckAndResponseExtFlashResult(result_testExtFlash);
			SimpleBLEPeripheral_enqueueAsySendMsg();
		}

	}
}


#endif

