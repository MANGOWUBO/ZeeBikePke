/******************************************************************************

 @file  simple_peripheral.c

 @brief This file contains the Simple BLE Peripheral sample application for use
        with the CC2650 Bluetooth Low Energy Protocol Stack.

 Group: CMCU, SCS
 Target Device: CC2640R2

 ******************************************************************************
 
 Copyright (c) 2013-2017, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 Release Name: simplelink_cc2640r2_sdk_1_30_00_25
 Release Date: 2017-03-02 20:08:31
 *****************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include <string.h>

#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Queue.h>


#include "hci_tl.h"
#include "gatt.h"
#include "linkdb.h"
#include "gapgattserver.h"
#include "gattservapp.h"
#include "devinfoservice.h"

//#include "simple_gatt_profile.h"
#include "zeebike1_profile.h"
#include "ll_common.h"

#if defined(FEATURE_OAD) || defined(IMAGE_INVALIDATE)
#include "oad_target.h"
#include "oad.h"
#endif //FEATURE_OAD || IMAGE_INVALIDATE

//#include "peripheral.h"
	 
#include "peripheralObserver.h"
	 
#include "gapbondmgr.h"

#include "osal_snv.h"
#include "icall_apimsg.h"

#include "util.h"

#ifdef USE_RCOSC
#include "rcosc_calibration.h"
#endif //USE_RCOSC
	 
#ifdef TI_DRIVERS_LCD_MANGO
#include "board_lcd.h"	 
#endif

#ifdef USE_CORE_SDK
  #include <ti/display/Display.h>
#else // !USE_CORE_SDK
  #include <ti/mw/display/Display.h>
#endif // USE_CORE_SDK
#include "board_key.h"

#include "board.h"
#include <ti/drivers/Watchdog.h>  
#include "gpscontrol.h"
#include "systemcontrol.h"
#include "simple_peripheral.h"
#include "ATControl.h"
#include "systemcontrol.h"	
#include "Guard.h"
#include "FlashSNV.h"
#include "debug.h"

#include "VoicePacket.h"


#if defined( USE_FPGA ) || defined( DEBUG_SW_TRACE )
#include <driverlib/ioc.h>
#endif // USE_FPGA | DEBUG_SW_TRACE

#include "icall_api.h"
#if PLUS_OBSERVER 	
#include "math.h"
#endif

#if PLUS_OBSERVER 	
// Discovey mode (limited, general, all)
#define DEFAULT_DISCOVERY_MODE                DEVDISC_MODE_ALL
// TRUE to use active scan
#define DEFAULT_DISCOVERY_ACTIVE_SCAN         FALSE
// TRUE to use white list during discovery
#define DEFAULT_DISCOVERY_WHITE_LIST          FALSE
// Maximum number of scan responses
#define DEFAULT_MAX_SCAN_RES                  15 
// Scan duration in ms
#define DEFAULT_SCAN_DURATION                 1000

#define BLE_PARK_DISTANCE                     1.0 
/*************************Rssi*****************************/	 
//Signal intensity At One Meter
#define SignalintensityAtOneMeter              59
//Environmental attenuation factor
#define EnvironmentalAttenuationFactor         2



/**************************************************************/   	 
	 

#endif

/*********************************************************************
 * CONSTANTS
 */

// Advertising interval when device is discoverable (units of 625us, 160=100ms)
#define DEFAULT_ADVERTISING_INTERVAL          100//160	50ms

// Limited discoverable mode advertises for 30.72s, and then stops
// General discoverable mode advertises indefinitely
#define DEFAULT_DISCOVERABLE_MODE             GAP_ADTYPE_FLAGS_GENERAL

#ifndef FEATURE_OAD
// Minimum connection interval (units of 1.25ms, 80=100ms) if automatic
// parameter update request is enabled
#define DEFAULT_DESIRED_MIN_CONN_INTERVAL     32//80

// Maximum connection interval (units of 1.25ms, 800=1000ms) if automatic
// parameter update request is enabled
#define DEFAULT_DESIRED_MAX_CONN_INTERVAL    40//800
#else //!FEATURE_OAD
// Minimum connection interval (units of 1.25ms, 8=10ms) if automatic
// parameter update request is enabled
#define DEFAULT_DESIRED_MIN_CONN_INTERVAL     20//16//6// 8

// Maximum connection interval (units of 1.25ms, 8=10ms) if automatic
// parameter update request is enabled
#define DEFAULT_DESIRED_MAX_CONN_INTERVAL     40//32//24//12//8
#endif // FEATURE_OAD

// Slave latency to use if automatic parameter update request is enabled
#define DEFAULT_DESIRED_SLAVE_LATENCY         0

// Supervision timeout value (units of 10ms, 1000=10s) if automatic parameter
// update request is enabled
#define DEFAULT_DESIRED_CONN_TIMEOUT        100//1000

// Whether to enable automatic parameter update request when a connection is
// formed
#define DEFAULT_ENABLE_UPDATE_REQUEST         GAPROLE_LINK_PARAM_UPDATE_INITIATE_BOTH_PARAMS

// Connection Pause Peripheral time value (in seconds)
#define DEFAULT_CONN_PAUSE_PERIPHERAL         6

// How often to perform periodic event (in msec)
#define SBP_PERIODIC_EVT_PERIOD               500//1000//5000
	







#ifdef FEATURE_OAD
// The size of an OAD packet.
#define OAD_PACKET_SIZE                       ((OAD_BLOCK_SIZE) + 2)
#endif // FEATURE_OAD

// Task configuration
#define SBP_TASK_PRIORITY                     1


#ifndef SBP_TASK_STACK_SIZE
#define SBP_TASK_STACK_SIZE                   644
#endif

	 


#define	SBP_CHAR_AsyNotify_EVT				  0x0010
#define SBP_IniativeDisconnect_EVT	 		  0x0020
#define SBP_FLASH_OPERATION_EVT				  0x0040

	 
#if PLUS_OBSERVER
#define SBP_OBSERVER_STATE_CHANGE_EVT         Event_Id_03  
#define SBC_START_DISCOVERY_EVT               0x0080
#endif
// Internal events for RTOS application
#define SBP_ICALL_EVT                         ICALL_MSG_EVENT_ID  // Event_Id_31
#define SBP_QUEUE_EVT                         UTIL_QUEUE_EVENT_ID // Event_Id_30
#define SBP_STATE_CHANGE_EVT                  Event_Id_00         // Add
#define SBP_CHAR_CHANGE_EVT                   Event_Id_01         // Add
#define SBP_PERIODIC_EVT                      Event_Id_02         // Add
#define SBP_CONN_EVT_END_EVT                  Event_Id_30         // Add


#define SBP_QUEUE_PING_EVT                    Event_Id_04         // Add

#if PLUS_OBSERVER
#define SBP_ALL_EVENTS                       (SBP_ICALL_EVT|                \
                                              SBP_QUEUE_EVT|                \
											  SBP_STATE_CHANGE_EVT|         \
											  SBP_CHAR_CHANGE_EVT|          \
											  SBP_PERIODIC_EVT|             \
											  SBP_CONN_EVT_END_EVT|         \
											  SBP_CHAR_AsyNotify_EVT|       \
											  SBP_IniativeDisconnect_EVT|   \
											  SBP_FLASH_OPERATION_EVT|      \
											  SBP_QUEUE_PING_EVT |          \
											  SBC_START_DISCOVERY_EVT |     \
											  SBP_OBSERVER_STATE_CHANGE_EVT  )
#else
#define SBP_ALL_EVENTS                       (SBP_ICALL_EVT|                \
                                              SBP_QUEUE_EVT|                \
                                              SBP_STATE_CHANGE_EVT|         \
                                              SBP_CHAR_CHANGE_EVT|          \
                                              SBP_PERIODIC_EVT|             \
                                              SBP_CONN_EVT_END_EVT|         \
                                              SBP_CHAR_AsyNotify_EVT|       \
                                              SBP_IniativeDisconnect_EVT|   \
                                              SBP_FLASH_OPERATION_EVT|      \
                                              SBP_QUEUE_PING_EVT)

#endif												  

