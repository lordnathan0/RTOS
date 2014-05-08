/*
 * I2C.c
 *
 *  Created on: Oct 18, 2012
 *      Author: Anil Ozyalcin
 */

#include <xdc/std.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <xdc/runtime/System.h>

#include <ti/sysbios/BIOS.h>

#include <ti/sysbios/knl/Task.h>

#define xdc__strict
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/family/c28/Hwi.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>
#include <xdc/cfg/global.h>
#include "DSP28x_Project.h"

#include "I2C.h"
#include "ext_adc.h"
#include "ALFAT.h"
#include "main.h"
#define ALFAT_SLAVE_ADDR        0x52
#define ADC_SLAVE_ADDR        72

// Ext ADC variables
char count2;
int adc_status;
char adc_current_count;
int adc_current;
int initializable=0;

extern ext_ADC_st ext_ADC_status;
extern int write_done;
// ALFAT variables
int k=0, l=0, m=0;
int counter=0,counter2=0;
int array[75];
int receive_done=1;
int time_in=0,date_in=0;
int time[8];
char time_char[10];
int date[10];
int hour=0,minute=0,second=0;
int day=0,month=0,year=0;
int nonzero=0;
int tor=0;

int Time1 = 0;
int Time2 = 0;
int firstCoul = 0;
int Timediff=0;

int failed_receive=0;
char* global_observe;
char SD_error[3];

int FAIL_TEST=0;

extern Bike_Status Bike;
extern ALFAT_st ALFAT_status;

void I2C_Disable()
{
	ext_ADC_status.Init=0;
	EALLOW;
	I2caRegs.I2CMDR.bit.IRS = 0;
   	GpioCtrlRegs.GPBMUX1.bit.GPIO32 = 0;	// GPIO32 = I2C - SDA
   	GpioCtrlRegs.GPBMUX1.bit.GPIO33 = 0;	// GPIO33 = I2C - SCL
    GpioCtrlRegs.GPBPUD.bit.GPIO32 = 1;    // Enable pull-up for GPIO32 (SDAA)
    GpioCtrlRegs.GPBPUD.bit.GPIO33 = 1;	   // Enable pull-up for GPIO33 (SCLA)
    GpioDataRegs.GPBCLEAR.bit.GPIO32 = 1;
    GpioDataRegs.GPBCLEAR.bit.GPIO33 = 1;
    EDIS;
    //counter = 0;
    //counter2 = 0;
}
void I2C_Startup(void)
{
    EALLOW;

    Time1 = 0;
    Time2 = 0;
    firstCoul = 0;


   	GpioCtrlRegs.GPBMUX1.bit.GPIO32 = 1;	// GPIO32 = I2C - SDA
   	GpioCtrlRegs.GPBMUX1.bit.GPIO33 = 1;	// GPIO33 = I2C - SCL

    GpioCtrlRegs.GPBQSEL1.bit.GPIO32 = 3;  // Asynch input GPIO32 (SDAA)
    GpioCtrlRegs.GPBQSEL1.bit.GPIO33 = 3;  // Asynch input GPIO33 (SCLA)

    GpioCtrlRegs.GPBPUD.bit.GPIO32 = 0;    // Enable pull-up for GPIO32 (SDAA)
    GpioCtrlRegs.GPBPUD.bit.GPIO33 = 0;	   // Enable pull-up for GPIO33 (SCLA)

    EDIS;

	I2caRegs.I2CMDR.bit.IRS = 0;	// Reset the I2C module
	I2caRegs.I2CSAR = 0x52;		// I2C slave address register
	// I2C Prescale Register
	I2caRegs.I2CPSC.all = 14;		// Internal I2C module clock = SYSCLK/(PSC +1)
								    // = 10 MHz
	I2caRegs.I2CCLKL = 13;			// I2C Bus running @ ~250 KHZ
	I2caRegs.I2CCLKH = 13;			// I2C Bus running @ ~250 KHZ

//	I2caRegs.I2CCLKL = 45;
//	I2caRegs.I2CCLKH = 45;			// for I2C 100 kHz:
									// Tmaster = 10 µs *150 MHz / 15 = 100 = (ICCL + ICCH + 10)
									// ICCL + ICCH = 90
									// ICCL = ICH = 90/2 = 45
	I2caRegs.I2CIER.bit.RRDY=1;

	I2caRegs.I2CFFTX.all = 0;			// reset TXFIFO
	I2caRegs.I2CFFTX.bit.TXFFIL  = 0;	// interrupt level = 0 (TXFIFO empty)
	I2caRegs.I2CFFTX.bit.I2CFFEN = 1;	// enable FIFOs
	I2caRegs.I2CFFTX.bit.TXFFRST = 1;	// enable TX - operation


	I2caRegs.I2CMDR.bit.IRS = 1;	// Take I2C out of reset
	//receive_done = 1;
}


