/*
 * Bluetooth.h
 *
 *  Created on: Sep 13, 2012
 *      Author: LordLaptop
 */

#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

void scia_fifo_init(void);
unsigned char BT_msg(char * msg);
unsigned char Send_SCIA(int a);
void BT_Startup();
void BT_Disable();
void check_low_cells();
void check_high_cells();
unsigned short determineCell(int dev, int cellOfDev);


#define BLUETOOTH_TIMEOUT_COUNT 	 10
#define BLUETOOTH_TIMEOUT 100
#define separator " | \0"

typedef struct BLUETOOTH
{
	unsigned char init;
	unsigned short timeout_count;
	unsigned long Timestamp;
	Uint16 rdata[8];     // Receive data buffer
} Bluetooth_Status;

#endif /* BLUETOOTH_H_ */