#define	Timer_UnInteractive_TimeOut_Per1s		20

/*********************************************************************
 * TYPEDEFS
 */

// App event passed from profiles.

typedef struct
{
  appEvtHdr_t hdr ;  // event header.
  uint8_t data_len ;
  uint8_t data[BLE_CHAR_MAX_LEN] ;
} sbpEvt_t;



/*********************************************************************
 * GLOBAL VARIABLES
 */

// Display Interface
Display_Handle dispHandle = NULL;
//PIN_Handle hGPIOPin = NULL;
//PIN_State pinStateGPIO;
/*********************************************************************
 * LOCAL VARIABLES
 */

// Entity ID globally used to check for source and/or destination of messages
static ICall_EntityID selfEntity;

// Semaphore globally used to post events to the application thread
//static ICall_Semaphore sem;
static ICall_SyncHandle syncEvent;    //Add
// Clock instances for internal periodic events.
static Clock_Struct periodicClock;

// Queue object used for app messages
static Queue_Struct appMsg;
static Queue_Handle appMsgQueue;

#if defined(FEATURE_OAD)
// Event data from OAD profile.
static Queue_Struct oadQ;
static Queue_Handle hOadQ;
#endif //FEATURE_OAD

// events flag for internal application events.
//static uint16_t events;

// Task configuration
Task_Struct sbpTask;
Char sbpTaskStack[SBP_TASK_STACK_SIZE];

#if WATCHDOGTIMER_COMPILE
static Watchdog_Handle hWatchdog;
#endif

// Profile state and parameters
//static gaprole_States_t gapProfileState = GAPROLE_INIT;

// GAP - SCAN RSP data (max size = 31 bytes)
static uint8_t scanRspData[] =
{
  // complete name
  0x0D,   // length of this data
  GAP_ADTYPE_LOCAL_NAME_COMPLETE,
  'L',
  'e',
  't',
  'U',
  's',
  'Z',
  'e',
  'e',
  'B',
  'i',
  'k',
  'e',


  // connection interval range
  0x05,   // length of this data
  GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE,
  LO_UINT16(DEFAULT_DESIRED_MIN_CONN_INTERVAL),   // 100ms
  HI_UINT16(DEFAULT_DESIRED_MIN_CONN_INTERVAL),
  LO_UINT16(DEFAULT_DESIRED_MAX_CONN_INTERVAL),   // 1s
  HI_UINT16(DEFAULT_DESIRED_MAX_CONN_INTERVAL),

  // Tx power level
  0x02,   // length of this data
  GAP_ADTYPE_POWER_LEVEL,
  5	//5dBm	 //0       // 0dBm
};

// GAP - Advertisement data (max size = 31 bytes, though this is
// best kept short to conserve power while advertisting)
static uint8_t advertData[] =
{
  // Flags; this sets the device to use limited discoverable
  // mode (advertises for 30 seconds at a time) instead of general
  // discoverable mode (advertises indefinitely)
  0x02,   // length of this data
  GAP_ADTYPE_FLAGS,
  DEFAULT_DISCOVERABLE_MODE | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,

  // service UUID, to notify central devices what services are included
  // in this peripheral
#if !defined(FEATURE_OAD) || defined(FEATURE_OAD_ONCHIP)
  0x03,   // length of this data
#else //OAD for external flash
  0x05,  // lenght of this data
#endif //FEATURE_OAD
  GAP_ADTYPE_16BIT_MORE,      // some of the UUID's, but not all
#ifdef FEATURE_OAD
  LO_UINT16(OAD_SERVICE_UUID),
  HI_UINT16(OAD_SERVICE_UUID),
#endif //FEATURE_OAD
#ifndef FEATURE_OAD_ONCHIP
  LO_UINT16(SIMPLEPROFILE_SERV_UUID),
  HI_UINT16(SIMPLEPROFILE_SERV_UUID),
#endif //FEATURE_OAD_ONCHIP
	  
	0X0A,	  
	GAP_ADTYPE_MANUFACTURER_SPECIFIC,
	0X3c,		//"b"
	0X5a,		//"z"	
	'Z',
	'e',
	'e',
	'B',
	'i',
	'k',
	'e'
	  
};

// GAP GATT Attributes
static uint8_t attDeviceName[GAP_DEVICE_NAME_LEN] = "LetUsZeebike" ;//"Simple BLE Peripheral";

// Globals used for ATT Response retransmission
static gattMsgEvent_t *pAttRsp = NULL;
static uint8_t rspTxRetry = 0;

static uint8_t timer_uninteractive_timerout_per1s = Timer_UnInteractive_TimeOut_Per1s;

bool flag_oading = false ;
#if PLUS_OBSERVER  
uint8_t PkeINDistance=false;
uint8_t PkeOutDistancecount=0;
//distance calculae by rssi
float distance=0xff;
uint8_t *PgapObserverRoletemp=NULL;
uint8_t scanRecount=0;
uint8_t BLE_SEARCH_UUID[16]={0xFD,0xA5,0x06,0x93,0xA4,0xE2,0x4F,0xB1,0xAF,0xCF,0xC6,0xEB,0x07,0x64,0x78,0x25};
// Scan result list
static gapDevRec_t devList[DEFAULT_MAX_SCAN_RES];
static float GetDistance_ByRssi(uint8_t i);
#endif
/*********************************************************************
 * LOCAL FUNCTIONS
 */

static void SimpleBLEPeripheral_init( void );
static void SimpleBLEPeripheral_taskFxn(UArg a0, UArg a1);

static uint8_t SimpleBLEPeripheral_processStackMsg(ICall_Hdr *pMsg);
static uint8_t SimpleBLEPeripheral_processGATTMsg(gattMsgEvent_t *pMsg);
static void SimpleBLEPeripheral_processAppMsg(sbpEvt_t *pMsg);
static void SimpleBLEPeripheral_processStateChangeEvt(gaprole_States_t newState);
//static void SimpleBLEPeripheral_processCharValueChangeEvt(uint8_t paramID);
//static void SimpleBLEPeripheral_processCharValueChangeEvt(uint8_t paramID,uint8_t data_len);
static void SimpleBLEPeripheral_processCharValueChangeEvt(sbpEvt_t *pMsg);
static void SimpleBLEPeripheral_processAsyNotifyEvt(sbpEvt_t *pMsg);

static void user_flashOperationMessageCB(const uint8_t action) ;
static void SimpleBLEPeripheral_processIniativeDisconnectEvt(sbpEvt_t *pMsg);
static void SimpleBLEPeripheral_performPeriodicTask(void);
static void SimpleBLEPeripheral_clockHandler(UArg arg);

static void SimpleBLEPeripheral_sendAttRsp(void);
static void SimpleBLEPeripheral_freeAttRsp(uint8_t status);

static void SimpleBLEPeripheral_stateChangeCB(gaprole_States_t newState);
#ifndef FEATURE_OAD_ONCHIP
static void SimpleBLEPeripheral_charValueChangeCB(uint8_t paramID);
#endif //!FEATURE_OAD_ONCHIP
static void SimpleBLEPeripheral_enqueueMsg(uint8_t event, uint8_t state);
static bool  IsUnInteractiveTimeoutDecreaseConditionMet(void) ;

#if WATCHDOGTIMER_COMPILE
static void WatchDogTimerInitFxn(void) ;
static void WatchDogTimerCallback(UArg handle) ; 
#endif

static uint8_t Hex2ASCII(uint8_t data) ;
static void ChangeDeviceName(void) ;

#ifdef FEATURE_OAD
void SimpleBLEPeripheral_processOadWriteCB(uint8_t event, uint16_t connHandle,
                                           uint8_t *pData);
#endif //FEATURE_OAD

#if PLUS_OBSERVER  
static void SimpleBLEPeripheralObserver_processRoleEvent(gapPeripheralObserverRoleEvent_t *pEvent);
//static void SimpleBLEPeripheral_ObserverStateChangeCB(gapPeripheralObserverRoleEvent_t *pEvent);
static void SimpleBLEPeripheralObserver_StateChangeCB(gapPeripheralObserverRoleEvent_t *pEvent);
char *Util_convertBytes2Str(uint8_t *pData, uint8_t length);
void SimpleBLECentral_startDiscHandler();
static void SimpleBLECentral_startDiscovery(void);
static void SimpleBLECentral_addDeviceInfo(uint8_t *pAddr, uint8_t addrType,uint8_t rssidata);
#endif
/*********************************************************************
 * PROFILE CALLBACKS
 */

