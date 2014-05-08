/*
 * ALFAT.c
 *
 *  Created on: Oct 16, 2012
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
#include "eCAN.h"

#include "ALFAT.h"
#include "math.h"
#include "bq_pack.h"
#include "I2C.h"
#include "data_flash.h"
#include "main.h"
#include "ext_adc.h"

int flush_error_count=0;
int flush_error=0;
int file_open_error=0;
int write_done=1;
int logging_done=1;
int sleep_done=1;
char out_com1[6];
char out_com2[7];
char out_com3[8];
char WILLGO[20];

char length_str1[1];
char length_str2[2];
char length_str3[3];

char global_file[40];
char hand_hex;

extern int time_in;
extern int date_in;
extern int receive_done;
extern int array[];
extern int tor;
extern char time_char[10];
char hour_char[2];
char minute_char[2];
char second_char[2];
char MergedData[4095];

extern unsigned long CAN_cache[W2000_LOG_RATE];
extern unsigned int cache_counter;

// THIS PART IS TO TEST RTC SET TIME!!!
int set_time=0;
char dateyear[]="13/03/13";
char timehour[]="16:28:00";
//

extern ext_ADC_st ext_ADC_status;
extern int hour;
extern int minute;
extern int second;
extern int year;
extern int month;
extern int day;
extern int counter, counter2,l,nonzero;



extern bq_pack_t bq_pack;
extern w2000_Status W2000;
ALFAT_st ALFAT_status;
extern Bike_Status Bike;


void ALFAT_Disable()
{
	Clock_stop(ALFAT_CLOCK);
	Task_sleep(3000);
	while(sleep_done==0)
	{
		Task_sleep(500);
	}
	ALFAT_status.Init = 0;
	ALFAT_status.error = 0;
	EALLOW;
	GpioCtrlRegs.GPBPUD.bit.GPIO60 = 1;
	GpioCtrlRegs.GPBDIR.bit.GPIO60 = 1;
	GpioDataRegs.GPBCLEAR.bit.GPIO60 = 0;
	EDIS;
}
void ALFAT_Startup()
{
	/*
	 * ------------------------------------
	 * STARTUP I2C BEFORE STARTING UP ALFAT
	 * ------------------------------------
	 */

	if(ext_ADC_status.Init==1)
	{
		Clock_stop(EXT_ADC_CLOCK);
	}

	//------ ALFTA RESET PIN --------
	sleep_done=0;	// THIS IS TEMPORARY!!!!!!!!!!!!!!!!!!!!!!!!!!

	while(logging_done==0)
	{
		Task_sleep(5);
	}
	logging_done=0;

	int welcome = 1;
	write_done=1;  //<======================== THERE NEEDS TO BE A WAIT LOOP HERE TO MAKE SURE ADC ISNT DOING ANYTHING WHILE ALFAT IS INITIALIZING!!!
	if (ALFAT_status.Init != 1)
	{
		EALLOW;
		GpioCtrlRegs.GPBPUD.bit.GPIO60 = 0;
		GpioCtrlRegs.GPBDIR.bit.GPIO60 = 1;
		GpioDataRegs.GPBCLEAR.bit.GPIO60 = 1;

		EDIS;

		Task_sleep(50);

		EALLOW;
		GpioCtrlRegs.GPBPUD.bit.GPIO60 = 0;
		GpioDataRegs.GPBSET.bit.GPIO60 = 1;
		GpioCtrlRegs.GPBDIR.bit.GPIO60 = 1;

		EDIS;
		Task_sleep(500);

		ALFAT_status.error = 0;

		while((GpioDataRegs.GPADAT.bit.GPIO10==1)||(receive_done==0))
		{
			Task_sleep(5);
		}

		//command to receive intro message
		receive_done = 0;
		array[7] = 0;
		array[14]= 0;
		array[23]= 0;
		array[50]= 0;
		array[60]= 0;
		array[66]= 0;

		counter=0;
		counter2=0;
		nonzero=0;
		l=0;

		tor = 73;
		I2caRegs.I2CSAR = 0x52;
		I2caRegs.I2CSTR.bit.ARDY = 1;
		I2caRegs.I2CSTR.bit.XRDY = 1;
		I2caRegs.I2CCNT = 73;
		I2caRegs.I2CMDR.all = 0x6C20;

		Task_sleep(5000);

		int i=0;
		welcome = 0;

	   	for(i=0;i<5;i++)
	   	{
	   		Task_sleep(500);
	   		if((array[7]==0x006C)&&(array[14]==0x0069)&&(array[23]==0x000A)&&(array[50]==0x0046)&&(array[60]==0x006F)&&(array[66]==0x0072))
	   		{
				welcome=1;
				int temp_error2=0;
				temp_error2=I2C_print("T B",0,4,0x52,0);
				if(temp_error2==0)
				Get_Time();
				Clock_start(TIMESTAMP_CLOCK);
	   			break;
	   		}
	   		else
	   		{
	   			if(i==4)
	   			{
	   				welcome=0;
	   				logging_done=1;
	   				ALFAT_status.Init = 0;
	   				ALFAT_status.error = 1;
	   				sleep_done=1;
	   				Bike.Warning.bit.SD_Startup=1;
	   				Event_post(EVT_Error,Event_Id_00);
	   			}
	   		}
	   	}
	}
	receive_done=1;	// NOT SURE IF THIS SHOULD BE HERE!!!!!!!!!!!!!!!!!!!!!!!!
   	if(welcome==1)
   	{
		// MOUNT FILE SYSTEM
   		int temp_error1=0;
   		// Uncomment and put a breakpoint to find out ALFAT FW Version
   		//I2C_print("V",0,11,0x52,0);

   		if (ALFAT_status.Init == 0)
   		{
			temp_error1=I2C_print("I M:",0,4,0x52,0);

			if(set_time==1)
			Set_RTC_Time(dateyear, timehour);
   		}
		if(temp_error1==0)
		{
			I2C_print("O 1A>M:\\Events.CSV\0",0,4,0x52,0);
			I2C_print("O 2A>M:\\W2000_Status.CSV\0",0,4,0x52,0);
			if(Bike.Mode==0)	//If in Race Mode
			{
				char file_name[40];
				char temp1[3];
				char temp2[4];
				char zero[1];

				strcpy(zero, "0");
				strcpy(file_name, "O 0A>M:\\Race_");
				ltoa(year,temp2);
				if(year < 10)
				{
					file_name[13]= zero[0];
					file_name[14]= zero[0];
					file_name[15]= zero[0];
					file_name[16]= temp2[0];

				}
				else if(year < 100)
				{

					file_name[13]= zero[0];
					file_name[14]= zero[0];
					file_name[15]= temp2[0];
					file_name[16]= temp2[1];
				}
				else if(year < 1000)
				{
					file_name[13]= zero[0];
					file_name[14]=temp2[0];
					file_name[15]=temp2[1];
					file_name[16]=temp2[2];
				}
				else
				{
					file_name[13]= temp2[0];
					file_name[14]= temp2[1];
					file_name[15]= temp2[2];
					file_name[16]= temp2[3];
				}

				file_name[17]='-';

				ltoa(month,temp1);

				if(month < 10)
				{
					file_name[19]=temp1[0];
					file_name[18]=zero[0];
				}
				else
				{
					file_name[18]=temp1[0];
					file_name[19]=temp1[1];
				}
				file_name[20]='-';

				ltoa(day,temp1);

				if(day < 10)
				{
					file_name[22]=temp1[0];
					file_name[21]=zero[0];
				}
				else
				{
					file_name[21]=temp1[0];
					file_name[22]=temp1[1];
				}
				file_name[23]='_';

				ltoa(hour,temp1);

				if(hour < 10)
				{
					file_name[25]=temp1[0];
					file_name[24]=zero[0];
				}
				else
				{
					file_name[24]=temp1[0];
					file_name[25]=temp1[1];
				}
				file_name[26]='-';

				ltoa(minute,temp1);

				if(minute < 10)
				{
					file_name[28]=temp1[0];
					file_name[27]=zero[0];
				}
				else
				{
					file_name[27]=temp1[0];
					file_name[28]=temp1[1];
				}
				file_name[29]='-';

				ltoa(second,temp1);
				if(second < 10)
				{
					file_name[31]=temp1[0];
					file_name[30]=zero[0];
				}
				else
				{
					file_name[30]=temp1[0];
					file_name[31]=temp1[1];
				}
				file_name[32]='.';
				file_name[33]='c';
				file_name[34]='s';
				file_name[35]='v';
				file_name[36]='\0';

				strcpy(global_file,file_name);

				file_open_error=I2C_print(file_name,0,4,0x52,0);

				if(file_open_error==0)
				{
					strcpy(MergedData,"Time,");
					Battery_Voltage_Headers();
					char header_1[15];
					char header_2[15];
					strcpy(header_1,"BQ 0 Temp 1,");
					strcpy(header_2,"BQ 0 Temp 2,");
					int bq_number;
					for(bq_number=0;bq_number<10;bq_number++)
					{
						strcat(MergedData,header_1);
						strcat(MergedData,header_2);
						header_1[3]++;
						header_2[3]++;
					}

					strcpy(header_1,"BQ 10 Temp 1,");
					strcpy(header_2,"BQ 10 Temp 2,");

					for(bq_number=10;bq_number<NUMBER_OF_BQ_DEVICES;bq_number++)
					{
						strcat(MergedData,header_1);
						strcat(MergedData,header_2);

						if(header_1[4]=='9')
						{
							header_1[3]++;
							header_1[4]='0';
						}
						else
						{
							header_1[4]++;
						}
						if(header_2[4]=='9')
						{
							header_2[3]++;
							header_2[4]='0';
						}
						else
						{
							header_2[4]++;
						}

					}

					strcat(MergedData,"Motor Vel Cmd,");
					strcat(MergedData,"Motor Cur Cmd,");
					strcat(MergedData,"Bus Cur Cmd,");
					strcat(MergedData,"Bus Current,");
					strcat(MergedData,"Bus Voltage,");
					strcat(MergedData,"Motor Velocity,");
					strcat(MergedData,"Motor Temperature,");
					strcat(MergedData,"Vehicle Velocity,");
					strcat(MergedData,"Phase A Temp,");
					strcat(MergedData,"Bus Amp Hour,");
					strcat(MergedData,"Odometer,");
					strcat(MergedData,"Phase B Cur,");
					strcat(MergedData,"Phase C Cur,");
					strcat(MergedData,"Motor Vd,");
					strcat(MergedData,"Motor Vq,");
					strcat(MergedData,"Motor Id,");
					strcat(MergedData,"Motor Iq,");
					strcat(MergedData,"Ext ADC Current,");
					strcat(MergedData,"Pack Voltage,");
					strcat(MergedData,"Highest Voltage,");
					strcat(MergedData,"Lowest Voltage,");
					strcat(MergedData,"Highest Pack Temp,");

					strcat(MergedData,"OV Warning,");
					strcat(MergedData,"UV Warning,");
					strcat(MergedData,"OT Warning,");
					strcat(MergedData,"Discharge OC Warning,");
					strcat(MergedData,"Charge OC Warning,");
					strcat(MergedData,"Capacity Warning,");
					strcat(MergedData,"BMM Loss Warning,");
					strcat(MergedData,"Low Voltage Warning,");
					strcat(MergedData,"W2000 Off Warning,");
					strcat(MergedData,"Cur Sense Warning,");

					strcat(MergedData,"OV Fault,");
					strcat(MergedData,"UV Fault,");
					strcat(MergedData,"OT Fault,");
					strcat(MergedData,"Discharge OC Fault,");
					strcat(MergedData,"Charge OC Fault,");
					strcat(MergedData,"Motor Error Fault,");
					strcat(MergedData,"BMM Init Fault,");
					strcat(MergedData,"Switch_Error F,");
					strcat(MergedData,"Low 12V Bat F,");
					strcat(MergedData,"ESTOP Fault,");

					strcat(MergedData,"\n");
					Write_SD(MergedData,0);

					Flush_SD(0);
					ALFAT_status.Init = 1;
					logging_done = 1;
					Clock_start(ALFAT_CLOCK);
					Event_pend(EVT_Alfat_DONE,Event_Id_00, Event_Id_NONE,BIOS_WAIT_FOREVER);

				}
				else
				{
					//write_done=1;
					//logging_done=1;
	   				ALFAT_status.Init = 0;
	   				ALFAT_status.error = 1;
	   				sleep_done=1;
	   				Bike.Warning.bit.SD_Startup=1;
	   				Event_post(EVT_Error,Event_Id_00);
				}
			}
			else if(Bike.Mode==1)	//If in Charge Mode
			{
				char file_name[40];
				char temp1[3];
				char temp2[4];
				char zero[1];

				strcpy(zero, "0");

				strcpy(file_name, "O 0A>M:\\Chrg_");
				ltoa(year,temp2);
				if(year < 10)
				{
					file_name[13]= zero[0];
					file_name[14]= zero[0];
					file_name[15]= zero[0];
					file_name[16]= temp2[0];
				}
				else if(year < 100)
				{

					file_name[13]= zero[0];
					file_name[14]= zero[0];
					file_name[15]= temp2[0];
					file_name[16]= temp2[1];
				}
				else if(year < 1000)
				{
					file_name[13]= zero[0];
					file_name[14]=temp2[0];
					file_name[15]=temp2[1];
					file_name[16]=temp2[2];
				}
				else
				{
					file_name[13]= temp2[0];
					file_name[14]= temp2[1];
					file_name[15]= temp2[2];
					file_name[16]= temp2[3];
				}

				file_name[17]='-';

				ltoa(month,temp1);

				if(month < 10)
				{
					file_name[19]=temp1[0];
					file_name[18]=zero[0];
				}
				else
				{
					file_name[18]=temp1[0];
					file_name[19]=temp1[1];
				}
				file_name[20]='-';

				ltoa(day,temp1);

				if(day < 10)
				{
					file_name[22]=temp1[0];
					file_name[21]=zero[0];
				}
				else
				{
					file_name[21]=temp1[0];
					file_name[22]=temp1[1];
				}
				file_name[23]='_';

				ltoa(hour,temp1);

				if(hour < 10)
				{
					file_name[25]=temp1[0];
					file_name[24]=zero[0];
				}
				else
				{
					file_name[24]=temp1[0];
					file_name[25]=temp1[1];
				}
				file_name[26]='-';

				ltoa(minute,temp1);

				if(minute < 10)
				{
					file_name[28]=temp1[0];
					file_name[27]=zero[0];
				}
				else
				{
					file_name[27]=temp1[0];
					file_name[28]=temp1[1];
				}
				file_name[29]='-';

				ltoa(second,temp1);
				if(second < 10)
				{
					file_name[31]=temp1[0];
					file_name[30]=zero[0];
				}
				else
				{
					file_name[30]=temp1[0];
					file_name[31]=temp1[1];
				}
				file_name[32]='.';
				file_name[33]='c';
				file_name[34]='s';
				file_name[35]='v';
				file_name[36]='\0';

				strcpy(global_file,file_name);

				file_open_error=I2C_print(file_name,0,4,0x52,0);

				if(file_open_error==0)
				{
					strcpy(MergedData,"Time,");
					Battery_Voltage_Headers();

					char header_1[15];
					char header_2[15];
					strcpy(header_1,"BQ 0 Temp 1,");
					strcpy(header_2,"BQ 0 Temp 2,");
					int bq_number;
					for(bq_number=0;bq_number<10;bq_number++)
					{
						strcat(MergedData,header_1);
						strcat(MergedData,header_2);
						header_1[3]++;
						header_2[3]++;
					}

					strcpy(header_1,"BQ 10 Temp 1,");
					strcpy(header_2,"BQ 10 Temp 2,");

					for(bq_number=10;bq_number<NUMBER_OF_BQ_DEVICES;bq_number++)
					{
						strcat(MergedData,header_1);
						strcat(MergedData,header_2);

						if(header_1[4]=='9')
						{
							header_1[3]++;
							header_1[4]='0';
						}
						else
						{
							header_1[4]++;
						}
						if(header_2[4]=='9')
						{
							header_2[3]++;
							header_2[4]='0';
						}
						else
						{
							header_2[4]++;
						}

					}


					strcat(MergedData,"Ext ADC Current,");
					strcat(MergedData,"Pack Voltage,");
					strcat(MergedData,"Highest Voltage,");
					strcat(MergedData,"Lowest Voltage,");
					strcat(MergedData,"Highest Pack Temp,");

					strcat(MergedData,"OV Warning,");
					strcat(MergedData,"UV Warning,");
					strcat(MergedData,"OT Warning,");
					strcat(MergedData,"Discharge OC Warning,");
					strcat(MergedData,"Charge OC Warning,");
					strcat(MergedData,"Capacity Warning,");
					strcat(MergedData,"BMM Loss Warning,");
					strcat(MergedData,"Low Voltage Warning,");
					strcat(MergedData,"W2000 Off Warning,");
					strcat(MergedData,"Cur Sense Warning,");

					strcat(MergedData,"OV Fault,");
					strcat(MergedData,"UV Fault,");
					strcat(MergedData,"OT Fault,");
					strcat(MergedData,"Discharge OC Fault,");
					strcat(MergedData,"Charge OC Fault,");
					strcat(MergedData,"Motor Error Fault,");
					strcat(MergedData,"BMM Init Fault,");
					strcat(MergedData,"Switch_Error F,");
					strcat(MergedData,"Low 12V Bat F,");
					strcat(MergedData,"ESTOP Fault,");


					strcat(MergedData,"\n");
					Write_SD(MergedData,0);

					Flush_SD(0);
					ALFAT_status.Init = 1;
					logging_done = 1;
					Clock_start(ALFAT_CLOCK);
					Event_pend(EVT_Alfat_DONE,Event_Id_00, Event_Id_NONE,BIOS_WAIT_FOREVER);
				}


			}
			else if(Bike.Mode==2)	//If in Sleep Mode
			{
				//TRY THE FOLLOWING COMMAND!!!
				//I2C_print("? M:\\Sleep.csv",0,4,0x52,0);

				I2C_print("O 0A>M:\\Sleep.csv",0,4,0x52,0);
				sleep_done=0;
				cache_counter=0;
			   	//Task_sleep(1000);
				//logging_done=1;
			   	ALFAT_status.Init = 1;
				// For one-time logging
			   	logging_done=1;
				Event_post(EVT_Alfat_CLK,Event_Id_00);

				// EVENT POST TO MAKE SURE IT IS DONE WRITING TO SD
				Event_pend(EVT_Alfat_DONE,Event_Id_00, Event_Id_NONE,BIOS_WAIT_FOREVER);
			}

		}
		else
		{
			ALFAT_status.error = 3;
			Bike.Warning.bit.SD_Startup=1;
			ALFAT_status.Init = 0;
			Event_post(EVT_Error,Event_Id_00);
			sleep_done=1;
		}
   	}
   	else
   	{
   		ALFAT_status.error = 0;
   		Bike.Warning.bit.SD_Startup=1;
   		ALFAT_status.Init = 0;
   		Event_post(EVT_Error,Event_Id_00);
   		sleep_done=1;

   	}

   	logging_done=1;

   	if(ext_ADC_status.Init==1)
	{
		Clock_start(EXT_ADC_CLOCK);
	}

}


