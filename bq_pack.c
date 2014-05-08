
#include <string.h>
#include <stdio.h>

#include "main.h"
#include "bq_pack.h"
#include "spi_if.h"
#include "data_flash.h"

#include <xdc/std.h>

#include <xdc/runtime/System.h>

#include <ti/sysbios/BIOS.h>

#include <ti/sysbios/knl/Task.h>

#define xdc__strict

#include <ti/sysbios/knl/Task.h>

#include <ti/sysbios/knl/Clock.h>

#include <ti/sysbios/knl/Event.h>

#include <ti/sysbios/family/c28/Hwi.h>

#include <ti/sysbios/knl/Semaphore.h>

#include <xdc/cfg/global.h>

#include "main.h"

#include "DSP28x_Project.h"

#include "BQ.h"
  
/******************************************************************************/
/*                            Global variables                                */
/******************************************************************************/
//Global variable that contains the battery pack information
bq_pack_t bq_pack;
extern Bike_Status Bike;
extern char first_bq;
//SW flag that controls the Charge/Discharge mode

/******************************************************************************/
/*                            Local variables                                 */
/******************************************************************************/


void BMM_Sleep()
{
	if(bq_pack.Init == 1)
	{
		int i;
		Clock_stop(BQ_CLOCK);
		Event_pend(EVT_BQ,Event_Id_00, Event_Id_NONE,0);
		bq_pack.Init = 0;
		for (i=(NUMBER_OF_BQ_DEVICES-1); i >= 0; i--)
		{
		bq_dev_write_reg(bq_pack.bq_devs[i].device_address, IO_CONTROL_REG, 0);
		}
		/*
		for (i=(NUMBER_OF_BQ_DEVICES-1); i > 0; i--)
		{
			bq_dev_write_reg(bq_pack.bq_devs[i].device_address, IO_CONTROL_REG, BIT2);
			bq_dev_clear_alerts(&bq_pack.bq_devs[i]);
			Event_pend(EVT_BQ,Event_Id_00, Event_Id_NONE,0);
		}
		*/
	}


//	BQ_Disable();
}

void BMM_Wake()
{
	int i;
	first_bq = 0;
    /*
	for (i=(NUMBER_OF_BQ_DEVICES-1); i > 0; i--)
	{
		bq_dev_write_reg(bq_pack.bq_devs[i].device_address, IO_CONTROL_REG, IO_CONTROL_VAL);
	}
	*/
	bq_dev_write_reg(BROADCAST_ADDR, IO_CONTROL_REG, IO_CONTROL_VAL);
	Task_sleep(100); //wait for bq to wake up
	Event_post(EVT_BQ,Event_Id_00);
	while (first_bq == 0)
	{	//wait until first update is done

		Task_sleep(5);
	}
	Clock_start(BQ_CLOCK);
}

/**
* @brief Function Name: bq_pack_address_discovery                            .                     
* @brief Description  : BQ Pack address discovery and assignement algorithm .
* @param parameters   : void                                                    
* @return Value       : number of stacked BQ76PL536 devices                                                    
*/     
short bq_pack_address_discovery(void)
{
  unsigned short i, n;
  unsigned char reg_val[2];

  i=NUMBER_OF_BQ_DEVICES; //controls iteration loop
  while (i>0)
  {
    //*Send BROADCAST_RESET to address 0x00*/
    if(bq_dev_write_reg(BROADCAST_ADDR, RESET_REG, BQ76PL536_RESET) == 0)
    {
    	return 0;
    }
    Task_sleep(100); //wait for bq to reset
    n=0;  //controls number of discovered devices
    while (n<NUMBER_OF_BQ_DEVICES)
    {
    //*Read DEVICE_STATUS reg at address 0x00*/
    if (bq_dev_read_reg(DISCOVERY_ADDR, DEVICE_STATUS_REG, 1, DISCARD_CRC, reg_val) == 0)
    {
    	return 0;
    }
  
      //*Verify if MSB is equal to 0*/
      if (reg_val[0] & (1<<7))
      {
        n = NUMBER_OF_BQ_DEVICES; //break internal loop
      }
      else
      {
        //*Assign a new address*/        

        //Save assigned address
        n++;
        bq_pack.bq_devs[n-1].device_address = n;
        
        //ADDR_CTRL = n;   
        if(bq_dev_write_reg(DISCOVERY_ADDR, ADDRESS_CONTROL_REG, n) == 0)
        {
        	return 0;
        }
        
        //read ADDR_CTRL
        if (bq_dev_read_reg(n, ADDRESS_CONTROL_REG, 1, DISCARD_CRC, reg_val) == 0)
        {
        	return 0;
		}

        if ((reg_val[0]&0x3F) == n)
        {
          //address next device or finish device detection
          if (n==NUMBER_OF_BQ_DEVICES)
            return n;
        }
        else
        {
          //break internal loop
          n = NUMBER_OF_BQ_DEVICES;
        }
      }
    }

    i--;
  }
  
  return 0;
}

/**
* @brief Function Name: bq_pack_init.                                              
* @brief Description  : Configure each BQ device in the stack and initialize 
* data structures for whole BQ stack .
* @param parameters   :  none                                                  
* @return Value       :  return  if there isn't problems while init the pack                                                   
*/
short bq_pack_init(void)
{
  unsigned char i;
  
  for (i=0; i<NUMBER_OF_BQ_DEVICES; i++)
  {
    //*Init cell count for each BQ device*/ 
    bq_pack.bq_devs[i].cell_count = MAX_CELLS_NUMBER_IN_BQ;
       
    //*Configure each BQ device*/
    if(conf_bq_dev(&bq_pack.bq_devs[i],bq_pack.bq_devs[i].cell_count) == 1)
    {
    	return 1;
    }

    //*Initilize data structures*/    
    if(init_bq_dev_data_structure(&bq_pack.bq_devs[i]) == 1)
    {
    	return 1;
    }
    
    //*Read cell voltage*/
    if(bq_dev_read_cell_voltage(&bq_pack.bq_devs[i]) == 0)
    {
    	return 1;
    }
  }
  //battery manager initial mode
  bq_pack.error_status = 0;
  bq_pack.voltage = 0;
  bq_pack.Cell_Bal = CELL_BALANCING_EN;
  return 0;
}


