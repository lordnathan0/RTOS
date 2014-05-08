 // GPIO16 as SPI SIMO
 // GPIO17 as SPI SOMI
 // GPIO18 as SPI CLK
 // GPIO19 as SPI STE
// GPIO 48 Alert
// GPIO 49 Fault
// GPIO 62 DataReady
// GPIO 63 Cov
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

#include "main.h"

extern Bike_Status Bike;


extern bq_pack_t bq_pack;
char fualt;
char first_bq = 0;


void BQ_Startup()
{
	first_bq = 0;
    InitSpiaGpio();

    spi_fifo_init();

    InitBQ76PL536ports();

	init_BQ();



	if(Bike.Fault.bit.BMM_Init == 0)
	{
		Event_post(EVT_BQ,Event_Id_00);
		while (first_bq == 0)				//wait until first update is done
		{
			Task_sleep(5);
		}
		Clock_start(BQ_CLOCK);
		bq_pack.Init = 1;
	}
}

void BQ_Disable()
{
	//Bike.Fault.bit.BMM_Init = 1;
    GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 0; // Configure GPIO16 as SPISIMOA
    GpioCtrlRegs.GPAMUX2.bit.GPIO17 = 0; // Configure GPIO17 as SPISOMIA
    GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 0; // Configure GPIO18 as SPICLKA
    GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 0; // Configure GPIO19 as SPISTEA
	Clock_stop(BQ_CLOCK);
}

void init_BQ()
{
	//  InitMCU();

	  //Enable interrupts
//	  __bis_SR_register(GIE);

	int num = bq_pack_address_discovery();
	  if(num != NUMBER_OF_BQ_DEVICES)
	  {
		  Bike.Fault.bit.BMM_Init = 1;
	  }
	  else
	  {
	  //Initialize detected BQ76PL536 devices
	  if(bq_pack_init() == 0)
	  {
		  Bike.Fault.bit.BMM_Init = 0;
	  }
	  else
	  {
		  Bike.Fault.bit.BMM_Init = 1;
	  }
	  }
}


void HWI_BQFault(UArg a0, UArg a1)
{
	Event_post(EVT_BQFault,Event_Id_00);
}

void TSK_BQFault(UArg a0, UArg a1)
{

  char i;
  while (1)
  {
	  Event_pend(EVT_BQFault,Event_Id_00, Event_Id_NONE,BIOS_WAIT_FOREVER);
  //*Read BQ device for detailed status*/
  //*Read each device errors*/
	  if(bq_pack.Init == 1)
	  {
		  for(i=0;i<NUMBER_OF_BQ_DEVICES;i++)
		  {
	  //read device errors
			  bq_dev_read_errors(&bq_pack.bq_devs[i]);

	  /*Clear faults */
			  bq_dev_clear_faults(&bq_pack.bq_devs[i]);

	  /*Read each device temperature*/
			  bq_dev_read_temps(&bq_pack.bq_devs[i]);
		  }

	  //check the POT, CUV and COV conditions
	  CheckFaultConditions();
	  }


  }

}

void HWI_BQAlert(UArg a0, UArg a1)
{
	Event_post(EVT_BQAlert,Event_Id_00);
}

void TSK_BQAlert(UArg a0, UArg a1)
{

   char i;
   while (1)
   {
	   Event_pend(EVT_BQAlert,Event_Id_00, Event_Id_NONE,BIOS_WAIT_FOREVER);
	   if(bq_pack.Init == 1)
	   {
	   //*Read BQ device for detailed status*/
	   //*Read each device errors*/
		   for(i=0;i<NUMBER_OF_BQ_DEVICES;i++)
		   {

			   bq_dev_read_errors(&bq_pack.bq_devs[i]);

		//*Clear alerts */
			   bq_dev_clear_alerts(&bq_pack.bq_devs[i]);

		//*Read each device temperature*/
			   bq_dev_read_temps(&bq_pack.bq_devs[i]);

		   }

	  //*Signal error to the HOST*/
	  //check the POT, CUV and COV conditions
		   CheckFaultConditions();
	   }

   }
}

void RTSK_BQ_Event(UArg a0, UArg a1)
{
	Event_post(EVT_BQ,Event_Id_00);
}

void RTSK_BQ(UArg a0, UArg a1)
{
	while(1)
	{

		Event_pend(EVT_BQ,Event_Id_00, Event_Id_NONE,BIOS_WAIT_FOREVER);
  //Start Data processing
		if (Bike.Fault.bit.BMM_Init == 0)
		{
			bq_pack_start_conv();
		}
	}

}

void HWI_BQ_DRDY(UArg a0, UArg a1)
{
	Event_post(EVT_BQ_DRDY,Event_Id_00);
}

