/*
 * ext_adc.h
 *
 *  Created on: Oct 21, 2012
 *      Author: Anil Ozyalcin
 */

#ifndef EXT_ADC_H_
#define EXT_ADC_H_

void ext_ADC_Startup();
void ext_ADC_Disable();
void Ext_Adc_Error_Check();

typedef struct ext_ADC
{
  unsigned short Init;
  double current;
  unsigned short status;
  float coul;
  unsigned int timeout_count;
} ext_ADC_st;



#endif /* EXT_ADC_H_ */

