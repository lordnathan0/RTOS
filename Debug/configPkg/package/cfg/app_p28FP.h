/*
 *  Do not modify this file; it is automatically 
 *  generated and any modifications will be overwritten.
 *
 * @(#) xdc-x20
 */

#include <xdc/std.h>

#include <ti/sysbios/family/c28/Hwi.h>
extern const ti_sysbios_family_c28_Hwi_Handle spitx;

#include <ti/sysbios/family/c28/Hwi.h>
extern const ti_sysbios_family_c28_Hwi_Handle spirx;

#include <ti/sysbios/knl/Event.h>
extern const ti_sysbios_knl_Event_Handle EVT_SPI_Tx;

#include <ti/sysbios/knl/Event.h>
extern const ti_sysbios_knl_Event_Handle EVT_SPI_Rx;

#include <ti/sysbios/knl/Semaphore.h>
extern const ti_sysbios_knl_Semaphore_Handle SEM_SPI_Tx;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle Start;

#include <ti/sysbios/family/c28/Hwi.h>
extern const ti_sysbios_family_c28_Hwi_Handle BQFault;

#include <ti/sysbios/family/c28/Hwi.h>
extern const ti_sysbios_family_c28_Hwi_Handle BQAlert;

#include <ti/sysbios/knl/Event.h>
extern const ti_sysbios_knl_Event_Handle EVT_BQFault;

#include <ti/sysbios/knl/Event.h>
extern const ti_sysbios_knl_Event_Handle EVT_BQAlert;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle BQFualtTask;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle BQAlertTask;

#include <ti/sysbios/knl/Event.h>
extern const ti_sysbios_knl_Event_Handle EVT_BQ_DRDY;

#include <ti/sysbios/family/c28/Hwi.h>
extern const ti_sysbios_family_c28_Hwi_Handle BQ_DRDY;

#include <ti/sysbios/knl/Event.h>
extern const ti_sysbios_knl_Event_Handle EVT_Error;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle ErrorHandler;

#include <ti/sysbios/knl/Event.h>
extern const ti_sysbios_knl_Event_Handle EVT_BQ;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle BQ;

#include <ti/sysbios/knl/Clock.h>
extern const ti_sysbios_knl_Clock_Handle BQ_CLOCK;

#include <ti/sysbios/knl/Event.h>
extern const ti_sysbios_knl_Event_Handle EVT_eCAN_Int;

#include <ti/sysbios/family/c28/Hwi.h>
extern const ti_sysbios_family_c28_Hwi_Handle Ecan_int;

#include <ti/sysbios/knl/Event.h>
extern const ti_sysbios_knl_Event_Handle EVT_SCIa_Tx;

#include <ti/sysbios/knl/Event.h>
extern const ti_sysbios_knl_Event_Handle EVT_SCIa_Rx;

#include <ti/sysbios/family/c28/Hwi.h>
extern const ti_sysbios_family_c28_Hwi_Handle SCIa_RX;

#include <ti/sysbios/family/c28/Hwi.h>
extern const ti_sysbios_family_c28_Hwi_Handle SCIa_TX;

#include <ti/sysbios/knl/Event.h>
extern const ti_sysbios_knl_Event_Handle EVT_LCD_Tx;

#include <ti/sysbios/family/c28/Hwi.h>
extern const ti_sysbios_family_c28_Hwi_Handle LCD_TX;

#include <ti/sysbios/knl/Clock.h>
extern const ti_sysbios_knl_Clock_Handle LCD_CLOCK;

#include <ti/sysbios/knl/Event.h>
extern const ti_sysbios_knl_Event_Handle EVT_LCD_CLK;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle LCD;

#include <ti/sysbios/knl/Clock.h>
extern const ti_sysbios_knl_Clock_Handle BT_CLOCK;

#include <ti/sysbios/knl/Event.h>
extern const ti_sysbios_knl_Event_Handle EVT_BT_CLK;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle Bluetooth;

#include <ti/sysbios/knl/Clock.h>
extern const ti_sysbios_knl_Clock_Handle ALFAT_CLOCK;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle TSK_Rec;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle Alfat;

#include <ti/sysbios/knl/Event.h>
extern const ti_sysbios_knl_Event_Handle EVT_Alfat_CLK;

#include <ti/sysbios/knl/Event.h>
extern const ti_sysbios_knl_Event_Handle EVT_byteRec;

#include <ti/sysbios/family/c28/Hwi.h>
extern const ti_sysbios_family_c28_Hwi_Handle I2C;

#include <ti/sysbios/knl/Clock.h>
extern const ti_sysbios_knl_Clock_Handle EXT_ADC_CLOCK;

#include <ti/sysbios/knl/Event.h>
extern const ti_sysbios_knl_Event_Handle EVT_Ext_ADC_CLK;

#include <ti/sysbios/knl/Event.h>
extern const ti_sysbios_knl_Event_Handle EVT_Sleep;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle change_mode;

#include <ti/sysbios/family/c28/Hwi.h>
extern const ti_sysbios_family_c28_Hwi_Handle wakeint;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle sleep_measurement;

#include <ti/sysbios/knl/Event.h>
extern const ti_sysbios_knl_Event_Handle EVT_Change_Mode;

#include <ti/sysbios/knl/Event.h>
extern const ti_sysbios_knl_Event_Handle EVT_Sleep_M;

#include <ti/sysbios/family/c28/Hwi.h>
extern const ti_sysbios_family_c28_Hwi_Handle ESTOP;

#include <ti/sysbios/family/c28/Hwi.h>
extern const ti_sysbios_family_c28_Hwi_Handle Charge_Sense;

#include <ti/sysbios/family/c28/Hwi.h>
extern const ti_sysbios_family_c28_Hwi_Handle ON_SW;

#include <ti/sysbios/family/c28/Hwi.h>
extern const ti_sysbios_family_c28_Hwi_Handle ADC_complete;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle adc_task;

#include <ti/sysbios/knl/Event.h>
extern const ti_sysbios_knl_Event_Handle EVT_ADC;

#include <ti/sysbios/knl/Clock.h>
extern const ti_sysbios_knl_Clock_Handle ADC_CLOCK;

#include <ti/sysbios/knl/Clock.h>
extern const ti_sysbios_knl_Clock_Handle TIMESTAMP_CLOCK;

#include <ti/sysbios/knl/Event.h>
extern const ti_sysbios_knl_Event_Handle EVT_TimeStamp_CLK;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle TimeStamp;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle ext_adc;

#include <ti/sysbios/knl/Event.h>
extern const ti_sysbios_knl_Event_Handle EVT_Alfat_DONE;

#include <ti/sysbios/family/c28/Hwi.h>
extern const ti_sysbios_family_c28_Hwi_Handle Fault_Override;

extern int xdc_runtime_Startup__EXECFXN__C;

extern int xdc_runtime_Startup__RESETFXN__C;

#ifndef ti_sysbios_knl_Task__include
#ifndef __nested__
#define __nested__
#include <ti/sysbios/knl/Task.h>
#undef __nested__
#else
#include <ti/sysbios/knl/Task.h>
#endif
#endif

extern ti_sysbios_knl_Task_Struct TSK_idle;

