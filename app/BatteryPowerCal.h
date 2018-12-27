#ifndef __BATTERYPOWERCAL_H
#define __BATTERYPOWERCAL_H

#include "OLC.h"
/*****************************************************/
/*******************Annotate*********************************
*	一线通间隔时间约为0.6784s
*	一线通中电流信息为除以4为真实值
*	20Ah的电池data_max= 424528
*	

****************************************************/
/*********************typedef************************/
/*typedef enum
{ 
	Status_UnConnected_AfterOpCal  =0 ,
	Status_Connected ,
	Status_UnConnected_BeforeFirstAD,
	Status_UnConnected_BeforeSecondAD,
}StatusPowerConnect;*/

typedef struct
{
	uint8_t 	id ;
	uint32_t	value_single_sample ;
	uint64_t	sum_temp ;
	uint8_t 	counter_sample ;
	uint16_t	average_value ;
	const uint8_t fix_counter_sample ;
	const uint8_t fix_counter_shift ;
	bool		flag_cal ;
}ADCSample_TypeDef ;



typedef	struct
{
	/******************status*************************/
	//StatusPowerConnect status_opencircuit ;
	
	/***********************LV***********************/
	//uint8_t  counter_sample ;
	//uint16_t sum_temp_adc  ;
	//uint16_t avr_adc  ;		
	//uint16_t data_45min_afteropen; 
	//uint16_t data_1h_afteropen  ;	
	//uint16_t timer_openciruit_1s ;	
	bool flag_batterycharged ;
}LV_TypeDef;

typedef	struct
{
	uint16_t soc_initial  ;
	//uint8_t soc_1h_afteropen ;
	uint16_t	soc_release;
	Union4Bytes_TypeDef data_max   ;
	uint8_t soc_percent ;
	
#if(Commu_Mode_Common != Commu_Mode)
	uint32_t data_release_temp  ;
	uint32_t data_initial  ;
	Union4Bytes_TypeDef data_remainder  ;	//因为需要把该参数用蓝牙发送，因此需要转换格式
	uint32_t sum_in_fixedtime ;
	uint64_t data_lowpassfilter_perframe ;
#endif
	
	
}Q_TypeDef;

typedef	struct
{
	UnionSumWithH8L8_TypeDef sum_in_174s ;
	uint16_t avr_in_174s ;
	uint8_t counter ;
	//	#if(Commu_Mode_Common != Commu_Mode)
	////	uint16_t sum_in_fixedtime ;
	//  	#endif
	
}Current_TypeDef;







/*****************************************************/
/**********************define************************/
/***********************soc*************************/
#define SOC_MAX					100
#define SOC_MIN	                0

/********************Dividing resistor***************/
#if(LV_Grade == 36)						
	#define R7			(24000)			//24k
	#define R9			(1500)			//1.5k
#elif(LV_Grade == 48)						
	#define R7			(33000)			//33k
	#define R9			(1300)			//1.3k
#elif(60 ==	LV_Grade)
	#define	R7			(24000)			//24k
	#define R9			(750)			//0.75k
#elif(72 ==	LV_Grade)
	#define	R7			(100000)//(62000)		100K	//62k
	#define R9			(3000)//(1600)			3K//1.6k
#else
#endif

#define ADC_RSum		(R7 + R9)

/*****************LV ANI and Digital****************/
#if defined(STM32) 
#define LVDigital2Analog			((R7 + R9)* 3.3 /( R9 * 4096)
#define LVAnalog2Digital			(R9 * 4096 / ((R7 + R9) * 3.3))
#endif

#if defined(CC26XX)
#define LVDigital2Analog			((R7 + R9) /( R9 * 1000))
#define LVAnalog2Digital			(R9 * 1000 / (R7 + R9) )	
#endif

/************Volatage single_cell********************/
#define	Lead_Acid_LV_OPENCRI_MIN_Single_Cell_A			11.5	//46/4
#define Lead_Acid_LV_OPENCRI_MAX_Single_Cell_A			13.7	//54.8/4
#define	Lead_Acid_LV_FlashProtect_Single_Cell_A			10.0	//40/4
#define	Lead_Acid_LV_FlashProtect_Schmidt_Single_Cell_A	10.25	//41/4	

#define Lead_Acid_LV_OPENCRI_MIN_A			(Lead_Acid_LV_OPENCRI_MIN_Single_Cell_A * Lead_Acid_Battery_Section_Num)
#define Lead_Acid_LV_OPENCRI_MAX_A			(Lead_Acid_LV_OPENCRI_MAX_Single_Cell_A * Lead_Acid_Battery_Section_Num) 
#define Lead_Acid_LV_FlashProtect_A			(Lead_Acid_LV_FlashProtect_Single_Cell_A * Lead_Acid_Battery_Section_Num)
#define	Lead_Acid_LV_FlashProtect_Schmidt_A	(Lead_Acid_LV_FlashProtect_Schmidt_Single_Cell_A * Lead_Acid_Battery_Section_Num)


