#ifndef __DEFINES_H
#define __DEFINES_H

#include "stdint.h"
#include "stdio.h"
#include "stdbool.h"
#include "hal_defs.h"
#include "stdlib.h"
#include "bcomdef.h"


#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/drivers/gpio/GPIOCC26XX.h>
#include <ti/drivers/timer/GPTimerCC26XX.h>
#include "board.h"

/**********************define************************/
/*****************************************************/
/******************** BIT define *****************************/
#define U8_BIT0						0x01
#define U8_BIT1						0x02
#define U8_BIT2						0x04
#define U8_BIT3						0x08
#define U8_BIT4						0x10
#define U8_BIT5						0x20
#define U8_BIT6						0x40
#define U8_BIT7						0x80

#define	OneByteBitNum				8

#define Counter_1Byte_Shift			8	//移位

/******************** Logic define *****************************/
//#define	true						1
//#define false						0
#define YES	        				1
#define NO	        				0
#define ON							1
#define	OFF							0
#define	High						1
#define	Low							0

#define	SET_SUCCESS_U8					0X01
#define	SET_FAIL_U8						0X00
#define	Query_U8						0X00

/*typedef enum 
{
	RESET = 0, 
	SET = !RESET
} bool;*/

/***************** Nop define **********************/
#define NOP							asm( " nop " )


//#define	uint8_t 					uint8
//#define	uint16_t					uint16
//#define	uint32_t					uint32
/********************const*************/
#define	PI							3.141592654	

/******************** const num*****************************/
#define TEN							10
#define ONE_HUNDRED					100
#define ONE_THOUSAND				1000
#define TEN_THOUSAND				10000

/******************** const string ASCII*****************************/
#define		ASCII_QuestionMark				0X3F	
#define		ASCII_Num_0						0X30
#define		ASCII_Num_1						0X31
#define		ASCII_Num_2						0X32

/******************** const Time*****************************/
#define		Hours_PerDay						24
#define		Minutes_PerHour						60
#define		Seconds_PerMinute					60

/******************** const BLE*****************************/
#define	BLE_CHAR_MAX_LEN				21//

/*************DevInfoSet******************/
#define		Pole_Pairs_Tricycle_Standard		4	//三轮车标准极对数
#define		Pole_Pairs_TwoWheel_Min				23
#define		Pole_Pairs_TwoWheel_Max				50
#define		Wheel_Diameter_Inch_Int_Min			16
#define		Wheel_Diameter_Inch_Int_Max			40
#define		AH_Battery_Min						12

/*************UartBuffLen******************/
//#define		UartBuff_Len_Max					50

#define		HEXTOCHARBYTE_NUM(a,n)			((uint8_t)((a) >> n) + 0X30)
//#define		CUSTOMERTOCHARBYTE_NUM(a,n_10)		((uint8_t)(a/n_10+ 0X30))

/*************Typedef******************/
typedef union
{
    uint16_t sum;
    struct
    {
	uint8_t l8;
	uint8_t h8;	
    };
}UnionSumWithH8L8_TypeDef;

typedef union				
{
	uint32_t sum32;
	struct
	{
		uint16_t l16;	
		uint16_t h16;
	}StructH16L16;
	uint8_t bit8[4] ;
}Union4Bytes_TypeDef;

typedef enum
{
	SIF_IDLE = 0 ,
	SIF_Sendding ,
}SIFStatus_TypeDef ;
	
typedef enum
{ 
    Low_Level = 0 , 
    High_Level 
}LevelStatus_TypeDef;


typedef struct
{ 
    LevelStatus_TypeDef 	status ;
    uint8_t 		timer_newlevel_per2ms  ; 
}SwitchLevelTypeDef;

/*typedef struct
{ 
    LevelStatus_TypeDef 	status ;
	LevelStatus_TypeDef 	status_temp ;
    volatile uint16_t timer_temp  ;
    uint16_t 		timer_highlevel_OLC ;
    uint16_t 		timer_lowlevel_OLC ; 
}CommunicaLevelTypeDef;	*/

//BitBand  1byte(8bit) expand to 32bit 
//please read <Cortex‐M3 权威指南> P91
//---------------------------------------------------------------------------------------------------------------------
/*#define RAMBitBandVal(addr, bitnum, Val)	(*(volatile uint32_t*)((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF)<<5) + (bitnum<<2)) = Val)
#define SetRAMBitBand(addr, bitnum)			(*(volatile uint32_t*)((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF)<<5) + (bitnum<<2)) = 1)
#define ClrRAMBitBand(addr, bitnum)			(*(volatile uint32_t*)((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF)<<5) + (bitnum<<2)) = 0)
#define XorRAMBitBand(addr, bitnum)			(*(volatile uint32_t*)((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF)<<5) + (bitnum<<2)) ^= 1)
#define GetRAMBitBand(addr, bitnum)			(*(volatile uint32_t*)((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF)<<5) + (bitnum<<2)))


#define	GPIOx_IDR_Offset					0X08
#define	GPIOx_ODR_Offset					0X0C*/
//---------------------------------------------------------------------------------------------------------------------
//#define PortBitVal(addr, bitnum, Val)		RAMBitBandVal(addr, bitnum, Val)
//#define GetPortBit(GPIOx, bitnum)			GetSFRBitBand(GPIOx ## _IDR_ADR, bitnum)	
//#define SetPortBit(GPIOx, bitnum)			SetSFRBitBand(GPIOx ## _ODR_ADR, bitnum)
//#define ClrPortBit(GPIOx, bitnum)			ClrSFRBitBand(GPIOx ## _ODR_ADR, bitnum)
//#define XorPortBit(GPIOx, bitnum)			XorSFRBitBand(GPIOx ## _ODR_ADR, bitnum)
//
//
////output port
//#define PinSet(PinNamex,Val)				PortBitVal(Port_ ## PinNamex + GPIOx_ODR_Offset, Pin_ ## PinNamex, Val)
















#endif