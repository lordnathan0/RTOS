/*
 * Bluetooth.c
 *
 *  Created on: Sep 13, 2012
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

#include "Bluetooth.h"

#include <stdlib.h>

#include "bq_pack.h"

#include "main.h"

#include "eCAN.h"

#include "ALFAT.h"

#include "ext_adc.h"

extern bq_pack_t bq_pack;

extern w2000_Status W2000;
extern Bike_Status Bike;
extern ext_ADC_st ext_ADC_status;

Bluetooth_Status BT;

// GPIO28 (SCIRXDA)
// GPIO29 (SCITXDA)

void BT_Startup()
{
    InitSciaGpio();

    scia_fifo_init();  // Init SCI-A

    Clock_start(BT_CLOCK);

    BT.init = 1;
}

void BT_Disable()
{
	BT.init = 0;
	Clock_stop(BT_CLOCK);
}

Void CLK_BT(UArg a0, UArg a1)
{
	Event_post(EVT_BT_CLK,Event_Id_00);
}

unsigned short determineCell(int dev, int cellOfDev)
{
	//Finds the actual cell number and returns it
	unsigned short resultCell = 0;
	int i;
	for(i=0; i < dev; i++)
	{
		resultCell = resultCell + (i*bq_pack.bq_devs[i].cell_count);
	}
	resultCell = resultCell + cellOfDev;
	return resultCell;
}

void check_low_cells()
{
	int i, j;
	// Displays to Bluetooth all cells of voltage under the threshold in format (DEV-CELL-VOLTAGE)
	// Does not add endlines
	char msg[25];
	char printLabel = '1';

	for (i=0; i<NUMBER_OF_BQ_DEVICES; i++)
	{
		for(j=0; j<bq_pack.bq_devs[i].cell_count; j++)
		{
			if(Bike.Mode == MODE_RACE)
			{
				if(bq_pack.bq_devs[i].cell_voltage[j] <= UV_WARNING_THRESHOLD_RACE)
				{
					if(printLabel == '1')
					{
						BT_msg("Low Cells: \0");
					}
					printLabel = '0';
					BT_msg("[\0");

					ltoa(determineCell(i,j),msg);
					BT_msg(msg);
					BT_msg("-\0");

					ltoa(bq_pack.bq_devs[i].cell_voltage[j],msg);

					BT_msg(msg);
					BT_msg("],\0");
				}
			}
			if(Bike.Mode == MODE_CHARGE)
			{
				if(bq_pack.bq_devs[i].cell_voltage[j] <= UV_WARNING_THRESHOLD_CHARGE)
				{
					if(printLabel == '1')
					{
						BT_msg("Low Cells: \0");
					}
					printLabel = '0';
					BT_msg("[\0");

					ltoa(determineCell(i,j),msg);
					BT_msg(msg);
					BT_msg("-\0");

					ltoa(bq_pack.bq_devs[i].cell_voltage[j],msg);

					BT_msg(msg);
					BT_msg("],\0");

				}
			}
			if(Bike.Mode == MODE_SLEEP)
			{
				if(bq_pack.bq_devs[i].cell_voltage[j] <= UV_WARNING_THRESHOLD_SLEEP)
				{
					if(printLabel == '1')
					{
						BT_msg("Low Cells: \0");
					}
					printLabel = '0';
					BT_msg("[\0");

					ltoa(determineCell(i,j),msg);
					BT_msg(msg);
					BT_msg("-\0");

					ltoa(bq_pack.bq_devs[i].cell_voltage[j],msg);

					BT_msg(msg);
					BT_msg("],\0");
				}
			}
		}
	}
}
void check_high_cells()
{
	// Displays to Bluetooth all cells of voltage under the threshold in format (DEV-CELL-VOLTAGE)
	// Does not add endlines
	char msg[25];
	char printLabel = '1';

	int i, j;
	for (i=0; i<NUMBER_OF_BQ_DEVICES; i++)
	{
		for(j=0; j<MAX_CELLS_NUMBER_IN_BQ; j++)
		{
			if(Bike.Mode == MODE_RACE)
			{
				if(bq_pack.bq_devs[i].cell_voltage[j] >= OV_WARNING_THRESHOLD_RACE)
				{
					if(printLabel == '1')
					{
						BT_msg("High Cells: \0");
					}
					printLabel = '0';
					BT_msg("[\0");

					ltoa(determineCell(i,j),msg);
					BT_msg(msg);
					BT_msg("-\0");

					ltoa(bq_pack.bq_devs[i].cell_voltage[j],msg);

					BT_msg(msg);
					BT_msg("],\0");
				}
			}
			if(Bike.Mode == MODE_CHARGE)
			{
				if(bq_pack.bq_devs[i].cell_voltage[j] >= OV_WARNING_THRESHOLD_CHARGE)
				{
					if(printLabel == '1')
					{
						BT_msg("High Cells: \0");
					}
					printLabel = '0';
					BT_msg("[\0");

					ltoa(determineCell(i,j),msg);
					BT_msg(msg);
					BT_msg("-\0");

					ltoa(bq_pack.bq_devs[i].cell_voltage[j],msg);

					BT_msg(msg);
					BT_msg("],\0");
				}
			}
			if(Bike.Mode == MODE_SLEEP)
			{
				if(bq_pack.bq_devs[i].cell_voltage[j] >= OV_WARNING_THRESHOLD_SLEEP)
				{
					if(printLabel == '1')
					{
						BT_msg("High Cells: \0");
					}
					printLabel = '0';
					BT_msg("[\0");

					ltoa(determineCell(i,j),msg);
					BT_msg(msg);
					BT_msg("-\0");

					ltoa(bq_pack.bq_devs[i].cell_voltage[j],msg);

					BT_msg(msg);
					BT_msg("],\0");
				}
			}
		}
	}
}
void TSK_BT(UArg a0, UArg a1)
{

short count = 0;
while (1)
{
	if(count > 9)
	{
		BT_msg(" Highest and High Volt Cells | Lowest and Low Volt Cells |\0");
		BT_msg(" Highest Temp | Motor Velocity | EXT_ADC Current |\0");
		BT_msg(" Bus Volt | MotorTemp | PhaseATemp | BusAmphour | Odometer |\0");
		BT_msg(" Faults | Warnings |\0");
		BT_msg("\n\0");
		count = 0;
	}
	char msg[25];
	Event_pend(EVT_BT_CLK,Event_Id_00, Event_Id_NONE,BIOS_WAIT_FOREVER);

	//Displays Highest Cell Volt and all high cells
	ltoa(bq_pack.highest_cell_volts,msg);
	BT_msg(msg);
	BT_msg(" \0");
	check_high_cells();

	BT_msg(separator);
	//Displays Lowest Cell Volt and all low cells
	ltoa(bq_pack.lowest_cell_volts,msg);
	BT_msg(msg);
	BT_msg(" \0");
	check_low_cells();

	BT_msg(separator);
	//Highest Temp
	ltoa(bq_pack.highest_temp,msg);
	BT_msg(msg);

	BT_msg(separator);
	//MOTOR VELOCITY
	long extra = W2000.MotorVelocity;
	ltoa(W2000.MotorVelocity,msg);
	BT_msg(msg);
	double extra1 = W2000.MotorVelocity - extra;
	extra = extra1 * 1000;
	BT_msg(".");
	ltoa(extra,msg);
	BT_msg(msg);

	BT_msg(separator);
	//ADC Current
	double2string(ext_ADC_status.current,1,msg);
	BT_msg(msg);

	BT_msg(separator);
	//Bus Volt
	ltoa(W2000.BusCurrent,msg);
	BT_msg(msg);

	BT_msg(separator);
	// Motor Temp
	ltoa(W2000.MotorTemp,msg);
	BT_msg(msg);

	BT_msg(separator);
	// PhaseATemp
	ltoa(W2000.PhaseATemp, msg);
	BT_msg(msg);

	BT_msg(separator);
	//BusAmphour
	ltoa(W2000.BusAmphour,msg);
	BT_msg(msg);

	BT_msg(separator);
	//Odometer
	ltoa(W2000.Odometer, msg);
	BT_msg(msg);

	BT_msg(separator);

	// Faults
	if(Bike.Fault.all != 0)
	{
		BT_msg("Faults: \0");
	}
	else
	{
		BT_msg("No Faults \0");
	}
	if(Bike.Fault.bit.estop != 0)
	{
		BT_msg("EStop \0");
	}
	if(Bike.Fault.bit.Charge_OC != 0)
	{
		BT_msg("Charge_OC \0");
	}
	if(Bike.Fault.bit.Dis_OC != 0)
	{
		BT_msg("Dis_OC \0");
	}
	if(Bike.Fault.bit.Switch_Error != 0)
	{
		BT_msg("Switch_Error \0");
	}
	if(Bike.Fault.bit.BMM_Init != 0)
	{
		BT_msg("BMM_Init \0");
	}

	BT_msg(separator);

	// Warnings
	if(Bike.Warning.all != 0)
	{
		BT_msg("Warnings: \0");
	}
	else
	{
		BT_msg("No warnings \0");
	}
	if(Bike.Warning.bit.BMM_loss != 0)
	{
		BT_msg("BMM_Loss \0");
	}
	if(Bike.Warning.bit.Capacity != 0)
	{
		BT_msg("Capacity \0");
	}
	if(Bike.Warning.bit.Charage_OC != 0)
	{
		BT_msg("Charge_OC \0");
	}
	if(Bike.Warning.bit.Dis_OC != 0)
	{
		BT_msg("Dis_OC \0");
	}
	if(Bike.Warning.bit.SD_Startup != 0)
	{
		BT_msg("SD_Startup \0");
	}
	if(Bike.Warning.bit.SD_Write != 0)
	{
		BT_msg("SD_Write \0");
	}
	if(Bike.Warning.bit.current_sense_error != 0)
	{
		BT_msg("Current_Sense_Error \0");
	}
	if(Bike.Warning.bit.w2000_off != 0)
	{
		BT_msg("W2000_Off \0");
	}
	BT_msg(separator);
	BT_msg("\n\0");
	count++;
	}

}

void scia_fifo_init()
{
   SciaRegs.SCICCR.all =0x0007;   // 1 stop bit,  No loopback
                                  // No parity,8 char bits,
                                  // async mode, idle-line protocol
   SciaRegs.SCICTL1.all =0x0003;  // enable TX, RX, internal SCICLK,
                                  // Disable RX ERR, SLEEP, TXWAKE
   SciaRegs.SCICTL2.bit.TXINTENA =1;
   SciaRegs.SCICTL2.bit.RXBKINTENA =1;
//   SciaRegs.SCIHBAUD = 0x0000;
//   SciaRegs.SCILBAUD = SCI_PRD;
   SciaRegs.SCIHBAUD    =0x0000;  // 9600 baud @LSPCLK = 37.5MHz.
   SciaRegs.SCILBAUD    =0x00F3;
//   SciaRegs.SCICCR.bit.LOOPBKENA = 1; // Enable loop back
   SciaRegs.SCIFFTX.all=0xC028;
   SciaRegs.SCIFFRX.all=0x0028;
   SciaRegs.SCIFFCT.all=0x00;

   SciaRegs.SCICTL1.all =0x0023;     // Relinquish SCI from Reset
   SciaRegs.SCIFFTX.bit.TXFIFOXRESET=1;
   SciaRegs.SCIFFRX.bit.RXFIFORESET=1;


}

unsigned char BT_msg(char *msg)
{
    int i;
    i = 0;
    while(msg[i] != '\0')
    {
        while(Send_SCIA(msg[i]) == 0 && BT.timeout_count < BLUETOOTH_TIMEOUT_COUNT);
        i++;
    }
    if(BT.timeout_count < BLUETOOTH_TIMEOUT_COUNT)
    {
    	return 0;
    }
    else
    {
    	return 1;
    }
}

unsigned char Send_SCIA(int a)
{
    SciaRegs.SCITXBUF=a;
    SciaRegs.SCIFFTX.bit.TXFFINTCLR=1;	// Clear SCI Interrupt flag
    unsigned char ret = Event_pend(EVT_SCIa_Tx,Event_Id_00, Event_Id_NONE,BLUETOOTH_TIMEOUT);
    if(ret == 0)
    {
    	BT.timeout_count++;
    }
    else
    {
    	BT.timeout_count = 0;
    }
	return ret;
}

 void HWI_SCIa_TxComplete(UArg a0, UArg a1)
{
    Event_post(EVT_SCIa_Tx,Event_Id_00);
}

 void HWI_SCIa_RxRecieved(UArg a0, UArg a1)
{
	 //interrupts when fifo is full
    Uint16 i;
	for(i=0;i<8;i++)
	{
	   BT.rdata[i]=SciaRegs.SCIRXBUF.all;	 // Read data
	}
	SciaRegs.SCIFFRX.bit.RXFFOVRCLR=1;   // Clear Overflow flag
	SciaRegs.SCIFFRX.bit.RXFFINTCLR=1;   // Clear Interrupt flag
}