//因为有除法，所以起始值应该比LV_OPENCRI_MIN_A大
#define	Lead_Acid_LV_OPENCRI_MIN_D			(uint16_t)(Lead_Acid_LV_OPENCRI_MIN_A * LVAnalog2Digital)//(uint16_t)(SOC_FACTOR3 * SOC_FACTOR2 / SOC_FACTOR1 + 1)//
#define	Lead_Acid_LV_OPENCRI_MAX_D			(uint16_t)(Lead_Acid_LV_OPENCRI_MAX_A * LVAnalog2Digital)
#define Lead_Acid_LV_FlashProtect_D			(uint16_t)(Lead_Acid_LV_FlashProtect_A * LVAnalog2Digital)
#define	Lead_Acid_LV_FlashProtect_Schmidt_D (uint16_t)(Lead_Acid_LV_FlashProtect_Schmidt_A * LVAnalog2Digital)
#define Lead_Acid_LV_Uncharging_Max_D			(uint16_t)(Lead_Acid_LV_OPENCRI_MAX_A * LVAnalog2Digital)
#define	Lead_Acid_LV_Grade_D					(uint16_t)(Lead_Acid_LV_Grade * LVAnalog2Digital)

#define	SOC_FACTOR1					7
#define	SOC_FACTOR2					29
#define	SOC_FACTOR3					522

#if(COMPILE_ENABLE == Double_LV)
		#define	Lead_Acid_LV_Level1_Single_Cell_A				11.25		//45/4
		#define	Lead_Acid_LV_Level2_Single_Cell_A				10.5		//42/4
		#define	Lead_Acid_LV_Level1Recover_Single_Cell_A		12			//48/4

		#define Lead_Acid_LV_Level1_A					(Lead_Acid_LV_Level1_Single_Cell_A * Battery_Section_Num)
		#define Lead_Acid_LV_Level2_A					(Lead_Acid_LV_Level2_Single_Cell_A * Battery_Section_Num)
		#define	Lead_Acid_LV_Level1Recover_A			(Lead_Acid_LV_Level1Recover_Single_Cell_A * Battery_Section_Num)

		#define  Lead_Acid_LV_Level1_D					(uint16_t)(Lead_Acid_LV_Level1_A * LVAnalog2Digital)
		#define  Lead_Acid_LV_Level2_D					(uint16_t)(Lead_Acid_LV_Level2_A * LVAnalog2Digital)
		#define  Lead_Acid_LV_Level1Recover_D			(uint16_t)(Lead_Acid_LV_Level1Recover_A * LVAnalog2Digital)
#endif







/************Volatage single_cell********************/
/*	整个电池在15A恒流放电的过程中，电压到达52.35V时停止，
* 	里面有某组提前达到2.8V。因此认为整块电池的最低soc开路电压为54V
*	因为整块电池开始会快速下降电压，放电15A到达62.6625V时会趋于平稳
* 	因此认为整块电池的最高soc的开路电压为（62.6625+1.65 = 64.3125V）
***************************************************/ 
#define	MnNiCo_LV_FlashProtect_Single_Cell_A				2.85
#define	MnNiCo_LV_FlashProtect_Schmidt_Single_Cell_A		2.9
	
#define MnNiCo_LV_FlashProtect_Con_A						(MnNiCo_LV_FlashProtect_Single_Cell_A * MnNiCo_Battery_Con_Section_Num)
#define	MnNiCo_LV_FlashProtect_Schmidt_Con_A				(MnNiCo_LV_FlashProtect_Schmidt_Single_Cell_A* MnNiCo_Battery_Con_Section_Num)




	
#define MnNiCo_LV_FlashProtect_UnCon_A						(MnNiCo_LV_FlashProtect_Single_Cell_A * MnNiCo_Battery_UnCon_Section_Num)
#define	MnNiCo_LV_FlashProtect_Schmidt_UnCon_A				(MnNiCo_LV_FlashProtect_Schmidt_Single_Cell_A* MnNiCo_Battery_UnCon_Section_Num)



#if defined(STM32)	
#define MnNiCo_LV_FlashProtect_Con_D						(uint16_t)(MnNiCo_LV_FlashProtect_Con_A * LVAnalog2Digital)
#define	MnNiCo_LV_FlashProtect_Schmidt_Con_D				(uint16_t)(MnNiCo_LV_FlashProtect_Schmidt_Con_A * LVAnalog2Digital)

#define MnNiCo_LV_FlashProtect_UnCon_D						(uint16_t)(MnNiCo_LV_FlashProtect_UnCon_A * LVAnalog2Digital)
#define	MnNiCo_LV_FlashProtect_Schmidt_UnCon_D				(uint16_t)(MnNiCo_LV_FlashProtect_Schmidt_UnCon_A * LVAnalog2Digital)
#endif

#if defined(CC26XX)	
#define MnNiCo_LV_FlashProtect_Con_D						(uint16_t)(MnNiCo_LV_FlashProtect_Con_A * R9 * 1000 / ADC_RSum)
#define	MnNiCo_LV_FlashProtect_Schmidt_Con_D				(uint16_t)(MnNiCo_LV_FlashProtect_Schmidt_Con_A * R9 * 1000 / ADC_RSum)

#define MnNiCo_LV_FlashProtect_UnCon_D						(uint16_t)(MnNiCo_LV_FlashProtect_UnCon_A * R9 * 1000 / ADC_RSum)
#define	MnNiCo_LV_FlashProtect_Schmidt_UnCon_D				(uint16_t)(MnNiCo_LV_FlashProtect_Schmidt_UnCon_A * R9 * 1000 / ADC_RSum)
#endif






