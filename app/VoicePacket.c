#include "defines.h"
#include "VoicePacket.h"








//音频控制
//音调1:63624 2:63864 3:64024
//以下定义声音常量
#define   AudioModeConst       0//报警方式常量
#define   AudioHighTimeConst   1//报警高电平时间常量
#define   AudioCeilingConst    2//报警上限低电平时间常量
#define   AudioLowTimeConst    3//下限低电平时间常量
#define   AudioStepConst       4//报警步长常量
#define   AudioNumberConst     5//脉冲个数常量
#define   AudioTotalTimeConst  6//报警时间常量
/*const    uint16_t  BaoJingYingDuan1[]={0，报警方式           1,报警高电平时间     
                   2，报警上限低电平时间 3,下限低电平时间
                   4，报警步长           5,脉冲个数
                   6，报警时间           7,初始报警变化步长};*/
                                    //  0     1    2    3    4    5     6     7  //
const    uint16_t  BaoJingYingDuan1[] ={0x01,0x13,0x3c,0x11,0x01,0x05,0x0480,0x3c}; 
const    uint16_t  BaoJingYingDuan2[] ={0x02,0x13,0x8c,0x50,0x05,0x10,0x00A0,0x50};
const    uint16_t  BaoJingYingDuan3[] ={0x03,0x16,0x33,0x33,0x00,0x00,0x04E2,0x33};
const    uint16_t  BaoJingYingDuan4[] ={0x04,0x16,0x58,0x58,0x00,0x00,0x0354,0x58}; 
const    uint16_t  BaoJingYingDuan5[] ={0x05,0x16,0x33,0x33,0x00,0x00,0x04E2,0x33};
const    uint16_t  BaoJingYingDuan6[] ={0x06,0x16,0x58,0x58,0x00,0x00,0x0354,0x58}; 
const    uint16_t  BaoJingYingDuan7[] ={0x07,0x16,0x33,0x33,0x00,0x00,0x04E2,0x33};
const    uint16_t  BaoJingYingDuan8[] ={0x08,0x16,0x58,0x58,0x00,0x00,0x0354,0x58};                            
const    uint16_t  BaoJingYingDuan9[] ={0x09,0x1A,0xcD,0x3C,0x01,0x1e,0x0062,0xcD};
const    uint16_t  BaoJingYingDuan10[]={0x0A,0x13,0xAE,0x86,0x14,0x01,0x0090,0xAE};  
const    uint16_t  BaoJingYingDuan11[]={63724,63964,64124};  
const    uint16_t  BaoJingYingDuan12[]={64124,63964,63724};  
const    uint16_t  BaoJingYingDuan13[]={64836,64836,64836};  

//以下定义声音所用到的变量
//static volatile  uint8_t  AudioStep;//报警步长
static volatile  uint16_t  AudioControlArray[7];//AudioControlArray[6]也可代表暂停时间，或停止时间
static volatile  uint8_t   AudioNumber;//脉冲个数
static   uint8_t   AudioSendSome;//按键发声音的次数
static   uint16_t  AudioChangeStep;//报警变化步长

static volatile  uint8_t Audio_HBit;//音频脉冲高电平
static volatile  uint8_t Audio_StauesBit;//报警状态 ==1时递加，==0时递减
static volatile  uint8_t Audio_PauseBit;//音频暂停标志位 ==1暂停
static volatile  uint8_t Audio_StopBit;//音频停止标志位 ==1停止 
static volatile  uint8_t Audio_SendSomeBit;//按键音标志位

static volatile  uint8_t  Audio_PuTongBit;//按键音标志位
static volatile  uint16_t Audio_PauseNumber;//音频暂停时间 
static volatile  uint16_t Timer_isr_temp;
static volatile  uint8_t  AudioSendSomePuTong;





/*************************************************/
/*******************Variable**********************/
static UnionSumWithH8L8_TypeDef voicepacket_cmd ;



