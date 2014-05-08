/*
 * Sleep.h
 *
 *  Created on: Nov 1, 2012
 *      Author: LordLaptop
 */

#ifndef SLEEP_H_
#define SLEEP_H_
#define BLEEDER_TIME 20000
#define WD_COUNT 100 //200 is one min

void Sleep();
void GPIO_Sleep();
void CPU_Sleep();
void TSK_Sleep_Measurment();
void GPIO_Sleep_Measurment();
char Start_Sleep_Measurment();
void Sleep_Peripherals();
void Init_WAKEINT();

#endif /* SLEEP_H_ */