/***********************LV OPENCRI*********************/


/*********************CHILWEE***********************/	
/*单节高压	4.19V 		单节低压	3.375V
*	6个点 	54V			0%;	
			55.64063V	8.063668%;	
			58.375V		50.29196%;	
			64.78125V	97.2119%;	
			67.04V		100%
*			15A负载下						56.725V				63.13125				65.43
******************************************************/
#define MnNiCo_LV_OPENCRI_MIN_Single_Cell_A_CHILWEE       		(69.523/20)
#define MnNiCo_LV_OPENCRI_MAX_Single_Cell_A_CHILWEE				4.187//4.19//64.3125/16	66.4/16
#define	MnNiCo_LV_K_Turning_Point1_Single_Cell_A_CHILWEE		(56.512 / 16)		//56.512/16
#define	MnNiCo_LV_K_Turning_Point2_Single_Cell_A_CHILWEE		(59.296 / 16)
#define	MnNiCo_LV_K_Turning_Point3_Single_Cell_A_CHILWEE		(62.592 / 16)
#define	MnNiCo_LV_K_Turning_Point4_Single_Cell_A_CHILWEE		(64.592 / 16)

/*conventional battery num*/
#define MnNiCo_LV_OPENCRI_MIN_A_Con_CHILWEE				(MnNiCo_LV_OPENCRI_MIN_Single_Cell_A_CHILWEE * MnNiCo_Battery_Con_Section_Num)
#define MnNiCo_LV_OPENCRI_MAX_A_Con_CHILWEE				(MnNiCo_LV_OPENCRI_MAX_Single_Cell_A_CHILWEE * MnNiCo_Battery_Con_Section_Num) 
#define	MnNiCo_LV_K_Turning_Point1_A_Con_CHILWEE		(MnNiCo_LV_K_Turning_Point1_Single_Cell_A_CHILWEE * MnNiCo_Battery_Con_Section_Num)// 56.512 (55.64063)
#define	MnNiCo_LV_K_Turning_Point2_A_Con_CHILWEE		(MnNiCo_LV_K_Turning_Point2_Single_Cell_A_CHILWEE * MnNiCo_Battery_Con_Section_Num)// 59.296 (58.375)
#define	MnNiCo_LV_K_Turning_Point3_A_Con_CHILWEE		(MnNiCo_LV_K_Turning_Point3_Single_Cell_A_CHILWEE * MnNiCo_Battery_Con_Section_Num)// 62.592 (64.78125)
#define	MnNiCo_LV_K_Turning_Point4_A_Con_CHILWEE		(MnNiCo_LV_K_Turning_Point4_Single_Cell_A_CHILWEE * MnNiCo_Battery_Con_Section_Num)// 64.592

#define	MnNiCo_LV_OPENCRI_MIN_D_Con_CHILWEE				(uint16_t)(MnNiCo_LV_OPENCRI_MIN_A_Con_CHILWEE * LVAnalog2Digital)//(uint16_t)(SOC_FACTOR3 * SOC_FACTOR2 / SOC_FACTOR1 + 1)//
#define	MnNiCo_LV_OPENCRI_MAX_D_Con_CHILWEE				(uint16_t)(MnNiCo_LV_OPENCRI_MAX_A_Con_CHILWEE * LVAnalog2Digital)
#define	MnNiCo_LV_K_Turning_Point1_D_Con_CHILWEE		(uint16_t)(MnNiCo_LV_K_Turning_Point1_A_Con_CHILWEE * LVAnalog2Digital)
#define	MnNiCo_LV_K_Turning_Point2_D_Con_CHILWEE		(uint16_t)(MnNiCo_LV_K_Turning_Point2_A_Con_CHILWEE * LVAnalog2Digital)	
#define	MnNiCo_LV_K_Turning_Point3_D_Con_CHILWEE		(uint16_t)(MnNiCo_LV_K_Turning_Point3_A_Con_CHILWEE * LVAnalog2Digital)
#define	MnNiCo_LV_K_Turning_Point4_D_Con_CHILWEE		(uint16_t)(MnNiCo_LV_K_Turning_Point4_A_Con_CHILWEE * LVAnalog2Digital)




/*unconventional battery num*/
#define MnNiCo_LV_OPENCRI_MIN_A_UnCon_CHILWEE			(MnNiCo_LV_OPENCRI_MIN_Single_Cell_A_CHILWEE * MnNiCo_Battery_UnCon_Section_Num)
#define MnNiCo_LV_OPENCRI_MAX_A_UnCon_CHILWEE			(MnNiCo_LV_OPENCRI_MAX_Single_Cell_A_CHILWEE * MnNiCo_Battery_UnCon_Section_Num) 
#define	MnNiCo_LV_K_Turning_Point1_A_UnCon_CHILWEE		(MnNiCo_LV_K_Turning_Point1_Single_Cell_A_CHILWEE * MnNiCo_Battery_UnCon_Section_Num)// 56.512 (55.64063)
#define	MnNiCo_LV_K_Turning_Point2_A_UnCon_CHILWEE		(MnNiCo_LV_K_Turning_Point2_Single_Cell_A_CHILWEE * MnNiCo_Battery_UnCon_Section_Num)// 59.296 (58.375)
#define	MnNiCo_LV_K_Turning_Point3_A_UnCon_CHILWEE		(MnNiCo_LV_K_Turning_Point3_Single_Cell_A_CHILWEE * MnNiCo_Battery_UnCon_Section_Num)// 62.592 (64.78125)
#define	MnNiCo_LV_K_Turning_Point4_A_UnCon_CHILWEE		(MnNiCo_LV_K_Turning_Point4_Single_Cell_A_CHILWEE * MnNiCo_Battery_UnCon_Section_Num)// 64.592

