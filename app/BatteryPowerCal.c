#include "defines.h"
#include <ti/drivers/ADC.h>
#include <ti/drivers/adc/ADCCC26XX.h>



#include "BatteryPowerCal.h"
#include "systemcontrol.h"
#include "Mileagecal.h"
#include "gpscontrol.h"
#include "Guard.h"
#include "debug.h"

#if(Battery_Materials_InFlash > 0X1F)
	#if(13 == MnNiCo_Battery_Section_Num)
		#if( MnNiCo_Ternary_Battery_PHYLION_OLD == Battery_Materials_InFlash)
		const uint16_t LVVoltageTable[100] =			
		{
			1446,1512,1578,1644,1710,1726,1742,1758,1773,1789,
			1793,1797,1801,1805,1809,1813,1817,1821,1825,1828,
			1835,1842,1849,1856,1863,1867,1871,1875,1879,1883,
			1886,1889,1892,1894,1897,1900,1903,1906,1909,1912,
			1914,1916,1918,1920,1922,1924,1926,1928,1930,1932,
			1934,1936,1938,1940,1942,1944,1946,1948,1950,1952,
			1955,1958,1961,1963,1966,1968,1970,1972,1974,1976,
			1978,1980,1982,1984,1986,1988,1990,1992,1994,1996,
			1998,2000,2002,2004,2006,2007,2008,2009,2010,2011,
			2014,2017,2020,2023,2026,2034,2043,2052,2061,2070
		};

		#elif( MnNiCo_Ternary_Battery_PHYLION_NEW == Battery_Materials_InFlash)
		const uint16_t LVVoltageTable[100] =			
		{
			1286,1400,1510,1568,1607,1637,1662,1682,1698,1707,
			1713,1716,1720,1723,1726,1730,1734,1739,1743,1747,
			1751,1755,1759,1763,1767,1771,1775,1780,1784,1790,
			1795,1801,1808,1814,1821,1827,1834,1841,1847,1853,
			1859,1864,1868,1872,1876,1879,1883,1885,1888,1891,
			1893,1896,1898,1900,1903,1905,1908,1910,1912,1915,
			1917,1920,1923,1927,1931,1935,1940,1944,1948,1952,
			1956,1960,1963,1967,1970,1973,1976,1979,1982,1984,
			1986,1988,1990,1991,1993,1995,1997,1999,2001,2003,
			2005,2008,2011,2015,2019,2023,2029,2035,2043,2052
		};
		#elif( MnNiCo_Ternary_Battery_TIANNENG == Battery_Materials_InFlash)
		const uint16_t LVVoltageTable[100] =
		{	
			1706,1707,1709,1710,1711,1713,1716,1718,1721,1723,
			1727,1730,1733,1737,1740,1742,1745,1747,1750,1752,
			1755,1757,1759,1761,1763,1764,1766,1768,1769,1771,
			1772,1774,1775,1776,1778,1778,1780,1781,1782,1784,
			1786,1787,1789,1791,1793,1796,1799,1801,1803,1805,
			1808,1811,1814,1817,1820,1824,1828,1832,1836,1840,
			1844,1849,1853,1858,1863,1866,1871,1876,1880,1885,
			1889,1894,1899,1903,1908,1915,1919,1924,1929,1934,
			1937,1942,1947,1953,1958,1966,1971,1976,1982,1987,
			1991,1997,2003,2009,2015,2026,2034,2042,2050,2058
		};
		#endif
	#endif	
#else
	#if(LV_Grade == 48)
		#if( Lead_Acid_Batteries_TIANNENG == Battery_Materials_InFlash)
		const uint16_t LVVoltageTable[100] =			
		{
			1769,1772,1774,1776,1779,1781,1784,1786,1789,1791,
			1793,1796,1798,1801,1803,1805,1808,1810,1813,1815,
			1818,1820,1822,1825,1827,1830,1832,1835,1837,1839,
			1842,1844,1847,1849,1852,1854,1856,1859,1861,1864,
			1866,1869,1871,1873,1876,1878,1881,1883,1886,1888,
			1890,1893,1895,1898,1900,1903,1905,1907,1910,1912,
			1915,1917,1919,1922,1924,1927,1929,1932,1934,1936,
			1939,1941,1944,1946,1949,1951,1953,1956,1958,1961,
			1963,1966,1968,1970,1973,1975,1978,1980,1983,1985,
			1987,1990,1992,1995,1997,2000,2002,2004,2007,2009
		};
		#endif
	#elif(LV_Grade == 72)	
		#if( Lead_Acid_Batteries_TIANNENG == Battery_Materials_InFlash)
		const uint16_t LVVoltageTable[100] =							//100K 0603+ 3K 0402
		{
			2039,2042,2045,2048,2050,2053,2056,2059,2062,2064,
			2067,2070,2073,2076,2078,2081,2084,2087,2090,2092,
			2095,2098,2101,2104,2106,2109,2112,2115,2118,2120,
			2123,2126,2129,2131,2134,2137,2140,2143,2145,2148,
			2151,2154,2157,2159,2162,2165,2168,2171,2173,2176,
			2179,2182,2185,2187,2190,2193,2196,2199,2201,2204,
			2207,2210,2213,2215,2218,2221,2224,2227,2229,2232,
			2235,2238,2241,2243,2246,2249,2252,2255,2257,2260,
			2263,2266,2268,2271,2274,2277,2280,2282,2285,2288,
			2291,2294,2296,2299,2302,2305,2308,2310,2313,2316                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
		};
		#endif
	#else
	#endif
