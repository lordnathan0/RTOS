/*
 * eCAN.c
 */




#include <xdc/std.h>

#include <xdc/runtime/System.h>

#include <ti/sysbios/BIOS.h>

#include <ti/sysbios/knl/Queue.h>

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

#include "eCAN.h"


w2000_Status W2000;
extern Bike_Status Bike;
unsigned long read;
unsigned long CAN_cache[W2000_LOG_RATE];
unsigned int cache_counter=0;

void W2000_Startup()
{
	InitECanGpio();

	InitECana();

    eCAN_init();

    W2000_Full_Read();

//    W2000.init = 1;
}

void W2000_Disable()
{

	W2000.init = 0;
	EDIS;

	GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 0;
	GpioCtrlRegs.GPAMUX2.bit.GPIO30 = 0;
	ECanaRegs.CANME.all = 0;	//disable mailboxes
	EALLOW;

}

void W2000_Full_Read()
{
	int i = 0;
	read = 0;
	//	while (read != 839 && i < PACKET_WAIT_TIME)			//THIS IS FOR TESTING
	while (read != 0xff && i < PACKET_WAIT_TIME) 		//wait until all messeges are read once
	{
		Task_sleep(5);
		i++;
	}
	if (i == PACKET_WAIT_TIME)
	{
		W2000.init = 0;
		Bike.Warning.bit.w2000_off = 1;
		Event_post(EVT_Error,Event_Id_00);
	}
	else
	{
		W2000.init = 1;
	}
}

void W2000_Error_Check()
{
	if ((W2000.Status&0xFFFF0000) != 0)
	{
		Bike.Fault.bit.Motor_Error = 1;
		Event_post(EVT_Error,Event_Id_00);
	}
}