#define	MnNiCo_LV_OPENCRI_MIN_D_UnCon_CHILWEE			(uint16_t)(MnNiCo_LV_OPENCRI_MIN_A_UnCon_CHILWEE * LVAnalog2Digital)//(uint16_t)(SOC_FACTOR3 * SOC_FACTOR2 / SOC_FACTOR1 + 1)//
#define	MnNiCo_LV_OPENCRI_MAX_D_UnCon_CHILWEE			(uint16_t)(MnNiCo_LV_OPENCRI_MAX_A_UnCon_CHILWEE * LVAnalog2Digital)
#define	MnNiCo_LV_K_Turning_Point1_D_UnCon_CHILWEE		(uint16_t)(MnNiCo_LV_K_Turning_Point1_A_UnCon_CHILWEE * LVAnalog2Digital)
#define	MnNiCo_LV_K_Turning_Point2_D_UnCon_CHILWEE		(uint16_t)(MnNiCo_LV_K_Turning_Point2_A_UnCon_CHILWEE * LVAnalog2Digital)	
#define	MnNiCo_LV_K_Turning_Point3_D_UnCon_CHILWEE		(uint16_t)(MnNiCo_LV_K_Turning_Point3_A_UnCon_CHILWEE * LVAnalog2Digital)
#define	MnNiCo_LV_K_Turning_Point4_D_UnCon_CHILWEE		(uint16_t)(MnNiCo_LV_K_Turning_Point4_A_UnCon_CHILWEE * LVAnalog2Digital)


/*********************PHYLION***********************/	
/*单节高压	4.2V 		单节低压	2.8V
*	6个点 
	2.8V		0%;	
	3.47V		5%;	
	3.63V		10%;	
	3.85V		35%;	
	4.11V		95%
	4.2V		100%
******************************************************/
#define MnNiCo_LV_OPENCRI_MIN_Single_Cell_A_PHYLION       		(2.8)
#define MnNiCo_LV_OPENCRI_MAX_Single_Cell_A_PHYLION				(4.2)
#define	MnNiCo_LV_K_Turning_Point1_Single_Cell_A_PHYLION		(3.47)		//56.512/16
#define	MnNiCo_LV_K_Turning_Point2_Single_Cell_A_PHYLION		(3.63)
#define	MnNiCo_LV_K_Turning_Point3_Single_Cell_A_PHYLION		(3.85)
#define	MnNiCo_LV_K_Turning_Point4_Single_Cell_A_PHYLION		(4.11)

/*conventional battery num*/
#define MnNiCo_LV_OPENCRI_MIN_A_Con_PHYLION				(MnNiCo_LV_OPENCRI_MIN_Single_Cell_A_PHYLION * MnNiCo_Battery_Con_Section_Num)
#define MnNiCo_LV_OPENCRI_MAX_A_Con_PHYLION				(MnNiCo_LV_OPENCRI_MAX_Single_Cell_A_PHYLION * MnNiCo_Battery_Con_Section_Num) 
#define	MnNiCo_LV_K_Turning_Point1_A_Con_PHYLION		(MnNiCo_LV_K_Turning_Point1_Single_Cell_A_PHYLION * MnNiCo_Battery_Con_Section_Num)// 56.512 (55.64063)
#define	MnNiCo_LV_K_Turning_Point2_A_Con_PHYLION		(MnNiCo_LV_K_Turning_Point2_Single_Cell_A_PHYLION * MnNiCo_Battery_Con_Section_Num)// 59.296 (58.375)
#define	MnNiCo_LV_K_Turning_Point3_A_Con_PHYLION		(MnNiCo_LV_K_Turning_Point3_Single_Cell_A_PHYLION * MnNiCo_Battery_Con_Section_Num)// 62.592 (64.78125)
#define	MnNiCo_LV_K_Turning_Point4_A_Con_PHYLION		(MnNiCo_LV_K_Turning_Point4_Single_Cell_A_PHYLION * MnNiCo_Battery_Con_Section_Num)// 64.592

	/*unconventional battery num*/
