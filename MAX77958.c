#include "MAX77958.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "nvic_table.h"
#include "tmr.h"
#include "dev_i2c.h"
#include "mxc_delay.h"

/** I2C functions provided in dev_I2C files 
 * 
 *  int8_t dev_I2C_Write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len);
 *  int8_t dev_I2C_Read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len);
 *  int8_t dev_I2C_Init();
 */ 


/***    Globals ***/



//  Prototypes

/***    MAX77958 and I2C initialization function    ***/
int32_t MAX77958_Init();

/*** Basic MAX77958 register functions  ***/
int32_t write_reg(MAX77958_REG_ADD register_address, uint16_t len, uint8_t *data);
int32_t read_reg(MAX77958_REG_ADD register_address, uint16_t len, uint8_t *data);

/*** MAX77958 OpCode Basic Functions    ***/
int32_t write_opcode(MAX77958_USBC_DATA_t *USBC_data);
int32_t read_opcode(MAX77958_USBC_DATA_t *USBC_data);

/***    Source PDO Functions    ***/
uint8_t find_SRC_Cap_target(MAX77958_USBC_DATA_t *USBC_data, USBC_PDO_Voltage_t target_voltage, USBC_PDO_Current_t target_current);
int32_t get_SRC_Cap(MAX77958_USBC_DATA_t *USBC_data);
int32_t set_SRC_Cap(MAX77958_USBC_DATA_t *USBC_data, uint8_t desired_PDO_pos);
int32_t read_get_SRC_Cap(MAX77958_USBC_DATA_t *USBC_data);

/*** Poll Register  ***/
int32_t Poll_Reg(MAX77958_REG_ADD reg_add, uint8_t *reg_value);
uint8_t get_VBus_Voltage(void);
uint8_t get_PD_msg(void);
uint8_t get_SYS_msg(void);


/***  Print PDO Data  ***/
void print_PDO_data(MAX77958_USBC_DATA_t *USBC_data);
void print_fixed_PDO(MAX77958_PDO_FixedSupply_t PDO);
void print_augmented_PDO(MAX77958_PDO_Augmented_t APDO);
void print_battery_PDO(MAX77958_PDO_Battery_t PDO);

int32_t MAX77958_Init(){
    
    dev_I2C_Init();     //Setup the I2C peripheral

    //TODO: Insert any initialization code for starting the MAX77958 (EN high, Power on, etc.)

    uint8_t device_id = 0;
    int8_t rslt = 0;
    //Read and confirm the ID number
    if((rslt = read_reg(DEVICE_ID, (uint16_t)1, &device_id))){
        printf("Error initializing MAX77958! Error Code: %d\n",rslt);
        return (rslt);
    }
    if (device_id != MAX77958_DEV_ID){
        printf("MAX77958 Device ID does not match! Expected Device ID: 0x%X but got 0x%X\n", MAX77958_DEV_ID, device_id);
        return (-1);
    }
    printf("MAX77958 initialized succesfully and Device ID confirmed\n");
    return 0;
}

int32_t write_reg(MAX77958_REG_ADD register_address, uint16_t len, uint8_t *data){
    int8_t rslt = 0;
    
    if((rslt = dev_I2C_Write(MAX77958_ADDRESS, register_address, data, len))){
        printf("Bad I2C write at register: 0x%X\n",register_address);
        return (rslt);
    }

    return 0;
}

int32_t read_reg(MAX77958_REG_ADD register_address, uint16_t len, uint8_t *data){
    int8_t rslt = 0;
    
    if((rslt = dev_I2C_Read(MAX77958_ADDRESS, register_address, data, len))){
        printf("Bad I2C read at register: 0x%X\n",register_address);
        return (rslt);
    }

    return 0;
}


int32_t write_opcode(MAX77958_USBC_DATA_t *USBC_data){
    
    uint8_t end_byte[] = {0x00};
    int8_t rslt = 0;

    //Send opcode data array
    if((rslt = write_reg(OPCODE_IN_S, 32, USBC_data->AP_DATA_IN))){
        printf("Error writing OpCode %d (Error Code: %d)\n",USBC_data->AP_DATA_IN[0], rslt);
        return (rslt);
    }

    //Send the final byte to update OpCode
    if((rslt = write_reg(OPCODE_IN_E, 1, &end_byte))){
        printf("Error writing OpCode %d (Error Code: %d)\n",USBC_data->AP_DATA_IN[0], rslt);
        return (rslt);
    }
    return (0);
}

