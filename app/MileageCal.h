/*
1.对于未指定控制器为东大蓝鸟控制器的蓝牙模块来说：
	单独取出一根霍尔线，累计的霍尔数为该霍尔线0.5s内累计的数目



2.对于指定控制器为东大蓝鸟控制器的蓝牙模块来说：
	霍尔数为一线通发出的0.5s内霍尔数目
	Attention：这里的霍尔数是以一个周期6次霍尔变化来判断的。

一线通一帧数据时间为0.678s

以闫工的电动车为例子 ，全速电流为8A 时速为32km
指定控制器为东大，电动车能够骑行的时间为time=Qremainder*0.678s/8A
		里程=time*25*1000/3600
*/

#ifndef __MILEAGE_H
#define __MILEAGE_H

/****************************************************/
/*********************typedef************************/

typedef	struct
{
#if(Commu_Mode_Common == Commu_Mode)
	Level_Type Level	;
	uint16_t counter_temp_hallUchange_per500ms ;
	uint16_t counter_hallchange_per500ms ;
#else
	uint8_t counter_frame ;
	uint32_t counter_hallchange_perfixedtime ;
	uint32_t counter_hallchange_remainder_basedpresent ;
	uint32_t counter_hallchange_remainder_basedprevious ;	
	uint32_t counter_lowpassfilter_perframe ;
	
	
	uint16_t ratio_releasedivmileage_mul256 ;		//为了精度 该比例是乘以256的 计算的时候要去除256的影响
#endif
	uint16_t counter_hall_speed_filtered ;
	Union4Bytes_TypeDef counter_hallchange_accumu ;
	Union4Bytes_TypeDef counter_hallchange_max ;
	uint32_t counter_hallchange_initial ;
	Union4Bytes_TypeDef counter_hallchange_remainder ;//因为需要把该参数用蓝牙发送，因此需要转换格式
	Union4Bytes_TypeDef odometer_perKM ;
	uint32_t caledvalue_hallchange_1KM ;
	Union4Bytes_TypeDef counter_hallchange_accumu_under1KM ;
	#if HistoryMileage_COMPILE   
	float  Meter;
    #endif
}Mileage_TypeDef;

	
	
/******************define**********************/
#if(Commu_Mode_Common == Commu_Mode)
	#define	HallFrameTime_Compensation_Ratio						1
#else
	#define	Counter_Frame_Onelinecommunica_For_MileageCal			15//177
	#define	Timer_PerFrame_Onelinecommunica_For_MileageCal			0.678	//s
	#define	Timer_HallSpeedPerFrame_Onelinecommunica_For_MileageCal	0.5	//s
	#define	HallFrameTime_Compensation_Ratio	(Timer_HallSpeedPerFrame_Onelinecommunica_For_MileageCal\
													/ Timer_PerFrame_Onelinecommunica_For_MileageCal)
	#define	Mileage_Remainder_Weighted_CardinalNumber				10
	#define	Mileage_Remainder_Weighted_Persent						1
	#define	Mileage_Remainder_Weighted_Previous				(Mileage_Remainder_Weighted_CardinalNumber-Mileage_Remainder_Weighted_Persent)
	#define	Mileage_Max_Weighted_CardinalNumber					10
	#define	Mileage_Max_Weighted_CumulativeOperationResults		8
	#define	Mileage_Max_Weighted_ThisRunResult				\
		(Mileage_Max_Weighted_CardinalNumber-Mileage_Max_Weighted_CumulativeOperationResults)	
	#define	QDivMileage_Weighted_CardinalNumber					10
	#define	QDivMileage_Weighted_CumulativeOperationResults		9
	#define	QDivMileage_Weighted_ThisRunResult				\
		(QDivMileage_Weighted_CardinalNumber- QDivMileage_Weighted_Weighted_CumulativeOperationResults)			
			
#endif

			
		

#define	Gear_Ratio						(Gear_Ratio_Motor_InFlash / Gear_Ratio_Wheel_InFlash)

#define Perimeter_Wheel_Meter			(PI * (Wheel_Diameter_Inch_Int_InFlash + Wheel_Diameter_Inch_Dec_InFlash) * 2.54 /100)//1.3
#define HallNum_Per_Round               (6 * Number_Pole_Pairs_InFlash * Gear_Ratio)
#define Mileage_PerKm					(uint32_t)(1000 * HallFrameTime_Compensation_Ratio * Gear_Ratio *\
										Number_Pole_Pairs_InFlash * 6 / Perimeter_Wheel_Meter)