Void CLK_Alfat(UArg a0, UArg a1)
{
	Event_post(EVT_Alfat_CLK,Event_Id_00);
}

void TSK_Alfat(UArg a0, UArg a1)
{
	while (1)
	{
		Event_pend(EVT_Alfat_CLK,Event_Id_00, Event_Id_NONE,BIOS_WAIT_FOREVER);
		if(ALFAT_status.Init==1)
		{
			while(logging_done==0)
			{
				Task_sleep(5);
			}
			logging_done=0;
			//Write_SD("Does it work?  \n",0);

//			Write_SD(time_char,0);
//			double2string(ext_ADC_status.current,2.0,WILLGO);
//			Write_SD(WILLGO,0);
//			~12 KB/sec write test... successful
//			Write_SD("A written language is the representation of a language by means of a writing system. Written language is an invention in that it must be taught to children; children will pick up spoken language (oral or sign) by exposure without being specifically 123456789A written language is the representation of a language by means of a writing system. Written language is an invention in that it must be taught to children; children will pick up spoken language (oral or sign) by exposure without being specifically 123456789A written language is the representation of a language by means of a writing system. Written language is an invention in that it must be taught to children; children will pick up spoken language (oral or sign) by exposure without being specifically 123456789A written language is the representation of a language by means of a writing system. Written language is an invention in that it must be taught to children; children will pick up spoken language (oral or sign) by exposure without being specifically 123456789A written language is the representation of a language by means of a writing system. Written language is an invention in that it must be taught to children; children will pick up spoken language (oral or sign) by exposure without being specifically 123456789A written language is the representation of a language by means of a writing system. Written language is an invention in that it must be taught to children; children will pick up spoken language (oral or sign) by exposure without being specifically 123456789A written language is the representation of a language by means of a writing system. Written language is an invention in that it must be taught to children; children will pick up spoken language (oral or sign) by exposure without being specifically 123456789A written language is the representation of a language by means of a writing system. Written language is an invention in that it must be taught to children; children will pick up spoken language (oral or sign) by exposure without being specifically 123456789guage by means of a writing system. Written language is an invention in that it must be taught to children; children will pick up spoken language (oral or sign) by exposure without being specifically 123456789A written language is the representation of a language by means of a writing system. Written language is an invention in that it must be taught to children; children will pick up spoken language (oral or sign) by exposure without being specifically 123456789spoken language (oral or systA written language is the representation of a language by means of a writing system. Written language is an invention in that it must be taught to children; children will pick up spoken language (oral or sign) by exposure without being specifically 123456789A written language is the representation of a language by means of a writing system. Written language is an invention in that it must be taught to children; children will pick up spoken language (oral or sign) by exposure without being specifically 123456789A written language is the representation of a language by means of a writing system. Written language is an invention in that it must be taught to children; children will pick up spoken language (oral or sign) by exposure without being specifically 123456789A written language is the representation of a language by means of a writing system. Written language is an invention in that it must be taught to children; children will pick up spoken language (oral or sign) by exposure without being specifically 123456789A written language is the representation of a language by means of a writing system. Written language is an invention in that it must be taught to children; children will pick up spoken language (oral or sign) by exposure without being specifically 123456789A written language is the representation of a language by means of a writing system. Written language is an invention in that it must be tau\n",0);
//			Write_SD("A written language is the representation of a language by means of a writing system. Written language is an invention in that it must be taught to children; children will pick up spoken language (oral or sign) by exposure without being specifically 123456789A written language is the representation of a language by means of a writing system. Written language is an invention in that it must be taught to children; children will pick up spoken language (oral or sign) by exposure without being specifically 123456789A written language is the representation of a language by means of a writing system. Written language is an invention in that it must be taught to children; children will pick up spoken language (oral or sign) by exposure without being specifically 123456789A written language is the representation of a language by means of a writing system. Written language is an invention in that it must be taught to children; children will pick up spoken language (oral or sign) by exposure without being specifically 123456789A written language is the representation of a language by means of a writing system. Written language is an invention in that it must be taught to children; children will pick up spoken language (oral or sign) by exposure without being specifically 123456789A written language is the representation of a language by means of a writing system. Written language is an invention in that it must be taught to children; children will pick up spoken language (oral or sign) by exposure without being specifically 123456789A written language is the representation of a language by means of a writing system. Written language is an invention in that it must be taught to children; children will pick up spoken language (oral or sign) by exposure without being specifically 123456789A written language is the representation of a language by means of a writing system. Written language is an invention in that it must be taught to children; children will pick up spoken language (oral or sign) by exposure without being specifically 123456789guage by means of a writing system. Written language is an invention in that it must be taught to children; children will pick up spoken language (oral or sign) by exposure without being specifically 123456789A written language is the representation of a language by means of a writing system. Written language is an invention in that it must be taught to children; children will pick up spoken language (oral or sign) by exposure without being specifically 123456789spoken language (oral or systA written language is the representation of a language by means of a writing system. Written language is an invention in that it must be taught to children; children will pick up spoken language (oral or sign) by exposure without being specifically 123456789A written language is the representation of a language by means of a writing system. Written language is an invention in that it must be taught to children; children will pick up spoken language (oral or sign) by exposure without being specifically 123456789A written language is the representation of a language by means of a writing system. Written language is an invention in that it must be taught to children; children will pick up spoken language (oral or sign) by exposure without being specifically 123456789A written language is the representation of a language by means of a writing system. Written language is an invention in that it must be taught to children; children will pick up spoken language (oral or sign) by exposure without being specifically 123456789A written language is the representation of a language by means of a writing system. Written language is an invention in that it must be taught to children; children will pick up spoken language (oral or sign) by exposure without being specifically 123456789A written language is the representation of a language by means of a writing system. Written language is an invention in that it must be tau\n",0);
//			Write_SD("A written language is the representation of a language by means of a writing system. Written language is an invention in that it must be taught to children; children will pick up spoken language (oral or sign) by exposure without being specifically 123456789A written language is the representation of a language by means of a writing system. Written language is an invention in that it must be taught to children; children will pick up spoken language (oral or sign) by exposure without being specifically 123456789A written language is the representation of a language by means of a writing system. Written language is an invention in that it must be taught to children; children will pick up spoken language (oral or sign) by exposure without being specifically 123456789A written language is the representation of a language by means of a writing system. Written language is an invention in that it must be taught to children; children will pick up spoken language (oral or sign) by exposure without being specifically 123456789A written language is the representation of a language by means of a writing system. Written language is an invention in that it must be taught to children; children will pick up spoken language (oral or sign) by exposure without being specifically 123456789A written language is the representation of a language by means of a writing system. Written language is an invention in that it must be taught to children; children will pick up spoken language (oral or sign) by exposure without being specifically 123456789A written language is the representation of a language by means of a writing system. Written language is an invention in that it must be taught to children; children will pick up spoken language (oral or sign) by exposure without being specifically 123456789A written language is the representation of a language by means of a writing system. Written language is an invention in that it must be taught to children; children will pick up spoken language (oral or sign) by exposure without being specifically 123456789guage by means of a writing system. Written language is an invention in that it must be taught to children; children will pick up spoken language (oral or sign) by exposure without being specifically 123456789A written language is the representation of a language by means of a writing system. Written language is an invention in that it must be taught to children; children will pick up spoken language (oral or sign) by exposure without being specifically 123456789spoken language (oral or systA written language is the representation of a language by means of a writing system. Written language is an invention in that it must be taught to children; children will pick up spoken language (oral or sign) by exposure without being specifically 123456789A written language is the representation of a language by means of a writing system. Written language is an invention in that it must be taught to children; children will pick up spoken language (oral or sign) by exposure without being specifically 123456789A written language is the representation of a language by means of a writing system. Written language is an invention in that it must be taught to children; children will pick up spoken language (oral or sign) by exposure without being specifically 123456789A written language is the representation of a language by means of a writing system. Written language is an invention in that it must be taught to children; children will pick up spoken language (oral or sign) by exposure without being specifically 123456789A written language is the representation of a language by means of a writing system. Written language is an invention in that it must be taught to children; children will pick up spoken language (oral or sign) by exposure without being specifically 123456789A written language is the representation of a language by means of a writing system. Written language is an invention in that it must be tau\n",0);
			DataMerger();
			Write_SD(MergedData,0);
			Flush_SD(0);
			if((cache_counter!=0)&&(Bike.Mode==0))
			{
				int i=0;
				for(i=0;i<cache_counter;i++)
				{
					if(i==0)
					{
						long2string(CAN_cache[i],WILLGO);
						strcpy(MergedData,time_char);
						strcat(MergedData,WILLGO);
						strcat(MergedData,"\n");
					}
					else
					{
						strcat(MergedData,",");
						long2string(CAN_cache[i],WILLGO);
						strcat(MergedData,WILLGO);
						strcat(MergedData,"\n");
					}
				}
				Write_SD(MergedData,2);
				cache_counter=0;
				Flush_SD(2);
			}
			logging_done=1;
			sleep_done=1;
		}

		Event_post(EVT_Alfat_DONE,Event_Id_00);	// <<<<========== TRY GETTING RID OF THIS, PROBS NO NEED ANYMORE

	}
}
void DataMerger()
{
	strcpy(MergedData,time_char);
	int v=0,n=0;

//	if(Bike.Mode==0) //If in Race Mode
	{
		for(n=0;n<NUMBER_OF_BQ_DEVICES;n++)
		{
			for(v=0;v<bq_pack.bq_devs[n].cell_count;v++)
			{
				integer2string(bq_pack.bq_devs[n].cell_voltage[v],WILLGO);
				strcat(MergedData,WILLGO);
				strcat(MergedData,",");
			}
		}

		for(n=0;n<NUMBER_OF_BQ_DEVICES;n++)
		{
			integer2string(bq_pack.bq_devs[n].temperature1,WILLGO);
			strcat(MergedData,WILLGO);
			strcat(MergedData,",");
			integer2string(bq_pack.bq_devs[n].temperature2,WILLGO);
			strcat(MergedData,WILLGO);
			strcat(MergedData,",");
		}

//		long2string(W2000.Status,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
		double2string((double)(W2000.MotorVelocityCommand),3.0,WILLGO);
		strcat(MergedData,WILLGO);
		strcat(MergedData,",");
		double2string((double)(W2000.MotorCurrentCommand),3.0,WILLGO); //throttle
		strcat(MergedData,WILLGO);
		strcat(MergedData,",");
		double2string((double)(W2000.BusCurrentCommand),3.0,WILLGO);
		strcat(MergedData,WILLGO);
		strcat(MergedData,",");
		double2string((double)(W2000.BusCurrent),3.0,WILLGO);
		strcat(MergedData,WILLGO);
		strcat(MergedData,",");
		double2string((double)(W2000.BusVoltage),3.0,WILLGO);
		strcat(MergedData,WILLGO);
		strcat(MergedData,",");
		double2string((double)(W2000.MotorVelocity),3.0,WILLGO);
		strcat(MergedData,WILLGO);
		strcat(MergedData,",");
		double2string((double)(W2000.MotorTemp),3.0,WILLGO);
		strcat(MergedData,WILLGO);
		strcat(MergedData,",");
		double2string((double)(W2000.VehicleVelocity),3.0,WILLGO);
		strcat(MergedData,WILLGO);
		strcat(MergedData,",");
		double2string((double)(W2000.PhaseATemp),3.0,WILLGO);
		strcat(MergedData,WILLGO);
		strcat(MergedData,",");
		double2string((double)(W2000.BusAmphour),3.0,WILLGO);
		strcat(MergedData,WILLGO);
		strcat(MergedData,",");
		double2string((double)(W2000.Odometer),3.0,WILLGO);	// IM NOT SURE IF FLOAT MAKES SENSE FOR ODOMETER!!!!!!
		strcat(MergedData,WILLGO);
		strcat(MergedData,",");

		double2string((double)(W2000.PhaseBcurrent),3.0,WILLGO);	// IM NOT SURE IF FLOAT MAKES SENSE FOR ODOMETER!!!!!!
		strcat(MergedData,WILLGO);
		strcat(MergedData,",");
		double2string((double)(W2000.PhaseCcurrent),3.0,WILLGO);	// IM NOT SURE IF FLOAT MAKES SENSE FOR ODOMETER!!!!!!
		strcat(MergedData,WILLGO);
		strcat(MergedData,",");
		double2string((double)(W2000.MotorVd),3.0,WILLGO);	// IM NOT SURE IF FLOAT MAKES SENSE FOR ODOMETER!!!!!!
		strcat(MergedData,WILLGO);
		strcat(MergedData,",");
		double2string((double)(W2000.MotorVq),3.0,WILLGO);	// IM NOT SURE IF FLOAT MAKES SENSE FOR ODOMETER!!!!!!
		strcat(MergedData,WILLGO);
		strcat(MergedData,",");
		double2string((double)(W2000.MotorId),3.0,WILLGO);	// IM NOT SURE IF FLOAT MAKES SENSE FOR ODOMETER!!!!!!
		strcat(MergedData,WILLGO);
		strcat(MergedData,",");
		double2string((double)(W2000.MotorIq),3.0,WILLGO);	// IM NOT SURE IF FLOAT MAKES SENSE FOR ODOMETER!!!!!!
		strcat(MergedData,WILLGO);
		strcat(MergedData,",");


		double2string(ext_ADC_status.current,3.0,WILLGO);
		strcat(MergedData,WILLGO);
		strcat(MergedData,",");
		//-------------------------------------------------------------
		long2string(bq_pack.voltage,WILLGO);
		strcat(MergedData,WILLGO);
		strcat(MergedData,",");
		integer2string(bq_pack.highest_cell_volts,WILLGO);
		strcat(MergedData,WILLGO);
		strcat(MergedData,",");
		integer2string(bq_pack.lowest_cell_volts,WILLGO);
		strcat(MergedData,WILLGO);
		strcat(MergedData,",");
		integer2string(bq_pack.highest_temp,WILLGO);
		strcat(MergedData,WILLGO);
		strcat(MergedData,",");

//----------------------------------------------------------------------
		integer2string((int)Bike.Warning.bit.OV,WILLGO);
		strcat(MergedData,WILLGO);
		strcat(MergedData,",");
		integer2string((int)Bike.Warning.bit.UV,WILLGO);
		strcat(MergedData,WILLGO);
		strcat(MergedData,",");
		integer2string((int)Bike.Warning.bit.OT,WILLGO);
		strcat(MergedData,WILLGO);
		strcat(MergedData,",");
		integer2string((int)Bike.Warning.bit.Dis_OC,WILLGO);
		strcat(MergedData,WILLGO);
		strcat(MergedData,",");
		integer2string((int)Bike.Warning.bit.Charage_OC,WILLGO);
		strcat(MergedData,WILLGO);
		strcat(MergedData,",");
		integer2string((int)Bike.Warning.bit.Capacity,WILLGO);
		strcat(MergedData,WILLGO);
		strcat(MergedData,",");
		integer2string((int)Bike.Warning.bit.BMM_loss,WILLGO);
		strcat(MergedData,WILLGO);
		strcat(MergedData,",");
		integer2string((int)Bike.Warning.bit.lowv_bat,WILLGO);
		strcat(MergedData,WILLGO);
		strcat(MergedData,",");
		integer2string((int)Bike.Warning.bit.w2000_off,WILLGO);
		strcat(MergedData,WILLGO);
		strcat(MergedData,",");
		integer2string((int)Bike.Warning.bit.current_sense_error,WILLGO);
		strcat(MergedData,WILLGO);
		strcat(MergedData,",");

//-------------------------------------------------------------------------
		integer2string((int)Bike.Fault.bit.OV,WILLGO);
		strcat(MergedData,WILLGO);
		strcat(MergedData,",");
		integer2string((int)Bike.Fault.bit.UV,WILLGO);
		strcat(MergedData,WILLGO);
		strcat(MergedData,",");
		integer2string((int)Bike.Fault.bit.OT,WILLGO);
		strcat(MergedData,WILLGO);
		strcat(MergedData,",");
		integer2string((int)Bike.Fault.bit.Dis_OC,WILLGO);
		strcat(MergedData,WILLGO);
		strcat(MergedData,",");
		integer2string((int)Bike.Fault.bit.Charge_OC,WILLGO);
		strcat(MergedData,WILLGO);
		strcat(MergedData,",");
		integer2string((int)Bike.Fault.bit.Motor_Error,WILLGO);
		strcat(MergedData,WILLGO);
		strcat(MergedData,",");
		integer2string((int)Bike.Fault.bit.BMM_Init,WILLGO);
		strcat(MergedData,WILLGO);
		strcat(MergedData,",");
		integer2string((int)Bike.Fault.bit.Switch_Error,WILLGO);
		strcat(MergedData,WILLGO);
		strcat(MergedData,",");
		integer2string((int)Bike.Fault.bit.lowv_bat,WILLGO);
		strcat(MergedData,WILLGO);
		strcat(MergedData,",");
		integer2string((int)Bike.Fault.bit.estop,WILLGO);
		strcat(MergedData,WILLGO);
		strcat(MergedData,",");

	}
//	else if(Bike.Mode==1)	//If in Charge Mode
//	{
//		for(n=0;n<NUMBER_OF_BQ_DEVICES;n++)
//		{
//			for(v=0;v<bq_pack.bq_devs[n].cell_count;v++)
//			{
//				integer2string(bq_pack.bq_devs[n].cell_voltage[v],WILLGO);
//				strcat(MergedData,WILLGO);
//				strcat(MergedData,",");
//			}
//		}
//		for(n=0;n<NUMBER_OF_BQ_DEVICES;n++)
//		{
//			integer2string(bq_pack.bq_devs[n].temperature1,WILLGO);
//			strcat(MergedData,WILLGO);
//			strcat(MergedData,",");
//			integer2string(bq_pack.bq_devs[n].temperature2,WILLGO);
//			strcat(MergedData,WILLGO);
//			strcat(MergedData,",");
//		}
//		//-------------------------------------------------------------
//		double2string(ext_ADC_status.current,3.0,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//		long2string(bq_pack.voltage,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//		integer2string(bq_pack.highest_cell_volts,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//		integer2string(bq_pack.lowest_cell_volts,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//		integer2string(bq_pack.highest_temp,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//
////----------------------------------------------------------------------
//		integer2string((int)Bike.Warning.bit.OV,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//		integer2string((int)Bike.Warning.bit.UV,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//		integer2string((int)Bike.Warning.bit.OT,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//		integer2string((int)Bike.Warning.bit.Dis_OC,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//		integer2string((int)Bike.Warning.bit.Charage_OC,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//		integer2string((int)Bike.Warning.bit.Capacity,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//		integer2string((int)Bike.Warning.bit.BMM_loss,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//		integer2string((int)Bike.Warning.bit.lowv_bat,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//		integer2string((int)Bike.Warning.bit.w2000_off,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//		integer2string((int)Bike.Warning.bit.current_sense_error,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//
////-------------------------------------------------------------------------
//		integer2string((int)Bike.Fault.bit.OV,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//		integer2string((int)Bike.Fault.bit.UV,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//		integer2string((int)Bike.Fault.bit.OT,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//		integer2string((int)Bike.Fault.bit.Dis_OC,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//		integer2string((int)Bike.Fault.bit.Charge_OC,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//		integer2string((int)Bike.Fault.bit.Motor_Error,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//		integer2string((int)Bike.Fault.bit.BMM_Init,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//		integer2string((int)Bike.Fault.bit.Switch_Error,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//		integer2string((int)Bike.Fault.bit.lowv_bat,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//		integer2string((int)Bike.Fault.bit.estop,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//
//	}
//	else if(Bike.Mode==2)	//If in Sleep Mode
//	{
//		for(n=0;n<NUMBER_OF_BQ_DEVICES;n++)
//		{
//			for(v=0;v<bq_pack.bq_devs[n].cell_count;v++)
//			{
//				integer2string(bq_pack.bq_devs[n].cell_voltage[v],WILLGO);
//				strcat(MergedData,WILLGO);
//				strcat(MergedData,",");
//			}
//		}
//		for(n=0;n<NUMBER_OF_BQ_DEVICES;n++)
//		{
//			integer2string(bq_pack.bq_devs[n].temperature1,WILLGO);
//			strcat(MergedData,WILLGO);
//			strcat(MergedData,",");
//			integer2string(bq_pack.bq_devs[n].temperature2,WILLGO);
//			strcat(MergedData,WILLGO);
//			strcat(MergedData,",");
//		}
//		//-------------------------------------------------------------
//
//		long2string(bq_pack.voltage,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//		integer2string(bq_pack.highest_cell_volts,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//		integer2string(bq_pack.lowest_cell_volts,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//		integer2string(bq_pack.highest_temp,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//
////----------------------------------------------------------------------
//		integer2string((int)Bike.Warning.bit.OV,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//		integer2string((int)Bike.Warning.bit.UV,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//		integer2string((int)Bike.Warning.bit.OT,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//		integer2string((int)Bike.Warning.bit.Dis_OC,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//		integer2string((int)Bike.Warning.bit.Charage_OC,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//		integer2string((int)Bike.Warning.bit.Capacity,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//		integer2string((int)Bike.Warning.bit.BMM_loss,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//		integer2string((int)Bike.Warning.bit.lowv_bat,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//		integer2string((int)Bike.Warning.bit.w2000_off,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//		integer2string((int)Bike.Warning.bit.current_sense_error,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//
////-------------------------------------------------------------------------
//		integer2string((int)Bike.Fault.bit.OV,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//		integer2string((int)Bike.Fault.bit.UV,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//		integer2string((int)Bike.Fault.bit.OT,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//		integer2string((int)Bike.Fault.bit.Dis_OC,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//		integer2string((int)Bike.Fault.bit.Charge_OC,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//		integer2string((int)Bike.Fault.bit.Motor_Error,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//		integer2string((int)Bike.Fault.bit.BMM_Init,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//		integer2string((int)Bike.Fault.bit.Switch_Error,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//		integer2string((int)Bike.Fault.bit.lowv_bat,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//		integer2string((int)Bike.Fault.bit.estop,WILLGO);
//		strcat(MergedData,WILLGO);
//		strcat(MergedData,",");
//
//	}
	strcat(MergedData,"\n");
}
void integer2string(int integer,char* toror)
{
	char toror1[20];

	ltoa(((long)integer),toror1);
	strcpy(toror,toror1);
}
void long2string(long long_number,char* toror)
{
	char toror1[20];

	ltoa(long_number,toror1);
	strcpy(toror,toror1);
}

