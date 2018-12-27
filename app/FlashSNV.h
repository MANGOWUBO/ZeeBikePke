#ifndef __FLASHSNV_H
#define __FLASHSNV_H
#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */



/*********************************************************************
*  EXTERNAL VARIABLES
*/

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * MACROS
 */


/*********************************************************************
 * Variables
 */
/*********************************************************************
 * FUNCTIONS
 */
/**
 * Send notify message Call back definition
 */
typedef void (*flashOperationMessage_t)(uint8_t action);

typedef struct
{
	flashOperationMessage_t        pfnNotifyCb;  // Called when characteristic value changes
} flashOperationMessageCBs_t;

typedef enum
{
	Flash_Protection  = 0 ,
	Flash_READ,
	Flash_WRITE,
}FlashActionState_TypeDef;

extern void FlashProtectionAction(void);

#if ResetMcuAccWork_COMPILE
extern void NotifyFlashAction(const uint8_t action);
#endif

extern  void ReadGuardContextFromFlashWhenInitial(void) ;
extern void FlashProtectWhenLVLow(void);
#if 0
extern void FlashTest(void) ;
#endif
extern bStatus_t FlashOperationMessage_RegisterAppCBs( flashOperationMessageCBs_t *appCallbacks );
extern void NotifyFlashAction(const uint8_t action);
extern void SaveFlashWithResetMCU(void) ;


#if TEST_EXTFLASH_COMPILE
extern void SetFlagCheckExtFlash(void) ;
extern void CheckExtFlashIfFlagSet(void) ;
#endif


/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif
