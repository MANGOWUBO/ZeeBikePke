#ifndef __InitialConfig_H
#define __InitialConfig_H

/****************************************************/
/*********************typedef************************/




/****************************************************/
/**********************define************************/
/*********************Base_Address*******************/
//�μ�stm32f10x.h L1327 ���������Ϊ0X40013800�� DR��ƫ�Ƶ�ַ��0X04(�μ�stm32f10x.h L1238) 
//��˼�������ս�� Ϊ0X40013804
//#define USART1_DR_Base	0x40013804			

//�μ�stm32f10x.h L1300 ���������Ϊ0X40014400�� DR��ƫ�Ƶ�ַ��0X04(�μ�stm32f10x.h L1238) 
//��˼�������ս�� Ϊ0X40014404
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
