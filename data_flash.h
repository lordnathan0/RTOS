
#ifndef DATA_FLASH_H
#define DATA_FLASH_H


//SPI communication definition
#define SPI_TIMEOUT			100
#define SPI_TIMEOUT_COUNT	10

//Battery pack definition
#define NUMBER_OF_BQ_DEVICES      18 //3 BQ76PL536 devices are connected
#define NUMBER_OF_CELLS           108 //Total number of cells in the system
#define MAX_CELLS_NUMBER_IN_BQ    6  //MAX number of cells per BQ76PL536 device
#define CELL_BALANCING_EN         0  //set to 1 to enable cell balancing
#define ONE_MINUTE					60

//Battery pack information and threshold values
#define dCOV_THRESHOLD                 4000  //COV_THRESHOLD           [mV]
#define dCOV_RECOVERY_THRESHOLD        3900  //COV_RECOVERY_THRESHOLD  [mV]
#define dCOV_TIME                      5 //20 //COV_TIME (max value 32) [100ms] 
#define dCUV_THRESHOLD                 3000  //CUV_THRESHOLD           [mV]
#define dCUV_RECOVERY_THRESHOLD        3200  //CUV_RECOVERY_THRESHOLD  [mV]
#define dCUV_TIME                      5 //20 //CUV_TIME (max value 32) [100ms] 

/*
CONFIG_OT	VTS_SET	VTS_CLEAR	TSET		TCLEAR
0 disable
1			0.2		0.1766		39.55 ºC	34.32 ºC
2			0.2244	0.2			44.89 ºC	39.55 ºC
3			0.2488	0.227		50.19 ºC	45.46 ºC
4			0.2712	0.2498		55.09 ºC	50.41 ºC
5			0.2956	0.275		60.54 ºC	55.93 ºC
6			0.3156	0.2956		65.16 ºC	60.54 ºC
7			0.3356	0.3162		69.97 ºC	65.3 ºC
8			0.3556	0.3368		75.02 ºC	70.26 ºC
9			0.3712	0.3528		79.19 ºC	74.3 ºC
10			0.3866	0.3688		83.54 ºC	78.53 ºC
11			0.4		0.3824		87.56 ºC	82.32 ºC

*/

#define dPACK_OVER_TEMP1               	40    //3 PACK_OVER_TEMP1   [st C]
#define dPACK_OVER_TEMP1_Config			1
#define dPACK_OVER_TEMP1_RATIO_CLEAR   	.1766 //USE TABLE ABOVE TO SET VALUE
#define dPACK_OVER_TEMP1_RATIO_SET   	.2 //USE TABLE ABOVE TO SET VALUE
#define dPACK_OT_TIME1                 	200  //PACK_OT_TIME1       [10ms]
#define dPACK_OVER_TEMP2              	40    //3 PACK_OVER_TEMP1   [st C]
#define dPACK_OVER_TEMP2_Config			1
#define dPACK_OVER_TEMP2_RATIO_CLEAR   	.1766 //USE TABLE ABOVE TO SET VALUE
#define dPACK_OVER_TEMP2_RATIO_SET   	.2 //USE TABLE ABOVE TO SET VALUE
#define dPACK_OT_TIME2                 	200  //PACK_OT_TIME1       [10ms]

//PACK_END_OF_CHARGE_VOLTAGE   [mV] 
#define dPACK_END_OF_CHARGE_VOLTAGE    (unsigned long)dCOV_THRESHOLD*NUMBER_OF_CELLS
#define dCC_CV_QUAL_TIME               20    //CC_CV_QUAL_TIME              [s]
//PACK_END_OF_DISCHARGE_VOLTAGE[mV]
#define dPACK_END_OF_DISCHARGE_VOLTAGE (unsigned long)dCUV_THRESHOLD*NUMBER_OF_CELLS
#define dEND_OF_DISCHARGE_QUAL_TIME    20    //END_OF_DISCHARGE_QUAL_TIME   [s]

