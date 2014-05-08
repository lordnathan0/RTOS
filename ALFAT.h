/*
 * ALFAT.h
 *
 *  Created on: Oct 16, 2012
 *      Author: Anil Ozyalcin
 */

#ifndef ALFAT_H_
#define ALFAT_H_

void ALFAT_Startup();
void Flush_SD(int handler);
int I2C_print(char *string, int length,int requested, int I2C_Address, int linefeed);
void Get_Time(void);
int Write_SD(char *string, int handler);
void ALFAT_Disable();
void integer2string(int integer, char* toror);
void double2string(double doub, double precision, char* toror1);
void DataMerger();
void long2string(long long_number,char* toror);
void Set_RTC_Time(char* date_out, char* time_out);
void Event_Logger(char *string);
void Battery_Voltage_Headers();
/*
 * error = 0 means no error
 * error = 1 means no welcome message received and ALFAT not started
 * error = 2 means write error
 * error = 3 NO SD card/SD couldn't initialize
 */
typedef struct ALFAT
{
  unsigned short error;
  unsigned short Init;
} ALFAT_st;

#endif /* ALFAT_H_ */
