#include "defines.h"
#include "systemcontrol.h"
#include "MileageCal.h"
#include "BatteryPowerCal.h"
#include "OLC.h"
#include "KeyLess.h"
#include "BatteryPowerCal.h"
#include "Guard.h"

/*************************************************/
/*******************Variable**********************/
Mileage_TypeDef Mileage ;


/**********************function**********************/

#if(Commu_Mode_Common == Commu_Mode)
/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
void UpdateHallLevel(void)
{
	//LevelDetectionAndFilter已经带滤波
#if(BLE_E == PCB_VERSION )	
	//Mileage.Level.level1 = LevelDetectionAndFilter(Port_KEY1,Pin_KEY1) ;
	filter ;
#elif(BLE_G == PCB_VERSION )
	//Mileage.Level.level1 = LevelDetectionAndFilter(Port_HALLU,Pin_HALLU) ;
	filter ;
#else
#endif
	
	if(Mileage.Level.levelstatus != Mileage.Level.level1)
	{
		Mileage.Level.levelstatus = Mileage.Level.level1 ;
		Mileage.counter_temp_hallUchange_per500ms ++ ; 		
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
void UpdateHallSpeedPer500ms(void)
{
	Mileage.counter_hallchange_per500ms = Mileage.counter_temp_hallUchange_per500ms * 3 ;	//单霍尔->全霍尔
	Mileage.counter_hallchange_accumu.bit32 += Mileage.counter_hallchange_per500ms ; 		//累计霍尔
	GetHallSpeedFilteredValue();
	CalMileageRemainder();
	//Guard.status_park = AnalysisStateOfParkWhenConnectedWithController();
	
	Mileage.counter_temp_hallUchange_per500ms = 0 ;	
}

/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
void CalMileageRemainder(void)
{
	if(Mileage.counter_hallchange_remainder.bit32 > Mileage.counter_hallchange_per500ms)
		Mileage.counter_hallchange_remainder.bit32 -= Mileage.counter_hallchange_per500ms ;
	else
		Mileage.counter_hallchange_remainder.bit32 = 0 ;
}



#else
/*******************************************************************************
* 函数名  		: 
* 函数描述    	:
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
void CalQAndMileageReleaseAndCurrentAvr(void)                 //或者当PWM输出为0时
{
	Q.data_release_temp += OLCM.data_Q_thisframe ;
	
	//确保霍尔有变化的时候，有电流输出,防止人工推车的状态计入里程计算，引入误差
	//if( 0 != OLCM.data_Q_thisframe)
	//	Mileage.counter_hallchange_accumu.sum32 += OLCM.counter_hall_speed.sum ;

	//cal odometer
	//Mileage.counter_hallchange_accumu_under1KM.sum32 += OLCM.counter_hall_speed.sum  ;
	
/*
	if(Mileage.counter_hallchange_accumu.sum32 > Mileage_PerHundredM)
	{
		Mileage.counter_hallchange_accumu.sum32=0;
		Mileage.counter_hallchange_accumu_under1KM.sum32 += 1 ;
		Mileage.odometer_perKM.sum32 += 1 ;
	}
*/
	
}

/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
void CalCurrentReleaseAndHallChangeInFixedTime(void)                 
{
	Mileage.counter_hallchange_perfixedtime += OLCM.counter_hall_speed.sum ;
	Q.sum_in_fixedtime += OLCM.data_Q_thisframe;
	Mileage.counter_frame ++ ;
	if(Counter_Frame_Onelinecommunica_For_MileageCal == Mileage.counter_frame)
	{
		ReduceQRemainderBasedCurrent(); 
		//if(0 != Q.data_release_temp && 0 != Q.sum_in_fixedtime)
		
//		if((Q.data_release_temp > (Q.data_max.bit32 / 100 )  ) &&( Mileage.counter_hallchange_accumu.bit32 > (Mileage.counter_hallchange_max.bit32/100)))
//		{	
//			Mileage.counter_hallchange_remainder.bit32 = CalMileageRemainderBasedWeightedCal();
//			//Mileage.ratio_releasedivmileage_mul256 = GetRatioReleaseDivMileageMul256();
//			//Mileage.counter_hallchange_remainder.bit32 = GetMileageRemainderBasedRatioReleaseDivMileage();
//		}	
//		else
//		{
//			Mileage.counter_hallchange_remainder.bit32 = GetMileageRemainderBasedRatioReleaseDivMileage();
//		}
		
		//Mileage.counter_hallchange_remainder.bit32 = CalMileageRemainderBasedLowPassFilterRatioPerFrame();
		//Mileage.counter_hallchange_remainder.bit32 = CalMileageRemainderBasedRatedRatio();	
		//Mileage.counter_hallchange_remainder.bit32 = CalMileageBasedRatedRatio(Q.data_remainder.bit32);
		Mileage.counter_hallchange_remainder.sum32 = CalMileageRemainderBasedSubtract();
		Q.sum_in_fixedtime = 0 ;
		Mileage.counter_hallchange_perfixedtime  =0 ;
		Mileage.counter_frame = 0 ;
	} 
}

/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
uint32_t CalCounterHallchange1KM(void)
{
	//return (Mileage.counter_hallchange_max.sum32 / DevicePara.batterystandardKM) ;
	return (Mileage.counter_hallchange_max.sum32 / BatteryStandardM_InFlash) ;
}


#if 0
/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
/*
uint32_t CalMileageBasedRatedRatio(uint32_t Q_temp)
{	
	uint64_t temp_cal ;
	temp_cal = Q_temp ;
	temp_cal = temp_cal * Rated_HallChange_PerFrame/ Rated_Q_Release_PerFrame ;
	//temp_cal = temp_cal * 256 ;	//因为Mileage.counter_lowpassfilter_perframe大256倍，在上一个计算里面作为分母
	//temp_cal = temp_cal ;	//回复计算剩余里程的精度
	return (uint32_t)temp_cal ;
}
*/
#endif
///*******************************************************************************
//* 函数名  		: 
//* 函数描述    	: 
//* 输入参数      : 无
//* 输出结果      : 无
//* 返回值        : 无
//*******************************************************************************/
//uint32_t CalMileageRemainderBasedRatedRatio(uint32_t Q_temp)
//{	
//	uint64_t temp_cal ;
//	temp_cal = Q.data_remainder.bit32 ;
//	temp_cal = temp_cal * Rated_HallChange_PerFrame/ Rated_Q_Release_PerFrame ;
//	//temp_cal = temp_cal * 256 ;	//因为Mileage.counter_lowpassfilter_perframe大256倍，在上一个计算里面作为分母
//	//temp_cal = temp_cal ;	//回复计算剩余里程的精度
//	return (uint32_t)temp_cal ;
//}

/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
uint32_t CalMileageRemainderBasedSubtract(void)
{	
	if(Mileage.counter_hallchange_initial > Mileage.counter_hallchange_accumu.sum32)
		return (Mileage.counter_hallchange_initial - Mileage.counter_hallchange_accumu.sum32);
	else
		return 0 ;
}

/*******************************************************************************
* 函数名  		: 
* 函数描述    	: Q.data_remainder 32bit
				Q.data_lowpassfilter_perframe	(12+8)bit
				总位数 = 32+ 20 +8 =60bit
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
/*
uint32_t CalMileageRemainderBasedLowPassFilterRatioPerFrame(void)
{	
//	uint64_t temp_cal ;
//	temp_cal = Q.data_remainder.bit32 ;
//	temp_cal = temp_cal * Q.data_lowpassfilter_perframe \
//		/ Mileage.counter_lowpassfilter_perframe ;
//	//temp_cal = temp_cal * 256 ;	//因为Mileage.counter_lowpassfilter_perframe大256倍，在上一个计算里面作为分母
//	temp_cal = temp_cal >> LV_Accuracy_Reduced_ForQ ;	//回复计算剩余里程的精度
	uint64_t temp_cal ;
	temp_cal = Q.data_lowpassfilter_perframe >> LV_Accuracy_Reduced_ForQ  ;// Q.data_remainder.bit32 ;
	temp_cal = Q.data_remainder.sum32 * Mileage.counter_lowpassfilter_perframe\
		/temp_cal ;
	//temp_cal = temp_cal * 256 ;	//因为Mileage.counter_lowpassfilter_perframe大256倍，在上一个计算里面作为分母
	//temp_cal = temp_cal ;	//回复计算剩余里程的精度
	return (uint32_t)temp_cal ;
}	
*/		
/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
/*
uint32_t CalMileageRemainderBasedPersentOperatingStatus(void)
{
//	Mileage.counter_hallchange_remainder_basedpresent = Q.data_remainder.bit32\
//				/Current.sum_in_fixedtime * Mileage.counter_hallchange_perfixedtime   ;
	
	uint64_t temp_cal ;
	temp_cal = Q.data_remainder.sum32 ;
	temp_cal = temp_cal * Mileage.counter_hallchange_perfixedtime / Q.sum_in_fixedtime ;
	return (uint32_t)temp_cal ;
	
	//return (Q.data_remainder.bit32 * Mileage.counter_hallchange_perfixedtime / Q.sum_in_fixedtime)   ;
}
*/
/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
/*
uint32_t CalMileageRemainderBasedPreviousOperatingStatus(void)
{

//	Mileage.counter_hallchange_remainder_basedprevious = Q.data_remainder.bit32\
//				/ Q.data_release_temp * Mileage.counter_hallchange_accumu ;

	uint64_t temp_cal ;
	temp_cal = Q.data_remainder.sum32 ;
	temp_cal = temp_cal * Mileage.counter_hallchange_accumu.sum32 / Q.data_release_temp ;
	return (uint32_t)temp_cal ;
	//return (Q.data_remainder.bit32 * Mileage.counter_hallchange_accumu.bit32 / Q.data_release_temp) ;
}
*/
/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
/*
uint32_t CalMileageRemainderBasedWeightedCal(void)
{
//	if(0 == Mileage.counter_hallchange_remainder_basedpresent )
//		Mileage.counter_hallchange_remainder.bit32
//		= Mileage.counter_hallchange_remainder_basedprevious / Mileage_Remainder_Weighted_CardinalNumber* Mileage_Remainder_Weighted_Previous	;
//	else
//		Mileage.counter_hallchange_remainder.bit32
//		= Mileage.counter_hallchange_remainder_basedpresent /Mileage_Remainder_Weighted_CardinalNumber * Mileage_Remainder_Weighted_Persent \
//		+ Mileage.counter_hallchange_remainder_basedprevious/Mileage_Remainder_Weighted_CardinalNumber * Mileage_Remainder_Weighted_Previous	;	
//	if(0 == Q.sum_in_fixedtime )
//		 return (CalMileageRemainderBasedPreviousOperatingStatus() 
//			/ Mileage_Remainder_Weighted_CardinalNumber* Mileage_Remainder_Weighted_Previous )	;
//	else
//		return (CalMileageRemainderBasedPersentOperatingStatus() \
//			/Mileage_Remainder_Weighted_CardinalNumber * Mileage_Remainder_Weighted_Persent \
//				+ CalMileageRemainderBasedPreviousOperatingStatus()
//			/Mileage_Remainder_Weighted_CardinalNumber * Mileage_Remainder_Weighted_Previous )	;
//	
//	if(0 == Q.sum_in_fixedtime )
//		 return (CalMileageRemainderBasedPreviousOperatingStatus() );
//	else
//	{
//		uint64_t temp_cal;
//		temp_cal = CalMileageRemainderBasedPersentOperatingStatus();
//		temp_cal = temp_cal * Mileage_Remainder_Weighted_Persent / Mileage_Remainder_Weighted_CardinalNumber  \
//				+ CalMileageRemainderBasedPreviousOperatingStatus()\
//				* Mileage_Remainder_Weighted_Previous / Mileage_Remainder_Weighted_CardinalNumber ;
//		return (uint32_t)temp_cal ;
//	}
	
	if(0 == Q.sum_in_fixedtime )
		 return (CalMileageRemainderBasedPreviousOperatingStatus() );
	else
	{
		uint64_t temp_cal;
		temp_cal = CalMileageRemainderBasedPersentOperatingStatus();
		temp_cal = temp_cal * Mileage_Remainder_Weighted_Persent / Mileage_Remainder_Weighted_CardinalNumber  \
				+ GetMileageRemainderBasedRatioReleaseDivMileage()\
				* Mileage_Remainder_Weighted_Previous / Mileage_Remainder_Weighted_CardinalNumber ;
		return (uint32_t)temp_cal ;
	}
//		return (CalMileageRemainderBasedPersentOperatingStatus() \
//			* Mileage_Remainder_Weighted_Persent / Mileage_Remainder_Weighted_CardinalNumber  \
//				+ CalMileageRemainderBasedPreviousOperatingStatus()
//			* Mileage_Remainder_Weighted_Previous / Mileage_Remainder_Weighted_CardinalNumber  )	;
}

*/
/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
uint16_t GetRatioReleaseDivMileageMul256(void)
{
	uint64_t temp_cal ;
	temp_cal = Q.data_release_temp ;
	temp_cal = temp_cal * UINT8_MAX / Mileage.counter_hallchange_accumu.sum32 ;
	return (uint16_t)temp_cal ;
}

/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
/*
uint32_t GetMileageRemainderBasedRatioReleaseDivMileage(void)
{	
	uint64_t temp_cal ;
	temp_cal = Q.data_remainder.sum32 ;
	temp_cal = temp_cal * UINT8_MAX / Mileage.ratio_releasedivmileage_mul256 ;
//	temp_cal = (temp_cal * QDivMileage_Weighted_CumulativeOperationResults\
//				+ Mileage.ratio_releasedivmileage_mul256 * QDivMileage_Weighted_ThisRunResult )\
//				/ QDivMileage_Weighted_CardinalNumber	 ;
	
	return (uint32_t)temp_cal ;
	
	//return (Q.data_remainder.bit32 * Mileage.counter_hallchange_perfixedtime / Q.sum_in_fixedtime)   ;
}
*/

#endif




/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 用移动平均滤波法处理hallspeed
				当Num_HallSpeed_Filter =4时，通用版滤波覆盖时间为2s，其他版本滤波覆盖时间约为2.7s
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
void GetHallSpeedFilteredValue(void)
{
	static uint16_t arr[Num_HallSpeed_Filter]={0};
	uint32_t sum_temp = 0 ;
	for(uint8_t i = 0 ; i < (Num_HallSpeed_Filter - 1) ; i ++)
	{
		arr[i] = arr[i + 1] ;
		sum_temp += arr[i] ;
	}
#if(Commu_Mode_Common == Commu_Mode)	
	arr[Num_HallSpeed_Filter - 1] =  Mileage.counter_hallchange_per500ms ;
#else
	arr[Num_HallSpeed_Filter - 1] =  OLCM.counter_hall_speed.sum ;
#endif
	Mileage.counter_hall_speed_filtered = (uint16_t)((sum_temp + arr[Num_HallSpeed_Filter - 1])/ Num_HallSpeed_Filter) ;
}

/*******************************************************************************
* 函数名        :
* 函数描述    	:因为控制器计算有HallSpeed有余数（按照比例计算出），电机静止时hall速度是0x12 
因此设定hallspeed最小值，让hallspeed小于0x15时，显示为0
* 输入参数      : None
* 输出结果      : None
* 返回值        : None
*******************************************************************************/
uint16_t GetHallSpeed(void)
{	
#if(Commu_Mode_Common == Commu_Mode)
	return Mileage.counter_hall_speed_filtered ;
#else
	if(true == BLEMdlCnt.status_connect2controller)
		return OLCM.counter_hall_speed.sum ;
	//return Mileage.counter_hall_speed_filtered;			delay time is too long
	#if(KeyLessStart == Anti_theftDevice_Function)	
	if(IsACCPoweredOff())
	{
		if(High_Level == GetPinLevel(Pin_LD))
			return 0 ;
	}
	#endif
	return HallCounter_WhenYXTBroken ;
#endif
}

/*******************************************************************************
* 
*/
bool IsHallSpeedNearlyStop(void)
{
	if(Low_Level == GetPinLevel(Pin_LD))
		return false ;
	if(GetHallSpeed() > MinHall_WhenMotorRunning)
		return false ;
	return true ;
}




/*******************************************************************************
* 函数名  		: 
* 函数描述    	: run状态，timer_stall_500ms不断清零；非run状态，timer_stall_500ms不断累加。
				当	timer_stall_500ms累计到Time_Stall_Max 进入park状态				
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
StatusMotor_TypeDef GetMotorStatus(StatusMotor_TypeDef status_last)  
{
	if(false == IsHallSpeedNearlyStop())
	{
		Motor.timer_stall_500ms = 0 ;	
		return Status_Motor_Run ;
	}
	//GetHallSpeed() < MinHall_WhenMotorRunning
	if(Status_Motor_Run == status_last)
	{
		Motor.timer_stall_500ms = 0 ;	
#if(Commu_Mode_Common != Commu_Mode)			
		return Status_Motor_Stall ;
#else
	#error "need to code:5V == BLEMdlCnt.status_connect2controller"
#endif
	}
	if(Status_Motor_Stall == status_last)
	{
#if(Commu_Mode_Common != Commu_Mode)			
		if(Motor.timer_stall_500ms >= Time_Stall_Max )	//stall计时达到
			return Status_Motor_Park ;
#else
	#error "need to code:5V == BLEMdlCnt.status_connect2controller"
#endif		
	}
	return status_last ;
}






/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
void UpdateMileageInitialAndRemainder(void)
{
	//Q.soc_initial = GetBatterySoc();
	Mileage.counter_hallchange_accumu.sum32 = 0 ;
	
//	uint64_t temp_cal;
//	temp_cal = Q.data_remainder.bit32 ;
//	temp_cal = temp_cal * Rated_HallChange_PerFrame/ Rated_Q_Release_PerFrame ;
//	Mileage.counter_hallchange_initial = (uint32_t)temp_cal ;
	
//#if(Commu_Mode_Common == Commu_Mode)	
	//uint64_t cal_temp ;
	//cal_temp = GetBatterySoc();
	//cal_temp = cal_temp * Mileage.counter_hallchange_max.bit32 / (16 * 100) ;
	//Mileage.counter_hallchange_initial = cal_temp ;
//#else
	//Mileage.counter_hallchange_initial = CalMileageBasedRatedRatio(Q.data_remainder.sum32) ;
//#endif

	uint64_t cal_temp ;
	cal_temp = GetBatterySoc()/16 ;
	cal_temp = cal_temp * Mileage.counter_hallchange_max.sum32 /  100 ;
	Mileage.counter_hallchange_initial = cal_temp ;

	
	
	//Mileage.counter_hallchange_initial = Mileage.counter_hallchange_max.bit32 * Q.soc_initial / 100  ;
	
	Mileage.counter_hallchange_remainder.sum32 = Mileage.counter_hallchange_initial ;
}

/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
/*void MileageInitalAndQuitCalMileageMax(void)
{
	InitialMileage();
}*/

/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
/*void UpdataMileageRemainder(void)
{
	InitialMileage();
}*/

/*******************************************************************************
* 函数名        :
* 函数描述    	:
* 输入参数      : None
* 输出结果      : None
* 返回值        : None
*******************************************************************************/
/*bool AnalysisStateOfParkWhenConnectedWithController(void)
{
#if(Commu_Mode_Common == Commu_Mode)
	if(0 != Mileage.counter_hallchange_per500ms)
#else
	if(true == OLCM.flag_motor_running)
#endif
	   	return false ;
	else
	 	return true ;
}*/

#if 0
/*******************************************************************************
* 函数名  		: 
* 函数描述    	: Mmax为加权计算出 大部分是原来Mmax 小部分是根据本次运行结果算出的Mmax
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
/*
uint32_t CalMileageMax(void)
{
#if(Commu_Mode_Common == Commu_Mode)
	if( Q.soc_release > 0 )
	{
		uint64_t temp_cal;
		temp_cal = Mileage.counter_hallchange_accumu.bit32  ;
		temp_cal = temp_cal * 16 * 100 / Q.soc_release ;
		return (uint32_t)temp_cal ;
	}
	//	return (Mileage.counter_hallchange_accumu.bit32 * 100 / Q.soc_release )  ;
	else

		return Mileage.counter_hallchange_max.bit32  ;
#else
	if( Q.soc_release > 0)
	{
		
		uint64_t temp_cal;
		temp_cal = Mileage.counter_hallchange_accumu.bit32  ;
	    temp_cal = temp_cal * Mileage_Max_Weighted_CumulativeOperationResults / Mileage_Max_Weighted_CardinalNumber \
				+ Mileage.counter_hallchange_accumu.bit32 * 100 * Mileage_Max_Weighted_ThisRunResult\
				/ Q.soc_release / Mileage_Max_Weighted_CardinalNumber ;
		return (uint32_t)temp_cal ;
		
		uint64_t temp_cal;
	    temp_cal = Q.data_max.bit32 ;
		temp_cal = temp_cal * Rated_HallChange_PerFrame/ Rated_Q_Release_PerFrame ;
		return (uint32_t)temp_cal ;
		uint64_t temp_cal;
		temp_cal = Q.data_max.bit32 ;
		temp_cal = temp_cal * Rated_HallChange_PerFrame/ Rated_Q_Release_PerFrame ;
		return CalMileageBasedRatedRatio(Q.data_max.sum32) ;
		
	}
//	return ( Mileage.counter_hallchange_max.bit32  \
//		* Mileage_Max_Weighted_CumulativeOperationResults / Mileage_Max_Weighted_CardinalNumber \
//		+ Mileage.counter_hallchange_accumu.bit32 * 100 * Mileage_Max_Weighted_ThisRunResult\
//			/ Q.soc_release / Mileage_Max_Weighted_CardinalNumber ) ;	
	else
		return Mileage.counter_hallchange_max.sum32  ;
	//  UpdateMileageMaxInFlash();
#endif	
}
*/
#endif