#define dCHARGE_CURRENT                1100  //CHARGE_CURRENT               [mA]
#define dCHARGE_TAPER_CURRENT          300   //CHARGE_TAPER_CURRENT         [mA]
#define dCHARGE_TAPER_TIME             (unsigned long)240*ONE_MINUTE//CHARGE_TAPER_TIME[s]
#define dMAX_CHARGE_TIME               (unsigned long)200*ONE_MINUTE//MAX_CHARGE_TIME  [s]

//FULL_DISCHARGE_CLEAR_VOLTS   [mV]
#define dFULL_DISCHARGE_CLEAR_VOLTS    dPACK_END_OF_DISCHARGE_VOLTAGE
//FULL_CHARGE_CLEAR_VOLTS      [mV] 
#define dFULL_CHARGE_CLEAR_VOLTS       dPACK_END_OF_CHARGE_VOLTAGE    
#define dDELTA_CHARGE_V                300   //DELTA_CHARGE_V               [mv]
#define dCHARGE_DISCHARGE_TIME         (unsigned long)5*ONE_MINUTE//CHARGE_DISCHARGE_TIME [s]
#define dDELTA_DISCHARGE_V             200   //DELTA_DISCHARGE_V            [mV]

#define dSOV_THRESHOLD                 4200  //SOV_THRESHOLD                [mV]
#define dSOV_RECOVERY_THRESHOLD        3800  //SOV_RECOVERY_THRESHOLD       [mV]
#define dSOV_TIME                      3000  //SOV_TIME                     [ms]

#define dCELL_IMBALANCE_FAIL_THRESHOLD 500   //CELL_IMBALANCE_FAIL_THRESHOLD[mV]
#define dCELL_IMBALANCE_FAIL_TIME      (unsigned long)120*ONE_MINUTE//CELL_IMBALANCE_FAIL_TIME[s]
#define dBALANCE_TIME                  (unsigned long)1    //BALANCE_TIME A.K.A CB_TIME[s]
#define dBALANCE_VOLTS_THRESHOLD       50    //BALANCE_VOLTS_THRESHOLD      [mV]
#define dMIN_BALANCE_VOLTS             dCUV_RECOVERY_THRESHOLD //MIN_BALANCE_VOLTS[mV]
#define dMAX_BALANCE_TIME              (unsigned long)120*ONE_MINUTE//MAX_BALANCE_TIME[s]



/**
* @brief  Global defines         .                     
*/

//definition of the Parameters structure
typedef enum PARAM_ID
{
  /*Voltage*/
  COV_THRESHOLD, //COV_THRESHOLD = 0,
  COV_RECOVERY_THRESHOLD,
  COV_TIME,
  CUV_THRESHOLD,
  CUV_RECOVERY_THRESHOLD,
  CUV_TIME,
  
  /*Temperature*/
  PACK_OVER_TEMP1,
  PACK_OT_TIME1,
  PACK_OVER_TEMP2,
  PACK_OT_TIME2,
  
  /*Charge and Discharge*/
  PACK_END_OF_CHARGE_VOLTAGE,
  CC_CV_QUAL_TIME,
  PACK_END_OF_DISCHARGE_VOLTAGE,
  END_OF_DISCHARGE_QUAL_TIME,
  CHARGE_CURRENT,
  CHARGE_TAPER_CURRENT,
  CHARGE_TAPER_TIME,
  MAX_CHARGE_TIME,
  FULL_DISCHARGE_CLEAR_VOLTS,
  FULL_CHARGE_CLEAR_VOLTS,
  DELTA_CHARGE_V,
  CHARGE_DISCHARGE_TIME,
  DELTA_DISCHARGE_V,
  
  /*Safety*/
  SOV_THRESHOLD,
  SOV_RECOVERY_THRESHOLD,
  SOV_TIME,

  /*Balancing*/
  CELL_IMBALANCE_FAIL_THRESHOLD,
  CELL_IMBALANCE_FAIL_TIME,
  BALANCE_TIME,
  BALANCE_VOLTS_THRESHOLD,
  MIN_BALANCE_VOLTS,
  MAX_BALANCE_TIME
  
} param_id_t;


/**
* @brief  Global functions declaration         .                     
*/

extern unsigned short get_u16_value(param_id_t param_id);
extern unsigned long get_u32_value(param_id_t param_id);

#endif

/*EOF*/
