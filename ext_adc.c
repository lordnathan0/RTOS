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

#include "main.h"
#include "ALFAT.h"
#include "ext_adc.h"


#define ADC_SLAVE_ADDR        72

// Ext ADC vars
extern char count2;
extern int adc_status;
extern int write_done;
extern char adc_current_count;
extern int adc_current;
extern int initializable;

ext_ADC_st ext_ADC_status;
extern Bike_Status Bike;

// ALFAT vars
extern int receive_done;
extern ALFAT_st ALFAT_status;
extern int write_done;
extern int logging_done;

void ext_ADC_Disable()
{
	Clock_stop(EXT_ADC_CLOCK);
	ext_ADC_status.Init=0;
	while(logging_done==0)
	{
		Task_sleep(5);
	}
}
void ext_ADC_Startup()
{
	ext_ADC_status.Init=0;
	ext_ADC_status.status = 0;
	ext_ADC_status.timeout_count = 0;

	if(ALFAT_status.Init == 1)
	{
		Clock_stop(ALFAT_CLOCK);
		while(logging_done==0)
		{
			Task_sleep(5);
		}
		logging_done=0;
	}

	while((receive_done==0))
	{
		Task_sleep(5);
	}

	receive_done=0;
	initializable=0;
	adc_current_count = 0;
	I2caRegs.I2CSAR = ADC_SLAVE_ADDR;
	I2caRegs.I2CCNT = 3;
//	I2caRegs.I2CSTR.bit.ARDY = 1;
//	I2caRegs.I2CSTR.bit.SCD = 1;
//	I2caRegs.I2CMDR.bit.BC = 0;
//	I2caRegs.I2CMDR.bit.DLB = 0;
//	I2caRegs.I2CMDR.bit.FDF = 0;
//	I2caRegs.I2CMDR.bit.FREE = 1;
//	I2caRegs.I2CMDR.bit.MST = 1;
//	I2caRegs.I2CMDR.bit.NACKMOD = 0;
//	I2caRegs.I2CMDR.bit.STB = 0;
//	I2caRegs.I2CMDR.bit.STP = 1;
//	I2caRegs.I2CMDR.bit.TRX = 0;
//	I2caRegs.I2CMDR.bit.XA = 0;
//	I2caRegs.I2CMDR.bit.STT = 1;
//	I2caRegs.I2CMDR.bit.RM = 0;
	I2caRegs.I2CMDR.all = 0x6C20;
	Task_sleep(5000);

	if(initializable==1)
	{
		//CHECK FOR MODE AND FUNCTION ACCORDINGLY
		if(Bike.Mode==2)
		{
			Event_post(EVT_Ext_ADC_CLK,Event_Id_00);
			ext_ADC_status.Init=1;
		}
		else
		{
			Clock_start(EXT_ADC_CLOCK);
			ext_ADC_status.Init=1;
		}
   	}
   	else
   	{
   		receive_done=1;
 		ext_ADC_status.Init=0;
  		Bike.Warning.bit.current_sense_error = 1;
  		Event_post(EVT_Error,Event_Id_00);
   	}

	if(ALFAT_status.Init == 1)
	{
		Clock_start(ALFAT_CLOCK);
		logging_done=1;
	}
}


Void CLK_Ext_ADC(UArg a0, UArg a1)
{
	Event_post(EVT_Ext_ADC_CLK,Event_Id_00);
}

void TSK_Ext_ADC()
{
	while(1)
	{
		Event_pend(EVT_Ext_ADC_CLK,Event_Id_00, Event_Id_NONE,BIOS_WAIT_FOREVER);
		if(ext_ADC_status.Init==1)
		{
			while((receive_done==0)||(logging_done==0))
			{
				Task_sleep(5);
			}
			logging_done=0;
			receive_done=0;

			adc_current_count = 0;
			I2caRegs.I2CSAR = ADC_SLAVE_ADDR;
			I2caRegs.I2CCNT = 3;
	//		I2caRegs.I2CSTR.bit.ARDY = 1;
	//		I2caRegs.I2CSTR.bit.SCD = 1;
	//		I2caRegs.I2CMDR.all = 0x6C20;
	//		I2caRegs.I2CMDR.bit.BC = 0;
	//		I2caRegs.I2CMDR.bit.DLB = 0;
	//		I2caRegs.I2CMDR.bit.FDF = 0;
	//		I2caRegs.I2CMDR.bit.FREE = 1;
	//		I2caRegs.I2CMDR.bit.MST = 1;
	//		I2caRegs.I2CMDR.bit.NACKMOD = 0;
	//		I2caRegs.I2CMDR.bit.STB = 0;
	//		I2caRegs.I2CMDR.bit.STP = 1;
	//		I2caRegs.I2CMDR.bit.TRX = 0;
	//		I2caRegs.I2CMDR.bit.XA = 0;
	//		I2caRegs.I2CMDR.bit.STT = 1;
			I2caRegs.I2CMDR.all = 0x6C20;
			while((receive_done==0))
			{
				Task_sleep(5);
				ext_ADC_status.timeout_count++;
				if(ext_ADC_status.timeout_count>1000)
				{
					Clock_stop(EXT_ADC_CLOCK);
					ext_ADC_status.Init=0;
					receive_done=1;
					Bike.Warning.bit.current_sense_error=1;
					Event_post(EVT_Error,Event_Id_00);
				}

			}
			ext_ADC_status.timeout_count=0;
			logging_done=1;
		}
	}

}