static void LoadVoicePacketSIFByte(void) ;
SIFOUT_TypeDef VoicePacketSIF =
{
	.id = 3 ,
	.pin_id = Board_VoicePacket ,
	.mode = Data_LowLevelFirst ,
	.fixtimer_head_lowlevel_per100us = 620 ,
	.fixtimer_head_highlevel_per100us = 20 ,
	.fixtimer_data_longlevel_per100us = 60 ,
	.fixtimer_data_shortlevel_per100us = 20,
	.fixcounter_byte = (1 + 2 ),
	.fixcounter_cycle = 1 ,
	.status = SIF_IDLE ,
	.timer_lowlevel_per100us = 0 ,
	.timer_highlevel_per100us = 0 ,
	.index_byte = 0 ,
	.index_bit = 0 ,
	.index_cycle = 0 ,							//if equel 0xff ,cycle forever
	.data_send = 0 ,
	.LoadSIFOutByteFunc = LoadVoicePacketSIFByte 
};





void GPTimerLoadValue(uint32_t value );
void  OpenAudio(void);
void  CloseAudio(void);
uint8_t  AudioCopy(uint8_t Segment);
/****************************************************/
/**********************function**********************/
/*******************************************************************************
* 
*/
static void LoadVoicePacketSIFByte(void)
{	
	VoicePacketSIF.databuff[1] = voicepacket_cmd.h8 ;
	VoicePacketSIF.databuff[2] = voicepacket_cmd.l8 ;
}

/*******************************************************************************
* 
*/
void SetVoicePacketCmd(uint16_t cmd_16)
{
	voicepacket_cmd.sum = cmd_16 ;
}

#if VOICEPACKETPWM_COMPILE
/*
  函数名称: DelayUs
  函数功能: 延迟多少US
  输入参数: 无
  返回值:   0:表示初始化失败，1:表示初始化成功
*/
void  DelayUs(uint32_t  UsTime)
{
	while(UsTime>0) {UsTime--;}	//延时Ustime微秒
}