/**
* @brief Function Name: update_bq_pack_data                                                  
* @brief Description  : Reads data from BQ DEV registers and updates BQ PACK 
* structure 
* @param parameters   : none                                                    
* @return Value       : none                                                    
*/     
unsigned char update_bq_pack_data(void)
{
  unsigned char i, cell_cnt;
  long stack_voltage = 0;
  unsigned short high_temp;
  unsigned short high_temp_num = 0;
  unsigned short total_temps = 0;
  unsigned short high_cell;
  unsigned short low_cell;
  unsigned short high_cell_num = 0;
  unsigned short low_cell_num = 0;
  unsigned short total_cells = 0;
  unsigned short highest_crc = 0;
  unsigned short highest_crc_num = 0;
  //read the pack data
  for (i=0; i<NUMBER_OF_BQ_DEVICES; i++)
  {
	  bq_pack.bq_devs[i].Timestamp = Clock_getTicks();


    //*Read each device status*/
	bq_dev_toggle_gpio(&bq_pack.bq_devs[i]);

    bq_dev_read_status(&bq_pack.bq_devs[i]);
    
    //*Read each cell voltage and calculate BQ Pack voltage*/
    bq_dev_read_cell_voltage(&bq_pack.bq_devs[i]);
    

    //Identify the lowest and highest voltage in the pack
    if (i == 0)
    {
      low_cell = bq_pack.bq_devs[i].cell_voltage[0];
      high_cell = bq_pack.bq_devs[i].cell_voltage[0];
      highest_crc =  bq_pack.bq_devs[i].crc_error_count;
      highest_crc_num = 0;
    }
    if (bq_pack.bq_devs[i].crc_error_count > highest_crc)
    {
    	highest_crc = bq_pack.bq_devs[i].crc_error_count;
    	highest_crc_num = i;
    }
    //calculate the pack voltage
    for (cell_cnt=0; cell_cnt<bq_pack.bq_devs[i].cell_count; cell_cnt++)
    {
      stack_voltage += bq_pack.bq_devs[i].cell_voltage[cell_cnt];
      
      if (bq_pack.bq_devs[i].cell_voltage[cell_cnt] 
          < low_cell)
      {
        low_cell = bq_pack.bq_devs[i].cell_voltage[cell_cnt];
        low_cell_num = total_cells;
      }
      else if (bq_pack.bq_devs[i].cell_voltage[cell_cnt] 
               > high_cell)
      {
        high_cell = bq_pack.bq_devs[i].cell_voltage[cell_cnt];
        high_cell_num = total_cells;
      }
      total_cells++;
    }
    
    //*Read each device errors*/
    bq_dev_read_errors(&bq_pack.bq_devs[i]);
     
    //*Read each device temperature*/
    bq_dev_read_temps(&bq_pack.bq_devs[i]);


    if (i == 0)
    {
    	if(bq_pack.bq_devs[i].temperature1 > bq_pack.bq_devs[i].temperature2)
    		high_temp = bq_pack.bq_devs[i].temperature1;
    	else
    	{
    		high_temp = bq_pack.bq_devs[i].temperature2;
    		high_temp_num = 1;
    	}
    }
    else
    {
    	if(bq_pack.bq_devs[i].temperature1 > high_temp)
    	{
    		high_temp = bq_pack.bq_devs[i].temperature1;
    		high_temp_num = total_temps;
    	}
    	if(bq_pack.bq_devs[i].temperature2 > high_temp)
    	{
    		high_temp = bq_pack.bq_devs[i].temperature2;
    		high_temp_num = total_temps+1;
    	}
    }



    bq_dev_read_gpai(&bq_pack.bq_devs[i]);
    total_cells++;
    total_temps = total_temps+2;
  }
  
  //*Save BQ Pack voltage*/
  bq_pack.voltage = stack_voltage;
  bq_pack.highest_temp = high_temp;
  bq_pack.highest_temp_num = high_temp_num;
  bq_pack.lowest_cell_volts = low_cell;
  bq_pack.highest_cell_volts = high_cell;
  bq_pack.lowest_cell_num = low_cell_num;
  bq_pack.highest_cell_num = high_cell_num;
  bq_pack.highest_crc = highest_crc;
  bq_pack.highest_crc_num = highest_crc_num;
  return 0;
}

/**
* @brief Function Name: bq_pack_start_conv   .                                               
* @brief Description  : triggers the ADC on the BQ devices.
* @param parameters   : none                                                    
* @return Value       : none                                                    
*/     
unsigned char bq_pack_start_conv(void)
{
  //Should address all devices in the stack
  if(bq_dev_write_reg(BROADCAST_ADDR, ADC_CONVERT_REG, ADC_CONVERT_VAL) == 0)
  {
  	return 1;
  }
  return 0;
}

