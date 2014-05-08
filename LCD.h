/*
 * LCD.h
 *
 *  Created on: Sep 13, 2012
 *      Author: LordLaptop
 */

#ifndef LCD_H_
#define LCD_H_

#define LCD_TIMEOUT_COUNT 	 10
#define LCD_TIMEOUT 100

void LCD_Reset(void);
void LCD_Disable();
void InitLCD(void);
void LCD_Clear(void);
void LCD_ScrollR(void);
void LCD_ScrollL(void);
void LCD_Backlight(int value);
void LCD_Underline(int value);
void LCD_Cursor(int x, int y);
void LCD_Blink(int value);
void LCD_MoveCursor(char value);
void LCD_Splash(void);
void LCD_SplashSave(void);
void LCD_Baud(int value);
void scib_fifo_init(void);
unsigned char scib_msg(char * msg);
unsigned char Send_LCD(int a);
void LCD_Startup();

typedef struct LCD
{
	unsigned char init;
	unsigned short timeout_count;
	unsigned long Timestamp;
	Uint16 rdata[8];     // Receive data buffer
}LCD_Status;


#endif /* LCD_H_ */