#endif	
	
//#define		Counter_Sample_PowerOffDetect							16
//#define 	Counter_LV_Sample_PowerOffDete_Shift						4


/*************************************************/
/*******************Variable**********************/
//ADC
ADC_Params	adcParams ;

static int_fast16_t 	adcStatus;
static uint16_t 		adcValue;
static uint32_t 	microvalue = 0 ;
//static uint32_t 	value_adc_temp = 0 ;

//static uint64_t 	sum_temp_adc = 0 ;
//static uint8_t 		counter_sample = 0 ;

//static uint64_t 	sum_temp_adc_poweroffdete = 0 ;
//static uint8_t 		counter_sample_poweroffdete = 16 ;
//static uint16_t		LV_value_poweroffdete = 0 ;
static ADCSample_TypeDef ADCSample_Table[]=
{
	{
		.id = 0 ,	//adc for soc
		.value_single_sample = 0 ,
		.sum_temp = 0 ,
		.counter_sample = 0 ,
		.average_value = 0 ,
		.fix_counter_sample = 0 ,	//for 256 timers
		.fix_counter_shift = 8 ,
		.flag_cal = false 
	},
	{
		.id = 1 ,	//adc for flash save
		.value_single_sample = 0 ,
		.sum_temp = 0 ,
		.counter_sample = 16 ,
		.average_value = 0 ,
		.fix_counter_sample = 16 ,
		.fix_counter_shift = 4
	}
};

//uint16_t 		adcValue_48V = 0  ;
//uint16_t 		adcValue_48VSW =  0 ;
static bool 	flag_LV_init = false ; 

typedef struct
{
	uint8_t arrcounter_min ;
	uint8_t arrcounter_max ;
	const uint16_t * pTable ;
	uint16_t valueforcomp ;
	//uint8_t arrcounter_value ;
}YDichotomy_TypeDef ;

static YDichotomy_TypeDef	SOC_Vol ; 


LV_TypeDef		LV ;
Q_TypeDef		Q ;
Current_TypeDef	Current ;


/****************************************************/
static void BatteryCalInitial(void) ;

static uint8_t CalBatterySoc(uint16_t adc_sample) ;
static void InitSOCVoltagePara(uint16_t samplevalue) ;
static uint8_t	GetValueByDichotomy(YDichotomy_TypeDef  * pyDichotomy) ;





