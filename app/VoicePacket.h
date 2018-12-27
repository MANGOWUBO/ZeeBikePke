#ifndef __VOICEPACKET_H
#define __VOICEPACKET_H

#include "systemcontrol.h"


#define		VOICE_UNLOCK			           0x0101
#define		VOICE_LOCK				           0x0202
#define     VOICE_BLECONNECT                   0X0303
#define     VOICE_SearchCar                    0X0404
#define     VOICE_CarInRepair                  0X0505
#define     VOICE_CarBeenrented                0X0606
#define		VOICE_UNLOCKAgain	               0x0707
#define		VOICE_ReturnCar	                   0x0808
#define		VOICE_ReturnCarNotInArea	       0x0909

















extern SIFOUT_TypeDef VoicePacketSIF ;
extern void SetVoicePacketCmd(uint16_t cmd_16) ;

#if VOICEPACKETPWM_COMPILE
extern void GPTimerIntit(void);
extern void SendSomeAudio(uint8_t SeveralAudio,uint8_t LogicalVariablesBit);
extern void SendTiShiAudio(uint8_t SeveralAudio);
#endif

#endif
