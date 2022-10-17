#ifndef MAX77958_H
#define MAX77958_H

#include <stdio.h>
#include <stdint.h>


//I2C Paramaters
#define I2C_MASTER      MXC_I2C1    //MXC_I2C2: *I2C Port 2
#define I2C_FREQ        100000      //Frequency set in Hz (*100000: 100 kHz)
#define I2C_CONFIG      1           //0: I2C Slave Configuration,   1: *I2C Master Configuration

/**
* @brief        Initialize I2C object
*
* @return       0 (Success), -1 (Failure)
*/
int8_t dev_I2C_Init();

/**
* @brief        Write to a specified register of MAX77958
* @param[in]    dev_addr - MAX77958 device address
* @param[in]    reg_addr - MAX77958 register address to write
* @param[out]	data - byte value to write to address
*
* @return       0 (Success), -1 (Failure)
*/
int8_t dev_I2C_Write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len);

/**
* @brief        Read from a specified register of MAX77958
* @param[in]    dev_addr - MAX77958 device address
* @param[in]    reg_addr - MAX77958 register address to read
* @param[out]	data - pointer to location where data should be stored
*
* @return       0 (Success), -1 (Failure)
*/
int8_t dev_I2C_Read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len);


#endif /* MAX77958_h */