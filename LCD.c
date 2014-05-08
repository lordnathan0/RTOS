
#include <xdc/std.h>

#include <xdc/runtime/System.h>

#include <ti/sysbios/BIOS.h>

#define xdc__strict

#include <ti/sysbios/knl/Task.h>

#include <ti/sysbios/knl/Clock.h>

#include <ti/sysbios/knl/Event.h>

#include <ti/sysbios/family/c28/Hwi.h>

#include <ti/sysbios/knl/Semaphore.h>

#include <ti/sysbios/knl/Task.h>

#include <xdc/cfg/global.h>

#include "DSP28x_Project.h"

#include "LCD.h"

#include "bq_pack.h"

#include "main.h"

#include "stdlib.h"

#include "stdio.h"

#include "math.h"

#include "string.h"

#include "ALFAT.h"

#include "eCAN.h"


extern w2000_Status W2000;
extern bq_pack_t bq_pack;
extern Bike_Status Bike;
LCD_Status LCD_S;

// GPIO18 (SCITXDB)
// power 3.3v

/*
 *  ======== taskFxn ========
 */

Void CLK_LCD(UArg a0, UArg a1)
{
	Event_post(EVT_LCD_CLK,Event_Id_00);
}

void TSK_LCD(UArg a0, UArg a1)
{
 //   System_printf("enter taskFxn()\n");
	char msg[16];
	short index = 0;

	while (1)
	{
		Event_pend(EVT_LCD_CLK,Event_Id_00, Event_Id_NONE,BIOS_WAIT_FOREVER);
		if(LCD_S.init == 1)
		{
			LCD_Clear();
			LCD_Cursor(0,0);

			if (Bike.Mode == MODE_CHARGE)
			{
				msg[0] = 'H';
				msg[1] = '\0';
				scib_msg(msg);

				LCD_Cursor(1,0);

				double2string((double)(bq_pack.highest_cell_volts/1000.0), 2.0, msg);
				scib_msg(msg);

				msg[0] = ' ';
				msg[1] = 'L';
				msg[2] = '\0';
				scib_msg(msg);

				double2string((double)(bq_pack.lowest_cell_volts/1000.0), 2.0, msg);
				scib_msg(msg);


				msg[0] = ' ';
				msg[1] = 'T';
				msg[2] = '\0';
				scib_msg(msg);

				unsigned short temp = bq_pack.highest_temp;
				ltoa(temp,msg);
				msg[2] = '\0';
				scib_msg(msg);
			}
			if(Bike.Mode == MODE_RACE)
			{
				ltoa((long)(W2000.Odometer),msg); //mph
				scib_msg(msg);

				scib_msg(":\0");
				ltoa((long)(W2000.PhaseATemp),msg); //degree C
				scib_msg(msg);

				scib_msg(":\0");
				ltoa((long)(W2000.MotorVelocity),msg); //amps
			}


			LCD_Cursor(0,1);
			if(Bike.Fault.all != 0)
			{
				index = 0;
				if(Bike.Fault.bit.precharge != 0 && index < 9)
				{
					msg[0] = 'F';
					msg[1] = ':';
					msg[2] = 'P';
					msg[3] = 'R';
					msg[4] = 'E';
					msg[5] = 'C';
					msg[6] = 'H';
					msg[7] = ' ';
					msg[8] = '\0';
					scib_msg(msg);
					index = index + 8;
				}
				if(Bike.Fault.bit.BMM_Init != 0 && index < 11)
				{
					msg[0] = 'F';
					msg[1] = ':';
					msg[2] = 'B';
					msg[3] = 'M';
					msg[4] = 'M';
					msg[5] = ' ';
					msg[6] = '\0';
					scib_msg(msg);
					index = index + 6;
				}
				if(Bike.Fault.bit.OV != 0 && index < 12)
				{
					msg[0] = 'F';
					msg[1] = ':';
					msg[2] = 'O';
					msg[3] = 'V';
					msg[7] = ' ';
					msg[8] = '\0';
					scib_msg(msg);
					index = index + 5;
				}
				if(Bike.Fault.bit.UV != 0 && index < 12)
				{
					msg[0] = 'F';
					msg[1] = ':';
					msg[2] = 'U';
					msg[3] = 'V';
					msg[7] = ' ';
					msg[8] = '\0';
					scib_msg(msg);
					index = index + 5;
				}
				if(Bike.Fault.bit.estop != 0 && index < 9)
				{
					msg[0] = 'F';
					msg[1] = ':';
					msg[2] = 'E';
					msg[3] = 'S';
					msg[4] = 't';
					msg[5] = 'o';
					msg[6] = 'p';
					msg[7] = ' ';
					msg[8] = '\0';
					//strcpy(msg,"F:EStop ");
					scib_msg(msg);
					index = index + 8;
				}
				if(Bike.Fault.bit.Charge_OC != 0 && index < 10)
				{
					msg[0] = 'F';
					msg[1] = ':';
					msg[2] = 'C';
					msg[3] = 'h';
					msg[4] = 'r';
					msg[5] = 'g';
					msg[6] = ' ';
					msg[7] = '\0';
					scib_msg(msg);
					index = index + 7;
				}
				if(Bike.Fault.bit.Dis_OC != 0 && index < 11)
				{
					msg[0] = 'F';
					msg[1] = ':';
					msg[2] = 'D';
					msg[3] = 'i';
					msg[4] = 's';
					msg[5] = ' ';
					msg[6] = '\0';
					scib_msg(msg);
					index = index + 6;
				}
				if(Bike.Fault.bit.Switch_Error != 0 && index < 9)
				{
					msg[0] = 'F';
					msg[1] = ':';
					msg[2] = 'S';
					msg[3] = '_';
					msg[4] = 'E';
					msg[5] = 'r';
					msg[6] = 'r';
					msg[7] = ' ';
					msg[8] = '\0';
					scib_msg(msg);
					index = index + 8;
				}
			}
			else if(Bike.Warning.all != 0)
			{
				index = 0;
				if(Bike.Warning.bit.BMM_loss != 0 && index < 11)
				{
					msg[0] = 'W';
					msg[1] = ':';
					msg[2] = 'B';
					msg[3] = 'M';
					msg[4] = 'M';
					msg[5] = ' ';
					msg[6] = '\0';
					scib_msg(msg);
					index = index + 6;
				}
				if(Bike.Warning.bit.w2000_off != 0 && index < 9)
				{
					msg[0] = 'W';
					msg[1] = ':';
					msg[2] = 'W';
					msg[3] = '2';
					msg[4] = '0';
					msg[5] = '0';
					msg[6] = '0';
					msg[7] = ' ';
					msg[8] = '\0';
					scib_msg(msg);
					index = index + 8;
				}
				if(Bike.Warning.bit.bus_volt != 0 && index < 9)
				{
					msg[0] = 'W';
					msg[1] = ':';
					msg[2] = 'B';
					msg[3] = 'V';
					msg[4] = 'O';
					msg[5] = 'L';
					msg[6] = 'T';
					msg[7] = ' ';
					msg[8] = '\0';
					scib_msg(msg);
					index = index + 8;
				}
				if(Bike.Warning.bit.current_sense_error != 0 && index < 11)
				{
					msg[0] = 'W';
					msg[1] = ':';
					msg[2] = 'C';
					msg[3] = 'S';
					msg[4] = 'E';
					msg[5] = ' ';
					msg[6] = '\0';
					scib_msg(msg);
					index = index + 6;
				}
				if(Bike.Warning.bit.Charage_OC != 0 && index < 10)
				{
					msg[0] = 'W';
					msg[1] = ':';
					msg[2] = 'C';
					msg[3] = 'h';
					msg[4] = 'r';
					msg[5] = 'g';
					msg[6] = ' ';
					msg[7] = '\0';
					scib_msg(msg);
					index = index + 7;
				}
				if(Bike.Warning.bit.Dis_OC != 0 && index < 11)
				{
					msg[0] = 'W';
					msg[1] = ':';
					msg[2] = 'D';
					msg[3] = 'i';
					msg[4] = 's';
					msg[5] = ' ';
					msg[6] = '\0';
					scib_msg(msg);
					index = index + 6;
				}
				if(Bike.Warning.bit.SD_Startup != 0 && index < 10)
				{
					msg[0] = 'W';
					msg[1] = ':';
					msg[2] = 'S';
					msg[3] = 'D';
					msg[4] = '_';
					msg[5] = 'S';
					msg[6] = ' ';
					msg[7] = '\0';
					scib_msg(msg);
					index = index + 7;
				}
				if(Bike.Warning.bit.SD_Write != 0 && index < 10)
				{
					msg[0] = 'W';
					msg[1] = ':';
					msg[2] = 'S';
					msg[3] = 'D';
					msg[4] = '_';
					msg[5] = 'W';
					msg[6] = ' ';
					msg[7] = '\0';
					scib_msg(msg);
					index = index + 7;
				}
				if(Bike.Warning.bit.Capacity != 0 && index < 11)
				{
					msg[0] = 'W';
					msg[1] = ':';
					msg[2] = 'C';
					msg[3] = 'a';
					msg[4] = 'p';
					msg[5] = ' ';
					msg[6] = '\0';
					scib_msg(msg);
					index = index + 6;
				}
			}
			else
			{
				if(Bike.Mode == MODE_RACE)
				{
					//scib_msg("Vel:\0");
					ltoa((long)(W2000.VehicleVelocity*(2.2)),msg); //mph
					scib_msg(msg);

					scib_msg(":\0");
					ltoa((long)(W2000.MotorTemp),msg); //degree C
					scib_msg(msg);

					scib_msg(":\0");
					ltoa((long)(W2000.BusCurrent),msg); //amps
					scib_msg(msg);
				}
				else if (Bike.Mode == MODE_CHARGE)
				{
					scib_msg("V:\0");
					ltoa((long)W2000.BusVoltage,msg);
					scib_msg(msg);

					scib_msg("C:\0");
					ltoa((long)(W2000.BusCurrent),msg); //amps
					scib_msg(msg);
				}

				/*
				 FOR REFERENCE
				scib_msg("MBV:\0");
				ltoa((long)W2000.BusVoltage,msg);
				scib_msg(msg);

				scib_msg("BV:\0");
				ltoa((long)(bq_pack.voltage/1000),msg);
				scib_msg(msg);

				scib_msg("Vel:\0");
				ltoa((long)(W2000.VehicleVelocity*(2.2)),msg); //mph
				scib_msg(msg);

				scib_msg("MT:\0");
				ltoa((long)(W2000.MotorTemp),msg); //degree C
				scib_msg(msg);

				scib_msg("MC:\0");
				ltoa((long)(W2000.BusCurrent),msg); //amps
				scib_msg(msg);
				*/
			}


			Task_sleep(2);
		}
//    System_printf("exit taskFxn()\n");
	}
}

