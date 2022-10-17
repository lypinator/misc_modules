#include "dev_i2c.h"
#include <stdio.h>
#include <stdint.h>
#include "mxc_delay.h"
#include "i2c.h"
#include "i2c_regs.h"

/*****  Global Variables *****/  

mxc_i2c_req_t reqMaster = {
    .addr = 0x00,
    .callback = NULL,
    .i2c = I2C_MASTER,
    .restart = 0,
    .rx_buf = NULL,
    .rx_len = 0,
    .tx_buf = NULL,
    .tx_len = 0
};

/***** Function Prototypes *****/

int8_t dev_I2C_Init();
int8_t dev_I2C_Read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len);
int8_t dev_I2C_Write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len);

int8_t dev_I2C_Init(){

     int rslt= E_NO_ERROR;

    //Initilize the I2C Port as a master
    if((rslt = MXC_I2C_Init(I2C_MASTER, I2C_CONFIG, 0)) != E_NO_ERROR){
        printf("Error initializing the I2C Port 2 (Error: %d)\n",rslt);
        return E_BAD_PARAM;
    }
    
    //Set the frequency of communication over I2C port
    if((rslt = MXC_I2C_SetFrequency(I2C_MASTER, I2C_FREQ)) < E_NO_ERROR){
        printf("Error setting I2C speed (Error: %d)\n",rslt);
        return E_BAD_PARAM;
    }

    return E_SUCCESS;

}


int8_t dev_I2C_Write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len){

    int rslt = E_NO_ERROR;

    //Allocate memory for register address and data
    uint8_t *TXData = (uint8_t *)malloc(sizeof(data)+sizeof(reg_addr));
    memcpy(TXData, &reg_addr, 1);
    memcpy(TXData+1, data, len);

    //Set paramaters for I2C read
    reqMaster.addr = dev_addr;          //Set device address 
    reqMaster.tx_buf = TXData;          //Pointer to register address
    reqMaster.tx_len = len+1;           //Number of bytes to send
    reqMaster.rx_buf = NULL;            //Pointer to structure to store read values
    reqMaster.rx_len = 0;               //Number of bytes to read

    //Send I2C data
    if ((rslt = MXC_I2C_MasterTransaction(&reqMaster)) != E_NO_ERROR) {
        //Communication error
        if(rslt != 1){
            printf("Error (%d) writing data: Device = 0x%X; Register = 0x%X\n", rslt, dev_addr, reg_addr);
            return E_UNDERFLOW;
        }
        //Message not acknowledged
        else{
            printf("Write was not acknowledged: Device = 0x%X; Register = 0x%X\n", dev_addr, reg_addr);
            return E_NO_RESPONSE;
        }
    }
    
    free(TXData);

    return E_NO_ERROR;
}


int8_t dev_I2C_Read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len){

    //TODO: Setupe I2C read
    int rslt = E_SUCCESS;

    //Set paramaters for I2C read
    reqMaster.addr = dev_addr;          //Set device address 
    reqMaster.tx_buf = &reg_addr;       //Pointer to register address
    reqMaster.tx_len = 1;               //Number of bytes to send
    reqMaster.rx_buf = data;            //Pointer to structure to store read values
    reqMaster.rx_len = len;             //Number of bytes to read

    if ((rslt = MXC_I2C_MasterTransaction(&reqMaster)) != E_NO_ERROR) {
        
        //Communication error
        if(rslt != 1){
            printf("Error (%d) reading data: Device = 0x%X; Register = 0x%X\n", rslt, dev_addr, reg_addr);
            return E_UNDERFLOW;
        }
        //Message not acknowledged
        else{
            printf("Read was not acknowledged: Device = 0x%X; Register = 0x%X\n", dev_addr, reg_addr);
            return E_NO_RESPONSE;
        }
    }

    return E_SUCCESS;
}