int32_t read_opcode(MAX77958_USBC_DATA_t *USBC_data){
    int8_t rslt = 0;
    uint8_t timeout_counter = 0;
    if((rslt = read_reg(OPCODE_OUT_S, 32, USBC_data->AP_DATA_OUT))){
        printf("Error reading OpCode (Error Code: %d)\n",rslt);
        return (rslt);
    }
    return(0);
}

int32_t Poll_Reg(MAX77958_REG_ADD reg_add, uint8_t *reg_value){
    return(read_reg(reg_add,1,reg_value));
}

uint8_t get_VBus_Voltage(void){
    uint8_t temp;
    //TODO: Add bad transaction check
    Poll_Reg(USBC_STATUS1,&temp);
    return (temp);
}

int32_t get_SRC_Cap(MAX77958_USBC_DATA_t *USBC_data){
    int8_t rslt = 0;

    //Setup the AP data array
    USBC_data->AP_DATA_IN[0] = Get_Source_Cap_Opcode;
    memset(&(USBC_data->AP_DATA_IN[1]),0,31);
    
    if((rslt = write_opcode(USBC_data))){
        printf("Error writing OpCode: %d (Error Code: %d)\n",rslt);
        return (rslt);
    }
    return(rslt);
}

int32_t read_get_SRC_Cap(MAX77958_USBC_DATA_t *USBC_data){
    int8_t rslt = 0;
    
    //Read Response
    if((rslt = read_opcode(USBC_data))){
        printf("Error reading OpCode (Error Code: %d)\n",rslt);
        return (rslt);
    }
    //Copy PDO data
    USBC_data->num_PDOs = ((MAX77958_SRC_CAP_CNFG_t)(USBC_data->AP_DATA_OUT[1])).NumOfPDO;
    memcpy(USBC_data->PDO_DATA,&(USBC_data->AP_DATA_OUT[2]),sizeof(uint32_t)*USBC_data->num_PDOs);

    return (USBC_data->num_PDOs);
}

uint8_t check_PDO_param(uint32_t PDO, USBC_PDO_Voltage_t target_voltage, USBC_PDO_Current_t target_current){    
    MAX77958_PDO_FixedSupply_t *temp = &PDO;
    if(temp->PDO_SRC_Supply_Type != USBC_FIXEDOUT){
        return(0);
    }
    if(temp->PDO_SRC_Voltage != target_voltage){
        return(0);
    }
    if(temp->PDO_SRC_Max_Current != target_current){
        return(0);
    }
    return(1);
}

int32_t set_SRC_Cap(MAX77958_USBC_DATA_t *USBC_data, uint8_t desired_PDO_pos){
    int8_t rslt = 0;    
    if(((MAX77958_PDO_FixedSupply_t)(USBC_data->PDO_DATA[desired_PDO_pos])).PDO_SRC_Supply_Type != USBC_FIXEDOUT){
        printf("Trying to set a non fixed-out PDO\n");
        return(-2);
    }
    //Setup the AP data to send request
    USBC_data->AP_DATA_IN[0] = Src_Cap_Request_Opcode;
    USBC_data->AP_DATA_IN[1] = desired_PDO_pos;
    memset(&(USBC_data->AP_DATA_IN[2]),0,30);

    //Write Opcode
    if((rslt = write_opcode(USBC_data))){
        printf("Error writing OpCode: %d (Error Code: %d)\n",rslt);
        return (rslt);
    }

    return (desired_PDO_pos);
}

uint8_t find_SRC_Cap_target(MAX77958_USBC_DATA_t *USBC_data, USBC_PDO_Voltage_t target_voltage, USBC_PDO_Current_t target_current){
    uint8_t desired_PDO_pos = 0xff;
    for(int i =0;i<USBC_data->num_PDOs;i++){
        if(check_PDO_param(USBC_data->PDO_DATA[i], target_voltage, target_current)){
            desired_PDO_pos = i+1;
        }
    }
    
    //Could not find PDO, so set to safe 5V at PDO 0
    if(desired_PDO_pos == 0xff){
        printf("Could not find Fixed PDO with %d mV and %d mA capabilities\n",target_voltage*50,target_current*10);
        desired_PDO_pos = 0x00;
    }
    
    return(desired_PDO_pos);
}

int32_t reset_device(){
    uint8_t reset_command[] = {0x0F};
    write_reg(SW_RESET, 1, reset_command);
    return(0);
}

