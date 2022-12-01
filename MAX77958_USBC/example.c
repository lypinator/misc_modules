/**
 * @file    example.c
 * @brief   Example program for MAX77958 USB-C power negotiations module
 * @details 
 *          Notes:
 *              - PDO positioning starts at 1, not 0
 *              - If a USB-C cable is not rated for 100W, then the SRC will advertise for 60W max PDOs only
 *              - It takes ~300ms or so before you can request a new PDO (Experimental)
 *              - It takes ~70ms or so before you can request a PDO (Experimental)
 */

/* **** Includes **** */
#include "main.h"
#include "servoctrl.h"
#include "kws.h"
#include "scheduler.h"
#include "MAX77958.h"
#include <string.h>
#include <stdio.h>
#include "mxc_delay.h"
#include "nvic_table.h"
#include "tmr.h"
#include "led.h"

/* Globals */

MAX77958_USBC_DATA_t USBC_data;

    //TODO: Put these variables in some kind of USB-C Controller Structure so all PDOs are saved and stuff


/* **************************************************************************** */

int main(void)
{
    printf("USB-C Testing for MAX77958\n");
    MXC_Delay(MXC_DELAY_SEC(2)); // Create window for debugger to connect after reset
    //Initialize MAX77958
    printf("MAX77958 initialization returned %d (0 for Sucess)\n\n",MAX77958_Init());

    
    if(get_SRC_Cap(&USBC_data) < 0){
        printf("Error reading PDOs\n");
        //TODO: Handle errors

    }
    else{
        uint8_t PDMsg_reg;
        do{
            Poll_Reg(PD_STATUS0,&PDMsg_reg);
            MXC_Delay(500);
        } while(PDMsg_reg != Sink_PD_Evaluate_State_SrcCap_Received);

        //Read the PDO data once it is ready
        read_get_SRC_Cap(&USBC_data);
        printf("Number of PDOs: %i\n", USBC_data.num_PDOs);
    }

    printf("Source Information:\n\n");
    
    print_PDO_data(&USBC_data);    //Prints all the available PDOs

    printf("Testing Specific PDO finder...\n\n");
    printf("Attempting to find fixed PDO at %d mV and %d mA capabilities\n",USBC_20VOUT*50,USBC_3AOUT*10);
    
    
    uint8_t PDO_position = 0;
    
    if((PDO_position = find_SRC_Cap_target(&USBC_data, USBC_20VOUT, USBC_3AOUT))){
        set_SRC_Cap(&USBC_data,PDO_position);
    }
    else{
        //TODO: Handle if PDO not found
    
    }
    
    
    MXC_Delay(MXC_DELAY_SEC(2));


    printf("\nCycling through PDO voltages...\n\n");
    for(int i =0;i<USBC_data.num_PDOs;i++){
        printf("Selecting PDO Source %i\n",i+1);
        set_SRC_Cap(&USBC_data,i+1);

        MXC_Delay(300000);  //Minimum delay to allow USB-C bus SRC to allow for changes again-- Normally
                            //this should not matter because you won't keep changin PDOs over and over again

        MAX77958_USBC_Status1_REG_t temp;
        temp.reg_value = get_VBus_Voltage();
        printf("Current VBus reading from MAX77958 ADC    : %d V\n",(temp.VbADC+3));
    }

    while(1) {
     
    }
}
