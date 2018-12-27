#ifndef __PARAMETER_H
#define __PARAMETER_H

/********************PCB Version*****************************/
#define	CC2650_LAUNCHXL			0x0201
#define	RGZ_B					0x0202
#define	DLCC01					0x0203
#define	DLCC04					0x0204
#define ZBJ02A					0x0205
#define ZBJ02B					0x0206
#define	ZBJ05A					0x0207
#define	ZBJ02D					0x0208




/********************BLEATCmd Version*****************************/
#define	ATCmdVersion_ChinaMainland		0X01
#define	ATCmdVersion_Vietnam			0X02

/******************** Commu_Mode *****************************/
#define Commu_Mode_Common               0x01           //0x00：通用控制器版本  0x01：一线通通讯版本    0x02：双向控制版本 
#define Commu_Mode_OneCommu             0x02
#define Commu_Mode_TwoWire              0x03


/******************** Battery_Materials *****************************/
//high 3bit means Materials, low 5bit means factory
#define	Lead_Acid_Batteries					0X00		//铅酸电池
#define	Lead_Acid_Batteries_TIANNENG		0X02		//铅酸电池 	 天能
#define	MnNiCo_Ternary_Battery_PHYLION_OLD	0X21		//三元锂电池 星恒
#define	MnNiCo_Ternary_Battery_PHYLION_NEW	0X23		//三元锂电池 星恒新电芯
#define	MnNiCo_Ternary_Battery_TIANNENG		0X22		//三元锂电池 天能

#define	Lead_Acid_Batteries_MaX				0X1F

/******************** Customer type *****************************/
#define Manufactor_Ebike		0X01	//电动车厂家
#define	Rent_Ebike				0X02	//租车商户用

/******************** Anti-theft Device Function *****************************/
#define External_Anti_theft		0X01	//外置防盗器 不带机械锁、鞍座锁控制、不带电动双撑脚、不带免钥匙启动
#define	KeyLessStart			0X02	//带免钥匙启动
//#define	MechancialLockControl		0X03	//在上一个的基础上增加机械锁、鞍座锁控制
//#define	ElectricDouSupport		0X04	//在上一个的基础上增加电动双撑脚

/******************SeatLock_Manufacturer_ID for 9V************************/
#define	TangZe_SeatLock_Manufacturer_ID					0X01
#define	LeCi_BatteryLock_Manufacturer_ID				0X02

/******************ElectroMagneticLock_Manufacturer_ID************************/
#define	YAKOT_SeatLock_Manufacturer_ID					0X01			//VDD control
#define XinChuanDa_SeatLock_Manufacturer_ID				0X02			//VDD control

/******************** MechancialLockControl Mode *****************************/
#define	ELU_BreakSingle_Mode			0X01
#define	ELU_DirectlyUnlocked_Mode		0X02

/******************MechanicalLock_Manufacturer_ID************************/
#define	TangZe_MechanicalLock_Manufacturer_ID			0X01
#define	LeCi_MechanicalLock_Manufacturer_ID				0X02
#define	Leci_FaucetLock_Manufacturer_ID					0X03

/********************DouSupport Manufacturer ID *****************************/
#define	FuzhouPeipeng_DouSupport_ManufactID		0X01
#define	DongdaLanniao_DouSupport_ManufactID		0X02

/******************** Cyclic Send TimePeriod *****************************/
#define CyclicSendMessagePer30s			0X01	//30s 传送一次循环发送信息
#define CyclicSendMessagePer0_678s		0X02	//和控制器连接正常时 每0.678s传送一次 断开时每1s传送1次。	


/********************Host Type*****************************/
#define	HostTypeA				0X01
#define	HostTypeB				0X02

/********************data_size*************/
#define BufferSize			        50
#define SENDBUFF_SIZE				50
#define RECEIVEBUFF_SIZE			50

/**********************if compile enable************/
#define	COMPILE_ENABLE			1
#define	COMPILE_DISABLE			0

/**********************FirmwareVersion************/
#define FirmwareVersion			0X02020C
	
/**********************Version******************/
#define China_CountryVersion			0X01
#define	Vietnam_CountryVersion			0X02





















#endif