// GAP Role Callbacks
static gapRolesCBs_t SimpleBLEPeripheral_gapRoleCBs =
{
  SimpleBLEPeripheral_stateChangeCB     // Profile State Change Callbacks
  #if PLUS_OBSERVER  
   ,SimpleBLEPeripheralObserver_StateChangeCB
  #endif
};

// GAP Bond Manager Callbacks
static gapBondCBs_t simpleBLEPeripheral_BondMgrCBs =
{
  NULL, // Passcode callback (not used by application)
  NULL  // Pairing / Bonding state Callback (not used by application)
};

// Simple GATT Profile Callbacks
#ifndef FEATURE_OAD_ONCHIP
static simpleProfileCBs_t SimpleBLEPeripheral_simpleProfileCBs =
{
  .pfnSimpleProfileChange=SimpleBLEPeripheral_charValueChangeCB // Characteristic value change callback
};
#endif //!FEATURE_OAD_ONCHIP

#ifdef FEATURE_OAD
static oadTargetCBs_t simpleBLEPeripheral_oadCBs =
{
  SimpleBLEPeripheral_processOadWriteCB // Write Callback.
};
#endif //FEATURE_OAD

static flashOperationMessageCBs_t user_flashOperationMessageCBs =
{
  .pfnNotifyCb=user_flashOperationMessageCB
};
/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      SimpleBLEPeripheral_createTask
 *
 * @brief   Task creation function for the Simple BLE Peripheral.
 *
 * @param   None.
 *
 * @return  None.
 */
void SimpleBLEPeripheral_createTask(void)
{
  Task_Params taskParams;

  // Configure task
  Task_Params_init(&taskParams);
  taskParams.stack = sbpTaskStack;
  taskParams.stackSize = SBP_TASK_STACK_SIZE;
  taskParams.priority = SBP_TASK_PRIORITY;

  Task_construct(&sbpTask, SimpleBLEPeripheral_taskFxn, &taskParams, NULL);
}

/*********************************************************************
 * @fn      SimpleBLEPeripheral_init
 *
 * @brief   Called during initialization and contains application
 *          specific initialization (ie. hardware initialization/setup,
 *          table initialization, power up notification, etc), and
 *          profile initialization/setup.
 *
 * @param   None.
 *
 * @return  None.
 */
static void SimpleBLEPeripheral_init(void)
{
  // ******************************************************************
  // N0 STACK API CALLS CAN OCCUR BEFORE THIS CALL TO ICall_registerApp
  // ******************************************************************
  // Register the current thread as an ICall dispatcher application
  // so that the application can send and receive messages.
	 hGpioPin = PIN_open(&pinStateGPIO, BoardGpioInitTable) ;
  //GetGpioPinHandle();
	
  ICall_registerApp(&selfEntity, &syncEvent);

#ifdef USE_RCOSC
  RCOSC_enableCalibration();
#endif // USE_RCOSC

#if defined( USE_FPGA )
  // configure RF Core SMI Data Link
  IOCPortConfigureSet(IOID_12, IOC_PORT_RFC_GPO0, IOC_STD_OUTPUT);
  IOCPortConfigureSet(IOID_11, IOC_PORT_RFC_GPI0, IOC_STD_INPUT);

  // configure RF Core SMI Command Link
  IOCPortConfigureSet(IOID_10, IOC_IOCFG0_PORT_ID_RFC_SMI_CL_OUT, IOC_STD_OUTPUT);
  IOCPortConfigureSet(IOID_9, IOC_IOCFG0_PORT_ID_RFC_SMI_CL_IN, IOC_STD_INPUT);

  // configure RF Core tracer IO
  IOCPortConfigureSet(IOID_8, IOC_PORT_RFC_TRC, IOC_STD_OUTPUT);
#else // !USE_FPGA
  #if defined( DEBUG_SW_TRACE )
    // configure RF Core tracer IO
    IOCPortConfigureSet(IOID_8, IOC_PORT_RFC_TRC, IOC_STD_OUTPUT | IOC_CURRENT_4MA | IOC_SLEW_ENABLE);
  #endif // DEBUG_SW_TRACE
#endif // USE_FPGA

  // Create an RTOS queue for message from profile to be sent to app.
  appMsgQueue = Util_constructQueue(&appMsg);

  // Create one-shot clocks for internal periodic events.
  Util_constructClock(&periodicClock, SimpleBLEPeripheral_clockHandler,
                      SBP_PERIODIC_EVT_PERIOD, 0, false, SBP_PERIODIC_EVT);

#if WATCHDOGTIMER_COMPILE 
   WatchDogTimerInitFxn();
#endif 
  dispHandle = Display_open(Display_Type_LCD, NULL);
  
  #if PLUS_OBSERVER
  //Setup GAP Observer params
  {
    uint8_t scanRes = DEFAULT_MAX_SCAN_RES;
    
    GAPRole_SetParameter(GAPROLE_MAX_SCAN_RES, sizeof(uint8_t), 
                                &scanRes);
    // Set the GAP Characteristics
    GAP_SetParamValue(TGAP_GEN_DISC_SCAN, DEFAULT_SCAN_DURATION); //how long to scan (in scan state)
    GAP_SetParamValue(TGAP_LIM_DISC_SCAN, DEFAULT_SCAN_DURATION);    
    //Set scan interval
    GAP_SetParamValue(TGAP_GEN_DISC_SCAN_INT, 350); //every sec 1600*.625=1000ms 
    //Set scan window
    GAP_SetParamValue(TGAP_GEN_DISC_SCAN_WIND, 320); //every 200ms
  
  }  
  #endif
  // Setup the GAP
  GAP_SetParamValue(TGAP_CONN_PAUSE_PERIPHERAL, DEFAULT_CONN_PAUSE_PERIPHERAL);
  // Setup the GAP Peripheral Role Profile
  {
    // For all hardware platforms, device starts advertising upon initialization
    //close adv , get ble address ,then open adv
    uint8_t initialAdvertEnable = TRUE;

    // By setting this to zero, the device will go into the waiting state after
    // being discoverable for 30.72 second, and will not being advertising again
    // until the enabler is set back to TRUE
    uint16_t advertOffTime = 0;

    uint8_t enableUpdateRequest = DEFAULT_ENABLE_UPDATE_REQUEST;
    uint16_t desiredMinInterval = DEFAULT_DESIRED_MIN_CONN_INTERVAL;
    uint16_t desiredMaxInterval = DEFAULT_DESIRED_MAX_CONN_INTERVAL;
    uint16_t desiredSlaveLatency = DEFAULT_DESIRED_SLAVE_LATENCY;
    uint16_t desiredConnTimeout = DEFAULT_DESIRED_CONN_TIMEOUT;

    // Set the GAP Role Parameters
   GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t),
                        &initialAdvertEnable);
    GAPRole_SetParameter(GAPROLE_ADVERT_OFF_TIME, sizeof(uint16_t),
                         &advertOffTime);

    GAPRole_SetParameter(GAPROLE_SCAN_RSP_DATA, sizeof(scanRspData),
                         scanRspData);
    GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(advertData), advertData);

    GAPRole_SetParameter(GAPROLE_PARAM_UPDATE_ENABLE, sizeof(uint8_t),
                         &enableUpdateRequest);
    GAPRole_SetParameter(GAPROLE_MIN_CONN_INTERVAL, sizeof(uint16_t),
                         &desiredMinInterval);
    GAPRole_SetParameter(GAPROLE_MAX_CONN_INTERVAL, sizeof(uint16_t),
                         &desiredMaxInterval);
    GAPRole_SetParameter(GAPROLE_SLAVE_LATENCY, sizeof(uint16_t),
                         &desiredSlaveLatency);
    GAPRole_SetParameter(GAPROLE_TIMEOUT_MULTIPLIER, sizeof(uint16_t),
                         &desiredConnTimeout);
  }

  // Set the GAP Characteristics
  GGS_SetParameter(GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN, attDeviceName);

  // Set advertising interval
  {
    uint16_t advInt = DEFAULT_ADVERTISING_INTERVAL;

    GAP_SetParamValue(TGAP_LIM_DISC_ADV_INT_MIN, advInt);
    GAP_SetParamValue(TGAP_LIM_DISC_ADV_INT_MAX, advInt);
    GAP_SetParamValue(TGAP_GEN_DISC_ADV_INT_MIN, advInt);
    GAP_SetParamValue(TGAP_GEN_DISC_ADV_INT_MAX, advInt);
  }

  // Setup the GAP Bond Manager
  {
    uint32_t passkey = 0; // passkey "000000"
    uint8_t pairMode = GAPBOND_PAIRING_MODE_WAIT_FOR_REQ;
	//uint8_t pairMode = GAPBOND_PAIRING_MODE_INITIATE;
    uint8_t mitm = FALSE;
    uint8_t ioCap = GAPBOND_IO_CAP_NO_INPUT_NO_OUTPUT;
    uint8_t bonding = TRUE;

    GAPBondMgr_SetParameter(GAPBOND_DEFAULT_PASSCODE, sizeof(uint32_t),
                            &passkey);
    GAPBondMgr_SetParameter(GAPBOND_PAIRING_MODE, sizeof(uint8_t), &pairMode);
    GAPBondMgr_SetParameter(GAPBOND_MITM_PROTECTION, sizeof(uint8_t), &mitm);
    GAPBondMgr_SetParameter(GAPBOND_IO_CAPABILITIES, sizeof(uint8_t), &ioCap);
    GAPBondMgr_SetParameter(GAPBOND_BONDING_ENABLED, sizeof(uint8_t), &bonding);
  }

   // Initialize GATT attributes
  GGS_AddService(GATT_ALL_SERVICES);           // GAP
  GATTServApp_AddService(GATT_ALL_SERVICES);   // GATT attributes
  DevInfo_AddService();                        // Device Information Service