void double2string(double doub, double precision, char* toror)
{
	char toror3[10];
    char toror2[10];
    char toror1[10];
    int full=0;
    int decimal=0;

    if(doub>=0)
    {
		full = (int) doub;	// INSTEAD OF INT, TRY LONG TYPE CASTING TO INCREASE THE RANGE!!!!!!!!!!!!
		ltoa(((long)full),toror1);
		decimal=(int)((doub - (double)full)*pow(10,precision));	// INSTEAD OF INT, TRY LONG TYPE CASTING TO INCREASE THE RANGE!!!!!!!!!!!!
		if(decimal<pow(10,precision-1))
		{
			int i=0;
			for(i=0;i<=precision-1;i++)
			{
				if(i==precision-1)
					toror2[i]=0;
				else if(i==1)
				{
					if(decimal<pow(10,precision-2))
					toror2[i]='0';
				}
				else
				toror2[i]='0';
			}
			ltoa(((long)decimal),toror3);
			strcat(toror2,toror3);
		}
		else
		ltoa(((long)decimal),toror2);
		strcat(toror1,".");
		strcat(toror1,toror2);
		strcpy(toror,toror1);
    }
    else
    {
    	doub=doub*(-1.0);
        full = (int) doub;
        ltoa(((long)full),toror1);
		decimal=(int)((doub - (double)full)*pow(10,precision));	// INSTEAD OF INT, TRY LONG TYPE CASTING TO INCREASE THE RANGE!!!!!!!!!!!!
		if(decimal<pow(10,precision-1))
		{
			int i=0;
			for(i=0;i<=precision-1;i++)
			{
				if(i==precision-1)
					toror2[i]=0;
				else if(i==1)
				{
					if(decimal<pow(10,precision-2))
					toror2[i]='0';
				}
				else
				toror2[i]='0';
			}
			ltoa(((long)decimal),toror3);
			strcat(toror2,toror3);
		}
		else
		ltoa(((long)decimal),toror2);
        strcat(toror1,".");
        strcat(toror1,toror2);
        strcpy(toror,"-");
        strcat(toror,toror1);
    }

}

