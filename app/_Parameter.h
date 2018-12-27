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
#define Commu_Mode_Common               0x01           //0x00��ͨ�ÿ������汾  0x01��һ��ͨͨѶ�汾    0x02��˫����ư汾 
#define Commu_Mode_OneCommu             0x02
#define Commu_Mode_TwoWire              0x03


/******************** Battery_Materials *****************************/
//high 3bit means Materials, low 5bit means factory
#define	Lead_Acid_Batteries					0X00		//Ǧ����
#define	Lead_Acid_Batteries_TIANNENG		0X02		//Ǧ���� 	 ����
#define	MnNiCo_Ternary_Battery_PHYLION_OLD	0X21		//��Ԫ﮵�� �Ǻ�
#define	MnNiCo_Ternary_Battery_PHYLION_NEW	0X23		//��Ԫ﮵�� �Ǻ��µ�о
#define	MnNiCo_Ternary_Battery_TIANNENG		0X22		//��Ԫ﮵�� ����

#define	Lead_Acid_Batteries_MaX				0X1F

/******************** Customer type *****************************/
#define Manufactor_Ebike		0X01	//�綯������
#define	Rent_Ebike				0X02	//�⳵�̻���

/******************** Anti-theft Device Function *****************************/
#define External_Anti_theft		0X01	//���÷����� ������е�������������ơ������綯˫�Žš�������Կ������
#define	KeyLessStart			0X02	//����Կ������
//#define	MechancialLockControl		0X03	//����һ���Ļ��������ӻ�е��������������
//#define	ElectricDouSupport		0X04	//����һ���Ļ��������ӵ綯˫�Ž�

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
#define CyclicSendMessagePer30s			0X01	//30s ����һ��ѭ��������Ϣ
#define CyclicSendMessagePer0_678s		0X02	//�Ϳ�������������ʱ ÿ0.678s����һ�� �Ͽ�ʱÿ1s����1�Ρ�	


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