#define MnNiCo_LV_OPENCRI_MIN_A_UnCon_PHYLION			(MnNiCo_LV_OPENCRI_MIN_Single_Cell_A_PHYLION * MnNiCo_Battery_UnCon_Section_Num)
#define MnNiCo_LV_OPENCRI_MAX_A_UnCon_PHYLION			(MnNiCo_LV_OPENCRI_MAX_Single_Cell_A_PHYLION * MnNiCo_Battery_UnCon_Section_Num) 
#define	MnNiCo_LV_K_Turning_Point1_A_UnCon_PHYLION		(MnNiCo_LV_K_Turning_Point1_Single_Cell_A_PHYLION * MnNiCo_Battery_UnCon_Section_Num)// 56.512 (55.64063)
#define	MnNiCo_LV_K_Turning_Point2_A_UnCon_PHYLION		(MnNiCo_LV_K_Turning_Point2_Single_Cell_A_PHYLION * MnNiCo_Battery_UnCon_Section_Num)// 59.296 (58.375)
#define	MnNiCo_LV_K_Turning_Point3_A_UnCon_PHYLION		(MnNiCo_LV_K_Turning_Point3_Single_Cell_A_PHYLION * MnNiCo_Battery_UnCon_Section_Num)// 62.592 (64.78125)
#define	MnNiCo_LV_K_Turning_Point4_A_UnCon_PHYLION		(MnNiCo_LV_K_Turning_Point4_Single_Cell_A_PHYLION * MnNiCo_Battery_UnCon_Section_Num)// 64.592

#if defined(STM32)	
#define	MnNiCo_LV_OPENCRI_MIN_D_Con_PHYLION				(uint16_t)(MnNiCo_LV_OPENCRI_MIN_A_Con_PHYLION * LVAnalog2Digital)//(uint16_t)(SOC_FACTOR3 * SOC_FACTOR2 / SOC_FACTOR1 + 1)//
#define	MnNiCo_LV_OPENCRI_MAX_D_Con_PHYLION				(uint16_t)(MnNiCo_LV_OPENCRI_MAX_A_Con_PHYLION * LVAnalog2Digital)
#define	MnNiCo_LV_K_Turning_Point1_D_Con_PHYLION		(uint16_t)(MnNiCo_LV_K_Turning_Point1_A_Con_PHYLION * LVAnalog2Digital)
#define	MnNiCo_LV_K_Turning_Point2_D_Con_PHYLION		(uint16_t)(MnNiCo_LV_K_Turning_Point2_A_Con_PHYLION * LVAnalog2Digital)	
#define	MnNiCo_LV_K_Turning_Point3_D_Con_PHYLION		(uint16_t)(MnNiCo_LV_K_Turning_Point3_A_Con_PHYLION * LVAnalog2Digital)
#define	MnNiCo_LV_K_Turning_Point4_D_Con_PHYLION		(uint16_t)(MnNiCo_LV_K_Turning_Point4_A_Con_PHYLION * LVAnalog2Digital)
	
#define	MnNiCo_LV_OPENCRI_MIN_D_UnCon_PHYLION			(uint16_t)(MnNiCo_LV_OPENCRI_MIN_A_UnCon_PHYLION * LVAnalog2Digital)//(uint16_t)(SOC_FACTOR3 * SOC_FACTOR2 / SOC_FACTOR1 + 1)//
#define	MnNiCo_LV_OPENCRI_MAX_D_UnCon_PHYLION			(uint16_t)(MnNiCo_LV_OPENCRI_MAX_A_UnCon_PHYLION * LVAnalog2Digital)
#define	MnNiCo_LV_K_Turning_Point1_D_UnCon_PHYLION		(uint16_t)(MnNiCo_LV_K_Turning_Point1_A_UnCon_PHYLION * LVAnalog2Digital)
#define	MnNiCo_LV_K_Turning_Point2_D_UnCon_PHYLION		(uint16_t)(MnNiCo_LV_K_Turning_Point2_A_UnCon_PHYLION * LVAnalog2Digital)	
#define	MnNiCo_LV_K_Turning_Point3_D_UnCon_PHYLION		(uint16_t)(MnNiCo_LV_K_Turning_Point3_A_UnCon_PHYLION * LVAnalog2Digital)
#define	MnNiCo_LV_K_Turning_Point4_D_UnCon_PHYLION		(uint16_t)(MnNiCo_LV_K_Turning_Point4_A_UnCon_PHYLION * LVAnalog2Digital)	
#endif

#if defined(CC26XX)	
//#define test_PHYLION	(uint32_t)(MnNiCo_LV_OPENCRI_MAX_A_Con_PHYLION * 1000*R9/(R7+R9))
#define	MnNiCo_LV_OPENCRI_MIN_D_Con_PHYLION				(uint16_t)(MnNiCo_LV_OPENCRI_MIN_A_Con_PHYLION * R9*1000/ADC_RSum)//(uint16_t)(SOC_FACTOR3 * SOC_FACTOR2 / SOC_FACTOR1 + 1)//
#define	MnNiCo_LV_OPENCRI_MAX_D_Con_PHYLION				(uint16_t)(MnNiCo_LV_OPENCRI_MAX_A_Con_PHYLION * R9*1000/ADC_RSum)
#define	MnNiCo_LV_K_Turning_Point1_D_Con_PHYLION		(uint16_t)(MnNiCo_LV_K_Turning_Point1_A_Con_PHYLION * R9*1000/ADC_RSum)
#define	MnNiCo_LV_K_Turning_Point2_D_Con_PHYLION		(uint16_t)(MnNiCo_LV_K_Turning_Point2_A_Con_PHYLION * R9*1000/ADC_RSum)	
#define	MnNiCo_LV_K_Turning_Point3_D_Con_PHYLION		(uint16_t)(MnNiCo_LV_K_Turning_Point3_A_Con_PHYLION * R9*1000/ADC_RSum)
#define	MnNiCo_LV_K_Turning_Point4_D_Con_PHYLION		(uint16_t)(MnNiCo_LV_K_Turning_Point4_A_Con_PHYLION * R9*1000/ADC_RSum)
	