/**
* @brief Function Name: CheckFaultConditions.                                                 
* @brief Description  : Check fault conditions on the pack.
* @param parameters   : none                                                    
* @return Value       : none                                                    
*/     
void CheckFaultConditions(void)
{
  //check for POT or COV conditions
	int warning = Bike.Warning.all;
	char error = 0; //no error
	int i;
  if (check_for_pot())
  {
    bq_pack.error_status |= STATUS_ERROR_POT;
  }
  else
  {
	  bq_pack.error_status &= ~STATUS_ERROR_POT;
  }
  //Check for CUV condition
  if (check_for_cuv())
  {
    bq_pack.error_status |= STATUS_ERROR_CUV;
  }
  else
  {
	  bq_pack.error_status &= ~STATUS_ERROR_CUV;
  }

  if (check_for_cov())
  {
	  bq_pack.error_status |= STATUS_ERROR_COV;
  }
  else
  {
	  bq_pack.error_status &= ~STATUS_ERROR_COV;
  }

  if(check_for_timeout())
  {
	  bq_pack.error_status |= STATUS_ERROR_TIMEOUT;
	  Bike.Warning.bit.BMM_loss = 1;
	  error = 1;
  }
  else
  {
	  bq_pack.error_status &= ~STATUS_ERROR_TIMEOUT;
  }

  switch (Bike.Mode)
  {
  case MODE_CHARGE:
    if (bq_pack.lowest_cell_volts <= UV_WARNING_THRESHOLD_CHARGE)
    {
    	Bike.Warning.bit.UV = 1;
    	error = 1;
    	if (bq_pack.lowest_cell_volts <= UV_FAULT_THRESHOLD_CHARGE)
    	{
    		Bike.Fault.bit.UV = 1;
    		error = 1;
    	}
    }
	else
	{
		Bike.Warning.bit.UV = 0; //unlatch warning
	}
    if (bq_pack.highest_cell_volts >= OV_WARNING_THRESHOLD_CHARGE)
    {
    	Bike.Warning.bit.OV = 1;
    	error = 1;
    	if (bq_pack.lowest_cell_volts >= UV_FAULT_THRESHOLD_CHARGE)
    	{
    		Bike.Fault.bit.OV = 1;
    		error = 1;
    	}
    }
	else
	{
		Bike.Warning.bit.OV = 0; //unlatch warning
	}
		if(bq_pack.highest_temp >= OT_WARNING_THRESHOLD_CHARGE )
		{
			Bike.Warning.bit.OT = 1;
			error = 1;
			if(bq_pack.highest_temp >= OT_FAULT_THRESHOLD_CHARGE )
			{
				Bike.Fault.bit.OT = 1;
				error = 1;
			}
		}
		else
		{
			Bike.Warning.bit.OT = 0; //unlatch warning
		}
		if(bq_pack.highest_crc > BMM_LOSS_THRESHOLD)
		{
			Bike.Warning.bit.BMM_loss = 1;
		}
    break;
  case MODE_RACE:
	    if (bq_pack.lowest_cell_volts <= UV_WARNING_THRESHOLD_RACE)
	    {
	    	Bike.Warning.bit.UV = 1;
	    	error = 1;
	    	if (bq_pack.lowest_cell_volts <= UV_FAULT_THRESHOLD_RACE)
	    	{
	    		Bike.Fault.bit.UV = 1;
	    		error = 1;
	    	}
	    }
		else
		{
			Bike.Warning.bit.UV = 0; //unlatch warning
		}
	    if (bq_pack.highest_cell_volts >= OV_WARNING_THRESHOLD_RACE)
	    {
	    	Bike.Warning.bit.OV = 1;
	    	error = 1;
	    	if (bq_pack.lowest_cell_volts >= UV_FAULT_THRESHOLD_RACE)
	    	{
	    		Bike.Fault.bit.OV = 1;
	    		error = 1;
	    	}
	    }
		else
		{
			Bike.Warning.bit.OV = 0; //unlatch warning
		}
		if(bq_pack.highest_temp >= OT_WARNING_THRESHOLD_RACE )
		{
			Bike.Warning.bit.OT = 1;
			error = 1;
			if(bq_pack.highest_temp >= OT_FAULT_THRESHOLD_RACE )
			{
				Bike.Fault.bit.OT = 1;
				error = 1;
			}
		}
		else
		{
			Bike.Warning.bit.OT = 0;
		}
		if(bq_pack.highest_crc > BMM_LOSS_THRESHOLD)
		{
			Bike.Warning.bit.BMM_loss = 1;
		}
    break;
  case MODE_SLEEP:
	    if (bq_pack.lowest_cell_volts <= UV_WARNING_THRESHOLD_SLEEP)
	    {
	    	Bike.Warning.bit.UV = 1;
	    	error = 1;
	    	if (bq_pack.lowest_cell_volts <= UV_FAULT_THRESHOLD_SLEEP)
	    	{
	    		Bike.Fault.bit.UV = 1;
	    		error = 1;
	    	}
	    }
		else
		{
			Bike.Warning.bit.UV = 0; //unlatch warning
		}
	    if (bq_pack.highest_cell_volts >= OV_WARNING_THRESHOLD_SLEEP)
	    {
	    	Bike.Warning.bit.OV = 1;
	    	error = 1;
	    	if (bq_pack.lowest_cell_volts >= UV_FAULT_THRESHOLD_SLEEP)
	    	{
	    		Bike.Fault.bit.OV = 1;
	    		error = 1;
	    	}
	    }
		else
		{
			Bike.Warning.bit.OV = 0; //unlatch warning
		}
		if(bq_pack.highest_temp >= OT_WARNING_THRESHOLD_SLEEP )
		{
			Bike.Warning.bit.OT = 1;
			error = 1;
			if(bq_pack.highest_temp >= OT_FAULT_THRESHOLD_SLEEP )
			{
				Bike.Fault.bit.OT = 1;
				error = 1;
			}
		}
		else
		{
			Bike.Warning.bit.OT = 0;
		}
		if(bq_pack.highest_crc > BMM_LOSS_THRESHOLD)
		{
			Bike.Warning.bit.BMM_loss = 1;
		}
    break;
  }

  if (error || warning != Bike.Warning.all)
  {
	  Event_post(EVT_Error,Event_Id_00);
  }

}



