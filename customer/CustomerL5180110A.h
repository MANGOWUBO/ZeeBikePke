#ifndef __CUSTOMERL5180110A_H  
#define __CUSTOMERL5180110A_H

#define	TEST								COMPILE_DISABLE		
/********************Common Configure*****************************/
//Lead_Acid_Batteries & MnNiCo_Ternary_Battery_CHILWEE & MnNiCo_Ternary_Battery_PHYLION & MnNiCo_Ternary_Battery_TIANNENG
#define	Battery_Materials_InFlash					Lead_Acid_Batteries_TIANNENG

//48 & 60 & 72
#define LV_Grade				 			48

#if(MnNiCo_Ternary_Battery_CHILWEE == Battery_Materials_InFlash)
	#define			IsNormalMnNiCoBatteryNum		1  //if MnNiCo_Battery_Num is 13/16/20 ,set 1,else set 0(14/17)
#else
	#define			IsNormalMnNiCoBatteryNum		1  //if MnNiCo_Battery_Num is 13/16/20 ,set 1,else set 0(14/17)
#endif

#define	MnNiCo_Battery_Section_Num					13

#define AH_Battery_InFlash		       	 			20


#define	Wheel_Diameter_Inch_Int_InFlash	    		17
#define	Wheel_Diameter_Inch_Dec_InFlash			00
#define Gear_Ratio_Motor_InFlash        		1    //Gear_Ratio_Motor > Gear_Ratio_Wheel
#define Gear_Ratio_Wheel_InFlash        		1
#define	Number_Pole_Pairs_InFlash			30   //电机极对数
#define	BatteryStandardM_InFlash			50000
#define	Rated_Q_Release_PerFrame_A_InFlash		(8)  //等于额定AH*额定速度/额定公里数	两轮车额定速度一般是30km/h	
 
//Commu_Mode_Common & Commu_Mode_OneCommu & Commu_Mode_TwoWire
#define Commu_Mode             				Commu_Mode_TwoWire 
#define	Customer_Type					    Rent_Ebike

//External_Anti_theft & KeyLessStart 
#define Anti_theftDevice_Function		KeyLessStart

#define	MECHANICALLOCK_EXTERNAL_COMPILE		COMPILE_DISABLE

#if	MECHANICALLOCK_EXTERNAL_COMPILE
//TangZe_MechanicalLock_Manufacturer_ID &	LeCi_MechanicalLock_Manufacturer_ID
#define MechanicalLock_Manufacturer_ID 			LeCi_MechanicalLock_Manufacturer_ID	
#endif

#define DOUSUPPORT_EXTERNAL_COMPILE			COMPILE_ENABLE
#if DOUSUPPORT_EXTERNAL_COMPILE
//FuzhouPeipeng_DouSupport_ManufactID &	DongdaLanniao_DouSupport_ManufactID
#define	DouSupport_Manufacturer_ID		FuzhouPeipeng_DouSupport_ManufactID						
#endif

#define	SEATLOCK_COMPILE				COMPILE_ENABLE
#define	ELEMGLOCK_COMPILE				COMPILE_DISABLE
#if	SEATLOCK_COMPILE
	#if !ELEMGLOCK_COMPILE
		#define	  SeatLock_Manufacturer_ID      TangZe_SeatLock_Manufacturer_ID
	#endif
#endif


#define GPS_COMPILE					COMPILE_ENABLE

#define LOCKBUTTON_COMPILE				COMPILE_DISABLE
#define	VOICEPACKET_COMPILE				COMPILE_DISABLE

//BLE_E & BLE_G & BLE_J
#define PCB_VERSION					DLCC04

//China_CountryVersion & Vietnam_CountryVersion
#define COUNTRY_COMPILE					China_CountryVersion

/********************Device Information*****************************/
#define	PCBVersion_DeviceInfo				"DLCC04"
#define	FirmwareVersion_DeviceInfo			"2.2.A"	
#define	Customer_DeviceInfo				"L5180110A"	

/********************Configure InFlash*****************************/











/********************Feature  Configure*****************************/








#endif