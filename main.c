
/*
 *  ======== main.c ========
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

#include "spi_if.h"

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

extern ALFAT_st ALFAT_status;
Bike_Status Bike;
extern w2000_Status W2000;
extern Bluetooth_Status BT;
extern LCD_Status LCD_S;
extern ALFAT_st ALFAT_status;
extern ext_ADC_st ext_ADC_status;
extern ADC_st ADC;
extern bq_pack_t bq_pack;


void Initialization()
{
	//This function goes through all the include files and initializes
	//all the structs to zero (or null character)

	//the file being manipulated is commented before the code

//Main.h
	Bike.Mode= MODE_SLEEP;
	Bike.Fault.all = 0;
	Bike.Fault.bit.BMM_Init = 0;
	Bike.Fault.bit.Charge_OC = 0;
	Bike.Fault.bit.Dis_OC = 0;
	Bike.Fault.bit.Motor_Error = 0;
	Bike.Fault.bit.OT = 0;
	Bike.Fault.bit.OV = 0;
	Bike.Fault.bit.Switch_Error = 0;
	Bike.Fault.bit.UV = 0;
	Bike.Fault.bit.estop = 0;
	Bike.Fault.bit.lowv_bat = 0;

	Bike.Warning.all = 0;
	Bike.Warning.bit.BMM_loss= '\0';
	Bike.Warning.bit.Capacity = '\0';
	Bike.Warning.bit.Charage_OC = '\0';
	Bike.Warning.bit.Dis_OC = '\0';
	Bike.Warning.bit.OT = '\0';
	Bike.Warning.bit.OV = '\0';
	Bike.Warning.bit.SD_Startup = '\0';
	Bike.Warning.bit.SD_Write = '\0';
	Bike.Warning.bit.UV = '\0';
	Bike.Warning.bit.current_sense_error = '\0';
	Bike.Warning.bit.lowv_bat = '\0';
	Bike.Warning.bit.w2000_off = '\0';

//bq_pack.h
	bq_pack.Cell_Bal = 0;
	bq_pack.Init = 0;

	int i=0;
	while(i<NUMBER_OF_BQ_DEVICES)
	{
		bq_pack.bq_devs[i].GPAI = 0;
		bq_pack.bq_devs[i].Timestamp = 0;
		bq_pack.bq_devs[i].alert_status = 0;
		bq_pack.bq_devs[i].cell_bal = 0;
		bq_pack.bq_devs[i].cell_count = 6;
		bq_pack.bq_devs[i].cov_fault = '\0';
		bq_pack.bq_devs[i].crc_error_count = 0;
		bq_pack.bq_devs[i].cuv_fault = '\0';
		bq_pack.bq_devs[i].device_address = '\0';
		bq_pack.bq_devs[i].device_status = '\0';
		bq_pack.bq_devs[i].fault_status = '\0';
		bq_pack.bq_devs[i].temperature1 = 0;
		bq_pack.bq_devs[i].temperature1ratio = 0;
		bq_pack.bq_devs[i].temperature2 = 0;
		bq_pack.bq_devs[i].temperature2ratio = 0;
		bq_pack.bq_devs[i].timeout_count = 0;

		i++;
	}

	int z1=0;
	int z2=0;

	while (z1 < NUMBER_OF_BQ_DEVICES)
	{
		while (z2 < bq_pack.bq_devs[z1].cell_count)
		{
			bq_pack.bq_devs[z1].cell_voltage[z2] = 0;
			z2++;
		}
		z1++;
		z2 = 0;
	}

	bq_pack.error_status = 0;
	bq_pack.highest_cell_volts = 0;
	bq_pack.highest_temp = 0;
	bq_pack.lowest_cell_volts = 0;
	bq_pack.voltage = 0;
	bq_pack.highest_cell_num = 0;
	bq_pack.lowest_cell_num = 0;
	bq_pack.highest_temp_num = 0;

//eCAN.h
	W2000.BusAmphour = 0;
	W2000.init = '\0';
	W2000.Status = 0;
	W2000.BusCurrent = 0;
	W2000.BusVoltage = 0;
	W2000.MotorVelocity = 0;
	W2000.VehicleVelocity = 0;
	W2000.PhaseBcurrent = 0;
	W2000.PhaseCcurrent = 0;
	W2000.MotorVd = 0;
	W2000.MotorVq = 0;
	W2000.MotorId = 0;
	W2000.MotorIq = 0;
	W2000.MotorTemp = 0;
	W2000.PhaseATemp = 0;
	W2000.BusAmphour = 0;
	W2000.Odometer = 0;
	W2000.MotorCurrentCommand = 0;
	W2000.MotorVelocityCommand = 0;
	W2000.BusCurrentCommand = 0;

//bluetooth.h
	BT.Timestamp = 0;
	BT.init = '\0';
	BT.timeout_count = 0;
	//BT.rdata
	int x=0;
	while (x < 8)
	{
		BT.rdata[x] = 0;
		x++;
	}

//LCD.h
	LCD_S.Timestamp = 0;
	LCD_S.init = '\0';
	LCD_S.timeout_count = 0;
	//LCD_S.rdata
	int y=0;
	while (y < 8)
	{
		LCD_S.rdata[y] = 0;
		y++;
	}

//ALFAT.h
	ALFAT_status.Init = 0;
	ALFAT_status.error = 0;

//ext_adc.h
	ext_ADC_status.Init = 0;
	ext_ADC_status.current = 0;
	ext_ADC_status.status = 0;
	ext_ADC_status.coul = 0.0;

//ADC.h
	ADC.Init = 0;
	ADC.bat_12v = 0;

}


/*
 *  ======== taskFxn ========
 */
