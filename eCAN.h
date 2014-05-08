/*
 * eCAN.h
 *
 *  Created on: Sep 13, 2012
 *      Author: LordLaptop
 */

#ifndef ECAN_H_
#define ECAN_H_

#define W2000_LOG_RATE	20
#define PACKET_WAIT_TIME 3000
void eCAN_init(void);
void Send_eCAN(void);
void W2000_Startup();
void W2000_Full_Read();
void W2000_Disable();
void W2000_Error_Check();

typedef struct w2000
{
	unsigned char init;
	unsigned long Status;
	float BusCurrent;
	float BusVoltage;
	float MotorVelocity;
	float VehicleVelocity;
	float PhaseBcurrent;
	float PhaseCcurrent;
	float MotorVd;
	float MotorVq;
	float MotorId;
	float MotorIq;
	float MotorTemp;
	float PhaseATemp;
	float BusAmphour;
	float Odometer;
	float MotorCurrentCommand;
	float MotorVelocityCommand;
	float BusCurrentCommand;
} w2000_Status;



#endif /* ECAN_H_ */
