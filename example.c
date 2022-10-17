/*******************************************************************************
* Copyright (C) Maxim Integrated Products, Inc., All rights Reserved.
*
* This software is protected by copyright laws of the United States and
* of foreign countries. This material may also be protected by patent laws
* and technology transfer regulations of the United States and of foreign
* countries. This software is furnished under a license agreement and/or a
* nondisclosure agreement and may only be used or reproduced in accordance
* with the terms of those agreements. Dissemination of this information to
* any party or parties not specified in the license agreement and/or
* nondisclosure agreement is expressly prohibited.
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
* OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*
* Except as contained in this notice, the name of Maxim Integrated
* Products, Inc. shall not be used except as stated in the Maxim Integrated
* Products, Inc. Branding Policy.
*
* The mere transfer of this software does not imply any licenses
* of trade secrets, proprietary technology, copyrights, patents,
* trademarks, maskwork rights, or any other form of intellectual
* property whatsoever. Maxim Integrated Products, Inc. retains all
* ownership rights.
*******************************************************************************/

/**
 * @file    example.c
 * @brief   Example program for scheduler module
 * @details
 *
 *
 */

/* **** Includes **** */
#include "scheduler.h"
#include <string.h>
#include <stdio.h>
#include "mxc_delay.h"
#include "nvic_table.h"
#include "tmr.h"

//Test GPIOs for verification
mxc_gpio_cfg_t gpio_out1;
mxc_gpio_cfg_t gpio_out2;
mxc_gpio_cfg_t gpio_out3;
mxc_gpio_cfg_t gpio_out4;

void SysTick_Handler(void)
{
    scheduler_update();
}

int count1, count2, count3, count4, count5, indicator;
void Task1(uint32_t delay,uint32_t extraVal){
    MXC_GPIO_OutSet(gpio_out1.port,gpio_out1.mask);
    count1++;
    indicator+=extraVal;
    for(int i=0;i<delay;i++);
    MXC_GPIO_OutClr(gpio_out1.port,gpio_out1.mask);
}
void Task2(uint32_t test1, uint32_t test2, uint32_t test3, uint32_t test4){
    MXC_GPIO_OutSet(gpio_out2.port,gpio_out2.mask);
    count2++;
    printf("Test Variables = %d, %d, %d, %d\n",test1,test2,test3,test4);
    for(int i=0;i<150000;i++);
    MXC_GPIO_OutClr(gpio_out2.port,gpio_out2.mask);
}
void Task3(uint32_t *test_data, char *sample_string ){
    MXC_GPIO_OutSet(gpio_out3.port,gpio_out3.mask);
    printf("Test data from task 3: ");
    for(int i = 0; i <5;i++){
        printf("%d, ",test_data[i]);
    }
    printf("\n\n");
    printf("Sample string: %s", sample_string);
    count3++;
    for(int i=0;i<150000;i++);
    MXC_GPIO_OutClr(gpio_out3.port,gpio_out3.mask);
}
void Task4(){
    MXC_GPIO_OutSet(gpio_out4.port,gpio_out4.mask);
    count4++;
    for(int i=0;i<150000;i++);
    MXC_GPIO_OutClr(gpio_out4.port,gpio_out4.mask);
}
void Task5(){
    count5++;
}

void GPIO_TestInit(){
    gpio_out1.port = MXC_GPIO0;
    gpio_out1.mask = MXC_GPIO_PIN_17;
    gpio_out1.pad = MXC_GPIO_PAD_NONE;
    gpio_out1.func = MXC_GPIO_FUNC_OUT;
    MXC_GPIO_Config(&gpio_out1);
    
    gpio_out2.port = MXC_GPIO0;
    gpio_out2.mask = MXC_GPIO_PIN_16;
    gpio_out2.pad = MXC_GPIO_PAD_NONE;
    gpio_out2.func = MXC_GPIO_FUNC_OUT;
    MXC_GPIO_Config(&gpio_out2);
    
    gpio_out3.port = MXC_GPIO1;
    gpio_out3.mask = MXC_GPIO_PIN_1;
    gpio_out3.pad = MXC_GPIO_PAD_NONE;
    gpio_out3.func = MXC_GPIO_FUNC_OUT;
    MXC_GPIO_Config(&gpio_out3);
    
    gpio_out4.port = MXC_GPIO0;
    gpio_out4.mask = MXC_GPIO_PIN_8;
    gpio_out4.pad = MXC_GPIO_PAD_NONE;
    gpio_out4.func = MXC_GPIO_FUNC_OUT;
    MXC_GPIO_Config(&gpio_out4);
}


/* **************************************************************************** */

int main(void)
{
    //Generate test data array
    uint32_t sample_data[5];
    sample_data[0] = 10;
    sample_data[1] = 11;
    sample_data[2] = 12;
    sample_data[3] = 13;
    sample_data[4] = 14;

    //Test data string
    char *sample_string = "Hello world!";


    MXC_Delay(MXC_DELAY_SEC(2)); // Create window for debugger to connect after reset

    //Set 
    GPIO_TestInit();

    /*
    EXAMPLE TASK CALL:
    scheduler_addroutine(Deadline(ms),FunctionName,Priority,Number of Arguments,... add in the arguments here or leave empty);
    */

    //General test routine
    scheduler_addroutine(1000,Task5,LOW_PRIORITY_ROUTINE,0);

    //Provides 2 integer arguments and uses in function call
    scheduler_addroutine(3000,Task1,HIGH_PRIORITY_ROUTINE,2,150000,2);

    //Provides 4 integer arguments
    scheduler_addroutine(2000,Task2,MEDIUM_PRIORITY_ROUTINE,4,6,5,4,3);

    //Provides 2 pointers to string and integer array
    scheduler_addroutine(1000,Task3,LOW_PRIORITY_ROUTINE,2,sample_data,sample_string);

    //General Test Routine
    scheduler_addroutine(500,Task4,HIGH_PRIORITY_ROUTINE,0);

    //Initialize the scheduler (1ms with 60MHz system clock)
    if(scheduler_init(SYSTICK_DIVIDER_MS) != E_NO_ERROR) {
        printf("ERROR: Ticks is not valid");
        //LED_On(1);
    }

    while(1) {
        
        /* Add in regular program here. Scheduler will runn each routine as an interrupt */
    }
}