void eCAN_init(void)
{

	EALLOW;
		struct	ECAN_REGS ECanaShadow;
		ECanaShadow.CANMIM.all = 0;
	  	ECanaShadow.CANMIM.bit.MIM0  = 1;  // MB#0  Mailbox interrupt is enabled
	  	ECanaShadow.CANMIM.bit.MIM1  = 1;  // MB#1  Mailbox interrupt is enabled
	  	ECanaShadow.CANMIM.bit.MIM2  = 1;  // MB#2  Mailbox interrupt is enabled
	  	ECanaShadow.CANMIM.bit.MIM3  = 1;  // MB#3  Mailbox interrupt is enabled
	  	ECanaShadow.CANMIM.bit.MIM4  = 1;  // MB#4  Mailbox interrupt is enabled
	  	ECanaShadow.CANMIM.bit.MIM5  = 1;  // MB#5  Mailbox interrupt is enabled
	  	ECanaShadow.CANMIM.bit.MIM6  = 1;  // MB#6  Mailbox interrupt is enabled
	  	ECanaShadow.CANMIM.bit.MIM7  = 1;  // MB#7  Mailbox interrupt is enabled
	  	ECanaShadow.CANMIM.bit.MIM8  = 1;  // MB#8  Mailbox interrupt is enabled
	  	ECanaShadow.CANMIM.bit.MIM9  = 1;  // MB#9  Mailbox interrupt is enabled
	  	ECanaRegs.CANMIM.all = ECanaShadow.CANMIM.all;
	    ECanaShadow.CANMIL.all = 0;
	  	ECanaShadow.CANMIL.bit.MIL0  = 1;  // Int.-Level MB#0  -> I1EN
	  	ECanaShadow.CANMIL.bit.MIL1  = 1;  // Int.-Level MB#1  -> I1EN
	  	ECanaShadow.CANMIL.bit.MIL2  = 1;  // Int.-Level MB#2  -> I1EN
	  	ECanaShadow.CANMIL.bit.MIL3  = 1;  // Int.-Level MB#3  -> I1EN
	  	ECanaShadow.CANMIL.bit.MIL4  = 1;  // Int.-Level MB#4  -> I1EN
	  	ECanaShadow.CANMIL.bit.MIL5  = 1;  // Int.-Level MB#5  -> I1EN
	  	ECanaShadow.CANMIL.bit.MIL6  = 1;  // Int.-Level MB#6  -> I1EN
	  	ECanaShadow.CANMIL.bit.MIL7  = 1;  // Int.-Level MB#7  -> I1EN
	  	ECanaShadow.CANMIL.bit.MIL8  = 1;  // Int.-Level MB#8  -> I1EN
	  	ECanaShadow.CANMIL.bit.MIL9  = 1;  // Int.-Level MB#9  -> I1EN
	  	ECanaRegs.CANMIL.all = ECanaShadow.CANMIL.all;
	    ECanaShadow.CANGIM.all = 0;
	  	ECanaShadow.CANGIM.bit.I1EN = 1;  // enable I1EN
	  	ECanaRegs.CANGIM.all = ECanaShadow.CANGIM.all;
	  	ECanaShadow.CANGAM.bit.AMI = 0; //must be standard
	  	ECanaRegs.CANGAM.all = ECanaShadow.CANGAM.all;
		EDIS;
		/* Receiver Mailbox #MBX0 - StatusInformation			*/
		ECanaMboxes.MBOX0.MSGID.bit.IDE = 0;
	 	ECanaMboxes.MBOX0.MSGID.bit.AME = 0;
	 	ECanaMboxes.MBOX0.MSGID.bit.AAM = 0;
	 	ECanaMboxes.MBOX0.MSGID.bit.STDMSGID = 0x401;	// receive ID
	 	ECanaMboxes.MBOX0.MSGCTRL.bit.DLC = 8;

		/* Receiver Mailbox #MBX1 - BusMeasurment			*/
	 	ECanaMboxes.MBOX1.MSGID.bit.IDE = 0;
	 	ECanaMboxes.MBOX1.MSGID.bit.AME = 0;
	 	ECanaMboxes.MBOX1.MSGID.bit.AAM = 0;
	 	ECanaMboxes.MBOX1.MSGID.bit.STDMSGID = 0x402;	// receive ID
	 	ECanaMboxes.MBOX1.MSGCTRL.bit.DLC = 8;

		/* Receiver Mailbox #MBX2 - VelocityMeasurment			*/
	 	ECanaMboxes.MBOX2.MSGID.bit.IDE = 0;
	 	ECanaMboxes.MBOX2.MSGID.bit.AME = 0;
	 	ECanaMboxes.MBOX2.MSGID.bit.AAM = 0;
	 	ECanaMboxes.MBOX2.MSGID.bit.STDMSGID = 0x403;	// receive ID
	 	ECanaMboxes.MBOX2.MSGCTRL.bit.DLC = 8;

		/* Receiver Mailbox #MBX3 - PhaseCurrentMeasurment			*/
	 	ECanaMboxes.MBOX3.MSGID.bit.IDE = 0;
	 	ECanaMboxes.MBOX3.MSGID.bit.AME = 0;
	 	ECanaMboxes.MBOX3.MSGID.bit.AAM = 0;
	 	ECanaMboxes.MBOX3.MSGID.bit.STDMSGID = 0x404;	// receive ID
	 	ECanaMboxes.MBOX3.MSGCTRL.bit.DLC = 8;

		/* Receiver Mailbox #MBX4 - MotorVoltage			*/
	 	ECanaMboxes.MBOX4.MSGID.bit.IDE = 0;
	 	ECanaMboxes.MBOX4.MSGID.bit.AME = 0;
	 	ECanaMboxes.MBOX4.MSGID.bit.AAM = 0;
	 	ECanaMboxes.MBOX4.MSGID.bit.STDMSGID = 0x405;	// receive ID
	 	ECanaMboxes.MBOX4.MSGCTRL.bit.DLC = 8;

		/* Receiver Mailbox #MBX5 - MotorCurrent			*/
	 	ECanaMboxes.MBOX5.MSGID.bit.IDE = 0;
	 	ECanaMboxes.MBOX5.MSGID.bit.AME = 0;
	 	ECanaMboxes.MBOX5.MSGID.bit.AAM = 0;
	 	ECanaMboxes.MBOX5.MSGID.bit.STDMSGID = 0x406;	// receive ID
	 	ECanaMboxes.MBOX5.MSGCTRL.bit.DLC = 8;

		/* Receiver Mailbox #MBX6 - Temperature			*/
	 	ECanaMboxes.MBOX6.MSGID.bit.IDE = 0;
	 	ECanaMboxes.MBOX6.MSGID.bit.AME = 0;
	 	ECanaMboxes.MBOX6.MSGID.bit.AAM = 0;
	 	ECanaMboxes.MBOX6.MSGID.bit.STDMSGID = 0x40b;	// receive ID
	 	ECanaMboxes.MBOX6.MSGCTRL.bit.DLC = 8;

		/* Receiver Mailbox #MBX7 - Odometer-amphours			*/
	 	ECanaMboxes.MBOX7.MSGID.bit.IDE = 0;
	 	ECanaMboxes.MBOX7.MSGID.bit.AME = 0;
	 	ECanaMboxes.MBOX7.MSGID.bit.AAM = 0;
	 	ECanaMboxes.MBOX7.MSGID.bit.STDMSGID = 0x40e;	// receive ID
	 	ECanaMboxes.MBOX7.MSGCTRL.bit.DLC = 8;

		/* Receiver Mailbox #MBX8 - MotorDriveCommand			*/
	 	ECanaMboxes.MBOX8.MSGID.bit.IDE = 0;
	 	ECanaMboxes.MBOX8.MSGID.bit.AME = 0;
	 	ECanaMboxes.MBOX8.MSGID.bit.AAM = 0;
	 	ECanaMboxes.MBOX8.MSGID.bit.STDMSGID = 0x501;	// receive ID
	 	ECanaMboxes.MBOX8.MSGCTRL.bit.DLC = 8;

		/* Receiver Mailbox #MBX9 - MotorPowerCommand			*/
	 	ECanaMboxes.MBOX9.MSGID.bit.IDE = 0;
	 	ECanaMboxes.MBOX9.MSGID.bit.AME = 0;
	 	ECanaMboxes.MBOX9.MSGID.bit.AAM = 0;
	 	ECanaMboxes.MBOX9.MSGID.bit.STDMSGID = 0x502;	// receive ID
	 	ECanaMboxes.MBOX9.MSGCTRL.bit.DLC = 8;


	  	ECanaShadow.CANMD.all = ECanaRegs.CANMD.all;
	  	ECanaShadow.CANMD.bit.MD0 = 1;          // receive
	  	ECanaShadow.CANMD.bit.MD1 = 1;          // receive
	  	ECanaShadow.CANMD.bit.MD2 = 1;          // receive
	  	ECanaShadow.CANMD.bit.MD3 = 1;          // receive
	  	ECanaShadow.CANMD.bit.MD4 = 1;          // receive
	  	ECanaShadow.CANMD.bit.MD5 = 1;          // receive
	  	ECanaShadow.CANMD.bit.MD6 = 1;          // receive
	  	ECanaShadow.CANMD.bit.MD7 = 1;          // receive
	  	ECanaShadow.CANMD.bit.MD8 = 1;          // receive
	  	ECanaShadow.CANMD.bit.MD9 = 1;          // receive
	  	ECanaRegs.CANMD.all = ECanaShadow.CANMD.all;
	  	ECanaShadow.CANME.all = ECanaRegs.CANME.all;
	  	ECanaShadow.CANME.bit.ME0 = 1;          // enable
	  	ECanaShadow.CANME.bit.ME1 = 1;          // enable
	  	ECanaShadow.CANME.bit.ME2 = 1;          // enable
	  	ECanaShadow.CANME.bit.ME3 = 1;          // enable
	  	ECanaShadow.CANME.bit.ME4 = 1;          // enable
	  	ECanaShadow.CANME.bit.ME5 = 1;          // enable
	  	ECanaShadow.CANME.bit.ME6 = 1;          // enable
	  	ECanaShadow.CANME.bit.ME7 = 1;          // enable
	  	ECanaShadow.CANME.bit.ME8 = 1;          // enable
	  	ECanaShadow.CANME.bit.ME9 = 1;          // enable
		ECanaRegs.CANME.all = ECanaShadow.CANME.all;

	EALLOW;
    ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
    ECanaShadow.CANMC.bit.STM = 0;    // No self-test mode
    ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;
    EDIS;
}