#define	MnNiCo_LV_OPENCRI_MIN_D_UnCon_PHYLION			(uint16_t)(MnNiCo_LV_OPENCRI_MIN_A_UnCon_PHYLION * R9*1000/ADC_RSum)//(uint16_t)(SOC_FACTOR3 * SOC_FACTOR2 / SOC_FACTOR1 + 1)//
#define	MnNiCo_LV_OPENCRI_MAX_D_UnCon_PHYLION			(uint16_t)(MnNiCo_LV_OPENCRI_MAX_A_UnCon_PHYLION * R9*1000/ADC_RSum)
#define	MnNiCo_LV_K_Turning_Point1_D_UnCon_PHYLION		(uint16_t)(MnNiCo_LV_K_Turning_Point1_A_UnCon_PHYLION * R9*1000/ADC_RSum)
#define	MnNiCo_LV_K_Turning_Point2_D_UnCon_PHYLION		(uint16_t)(MnNiCo_LV_K_Turning_Point2_A_UnCon_PHYLION * R9*1000/ADC_RSum)	
#define	MnNiCo_LV_K_Turning_Point3_D_UnCon_PHYLION		(uint16_t)(MnNiCo_LV_K_Turning_Point3_A_UnCon_PHYLION * R9*1000/ADC_RSum)
#define	MnNiCo_LV_K_Turning_Point4_D_UnCon_PHYLION		(uint16_t)(MnNiCo_LV_K_Turning_Point4_A_UnCon_PHYLION * R9*1000/ADC_RSum)	
#endif
	





/*********************TianNeng***********************/	
/*单节高压	4.2V 		单节低压	2.8V
*	6个点 
	3.459V		0%;	
	3.472V		5%;	
	3.576V		25%;	
	3.661V		50%;	
	4.030V		90%
	4.173V		100%
******************************************************/
#define MnNiCo_LV_OPENCRI_MIN_Single_Cell_A_TIANNENG       		(3.459)
#define MnNiCo_LV_OPENCRI_MAX_Single_Cell_A_TIANNENG			(4.173)
#define	MnNiCo_LV_K_Turning_Point1_Single_Cell_A_TIANNENG		(3.472)		
#define	MnNiCo_LV_K_Turning_Point2_Single_Cell_A_TIANNENG		(3.576)
#define	MnNiCo_LV_K_Turning_Point3_Single_Cell_A_TIANNENG		(3.661)
#define	MnNiCo_LV_K_Turning_Point4_Single_Cell_A_TIANNENG		(4.030)

/*conventional battery num*/
#define MnNiCo_LV_OPENCRI_MIN_A_Con_TIANNENG			(MnNiCo_LV_OPENCRI_MIN_Single_Cell_A_TIANNENG * MnNiCo_Battery_Con_Section_Num)
#define MnNiCo_LV_OPENCRI_MAX_A_Con_TIANNENG			(MnNiCo_LV_OPENCRI_MAX_Single_Cell_A_TIANNENG * MnNiCo_Battery_Con_Section_Num) 
#define	MnNiCo_LV_K_Turning_Point1_A_Con_TIANNENG		(MnNiCo_LV_K_Turning_Point1_Single_Cell_A_TIANNENG * MnNiCo_Battery_Con_Section_Num)// 56.512 (55.64063)
#define	MnNiCo_LV_K_Turning_Point2_A_Con_TIANNENG		(MnNiCo_LV_K_Turning_Point2_Single_Cell_A_TIANNENG * MnNiCo_Battery_Con_Section_Num)// 59.296 (58.375)
#define	MnNiCo_LV_K_Turning_Point3_A_Con_TIANNENG		(MnNiCo_LV_K_Turning_Point3_Single_Cell_A_TIANNENG * MnNiCo_Battery_Con_Section_Num)// 62.592 (64.78125)
#define	MnNiCo_LV_K_Turning_Point4_A_Con_TIANNENG		(MnNiCo_LV_K_Turning_Point4_Single_Cell_A_TIANNENG * MnNiCo_Battery_Con_Section_Num)// 64.592

/*unconventional battery num*/
#define MnNiCo_LV_OPENCRI_MIN_A_UnCon_TIANNENG			(MnNiCo_LV_OPENCRI_MIN_Single_Cell_A_TIANNENG * MnNiCo_Battery_UnCon_Section_Num)
#define MnNiCo_LV_OPENCRI_MAX_A_UnCon_TIANNENG			(MnNiCo_LV_OPENCRI_MAX_Single_Cell_A_TIANNENG * MnNiCo_Battery_UnCon_Section_Num) 
#define	MnNiCo_LV_K_Turning_Point1_A_UnCon_TIANNENG		(MnNiCo_LV_K_Turning_Point1_Single_Cell_A_TIANNENG * MnNiCo_Battery_UnCon_Section_Num)// 56.512 (55.64063)
#define	MnNiCo_LV_K_Turning_Point2_A_UnCon_TIANNENG		(MnNiCo_LV_K_Turning_Point2_Single_Cell_A_TIANNENG * MnNiCo_Battery_UnCon_Section_Num)// 59.296 (58.375)
#define	MnNiCo_LV_K_Turning_Point3_A_UnCon_TIANNENG		(MnNiCo_LV_K_Turning_Point3_Single_Cell_A_TIANNENG * MnNiCo_Battery_UnCon_Section_Num)// 62.592 (64.78125)
#define	MnNiCo_LV_K_Turning_Point4_A_UnCon_TIANNENG		(MnNiCo_LV_K_Turning_Point4_Single_Cell_A_TIANNENG * MnNiCo_Battery_UnCon_Section_Num)// 64.592

