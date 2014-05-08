/*
 * BQ.h
 *
 *  Created on: Sep 13, 2012
 *      Author: LordLaptop
 */

#ifndef BQ_H_
#define BQ_H_

#define SYSCLKOUT 150000000

void BQ_Startup();
void init_BQ();
void InitBQ76PL536ports();
void spi_fifo_init();
void BQ_Disable();


#endif /* BQ_H_ */
