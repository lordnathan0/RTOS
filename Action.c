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

#include "Action.h"



void Disable_WAKEINT()
{
	DisableDog();
}