#if defined(STM32)
#define	MnNiCo_LV_OPENCRI_MIN_D_Con_TIANNENG			(uint16_t)(MnNiCo_LV_OPENCRI_MIN_A_Con_TIANNENG * LVAnalog2Digital)//(uint16_t)(SOC_FACTOR3 * SOC_FACTOR2 / SOC_FACTOR1 + 1)//
#define	MnNiCo_LV_OPENCRI_MAX_D_Con_TIANNENG			(uint16_t)(MnNiCo_LV_OPENCRI_MAX_A_Con_TIANNENG * LVAnalog2Digital)
#define	MnNiCo_LV_K_Turning_Point1_D_Con_TIANNENG		(uint16_t)(MnNiCo_LV_K_Turning_Point1_A_Con_TIANNENG * LVAnalog2Digital)
#define	MnNiCo_LV_K_Turning_Point2_D_Con_TIANNENG		(uint16_t)(MnNiCo_LV_K_Turning_Point2_A_Con_TIANNENG * LVAnalog2Digital)	
#define	MnNiCo_LV_K_Turning_Point3_D_Con_TIANNENG		(uint16_t)(MnNiCo_LV_K_Turning_Point3_A_Con_TIANNENG * LVAnalog2Digital)
#define	MnNiCo_LV_K_Turning_Point4_D_Con_TIANNENG		(uint16_t)(MnNiCo_LV_K_Turning_Point4_A_Con_TIANNENG * LVAnalog2Digital)

#define	MnNiCo_LV_OPENCRI_MIN_D_UnCon_TIANNENG			(uint16_t)(MnNiCo_LV_OPENCRI_MIN_A_UnCon_TIANNENG * LVAnalog2Digital)//(uint16_t)(SOC_FACTOR3 * SOC_FACTOR2 / SOC_FACTOR1 + 1)//
#define	MnNiCo_LV_OPENCRI_MAX_D_UnCon_TIANNENG			(uint16_t)(MnNiCo_LV_OPENCRI_MAX_A_UnCon_TIANNENG * LVAnalog2Digital)
#define	MnNiCo_LV_K_Turning_Point1_D_UnCon_TIANNENG		(uint16_t)(MnNiCo_LV_K_Turning_Point1_A_UnCon_TIANNENG * LVAnalog2Digital)
#define	MnNiCo_LV_K_Turning_Point2_D_UnCon_TIANNENG		(uint16_t)(MnNiCo_LV_K_Turning_Point2_A_UnCon_TIANNENG * LVAnalog2Digital)	
#define	MnNiCo_LV_K_Turning_Point3_D_UnCon_TIANNENG		(uint16_t)(MnNiCo_LV_K_Turning_Point3_A_UnCon_TIANNENG * LVAnalog2Digital)
#define	MnNiCo_LV_K_Turning_Point4_D_UnCon_TIANNENG		(uint16_t)(MnNiCo_LV_K_Turning_Point4_A_UnCon_TIANNENG * LVAnalog2Digital)
#endif

#if defined(CC26XX)
#define	MnNiCo_LV_OPENCRI_MIN_D_Con_TIANNENG			(uint16_t)(MnNiCo_LV_OPENCRI_MIN_A_Con_TIANNENG * R9*1000/ADC_RSum)//(uint16_t)(SOC_FACTOR3 * SOC_FACTOR2 / SOC_FACTOR1 + 1)//
#define	MnNiCo_LV_OPENCRI_MAX_D_Con_TIANNENG			(uint16_t)(MnNiCo_LV_OPENCRI_MAX_A_Con_TIANNENG * R9*1000/ADC_RSum)
#define	MnNiCo_LV_K_Turning_Point1_D_Con_TIANNENG		(uint16_t)(MnNiCo_LV_K_Turning_Point1_A_Con_TIANNENG * R9*1000/ADC_RSum)
#define	MnNiCo_LV_K_Turning_Point2_D_Con_TIANNENG		(uint16_t)(MnNiCo_LV_K_Turning_Point2_A_Con_TIANNENG * R9*1000/ADC_RSum)	
#define	MnNiCo_LV_K_Turning_Point3_D_Con_TIANNENG		(uint16_t)(MnNiCo_LV_K_Turning_Point3_A_Con_TIANNENG * R9*1000/ADC_RSum)
#define	MnNiCo_LV_K_Turning_Point4_D_Con_TIANNENG		(uint16_t)(MnNiCo_LV_K_Turning_Point4_A_Con_TIANNENG * R9*1000/ADC_RSum)