Void TSK_Powerup(UArg a0, UArg a1)
{

		//adc_init()
		//BQ_Startup();
		//W2000_Startup();
		//BT_Startup();
		//LCD_Startup();
		//I2C_Startup();
		//ALFAT_Startup();
		//ext_ADC_Startup();
	GPIO_INIT();
	Initialization();
	Event_post(EVT_Change_Mode,Event_Id_00);
}

void TSK_Change_Mode()
{
	while(1)
	{
		Event_pend(EVT_Change_Mode,Event_Id_00, Event_Id_NONE,BIOS_WAIT_FOREVER);
		switch (Check_Mode())
		{
		  case MODE_CHARGE:
			  Charge();
		    break;
		  case MODE_RACE:
			  Race();
		    break;
		  case MODE_SLEEP:
			  Sleep();
		    break;
		  case ERROR:
			 Bike.Fault.bit.Switch_Error = 1;
			 Event_post(EVT_Error,Event_Id_00);
		  	break;
		  }
	}
}


/*
 *  ======== main ========
 */
Void main()
{ 
    /*
     * use ROV->SysMin to view the characters in the circular buffer
     */
//    System_printf("enter main()\n");
	MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);

	InitSysCtrl();

	InitFlash();


    BIOS_start();        /* enable interrupts and start SYS/BIOS */
}