/**
* @brief Function Name: CellBalancing.                                   
* @brief Description  : Identify the unbalanced cells and enables the bypass 
* resistors to balance the cells while in charging mode .
* @param parameters   : none                                                     
* @return Value       : none                                                    
*/     
void CellBalancing(void)
{

  unsigned short dev_id;

  //cell balancing enabled by event?
  if (bq_pack.Cell_Bal)
  {
	  	//cell balancing enabled by event? ->YES
        //cell balancing achieved?
        if ((bq_pack.highest_cell_volts - bq_pack.lowest_cell_volts) >= 
          get_u32_value(BALANCE_VOLTS_THRESHOLD))
        { //cell balancing achieved? ->No, 
          //then reset balancing timer & continue balancing
          for (dev_id=0; dev_id<NUMBER_OF_BQ_DEVICES; dev_id++)
          {
            //*Enable bypass resistor for all balanced cells*/ 
            enable_bypass_resistor(dev_id, (~find_imbalanced_cell(dev_id)));
          }
          
        }
        else
        {
        	//cell balancing achieved? ->YES,
          //Disable bypass resistors
          while(disable_all_bypass_resistors() == 1 && check_for_timeout() == 0);
        }
    }
    else //If balancing disabled...
    {

      //*Disable all bypass resistors*/
      while(disable_all_bypass_resistors() == 1 && check_for_timeout() == 0);
    }
}



/**
* @brief Function Name: BatteryPackManager    .                                             
* @brief Description  : Battery Manger Function.
* @param parameters   : none                                                    
* @return Value       : none                                                    
*/     
void BatteryPackManager(void)
{
	CheckFaultConditions();
	CellBalancing();
}

/**
* @brief Function Name: get_bq_pack_voltage.                                                 
* @brief Description  : Returns Pack voltage parameter.
* @param parameters   : none                                                    
* @return Value       : returns last known pack voltage                                                    
*/     
unsigned short get_bq_pack_voltage(void)
{
  return bq_pack.voltage;
}










/******************************************************************************/
/*                            Local functions                                 */
/******************************************************************************/
/**
* @brief Function Name:  conf_bq_dev                  .                              
* @brief Description  :  configures each BQ76PL536 included in the stack.
* @param parameters   :  number of BQ device in the stack                                                   
* @return Value       :  none                                                   
*/     
unsigned char conf_bq_dev(bq_dev_t* this, int cell_num)
{
  unsigned short temp,temp2;
  
  if(cell_num == 6)
  {
	  if(bq_dev_write_reg(this->device_address, ADC_CONTROL_REG, ADC_CONTROL_VAL_6) == 0)
	  {
		return 1;
	  }
  }
  else if(cell_num == 5)
  {
	  if(bq_dev_write_reg(this->device_address, ADC_CONTROL_REG, ADC_CONTROL_VAL_5) == 0)
	  	  {
	  		return 1;
	  	  }
  }
  else if(cell_num == 4)
  {
	  if(bq_dev_write_reg(this->device_address, ADC_CONTROL_REG, ADC_CONTROL_VAL_4) == 0)
	  	  {
	  		return 1;
	  	  }
  }

  if(bq_dev_write_reg(this->device_address, IO_CONTROL_REG, IO_CONTROL_VAL) == 0)
  {
  	return 1;
  }
  
  if(bq_dev_write_reg(this->device_address, CB_CTRL_REG, CB_CTRL_VAL) == 0)
  {
  	return 1;
  }
  if(bq_dev_write_reg(this->device_address, CB_TIME_REG, CB_TIME_VAL) == 0)
  {
  	return 1;
  }

  if(bq_dev_write_reg(this->device_address, SHDW_CTRL_REG, SHDW_CTRL_ACCESS_EN_VAL) == 0)
  {
  	return 1;
  }

  if(bq_dev_write_reg(this->device_address, FUNCTION_CONFIG_REG, FUNC_CONFIG_VAL_6) == 0)
  {
  	return 1;
  }
  
  if(bq_dev_write_reg(this->device_address, SHDW_CTRL_REG, SHDW_CTRL_ACCESS_EN_VAL) == 0)
  {
  	return 1;
  }
  
  if(bq_dev_write_reg(this->device_address, IO_CONFIG_REG, IO_CONFIG_VAL) ==0)
  {
  	return 1;
  }

  temp = get_u32_value(COV_THRESHOLD);
  if (temp > 2000)
    temp = (temp - 2000)/50;
  else
    temp = 0;
  
  if(bq_dev_write_reg(this->device_address, SHDW_CTRL_REG, SHDW_CTRL_ACCESS_EN_VAL) == 0)
  {
  	return 1;
  }
  if(bq_dev_write_reg(this->device_address, CONFIG_COV_REG, CONFIG_COV_VAL | temp) == 0)
  {
  	return 1;
  }
  if(bq_dev_write_reg(this->device_address, SHDW_CTRL_REG, SHDW_CTRL_ACCESS_EN_VAL) == 0)
  {
  	return 1;
  }
  if(bq_dev_write_reg(this->device_address, CONFIG_COVT_REG,
                   CONFIG_COVT_VAL | get_u16_value(COV_TIME)) == 0)
  {
  	return 1;
  }

  temp = get_u32_value(CUV_THRESHOLD);
  if (temp > 700)
    temp = (temp - 700)/100;
  else
    temp = 13;  /*Def CUV Threshold value = 2000mV*/
  if(bq_dev_write_reg(this->device_address, SHDW_CTRL_REG, SHDW_CTRL_ACCESS_EN_VAL) == 0)
  {
  	return 1;
  }
  /*Def CUV Threshold = 2V*/
  if(bq_dev_write_reg(this->device_address, CONFIG_CUV_REG, CONFIG_CUV_VAL | temp) == 0)
  {
  	return 1;
  }
  if(bq_dev_write_reg(this->device_address, SHDW_CTRL_REG, SHDW_CTRL_ACCESS_EN_VAL) == 0)
  {
  	return 1;
  }
  if(bq_dev_write_reg(this->device_address, CONFIG_CUVT_REG, CONFIG_CUVT_VAL
                   | get_u16_value(CUV_TIME)) == 0)
  {
  	return 1;
  }


/*  temp = get_u32_value(PACK_OVER_TEMP1);
  if (temp <= 40)
    temp = 1;
  else if (temp >= 90)
    temp = 11;
  else
	  temp = (temp - 35)/5;

  temp2 = get_u32_value(PACK_OVER_TEMP2);
  if (temp2 <= 40)
    temp2 = 1;
  else if (temp2 >= 90)
    temp2 = 11;
  else
	  temp2 = (temp2 - 35)/5


  temp = (temp2<<4);
*/

  temp = dPACK_OVER_TEMP1_Config + (dPACK_OVER_TEMP2_Config << 4);

  if(bq_dev_write_reg(this->device_address, SHDW_CTRL_REG, SHDW_CTRL_ACCESS_EN_VAL) == 0)
  {
  	return 1;
  }
  if(bq_dev_write_reg(this->device_address, CONFIG_OT_REG, temp) == 0)
  {
  	return 1;
  }
  if(bq_dev_write_reg(this->device_address, SHDW_CTRL_REG, SHDW_CTRL_ACCESS_EN_VAL) == 0)
  {
  	return 1;
  }
  if(bq_dev_write_reg(this->device_address, CONFIG_OTT_REG, get_u16_value(PACK_OT_TIME1)) == 0)
  {
  	return 1;
  }

  this -> crc_error_count = 0;
  this -> timeout_count = 0;
  return 0;
}