#define SpeedM_PerSecond				(Perimeter_Wheel_Meter/HallNum_Per_Round)
//#define Mileage_Max						(uint32_t)(BatteryStandardM  *\
//										HallFrameTime_Compensation_Ratio * Gear_Ratio *\
//										Number_Pole_Pairs * 6 / Perimeter_Wheel_Meter)
#define Mileage_Max_InFlash					(uint32_t)(BatteryStandardM_InFlash * HallNum_Per_Round\
                                            * HallFrameTime_Compensation_Ratio/ Perimeter_Wheel_Meter)

/*#define Mileage_Max_12Ah				(uint32_t)(Mileage_Max_20Ah * 12 / 20)
#if(20 == AH_Battery)
	#define Mileage_Max						Mileage_Max_20Ah
#elif(12 == AH_Battery)
	#define Mileage_Max						Mileage_Max_12Ah
#else
#endif*/
#define Mileage_max_Flash_Dleta_MAX		((uint32_t)(Mileage_Max_InFlash/100))

#define	BatteryStandardKM_InFlash			(uint8_t)(BatteryStandardM_InFlash/1000)

/*******************Rated************************/
#define	Rated_Q_Release_PerFrame		(uint8_t)(Rated_Q_Release_PerFrame_A_InFlash * 4)    
//#define	Rated_Q_Release_PerFrame		(60*(AverageVoltage_CalByEnergy_D >> LV_Accuracy_Reduced_ForQ))
//#define	Rated_HallChange_PerFrame		587

/*******************************************************************************************************************
*              总里程（50000m）                                              轮子转一圈的霍尔数（HallNum_Per_Round）
*    公式 = ———————————————————————————— * 一线通中hall速度的时间（0.5s）*————————————————————————————————————————————
*             安时数（20AH）                                                 轮子一圈的周长（Perimeter_Wheel_Meter）
*            —————————————— * 3600
*              均值电流（A）  

***********************************************************************************************************************/
#if 0
#define	Rated_HallChange_PerFrame		(uint16_t)(BatteryStandardM_InFlash * Rated_Q_Release_PerFrame_A_InFlash\
                                        * Timer_HallSpeedPerFrame_Onelinecommunica_For_MileageCal\
                                        * HallNum_Per_Round / (Perimeter_Wheel_Meter\
                                        * AH_Battery_InFlash * TimePeriod_1h_Per1s))
#endif

/*******************Variable**********************/
/******************Mileage**********************/
extern Mileage_TypeDef Mileage ;

/*******************Function*********************/
#if(Commu_Mode_Common == Commu_Mode)	
	void UpdateHallLevel(void);
	//void ReadHallLevel1AndLevel2(void);
	void UpdateHallSpeedPer500ms(void);
	void CalMileageRemainder(void) ;
#else
	void CalQAndMileageReleaseAndCurrentAvr(void)  ;
	void CalCurrentReleaseAndHallChangeInFixedTime(void);
	uint32_t CalCounterHallchange1KM(void) ;
#if 0
	uint32_t CalMileageBasedRatedRatio(uint32_t) ;
#endif
	uint32_t CalMileageRemainderBasedSubtract(void);
	//uint32_t CalMileageRemainderBasedRatedRatio(void) ;
	uint32_t CalMileageRemainderBasedLowPassFilterRatioPerFrame(void);
	uint32_t CalMileageRemainderBasedPersentOperatingStatus(void);
	uint32_t CalMileageRemainderBasedPreviousOperatingStatus(void);
	uint32_t CalMileageRemainderBasedWeightedCal(void);
	uint16_t GetRatioReleaseDivMileageMul256(void) ;
	uint32_t GetMileageRemainderBasedRatioReleaseDivMileage(void) ;
#endif

void GetHallSpeedFilteredValue(void) ;	
uint16_t GetHallSpeed(void);
bool IsHallSpeedNearlyStop(void) ;
#if 0
uint32_t CalMileageMax(void) ;
#endif
void UpdateMileageInitialAndRemainder(void);
//void MileageInitalAndQuitCalMileageMax(void);
//void UpdataMileageRemainder(void);
//bool AnalysisStateOfParkWhenConnectedWithController(void) ;






#endif