#ifndef FEATURE_OAD_ONCHIP
  SimpleProfile_AddService(GATT_ALL_SERVICES); // Simple GATT Profile
#endif //!FEATURE_OAD_ONCHIP

#ifdef FEATURE_OAD
  VOID OAD_addService();                 // OAD Profile
  OAD_register((oadTargetCBs_t *)&simpleBLEPeripheral_oadCBs);
  hOadQ = Util_constructQueue(&oadQ);
#endif //FEATURE_OAD

#ifdef IMAGE_INVALIDATE
  Reset_addService();
#endif //IMAGE_INVALIDATE


#ifndef FEATURE_OAD_ONCHIP
  // Setup the SimpleProfile Characteristic Values
  {
    uint8_t charValue1 = 1;
    /*uint8_t charValue2 = 2;
    uint8_t charValue3 = 3;
    uint8_t charValue4 = 4;
    uint8_t charValue5[SIMPLEPROFILE_CHAR5_LEN] = { 1, 2, 3, 4, 5 };*/

    SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR1, sizeof(uint8_t),
                               &charValue1);
   /* SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR2, sizeof(uint8_t),
                               &charValue2);
    SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR3, sizeof(uint8_t),
                               &charValue3);
    SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR4, sizeof(uint8_t),
                               &charValue4);
    SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR5, SIMPLEPROFILE_CHAR5_LEN,
                               charValue5);*/
  }

  // Register callback with SimpleGATTprofile
   SimpleProfile_RegisterAppCBs(&SimpleBLEPeripheral_simpleProfileCBs);
   FlashOperationMessage_RegisterAppCBs(&user_flashOperationMessageCBs);
#endif //!FEATURE_OAD_ONCHIP

  // Start the Device
  VOID GAPRole_StartDevice(&SimpleBLEPeripheral_gapRoleCBs);

  // Start Bond Manager
  VOID GAPBondMgr_Register(&simpleBLEPeripheral_BondMgrCBs);

  // Register with GAP for HCI/Host messages
  GAP_RegisterForMsgs(selfEntity);

  // Register for GATT local events and ATT Responses pending for transmission
  GATT_RegisterForMsgs(selfEntity);
//This should be included only if 4.2 length extension feature is enable....
 // HCI_LE_ReadMaxDataLenCmd();
#if !defined (USE_LL_CONN_PARAM_UPDATE)
  // Get the currently set local supported LE features
  // The HCI will generate an HCI event that will get received in the main
  // loop
  HCI_LE_ReadLocalSupportedFeaturesCmd();
#endif // !defined (USE_LL_CONN_PARAM_UPDATE)

#if defined FEATURE_OAD
#if defined (HAL_IMAGE_A)
  Display_print0(dispHandle, 0, 0, "BLE Peripheral A");
#else
  Display_print0(dispHandle, 0, 0, "BLE Peripheral B");
#endif // HAL_IMAGE_A
#else
  Display_print0(dispHandle, 0, 0, "BLE Peripheral");
#endif // FEATURE_OAD
}







/*********************************************************************
 * @fn      SimpleBLEPeripheral_taskFxn
 *
 * @brief   Application task entry point for the Simple BLE Peripheral.
 *
 * @param   a0, a1 - not used.
 *
 * @return  None.
 */
static void SimpleBLEPeripheral_taskFxn(UArg a0, UArg a1)
{
	
  uint32_t events; // Add
  // Initialize application
  SimpleBLEPeripheral_init();
  #if VOICEPACKETPWM_COMPILE
  GPTimerIntit();
  #endif
  ChangeDeviceName();
//  FlashTest();
  ReadGuardContextFromFlashWhenInitial();
  GuardStatusInitial();
  #ifdef TI_DRIVERS_LCD_MANGO 
  Board_openLCD();
  #endif
  
  SimpleBLECentral_startDiscHandler(); 
  
  // Application main loop
  for (;;)
  {	  

	 events = Event_pend(syncEvent, Event_Id_NONE, SBP_ALL_EVENTS,
                            ICALL_TIMEOUT_FOREVER); // Add
    if (events) // Add
    {
      ICall_EntityID dest;
      ICall_ServiceEnum src;
      ICall_HciExtEvt *pMsg = NULL;

      if (ICall_fetchServiceMsg(&src, &dest,
                                (void **)&pMsg) == ICALL_ERRNO_SUCCESS)
      {
        uint8 safeToDealloc = TRUE;

        if ((src == ICALL_SERVICE_CLASS_BLE) && (dest == selfEntity))
        {
          ICall_Stack_Event *pEvt = (ICall_Stack_Event *)pMsg;

          // Check for BLE stack events first
          if (pEvt->signature == 0xffff)
          {
           if (pEvt->event_flag & SBP_CONN_EVT_END_EVT)
            {
              // Try to retransmit pending ATT Response (if any)
              SimpleBLEPeripheral_sendAttRsp();
            }
          }
          else
          {
            // Process inter-task message
            safeToDealloc = SimpleBLEPeripheral_processStackMsg((ICall_Hdr *)pMsg);
          }
        }

        if (pMsg && safeToDealloc)
        {
          ICall_freeMsg(pMsg);
        }
      }

      // If RTOS queue is not empty, process app message.
	  if (events & SBP_QUEUE_EVT)
	  {
        while (!Queue_empty(appMsgQueue))
        {
        sbpEvt_t *pMsg = (sbpEvt_t *)Util_dequeueMsg(appMsgQueue);
        if (pMsg)
        {
          // Process message.
          SimpleBLEPeripheral_processAppMsg(pMsg);

          // Free the space from the message.
          ICall_free(pMsg);
        }
       }
	  }
	
	   if (events & SBP_PERIODIC_EVT)
      {
      //  events &= ~SBP_PERIODIC_EVT;
       Util_startClock(&periodicClock);
       // Perform periodic application task
      SimpleBLEPeripheral_performPeriodicTask();
      }
	   #ifdef FEATURE_OAD
	//for check the master is zeebiker's app
	 if(IsBLEConnectStatusInteractive())
	  {
		if (events & SBP_QUEUE_PING_EVT)
		{
		  while (!Queue_empty(hOadQ))
		 {
		   oadTargetWrite_t *oadWriteEvt = Queue_get(hOadQ);

		  // Identify new image.
		  if (oadWriteEvt->event == OAD_WRITE_IDENTIFY_REQ)
		  {
			flag_oading = true ;
			OAD_imgIdentifyWrite(oadWriteEvt->connHandle, oadWriteEvt->pData);
		  }
		  // Write a next block request.
		  else if (oadWriteEvt->event == OAD_WRITE_BLOCK_REQ)
		  {
			  
			OAD_imgBlockWrite(oadWriteEvt->connHandle, oadWriteEvt->pData);
		  }

		  // Free buffer.
		  ICall_free(oadWriteEvt);
		}
	   }
      }
   #endif //FEATURE_OAD
	
    }




  }
}