void print_PPS_PDO(MAX77958_PDO_PPS_t PPS){

    //Max and Min voltages are expressed in 100mV increments
    printf("Maximum Voltage   : %d mV (0x%X)\n",PPS.PPS_SRC_Max_Voltage*100,PPS.PPS_SRC_Max_Voltage);
    printf("Minimum Voltage   : %d mV (0x%X)\n",PPS.PPS_SRC_Min_Voltage*100,PPS.PPS_SRC_Min_Voltage);
    
    //Max current is stored in 50mA increments
    printf("Maximum Current   : %d mA (0x%X)\n\n\n\n",PPS.PPS_SRC_Max_Current*50,PPS.PPS_SRC_Max_Current);
}

void print_augmented_PDO(MAX77958_PDO_Augmented_t APDO){
    
    //Max and Min voltages are expressed in 50mV increments
    printf("Maximum Voltage   : %d mV (0x%X)\n",APDO.APDO_SRC_Max_Voltage*50,APDO.APDO_SRC_Max_Voltage);
    printf("Minimum Voltage   : %d mV (0x%X)\n",APDO.APDO_SRC_Min_Voltage*50,APDO.APDO_SRC_Min_Voltage);
    
    //Max current is stored in 10mA increments
    printf("Maximum Current   : %d mA (0x%X)\n\n\n\n",APDO.APDO_SRC_Max_Current*10,APDO.APDO_SRC_Max_Current);
}

void print_fixed_PDO(MAX77958_PDO_FixedSupply_t PDO){

    //Max and Min voltages are expressed in 50mV increments
    printf("Operating Voltage : %d mV (0x%X)\n",PDO.PDO_SRC_Voltage*50,PDO.PDO_SRC_Voltage);
    
    //Max current is stored in 10mA increments
    printf("Maximum Current   : %d mA (0x%X)\n\n\n\n",PDO.PDO_SRC_Max_Current*10,PDO.PDO_SRC_Max_Current);
}

void print_battery_PDO(MAX77958_PDO_Battery_t PDO){
    
    //Max and Min voltages are expressed in 50mV increments
    printf("Maximum Voltage   : %d mV (0x%X)\n",PDO.BATT_SRC_Max_Voltage*50,PDO.BATT_SRC_Max_Voltage);
    printf("Minimum Voltage   : %d mV (0x%X)\n",PDO.BATT_SRC_Min_Voltage*50,PDO.BATT_SRC_Min_Voltage);
    
    //Max power is stored in 250mW increments
    printf("Maximum Power     : %d mW (0x%X)\n\n\n\n",PDO.BATT_SRC_Max_Power*250,PDO.BATT_SRC_Max_Power);
}

void print_PDO_data(MAX77958_USBC_DATA_t *USBC_data){
    for(int i=0;i<USBC_data->num_PDOs;i++){
        printf("PDO #%i \n",i+1);
        printf("________________________\n\n");
        //Cast to a fixed supply so that we can easily acess bits 30-31 and check PDO type
        switch(((MAX77958_PDO_FixedSupply_t)(USBC_data->PDO_DATA[i])).PDO_SRC_Supply_Type){
            case (USBC_FIXEDOUT):
                printf("PDO Type: Fixed Output Power Supply\n");
                print_fixed_PDO((MAX77958_PDO_FixedSupply_t)(USBC_data->PDO_DATA[i]));
            break;

            case (USBC_BATTERY):
                printf("PDO Type: Battery\n");
                print_battery_PDO((MAX77958_PDO_Battery_t)(USBC_data->PDO_DATA[i]));
            break;

            case (USBC_AUGMENTED):
                printf("PDO Type: Augmented Power Supply\n");  
                print_augmented_PDO((MAX77958_PDO_Augmented_t)(USBC_data->PDO_DATA[i]));
            break;

            case USBC_PPS:
                printf("PDO Type: Programmable Power Supply\n");  
                print_PPS_PDO((MAX77958_PDO_PPS_t)(USBC_data->PDO_DATA[i]));
            break;

            default :
                printf("PDO Type: Uncrecognized... Not sure how we got here tbh\n");
            break;
        }
    }
}

uint8_t get_PD_msg( void ){
    uint8_t temp;
    Poll_Reg(PD_STATUS0,temp);
    return(temp);
}

uint8_t get_SYS_msg( void ){
    uint8_t temp;
    Poll_Reg(USBC_STATUS2,temp);
    return(temp);
}