void LCD_Startup()
{

    InitScibGpio();

    scib_fifo_init();  // Init SCI-B

//    LCD_Reset();
	LCD_Clear();
	LCD_Backlight(30);

	Clock_start(LCD_CLOCK);

    LCD_S.init = 1;



}

void LCD_Disable()
{
	Clock_stop(LCD_CLOCK);
	ScibRegs.SCICTL1.all =0x0000;
	EALLOW;
	GpioCtrlRegs.GPAPUD.bit.GPIO14 = 1;
	GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 0;
	GpioDataRegs.GPACLEAR.bit.GPIO14 = 1;
	EDIS;
    LCD_S.init = 0;

}


void scib_fifo_init()
{
   ScibRegs.SCICCR.all =0x0007;   // 1 stop bit,  No loopback
                                  // No parity,8 char bits,
                                  // async mode, idle-line protocol
   ScibRegs.SCICTL1.all =0x0002;  // enable TX, RX, internal SCICLK,
                                  // Disable RX ERR, SLEEP, TXWAKE
   ScibRegs.SCICTL1.bit.RXENA = 0; //disable receive
   ScibRegs.SCICTL2.bit.TXINTENA =1;
//   ScibRegs.SCICTL2.bit.RXBKINTENA =1;
   ScibRegs.SCIHBAUD    =0x0000;  // 9600 baud @LSPCLK = 37.5MHz.
   ScibRegs.SCILBAUD    =0x00F3;
//   SciaRegs.SCICCR.bit.LOOPBKENA = 1; // Enable loop back
   ScibRegs.SCIFFTX.all=0xC028;
//   ScibRegs.SCIFFRX.all=0x0028;
   ScibRegs.SCIFFCT.all=0x00;

   ScibRegs.SCICTL1.all =0x0023;     // Relinquish SCI from Reset
   ScibRegs.SCIFFTX.bit.TXFIFOXRESET=1;
//   ScibRegs.SCIFFRX.bit.RXFIFORESET=1;


}