/**
* @brief Function Name: init_bq_dev_data_structure .                                                
* @brief Description  : initializes the structure that stores the information 
* of each BQ device .
* @param parameters   : number of BQ device in the stack                                                      
* @return Value       : none                                                    
*/     
unsigned char init_bq_dev_data_structure(bq_dev_t* this)
{
  if(bq_dev_read_reg(this->device_address, DEVICE_STATUS_REG, 1, DISCARD_CRC,
                 (unsigned char*) &this->device_status) == 0)
                 {
                 	return 1;
                 }
    
  if(bq_dev_read_reg(this->device_address, TEMPERATURE1_L_REG, 2, DISCARD_CRC,
                 (unsigned char*) &this->temperature1) == 0)
                 {
                 	return 1;
                 }
  if(bq_dev_read_reg(this->device_address, TEMPERATURE2_L_REG, 2, DISCARD_CRC,
                 (unsigned char*) &this->temperature2) == 0)
                 {
                 	return 1;
                 }

  /*Errors handling*/
  if(bq_dev_read_reg(this->device_address, ALERT_STATUS_REG, 1, DISCARD_CRC,
                 (unsigned char*) &this->alert_status) == 0)
                 {
                 	return 1;
                 }
  if(bq_dev_read_reg(this->device_address, FAULT_STATUS_REG, 1, DISCARD_CRC,
                 (unsigned char*) &this->fault_status) == 0)
                 {
                 	return 1;
                 }
  if(bq_dev_read_reg(this->device_address, COV_FAULT_REG, 1, DISCARD_CRC,
                 (unsigned char*) &this->cov_fault) == 0)
                 {
                 	return 1;
                 }
  if(bq_dev_read_reg(this->device_address, CUV_FAULT_REG, 1, DISCARD_CRC,
                 (unsigned char*) &this->cuv_fault) == 0)
                 {
                 	return 1;
                 }

  /*Clear alerts and faults (May not be so trivial...)*/
  bq_dev_clear_alerts(this);
  bq_dev_clear_faults(this);  
  
  return 0;
}


/**
* @brief Function Name: bq_dev_read_cell_voltage.                                                  
* @brief Description  : reads the cell voltage on a specific BQ device.
* @param parameters   : BQ device number in the stack                                                   
* @return Value       : returns the cell voltage read from the BQ device                                                    
*/     
short bq_dev_read_cell_voltage(bq_dev_t* this)
{
  unsigned char* pPtr;
  unsigned char data[12];
  int temp;
  short i, ret_val;
  unsigned long voltage_comput;
  unsigned short cell_count = this->cell_count;
  
  ret_val = bq_dev_read_reg(this->device_address, VCELL1_L_REG, 
            (cell_count<<1) , DISCARD_CRC,
            (unsigned char *) &data);
  if(ret_val == 0)
  {
	  return 0;
  }
  
  for (i=0; i<cell_count; i++)
  {
    //Swap the data in the array as BQ dev returns data in Big Endian notation
//    pPtr = (unsigned char *)(&this->cell_voltage[i]);
//    temp = *pPtr;
//    *(pPtr) = *(pPtr+1);
//    *(pPtr+1) = temp;

//
//
//    //compute cell voltage -> cell_voltage=VCELL x 6250/16383
//    voltage_comput = this->cell_voltage[i] * (unsigned long)(adc_step_mul);
//    this->cell_voltage[i] = voltage_comput/((unsigned long)(adc_step_div));

	  __byte(&temp,1) = data[i*2];
	  __byte(&temp,0) = data[(i*2)+1];

	      //compute cell voltage -> cell_voltage=VCELL x 6250/16383
	  voltage_comput = temp * (unsigned long)(adc_step_mul);
	  this->cell_voltage[i] = voltage_comput/((unsigned long)(adc_step_div));
  }
  
  return ret_val;
}



/**
* @brief Function Name: bq_dev_clear_alerts .                                                 
* @brief Description  : Clear the alert flags on the BQ device.
* @param parameters   : specific device ID                                                    
* @return Value       : none                                                    
*/     
unsigned char bq_dev_clear_alerts(bq_dev_t* this)
{
  unsigned char Value;

  //clear alert bit in device status register
  if(bq_dev_read_reg(this->device_address, DEVICE_STATUS_REG, 1, DISCARD_CRC,
                 (unsigned char*) &Value) == 0)
  {
	  return 1;
  }
  Value |= BIT5; 
  //set alert bit as 1
  bq_dev_write_reg(this->device_address, DEVICE_STATUS_REG, Value) == 0;

  Value &= ~BIT5;
  //clear alert bit
  bq_dev_write_reg(this->device_address, DEVICE_STATUS_REG, Value);

  
  //Read ALERT_STATUS_REG register
  bq_dev_read_reg(this->device_address, ALERT_STATUS_REG, 1, DISCARD_CRC,
                 (unsigned char*) &Value);

  Value &= ~BIT0; //don't clear OT1
  Value &= ~BIT1; //don't clear OT2

  //Write 1's to ALERT_STATUS_REG register
  if(bq_dev_write_reg(this->device_address, ALERT_STATUS_REG, Value) == 0)
  {
	  return 1;
  }
  
  Value = 0x00;
  // Write 0's ALERT_STATUS_REG register
  if(bq_dev_write_reg(this->device_address, ALERT_STATUS_REG, Value) == 0)
  {
	  return 1;
  }
  return 0;
 
}