void Send_eCAN(void)
{
	struct	ECAN_REGS ECanaShadow;



	ECanaShadow.CANTRS.all = 0;
	ECanaShadow.CANTRS.bit.TRS5 = 1;     // Set TRS for mailbox under test
	ECanaRegs.CANTRS.all = ECanaShadow.CANTRS.all;

	Event_pend(EVT_eCAN_Int,Event_Id_00, Event_Id_NONE,BIOS_WAIT_FOREVER);


}

 void HWI_eCAN_Int(UArg a0, UArg a1)
{
	 Uint32 dummy;
  	unsigned int mailbox_nr;
  	mailbox_nr = ECanaRegs.CANGIF1.bit.MIV1;
	if(mailbox_nr == 0)  // mailbox #0 receive message - Status
  	{
		read |= BIT0;
		dummy = ECanaMboxes.MBOX0.MDL.all;
		memcpy(&W2000.Status,&dummy,2);
		ECanaRegs.CANRMP.bit.RMP0 = 1;
//		W2000_Error_Check();
//		CAN_cache[cache_counter]=W2000.Status;
//		if(cache_counter<W2000_LOG_RATE-1)
//		cache_counter++;
//		else
//		cache_counter=0;
   	}
	if(mailbox_nr == 1)  // mailbox #1 receive message - Bus
  	{
		read |= BIT1;
		dummy = ECanaMboxes.MBOX1.MDH.all;
		memcpy(&W2000.BusCurrent,&dummy,2);
		dummy = ECanaMboxes.MBOX1.MDL.all;
		memcpy(&W2000.BusVoltage,&dummy,2);
		if(Bike.Mode = MODE_RACE && W2000.BusVoltage <= BUSVOLT_WARNING_THRESHOLD_RACE)
		{
			Bike.Warning.bit.bus_volt = 1;
		}
		else
		{
			Bike.Warning.bit.bus_volt = 0; //unlatch
		}
		ECanaRegs.CANRMP.bit.RMP1 = 1;
   	}
	if(mailbox_nr == 2)  // mailbox #2 receive message - Velocity
  	{
		read |= BIT2;
		dummy = ECanaMboxes.MBOX2.MDL.all;
		memcpy(&W2000.MotorVelocity,&dummy,2);
		dummy = ECanaMboxes.MBOX2.MDH.all;
		memcpy(&W2000.VehicleVelocity,&dummy,2);
		ECanaRegs.CANRMP.bit.RMP2 = 1;
   	}
	if(mailbox_nr == 3)  // mailbox #3 receive message - Phase Current
  	{
		read |= BIT3;
		dummy = ECanaMboxes.MBOX3.MDH.all;
		memcpy(&W2000.PhaseCcurrent,&dummy,2);
		dummy = ECanaMboxes.MBOX3.MDL.all;
		memcpy(&W2000.PhaseBcurrent,&dummy,2);
		ECanaRegs.CANRMP.bit.RMP3 = 1;
   	}
	if(mailbox_nr == 4)  // mailbox #4 receive message - Motor Voltage
  	{
		read |= BIT4;
		dummy = ECanaMboxes.MBOX4.MDH.all;
		memcpy(&W2000.MotorVd,&dummy,2);
		dummy = ECanaMboxes.MBOX4.MDL.all;
		memcpy(&W2000.MotorVq,&dummy,2);
		ECanaRegs.CANRMP.bit.RMP4 = 1;
   	}
	if(mailbox_nr == 5)  // mailbox #5 receive message - Motor Current
  	{
		read |= BIT5;
		dummy = ECanaMboxes.MBOX5.MDH.all;
		memcpy(&W2000.MotorId,&dummy,2);
		dummy = ECanaMboxes.MBOX5.MDL.all;
		memcpy(&W2000.MotorIq,&dummy,2);
		ECanaRegs.CANRMP.bit.RMP5 = 1;
   	}
	if(mailbox_nr == 6)  // mailbox #1 receive message - Temperature
  	{
		read |= BIT6;
		dummy = ECanaMboxes.MBOX6.MDL.all;
		memcpy(&W2000.MotorTemp,&dummy,2);
		dummy = ECanaMboxes.MBOX6.MDH.all;
		memcpy(&W2000.PhaseATemp,&dummy,2);
		ECanaRegs.CANRMP.bit.RMP6 = 1;
   	}
	if(mailbox_nr == 7)  // mailbox #7 receive message - Odometer and amphour
  	{
		read |= BIT7;
		W2000.BusAmphour = ECanaMboxes.MBOX7.MDH.all;
		dummy = ECanaMboxes.MBOX7.MDH.all;
		memcpy(&W2000.BusAmphour,&dummy,2);
		W2000.Odometer = ECanaMboxes.MBOX7.MDL.all;
		memcpy(&W2000.Odometer,&dummy,2);
		ECanaRegs.CANRMP.bit.RMP7 = 1;
   	}
	if(mailbox_nr == 8)  // mailbox #8 receive message - Motor Drive Command
  	{
		read |= BIT8;
		dummy = ECanaMboxes.MBOX8.MDL.all;
		memcpy(&W2000.MotorVelocityCommand,&dummy,2);
		dummy = ECanaMboxes.MBOX8.MDH.all;
		memcpy(&W2000.MotorCurrentCommand,&dummy,2);
		ECanaRegs.CANRMP.bit.RMP8 = 1;
   	}
	if(mailbox_nr == 9)  // mailbox #9 receive message - Motor Power Command
  	{
		read |= BIT9;
		dummy = ECanaMboxes.MBOX9.MDH.all;
		memcpy(&W2000.BusCurrentCommand ,&dummy,2);
		ECanaRegs.CANRMP.bit.RMP9 = 1;
   	}


}

