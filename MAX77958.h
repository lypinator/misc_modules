#ifndef MAX77958_H
#define MAX77958_H

#include <stdio.h>
#include <stdint.h>
#include "MAX77958_def.h"



/*** Function Descriptions	***/

/**
* @brief        Initialize MAX77958 and I2C
* @param[in]    code - MAX77958 Opcode to write
* @param[in]    data - MAX77958 data for command
*
* @return       0 (Success), -1 (Failure)
*/
int32_t MAX77958_Init();

/**
* @brief        Write Opcode and data
* @param[in]    code - MAX77958 Opcode to write
* @param[in]    data - MAX77958 data for command
*
* @return       0 (Success), -1 (Failure)
*/
int32_t write_opcode(MAX77958_USBC_DATA_t *USBC_data);

/**
* @brief        Read Opcode and data
* @param[in]    data - MAX77958 data array to hold read data
* @param[in]    code - MAX77958 OpCode to verify function completion
*
* @return       0 (Success), -1 (Failure)
*/
int32_t read_opcode(MAX77958_USBC_DATA_t *USBC_data);

/**
* @brief        Write register at specified address
* @param[in]    register_address - MAX77958 register address to write
* @param[in]    len - Number of bytes to write
* @param[out]	data - MAX77958 data write to register address
*
* @return       0 (Success), -1 (Failure)
*/
int32_t write_reg(MAX77958_REG_ADD register_address, uint16_t len,  uint8_t *data);

/**
* @brief        Read data from specified address
* @param[in]    register_address - MAX77958 register address to read
* @param[in]    len - Number of bytes to read into the data array
* @param[out]	data - data array to hold data read from register address
*
* @return       0 (Success), -1 (Failure)
*/
int32_t read_reg(MAX77958_REG_ADD register_address, uint16_t len,  uint8_t *data);

/**
* @brief        Request the PDOs of the USB-C SRC
*
* @return       Number of PDOs (positive integer from 1 to 8), -1 for error
*/
int32_t get_SRC_Cap(MAX77958_USBC_DATA_t *USBC_data);

/**
* @brief        Read the PDOs of the USB-C SRC (Must call get_SRC_Cap() first)
* @param[in]    USBC_data - USBC data structure to hold the PDO information
*
* @return       Number of PDOs (positive integer from 1 to 8), -1 for error
*/
int32_t read_get_SRC_Cap(MAX77958_USBC_DATA_t *USBC_data);

/**
* @brief        Attempt to set PDO based on target voltage and current limit
* @param[in]	USBC_data 		- USBC data structure to hold the PDO information
* @param[in]	target_voltage	- target voltage to search for
* @param[in]	target_current	- target current limit to search for
*
* @return       0 (could not find matching PDO), PDO position (between 1-8)
*/
uint8_t find_SRC_Cap_target(MAX77958_USBC_DATA_t *USBC_data, USBC_PDO_Voltage_t target_voltage, USBC_PDO_Current_t target_current);

/**
* @brief        Set PDO position
* @param[in]	desired_PDO_pos	- PDO position to request
*
* @return       0 (Success), negative integer (Failure)
*/
int32_t set_SRC_Cap(MAX77958_USBC_DATA_t *USBC_data, uint8_t desired_PDO_pos);


/**
* @brief        Print PDO information
* @param[in]	USBC_data	 - USBC data structure to hold the PDO information
*
*/
void print_PDO_data(MAX77958_USBC_DATA_t *USBC_data);

/**
* @brief        Get the current PD message
*
* @return       Current PD message value
*/
uint8_t get_PD_msg( void );

/**
* @brief        Get the current SYS message
*
* @return       Current SYS message value
*/
uint8_t get_SYS_msg(void);

//TODO: Finish Functions Below
int32_t set_PPS(uint16_t target_votlage, uint8_t target_current);


#endif /* MAX77958_h */