/**
* @brief Function Name: bq_dev_clear_faults.                                                 
* @brief Description  : Clears the fault flags on the BQ device.
* @param parameters   : Device ID                                                     
* @return Value       : none                                                    
*/     
unsigned char bq_dev_clear_faults(bq_dev_t* this)
{
  unsigned char Value;
  
    //clear fault bit in device status register
  if(bq_dev_read_reg(this->device_address, DEVICE_STATUS_REG, 1, DISCARD_CRC,
                 (unsigned char*) &Value) == 0)
                 {
               	  return 1;
                 }
  Value |= BIT6; 
  //set fault bit as 1
//  if(bq_dev_write_reg(this->device_address, DEVICE_STATUS_REG, Value) == 0)
//  {
//	  return 1;
//  }
  bq_dev_write_reg(this->device_address, DEVICE_STATUS_REG, Value);
  Value &= ~BIT6; 
  //clear fault bit
//  if(bq_dev_write_reg(this->device_address, DEVICE_STATUS_REG, Value) == 0)
//  {
//	  return 1;
//  }
  bq_dev_write_reg(this->device_address, DEVICE_STATUS_REG, Value);

  //Read FAULT_STATUS register
  bq_dev_read_reg(this->device_address, FAULT_STATUS_REG, 1, DISCARD_CRC, 
                 (unsigned char*) &Value); 
  //Write 1's to FAULT_STATUS register
  Value &= ~BIT0; //don't clear cov
  Value &= ~BIT1; //don't clear cuv
  bq_dev_write_reg(this->device_address, FAULT_STATUS_REG, Value);
  
  Value = 0x00;
  // Write 0's FAULT_STATUS register
  bq_dev_write_reg(this->device_address, FAULT_STATUS_REG, Value);
  

  return 0;
}

unsigned char bq_dev_set_gpio(bq_dev_t* this)
{
  unsigned char Value;

    //clear fault bit in device status register
  if(bq_dev_read_reg(this->device_address, IO_CONTROL_REG, 1, DISCARD_CRC,
                 (unsigned char*) &Value) == 0)
                 {
               	  return 1;
                 }
  Value |= BIT6;
  //set GPIO_OUT bit as 1
  if(bq_dev_write_reg(this->device_address, IO_CONTROL_REG, Value) == 0)
  {
	  return 1;
  }
  return 0;
}

unsigned char bq_dev_clear_gpio(bq_dev_t* this)
{
  unsigned char Value;

    //clear fault bit in device status register
  if(bq_dev_read_reg(this->device_address, IO_CONTROL_REG, 1, DISCARD_CRC,
                 (unsigned char*) &Value) == 0)
  {
	  return 1;
  }
  Value &= ~BIT6;
  //clear GPIO_OUT
  if(bq_dev_write_reg(this->device_address, IO_CONTROL_REG, Value) == 0)
  {
	  return 1;
  }

  return 0;

}

short bq_dev_read_gpai(bq_dev_t* this)
{
  unsigned char data[2];

  //Bytes need to be swaped as BQ device supports Big Endian
  if(bq_dev_read_reg(this->device_address, GPAI_L_REG, 2, DISCARD_CRC,
                 (unsigned char*) &data[0]) == 0)
  {
  	return 1;
  }
  this->GPAI = ((data[0] << 8) | data[1]);


  return 0;
}


unsigned char bq_dev_toggle_gpio(bq_dev_t* this)
{
  unsigned char Value;

    //clear fault bit in device status register
  if(bq_dev_read_reg(this->device_address, IO_CONTROL_REG, 1, DISCARD_CRC,
                 (unsigned char*) &Value) == 0)
  {
	  return 1;
  }
  Value ^= BIT6;
  //toggle GPIO_OUT
  if(bq_dev_write_reg(this->device_address, IO_CONTROL_REG, Value) == 0)
  {
	  return 1;
  }

  return 0;
}

/**
* @brief Function Name: bq_dev_read_errors.                                                 
* @brief Description  : Reads the device status, alert status, fault status,
* COV status, CUV status registers.
* @param parameters   : Device ID                                                    
* @return Value       : zero when the BQ device was read sucessfully                                                    
*/     
short bq_dev_read_errors(bq_dev_t* this)
{
  if(bq_dev_read_reg(this->device_address, DEVICE_STATUS_REG, 1, DISCARD_CRC,
                 (unsigned char*) &this->device_status) == 0)
  {
  	return 1;
  }
  if(bq_dev_read_reg(this->device_address, ALERT_STATUS_REG, 1, DISCARD_CRC,
                 (unsigned char*) &this->alert_status) == 0)
  {
  	return 1;
  }
  if(bq_dev_read_reg(this->device_address, FAULT_STATUS_REG, 1, DISCARD_CRC,
                 (unsigned char*) &this->fault_status) == 0)
  {
  	return 1;
  }
  if(bq_dev_read_reg(this->device_address, COV_FAULT_REG, 1, DISCARD_CRC,
                 (unsigned char*) &this->cov_fault) == 0)
  {
  	return 1;
  }
  if(bq_dev_read_reg(this->device_address, CUV_FAULT_REG, 1, DISCARD_CRC,
                 (unsigned char*) &this->cuv_fault) == 0)
  {
  	return 1;
  }
  
  return 0;
}


