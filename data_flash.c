

#include "bq_pack.h"
#include "data_flash.h"



/**
* @brief  Local variables        .                     
*/
//Constant Array that stores the battery pack info on flash memory
const unsigned long DATA_FLASH[] =
{
  /*Cell Voltage*/
  dCOV_THRESHOLD,                  //COV_THRESHOLD           [mV]
  dCOV_RECOVERY_THRESHOLD,         //COV_RECOVERY_THRESHOLD  [mV]
  dCOV_TIME,                       //COV_TIME                [100ms]
  dCUV_THRESHOLD,                  //CUV_THRESHOLD           [mV]
  dCUV_RECOVERY_THRESHOLD,         //CUV_RECOVERY_THRESHOLD  [mV]
  dCUV_TIME,                       //CUV_TIME                [100ms]
  
  /*Temperature*/
  dPACK_OVER_TEMP1, //3 PACK_OVER_TEMP1   [st C]
  dPACK_OT_TIME1,   //PACK_OT_TIME1       [ms]
  dPACK_OVER_TEMP2,  //3 PACK_OVER_TEMP2   [st C]
  dPACK_OT_TIME2,   //PACK_OT_TIME2       [ms]
  
  /*Charge and Discharge*/
  dPACK_END_OF_CHARGE_VOLTAGE,     //PACK_END_OF_CHARGE_VOLTAGE    [mV] 
  dCC_CV_QUAL_TIME,                //CC_CV_QUAL_TIME               [s]
  dPACK_END_OF_DISCHARGE_VOLTAGE,  //PACK_END_OF_DISCHARGE_VOLTAGE [mV] 
  dEND_OF_DISCHARGE_QUAL_TIME,     //END_OF_DISCHARGE_QUAL_TIME    [s]
  dCHARGE_CURRENT,                 //CHARGE_CURRENT                [mA]
  dCHARGE_TAPER_CURRENT,           //CHARGE_TAPER_CURRENT          [mA]
  dCHARGE_TAPER_TIME,              //CHARGE_TAPER_TIME             [s]
  dMAX_CHARGE_TIME,                //MAX_CHARGE_TIME               [s]
  dFULL_DISCHARGE_CLEAR_VOLTS,     //FULL_DISCHARGE_CLEAR_VOLTS    [mV] 
  dFULL_CHARGE_CLEAR_VOLTS,        //FULL_CHARGE_CLEAR_VOLTS       [mV] 
  dDELTA_CHARGE_V,                 //DELTA_CHARGE_V                [mv]
  dCHARGE_DISCHARGE_TIME,          //CHARGE_DISCHARGE_TIME         [s]
  dDELTA_DISCHARGE_V,              //DELTA_DISCHARGE_V             [mV]
    
  /*Safety*/
  dSOV_THRESHOLD,                 //SOV_THRESHOLD                 [mV]
  dSOV_RECOVERY_THRESHOLD,        //SOV_RECOVERY_THRESHOLD        [mV]
  dSOV_TIME,                      //SOV_TIME                      [ms]
  
  /*Balancing*/
  dCELL_IMBALANCE_FAIL_THRESHOLD,    //CELL_IMBALANCE_FAIL_THRESHOLD [mV]
  dCELL_IMBALANCE_FAIL_TIME,         //CELL_IMBALANCE_FAIL_TIME      [s]
  dBALANCE_TIME,                     //BALANCE_TIME     [s] max value is 63
  dBALANCE_VOLTS_THRESHOLD,          //BALANCE_VOLTS_THRESHOLD       [mV]
  dMIN_BALANCE_VOLTS,                //MIN_BALANCE_VOLTS             [mV]
  dMAX_BALANCE_TIME,                 //MAX_BALANCE_TIME              [s]
  
};


/**
* @brief Function Name: get_u16_value.                                                 
* @brief Description  : Reads the flash memory and returns the value of the 
* parameter
* @param parameters   : battery pack parameter to be read                                                     
* @return Value       : value of the specific battery parameter                                                    
*/     
unsigned short get_u16_value(param_id_t param_id)
{

  return (unsigned short)DATA_FLASH[param_id];

}

/**
* @brief Function Name: get_u32_value.                                                 
* @brief Description  : Reads the flash memory and returns the value of the 
* parameter
* @param parameters   : battery pack parameter to be read                                                     
* @return Value       : value of the specific battery parameter
*/   
unsigned long get_u32_value(param_id_t param_id)
{
  return (unsigned long)DATA_FLASH[param_id];
}

/*EOF*/