//
void TSK_ErrorHandler(UArg a0, UArg a1)
{
	//acts on current error status according to mode
	while (1)
	{
		Event_pend(EVT_Error,Event_Id_00, Event_Id_NONE,BIOS_WAIT_FOREVER);
		if(GpioDataRegs.GPBDAT.A_FUALT_OVERRIDE_SW == FAULT_OVERRIDE_ON)
		{
			GpioDataRegs.GPASET.A_FAULT_OVERRIDE;
		}
		switch (Bike.Mode)
		{
		case MODE_CHARGE:
			if (Bike.Fault.all != 0)
			{
				GpioDataRegs.GPASET.A_FAULT = 1;
				if (Bike.Fault.bit.BMM_Init)
				{
					if(GpioDataRegs.GPBDAT.A_FUALT_OVERRIDE_SW != FAULT_OVERRIDE_ON)
					{
						Disconnect_Contactors();
					}
				}
				if (Bike.Fault.bit.Charge_OC)
				{
					if(GpioDataRegs.GPBDAT.A_FUALT_OVERRIDE_SW != FAULT_OVERRIDE_ON)
					{
						Disconnect_Contactors();
					}
				}
				if (Bike.Fault.bit.Dis_OC)
				{
					if(GpioDataRegs.GPBDAT.A_FUALT_OVERRIDE_SW != FAULT_OVERRIDE_ON)
					{
						Disconnect_Contactors();
					}
				}
				if (Bike.Fault.bit.Motor_Error)
				{
					if(GpioDataRegs.GPBDAT.A_FUALT_OVERRIDE_SW != FAULT_OVERRIDE_ON)
					{
						Disconnect_Contactors();
					}
				}
				if (Bike.Fault.bit.OT)
				{
					GpioDataRegs.GPASET.A_OT_FAULT = 1;
					if(GpioDataRegs.GPBDAT.A_FUALT_OVERRIDE_SW != FAULT_OVERRIDE_ON)
					{
						Disconnect_Contactors();
					}
				}
				if (Bike.Fault.bit.OV)
				{
					GpioDataRegs.GPASET.A_OV_FAULT = 1;
					if(GpioDataRegs.GPBDAT.A_FUALT_OVERRIDE_SW != FAULT_OVERRIDE_ON)
					{
						Disconnect_Contactors();
					}
				}
				if (Bike.Fault.bit.Switch_Error)
				{
					if(GpioDataRegs.GPBDAT.A_FUALT_OVERRIDE_SW != FAULT_OVERRIDE_ON)
					{
						Disconnect_Contactors();
					}
				}
				if (Bike.Fault.bit.UV)
				{
					GpioDataRegs.GPASET.A_UV_FAULT  = 1;
					if(GpioDataRegs.GPBDAT.A_FUALT_OVERRIDE_SW != FAULT_OVERRIDE_ON)
					{
						Disconnect_Contactors();
					}
				}
				if (Bike.Fault.bit.estop)
				{
					GpioDataRegs.GPASET.C_CHARGER_SW = 1; //check if this is correct
					Disconnect_Contactors();
				}
				if (Bike.Fault.bit.lowv_bat)
				{
					GpioDataRegs.GPBSET.B_12V_LOW = 1;
					if(GpioDataRegs.GPBDAT.A_FUALT_OVERRIDE_SW != FAULT_OVERRIDE_ON)
					{
						Disconnect_Contactors();
					}
				}
				if(Bike.Fault.bit.precharge)
				{

				}
			}
			if (Bike.Warning.all != 0)
			{
				GpioDataRegs.GPASET.A_WARNING = 1;
				if (Bike.Warning.bit.BMM_loss)
				{
					if(GpioDataRegs.GPBDAT.A_FUALT_OVERRIDE_SW != FAULT_OVERRIDE_ON)
					{
						//Disconnect_Contactors();
					}
				}
				if (Bike.Warning.bit.Capacity)
				{
					if(GpioDataRegs.GPBDAT.A_FUALT_OVERRIDE_SW != FAULT_OVERRIDE_ON)
					{
						//Disconnect_Contactors();
					}
				}
				if (Bike.Warning.bit.Charage_OC)
				{
					if(GpioDataRegs.GPBDAT.A_FUALT_OVERRIDE_SW != FAULT_OVERRIDE_ON)
					{
						//Disconnect_Contactors();
					}
				}
				if (Bike.Warning.bit.Dis_OC)
				{
					if(GpioDataRegs.GPBDAT.A_FUALT_OVERRIDE_SW != FAULT_OVERRIDE_ON)
					{
						//Disconnect_Contactors();
					}
				}
				if (Bike.Warning.bit.OT)
				{
					GpioDataRegs.GPBSET.B_OT_WARNING = 1;
					if(GpioDataRegs.GPBDAT.A_FUALT_OVERRIDE_SW != FAULT_OVERRIDE_ON)
					{
						//Disconnect_Contactors();
					}
				}
				if (Bike.Warning.bit.OV)
				{
					GpioDataRegs.GPASET.A_OV_WARNING = 1;
					if(GpioDataRegs.GPBDAT.A_FUALT_OVERRIDE_SW != FAULT_OVERRIDE_ON)
					{
						//Disconnect_Contactors();
					}
				}
				if (Bike.Warning.bit.SD_Startup)
				{
					//GpioDataRegs.GPCSET.C_CHARGER_SW = 1; //check if this is correct
					//Disconnect_Contactors();
				}
				if (Bike.Warning.bit.SD_Write)
				{
					//GpioDataRegs.GPCSET.C_CHARGER_SW = 1; //check if this is correct
					//Disconnect_Contactors();
				}
				if (Bike.Warning.bit.UV)
				{
					GpioDataRegs.GPASET.A_UV_WARNING = 1;
					if(GpioDataRegs.GPBDAT.A_FUALT_OVERRIDE_SW != FAULT_OVERRIDE_ON)
					{
						//Disconnect_Contactors();
					}
				}
				if (Bike.Warning.bit.lowv_bat)
				{
					GpioDataRegs.GPBSET.B_12V_LOW = 1;
					if(GpioDataRegs.GPBDAT.A_FUALT_OVERRIDE_SW != FAULT_OVERRIDE_ON)
					{
						//Disconnect_Contactors();
					}
				}
			}
			else //no warnings
			{
				GpioDataRegs.GPACLEAR.A_WARNING = 1; //non-latching warning LED
			}
			break;
		case MODE_RACE:
			if (Bike.Fault.all != 0)
			{
				GpioDataRegs.GPASET.A_FAULT = 1;
				if (Bike.Fault.bit.BMM_Init)
				{
					if(GpioDataRegs.GPBDAT.A_FUALT_OVERRIDE_SW != FAULT_OVERRIDE_ON)
					{
						Disconnect_Contactors();
					}
				}
				if (Bike.Fault.bit.Charge_OC)
				{
					if(GpioDataRegs.GPBDAT.A_FUALT_OVERRIDE_SW != FAULT_OVERRIDE_ON)
					{
						Disconnect_Contactors();
					}
				}
				if (Bike.Fault.bit.Dis_OC)
				{
					if(GpioDataRegs.GPBDAT.A_FUALT_OVERRIDE_SW != FAULT_OVERRIDE_ON)
					{
						Disconnect_Contactors();
					}
				}
				if (Bike.Fault.bit.Motor_Error)
				{
					if(GpioDataRegs.GPBDAT.A_FUALT_OVERRIDE_SW != FAULT_OVERRIDE_ON)
					{
						Disconnect_Contactors();
					}
				}
				if (Bike.Fault.bit.OT)
				{
					GpioDataRegs.GPASET.A_OT_FAULT = 1;
					if(GpioDataRegs.GPBDAT.A_FUALT_OVERRIDE_SW != FAULT_OVERRIDE_ON)
					{
						Disconnect_Contactors();
					}
				}
				if (Bike.Fault.bit.OV)
				{
					GpioDataRegs.GPASET.A_OV_FAULT = 1;
					if(GpioDataRegs.GPBDAT.A_FUALT_OVERRIDE_SW != FAULT_OVERRIDE_ON)
					{
						Disconnect_Contactors();
					}
				}
				if (Bike.Fault.bit.Switch_Error)
				{
					if(GpioDataRegs.GPBDAT.A_FUALT_OVERRIDE_SW != FAULT_OVERRIDE_ON)
					{
						Disconnect_Contactors();
					}
				}
				if (Bike.Fault.bit.UV)
				{
					GpioDataRegs.GPASET.A_UV_FAULT  = 1;
					if(GpioDataRegs.GPBDAT.A_FUALT_OVERRIDE_SW != FAULT_OVERRIDE_ON)
					{
						Disconnect_Contactors();
					}
				}
				if (Bike.Fault.bit.estop)
				{
					if(GpioDataRegs.GPBDAT.A_FUALT_OVERRIDE_SW != FAULT_OVERRIDE_ON)
					{
						Disconnect_Contactors();
					}
				}
				if (Bike.Fault.bit.lowv_bat)
				{
					GpioDataRegs.GPBSET.B_12V_LOW = 1;
					if(GpioDataRegs.GPBDAT.A_FUALT_OVERRIDE_SW != FAULT_OVERRIDE_ON)
					{
						Disconnect_Contactors();
					}
				}
			}
			if (Bike.Warning.all != 0)
			{
				GpioDataRegs.GPASET.A_WARNING = 1;
				if (Bike.Warning.bit.BMM_loss)
				{

				}
				if (Bike.Warning.bit.Capacity)
				{

				}
				if (Bike.Warning.bit.Charage_OC)
				{

				}
				if (Bike.Warning.bit.Dis_OC)
				{

				}
				if (Bike.Warning.bit.OT)
				{
					GpioDataRegs.GPBSET.B_OT_WARNING = 1;
				}
				if (Bike.Warning.bit.OV)
				{
					GpioDataRegs.GPASET.A_OV_WARNING = 1;
				}
				if (Bike.Warning.bit.SD_Startup)
				{

				}
				if (Bike.Warning.bit.SD_Write)
				{

				}
				if (Bike.Warning.bit.UV)
				{
					GpioDataRegs.GPASET.A_UV_WARNING = 1;
				}
				if (Bike.Warning.bit.lowv_bat)
				{
					GpioDataRegs.GPBSET.B_12V_LOW = 1;
				}
			}
			else //no warnings
			{
				GpioDataRegs.GPACLEAR.A_WARNING = 1; //non-latching warning LED
			}
			break;
		case MODE_SLEEP:
			if (Bike.Fault.all != 0)
			{
				GpioDataRegs.GPASET.A_FAULT = 1;
				if (Bike.Fault.bit.BMM_Init)
				{

				}
				if (Bike.Fault.bit.Charge_OC)
				{

				}
				if (Bike.Fault.bit.Dis_OC)
				{

				}
				if (Bike.Fault.bit.Motor_Error)
				{

				}
				if (Bike.Fault.bit.OT)
				{
					GpioDataRegs.GPASET.A_OT_FAULT = 1;
				}
				if (Bike.Fault.bit.OV)
				{
					GpioDataRegs.GPASET.A_OV_FAULT = 1;
				}
				if (Bike.Fault.bit.Switch_Error)
				{

				}
				if (Bike.Fault.bit.UV)
				{
					GpioDataRegs.GPASET.A_UV_FAULT  = 1;
				}
				if (Bike.Fault.bit.estop)
				{

				}
				if (Bike.Fault.bit.lowv_bat)
				{
					GpioDataRegs.GPBSET.B_12V_LOW = 1;
				}
			}
			if (Bike.Warning.all != 0)
			{
				GpioDataRegs.GPASET.A_WARNING = 1;
				if (Bike.Warning.bit.BMM_loss)
				{

				}
				if (Bike.Warning.bit.Capacity)
				{

				}
				if (Bike.Warning.bit.Charage_OC)
				{

				}
				if (Bike.Warning.bit.Dis_OC)
				{

				}
				if (Bike.Warning.bit.OT)
				{
					GpioDataRegs.GPBSET.B_OT_WARNING = 1;
				}
				if (Bike.Warning.bit.OV)
				{
					GpioDataRegs.GPASET.A_OV_WARNING = 1;
				}
				if (Bike.Warning.bit.SD_Startup)
				{

				}
				if (Bike.Warning.bit.SD_Write)
				{

				}
				if (Bike.Warning.bit.UV)
				{
					GpioDataRegs.GPASET.A_UV_WARNING = 1;
				}
				if (Bike.Warning.bit.lowv_bat)
				{
					GpioDataRegs.GPBSET.B_12V_LOW = 1;
				}
			}
			else //no warnings
			{
				GpioDataRegs.GPACLEAR.A_WARNING = 1; //non-latching warning LED
			}
			break;
		}
	}
}
