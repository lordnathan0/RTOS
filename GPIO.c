/*
 * GPIO.c
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

#include "main.h"

#include "GPIO.h"

#include "eCAN.h"

#include "bq_pack.h"


extern bq_pack_t bq_pack;
extern w2000_Status W2000;
extern Bike_Status Bike;
//
void GPIO_INIT()
{
	InitGpio(); //sets all as inputs and pull up

	EALLOW;
	//config outputs

	GpioDataRegs.GPBCLEAR.B_12V_LOW = 1;
	GpioCtrlRegs.GPBPUD.B_12V_LOW = 1;
	GpioCtrlRegs.GPBDIR.B_12V_LOW = 1;

	GpioDataRegs.GPBCLEAR.B_OT_WARNING= 1;
	GpioCtrlRegs.GPBPUD.B_OT_WARNING = 1;
	GpioCtrlRegs.GPBDIR.B_OT_WARNING = 1;

	GpioDataRegs.GPCSET.C_FUEL_DOOR= 1;
	GpioCtrlRegs.GPCPUD.C_FUEL_DOOR = 1;
	GpioCtrlRegs.GPCDIR.C_FUEL_DOOR = 1;

	GpioDataRegs.GPCCLEAR.C_3V3_MAIN = 1; //MAKE SURE THIS IS CORRECT!!!!!!
	GpioCtrlRegs.GPCPUD.C_3V3_MAIN = 1;
	GpioCtrlRegs.GPCDIR.C_3V3_MAIN = 1;

	GpioDataRegs.GPACLEAR.A_MAIN_CTRL = 1;
	GpioCtrlRegs.GPAPUD.A_MAIN_CTRL = 1;
	GpioCtrlRegs.GPADIR.A_MAIN_CTRL = 1;

	GpioDataRegs.GPACLEAR.A_PRECHARGE_CTRL = 1;
	GpioCtrlRegs.GPAPUD.A_PRECHARGE_CTRL = 1;
	GpioCtrlRegs.GPADIR.A_PRECHARGE_CTRL = 1;

	GpioDataRegs.GPASET.B_12V_MAIN_CTRL = 1;
	GpioCtrlRegs.GPAPUD.B_12V_MAIN_CTRL = 1;
	GpioCtrlRegs.GPADIR.B_12V_MAIN_CTRL = 1;

	GpioDataRegs.GPACLEAR.A_12V_AUX_CTRL = 1;
	GpioCtrlRegs.GPAPUD.A_12V_AUX_CTRL = 1;
	GpioCtrlRegs.GPADIR.A_12V_AUX_CTRL = 1;

	GpioDataRegs.GPACLEAR.A_OV_FAULT = 1;
	GpioCtrlRegs.GPAPUD.A_OV_FAULT = 1;
	GpioCtrlRegs.GPADIR.A_OV_FAULT = 1;

	GpioDataRegs.GPACLEAR.A_SIREN_CTRL = 1;
	GpioCtrlRegs.GPAPUD.A_SIREN_CTRL = 1;
	GpioCtrlRegs.GPADIR.A_SIREN_CTRL = 1;

	GpioDataRegs.GPACLEAR.A_WARNING = 1;
	GpioCtrlRegs.GPAPUD.A_WARNING = 1;
	GpioCtrlRegs.GPADIR.A_WARNING = 1;

	GpioDataRegs.GPACLEAR.A_FAULT = 1;
	GpioCtrlRegs.GPAPUD.A_FAULT = 1;
	GpioCtrlRegs.GPADIR.A_FAULT = 1;

	GpioDataRegs.GPACLEAR.A_FAULT_OVERRIDE = 1;
	GpioCtrlRegs.GPAPUD.A_FAULT_OVERRIDE = 1;
	GpioCtrlRegs.GPADIR.A_FAULT_OVERRIDE = 1;

	GpioDataRegs.GPACLEAR.A_SAFETY_INTERLOCK = 1;
	GpioCtrlRegs.GPAPUD.A_SAFETY_INTERLOCK = 1;
	GpioCtrlRegs.GPADIR.A_SAFETY_INTERLOCK = 1;
	if (GpioDataRegs.GPBDAT.B_12V_SENSE == 1)
	{
		GpioDataRegs.GPASET.A_SAFETY_INTERLOCK = 1;
	}
	else
	{
		GpioDataRegs.GPACLEAR.A_SAFETY_INTERLOCK = 1;
	}

	GpioDataRegs.GPACLEAR.A_CHARGING = 1;
	GpioCtrlRegs.GPAPUD.A_CHARGING = 1;
	GpioCtrlRegs.GPADIR.A_CHARGING = 1;

	GpioDataRegs.GPACLEAR.A_OV_WARNING = 1;
	GpioCtrlRegs.GPAPUD.A_OV_WARNING = 1;
	GpioCtrlRegs.GPADIR.A_OV_WARNING = 1;

	GpioDataRegs.GPACLEAR.A_UV_FAULT = 1;
	GpioCtrlRegs.GPAPUD.A_UV_FAULT = 1;
	GpioCtrlRegs.GPADIR.A_UV_FAULT = 1;

	GpioDataRegs.GPACLEAR.A_UV_WARNING = 1;
	GpioCtrlRegs.GPAPUD.A_UV_WARNING = 1;
	GpioCtrlRegs.GPADIR.A_UV_WARNING = 1;

	GpioDataRegs.GPACLEAR.A_OT_FAULT = 1;
	GpioCtrlRegs.GPAPUD.A_OT_FAULT = 1;
	GpioCtrlRegs.GPADIR.A_OT_FAULT = 1;


	//FOR ALFAT
	GpioDataRegs.GPACLEAR.bit.GPIO24 = 1;
	GpioCtrlRegs.GPAPUD.bit.GPIO24 = 1;
	GpioCtrlRegs.GPADIR.bit.GPIO24 = 1;

	GpioDataRegs.GPACLEAR.bit.GPIO10 = 1;
	GpioCtrlRegs.GPAPUD.bit.GPIO10 = 1;
	GpioCtrlRegs.GPADIR.bit.GPIO10 = 0;

	//config inputs

	GpioCtrlRegs.GPAQSEL1.A_ALARM_SILENCE_SW = 1; //push button Qualification
	GpioCtrlRegs.GPAPUD.A_ALARM_SILENCE_SW = 1;

	GpioCtrlRegs.GPBPUD.B_12V_SENSE = 1;


	GpioCtrlRegs.GPAPUD.A_ON_SW = 0;
	GpioCtrlRegs.GPAPUD.A_BORING_SW = 0;
	GpioCtrlRegs.GPBPUD.A_FUALT_OVERRIDE_SW = 0;
	GpioCtrlRegs.GPAPUD.A_CHARGE_SW = 0;


	//config XINTs
	// 0 and 2 is falling
	// 1 is raising
	//3 is both

//	GpioCtrlRegs.GPACTRL.bit.QUALPRD0 = 0xff;
//	GpioCtrlRegs.GPAQSEL1.A_ON_SW = 2;
//	GpioCtrlRegs.GPAQSEL1.A_CHARGE_SW = 2;
//	GpioCtrlRegs.GPAQSEL1.A_BORING_SW = 2;
//	GpioCtrlRegs.GPAQSEL1.A_FUALT_OVERRIDE_SW = 2;

	//estop
	GpioIntRegs.GPIOXINT6SEL.bit.GPIOSEL=58;    // Connect GPIO 58 to XINT6
	XIntruptRegs.XINT6CR.bit.ENABLE=1;        // Enable XINT6
  	XIntruptRegs.XINT6CR.bit.POLARITY = 3; //BOTH edge
  	//fault override
	GpioIntRegs.GPIOXINT7SEL.bit.GPIOSEL=59;    // Connect GPIO 59 to XINT7
	XIntruptRegs.XINT7CR.bit.ENABLE=1;        // Enable XINT7
  	XIntruptRegs.XINT7CR.bit.POLARITY = 3; //BOTH edge
  	//on_sw
	GpioIntRegs.GPIOXINT1SEL.bit.GPIOSEL=0;    // Connect GPIO 0 to XINT1
	XIntruptRegs.XINT1CR.bit.ENABLE=1;        // Enable XINT1
  	XIntruptRegs.XINT1CR.bit.POLARITY = 3; //both edge
  	//charge_sw
	GpioIntRegs.GPIOXINT2SEL.bit.GPIOSEL= 6;    // Connect GPIO 6 to XINT2
	XIntruptRegs.XINT2CR.bit.ENABLE=1;        // Enable XINT2
  	XIntruptRegs.XINT2CR.bit.POLARITY = 3; //both edge

    EDIS;

    GpioDataRegs.GPASET.B_12V_MAIN_CTRL = 1;
    GpioDataRegs.GPASET.A_UV_FAULT = 1;
    GpioDataRegs.GPBSET.B_12V_LOW = 1;
    GpioDataRegs.GPBSET.B_OT_WARNING= 1;
    GpioDataRegs.GPASET.A_OV_FAULT = 1;
    GpioDataRegs.GPASET.A_WARNING = 1;
    GpioDataRegs.GPASET.A_FAULT = 1;
    GpioDataRegs.GPASET.A_FAULT_OVERRIDE = 1;
    GpioDataRegs.GPASET.A_SAFETY_INTERLOCK = 1;
    GpioDataRegs.GPASET.A_CHARGING = 1;
    GpioDataRegs.GPASET.A_OV_WARNING = 1;
    GpioDataRegs.GPASET.A_UV_WARNING = 1;
    GpioDataRegs.GPASET.A_OT_FAULT = 1;

    Task_sleep(20000);

    GpioDataRegs.GPACLEAR.B_12V_MAIN_CTRL = 1;
    GpioDataRegs.GPBCLEAR.B_12V_LOW = 1;
    GpioDataRegs.GPBCLEAR.B_OT_WARNING= 1;
    GpioDataRegs.GPACLEAR.A_OV_FAULT = 1;
    GpioDataRegs.GPACLEAR.A_WARNING = 1;
    GpioDataRegs.GPACLEAR.A_FAULT = 1;
    GpioDataRegs.GPACLEAR.A_FAULT_OVERRIDE = 1;
    GpioDataRegs.GPACLEAR.A_SAFETY_INTERLOCK = 1;
    GpioDataRegs.GPACLEAR.A_CHARGING = 1;
    GpioDataRegs.GPACLEAR.A_OV_WARNING = 1;
    GpioDataRegs.GPACLEAR.A_UV_WARNING = 1;
    GpioDataRegs.GPACLEAR.A_OT_FAULT = 1;
}

//
char Check_Mode()
{
	if (GpioDataRegs.GPADAT.A_CHARGE_SW == 1)
	{
		if(GpioDataRegs.GPADAT.A_ON_SW == 1)
		{
			//CHARGE:ON, ON:ON
			//ERROR
			return MODE_SLEEP;
		}
		else
		{
			//CHARGE:ON, ON:OFF
			//CHARGE
			return MODE_RACE;
		}
	}
	else
	{
		if(GpioDataRegs.GPADAT.A_ON_SW == 1)
		{
			//CHARGE:OFF, ON:ON
			//CHARGE
			return MODE_CHARGE;
		}
		else
		{
			//CHARGE:OFF, ON:OFF
			//STORE
			return ERROR;
		}
	}
}
//
void HWI_12v_Sense()
{
	if (GpioDataRegs.GPBDAT.B_12V_SENSE == 0)
	{
		Disconnect_Contactors();
		GpioDataRegs.GPASET.A_SAFETY_INTERLOCK = 1;
		Bike.Fault.bit.estop = 1;
	}
	else
	{
		GpioDataRegs.GPACLEAR.A_SAFETY_INTERLOCK = 1;
		Bike.Fault.bit.estop = 0;
	}
}

void HWI_Charge_Sense()
{
	if(Check_Mode() != Bike.Mode)
	{
		Event_post(EVT_Change_Mode,Event_Id_00);
	}
}

void HWI_ON_SW()
{
	char mode = Check_Mode();
	if(Check_Mode() != Bike.Mode)
	{
		Event_post(EVT_Change_Mode,Event_Id_00);
	}
}

void HWI_Fault_Override()
{
	if(GpioDataRegs.GPBDAT.A_FUALT_OVERRIDE_SW == FAULT_OVERRIDE_ON)
	{
		GpioDataRegs.GPASET.A_FAULT_OVERRIDE = 1;
		Event_post(EVT_Error,Event_Id_00);
	}
	else
	{
		GpioDataRegs.GPACLEAR.A_FAULT_OVERRIDE = 1;
	}
}

//
void Disconnect_Contactors()
{
		if(Bike.Mode == MODE_RACE || Bike.Mode == MODE_SLEEP)
		{
			if(GpioDataRegs.GPBDAT.B_12V_SENSE == 1)
			{
				Task_sleep(FUEL_DOOR_WAIT);
			}
			GpioDataRegs.GPCSET.C_FUEL_DOOR  = 1; //stop drive
		}
		if(Bike.Mode == MODE_CHARGE)
		{
			GpioDataRegs.GPCSET.C_FUEL_DOOR  = 1; //stop drive
			GpioDataRegs.GPASET.C_CHARGER_SW = 1; //check if this is correct
		}
		GpioDataRegs.GPACLEAR.A_PRECHARGE_CTRL  = 1;
		GpioDataRegs.GPACLEAR.A_MAIN_CTRL  = 1;
}

void Close_Contactors()
{
	if (GpioDataRegs.GPBDAT.A_FUALT_OVERRIDE_SW != FAULT_OVERRIDE_ON)
	{
		if(Bike.Fault.all == 0)
		{
			GpioDataRegs.GPASET.A_PRECHARGE_CTRL  = 1;
			if(Event_pend(EVT_Change_Mode,Event_Id_00,Event_Id_NONE, PRECHARGE_TIME) != 0) //tenth of a second wait
			{
				GpioDataRegs.GPACLEAR.A_PRECHARGE_CTRL  = 1;
				Event_post(EVT_Change_Mode,Event_Id_00);
				return;
			}
			if((Bike.Fault.all == 0) && (W2000.BusVoltage  > PRECHARGE_VOLT))
			{
				GpioDataRegs.GPASET.A_MAIN_CTRL  = 1;
				GpioDataRegs.GPACLEAR.A_PRECHARGE_CTRL  = 1;
			}
			else
			{
				Bike.Fault.bit.precharge = 1;
				GpioDataRegs.GPACLEAR.A_PRECHARGE_CTRL  = 1;
			}
		}
		if(Bike.Mode == MODE_RACE)
		{
			GpioDataRegs.GPCCLEAR.C_FUEL_DOOR  = 1; //start drive
		}
		if(Bike.Mode == MODE_CHARGE)
		{
			GpioDataRegs.GPCSET.C_FUEL_DOOR  = 1; //stop drive
			GpioDataRegs.GPACLEAR.C_CHARGER_SW = 1; //check if this is correct
		}
	}
	else
	{
		GpioDataRegs.GPASET.A_PRECHARGE_CTRL  = 1;
		if(Event_pend(EVT_Change_Mode,Event_Id_00,Event_Id_NONE, PRECHARGE_TIME) != 0) //tenth of a second wait
		{
			GpioDataRegs.GPACLEAR.A_PRECHARGE_CTRL = 1;
			Event_post(EVT_Change_Mode,Event_Id_00);
			return;
		}
		if((W2000.BusVoltage  > PRECHARGE_VOLT))
		{
			GpioDataRegs.GPACLEAR.A_PRECHARGE_CTRL = 1;
			GpioDataRegs.GPASET.A_MAIN_CTRL  = 1;
		}
		else
		{
			Bike.Fault.bit.precharge = 1;
			GpioDataRegs.GPACLEAR.A_PRECHARGE_CTRL  = 1;
		}
		if(Bike.Mode == MODE_RACE)
		{
			GpioDataRegs.GPCCLEAR.C_FUEL_DOOR  = 1; //start drive
		}
		if(Bike.Mode == MODE_CHARGE)
		{
			GpioDataRegs.GPASET.C_CHARGER_SW = 1; //check if this is correct
			GpioDataRegs.GPACLEAR.C_CHARGER_SW = 1; //check if this is correct
		}
	}

}