/**
* @brief Function Name: bq_dev_read_temps.                                                 
* @brief Description  : reads the temperature registers.
* @param parameters   : Device ID                                                    
* @return Value       : zero when the BQ device was read sucessfully                                                    
*/     
short bq_dev_read_temps(bq_dev_t* this)
{
  unsigned char data[2];
  float temp;

  //RTS = (REGMSB × 256 + REGLSB) / 33104
  // RTS = .2 THEN TEMP = 40C
  // RTS = .4 then temp = 90C
  // slope = 250
  // intercept = -10
  // temp = (rts * 250) - 10
  //Bytes need to be swaped as BQ device supports Big Endian
  if(bq_dev_read_reg(this->device_address, TEMPERATURE1_L_REG, 2, DISCARD_CRC,
                 (unsigned char*) &data[0]) == 0)
  {
  	return 1;
  }

  this->temperature1ratio  = ((float)((data[0] << 8) | data[1]))/33104;
  this->temperature1 = (this->temperature1ratio * 250) - 10;

  if(bq_dev_read_reg(this->device_address, TEMPERATURE2_L_REG, 2, DISCARD_CRC,
                 (unsigned char*) &data[0]) == 0)
  {
  	return 1;
  }
  this->temperature2ratio  = ((float)((data[0] << 8) | data[1]))/33104;
  this->temperature2 = (this->temperature2ratio * 250) - 10;
 
  return 0;
}

/**
* @brief Function Name: bq_dev_read_status.                                                 
* @brief Description  : reads the devoce status register.
* @param parameters   : Device ID                                                    
* @return Value       : zero when the BQ device was read sucessfully  
*/  
unsigned char bq_dev_read_status(bq_dev_t* this)
{
  if(bq_dev_read_reg(this->device_address, DEVICE_STATUS_REG, 1,
         DISCARD_CRC, (unsigned char*) &this->device_status) == 0)
  {
  	return 1;
  }
  return 0;
}




/**
* @brief Function Name: check_for_cov    .                                             
* @brief Description  : Checks for COV conditions.
* @param parameters   : none                                                    
* @return Value       : 1 when the COV condition exist, 0 when it doesn't                                                    
*/     
short check_for_cov(void)
{

  unsigned short i, cell;

  for (i=0; i<NUMBER_OF_BQ_DEVICES; i++)
  {
    if (bq_pack.bq_devs[i].fault_status & (1<<FAULT_COV_POS))
    {
      //Check if error condition is still in effect
      for (cell=0; cell < bq_pack.bq_devs[i].cell_count; cell++)
      {
        if (bq_pack.bq_devs[i].cov_fault & (1<<cell))
        {
          if (bq_pack.bq_devs[i].cell_voltage[cell] 
              > get_u32_value(COV_RECOVERY_THRESHOLD))
          {
            //COV condition still present
            return 1; //error
          }
          else
          {
            //COV condition cleared
            //clear FAULT flag
            while(bq_dev_write_reg(bq_pack.bq_devs[i].device_address,
                             FAULT_STATUS_REG, (1<<FAULT_COV_POS)) == 0 && check_for_timeout() == 0);
            while(bq_dev_write_reg(bq_pack.bq_devs[i].device_address,
                             FAULT_STATUS_REG, (0<<FAULT_COV_POS)) == 0 && check_for_timeout() == 0);
          }
        }
      }
    }
  }
  
  return 0;
}


/**
* @brief Function Name: check_for_cuv .                                                
* @brief Description  : Checks for CUV conditions.
* @param parameters   : none                                                    
* @return Value       : 1 when the condition exist, 0 when it doesn't                                                    
*/     
short check_for_cuv(void)
{
  unsigned short i, cell;

  for (i=0; i<NUMBER_OF_BQ_DEVICES; i++)
  {
    if (bq_pack.bq_devs[i].fault_status & (1<<FAULT_CUV_POS))
    {
      //Check if error condition is still in effect
      for (cell=0; cell < bq_pack.bq_devs[i].cell_count; cell++)
      {
        if (bq_pack.bq_devs[i].cuv_fault & (1<<cell))
        {
          if (bq_pack.bq_devs[i].cell_voltage[cell] 
              < get_u32_value(CUV_RECOVERY_THRESHOLD))
          {
            //CUV condition still present
            return 1; //error
          }
          else
          {
            //CUV condition cleared
            //clear FAULT flag
            while(bq_dev_write_reg(bq_pack.bq_devs[i].device_address,
                            FAULT_STATUS_REG, (1<<FAULT_CUV_POS)) == 0 && check_for_timeout() == 0);
            while(bq_dev_write_reg(bq_pack.bq_devs[i].device_address,
                             FAULT_STATUS_REG, (0<<FAULT_CUV_POS)) == 0 && check_for_timeout() == 0);
          }
        }
      }
    }
  }
  
  return 0;
}


/**
* @brief Function Name: check_for_pot   .                                            
* @brief Description  : Checks for POT conditions.
* @param parameters   : none                                                    
* @return Value       : 1 when the condition exist, 0 when it doesn't                                                    
*/     
short check_for_pot(void)
{
  /*Temp sensor connected to first BQ device only*/
	int i;
	for (i=0; i<NUMBER_OF_BQ_DEVICES; i++)
	  {
//		if ((bq_pack.bq_devs[i].alert_status & (0x3<<ALERT_OT1_POS)) || (bq_pack.bq_devs[i].alert_status & (0x3<<ALERT_OT2_POS)))
//		{
//			return 1; //error
//		}
		if ((bq_pack.bq_devs[i].temperature1ratio >= dPACK_OVER_TEMP1_RATIO_CLEAR) || (bq_pack.bq_devs[i].temperature2ratio >= dPACK_OVER_TEMP2_RATIO_CLEAR))
		{
			return 1; //error
		}
		else
		{
		    while(bq_dev_write_reg(bq_pack.bq_devs[i].device_address,
		                     ALERT_STATUS_REG, (1<<ALERT_OT1_POS)) == 0 && check_for_timeout() == 0);
		    while(bq_dev_write_reg(bq_pack.bq_devs[i].device_address,
		                     ALERT_STATUS_REG, (0<<ALERT_OT1_POS)) == 0 && check_for_timeout() == 0);
		    while(bq_dev_write_reg(bq_pack.bq_devs[i].device_address,
		                     ALERT_STATUS_REG, (1<<ALERT_OT2_POS)) == 0 && check_for_timeout() == 0);
		    while(bq_dev_write_reg(bq_pack.bq_devs[i].device_address,
		                     ALERT_STATUS_REG, (0<<ALERT_OT2_POS)) == 0 && check_for_timeout() == 0);
		}


	  }
	return 0;
}