void TSK_BQ_DRDY(UArg a0, UArg a1)
{

	while (1)
	{
  ///*BQ76PL536-ADC conversion ready*/
  //Read data from BQ pack for overall system verification
		Event_pend(EVT_BQ_DRDY,Event_Id_00, Event_Id_NONE,BIOS_WAIT_FOREVER);

		update_bq_pack_data();

  //Analyze bq_pack data and perform required action (if necessary)
		BatteryPackManager();
		first_bq = 1;
	}
}

void InitBQ76PL536ports()
{

	EALLOW;
  //Inputs
  // I/O port control pad

  // Set pull up/down resistor configuration


  GpioCtrlRegs.GPBPUD.bit.GPIO48 = 0;
  GpioCtrlRegs.GPBPUD.bit.GPIO49 = 0;
  GpioCtrlRegs.GPBPUD.bit.GPIO62 = 0;


  // Set port direction as input


  GpioCtrlRegs.GPBDIR.bit.GPIO48 = 0;
  GpioCtrlRegs.GPBDIR.bit.GPIO49 = 0;
  GpioCtrlRegs.GPBDIR.bit.GPIO62 = 0;


  //SET UP Alert, fault, dataready interrupts ACCORDING TO PINs
  GpioIntRegs.GPIOXINT3SEL.bit.GPIOSEL=48;    // Connect GPIO 48 to XINT3
  XIntruptRegs.XINT3CR.bit.ENABLE=1;        // Enable XINT3
  XIntruptRegs.XINT3CR.bit.POLARITY = 1; //Raising edge

  GpioIntRegs.GPIOXINT4SEL.bit.GPIOSEL=49;    // Connect GPIO 49 to XINT4
  XIntruptRegs.XINT4CR.bit.ENABLE=1;        // Enable XINT4
  XIntruptRegs.XINT4CR.bit.POLARITY = 1; //raising edge

  GpioIntRegs.GPIOXINT5SEL.bit.GPIOSEL=62;    // Connect GPIO 62 to XINT5
  XIntruptRegs.XINT5CR.bit.ENABLE=1;        // Enable XINT5
  XIntruptRegs.XINT5CR.bit.POLARITY = 1; //raising edge


  //Output
  //I/O port control pad
  GpioDataRegs.GPASET.bit.GPIO19 = 1;
  GpioCtrlRegs.GPAPUD.bit.GPIO19 = 0;   // Disable pull-up on GPIO19 (SPISTEA)
  GpioCtrlRegs.GPADIR.bit.GPIO19 = 1;	// output
  GpioDataRegs.GPASET.bit.GPIO19 = 1;
  //Set pin to Logic Level 0


  //Set pull up/down resistor configuration
  GpioCtrlRegs.GPBPUD.bit.GPIO63 = 0;

  //Set port direction as output
  GpioCtrlRegs.GPBDIR.bit.GPIO63 = 1;

  EDIS;

}

void spi_fifo_init()
{
// Initialize SPI FIFO registers
   SpiaRegs.SPICCR.bit.SPISWRESET=0; // Reset SPI

  // SpiaRegs.SPICCR.all=0x001F;       //16-bit character, Loopback mode
   SpiaRegs.SPICCR.all=0x0007; 		//8-bit no loopback
 SpiaRegs.SPICTL.all=0x0017;       //Interrupt enabled, Master/Slave XMIT enabled/slave select enabled
//  SpiaRegs.SPICTL.all=0x001F;
   SpiaRegs.SPISTS.all=0x0000;
   SpiaRegs.SPIBRR = 127; //Baudrate is 250000
  // SpiaRegs.SPIBRR=0x0063;           // Baud rate
  // SpiaRegs.SPIFFTX.all=0xC028;      // Enable FIFO's, set TX FIFO level to 8 CHOOSE LEVEL ACCORDING TO APPLICATION
  //SpiaRegs.SPIFFRX.all=0x0028;      // Set RX FIFO level to 8

   SpiaRegs.SPIFFTX.all=0xC021;      // Enable FIFO's, set TX FIFO level to 1 CHOOSE LEVEL ACCORDING TO APPLICATION

   SpiaRegs.SPIFFRX.all=0x0021;      // Set RX FIFO level to 1
   SpiaRegs.SPIFFCT.all=0x00;

   SpiaRegs.SPIPRI.bit.FREE=1;

   SpiaRegs.SPICCR.bit.SPISWRESET=1;  // Enable SPI

   SpiaRegs.SPIFFTX.bit.TXFIFO=1;
  SpiaRegs.SPIFFTX.bit.SPIRST=1;
  SpiaRegs.SPIFFRX.bit.RXFIFORESET=1;

}
