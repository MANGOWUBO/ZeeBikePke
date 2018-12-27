#include "defines.h"
#include "VoicePacket.h"








//��Ƶ����
//����1:63624 2:63864 3:64024
//���¶�����������
#define   AudioModeConst       0//������ʽ����
#define   AudioHighTimeConst   1//�����ߵ�ƽʱ�䳣��
#define   AudioCeilingConst    2//�������޵͵�ƽʱ�䳣��
#define   AudioLowTimeConst    3//���޵͵�ƽʱ�䳣��
#define   AudioStepConst       4//������������
#define   AudioNumberConst     5//�����������
#define   AudioTotalTimeConst  6//����ʱ�䳣��
/*const    uint16_t  BaoJingYingDuan1[]={0��������ʽ           1,�����ߵ�ƽʱ��     
                   2���������޵͵�ƽʱ�� 3,���޵͵�ƽʱ��
                   4����������           5,�������
                   6������ʱ��           7,��ʼ�����仯����};*/
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

//���¶����������õ��ı���
//static volatile  uint8_t  AudioStep;//��������
static volatile  uint16_t  AudioControlArray[7];//AudioControlArray[6]Ҳ�ɴ�����ͣʱ�䣬��ֹͣʱ��
static volatile  uint8_t   AudioNumber;//�������
static   uint8_t   AudioSendSome;//�����������Ĵ���
static   uint16_t  AudioChangeStep;//�����仯����

static volatile  uint8_t Audio_HBit;//��Ƶ����ߵ�ƽ
static volatile  uint8_t Audio_StauesBit;//����״̬ ==1ʱ�ݼӣ�==0ʱ�ݼ�
static volatile  uint8_t Audio_PauseBit;//��Ƶ��ͣ��־λ ==1��ͣ
static volatile  uint8_t Audio_StopBit;//��Ƶֹͣ��־λ ==1ֹͣ 
static volatile  uint8_t Audio_SendSomeBit;//��������־λ

static volatile  uint8_t  Audio_PuTongBit;//��������־λ
static volatile  uint16_t Audio_PauseNumber;//��Ƶ��ͣʱ�� 
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
  ��������: DelayUs
  ��������: �ӳٶ���US
  �������: ��
  ����ֵ:   0:��ʾ��ʼ��ʧ�ܣ�1:��ʾ��ʼ���ɹ�
*/
void  DelayUs(uint32_t  UsTime)
{
	while(UsTime>0) {UsTime--;}	//��ʱUstime΢��
}

