#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdio.h>
#include <stdint.h>
#include "mxc_sys.h"
#include "nvic_table.h"
#include "circ_buff.h"

/* Glossary for scheduler.h and scheduler.c 
 * 
 *  Schedule       - Ordered list of functions that need to run on the processor.
 *  Scheduler      - A program that will make sure certain functions get added to the schedule after a set amount of time.
 *  Routine        - The function that is periodically added to the schedule by the scheduler.
 *  Deadline       - Defines how frequently the routine needs to be added to the schedule by the scheduler
 *  
 */

typedef enum 
{ 
    SYSTICK_DIVIDER_US  =   60,         //Divider value to run SysTick ISR 100,000 times per second (every Us)
    SYSTICK_DIVIDER_MS  =   60000,      //Divider value to run SysTick ISR 1,000 times per second (every Ms)  
    SYSTICK_DIVIDER_S   =   60000000    //Divider value to run SystTick ISR 1 time per second (every s)
} SysTick_Scaler;


/* Type for High, Medium, and Low priorities. High priority routines run first, then medium, then low. No preemption allowed */
typedef enum
{ 
    HIGH_PRIORITY_ROUTINE = 0,
    MEDIUM_PRIORITY_ROUTINE = 1,
    LOW_PRIORITY_ROUTINE = 2
} Scheduler_Priority;


/*
*   Structure for organizing each deadline
*/
volatile struct schedule_deadline {
    uint32_t routine_deadline;              //Deadline value (Number of ms between each time routines are scheduled)
    uint32_t deadline_counter;              //Current Counter Value (Routines are scheudled once count_timer=0
    uint32_t num_routines;              //Number of routines to run each time interval expires
    struct routine *routines_head;      //Points to the head of a list of routines to be executed once interval has expired
    struct schedule_deadline *next;         //Pointer to the next deadline structure (Linked List format)
};

/*
*   Structure for organizing the main schedule. This struct points to the linked list of deadlines
*   and assigns the process ID for every new routine add to the schedule.
*/
volatile struct scheduler {
    struct schedule_deadline *head;    //Head of deadline structures (Linked List format)
    uint32_t currentid;                //Routine ID number for next routine 
};

/*
*   Structure for holding the function pointers associated with a given deadline. Instead of holding
*   A new link in the linked list for every routine, we simply add a function to a linked list of "routines"
*   which are all scheduled together once the deadline expires
*/
volatile struct routine{
    int32_t routine_id;
    uint32_t routine_scheduled_flag;
    void (* function_pointer)();           //Fucntion Pointer to routine that must be run
    uint32_t *Arguments;                   //place to add arguments in future
    Scheduler_Priority routine_priority;   //Routine Priority
    struct routine *next;                  //Pointer to the next routine for a given deadline (Linked List format)
};

/*
*   Structure that holds the routines ready to be executed. SysTick Interval will run all of the ready routines
*   once they are added to the que
*/
volatile struct routine_que{  
    uint32_t routine_count;                 //Number of routines in ready que -- If this exceeds user-defined maximum number of allowable scheduled routines (Scheduler is falling behind) then you have to handle errors for application specific implementation
    uint8_t updating_flag;
    uint8_t priority_running_flag[3];    //Flag to alert ISR that routines are already running -- Prevents the same routine being called every single SysTick if it hasnt finished running at the next SysTick
    struct circ_buff_t *priority_buffers[3];
};

/**
* @brief        Initialize the SystTick timer with a given clock divider
* @param[in]    SYSTICK_DIVIDER - Defines how many clock cycles to count before triggering a SysTick ISR (SystTick period = clock_counter / 60 MHz)
*               
* @return       0 (Success), Error Code (Failure)
*/
int32_t scheduler_init();

/**
* @brief        Add a routine to the scheduler to execute at the provided deadline
* @param[in]    deadline - Number of SysTick interrupt routines to wait before routine is executed
* @param[in]    function_pointer - Function pointer to the routine that you want to run at defined deadline
* @param[in]    routine_priority - Priority of routine (High, Medium, Low)
* @param[in]    num_ars - Number of arguments required by the routine (MAX VALUE OF 5 arguments per routine)
* @param[in]    ... - Up to 5 arguments to add. Must be in correct order to be called in routine. Only 32-bit values allowed (i.e pointers, integers, etc.). No long long variables, uint64_t or strings
*
* @return       Positive Number (routine ID) (Success), Negative Number (Failure)
*/
int32_t scheduler_addroutine(uint32_t deadline, void *function, Scheduler_Priority routine_priority, uint16_t num_args, ...);

/**
* @brief        Remove a routine from the scheduler
* @param[in]    ID - ID number assigned to the routine when it was added
*
* @return       0 (Success), -1 (Failure)
*/
int32_t scheduler_removeroutine(uint32_t ID);

/**
* @brief        Function placed in SysTick ISR. Decrements the deadline counters, reloads 
*               deadline counters and adds routines to ready que when defined deadline expires
*/
void scheduler_update(uint32_t elapsed_val);

/**
* @brief        Print the routine ID and deadline for every active routine
*/
void print_routines();

/**
* @brief        Basic Test Program for the scheduler
*/
void SCHEDULER_TEST();

/**
* @brief        Run the tasks in the ready que
*
* @return       0 (All tasks completed) 1 (Reccursive call of function and lower priority routines still running)
*/
uint32_t scheduler_run_routines(void);

#endif