/*********************************************************************
 * @fn      SimpleBLEPeripheral_processStackMsg
 *
 * @brief   Process an incoming stack message.
 *
 * @param   pMsg - message to process
 *
 * @return  TRUE if safe to deallocate incoming message, FALSE otherwise.
 */
static uint8_t SimpleBLEPeripheral_processStackMsg(ICall_Hdr *pMsg)
{
  uint8_t safeToDealloc = TRUE;

  switch (pMsg->event)
  {
	#if PLUS_OBSERVER   
    case GAP_MSG_EVENT:
    // Process GATT message
      SimpleBLEPeripheralObserver_processRoleEvent((gapPeripheralObserverRoleEvent_t*)PgapObserverRoletemp);
    break;
    #endif    
    case GATT_MSG_EVENT:
      // Process GATT message
      safeToDealloc = SimpleBLEPeripheral_processGATTMsg((gattMsgEvent_t *)pMsg);
      break;

    case HCI_GAP_EVENT_EVENT:
      {
        // Process HCI message
        switch(pMsg->status)
        {
          case HCI_COMMAND_COMPLETE_EVENT_CODE:
            // Process HCI Command Complete Event
            break;

          default:
            break;
        }
      }
      break;

    default:
      // do nothing
      break;
  }

  return (safeToDealloc);
}

/*********************************************************************
 * @fn      SimpleBLEPeripheral_processGATTMsg
 *
 * @brief   Process GATT messages and events.
 *
 * @return  TRUE if safe to deallocate incoming message, FALSE otherwise.
 */
static uint8_t SimpleBLEPeripheral_processGATTMsg(gattMsgEvent_t *pMsg)
{
  // See if GATT server was unable to transmit an ATT response
  if (pMsg->hdr.status == blePending)
  {
    // No HCI buffer was available. Let's try to retransmit the response
    // on the next connection event.
    if (HCI_EXT_ConnEventNoticeCmd(pMsg->connHandle, selfEntity,
                                   SBP_CONN_EVT_END_EVT) == SUCCESS)
    {
      // First free any pending response
      SimpleBLEPeripheral_freeAttRsp(FAILURE);

      // Hold on to the response message for retransmission
      pAttRsp = pMsg;

      // Don't free the response message yet
      return (FALSE);
    }
  }
  else if (pMsg->method == ATT_FLOW_CTRL_VIOLATED_EVENT)
  {
    // ATT request-response or indication-confirmation flow control is
    // violated. All subsequent ATT requests or indications will be dropped.
    // The app is informed in case it wants to drop the connection.

    // Display the opcode of the message that caused the violation.
    Display_print1(dispHandle, 5, 0, "FC Violated: %d", pMsg->msg.flowCtrlEvt.opcode);
  }
  else if (pMsg->method == ATT_MTU_UPDATED_EVENT)
  {
    // MTU size updated
    Display_print1(dispHandle, 5, 0, "MTU Size: $d", pMsg->msg.mtuEvt.MTU);
  }

  // Free message payload. Needed only for ATT Protocol messages
  GATT_bm_free(&pMsg->msg, pMsg->method);

  // It's safe to free the incoming message
  return (TRUE);
}

/*********************************************************************
 * @fn      SimpleBLEPeripheral_sendAttRsp
 *
 * @brief   Send a pending ATT response message.
 *
 * @param   none
 *
 * @return  none
 */
static void SimpleBLEPeripheral_sendAttRsp(void)
{
  // See if there's a pending ATT Response to be transmitted
  if (pAttRsp != NULL)
  {
    uint8_t status;

    // Increment retransmission count
    rspTxRetry++;

    // Try to retransmit ATT response till either we're successful or
    // the ATT Client times out (after 30s) and drops the connection.
    status = GATT_SendRsp(pAttRsp->connHandle, pAttRsp->method, &(pAttRsp->msg));
    if ((status != blePending) && (status != MSG_BUFFER_NOT_AVAIL))
    {
      // Disable connection event end notice
      HCI_EXT_ConnEventNoticeCmd(pAttRsp->connHandle, selfEntity, 0);

      // We're done with the response message
      SimpleBLEPeripheral_freeAttRsp(status);
    }
    else
    {
      // Continue retrying
      Display_print1(dispHandle, 5, 0, "Rsp send retry: %d", rspTxRetry);
    }
  }
}

/*********************************************************************
 * @fn      SimpleBLEPeripheral_freeAttRsp
 *
 * @brief   Free ATT response message.
 *
 * @param   status - response transmit status
 *
 * @return  none
 */
static void SimpleBLEPeripheral_freeAttRsp(uint8_t status)
{
  // See if there's a pending ATT response message
  if (pAttRsp != NULL)
  {
    // See if the response was sent out successfully
    if (status == SUCCESS)
    {
      Display_print1(dispHandle, 5, 0, "Rsp sent retry: %d", rspTxRetry);
    }
    else
    {
      // Free response payload
      GATT_bm_free(&pAttRsp->msg, pAttRsp->method);

      Display_print1(dispHandle, 5, 0, "Rsp retry failed: %d", rspTxRetry);
    }

    // Free response message
    ICall_freeMsg(pAttRsp);

    // Reset our globals
    pAttRsp = NULL;
    rspTxRetry = 0;
  }
}

/*********************************************************************
 * @fn      SimpleBLEPeripheral_processAppMsg
 *
 * @brief   Process an incoming callback from a profile.
 *
 * @param   pMsg - message to process
 *
 * @return  None.
 */
static void SimpleBLEPeripheral_processAppMsg(sbpEvt_t *pMsg)
{
  switch (pMsg->hdr.event)
  {
    case SBP_STATE_CHANGE_EVT:
      SimpleBLEPeripheral_processStateChangeEvt((gaprole_States_t)pMsg->
                                                hdr.state);
      break;

    case SBP_CHAR_CHANGE_EVT:
//      SimpleBLEPeripheral_processCharValueChangeEvt(pMsg->hdr.state, p);
//		SimpleBLEPeripheral_processCharValueChangeEvt(pMsg->hdr.state, pMsg->data_len);
		SimpleBLEPeripheral_processCharValueChangeEvt(pMsg);
      break;
	case SBP_CHAR_AsyNotify_EVT:
		SimpleBLEPeripheral_processAsyNotifyEvt(pMsg);
		break;	
  	case SBP_FLASH_OPERATION_EVT:
		FlashProtectionAction();
		break ;
   case SBP_IniativeDisconnect_EVT:
	  	SimpleBLEPeripheral_processIniativeDisconnectEvt(pMsg);
		break ;  
  #if PLUS_OBSERVER      
    case SBP_OBSERVER_STATE_CHANGE_EVT:
      SimpleBLEPeripheral_processStackMsg((ICall_Hdr *)PgapObserverRoletemp);
      // Free the stack message
    //  ICall_freeMsg(pMsg->pData);
        break;
	case SBC_START_DISCOVERY_EVT:
		  SimpleBLECentral_startDiscovery();
		  break;
  #endif
    default:
      // Do nothing.
      break;
  }
}

/*********************************************************************
 * @fn      SimpleBLEPeripheral_stateChangeCB
 *
 * @brief   Callback from GAP Role indicating a role state change.
 *
 * @param   newState - new state
 *
 * @return  None.
 */