GPTimerCC26XX_Handle HandleTimer;
void timerCallback(GPTimerCC26XX_Handle handle, GPTimerCC26XX_IntMask interruptMask)
{
  //   interrupt callback code goes here. Minimize processing in interrupt.
	//PIN_setOutputValue(hGpioPin ,Pin_EleSeatLock ,0) ;
	if(Audio_PuTongBit)//��������־λ
	{
	  if(!Audio_PauseBit)//��ͣ��־λ 1:�ݶ�
      {
        if(Audio_HBit)  //��Ƶ����ߵ�ƽ
        	{
				PIN_setOutputValue(hGpioPin ,Pin_MLK1 ,0);DelayUs(100);
				PIN_setOutputValue(hGpioPin ,Pin_MLK2 ,1) ;Audio_HBit=0;Timer_isr_temp=AudioControlArray[AudioHighTimeConst]*6+1;}
        else 
        	{
				PIN_setOutputValue(hGpioPin ,Pin_MLK2 ,0);DelayUs(100);
				PIN_setOutputValue(hGpioPin ,Pin_MLK1 ,1);
				Audio_HBit=1;Timer_isr_temp=AudioChangeStep*6+1;}
		GPTimerLoadValue(Timer_isr_temp*48);
      	if(AudioNumber>0) AudioNumber--;  //�������
        else
         {
         	 AudioNumber=AudioControlArray[AudioNumberConst];
         	 if(Audio_StauesBit)//����״̬ ==1ʱ�ݼӣ�==0ʱ�ݼ�
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
          	if(AudioNumber>0) AudioNumber--;  //�������
          	else
          	{
          			  AudioNumber=AudioControlArray[AudioNumberConst];
          			  switch (AudioControlArray[AudioModeConst])//������ʽ����
              			{
              				case(1)://������ʽһ
              					{
              						if(Audio_StauesBit)//����״̬ ==1ʱ�ݼӣ�==0ʱ�ݼ�
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
              		          	
              		   if(Audio_PauseNumber>AudioSendSome){Audio_StopBit=1;AudioControlArray[AudioTotalTimeConst]=6;}//Audio_PauseNumber(��ͣ����)����3ʱ���趨ֹͣʱ��
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
  * @��������	  void  OpenAudio(void)
  * @����˵��   ������Ƶ
  * @�������   ��
  * @�������   ��
  * @���ز���   ��
*******************************************************************************/
void  OpenAudio(void)
{
//	Audio_StartBit=1;//��ʼ��־
	Audio_PauseBit=0;//��ͣ��־
	Audio_StopBit=0;//ֹͣ��־
	Audio_PauseNumber=0;//��Ƶ��ͣʱ�� 
//	AudioSendSome=3;//�����������Ĵ���
	Audio_SendSomeBit=0;
	GPTimerLoadValue(0x10*48);
    GPTimerCC26XX_start(HandleTimer);
	PIN_setOutputValue(hGpioPin ,Pin_MLK2 ,0) ;
	PIN_setOutputValue(hGpioPin ,Pin_MLK1 ,0) ;
	PIN_setOutputValue(hGpioPin ,Pin_SW9V ,1) ;                          //ע��PCBȷ���� �ر����IO�ڣ���ʱ������
}


/*******************************************************************************
  * @��������	  void  CloseAudio(void)
  * @����˵��   �ر���Ƶ
  * @�������   ��
  * @�������   ��
  * @���ز���   ��
*******************************************************************************/
void  CloseAudio(void)
{
	GPTimerCC26XX_stop(HandleTimer);
	PIN_setOutputValue(hGpioPin ,Pin_SW9V ,0) ;      
}


/*******************************************************************************
  * @��������	  void SendSomeAudio(uint8_t SeveralAudio)
  * @����˵��   ���Ʒ���������������������
  * @�������   SeveralAudio �����Ĵ��� 
                LogicalVariablesBit   ==0 ����������
                                      ==1 ������ 
  * @�������   ��
  * @���ز���   ��
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
  * @��������	  void SendTiShiAudio(uint8_t SeveralAudio)
  * @����˵��   ���Ʒ���ʾ��
  * @�������   SeveralAudio ����������
  * @�������   ��
  * @���ز���   ��
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
  * @��������	  uint8_t  AudioCopy(uint8_t Segment)
  * @����˵��   ��������Ƶ�����ַ�
  * @�������   Segment ѡ��1-6����Ƶ,����10ʱ��ֱ�ӷ���0x00; 
  * @�������   ��
  * @���ز���   0x00��ʾ�ɹ� 0x01��ʾʧ��
  const    uint16_t  BaoJingYingDuan1[]={1��������ʽ           2,�����ߵ�ƽʱ��     
                                         3���������޵͵�ƽʱ�� 4,���޵͵�ƽʱ��
                                         5����������           6,�������
                                         7������ʱ��};
*******************************************************************************/
uint8_t  AudioCopy(uint8_t Segment)
{
	uint16_t * pYinDuan;
	Segment++;//��һѡ������
	if(Segment>10) return 0x01;
	Audio_StauesBit=0; //����״̬ ==1ʱ�ݼӣ�==0ʱ�ݼ�
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
	AudioChangeStep=pYinDuan[7];    //��ʼ�����仯����
	Audio_HBit=1;      //��Ƶ����ߵ�ƽ
	Audio_PuTongBit=0; //��������־λ
	//AudioStep=0;
	AudioNumber=pYinDuan[5]; //�������
	return 0x00;
}


#endif
