

#ifndef _MAIN_H_
#define _MAIN_H_

#ifdef __cplusplus
extern "C"
{
#endif  
  
#define MODE_RACE 		0
#define MODE_CHARGE 	1
#define MODE_SLEEP 		2
#define ERROR 			3

#define	OV_WARNING_THRESHOLD_RACE							4210
#define	OV_WARNING_THRESHOLD_CHARGE							4210
#define	OV_WARNING_THRESHOLD_SLEEP							4210 //this should be fixed warning is higher than fault todo
#define	OV_FAULT_THRESHOLD_RACE								4260
#define	OV_FAULT_THRESHOLD_CHARGE							4260
#define	OV_FAULT_THRESHOLD_SLEEP							4200
#define	UV_WARNING_THRESHOLD_RACE							3000
#define	UV_WARNING_THRESHOLD_CHARGE							3400
#define	UV_WARNING_THRESHOLD_SLEEP							3400
#define	UV_FAULT_THRESHOLD_RACE								2750
#define	UV_FAULT_THRESHOLD_CHARGE							3000
#define	UV_FAULT_THRESHOLD_SLEEP							3000
#define	OT_WARNING_THRESHOLD_RACE							60
#define	OT_WARNING_THRESHOLD_CHARGE							50
#define	OT_WARNING_THRESHOLD_SLEEP							45
#define	OT_FAULT_THRESHOLD_RACE								70
#define	OT_FAULT_THRESHOLD_CHARGE							60
#define	OT_FAULT_THRESHOLD_SLEEP							50
#define	SOC_WARNING_THRESHOLD_RACE							10//%
#define	SOC_WARNING_THRESHOLD_CHARGE						10
#define	SOC_WARNING_THRESHOLD_SLEEP							20
#define DIS_OC_WARNING_THRESHOLD_CHARGE						-3
#define DIS_OC_WARNING_THRESHOLD_RACE						-250
#define DIS_OC_WARNING_THRESHOLD_SLEEP						-1
#define	DIS_OC_FAULT_THRESHOLD_RACE							-280
#define	DIS_OC_FAULT_THRESHOLD_CHARGE						-5
#define	DIS_OC_FAULT_THRESHOLD_SLEEP						-2
#define	CHARGE_OC_WARNING_THRESHOLD_RACE					50
#define	CHARGE_OC_WARNING_THRESHOLD_CHARGE					20
#define	CHARGE_OC_WARNING_THRESHOLD_SLEEP					2
#define	CHARGE_OC_FAULT_THRESHOLD_RACE						100
#define	CHARGE_OC_FAULT_THRESHOLD_CHARGE					40
#define	CHARGE_OC_FAULT_THRESHOLD_SLEEP						5
#define BMM_LOSS_THRESHOLD									5
#define LOWV_BATTERY_WARNING_THRESHOLD_SLEEP				12.5
#define LOWV_BATTERY_WARNING_THRESHOLD_RACE					12.0
#define LOWV_BATTERY_WARNING_THRESHOLD_CHARGE				12.2
#define LOWv_BATTERY_FUALT_THRESHOLD_SLEEP					12.5
#define LOWv_BATTERY_FUALT_THRESHOLD_RACE					11
#define LOWv_BATTERY_FUALT_THRESHOLD_CHARGE					12

#define	BUSVOLT_WARNING_THRESHOLD_RACE						325
#define	BUSVOLT_WARNING_THRESHOLD_CHARGE					400
#define	BUSBOLT_WARNING_THRESHOLD_SLEEP						400

struct  Fault_BITS {       // bit   description
	char     OV:1;
	char     UV:1;
	char     OT:1;
	char     Dis_OC:1;
	char     Charge_OC:1;
	char     Motor_Error:1;
	char 	 BMM_Init:1;
	char 	 Switch_Error:1;
	char 	 lowv_bat:1;
	char 	 estop:1;
	char 	precharge:1;
};

union Fault_REG {
	unsigned long 		all;
	struct Fault_BITS 	bit;
};

struct  Warning_BITS {
	char     OV:1;
	char     UV:1;
	char     OT:1;
	char     Dis_OC:1;
	char     Charage_OC:1;
	char     Capacity:1;
	char     BMM_loss:1;
	char     SD_Startup:1;
	char     SD_Write:1;
	char	 lowv_bat:1;
	char	 w2000_off:1;
	char 	current_sense_error:1;
	char 	bus_volt;
};

union Warning_REG {
	unsigned long 			all;
	struct Warning_BITS 	bit;
};

typedef struct bike
{
	char 					Mode;
	union Fault_REG 		Fault;
	union Warning_REG 		Warning;
} Bike_Status;



#ifdef __cplusplus
}
#endif
#endif /* _MAIN_H_ */
/*------------------------ Nothing Below This Line --------------------------*/