GPTimerCC26XX_Handle HandleTimer;
void timerCallback(GPTimerCC26XX_Handle handle, GPTimerCC26XX_IntMask interruptMask)
{
  //   interrupt callback code goes here. Minimize processing in interrupt.
	//PIN_setOutputValue(hGpioPin ,Pin_EleSeatLock ,0) ;
	if(Audio_PuTongBit)//按键音标志位
	{
	  if(!Audio_PauseBit)//暂停标志位 1:暂定
      {
        if(Audio_HBit)  //音频脉冲高电平
        	{
				PIN_setOutputValue(hGpioPin ,Pin_MLK1 ,0);DelayUs(100);
				PIN_setOutputValue(hGpioPin ,Pin_MLK2 ,1) ;Audio_HBit=0;Timer_isr_temp=AudioControlArray[AudioHighTimeConst]*6+1;}
        else 
        	{
				PIN_setOutputValue(hGpioPin ,Pin_MLK2 ,0);DelayUs(100);
				PIN_setOutputValue(hGpioPin ,Pin_MLK1 ,1);
				Audio_HBit=1;Timer_isr_temp=AudioChangeStep*6+1;}
		GPTimerLoadValue(Timer_isr_temp*48);
      	if(AudioNumber>0) AudioNumber--;  //脉冲个数
        else
         {
         	 AudioNumber=AudioControlArray[AudioNumberConst];
         	 if(Audio_StauesBit)//报警状态 ==1时递加，==0时递减
              {
               	AudioChangeStep +=AudioControlArray[AudioStepConst];
               	if(AudioChangeStep>AudioControlArray[AudioCeilingConst])
               		{
               			AudioChangeStep = AudioControlArray[AudioCeilingConst];
               			Audio_StauesBit=0;
               		} 
              }
             else 
              {
                AudioChangeStep -=AudioControlArray[AudioStepConst];
                if(AudioChangeStep<AudioControlArray[AudioLowTimeConst])
                	{
                		AudioChangeStep = AudioControlArray[AudioLowTimeConst];
                		Audio_StauesBit=1;
                	} 
              }
            if(AudioControlArray[AudioTotalTimeConst]>0) AudioControlArray[AudioTotalTimeConst]--;
          	else 
          	  {
          			  PIN_setOutputValue(hGpioPin ,Pin_MLK2 ,0) ;	
          			  Audio_PauseBit=1;
          			  Audio_PauseNumber=0;
              }
         }
      }
      else
      {
      	if(Audio_PauseNumber<1200)Audio_PauseNumber++;
      	else
      	{
      		Audio_PauseNumber=0;
      		AudioSendSomePuTong--;
      		if(AudioSendSomePuTong>0)
      		 { 
      			  CloseAudio();
      			  AudioCopy(0);
				  OpenAudio() ;	
      			  AudioControlArray[AudioTotalTimeConst]=0x60;
      			  Audio_PuTongBit=1;
		
      		 }
      		else
			 {
			      Audio_PuTongBit=0;PIN_setOutputValue(hGpioPin ,Pin_MLK2 ,0) ;PIN_setOutputValue(hGpioPin ,Pin_MLK1 ,0) ;Audio_PauseBit=0;
                  CloseAudio();         
  	            //  DelayUs(100000);
			 }
      	}				  
      }
	}
  else
	{
      	if(!Audio_PauseBit)
      	{
      	    if(Audio_HBit) {
				PIN_setOutputValue(hGpioPin ,Pin_MLK1 ,0);DelayUs(100);
				PIN_setOutputValue(hGpioPin ,Pin_MLK2 ,1) ;Audio_HBit=0;Timer_isr_temp=AudioControlArray[AudioHighTimeConst]*6+1;}
      		else {
				PIN_setOutputValue(hGpioPin ,Pin_MLK2 ,0);DelayUs(100);
				PIN_setOutputValue(hGpioPin ,Pin_MLK1 ,1);
				Audio_HBit=1;Timer_isr_temp=AudioChangeStep*6+1;}
			GPTimerLoadValue(Timer_isr_temp*48);
          	if(AudioNumber>0) AudioNumber--;  //脉冲个数
          	else
          	{
          			  AudioNumber=AudioControlArray[AudioNumberConst];
          			  switch (AudioControlArray[AudioModeConst])//报警方式常量
              			{
              				case(1)://报警方式一
              					{
              						if(Audio_StauesBit)//报警状态 ==1时递加，==0时递减
              							{
               								AudioChangeStep +=AudioControlArray[AudioStepConst];
               								if(AudioChangeStep>AudioControlArray[AudioCeilingConst])
               								{
               										AudioChangeStep = AudioControlArray[AudioCeilingConst];
               										Audio_StauesBit=0;
               								} 
              							}
              							else 
              							{
                							AudioChangeStep -=AudioControlArray[AudioStepConst];
                							if(AudioChangeStep<AudioControlArray[AudioLowTimeConst])
                								{
                									AudioChangeStep = AudioControlArray[AudioLowTimeConst];
                									Audio_StauesBit=1;
                								} 
              							}
              						break;
              					}
              				case(2):
              					{
           								AudioChangeStep +=AudioControlArray[AudioStepConst];
           								if(AudioChangeStep>AudioControlArray[AudioCeilingConst])
           									{
           										AudioChangeStep = AudioControlArray[AudioLowTimeConst];
           										Audio_StauesBit=0;
           									}         						
              						break;
              					}
							case(9):
					
              				case(10):
              					{        						
          								AudioChangeStep -=AudioControlArray[AudioStepConst];
          								if(AudioChangeStep<AudioControlArray[AudioLowTimeConst])
          									{
         									  	AudioChangeStep=AudioControlArray[AudioCeilingConst];
          										Audio_StauesBit=1;
          									}         						
              						break;
              					}	
							
				
              			} 
              			   				
          				  if(AudioControlArray[AudioTotalTimeConst]>0) AudioControlArray[AudioTotalTimeConst]--;
          				  else 
          					{
          						PIN_setOutputValue(hGpioPin ,Pin_MLK2 ,0) ;
								PIN_setOutputValue(hGpioPin ,Pin_MLK1 ,0) ;
          						if((AudioCopy(AudioControlArray[AudioModeConst]))||(Audio_SendSomeBit)) 
          						{
          							  Audio_PauseBit=1;
          							  if(Audio_SendSomeBit) AudioControlArray[AudioTotalTimeConst]=10;
          							  else AudioControlArray[AudioTotalTimeConst]=10;
          						}	
          					}
      		 }
  		  }
         else
          {
          	    AudioControlArray[AudioTotalTimeConst]--;
          		if(AudioControlArray[AudioTotalTimeConst] == 0)
          			{
              		   Audio_PauseNumber++;
              		   if(Audio_StopBit) 
              			{
              				Audio_PuTongBit=0;
							PIN_setOutputValue(hGpioPin ,Pin_MLK2 ,0) ;
							PIN_setOutputValue(hGpioPin ,Pin_MLK1 ,0) ;
							Audio_PauseBit=0;
              				CloseAudio();
  	                     //  DelayUs(200000);
  	                                                    
              			}    
              		          	
              		   if(Audio_PauseNumber>AudioSendSome){Audio_StopBit=1;AudioControlArray[AudioTotalTimeConst]=6;}//Audio_PauseNumber(暂停次数)大于3时，设定停止时间
              		   else 
              			{
              					AudioControlArray[AudioTotalTimeConst]=0x60;
              					Audio_PauseBit=0;
              			}
          			}
          }
    }
}