void Ext_Adc_Error_Check()
{
    unsigned long warning = Bike.Warning.all;
    switch (Bike.Mode)
    {

		case MODE_CHARGE:

			if (ext_ADC_status.current <= DIS_OC_WARNING_THRESHOLD_CHARGE)
			{
				Bike.Warning.bit.Dis_OC = 1;
				Event_post(EVT_Error,Event_Id_00);
			}
			else
			{
				Bike.Warning.bit.Dis_OC = 0; //unlatch warning
			}

			if (ext_ADC_status.current <= DIS_OC_FAULT_THRESHOLD_CHARGE)
			{
				Bike.Fault.bit.Dis_OC = 1;
				Event_post(EVT_Error,Event_Id_00);
			}

			if (ext_ADC_status.current >= CHARGE_OC_WARNING_THRESHOLD_CHARGE)
			{
				Bike.Warning.bit.Charage_OC = 1;
				Event_post(EVT_Error,Event_Id_00);
			}
			else
			{
				Bike.Warning.bit.Charage_OC = 0; //unlatch warning
			}

			if (ext_ADC_status.current >= CHARGE_OC_FAULT_THRESHOLD_CHARGE)
			{
				Bike.Fault.bit.Charge_OC = 1;
				Event_post(EVT_Error,Event_Id_00);
			}
			break;

		case MODE_RACE:

			if (ext_ADC_status.current <= DIS_OC_WARNING_THRESHOLD_RACE)
			{
				Bike.Warning.bit.Dis_OC = 1;
				Event_post(EVT_Error,Event_Id_00);
			}
			else
			{
				Bike.Warning.bit.Dis_OC = 0; //unlatch warning
			}

			if (ext_ADC_status.current <= DIS_OC_FAULT_THRESHOLD_RACE)
			{
				Bike.Fault.bit.Dis_OC = 1;
				Event_post(EVT_Error,Event_Id_00);
			}

			if (ext_ADC_status.current >= CHARGE_OC_WARNING_THRESHOLD_RACE)
			{
				Bike.Warning.bit.Charage_OC = 1;
				Event_post(EVT_Error,Event_Id_00);
			}
			else
			{
				Bike.Warning.bit.Charage_OC = 0; //unlatch warning
			}

			if (ext_ADC_status.current >= CHARGE_OC_FAULT_THRESHOLD_RACE)
			{
				Bike.Fault.bit.Charge_OC = 1;
				Event_post(EVT_Error,Event_Id_00);
			}
			break;

		case MODE_SLEEP:

			if (ext_ADC_status.current <= DIS_OC_WARNING_THRESHOLD_SLEEP)
			{
				Bike.Warning.bit.Dis_OC = 1;
				Event_post(EVT_Error,Event_Id_00);
			}
			else
			{
				Bike.Warning.bit.Dis_OC = 0; //unlatch warning
			}

			if (ext_ADC_status.current <= DIS_OC_FAULT_THRESHOLD_SLEEP)
			{
				Bike.Fault.bit.Dis_OC = 1;
				Event_post(EVT_Error,Event_Id_00);
			}

			if (ext_ADC_status.current >= CHARGE_OC_WARNING_THRESHOLD_SLEEP)
			{
				Bike.Warning.bit.Charage_OC = 1;
				Event_post(EVT_Error,Event_Id_00);
			}
			else
			{
				Bike.Warning.bit.Charage_OC = 0; //unlatch warning
			}

			if (ext_ADC_status.current >= CHARGE_OC_FAULT_THRESHOLD_SLEEP)
			{
				Bike.Fault.bit.Charge_OC = 1;
				Event_post(EVT_Error,Event_Id_00);
			}
			break;
    }
    if(warning != Bike.Warning.all) //for un-latching warnings
    {
    	Event_post(EVT_Error,Event_Id_00);
    }
}