void Set_RTC_Time(char* date_out, char* time_out)
{
	// !!!!!!!!!!! MIND THE FORMATS !!!!!!!!!!!!!
	// dd/mm/yy	<== Dats right, that's the correct format
	// hh:mm:ss	<== Needs to be in 24H format!
	long prep_time=0;
	char timeout_ascii[8];
	char timeout_command[12];

	int day_out = (10*(int)(date_out[0]-48))+(int)(date_out[1]-48);
	int month_out = (10*(int)(date_out[3]-48))+(int)(date_out[4]-48);
	int year_out = (10*(int)(date_out[6]-48))+(int)(date_out[7]-48)+20;

	int hour_out = (10*(int)(time_out[0]-48))+(int)(time_out[1]-48);
	int minute_out = (10*(int)(time_out[3]-48))+(int)(time_out[4]-48);
	int second_out = (int)(((10*(int)(time_out[6]-48))+(int)(time_out[7]-48))/2);

	prep_time=year_out;
	prep_time=prep_time*16; // shift left 4 bits
	prep_time=prep_time+month_out;
	prep_time=prep_time*32; // shift left 5 bits
	prep_time=prep_time+day_out;
	prep_time=prep_time*32; // shift left 5 bits
	prep_time=prep_time+hour_out;
	prep_time=prep_time*64; // shift left 6 bits
	prep_time=prep_time+minute_out;
	prep_time=prep_time*32; // shift left 5 bits
	prep_time=prep_time+second_out;



	if((prep_time/268435456)>9)
	timeout_ascii[0] = (int)(prep_time/268435456) + '7';
	else
	timeout_ascii[0] = (int)(prep_time/268435456) + '0';
	prep_time = prep_time&(0xFFFFFFF);

	if((prep_time/16777216)>9)
	timeout_ascii[1] = (int)(prep_time/16777216) + '7';
	else
	timeout_ascii[1] = (int)(prep_time/16777216) + '0';
	prep_time = prep_time&(0xFFFFFF);

	if((prep_time/1048576)>9)
	timeout_ascii[2] = (int)(prep_time/1048576) + '7';
	else
	timeout_ascii[2] = (int)(prep_time/1048576) + '0';
	prep_time = prep_time&(0xFFFFF);

	if((prep_time/65536)>9)
	timeout_ascii[3] = (int)(prep_time/65536) + '7';
	else
	timeout_ascii[3] = (int)(prep_time/65536) + '0';
	prep_time = prep_time&(0xFFFF);

	if((prep_time/4096)>9)
	timeout_ascii[4] = (int)(prep_time/4096) + '7';
	else
	timeout_ascii[4] = (int)(prep_time/4096) + '0';
	prep_time = prep_time&(0xFFF);

	if((prep_time/256)>9)
	timeout_ascii[5] = (int)(prep_time/256) + '7';
	else
	timeout_ascii[5] = (int)(prep_time/256) + '0';
	prep_time = prep_time&(0xFF);

	if((prep_time/16)>9)
	timeout_ascii[6] = (int)(prep_time/16) + '7';
	else
	timeout_ascii[6] = (int)(prep_time/16) + '0';
	prep_time = prep_time&(0xF);

	if((prep_time)>9)
	timeout_ascii[7] = (int)(prep_time) + '7';
	else
	timeout_ascii[7] = (int)(prep_time) + '0';


	strcpy(timeout_command,"S ");
	strcat(timeout_command,timeout_ascii);
	strcat(timeout_command,"\0");
	I2C_print(timeout_command,0,4,0x52,0);
}


