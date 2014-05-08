

#ifndef ADC_H_
#define ADC_H_


void adc_init();
void Disable_ADC();

#define ADC_MODCLK 	0x3 // HSPCLK = SYSCLKOUT/2*ADC_MODCLK2 = 150/(2*3)   = 25.0 MHz
#define ADC_CKPS   	0x1   // ADC module clock = HSPCLK/2*ADC_CKPS   = 25.0MHz/(1*2) = 12.5MHz
#define ADC_SHCLK  	0xf   // S/H width in ADC module periods                        = 16 ADC clocks
#define AVG        	1000  // Average sample limit
#define ZOFFSET    	0x00  // Average Zero offset
#define BUF_SIZE   	100  // Sample buffer size
#define ADC_RATIO 	6


typedef struct ADC
{
	float bat_12v;
	unsigned short Init;
} ADC_st;


#endif /* 12V_H_ */