/**********************function**********************/
/*******************************************************************************
* 函数名  	 	: 需要考虑其他状态进入连接状态的处理
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
/*void GetQmaxAndMmaxCalElement(void)                //使用状态机
{
	if(Status_UnConnected_AfterOpCal == LV.status_opencircuit)
	{
		if(true == BLEMdlCnt.status_connect2controller)          // && RESET == IfPWMOutPut())
		{
			CalQInitialAndMileageRemainder();	
			LV.status_opencircuit = Status_Connected ;
		}
		else
			;	
	}
	else if(Status_Connected == LV.status_opencircuit )
	{ 
		if(false == BLEMdlCnt.status_connect2controller)	
		{
			StartOpenCircuitVoltageTimer();
			LV.status_opencircuit  = Status_UnConnected_BeforeFirstAD ;
		}
		else
			;	
	} 
	else if(Status_UnConnected_BeforeFirstAD == LV.status_opencircuit)
	{
		if(true == BLEMdlCnt.status_connect2controller)          // && RESET == IfPWMOutPut())
		{		
			LV.status_opencircuit = Status_Connected ;
		}
		else
		{
			if(LV.timer_openciruit_1s >= TimePeriod_45min_Per1s)
			{
				LV.data_45min_afteropen = LV.avr_adc;
				LV.status_opencircuit  = Status_UnConnected_BeforeSecondAD ;		
			} 
			else
				;
		}
	}
	else
	{ 
		if(true == BLEMdlCnt.status_connect2controller)          // && RESET == IfPWMOutPut())
		{
			//QInitalAndQuitCalQmax();
#if(Commu_Mode_Common == Commu_Mode)	
			Q.soc_initial = GetBatterySoc(); 
#else
			UpdataQRemainder();
#endif
			UpdataMileageRemainder();
			
			LV.status_opencircuit = Status_Connected ;
		}
		else
		{
			if(LV.timer_openciruit_1s >= TimePeriod_1h_Per1s)
			{
				LV.data_1h_afteropen = LV.avr_adc;
				if(NO == IsOpenVoltageInChargingStatus())
				{
					Q.soc_release = Q.soc_initial - Q.soc_1h_afteropen ;
#if(Commu_Mode_Common != Commu_Mode)
					Q.data_max.bit32 = CalQmax();
					UpdataQRemainder();
#endif
					Mileage.counter_hallchange_max.bit32 = CalMileageMax() ;		  
					UpdataMileageRemainder();		  		  
				}
				else
					LV.flag_batterycharged = SetFlagBatteryCharged();			//          AbandonQmax();	  
				LV.status_opencircuit = Status_UnConnected_AfterOpCal ;
			}
		}
	}   
}

*/

/*******************************************************************************
*
*/
void ADCInitial(void)
{
	Board_initADC();
	ADC_Params_init(&adcParams);
	//counter_sample_poweroffdete = Counter_Sample_PowerOffDetect ;
}

/*******************************************************************************
*return value: real volatage*1000 000 
*/
uint32_t GetLVRawValue(uint_fast16_t ADCindex)
{
	ADC_Handle	adcHandle ;
	
	adcHandle = ADC_open(ADCindex,&adcParams);
	if(NULL != adcHandle)
	{
		adcStatus = ADC_convert(adcHandle, &adcValue);
		if( ADC_STATUS_SUCCESS == adcStatus)
		{
			microvalue = ADC_convertRawToMicroVolts(adcHandle,adcValue);
		}
		ADC_close(adcHandle);
	}
	return microvalue ;
}

/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
void LVADSample(void)
{ 
	ADCSample_Table[0].value_single_sample = GetLVRawValue(Board_ADCLV);
	ADCSample_Table[1].value_single_sample = ADCSample_Table[0].value_single_sample ;
	
	CalADCSampleAverageValue(&(ADCSample_Table[0]));
	CalADCSampleAverageValue(&(ADCSample_Table[1]));
	if(false == flag_LV_init)
	{
		if(true == ADCSample_Table[0].flag_cal)
		{	
			BatteryCalInitial();
			flag_LV_init = true ;
		}
	}	
}


/********************************************************
 * 
 */
bool IsFlagLVInitSet(void)
{
	return flag_LV_init ;
}

/********************************************************
 * 
 */
void CalADCSampleAverageValue(ADCSample_TypeDef * ADCStruct)
{
	ADCStruct->sum_temp += ADCStruct->value_single_sample ;
	ADCStruct->counter_sample -- ;
	if(0 == ADCStruct->counter_sample)
	{
		ADCStruct->average_value = (uint16_t)((ADCStruct->sum_temp >> ADCStruct->fix_counter_shift)/1000) ;
		ADCStruct->sum_temp = 0 ;
		ADCStruct->counter_sample = ADCStruct->fix_counter_sample ;
		ADCStruct->flag_cal = true ;
	}
 }


/********************************************************
 * 
 */
uint16_t GetLVValueFlashSaveDetect(void)
{
	//return LV_value_poweroffdete ;
	return ADCSample_Table[1].average_value ;
}
/*******************************************************************************
* 
*/
static void BatteryCalInitial(void)
{
	Q.data_max.sum32 = Q_Max_InFlash ;
	UpdataQInitalAndRemainder();	
}
	