Void CLK_TimeStamp(UArg a0, UArg a1)
{
	Event_post(EVT_TimeStamp_CLK,Event_Id_00);
}

void TSK_TimeStamp(UArg a0, UArg a1)
{
	while (1)
	{
		Event_pend(EVT_TimeStamp_CLK,Event_Id_00, Event_Id_NONE,BIOS_WAIT_FOREVER);

		second++;
		if(second>=60)
		{
			second=0;
			minute++;
		}
		if(minute>=60)
		{
			minute=0;
			hour++;
		}
		if(hour>=24)
		{
			hour=0;
			day++;
		}


		time_char[2]=':';
		time_char[5]=':';
		if (hour < 10)
		{
			time_char[0]='0';
			time_char[1]=(char)(hour+48);
		}
		else
		{
			time_char[0]=(char)((hour/10)+48);
			time_char[1]=(char)((hour%10)+48);
		}
		if (minute < 10)
		{
			time_char[3]='0';
			time_char[4]=(char)(minute+48);
		}
		else
		{
			time_char[3]=(char)((minute/10)+48);
			time_char[4]=(char)((minute%10)+48);
		}
		if (second < 10)
		{
			time_char[6]='0';
			time_char[7]=(char)(second+48);
		}
		else
		{
			time_char[6]=(char)((second/10)+48);
			time_char[7]=(char)((second%10)+48);
		}
		time_char[8]=',';
		time_char[9]=0;

	}
}