unsigned char scib_msg(char * msg)
{
    int i;
    i = 0;
    while(msg[i] != '\0')
    {
        while(Send_LCD(msg[i]) == 0 && LCD_S.timeout_count < LCD_TIMEOUT_COUNT);
        i++;
    }
    if(LCD_S.timeout_count < LCD_TIMEOUT_COUNT)
    {
    	return 0;
    }
    else
    {
    	return 1;
    }
}

unsigned char Send_LCD(int a)
{

    ScibRegs.SCITXBUF=a;
    ScibRegs.SCIFFTX.bit.TXFFINTCLR=1;	// Clear SCI Interrupt flag
    unsigned char ret = Event_pend(EVT_LCD_Tx,Event_Id_00, Event_Id_NONE,LCD_TIMEOUT);
    if(ret == 0)
    {
    	LCD_S.timeout_count++;
    }
    else
    {
    	LCD_S.timeout_count = 0;
    }
	return ret;
}

 void HWI_LCD_TxComplete(UArg a0, UArg a1)
{
    Event_post(EVT_LCD_Tx,Event_Id_00);
}


 /*
  * Clears the screen.
  */
 void LCD_Clear(void)
 {
	 Send_LCD(254);
	 Send_LCD(0x01);
 }

 /*
  * Slides all characters on the screen to right by one segemnt
  */
 void LCD_ScrollR(void)
 {
	 Send_LCD(254);
	 Send_LCD(0x1C);
 }

 /*
  * Slides all characters on the screen to left by one segemnt
  */
 void LCD_ScrollL(void)
 {
	 Send_LCD(254);
	 Send_LCD(0x18);
 }

 /*
  * Changes the brightness of backlight LCD.
  * Value between 0-30. 0=off, 30=%100 on
  */
 void LCD_Backlight(int value)
 {
	 Send_LCD(254);
	 Send_LCD(128+value);
 }

 /*
  * if value is 1, enables line under the character
  * 0 disables it.
  */
 void LCD_Underline(int value)
 {
	 Send_LCD(254);
     if(value==1)
    Send_LCD(0x0E);
 	else
 	Send_LCD(0x0C);
 }

 /*
  * Places cursor on the xth segment (0-15) on line Y(0-1)
  * Y=0 is the top line, Y=1 is the bottom line
  */
 void LCD_Cursor(int x, int y)
 {
	 Send_LCD(254);
	 Send_LCD(((128)+(64*y)+x));
 }

 /*
  * if value is 1, enables box cursor around character
  * 0 disables it.
  */
 void LCD_Blink(int value)
 {
	 Send_LCD(254);
     if(value==1)
    	 Send_LCD(0x0D);
 	else
 		Send_LCD(0x0C);
 }

 /*
  * if value is 'L' or 'l', moves the cursor to the left
  * by one segment, any other value will move the cursor
  * to the right by one segment.
  */
 void LCD_MoveCursor(char value)
 {
	 Send_LCD(254);
     if(value==(('L')||('l')))
    	 Send_LCD(0x10);
 	else
 		 Send_LCD(0x14);
 }

 /*
  * when called, it toggles splash screen. If called when
  * splash screen is enabled, it will be disabled and vice versa.
  */
 void LCD_Splash(void)
 {
	 Send_LCD(254);
	 Send_LCD(0x09);
 }

 /*
  * when called, whatever is currently on the LCD is saved to
  * ROM and will be displayed as the splash screen next time
  * the LCD boots up.
  * **OBSERVED ISSUES**
  * - Reset the LCD after using this command. Once the splash
  * 	 screen is updated, the LCD might not respond to commands
  * 	 until it is reset.
  * - Splash screen will not be displayed if splash screens are
  *   disabled. Try using the LCD_Splash function to enable it.
  * - To be safe, try using spaces to indent your custom splash
  *   screen instead of using LCD_Cursor. I noticed that there
  *   were undesired characters in the custom splash screen.
  */
 void LCD_SplashSave(void)
 {
	 Send_LCD(254);
	 Send_LCD(0x0A);
 }

 void LCD_Reset(void)
 {
	 Send_LCD(254);
	 Send_LCD(0x72);
 }
