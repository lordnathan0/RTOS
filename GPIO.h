/*
 * GPIO.h
 *
 *  Created on: Nov 1, 2012
 *      Author: LordLaptop
 */

#ifndef GPIO_H_
#define GPIO_H_


#define C_FUEL_DOOR						bit.GPIO87 //not included in BMC r01
#define C_3V3_MAIN						bit.GPIO85
#define C_CHARGER_SW 					bit.GPIO6 // FROM 84 ==> 6

#define B_12V_SENSE						bit.GPIO58
#define B_12V_LOW 						bit.GPIO34

#define B_OT_WARNING 					bit.GPIO61

#define A_ON_SW 						bit.GPIO0
#define A_BORING_SW 					bit.GPIO2
#define A_FUALT_OVERRIDE_SW				bit.GPIO59
#define A_CHARGE_SW 					bit.GPIO6
#define A_ALARM_SILENCE_SW 				bit.GPIO8
#define A_MAIN_CTRL 					bit.GPIO12
#define A_PRECHARGE_CTRL				bit.GPIO15
#define B_12V_MAIN_CTRL 				bit.GPIO24// FROM 34 ==> 24
#define A_12V_AUX_CTRL 					bit.GPIO26
#define A_SIREN_CTRL 					bit.GPIO1

#define A_OV_FAULT 						bit.GPIO22
#define A_WARNING 						bit.GPIO5
#define A_FAULT 						bit.GPIO7
#define A_FAULT_OVERRIDE 				bit.GPIO9
#define A_SAFETY_INTERLOCK 				bit.GPIO11
#define A_CHARGING 						bit.GPIO13
#define A_OV_WARNING 					bit.GPIO25 //not included in bmc r01
#define A_UV_FAULT 						bit.GPIO27	//not included in bmc r01
#define A_UV_WARNING 					bit.GPIO21
#define A_OT_FAULT 						bit.GPIO23

#define FAULT_OVERRIDE_ON				1
#define PRECHARGE_TIME					20000
#define PRECHARGE_VOLT					250
#define FUEL_DOOR_WAIT					10000


void GPIO_INIT();
char Check_Mode();
void Disconnect_Contactors();
void Close_Contactors();




#endif /* GPIO_H_ */