void Event_Logger(char *string)
{
	if(ALFAT_status.Init==1)
	{
		while(logging_done==0)
		{
			Task_sleep(5);
		}
		logging_done=0;
		Write_SD(time_char,1);
		Write_SD(string, 1);
		Write_SD("\n", 1);
		Flush_SD(1);
		logging_done=1;
	}
}

int Write_SD(char *string, int handler)
{
	while(write_done==0)
	{
		Task_sleep(5);
	}
	write_done=0;
	int outbound_length=((int)strlen(string));
	int error_code=0;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if((handler % 16)>9)
	hand_hex = (handler % 16) + '7';
	else
	hand_hex = (handler % 16) + '0';

	if((outbound_length>255)&&(outbound_length<4096))
	{
		out_com3[0]='W';
		out_com3[1]=' ';
		out_com3[2] = hand_hex;

		out_com3[3] = '>';


		if((outbound_length / 256)>9)
		length_str3[0] = (outbound_length / 256) + '7';
		else
		length_str3[0] = (outbound_length / 256) + '0';
		outbound_length = outbound_length&(0xFF);

		if((outbound_length / 16)>9)
		length_str3[1] = (outbound_length / 16) + '7';
		else
		length_str3[1] = (outbound_length / 16) + '0';
		outbound_length = outbound_length&(0xF);

		if((outbound_length)>9)
		length_str3[2] = (outbound_length) + '7';
		else
		length_str3[2] = (outbound_length) + '0';


		out_com3[4]=length_str3[0];
		out_com3[5]=length_str3[1];
		out_com3[6]=length_str3[2];
		out_com3[7] = 0;

		error_code=I2C_print(out_com3,0,4,0x52,0);

	}
	else if((outbound_length>15)&&(outbound_length<256))
	{
		out_com2[0]='W';
		out_com2[1]=' ';
		out_com2[2] = hand_hex;

		out_com2[3] = '>';

		if((outbound_length / 16)>9)
		length_str2[0] = (outbound_length / 16) + '7';
		else
		length_str2[0] = (outbound_length / 16) + '0';
		outbound_length = outbound_length&(0xF);

		if((outbound_length)>9)
		length_str2[1] = (outbound_length) + '7';
		else
		length_str2[1] = (outbound_length) + '0';

		out_com2[4]=length_str2[0];
		out_com2[5]=length_str2[1];
		out_com2[6] = 0;

		error_code=I2C_print(out_com2,0,4,0x52,0);

	}
	else if(outbound_length<16)
	{
		out_com1[0]='W';
		out_com1[1]=' ';

		out_com1[2] = hand_hex;

		out_com1[3]= '>';

		if(outbound_length>9)
		out_com1[4] = (outbound_length % 16) + '7';
		else
		out_com1[4] = (outbound_length % 16) + '0';


		out_com1[5] = 0;
		error_code=I2C_print(out_com1,0,4,0x52,0);
	}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if(error_code==0)
	{
		ALFAT_status.error = 0;
		I2C_print(string,0,14,0x52,1);
		write_done=1;
		return 0;
	}
	else
	{
		ALFAT_status.error = 2;
		ALFAT_status.Init = 0;
		Clock_stop(ALFAT_CLOCK);
		Bike.Warning.bit.SD_Write=1;
		write_done=1;
		Event_post(EVT_Error,Event_Id_00);
		return 1;
	}

}