static void SimpleBLEPeripheral_stateChangeCB(gaprole_States_t newState)
{
  SimpleBLEPeripheral_enqueueMsg(SBP_STATE_CHANGE_EVT, newState);
}

/*********************************************************************
 * @fn      SimpleBLEPeripheral_processStateChangeEvt
 *
 * @brief   Process a pending GAP Role state change event.
 *
 * @param   newState - new state
 *
 * @return  None.
 */
static void SimpleBLEPeripheral_processStateChangeEvt(gaprole_States_t newState)
{
#ifdef PLUS_BROADCASTER
  static bool firstConnFlag = false;
#endif // PLUS_BROADCASTER
 
#if DEBUG_COMPILE 
  if(test1 !=  newState)
  {
  	test2 = test1 ;
	test1= newState ;
  }
#endif
  
  switch ( newState )
  {
    case GAPROLE_STARTED:
		ChangeBLEConnectStatus(BLE_Unconnect) ;
		 #ifdef TI_DRIVERS_LCD_MANGO 
		 LCD_ManGo_Printf(2,"Initialized");	
		  
         #endif
    	break;

    case GAPROLE_ADVERTISING:
		ChangeBLEConnectStatus(BLE_Unconnect) ;
		 #ifdef TI_DRIVERS_LCD_MANGO 
		  LCD_ManGo_clearLines(0,6);
		 LCD_ManGo_Printf(2,"advertising");	
		 #endif
      	break;

#ifdef PLUS_BROADCASTER
    /* After a connection is dropped a device in PLUS_BROADCASTER will continue
     * sending non-connectable advertisements and shall sending this change of
     * state to the application.  These are then disabled here so that sending
     * connectable advertisements can resume.
     */
    case GAPROLE_ADVERTISING_NONCONN:		//adv which can't connected
      {
        uint8_t advertEnabled = FALSE;

        // Disable non-connectable advertising.
        GAPRole_SetParameter(GAPROLE_ADV_NONCONN_ENABLED, sizeof(uint8_t),
                           &advertEnabled);

        advertEnabled = TRUE;

        // Enabled connectable advertising.
        GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t),
                             &advertEnabled);

        // Reset flag for next connection.
        firstConnFlag = false;

        SimpleBLEPeripheral_freeAttRsp(bleNotConnected);
      }
      break;
#endif //PLUS_BROADCASTER

    case GAPROLE_CONNECTED:
      {
		 #ifdef TI_DRIVERS_LCD_MANGO 
		   LCD_ManGo_clearLines(0,6);
		   LCD_ManGo_Printf(2,"Connected");			
          #endif  
        linkDBInfo_t linkInfo;
        uint8_t numActive = 0;

        Util_startClock(&periodicClock);
        numActive = linkDB_NumActive();
        // Use numActive to determine the connection handle of the last
        // connection
        if ( linkDB_GetInfo( numActive - 1, &linkInfo ) == SUCCESS )
        {
          Display_print1(dispHandle, 2, 0, "Num Conns: %d", (uint16_t)numActive);
          Display_print0(dispHandle, 3, 0, Util_convertBdAddr2Str(linkInfo.addr));
        }
        else
        {
          uint8_t peerAddress[B_ADDR_LEN];
          GAPRole_GetParameter(GAPROLE_CONN_BD_ADDR, peerAddress);
          Display_print0(dispHandle, 2, 0, "Connected");
          Display_print0(dispHandle, 3, 0, Util_convertBdAddr2Str(peerAddress));
		  
        }

        #ifdef PLUS_BROADCASTER
          // Only turn advertising on for this state when we first connect
          // otherwise, when we go from connected_advertising back to this state
          // we will be turning advertising back on.
          if (firstConnFlag == false)
          {
            uint8_t advertEnabled = FALSE; // Turn on Advertising

            // Disable connectable advertising.
            GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t),
                                 &advertEnabled);

            // Set to true for non-connectabel advertising.
            advertEnabled = TRUE;

            // Enable non-connectable advertising.
            GAPRole_SetParameter(GAPROLE_ADV_NONCONN_ENABLED, sizeof(uint8_t),
                                 &advertEnabled);
            firstConnFlag = true;
          }
        #endif // PLUS_BROADCASTER
      }
	  
	  ChangeBLEConnectStatus(BLE_ConnectButNoneInteractive) ;	
      break;

    case GAPROLE_CONNECTED_ADV:		//unused in this project
      break;

    case GAPROLE_WAITING:
      Util_stopClock(&periodicClock);
      SimpleBLEPeripheral_freeAttRsp(bleNotConnected);	  
      break;

    case GAPROLE_WAITING_AFTER_TIMEOUT:
      SimpleBLEPeripheral_freeAttRsp(bleNotConnected);

      Display_print0(dispHandle, 2, 0, "Timed Out");

      // Clear remaining lines
      Display_clearLines(dispHandle, 3, 5);

      #ifdef PLUS_BROADCASTER
        // Reset flag for next connection.
        firstConnFlag = false;
      #endif //#ifdef (PLUS_BROADCASTER)
      break;

    case GAPROLE_ERROR:
      //Display_print0(dispHandle, 2, 0, "Error");
		SetFlagResetMCUWithCheckSpeedAndSaveFlash();
      break;

    default:
      Display_clearLine(dispHandle, 2);
	   #ifdef TI_DRIVERS_LCD_MANGO 
	   LCD_ManGo_clearLines(2,2);	
       #endif
      break;
  }

  // Update the state
  //gapProfileState = newState;
}

#ifndef FEATURE_OAD_ONCHIP
/*********************************************************************
 * @fn      SimpleBLEPeripheral_charValueChangeCB
 *
 * @brief   Callback from Simple Profile indicating a characteristic
 *          value change.
 *
 * @param   paramID - parameter ID of the value that was changed.
 *
 * @return  None.
 */
static void SimpleBLEPeripheral_charValueChangeCB(uint8_t paramID)
{
  SimpleBLEPeripheral_enqueueMsg(SBP_CHAR_CHANGE_EVT, paramID);
}
#endif //!FEATURE_OAD_ONCHIP

/*********************************************************************
 * @fn      SimpleBLEPeripheral_processCharValueChangeEvt
 *
 * @brief   Process a pending Simple Profile characteristic value change
 *          event.
 *
 * @param   paramID - parameter ID of the value that was changed.
 *
 * @return  None.
 */
//static void SimpleBLEPeripheral_processCharValueChangeEvt(uint8_t paramID)
static void SimpleBLEPeripheral_processCharValueChangeEvt(sbpEvt_t *pMsg)
{
#ifndef FEATURE_OAD_ONCHIP
	switch(pMsg->hdr.state)
  	{
    case SIMPLEPROFILE_CHAR1:
		{	
			//this strlen should use queue value.if use global variable,the strlen may be changed by other task
//			ATControl(pMsg->data, GetLengthOfUpdateCmdVariable());	//wrong
			ATControl(pMsg->data, pMsg->data_len);
			
			if(BLEResponse.length > 0)
			{
//				uint8_t someVal[BLE_CHAR_MAX_LEN] = {0};
//				memcpy(someVal, BLEResponse.arr, BLEResponse.length);
//				SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR1, BLEResponse.length, someVal);
				SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR1, BLEResponse.length, &BLEResponse.arr);
			}
				  
    
    
		}
      break;


    default:
      // should not reach here!
      break;
  }
#endif //!FEATURE_OAD_ONCHIP
}

/*********************************************************************
 * @brief   
 *
 * @param   paramID - parameter ID of the value that was changed.
 *
 * @return  None.
 */
//static void SimpleBLEPeripheral_processCharValueChangeEvt(uint8_t paramID)
static void SimpleBLEPeripheral_processAsyNotifyEvt(sbpEvt_t *pMsg)
{
#ifndef FEATURE_OAD_ONCHIP
	switch(pMsg->hdr.state)
	{
	case SIMPLEPROFILE_CHAR1:
		{	
			if(BLEResponse.length > 0)
			{
				SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR1, pMsg->data_len, &pMsg->data);
			}
			break;
			
		}	
		default:
			// should not reach here!
			break;
		
#endif //!FEATURE_OAD_ONCHIP
		
		
		
	}
}	