#define	MnNiCo_LV_OPENCRI_MIN_D_UnCon_TIANNENG			(uint16_t)(MnNiCo_LV_OPENCRI_MIN_A_UnCon_TIANNENG * R9*1000/ADC_RSum)//(uint16_t)(SOC_FACTOR3 * SOC_FACTOR2 / SOC_FACTOR1 + 1)//
#define	MnNiCo_LV_OPENCRI_MAX_D_UnCon_TIANNENG			(uint16_t)(MnNiCo_LV_OPENCRI_MAX_A_UnCon_TIANNENG * R9*1000/ADC_RSum)
#define	MnNiCo_LV_K_Turning_Point1_D_UnCon_TIANNENG		(uint16_t)(MnNiCo_LV_K_Turning_Point1_A_UnCon_TIANNENG * R9*1000/ADC_RSum)
#define	MnNiCo_LV_K_Turning_Point2_D_UnCon_TIANNENG		(uint16_t)(MnNiCo_LV_K_Turning_Point2_A_UnCon_TIANNENG * R9*1000/ADC_RSum)	
#define	MnNiCo_LV_K_Turning_Point3_D_UnCon_TIANNENG		(uint16_t)(MnNiCo_LV_K_Turning_Point3_A_UnCon_TIANNENG * R9*1000/ADC_RSum)
#define	MnNiCo_LV_K_Turning_Point4_D_UnCon_TIANNENG		(uint16_t)(MnNiCo_LV_K_Turning_Point4_A_UnCon_TIANNENG * R9*1000/ADC_RSum)
#endif




#define Delta_Voltage_Open_Stable_Max_D		(uint16_t)(LVAnalog2Digital / 2)    //*48V时约等于0.5V*LVAnalog2Digital =23.5 24*/




/***********************counter**********************/
#define Counter_LV_Sample_Max			256
#define Counter_LV_Sample_Shift			Counter_1Byte_Shift//8	//移位


/******************Q*********************************/
#define LV_Accuracy_Reduced_ForQ		2
#define Current_Sigma_A					(AH_Battery_InFlash * TimePeriod_1h_Per1s / 0.678 )	//计算出基于一线通 收到多少个电流值
#define Current_Sigma_D					(uint32_t)(Current_Sigma_A * 4) //一线通中电流信息为除以4为真实值

/*#if(Lead_Acid_Batteries == Battery_Materials)
	#define AverageVoltage_CalByEnergy_A					(LV_Grade * 100)
#elif(MnNiCo_Ternary_Battery == Battery_Materials)
	#define	AverageVoltage_CalByEnergy_A					5724//57.24
#else
#endif

#define	AverageVoltage_CalByEnergy_D		(uint16_t)(AverageVoltage_CalByEnergy_A * LVAnalog2Digital/100)	
#define Q_Max								(uint32_t)(Current_Sigma_D *(AverageVoltage_CalByEnergy_D >> LV_Accuracy_Reduced_ForQ))
#define Q_current_max_Flash_Dleta_MAX		((uint32_t)(Q_Max / 100)) */

#define		Current_Sigma_D_PerAH				21239	//(TimePeriod_1h_Per1s / 0.678 * 4)
#define 	Q_Max_InFlash								(uint32_t)(Current_Sigma_D)
#define 	Q_current_max_Flash_Dleta_MAX		((uint32_t)(Q_Max_InFlash / 100))

#define		PerFrame_LowPass_Filter_Factor				1
#define		PerFrame_LowPass_Filter_Cardinal			256


#define     Soc_Charge_Criteria         10//在平缓段 10%大约相差1.875V 第二段10% 相差3.75V		//5


/*************************************************/
/*******************Variable**********************/
extern LV_TypeDef			LV ;
extern Q_TypeDef			Q ;
extern Current_TypeDef		Current ;



/****************************************************/
/**********************function**********************/
//void GetQmaxAndMmaxCalElement(void);   
extern void ADCInitial(void) ;
extern uint32_t GetLVRawValue(uint_fast16_t ADCindex) ;
extern void LVADSample(void) ;
extern void CalADCSampleAverageValue(ADCSample_TypeDef * ADCStruct) ;
extern bool IsFlagLVInitSet(void) ;
extern uint16_t GetLVValueFlashSaveDetect(void) ;
uint16_t GetBatterySoc(void);
uint16_t CalSocMul16(uint16_t );

#if(Commu_Mode_Common != Commu_Mode)
	//void InitialQ(void);
	uint16_t ReturnRatioReleaseDivMileageMul256InitialValue(void) ;
	uint32_t CalQmax(void) ;
	void ReduceQRemainderBasedCurrent(void);
	//void QInitalAndQuitCalQMax(void);
	void UpdataQInitalAndRemainder(void);
	#if(COMPILE_ENABLE == Double_LV)
		bool IsLVLowerThanLV1Level(void) ;
		bool IsLVHigherThanLV1RecoverLevel(void) ;
	#endif
#endif


StatusMotor_TypeDef GetMotorStatus(StatusMotor_TypeDef);
bool IsChargeStatus(void) ;
//bool IsOpenVoltageInChargingStatus(void)  ;
//bool SetFlagBatteryCharged(void) ;

//void StartOpenCircuitVoltageTimer(void) ;
void CalQInitialAndMileageRemainder(void) ;
//uint32_t CalMileageRemainderWithoutUpdataQInitial(void) ;

bool IsBatteryNumConventional(void) ;
uint8_t ReturnCurrentQPercent(void);
void UpdateQPercentAndSend(void) ;




#endif
