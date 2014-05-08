/*
 * Sleep.c
 *
 *  Created on: Nov 1, 2012
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

#include "bq_pack.h"

#include "BQ.h"

#include "eCAN.h"

#include "Bluetooth.h"

#include "LCD.h"

#include "ALFAT.h"

#include "I2C.h"

#include "ext_adc.h"

#include "Sleep.h"

#include "Race.h"

#include "Sleep.h"

#include "Charge.h"

#include "Action.h"

#include "GPIO.h"

#include "adc.h"

#include "main.h"

extern Bike_Status Bike;
char watchdog_count;

void Sleep()
{

	Bike.Mode = MODE_SLEEP;
	Bike.Fault.all = 0;
	Bike.Warning.all = 0;
	watchdog_count = 0;
	Event_Logger("MODE CHANGE TO SLEEP");
	if(Event_pend(EVT_Change_Mode,Event_Id_00, Event_Id_NONE,BIOS_NO_WAIT) != 0)
	{
		Event_post(EVT_Change_Mode,Event_Id_00);
		return;
	}
	Disconnect_Contactors();
	if(Event_pend(EVT_Change_Mode,Event_Id_00,Event_Id_NONE, BLEEDER_TIME) != 0) //tenth of a second wait
	{
		Event_post(EVT_Change_Mode,Event_Id_00);
		return;
	}
	if(Event_pend(EVT_Change_Mode,Event_Id_00, Event_Id_NONE,BIOS_NO_WAIT) != 0)
	{
		Event_post(EVT_Change_Mode,Event_Id_00);
		return;
	}
	Sleep_Peripherals();
	if(Event_pend(EVT_Change_Mode,Event_Id_00, Event_Id_NONE,BIOS_NO_WAIT) != 0)
	{
		Event_post(EVT_Change_Mode,Event_Id_00);
		return;
	}
	GPIO_Sleep();
	if(Event_pend(EVT_Change_Mode,Event_Id_00, Event_Id_NONE,BIOS_NO_WAIT) != 0)
	{
		Event_post(EVT_Change_Mode,Event_Id_00);
		return;
	}
	Init_WAKEINT();
	if(Event_pend(EVT_Change_Mode,Event_Id_00, Event_Id_NONE,BIOS_NO_WAIT) != 0)
	{
		Event_post(EVT_Change_Mode,Event_Id_00);
		return;
	}
	CPU_Sleep();
}

void GPIO_Sleep()
{
	GpioDataRegs.GPCSET.C_FUEL_DOOR = 1;
	GpioDataRegs.GPCCLEAR.C_3V3_MAIN = 1;
	GpioDataRegs.GPACLEAR.A_OV_FAULT = 1;
	GpioDataRegs.GPACLEAR.A_WARNING 	= 1;
	GpioDataRegs.GPACLEAR.A_FAULT = 1;
	GpioDataRegs.GPACLEAR.A_FAULT_OVERRIDE = 1;
	GpioDataRegs.GPACLEAR.A_CHARGING = 1;
	GpioDataRegs.GPACLEAR.A_OV_WARNING = 1;
	GpioDataRegs.GPACLEAR.A_UV_FAULT= 1;
	GpioDataRegs.GPACLEAR.A_UV_WARNING= 1;
	GpioDataRegs.GPACLEAR.A_OT_FAULT= 1;
	GpioDataRegs.GPACLEAR.B_12V_MAIN_CTRL = 1;
	GpioDataRegs.GPACLEAR.A_12V_AUX_CTRL = 1;
	GpioDataRegs.GPBCLEAR.B_12V_LOW = 1;
	GpioDataRegs.GPBCLEAR.B_OT_WARNING= 1;

	Task_sleep(5000); //tenth of a second wait

	if (GpioDataRegs.GPBDAT.B_12V_SENSE == 0)
	{
		GpioDataRegs.GPASET.A_SAFETY_INTERLOCK = 1;
		Bike.Fault.bit.estop = 1;
	}
	else
	{
		GpioDataRegs.GPACLEAR.A_SAFETY_INTERLOCK = 1;
	}
}

void CPU_Sleep()
{
	GpioDataRegs.GPACLEAR.B_12V_MAIN_CTRL = 1;
	GpioDataRegs.GPACLEAR.A_12V_AUX_CTRL = 1;
	GpioDataRegs.GPCCLEAR.C_3V3_MAIN = 1;
    EALLOW;
    SysCtrlRegs.LPMCR0.bit.LPM = 1; //put into standby
    EDIS;
}

void TSK_Sleep_Measurment()
{

	while(1)
	{
		Event_pend(EVT_Sleep_M,Event_Id_00, Event_Id_NONE,BIOS_WAIT_FOREVER);
		if (watchdog_count < WD_COUNT)
		{
			watchdog_count++;
			Init_WAKEINT();
			CPU_Sleep();
		}
		else
		{
			watchdog_count = 0;
			Disable_WAKEINT();
			GPIO_Sleep_Measurment();
			Start_Sleep_Measurment();
			Sleep_Peripherals();
			Init_WAKEINT();
			CPU_Sleep();
		}
	}
}

void GPIO_Sleep_Measurment()
{
	GpioDataRegs.GPCSET.C_3V3_MAIN = 1;
	GpioDataRegs.GPACLEAR.B_12V_MAIN_CTRL = 1;
	GpioDataRegs.GPACLEAR.A_12V_AUX_CTRL = 1;

	Task_sleep(5000); //tenth of a second wait

	if (GpioDataRegs.GPBDAT.B_12V_SENSE == 1)
	{
		GpioDataRegs.GPASET.A_SAFETY_INTERLOCK = 1;
		Bike.Fault.bit.estop = 1;
	}
	else
	{
		GpioDataRegs.GPACLEAR.A_SAFETY_INTERLOCK = 1;
	}
}

void Sleep_Peripherals()
{
	BMM_Sleep();
	//BQ_Disable();
	Disable_ADC();
	W2000_Disable();
	BT_Disable();
	LCD_Disable();
	ext_ADC_Disable();
	ALFAT_Disable();
	I2C_Disable();
}

char Start_Sleep_Measurment()
{
	BQ_Startup();
	adc_init();
	I2C_Startup();
	ext_ADC_Startup();
	ALFAT_Startup();
	//
	if (Bike.Fault.all != 0)
	{
		return 0; //fail
	}
	return 1;
}

void HWI_WAKEINT()
{
	if(Check_Mode() != MODE_SLEEP)
	{
		watchdog_count = 0;
		Disable_WAKEINT();
		Event_post(EVT_Change_Mode,Event_Id_00);
	}
	else
	{
		Event_post(EVT_Sleep_M,Event_Id_00);
	}
}

void Init_WAKEINT()
{
    EALLOW;
    SysCtrlRegs.LPMCR0.bit.WDINTE = 1;		//allow watchdog to wake from standby
    SysCtrlRegs.SCSR = 2; 					//allow watchdog interrupt
    SysCtrlRegs.WDKEY = 0x0055;
    SysCtrlRegs.WDKEY = 0x00AA;				//reset watchdog counter
    SysCtrlRegs.WDCR = 0xAF;				//10101111
    										//clear WDFLAG
    										//enable watchdog
    										//watchdog check = 101
    										//wdclk = oscclk/512/64 about 1/4 of a second

    EDIS;
}