/*********************************************************************
 * @brief   
 *
 * @param   paramID -
 *
 * @return  None.
 */
static void user_flashOperationMessageCB(const uint8_t action)
{
	SimpleBLEPeripheral_enqueueMsg(SBP_FLASH_OPERATION_EVT, Flash_Protection);
}

/*********************************************************************
 * @brief   
 *
 * @param   paramID - 
 *
 * @return  None.
 */
static void SimpleBLEPeripheral_processIniativeDisconnectEvt(sbpEvt_t *pMsg)
{
#ifndef FEATURE_OAD_ONCHIP
	switch(pMsg->hdr.state)
	{
	case SIMPLEPROFILE_CHAR1:
		{	
			GAPRole_TerminateConnection(); 
			//bleconnectstatus = BLE_Unconnect ;
			SetBLEConnectStatus(BLE_Unconnect);
			break;
			
		}	
		default:
			// should not reach here!
			break;
		
#endif //!FEATURE_OAD_ONCHIP
		
		
		
	}
}	

/*********************************************************************
 * @fn      SimpleBLEPeripheral_performPeriodicTask
 *
 * @brief   Perform a periodic application task. This function gets called
 *          every five seconds (SBP_PERIODIC_EVT_PERIOD). In this example,
 *          the value of the third characteristic in the SimpleGATTProfile
 *          service is retrieved from the profile, and then copied into the
 *          value of the the fourth characteristic.
 *
 * @param   None.
 *
 * @return  None.
 */
static void SimpleBLEPeripheral_performPeriodicTask(void)
{
#ifndef FEATURE_OAD_ONCHIP


#endif //!FEATURE_OAD_ONCHIP
}


#ifdef FEATURE_OAD
/*********************************************************************
 * @fn      SimpleBLEPeripheral_processOadWriteCB
 *
 * @brief   Process a write request to the OAD profile.
 *
 * @param   event      - event type:
 *                       OAD_WRITE_IDENTIFY_REQ
 *                       OAD_WRITE_BLOCK_REQ
 * @param   connHandle - the connection Handle this request is from.
 * @param   pData      - pointer to data for processing and/or storing.
 *
 * @return  None.
 */
void SimpleBLEPeripheral_processOadWriteCB(uint8_t event, uint16_t connHandle,
                                           uint8_t *pData)
{
  oadTargetWrite_t *oadWriteEvt = ICall_malloc( sizeof(oadTargetWrite_t) + \
                                             sizeof(uint8_t) * OAD_PACKET_SIZE);

  if ( oadWriteEvt != NULL )
  {
    oadWriteEvt->event = event;
    oadWriteEvt->connHandle = connHandle;

    oadWriteEvt->pData = (uint8_t *)(&oadWriteEvt->pData + 1);
    memcpy(oadWriteEvt->pData, pData, OAD_PACKET_SIZE);

    Queue_put(hOadQ, (Queue_Elem *)oadWriteEvt);

    // Post the application's semaphore.
   // Semaphore_post(syncEvent);
	 Event_post(syncEvent, SBP_QUEUE_PING_EVT);
  }
  else
  {
    // Fail silently.
  }
}
#endif //FEATURE_OAD

/*********************************************************************
 * @fn      SimpleBLEPeripheral_clockHandler
 *
 * @brief   Handler function for clock timeouts.
 *
 * @param   arg - event type
 *
 * @return  None.
 */
static void SimpleBLEPeripheral_clockHandler(UArg arg)
{ 
	 Event_post(syncEvent, arg); // Add
}



#if PLUS_OBSERVER 
void SimpleBLECentral_startDiscHandler()
{ 
  SimpleBLEPeripheral_enqueueMsg(SBC_START_DISCOVERY_EVT , SUCCESS) ;
}

static void SimpleBLECentral_startDiscovery(void)
{	
     GAPObserverRole_StartDiscovery(DEFAULT_DISCOVERY_MODE,
                                    DEFAULT_DISCOVERY_ACTIVE_SCAN,
                                    DEFAULT_DISCOVERY_WHITE_LIST);  	  
}
#endif
/*********************************************************************
 * @fn      SimpleBLEPeripheral_enqueueMsg
 *
 * @brief   Creates a message and puts the message in RTOS queue.
 *
 * @param   event - message event.
 * @param   state - message state.
 *
 * @return  None.
 */
static void SimpleBLEPeripheral_enqueueMsg(uint8_t event, uint8_t state)
{
	sbpEvt_t *pMsg;
	
	// Create dynamic pointer to message.
	if ((pMsg = ICall_malloc(sizeof(sbpEvt_t))))
	{
		pMsg->hdr.event = event;
		pMsg->hdr.state = state;
		if(SBP_CHAR_AsyNotify_EVT == event)
		{
			pMsg->data_len = getLengthOfAsynchronousNotify() ;
			memcpy(pMsg->data,getHandleOfAsynchronousNotify() ,pMsg->data_len) ;	
		}
		else if(SBP_CHAR_CHANGE_EVT == event)
		{
			pMsg->data_len = GetLengthOfUpdateCmdVariable() ;
			SimpleProfile_GetParameter(state, pMsg->data);	
		}
		
		else
			;
		
		// Enqueue the message.
		Util_enqueueMsg(appMsgQueue, syncEvent, (uint8*)pMsg);
	}
}

/*********************************************************************
 * @brief  
 *
 * @param   
 *
 * @return  
 *********************************************************************/
void SimpleBLEPeripheral_enqueueAsySendMsg(void)
{
	SimpleBLEPeripheral_enqueueMsg(SBP_CHAR_AsyNotify_EVT , SIMPLEPROFILE_CHAR1) ;
}

/*********************************************************************
 * @brief  
 *
 * @param   
 *
 * @return  
 *********************************************************************/
void SimpleBLEPeripheral_enqueueInitiativeDisConnectMsg(void)
{
	SimpleBLEPeripheral_enqueueMsg(SBP_IniativeDisconnect_EVT , SIMPLEPROFILE_CHAR1) ;
}

/*********************************************************************
 * @brief  
 *
 * @param   
 *
 * @return  
 *********************************************************************/
 static bool  IsUnInteractiveTimeoutDecreaseConditionMet(void)
{
	if(BLE_ConnectButNoneInteractive == GetBLEConnectStatus())
		return true ;
	return false ;
 }

/*********************************************************************
 * @brief  
 *
 * @param   
 *
 * @return  
 *********************************************************************/
 void CheckUnInteractiveTimeout(void)
{
	if(IsUnInteractiveTimeoutDecreaseConditionMet())
	{
		if(true == IsVariableTimerDecreaseToZero(&timer_uninteractive_timerout_per1s))	
			SimpleBLEPeripheral_enqueueInitiativeDisConnectMsg();
	}
 }

/*********************************************************************
 * @brief  
 *
 * @param   
 *
 * @return  
 *********************************************************************/
void InitTimerUninteractiveTimeroutPer1s(void)
{
	timer_uninteractive_timerout_per1s = Timer_UnInteractive_TimeOut_Per1s ;
}

#if WATCHDOGTIMER_COMPILE
/*********************************************************************
 * @brief  
 *
 * @param   
 *
 * @return  
 *********************************************************************/
static void WatchDogTimerCallback(UArg handle)   
{  
	// Reset.
     HAL_SYSTEM_RESET(); 	 
}  

/*********************************************************************
 * @brief  
 *
 * @param   
 *
 * @return  
 *********************************************************************/
static void WatchDogTimerInitFxn(void) 
{
	Watchdog_Params params;    
	Watchdog_init();
    Watchdog_Params_init(&params);  
    params.callbackFxn    = WatchDogTimerCallback;
	params.resetMode      = Watchdog_RESET_ON;  
#if DEBUG_COMPILE	
	params.debugStallMode =  Watchdog_DEBUG_STALL_ON ;
#else
   	params.debugStallMode =  Watchdog_DEBUG_STALL_OFF ;
#endif
    
  
    hWatchdog = Watchdog_open(Board_WATCHDOG, &params);  
	uint32_t tickValue;
	tickValue = Watchdog_convertMsToTicks(hWatchdog, 1000);
    Watchdog_setReload(hWatchdog, tickValue); 	// 1sec (WDT runs always at 48MHz/32)  
}