short check_for_timeout(void)
{
	int i;
	for (i=0; i<NUMBER_OF_BQ_DEVICES; i++)
	  {
		if (bq_pack.bq_devs[i].timeout_count >= BMM_LOSS_THRESHOLD)
		{
			return 1; //error
		}
	  }
	return 0;
}






/**
* @brief Function Name: copy_cell_voltage_values.                                                  
* @brief Description  : creates a backup of the last known cells voltages.
* @param parameters   : none                                                    
* @return Value       : none                                                    
*/     
//void copy_cell_voltage_values(void)
//{
//  unsigned short i, j, cell_cnt;
//
//  j = 0;
//  for (i=0; i<NUMBER_OF_BQ_DEVICES; i++)
//  {
//    /*Read each cell voltage and compare it vs initial cell voltage*/
//    for (cell_cnt=0; cell_cnt<bq_pack.bq_devs[i].cell_count; cell_cnt++)
//    {
//      cell_values[j++] = bq_pack.bq_devs[i].cell_voltage[cell_cnt];
//    }
//  }
//
//  return;
//}
    

/**
* @brief Function Name: cell_imbalance_threshold_reached  .                                             
* @brief Description  : Checks if the cell imbalance threshold is reached
* @param parameters   : none                                                    
* @return Value       : 1 when the condition exist, 0 when it doesn't                                                    
*/   
unsigned short cell_imbalance_threshold_reached(void)
{
  if ((bq_pack.highest_cell_volts - bq_pack.lowest_cell_volts) 
       > get_u32_value(CELL_IMBALANCE_FAIL_THRESHOLD))
    return 1;
  else
    return 0;
}

/**
* @brief Function Name: find_imbalanced_cell .                                                 
* @brief Description  : finds the imbalanced cells.
* @param parameters   : device ID                                                    
* @return Value       : Returns 0 if cells are balanced and other if there are 
* any imbalanced cells                                                     .
*/     
unsigned short find_imbalanced_cell(unsigned short in_dev_id)
{
  unsigned short cell_id, imb_cells_mask, cnt;
  
  cnt = 0;
  imb_cells_mask = 0xFFFF;
  
  /*Read each cell voltage and compare it vs lowest cell voltage*/
  for (cell_id=0; cell_id<bq_pack.bq_devs[in_dev_id].cell_count; cell_id++)
  {
    imb_cells_mask &= ~(1<<cnt);
    //changed to use lowest
    if ((bq_pack.bq_devs[in_dev_id].cell_voltage[cell_id]) - bq_pack.lowest_cell_volts  <
          get_u32_value(BALANCE_VOLTS_THRESHOLD))
    {
      imb_cells_mask |= (1<<cnt);
    }
    cnt++;
  }
  
  return imb_cells_mask;
}

/**
* @brief Function Name: enable_bypass_resistor.                                                 
* @brief Description  : Enable bypass resistors by controlling the CB outputs. 
* @param parameters   : Device ID, CB outputs to be controlled                                                    
* @return Value       : none                                                     
*/     
unsigned char enable_bypass_resistor(unsigned short in_dev_id, unsigned short in_value)
{
  if(bq_dev_write_reg(bq_pack.bq_devs[in_dev_id].device_address,
                   CB_TIME_REG, get_u32_value(BALANCE_TIME)) == 0)
  {
  	return 1;
  }
  if(bq_dev_write_reg(bq_pack.bq_devs[in_dev_id].device_address,
                   CB_CTRL_REG, (0x003F & in_value)) == 0)
  {
  	return 1;
  }
  
  bq_pack.bq_devs[in_dev_id].cell_bal = in_value;

  return 0;
}


/**
* @brief Function Name: disable_bypass_resistor.                                                 
* @brief Description  : disable bypass resistors by controlling the CB outputs .
* @param parameters   : Device ID, CB outputs to be controlled                                                    
* @return Value       : none                                                     
*/
unsigned char disable_bypass_resistor(unsigned short in_dev_id, unsigned short in_value)
{
  unsigned char reg_val;
  //Read CB_CTRL_REG register
  if(bq_dev_read_reg(bq_pack.bq_devs[in_dev_id].device_address, CB_CTRL_REG, 1,
                  DISCARD_CRC, (unsigned char*) &reg_val) == 0)
  {
  	return 1;
  }
  
  //Write 0's to the bits in the CB_CTRL_REG register
  reg_val &= ~(in_value);
  if(bq_dev_write_reg(bq_pack.bq_devs[in_dev_id].device_address,
                   CB_CTRL_REG, reg_val) == 0)
  {
  	return 1;
  }
  
  return 0;
}


/**
* @brief Function Name: disable_all_bypass_resistors.                                                 
* @brief Description  : disable all battery pack bypass resistors by controlling 
* the CB outputs .
* @param parameters   : none                                                    
* @return Value       : none                                                     
*/
unsigned char disable_all_bypass_resistors(void)
{
  unsigned short bq_dev_id;
  
  for (bq_dev_id=0; bq_dev_id<NUMBER_OF_BQ_DEVICES; bq_dev_id++)
  {
    if(bq_dev_write_reg(bq_pack.bq_devs[bq_dev_id].device_address, CB_CTRL_REG, 0) == 0)
    {
    	return 1;
    }
    bq_pack.bq_devs[bq_dev_id].cell_bal = 0;
  }

  return 0;
}


/*EOF*/