void Flush_SD(int handler)
{
	char flush_command[4];
	strcpy(flush_command,"F 0\0");
	flush_command[2]=flush_command[2]+handler;
	flush_error=I2C_print(flush_command,0,4,0x52,0);

	if(flush_error==1)
	{
		flush_error_count++;
		Clock_stop(ALFAT_CLOCK);

		if((array[0]==0x21)&&(array[1]==69)&&(array[2]==69))
		{
			I2C_Startup();
			EALLOW;
			GpioCtrlRegs.GPBPUD.bit.GPIO60 = 1;
			GpioCtrlRegs.GPBDIR.bit.GPIO60 = 1;
			GpioDataRegs.GPBCLEAR.bit.GPIO60 = 1;

			EDIS;

			Task_sleep(50);

			EALLOW;
			GpioCtrlRegs.GPBPUD.bit.GPIO60 = 0;
			GpioDataRegs.GPBSET.bit.GPIO60 = 1;
			GpioCtrlRegs.GPBDIR.bit.GPIO60 = 1;
			EDIS;
			Task_sleep(500);

			while((GpioDataRegs.GPADAT.bit.GPIO10==1)||(receive_done==0))
			{
				Task_sleep(5);
			}

			//command to receive intro message
			receive_done = 0;

			counter=0;
			counter2=0;
			nonzero=0;
			l=0;

			tor = 73;
			I2caRegs.I2CSAR = 0x52;
			I2caRegs.I2CSTR.bit.ARDY = 1;
			I2caRegs.I2CSTR.bit.XRDY = 1;
			I2caRegs.I2CCNT = 73;
			I2caRegs.I2CMDR.all = 0x6C20;

			while((receive_done==0))
			{
				Task_sleep(5);
			}

			I2C_print("I M:",0,4,0x52,0);

			//INIT RTC
			I2C_print("T B",0,4,0x52,0);

			I2C_print(global_file,0,4,0x52,0);
			I2C_print("O 1A>M:\\Events.CSV\0",0,4,0x52,0);
			I2C_print("O 2A>M:\\W2000_Status.CSV\0",0,4,0x52,0);
			ALFAT_status.Init = 1;
			write_done=1;
			logging_done=1;
			Event_Logger("FLUSH RESET");
			Clock_start(ALFAT_CLOCK);
			if(ext_ADC_status.Init==1)
			{
				Clock_start(EXT_ADC_CLOCK);
			}

		}
	}
}
void Battery_Voltage_Headers()
{
	char header_1[13];
	if(NUMBER_OF_CELLS<10)
	{
		strcpy(header_1,"Battery0V,");
		int battery_number;
		for(battery_number=1;battery_number<NUMBER_OF_CELLS;battery_number++)
		{
			strcat(MergedData,header_1);
			header_1[7]++;
		}
	}
	else if ((NUMBER_OF_CELLS>=10)&&(NUMBER_OF_CELLS<100))
	{
		strcpy(header_1,"Battery0V,");
		int battery_number;
		for(battery_number=0;battery_number<10;battery_number++)
		{
			strcat(MergedData,header_1);
			header_1[7]++;
		}

		strcpy(header_1,"Battery10V,");

		for(battery_number=10;battery_number<NUMBER_OF_CELLS;battery_number++)
		{
			strcat(MergedData,header_1);
			if(header_1[8]=='9')
			{
				header_1[7]++;
				header_1[8]='0';
			}
			else
			{
				header_1[8]++;
			}
		}

	}
	else if ((NUMBER_OF_CELLS>=100)&&(NUMBER_OF_CELLS<200))
	{
		strcpy(header_1,"Battery0V,");
		int battery_number;
		for(battery_number=0;battery_number<10;battery_number++)
		{
			strcat(MergedData,header_1);
			header_1[7]++;
		}

		strcpy(header_1,"Battery10V,");

		for(battery_number=10;battery_number<100;battery_number++)
		{
			strcat(MergedData,header_1);
			if(header_1[8]=='9')
			{
				header_1[7]++;
				header_1[8]='0';
			}
			else
			{
				header_1[8]++;
			}
		}

		strcpy(header_1,"Battery100V,");

		for(battery_number=100;battery_number<NUMBER_OF_CELLS;battery_number++)
		{
			strcat(MergedData,header_1);
			if(header_1[9]=='9')
			{
				header_1[8]++;
				header_1[9]='0';
			}
			else
			{
				header_1[9]++;
			}
		}
	}

}

void Get_Time(void)
{
	time_in = 1;
	I2C_print("G T",0,13,0x52,0);
	date_in = 1;
	I2C_print("G D",0,15,0x52,0);
}