void TSK_Received(UArg a0, UArg a1)
{
	while(1)
	{
		Event_pend(EVT_byteRec,Event_Id_00, Event_Id_NONE,BIOS_WAIT_FOREVER);

		if(I2caRegs.I2CSAR == ALFAT_SLAVE_ADDR)
		{
			I2caRegs.I2CSTR.bit.RRDY=1;
			I2caRegs.I2CISRC.all=4;
			counter2++;

			//When each byte is received, following is executed
			array[counter] = I2caRegs.I2CDRR;


			if(array[counter]!=0)
			{
				nonzero++;
				l=0;
			}
			else
			{
				l=1;
			}

			if(l==0)
			{
				counter++;
			}


			if((counter2==I2caRegs.I2CCNT)&&(nonzero<tor))
			{
				while((GpioDataRegs.GPADAT.bit.GPIO10==1)||(I2caRegs.I2CSTR.bit.BB==1))
				{
					//Task_sleep(10);
					Task_sleep(5);
				}
				failed_receive++;

				// THIS IF STATEMENT IS FOR TESTING ONLY!!!! IT TESTS IF ALFAT RESETS PROPERLY WHEN FLUSH ERROR OCCURS.
				if(FAIL_TEST==1)
				{
					FAIL_TEST=0;
					failed_receive=200;
					//failed_receive=150;
				}

				if((global_observe[0]=='F')&&(global_observe[1]==' '))
				{
					if(failed_receive<25)
					{
						counter2=0;
						l=0;
						Task_sleep(500);
						//while(I2caRegs.I2CSTR.bit.SCD==1)
						//{
							//Task_sleep(3);
						//}

						I2caRegs.I2CSTR.bit.SCD = 1;
						I2caRegs.I2CSTR.bit.ARDY = 1;
						I2caRegs.I2CCNT = 10;
						I2caRegs.I2CMDR.all = 0x6C20;
					}
					else
					{
						failed_receive=0;
						Clock_stop(ALFAT_CLOCK);
						Clock_stop(EXT_ADC_CLOCK);
						//ALFAT_status.error = 2;
						ALFAT_status.Init = 0;
						//Bike.Warning.bit.SD_Write=1;
						array[0]=0x21;
						array[1]=69;
						array[2]=69;
						receive_done=1;
						//Event_post(EVT_Error,Event_Id_00);
					}
				}
				else
				{
					counter2=0;
					l=0;
					Task_sleep(500);
					//while(I2caRegs.I2CSTR.bit.SCD==1)
					//{
						//Task_sleep(3);
					//}

					I2caRegs.I2CSTR.bit.SCD = 1;
					I2caRegs.I2CSTR.bit.ARDY = 1;
					I2caRegs.I2CCNT = 10;
					I2caRegs.I2CMDR.all = 0x6C20;
				}
			}

			if((time_in==1)&&(nonzero==13))
			{
				for(k=0;k<8;k++)
				{
					time[k]=((int)array[k]-48);
					time_char[k]=(char)array[k];
				}
				time_char[8]=',';
				time_char[9]=0;
				time_in=0;

				second=time[7]+(10*time[6]);
				minute=time[4]+(10*time[3]);
				hour=time[1]+(10*time[0]);
			}

			if((date_in==1)&&(nonzero==13))
			{
				for(m=0;m<10;m++)
				{
					date[m]=((int)array[m]-48);
				}
				date_in=0;

				year=date[9]+(10*date[8])+(100*date[7])+(1000*date[6]);
				day=date[4]+(10*date[3]);
				month=date[1]+(10*date[0]);
			}
			if((nonzero>=tor)&&(counter2==I2caRegs.I2CCNT))
			{
				//Task_sleep(10);
				failed_receive=0;
				Task_sleep(5);
				receive_done=1;
			}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//			if((counter2==I2caRegs.I2CCNT)&&(nonzero<tor))
//			{
//				while((GpioDataRegs.GPADAT.bit.GPIO10==1)||(I2caRegs.I2CSTR.bit.BB==1))
//				{
//					//Task_sleep(10);
//					Task_sleep(5);
//				}
//				counter2=0;
//				l=0;
//				/////////////////////////////////////////// TRY GETTING RID OF THIS TASK_SLEEP
//				Task_sleep(10);
//
//				I2caRegs.I2CSTR.bit.SCD = 1;
//				I2caRegs.I2CSTR.bit.ARDY = 1;
//				I2caRegs.I2CCNT = 20; ////////////////////////////TRY TO DECREASE THIS TO 20 TO SPEED UP SYSTEM!!!!!!!!
//				I2caRegs.I2CMDR.all = 0x6C20;
//				//receive_done=0;
//			}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		}
		else if(I2caRegs.I2CSAR == ADC_SLAVE_ADDR)
		{
			Time2 = Clock_getTicks();//100 usecs
			if (adc_current_count == 0)
			{
				adc_current = I2caRegs.I2CDRR << 8;
				adc_current_count = 1;
			}
			else if(adc_current_count == 1)
			{
				//todo coulomb counting and warning checking
				adc_current = adc_current + I2caRegs.I2CDRR;
				//ext_ADC_status.current=(double)((adc_current/65.536)+.42);
				//ext_ADC_status.current=adc_current;
				ext_ADC_status.current=(double)((adc_current+31.0)/(32.744)); // THIS IS THE ACTUAL ADC CALIBRATION EQUATION!!!
				adc_current_count = 2;

				//coulomb counting
				if(firstCoul == 0)
				{
					Time1 = Time2;
					firstCoul = 1;
				}
				else if(ext_ADC_status.current < 1 && ext_ADC_status.current > -1)
				{
					Timediff = Time2 - Time1;
					Time1 = Time2;
				}
				else
				{
					Timediff = Time2 - Time1;
					Time1 = Time2;
					ext_ADC_status.coul = ext_ADC_status.coul + ext_ADC_status.current*((float)Timediff/1000); //amp * sec
				}
			}
			else
			{
				count2++;
				adc_current_count = 0;
				adc_status = I2caRegs.I2CDRR;
				ext_ADC_status.status=adc_status;

				if(ext_ADC_status.Init==0)
				{
					if(ext_ADC_status.status==0x8C)
					{
						initializable=1;
					}
				}

				if(ext_ADC_status.status!=0x8C)
				{
					Bike.Warning.bit.current_sense_error = 0;
					Clock_stop(EXT_ADC_CLOCK);
				}
/////////////////////////////////////////////////////////////////////////// TRY GETTING RID OF THIS TSK_SLEEP
				Ext_Adc_Error_Check();
				Task_sleep(5);
				receive_done=1;

				//Event_post(EVT_Ext_ADC,Event_Id_01);
			}
		}

	}
}

void HWI_I2C(UArg a0, UArg a1)
{
	if(I2caRegs.I2CISRC.bit.INTCODE==4)
	{
		I2caRegs.I2CSTR.bit.RRDY=1;
		Event_post(EVT_byteRec,Event_Id_00);
	}
}



int I2C_print(char *string, int length,int requested, int I2C_Address, int linefeed)
{
	///////////////////////////////////////////////// GLOBAL_OBSERVE IS TEMPORARY, GET RID OF IT!!!
	global_observe=string;
	while((GpioDataRegs.GPADAT.bit.GPIO10==1)||(receive_done==0))
	{
		//Task_sleep(10);
		Task_sleep(5);
	}

	receive_done=0;
	tor=requested;
	nonzero=0;
	l=0;
	I2caRegs.I2CSAR = I2C_Address;
	I2caRegs.I2CISRC.all=4;

	counter=0;
	counter2=0;

	if(length == 0)
	{
		if((strlen(string))>16)
		{
			int r=((strlen(string))/16);
			int d1=0,d2=0,d3=0,d4=0;

			for(d1=0;d1<r;d1++)
			{
				I2caRegs.I2CCNT=16;
				for(d2=0;d2<16;d2++)
				{
					I2caRegs.I2CDXR=(*string++);
				}
				I2caRegs.I2CSTR.bit.SCD = 1;
			    I2caRegs.I2CSTR.bit.ARDY = 1;
			    I2caRegs.I2CMDR.all = 0x6620;

			    while((I2caRegs.I2CSTR.bit.ARDY == 0))
			    {
			    	//Task_sleep(10);
			    	Task_sleep(5);
			    }
			}

			d4=((strlen(string))%16);
			if(d4!=0)
			{
				I2caRegs.I2CCNT=d4;
				for(d3=0;d3<d4;d3++)
				{
					I2caRegs.I2CDXR=(*string++);
				}
				I2caRegs.I2CSTR.bit.SCD = 1;
		    	I2caRegs.I2CSTR.bit.ARDY = 1;
		    	I2caRegs.I2CMDR.all = 0x6620;
			}

		}
		else
		{
			I2caRegs.I2CCNT = strlen(string);
			while (*string)
			{
				I2caRegs.I2CDXR=(*string++);
			}
			I2caRegs.I2CSTR.bit.SCD = 1;
		    I2caRegs.I2CSTR.bit.ARDY = 1;
		    I2caRegs.I2CMDR.all = 0x6620;
		}

	}
	else
	{
		//int f=0;

		if((strlen(string))>16)
		{
			int r=((strlen(string))/16);
			int d1=0,d2=0,d3=0,d4=0;

			for(d1=0;d1<r;d1++)
			{
				I2caRegs.I2CCNT=16;
				for(d2=0;d2<16;d2++)
				{
					I2caRegs.I2CDXR=(*string++);
				}
				I2caRegs.I2CSTR.bit.SCD = 1;
			    I2caRegs.I2CSTR.bit.ARDY = 1;
			    I2caRegs.I2CMDR.all = 0x6620;

			    while((I2caRegs.I2CSTR.bit.ARDY == 0))
			    {
			    	//Task_sleep(10);
			    	Task_sleep(5);
			    }
			}

			d4=((strlen(string))%16);
			if(d4!=0)
			{
				I2caRegs.I2CCNT=d4;
				for(d3=0;d3<d4;d3++)
				{
					I2caRegs.I2CDXR=(*string++);
				}
				I2caRegs.I2CSTR.bit.SCD = 1;
		    	I2caRegs.I2CSTR.bit.ARDY = 1;
		    	I2caRegs.I2CMDR.all = 0x6620;
			}

		}
		else
		{
			I2caRegs.I2CCNT = strlen(string);
			while (*string)
			{
				I2caRegs.I2CDXR=(*string++);
			}
			I2caRegs.I2CSTR.bit.SCD = 1;
		    I2caRegs.I2CSTR.bit.ARDY = 1;
		    I2caRegs.I2CMDR.all = 0x6620;
		}

	}

    while((I2caRegs.I2CSTR.bit.ARDY == 0))
    {
    	//Task_sleep(10);
    	Task_sleep(5);
    }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if(linefeed==0)
	{
		I2caRegs.I2CCNT=1;
		I2caRegs.I2CDXR = 0x0D;
		I2caRegs.I2CSTR.bit.SCD = 1;
		I2caRegs.I2CSTR.bit.ARDY = 1;
		I2caRegs.I2CMDR.all = 0x6620;
	}


    while((I2caRegs.I2CSTR.bit.ARDY == 0))
    {
    	//Task_sleep(10);
    	Task_sleep(5);
    }

	while((GpioDataRegs.GPADAT.bit.GPIO10==1))
	{
		//Task_sleep(10);
		Task_sleep(5);
	}

	if((strlen(global_observe)==3))
	{
		if((global_observe[0]=='F')&&(global_observe[1]==' '))
		{
			Task_sleep(600);
		}
	}
	I2caRegs.I2CCNT = 20; //////////////////////////// TRY DECREASING THIS TO 20 TO SPEED UP SYSTEM!!!!!!!!
	I2caRegs.I2CSTR.bit.ARDY = 1;
	I2caRegs.I2CSTR.bit.SCD = 1;
	I2caRegs.I2CMDR.all = 0x6C20;

//	GpioDataRegs.GPATOGGLE.bit.GPIO9 = 1;

	//Task_sleep(500);
	//Task_sleep(500);

	while((GpioDataRegs.GPADAT.bit.GPIO10==1)||(receive_done==0))
	{
		//Task_sleep(10);
		Task_sleep(5);
	}

	if((array[0]==0x21)&&(array[1]==0x30)&&(array[2]==0x30))
	{
		return 0;
	}
	else
	{
		SD_error[0]=array[0];
		SD_error[1]=array[1];
		SD_error[2]=array[2];
		return 1;
	}
}
