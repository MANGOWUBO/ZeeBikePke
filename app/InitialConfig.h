#ifndef __InitialConfig_H
#define __InitialConfig_H

/****************************************************/
/*********************typedef************************/




/****************************************************/
/**********************define************************/
/*********************Base_Address*******************/
//参见stm32f10x.h L1327 （计算出来为0X40013800） DR的偏移地址是0X04(参见stm32f10x.h L1238) 
//因此计算出最终结果 为0X40013804
//#define USART1_DR_Base	0x40013804			

//参见stm32f10x.h L1300 （计算出来为0X40014400） DR的偏移地址是0X04(参见stm32f10x.h L1238) 
//因此计算出最终结果 为0X40014404
//#define USART2_DR_Base	(USART2_BASE+0X04)			


/****************************************************/
/**********************function**********************/
void LVInitial(void);
void ClearAllVariablesInAStruct(uint8_t * ,uint8_t ) ;
void ClearAllVariables(void) ;
#if 0
void ReadFlashInitial(void);
#endif

void VariableInitial(void);



#endif
