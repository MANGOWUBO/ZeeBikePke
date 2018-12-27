/*******************************************************************************
  Filename:       board_lcd.h
  Revised:        $Date: 2014-02-28 14:18:14 -0800 (Fri, 28 Feb 2014) $
  Revision:       $Revision: 37461 $

  Description:    This file contains the SRF06EB Key Service definitions
                  and prototypes.

  Copyright 2014 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, 
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE, 
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com. 
*******************************************************************************/

#ifndef BOARD_LCD_H
#define BOARD_LCD_H

#ifdef __cplusplus
extern "C" {
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
 * TYPEDEFS
 */

/*********************************************************************
 * MACROS
 */
// LCD macros
#ifdef TI_DRIVERS_LCD_INCLUDED

#define LCD_WRITE_STRING(str, line) Board_writeString(str, line)
#define LCD_WRITE_STRING_VALUE(str, value, format, line) \
                               Board_writeStringValue(str, value, format, line)
#else

#define LCD_WRITE_STRING(str, line)
#define LCD_WRITE_STRING_VALUE(str, value, format, line)

#endif // TI_DRIVERS_LCD_INCLUDED

	 
								   
#ifdef TI_DRIVERS_LCD_INCLUDED
/*********************************************************************
 * API FUNCTIONS
 */

/*********************************************************************
 * @fn      Board_openLCD
 *
 * @brief   Open LCD peripheral on SRF06EB.
 *
 * @param   none
 *
 * @return  none
 */
extern void Board_openLCD(void);

/*********************************************************************
 * @fn      Board_writeString
 *
 * @brief   Write a string on the LCD display.
 *
 * @param   str - string to print
 * @param   line - line (page) to write (0-7)
 *
 * @return  void
 */
extern void Board_writeString(char *str, uint8_t line);

/*********************************************************************
 * @fn      Board_writeStringValue
 *
 * @brief   Write a string and value on the LCD display.
 *
 * @param   str - string to print
 * @param   value - value to print
 * @param   format - base of the value to print (2,8,16 etc)
 * @param   line - line (page) to write (0-7)
 *
 * @return  void
 */
extern void Board_writeStringValue(char *str, uint32_t value, uint8_t format,
                                   uint8_t line);

#endif



#define LCD_ManGo_Printf( line,  pText)    HalLcd_HW_WriteLine( line, pText)

#ifdef TI_DRIVERS_LCD_MANGO
extern void Draw_MANGO_Logo();
extern void Board_openLCD(void);
extern void LCD_Fill(unsigned char bmp_dat);
extern void LCD_P6x8Str(unsigned char x, unsigned char y,unsigned char ch[]);
extern void HalLcd_HW_WriteLine(unsigned char line, const char *pText);
extern void LCD_P8x16Str(unsigned char x, unsigned char y,unsigned char ch[]);
extern void LCD_P16x16Ch(unsigned char x, unsigned char y, unsigned char N);
extern void LCD_ManGo_clearLines(unsigned char fromLine,unsigned char toLine);
extern void LCD_ManGo_Printf1(unsigned char y, char ch[],unsigned char num);
extern void LCD_ManGo_Printf2(unsigned char y, char ch[],unsigned char arraycount);
extern uint8_t GetHexToAsciitodisplaylength(uint8_t arraycount);
extern char *HexToAsciitodisplay(uint8_t *hexarray,uint8_t arraycount);
#endif
/*********************************************************************
*********************************************************************/  

#ifdef __cplusplus
}
#endif

#endif /* BOARD_LCD_H */