/*******************************************************************************
* 函数名  		: 根据电压值计算电量的百分比soc值
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
uint16_t GetBatterySoc(void)
{
	//return CalSocMul16(LV.avr_adc); 
	return CalSocMul16(ADCSample_Table[1].average_value);
}
/*******************************************************************************
* 函数名  		: 根据电压值计算电量的百分比soc值
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
uint16_t CalSocMul16(uint16_t  adc_multhusd)
{
	uint16_t  data_soc ;
	

	#if(Lead_Acid_Batteries == Battery_Materials_InFlash)
		#error "unfinished!"
	#elif(MnNiCo_Ternary_Battery_CHILWEE == Battery_Materials_InFlash)
		#error "unfinished!"
	#else
	#endif
		
	data_soc = 16 * CalBatterySoc(adc_multhusd) ;

	return data_soc ;
}

#if(Commu_Mode_Common != Commu_Mode)
/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
/*void InitialQ(void)
{
	
	Q.data_release_temp= 0 ;  
	Q.data_initial= Q.data_max.bit32 / 100 * Q.soc_initial;
	Q.data_remainder.bit32 = Q.data_initial ; 
}*/

/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
uint16_t ReturnRatioReleaseDivMileageMul256InitialValue(void)
{
	uint64_t temp_cal ;
	temp_cal = Q.data_max.sum32 ;
	temp_cal = temp_cal * 255 / Mileage.counter_hallchange_max.sum32 ;
	return (uint16_t)temp_cal ;
}


/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
uint32_t CalQmax(void)
{
	/*Q_max = Current_release_temp * TimePeriod_OneLineCommu * 100 /(soc_initial - soc_1h_afteropen  );
	data_max = Q_max/TimePeriod_OneLineCommu*/
	if(0 != Q.soc_release)
	{
		uint64_t temp_cal;
		temp_cal = Q.data_release_temp   ;
		temp_cal = temp_cal * 16 * 100 / Q.soc_release   ;
		return (uint32_t)temp_cal ;
	}
		//return (Q.data_release_temp   * 100 / Q.soc_release); 
	else
		return Q.data_max.sum32 ;
	
	//  UpdateQmaxInFlash();
}

/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
void ReduceQRemainderBasedCurrent(void)
{ 
	//  if(Q.data_remainder.bit32 > 0)
	//	Q.data_remainder.bit32 = Q.data_initial- Q.data_release_temp;
	//  else
	//	Q.data_remainder.bit32 = 0 ;
	if(Q.data_initial > Q.data_release_temp)
		Q.data_remainder.sum32 = Q.data_initial- Q.data_release_temp;
	else
		Q.data_remainder.sum32 = 0 ;
}

/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
/*void QInitalAndQuitCalQMax(void)
{
	Q.soc_initial = GetBatterySoc();     
	//InitialQ(); 
    Q.data_release_temp= 0 ;  
	Q.data_initial= Q.data_max.bit32 / 100 * Q.soc_initial;
	Q.data_remainder.bit32 = Q.data_initial ; 
}*/

/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
void UpdataQInitalAndRemainder(void)
{
	/*QInitalAndQuitCalQMax();
	Q.data_remainder.bit32 =  Q.data_initial ; */
    
    Q.soc_initial = GetBatterySoc();     
	//InitialQ(); 
    Q.data_release_temp = 0 ;  
	 
	uint64_t temp_cal;
	temp_cal = Q.data_max.sum32   ;
	temp_cal = temp_cal * Q.soc_initial / 16 / 100 ;
	Q.data_initial = (uint32_t)temp_cal ;
	//Q.data_initial = Q.data_max.bit32 / 100 * Q.soc_initial;
	Q.data_remainder.sum32 = Q.data_initial ; 
	
	Q.soc_percent = ReturnCurrentQPercent() ; 
}

#if(COMPILE_ENABLE == Double_LV)
/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
bool IsLVLowerThanLV1Level(void)
{
	if(LV.avr_adc < LV_Level1_D)
		return true ;
	else
		return false ;
}

/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
bool IsLVHigherThanLV1RecoverLevel(void)
{
	if(LV.avr_adc >= LV_Level1Recover_D)
		return true ;
	else
		return false ;
}

#endif
#endif


/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 副作用		：	当电池实际电量远大于给定的电量时，可能会一直回复在充电中
						比如，实际20AH，代码中给定为45AH，就会在park时，不断报警在充电中
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************soc_release + ************************************************************/
bool IsChargeStatus(void) 
{
    uint16_t soc_charge ;
#if(Commu_Mode_Common == Commu_Mode)
	Union4Bytes_TypeDef temp_cal ;
    UnionSumWithH8L8_TypeDef mileage_remainder;
    UnionSumWithH8L8_TypeDef mileage_max ;
	
	mileage_remainder.StructH8L8.h8 = Mileage.counter_hallchange_remainder.bit8[3] ;
	mileage_remainder.StructH8L8.l8 = Mileage.counter_hallchange_remainder.bit8[2] ;
	
	mileage_max.StructH8L8.h8 = Mileage.counter_hallchange_max.bit8[3] ;
	mileage_max.StructH8L8.l8 = Mileage.counter_hallchange_max.bit8[2] ;
	
	temp_cal.sum = mileage_remainder.sum ;
	temp_cal.sum = temp_cal.sum * ONE_HUNDRED / mileage_max.sum + Soc_Charge_Criteria;
	soc_charge = temp_cal.StructH16L16.l16 ;
#else
	soc_charge = ReturnCurrentQPercent() + Soc_Charge_Criteria;
	if(soc_charge > ONE_HUNDRED)
		soc_charge = ONE_HUNDRED ;
#endif
 
	soc_charge = soc_charge * 16 ;

	if(GetBatterySoc() > soc_charge)		
        return true ;
    else
        return false ;
}