void GPTimerIntit(void)
{
  GPTimerCC26XX_Params params;
  GPTimerCC26XX_Params_init(&params);
  params.width          = GPT_CONFIG_16BIT;
  params.mode           = GPT_MODE_PERIODIC_UP;
  params.debugStallMode = GPTimerCC26XX_DEBUG_STALL_OFF;
  HandleTimer = GPTimerCC26XX_open(Board_GPTIMER0A, &params);
  if(HandleTimer == NULL) { Task_exit(); }
 
  GPTimerCC26XX_Value loadVal = 48*340/2;
  GPTimerCC26XX_setLoadValue(HandleTimer, loadVal);
  GPTimerCC26XX_registerInterrupt(HandleTimer, timerCallback, GPT_INT_TIMEOUT);
  
 
}



void GPTimerLoadValue(uint32_t value )
{
  GPTimerCC26XX_setLoadValue(HandleTimer, value);
}

/*******************************************************************************
  * @函数名称	  void  OpenAudio(void)
  * @函数说明   开启音频
  * @输入参数   无
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void  OpenAudio(void)
{
//	Audio_StartBit=1;//开始标志
	Audio_PauseBit=0;//暂停标志
	Audio_StopBit=0;//停止标志
	Audio_PauseNumber=0;//音频暂停时间 
//	AudioSendSome=3;//按键发声音的次数
	Audio_SendSomeBit=0;
	GPTimerLoadValue(0x10*48);
    GPTimerCC26XX_start(HandleTimer);
	PIN_setOutputValue(hGpioPin ,Pin_MLK2 ,0) ;
	PIN_setOutputValue(hGpioPin ,Pin_MLK1 ,0) ;
	PIN_setOutputValue(hGpioPin ,Pin_SW9V ,1) ;                          //注意PCB确定后 关闭这个IO口，临时调试用
}


/*******************************************************************************
  * @函数名称	  void  CloseAudio(void)
  * @函数说明   关闭音频
  * @输入参数   无
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void  CloseAudio(void)
{
	GPTimerCC26XX_stop(HandleTimer);
	PIN_setOutputValue(hGpioPin ,Pin_SW9V ,0) ;      
}


/*******************************************************************************
  * @函数名称	  void SendSomeAudio(uint8_t SeveralAudio)
  * @函数说明   控制发几次声音或正常报警音
  * @输入参数   SeveralAudio 发声的次数 
                LogicalVariablesBit   ==0 正常报警音
                                      ==1 按键音 
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void SendSomeAudio(uint8_t SeveralAudio,uint8_t LogicalVariablesBit)
{
	AudioCopy(0);
	OpenAudio();
	if(LogicalVariablesBit) 
	{
	  AudioControlArray[AudioTotalTimeConst]=0x60;
	 // AudioSendSomePuTong=SeveralAudio-1;  
	  AudioSendSomePuTong=SeveralAudio;
	  Audio_SendSomeBit=1;
	  Audio_PuTongBit=1;
	 // if(AudioSendSomePuTong>0){Audio_PauseNumber=12000;}
	}
}


/*******************************************************************************
  * @函数名称	  void SendTiShiAudio(uint8_t SeveralAudio)
  * @函数说明   控制发提示音
  * @输入参数   SeveralAudio 发声的声段
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void SendTiShiAudio(uint8_t SeveralAudio)
{
   uint16_t * pYinDuan;
	switch(SeveralAudio)
	{
		case 11:{pYinDuan=(uint16_t *)BaoJingYingDuan11;break;}
		case 12:{pYinDuan=(uint16_t *)BaoJingYingDuan12;break;}		
		case 13:{pYinDuan=(uint16_t *)BaoJingYingDuan13;break;}
	}
	//while(i<7){i++;AudioControlArray[i]=pYinDuan[i];}
	AudioControlArray[0]=pYinDuan[0];
	AudioControlArray[1]=pYinDuan[1];
	AudioControlArray[2]=pYinDuan[2];
	Audio_HBit=1;
	Audio_PuTongBit=1;
    AudioNumber=0;
    AudioSendSome=0;
	Audio_PauseBit=0;
	Audio_StopBit=0;
}      


                            
/*******************************************************************************
  * @函数名称	  uint8_t  AudioCopy(uint8_t Segment)
  * @函数说明   拷贝出音频控制字符
  * @输入参数   Segment 选择1-6段音频,大于10时，直接返回0x00; 
  * @输出参数   无
  * @返回参数   0x00表示成功 0x01表示失败
  const    uint16_t  BaoJingYingDuan1[]={1，报警方式           2,报警高电平时间     
                                         3，报警上限低电平时间 4,下限低电平时间
                                         5，报警步长           6,脉冲个数
                                         7，报警时间};
*******************************************************************************/
uint8_t  AudioCopy(uint8_t Segment)
{
	uint16_t * pYinDuan;
	Segment++;//加一选择音段
	if(Segment>10) return 0x01;
	Audio_StauesBit=0; //报警状态 ==1时递加，==0时递减
	switch(Segment)
	{
		case 1:{pYinDuan=(uint16_t *)BaoJingYingDuan1;break;}
		case 2:{pYinDuan=(uint16_t *)BaoJingYingDuan2;Audio_StauesBit=1;break;}
		case 3:{pYinDuan=(uint16_t *)BaoJingYingDuan3;break;}
		case 4:{pYinDuan=(uint16_t *)BaoJingYingDuan4;break;}
		case 5:{pYinDuan=(uint16_t *)BaoJingYingDuan5;break;}
		case 6:{pYinDuan=(uint16_t *)BaoJingYingDuan6;break;}
		case 7:{pYinDuan=(uint16_t *)BaoJingYingDuan7;break;}
		case 8:{pYinDuan=(uint16_t *)BaoJingYingDuan8;break;}
		case 9:{pYinDuan=(uint16_t *)BaoJingYingDuan9;break;}						
		case 10:{pYinDuan=(uint16_t *)BaoJingYingDuan10;break;}			
	}
	AudioControlArray[0]=pYinDuan[0];
	AudioControlArray[1]=pYinDuan[1];
	AudioControlArray[2]=pYinDuan[2];
	AudioControlArray[3]=pYinDuan[3];
	AudioControlArray[4]=pYinDuan[4];
	AudioControlArray[5]=pYinDuan[5];
	AudioControlArray[6]=pYinDuan[6];
	AudioChangeStep=pYinDuan[7];    //初始报警变化步长
	Audio_HBit=1;      //音频脉冲高电平
	Audio_PuTongBit=0; //按键音标志位
	//AudioStep=0;
	AudioNumber=pYinDuan[5]; //脉冲个数
	return 0x00;
}


#endif