/*********************************************************************
 * @brief  
 *
 * @param   
 *
 * @return  
 *********************************************************************/
void ClearWatchDogTimer(void) 
{
	Watchdog_clear(hWatchdog);
}

#endif
/*********************************************************************
 * @brief  
 *
 * @param   
 *
 * @return  
 *********************************************************************/
static uint8_t Hex2ASCII(uint8_t data)
{
	if(data <= 9)
		return (data + 0X30) ;
	if(0X0A <= data <= 0X0F)
		return (data + 0X37) ;
	else
		return data ;
}


/*********************************************************************   
 * @brief  
 *
 * @param   
 *
 * @return  
 *********************************************************************/
static void ChangeDeviceName(void)
{
  	uint8_t ownAddress[B_ADDR_LEN] ;
	uint8_t DeviceName_AddressASCII[GAP_DEVICE_NAME_LEN] ;
	//get MAC address
	GAPRole_GetParameter(GAPROLE_BD_ADDR, ownAddress) ;
	
	//Change 6byte Address Hex to 12byte ASCII Name 
	//for example: get 6byte Address hex are 0x06 0xB5 0xBC 0x84 0xBE 0xC4
	//should return ACSII: C4 BE 84 BC B5 06
	uint8_t temp ;
	for(uint8_t i = 0,  j = (B_ADDR_LEN - 1) ; i < GAP_DEVICE_NAME_LEN ; i++)
	{
		temp = ownAddress[j] ;
		if(i % 2==1)
		{
			temp &= 0X0F ; 
			DeviceName_AddressASCII[i] =  Hex2ASCII(temp) ;
			j-- ;
		}
		else
		{
			temp >>= 4 ;
			DeviceName_AddressASCII[i] =  Hex2ASCII(temp) ;
		}		
	}
		
	for(uint8_t i= 0 ; i < GAP_DEVICE_NAME_LEN ; i ++)
		attDeviceName[i] = DeviceName_AddressASCII[i] ;
	for(uint8_t i= 0 ; i < GAP_DEVICE_NAME_LEN ; i ++)
		scanRspData[ 2 + i] = DeviceName_AddressASCII[i] ;
	GAPRole_SetParameter( GAPROLE_SCAN_RSP_DATA, sizeof (scanRspData), scanRspData );
	GGS_SetParameter(GGS_DEVICE_NAME_ATT, sizeof (attDeviceName) , attDeviceName) ;
}

#if PLUS_OBSERVER
/*********************************************************************
 * @fn      Util_convertBytes2Str
 *
 * @brief   Convert bytes to string. Used to print advertising data. 
 *         
 * @param   pData - data
 * @param   length - data length
 *
 * @return  Adv/Scan data as a string
 */
char *Util_convertBytes2Str(uint8_t *pData, uint8_t length)
{
  uint8_t     charCnt;
  char        hex[] = "0123456789ABCDEF";
  static char str[(3*31)+1];
  char        *pStr = str;

  for (charCnt = 0; charCnt < length && charCnt < 31; charCnt++)
  {
    *pStr++ = hex[*pData >> 4];
    *pStr++ = hex[*pData++ & 0x0F];
    *pStr++ = ':';
  }
  pStr = NULL;

  return str;
}
/*********************************************************************
 * @fn      SimpleBLECentral_findSvcUuid
 *
 * @brief   Find a given macadress 
 *
 * @return  TRUE if service macadress found
 */
static bool SimpleBLECentral_findSvcUuid(uint16_t uuid, uint8_t *pData,
                                         uint8_t dataLen,uint8_t *addr)
{
    uint8_t const addrcomp[B_ADDR_LEN]={0xD2,0x85,0xE5,0xF1,0x34,0xA4};
    if(memcmp(addrcomp,addr,B_ADDR_LEN)==0)
	{
	  return TRUE;
	}
	

  // Match not found
  return FALSE;
}
/*********************************************************************
 * @fn      SimpleBLEPeripheralObserver_processRoleEvent
 *
 * @brief   Peripheral Observer role event processing function.
 *
 * @param   pEvent - pointer to event structure
 *
 * @return  none
 */

static void SimpleBLEPeripheralObserver_processRoleEvent(gapPeripheralObserverRoleEvent_t *pEvent)
{	
  switch (pEvent->gap.opcode)
  {
    case GAP_DEVICE_INFO_EVENT:
        {
          if (SimpleBLECentral_findSvcUuid(SIMPLEPROFILE_SERV_UUID,
                                           pEvent->deviceInfo.pEvtData,
                                           pEvent->deviceInfo.dataLen,
										   pEvent->deviceInfo.addr))			
			  {
                 SimpleBLECentral_addDeviceInfo(pEvent->deviceInfo.addr,
                                                pEvent->deviceInfo.addrType,pEvent->deviceInfo.rssi);
			  }
          
        }
		
      break;

    case GAP_DEVICE_DISCOVERY_EVENT:
		{
			if(scanRecount==0)
			{
					PkeOutDistancecount++;
	            	if(PkeOutDistancecount>=5) PkeINDistance=false;
			} 
			else
			{
		         for(uint8_t i=0;i<scanRecount;i++)
		        { 
			      #ifdef TI_DRIVERS_LCD_MANGO 
			      LCD_ManGo_Printf1(6,"RSSI",(uint8_t)(devList[i].rssi[0]));
			      #endif
				  distance=GetDistance_ByRssi(i);
				  if(distance<=BLE_PARK_DISTANCE) 
				  { 
					 PkeOutDistancecount=0;
					 PkeINDistance=true;
					 break; 
				  }
				  else if(distance>=5)
				  {
					  PkeINDistance=false;
				  }
		    	 }
		     //	 for(uint8_t j=0;j<DEFAULT_MAX_SCAN_RES;j++) memset(&devList[j],0,sizeof(gapDevRec_t));
			     scanRecount=0;
			  	  #ifdef TI_DRIVERS_LCD_MANGO 
		          LCD_ManGo_clearLines(2,3);
		          LCD_ManGo_Printf(4,"GAP_DEVICE_DISC_EVENT");
		          LCD_ManGo_Printf1(5,"Distance",(uint8_t)distance);
                  #endif
			}
         
         
		}

      break;

    default:
      break;
  }
}


/*********************************************************************
 * @fn      SimpleBLEPeripheralObserver_StateChangeCB
 *
 * @brief   Peripheral observer event callback function.
 *
 * @param   pEvent - pointer to event structure
 */
static void SimpleBLEPeripheralObserver_StateChangeCB(gapPeripheralObserverRoleEvent_t *pEvent)
{
	 SimpleBLEPeripheral_enqueueMsg(SBP_OBSERVER_STATE_CHANGE_EVT, SUCCESS);
			 PgapObserverRoletemp=(uint8_t *)pEvent;
}


/*********************************************************************
 * @fn      SimpleBLECentral_addDeviceInfo
 *
 * @brief   Add a device to the device discovery result list
 *
 * @return  none
 */
static void SimpleBLECentral_addDeviceInfo(uint8_t *pAddr, uint8_t addrType,uint8_t rssidata)
{
  uint8_t i;

  // If result count not at max
  if (scanRecount < DEFAULT_MAX_SCAN_RES)
  {
    // Check if device is already in scan results
    for (i = 0; i < scanRecount; i++)
    {
      if (memcmp(pAddr, devList[i].addr , B_ADDR_LEN) == 0)
      {
        return;
      }
    }
    // Add addr to scan result list
    memcpy(devList[scanRecount].addr, pAddr, B_ADDR_LEN);
    devList[scanRecount].addrType = addrType;
    devList[scanRecount].rssi[0]=-rssidata;
    // Increment scan result count
    scanRecount++;
  }
}

static float GetDistance_ByRssi(uint8_t i)
{
	 float  distancetemp=pow(10,(((float)(devList[i].rssi[0]-SignalintensityAtOneMeter))/(10*EnvironmentalAttenuationFactor)));
	 return distancetemp;
}

#endif

/*********************************************************************
*********************************************************************/