/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
void CalQInitialAndMileageRemainder(void)     
{
#if(Commu_Mode_Common != Commu_Mode)
	UpdataQInitalAndRemainder();
#endif
	UpdateMileageInitialAndRemainder();
}	

/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
/*uint32_t CalMileageRemainderWithoutUpdataQInitial(void)   
{
	 return (Mileage.counter_hallchange_max.bit32 / 100 * GetBatterySoc()) ;
}*/


/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
bool IsBatteryNumConventional(void) 
{
#if(36 == LV_Grade)	
	#if(10 == MnNiCo_Battery_Section_Num)
		return true ;
	//else
	#else	
		return false ;
	#endif	
#elif(48 == LV_Grade)
	//if(13 == DevicePara.battery_section_num)
	#if(13 == MnNiCo_Battery_Section_Num)
		return true ;
	//else
	#else	
		return false ;
	#endif	
#elif(60 == LV_Grade)
	//if(16 == DevicePara.battery_section_num)
	#if(16 == MnNiCo_Battery_Section_Num)
		return true ;
	//else
	#else
		return false ;
	#endif		
#elif(72 == LV_Grade)
		return true ;
#endif
}

/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
uint8_t ReturnCurrentQPercent(void)
{
	uint64_t temp_cal ;
	temp_cal = Q.data_remainder.sum32 >> 8 ;
	temp_cal =  temp_cal * 100 /(Q.data_max.sum32 >>8);
	return (uint8_t)temp_cal ;
}

/*******************************************************************************
* 函数名  		: 
* 函数描述    	: 
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
void UpdateQPercentAndSend(void)
{
	static uint8_t q_percent = 0xFF  ;	//if soc init is 0
	Q.soc_percent = ReturnCurrentQPercent() ; 

	//if out of range,it should update q percent in server
	if(false == IsInAbsoluteValueRang(Q.soc_percent, q_percent,3))
	{
		q_percent = Q.soc_percent ;
		BLEStatusChangeMsg_EnQueue((uint8_t)Guard.status);
	}
}


/*******************************************************************************
*
*/
static uint8_t CalBatterySoc(uint16_t adc_sample)
{
	if(adc_sample < LVVoltageTable[0])
		return 0 ;
	if(adc_sample > LVVoltageTable[99])
		return 100 ;
	
	InitSOCVoltagePara(adc_sample) ;
	return GetValueByDichotomy(&SOC_Vol);	
}

//#if(MnNiCo_Ternary_Battery_PHYLION_NEW == Battery_Materials_InFlash)
	//#if(13 == MnNiCo_Battery_Section_Num)

/*******************************************************************************
*	X:soc 	Y:voltage_digital
*	
*/
static void InitSOCVoltagePara(uint16_t samplevalue)
{
	SOC_Vol.arrcounter_min = 0 ;
	SOC_Vol.arrcounter_max = 99 ;
	SOC_Vol.pTable = LVVoltageTable ;
	SOC_Vol.valueforcomp = samplevalue ;
}
//#endif

/*******************************************************************************
*
*/
static uint8_t	GetValueByDichotomy(YDichotomy_TypeDef  * pyDichotomy)
{
	uint8_t arrcounter_middle = 0;
	while((pyDichotomy->arrcounter_min + 1) != pyDichotomy->arrcounter_max)
	{
		arrcounter_middle = (pyDichotomy->arrcounter_min + pyDichotomy->arrcounter_max)/2 ;
		if(pyDichotomy->valueforcomp > (pyDichotomy->pTable)[arrcounter_middle] )
			pyDichotomy->arrcounter_min = arrcounter_middle ;
		else 
			pyDichotomy->arrcounter_max = arrcounter_middle ;
	}
	return pyDichotomy->arrcounter_max ;		//arrcounter_min + 1		arr counter from  0
}

	

