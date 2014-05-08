/*
 * Charge.c
 *
 *  Created on: Nov 1, 2012
 *      Author: LordLaptop
 */

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
#include "eCAN.h"

extern w2000_Status W2000;

extern Bike_Status Bike;
void Charge()
{
	Bike.Mode = MODE_CHARGE;
	Bike.Fault.all = 0;
	Bike.Warning.all = 0;
	W2000.BusVoltage = 0;
	if(Event_pend(EVT_Change_Mode,Event_Id_00, Event_Id_NONE,BIOS_NO_WAIT) != 0)
	{
		Event_post(EVT_Change_Mode,Event_Id_00);
		return;
	}
	GPIO_Charge();
	if(Event_pend(EVT_Change_Mode,Event_Id_00, Event_Id_NONE,BIOS_NO_WAIT) != 0)
	{
		Event_post(EVT_Change_Mode,Event_Id_00);
		return;
	}
	if (Start_Charge_Peripheral() == 1 || GpioDataRegs.GPBDAT.A_FUALT_OVERRIDE_SW == FAULT_OVERRIDE_ON)
	{
		Close_Contactors();
	}
	Event_Logger("MODE CHANGE TO CHARGE");
}

void GPIO_Charge()
{
	//
	GpioDataRegs.GPCSET.C_FUEL_DOOR = 1;
	GpioDataRegs.GPCSET.C_3V3_MAIN = 1;
	GpioDataRegs.GPACLEAR.A_OV_FAULT = 1;
	GpioDataRegs.GPACLEAR.A_WARNING 	= 1;
	GpioDataRegs.GPACLEAR.A_FAULT = 1;
	GpioDataRegs.GPACLEAR.A_FAULT_OVERRIDE = 1;
	GpioDataRegs.GPASET.A_CHARGING = 1;
	GpioDataRegs.GPACLEAR.A_OV_WARNING = 1;
	GpioDataRegs.GPACLEAR.A_UV_FAULT= 1;
	GpioDataRegs.GPACLEAR.A_UV_WARNING= 1;
	GpioDataRegs.GPACLEAR.A_OT_FAULT= 1;
	GpioDataRegs.GPASET.B_12V_MAIN_CTRL = 1;
	GpioDataRegs.GPACLEAR.A_12V_AUX_CTRL = 1;
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

	if(GpioDataRegs.GPBDAT.A_FUALT_OVERRIDE_SW == FAULT_OVERRIDE_ON)
	{
		GpioDataRegs.GPASET.A_FAULT_OVERRIDE = 1;
	}
	else
	{
		GpioDataRegs.GPACLEAR.A_FAULT_OVERRIDE = 1;
	}
}

char Start_Charge_Peripheral()
{
	W2000_Startup();
	adc_init();
	BQ_Startup();
	BT_Startup();
	LCD_Startup();
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
