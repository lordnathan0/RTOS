/*
 * 12v.c
 *
 *  Created on: Nov 13, 2012
 *      Author: LordLaptop
 */

#include <stdio.h>
#include <xdc/std.h>

#include <xdc/runtime/System.h>

#include <ti/sysbios/BIOS.h>

#include <ti/sysbios/knl/Task.h>

#define xdc__strict

#include <ti/sysbios/knl/Task.h>

#include <ti/sysbios/knl/Clock.h>

#include <ti/sysbios/knl/Event.h>

#include <ti/sysbios/family/c28/Hwi.h>

#include <ti/sysbios/knl/Semaphore.h>

#include <xdc/cfg/global.h>

#include "DSP28x_Project.h"

#include "adc.h"

#include "main.h"

ADC_st ADC;
extern Bike_Status Bike;
char firsta = 0;
float convert_value;
int warning;

void adc_init()
{
	ADC.Init = 0;
	firsta = 0;
    EALLOW;
    SysCtrlRegs.HISPCP.all = ADC_MODCLK; // HSPCLK = SYSCLKOUT/ADC_MODCLK
    EDIS;

    InitAdc();  // Init the ADC

    // Specific ADC setup for this example:
    AdcRegs.ADCTRL1.bit.ACQ_PS = ADC_SHCLK;
    AdcRegs.ADCTRL3.bit.ADCCLKPS = ADC_CKPS;
    AdcRegs.ADCTRL1.bit.SEQ_CASC = 1;       // Cascaded mode
    AdcRegs.ADCMAXCONV.all = 0; //number of adcs - 1
    AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 8;	//result0 connected to B0
    AdcRegs.ADCTRL1.bit.CONT_RUN = 0;       // Setup Start and Stop Mode
    AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1 = 1;   // This bit enables the interrupt request to CPU by INT SEQ1.
    AdcRegs.ADCTRL2.bit.SOC_SEQ1 = 1;
    Event_post(EVT_ADC,Event_Id_00);
	while (firsta == 0) 		//wait on conversion is done
	{
		Task_sleep(5);
	}
	ADC.Init = 1;
	Clock_start(ADC_CLOCK);

}

void Disable_ADC()
{
	ADC.Init = 0;
	Clock_stop(ADC_CLOCK);
}

void CLK_ADC()
{
	Event_post(EVT_ADC,Event_Id_00);
}

void TSK_adc(UArg a0, UArg a1)
{
 //   System_printf("enter taskFxn()\n");
	while (1)
	{
		//start conversion
		Event_pend(EVT_ADC,Event_Id_00, Event_Id_NONE,BIOS_WAIT_FOREVER);
	    AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;    // Reset SEQ1
	    AdcRegs.ADCTRL2.bit.SOC_SEQ1 = 1;    // This is used to initiate a software initiated conversion.
	}
}

void HWI_ADC_ConversionComplete(UArg a0, UArg a1)
{
		firsta = 1;

	    convert_value = (float)(AdcRegs.ADCRESULT0 >> 4) ;   //the value 0-4095
	    convert_value= (convert_value / 4095) * 3;  //gives voltage in 0-3V range
	    convert_value = convert_value * ADC_RATIO;  //gives voltage in 0-12V range

	    ADC.bat_12v = convert_value;
	    warning = Bike.Warning.all;
	    switch (Bike.Mode)
	    {

			case MODE_CHARGE:

				if (ADC.bat_12v <= LOWV_BATTERY_WARNING_THRESHOLD_CHARGE)
				{
					Bike.Warning.bit.lowv_bat = 1;
					Event_post(EVT_Error,Event_Id_00);
				}
				else
				{
					Bike.Warning.bit.lowv_bat = 0; //unlatch warning
				}

				if (ADC.bat_12v <= LOWv_BATTERY_FUALT_THRESHOLD_CHARGE)
				{
					Bike.Fault.bit.lowv_bat = 1;
					Event_post(EVT_Error,Event_Id_00);
				}
				break;

			case MODE_RACE:

				if (ADC.bat_12v <= LOWV_BATTERY_WARNING_THRESHOLD_RACE )
				{
					Bike.Warning.bit.lowv_bat = 1;
					Event_post(EVT_Error,Event_Id_00);
				}
				else
				{
					Bike.Warning.bit.lowv_bat = 0; //unlatch warning
				}

				if (ADC.bat_12v <= LOWv_BATTERY_FUALT_THRESHOLD_RACE )
				{
					Bike.Fault.bit.lowv_bat = 1;
					Event_post(EVT_Error,Event_Id_00);
				}

			break;

			case MODE_SLEEP:

				if (ADC.bat_12v <= LOWV_BATTERY_WARNING_THRESHOLD_SLEEP )
				{
					Bike.Warning.bit.lowv_bat = 1;
					Event_post(EVT_Error,Event_Id_00);
				}
				else
				{
					Bike.Warning.bit.lowv_bat = 0; //unlatch warning
				}

				if (ADC.bat_12v <= LOWv_BATTERY_FUALT_THRESHOLD_SLEEP )
				{
					Bike.Fault.bit.lowv_bat = 1;
					Event_post(EVT_Error,Event_Id_00);
				}


			break;
	    }
	    if(warning != Bike.Warning.all) //for un-latching warnings
	    {
	    	Event_post(EVT_Error,Event_Id_00);
	    }
	    AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;       // Clear INT SEQ1 bit
}
