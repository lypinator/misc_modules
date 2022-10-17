#include "scheduler.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include "nvic_table.h"
#include "tmr.h"
#include "circ_buff.h"

#define QUE_MAX_SIZE 100        //Only 100 routines can be scheudled to run at a time. If you exceed this number, then you are behind schedule

//Globals
struct scheduler main_schedule = {
    .head = NULL,
    .currentid = 0
};

struct routine_que current_que = {
    .routine_count = 0,
    .updating_flag = 0,
    .priority_running_flag = {0,0,0},
    .priority_buffers = {0,0,0}
};


/*** Public Functions ***/

int32_t scheduler_init(void);
//Returns positive routine ID or negative for error
int32_t scheduler_addroutine(uint32_t deadline, void *function, Scheduler_Priority routine_priority, uint16_t num_args, ...);
//Deletes routine and returns 0 for success, -1 for ID not found
int32_t scheduler_removeroutine(uint32_t ID);
//Placed inside the SysTick handler for updating the structure
void scheduler_update(uint32_t elapsed_val);
//Print all of the active routines on the scheduler
void print_routines();
//Basic test program to make sure the scheduler can setup properly
void SCHEDULER_TEST();

uint32_t scheduler_run_routines(void);


/*** Private Functions ***/

//New timing deadline, so add a new node
struct schedule_deadline *create_node(uint32_t deadline);
//Add a function to the list for a timing deadline
int32_t add_function(struct schedule_deadline *routine ,void *function, Scheduler_Priority routine_priority, uint32_t *routine_arguments);
//Run the routines sitting in the ready que
void run_routines(Scheduler_Priority routine_priority);
//Move routines into the ready que
void stage_routine(struct schedule_deadline *node);
//Remove a node from the main schedule
void remove_node(struct schedule_deadline *node);

//IRQ Stuff
void OneshotTimerHandler(void);
void Setup_Timer_ISR(void);
void Setup_Timer_CONT(void);


int32_t scheduler_init(){
    //initialize the buffers
    for(int i=0;i<3;i++){
        current_que.priority_buffers[i] = (struct circ_buff_t *) malloc(sizeof(struct circ_buff_t ));
    }

    NVIC_SetVector(TMR5_IRQn, OneshotTimerHandler);
    NVIC_EnableIRQ(TMR5_IRQn);
    NVIC_SetPriority(TMR5_IRQn, 0);
    Setup_Timer_ISR();

    NVIC_SetPriority (SysTick_IRQn, 0);
    return(0);
}



int32_t scheduler_addroutine(uint32_t deadline, void *function, Scheduler_Priority routine_priority, uint16_t num_args, ...){
    
    struct schedule_deadline *current_timer;
    int32_t routine_id;
    uint32_t *routine_arguments = NULL;


    if(num_args){
        if(num_args > 5){
            printf("Error, too many arguments provided (maximum of 5)\n");
            return(-1);
        }
        if((routine_arguments = calloc(5,sizeof(uint32_t)))==NULL){
            printf("Error allocating memory for routine arguments\n");
            return(-1);
        }
        va_list args;
        va_start(args,num_args);
        for(int i=0;i<num_args;i++){
            routine_arguments[i] = va_arg(args,int);    //Possible issues caused here. Assumes int is a standard 32 bit allocation
        }
        va_end(args);
    }

    //List is empty
    if(main_schedule.head == NULL){
        if((current_timer = create_node(deadline)) == NULL){
            return(-1);
        }
        if ((routine_id = add_function(current_timer, function, routine_priority, routine_arguments)) == -1){
            return(-1);
        }
        return(routine_id);
    }
    //List not empty
    else{
        //Check to see if there is already a timer for this deadline
        current_timer = main_schedule.head;
        while(current_timer->routine_deadline != deadline && current_timer->next != NULL){
            current_timer = current_timer->next;
        }
        //No routine for this timer, create a new Node
        if(current_timer->routine_deadline != deadline){
            if((current_timer = create_node(deadline))== NULL){
                return(-1);
            }
            if ((routine_id = add_function(current_timer, function, routine_priority, routine_arguments)) == -1){
                return(-1);
            }
        }
        //There is already a routine created for this deadline, add the function to the list
        else{
            if ((routine_id = add_function(current_timer, function, routine_priority, routine_arguments)) == -1){
                return(-1);
            }
        }
        return(routine_id);
    }
}

struct schedule_deadline *create_node(uint32_t deadline){
    struct schedule_deadline *new_timer;
    struct routine *new_routine;
    if( (new_timer = (struct schedule_deadline *)malloc(sizeof(struct schedule_deadline))) == NULL){
        //error handler
        printf("Cannot allocate memory for node\n");
        return (NULL);  
    }
    if( (new_routine = (struct routine *)malloc(sizeof(struct routine))) == NULL){
        //error handler
        printf("Cannot allocate memory for routine\n");
        return (NULL);
    }
    
    //Initialize the counter
    new_timer->num_routines = 0;   //add_function will add to this, so start at 0
    new_timer->deadline_counter = deadline;  //Current timer count set to routine_deadline value
    new_timer->routines_head = new_routine;   //We will initialize further in add_function   
    new_timer->routine_deadline = deadline;     //Keep the routine_deadline value so you can reset the counter
    new_timer->next = NULL;         //End of the list

    //Initialize the routine list
    new_routine->function_pointer = NULL;
    new_routine->next = NULL;
    new_routine->routine_id = 0;
    new_routine->Arguments = NULL;


    //Add the new node to the list
    struct schedule_deadline *temp = main_schedule.head;
    if(temp == NULL){
        main_schedule.head = new_timer;
    }
    else{
        while(temp->next != NULL && temp != NULL){
            temp = temp->next;
        }
    }
    temp->next = new_timer;
    return(new_timer);
}


int32_t add_function(struct schedule_deadline *routine , void(*function), Scheduler_Priority routine_priority, uint32_t *routine_arguments){
    struct routine *new_routine;
    struct routine temp;

    new_routine = routine->routines_head;

    //First Task
    if(new_routine->function_pointer == NULL){
        new_routine->function_pointer = function;
        //Set the ID number
        new_routine->routine_id = main_schedule.currentid;
        //Set the Prioirty
        new_routine->routine_priority = routine_priority;
        //Provide the arguments
        new_routine->Arguments = routine_arguments;
        //Not Scheduled yet
        new_routine->routine_scheduled_flag = 0;
        //Increment the ID number
        main_schedule.currentid++;
        //Keep track of how many functions
        routine->num_routines++;

        return(new_routine->routine_id);
    }
    else{
        while(new_routine->next != NULL){
            new_routine = new_routine->next;
        }
        struct routine *temp;
        if( (temp = (struct routine *)malloc(sizeof(struct routine))) == NULL){
            //error handler
            printf("Cannot allocate memory for routine\n");
            return (-1);
        }
        new_routine->next = temp;
        temp->next = NULL;
        
        //Set the ID number
        temp->routine_id = main_schedule.currentid;
        //Increment the ID number
        main_schedule.currentid++;
        temp->function_pointer = function;
        //Set the ask priority
        temp->routine_priority = routine_priority;
        temp->Arguments = routine_arguments;
        temp->routine_scheduled_flag = 0;
        //Keep track of how many functions
        routine->num_routines++;
        return(temp->routine_id);
    }
}

int32_t scheduler_removeroutine(uint32_t ID){
    
    
    //Task ID can't exist because it is too high or negative
    if(main_schedule.currentid < ID || ID < 0){
        printf("%d is an invalid routine ID. Task could not be deleted. [ID out of Bounds]\n",ID);
        return(-1);
    }

    //traverse functions to find routine
    struct schedule_deadline *current_timer;
    struct routine *current_routine;
    struct routine *next_routine;

    current_timer = main_schedule.head;
    while(current_timer != NULL){
        current_routine = current_timer->routines_head;
        next_routine = current_routine;
        while(next_routine->routine_id != ID && next_routine != NULL){
            current_routine = next_routine;
            next_routine = current_routine->next;
        }
        //Task Found
        if(next_routine->routine_id == ID){
            //It is the only routine at this deadline
            if(current_timer->num_routines == 1){
                if(!next_routine->Arguments){
                    free(next_routine->Arguments);
                }
                free(next_routine);
                remove_node(current_timer);
                return(0);
            }
            else{
                //First routine of deadline
                if (next_routine == current_routine){
                    current_timer->routines_head = next_routine->next;
                }
                //Not first routine of deadline
                else{
                    current_routine->next = next_routine->next;
                }
                current_timer->num_routines--;
                if(!next_routine->Arguments){
                    free(next_routine->Arguments);
                }
                free(next_routine);
                return(0);
            }
        }
        else{
            current_timer = current_timer->next;
        }
    }
    printf("%d is an invalid routine ID. Task could not be deleted. [Could not find routine]\n",ID);
    return(-1);
}

void remove_node(struct schedule_deadline *node){
    struct schedule_deadline *current_node;
    struct schedule_deadline *next_node;
    current_node = main_schedule.head;
    next_node = current_node;
    while(next_node != node){
        current_node = next_node;
        next_node = current_node->next;
    }
    //first item on list
    if (current_node == next_node){
        main_schedule.head = next_node->next;
        free(next_node);
    }
    //not first item on list
    else{
        current_node->next = next_node->next;
        free(next_node);
    }
}

//Placed inside the SysTick handler for updating the structure
void scheduler_update(uint32_t elapsed_val){
    current_que.updating_flag = 1;
    struct schedule_deadline *current_node;
    current_node = main_schedule.head;
    while(current_node != NULL){
        current_node->deadline_counter = current_node->deadline_counter-elapsed_val;
        //Timer has expired, add routines to be executed and then reset timer
        if(current_node->deadline_counter > current_node->routine_deadline){
            //Stage routines
            stage_routine(current_node);
            //Reload timer
            current_node->deadline_counter = current_node->routine_deadline;
        }
        //Traverse to the next deadline
        current_node = current_node->next;
    }
    current_que.updating_flag = 0;
}

uint32_t scheduler_run_routines(void){
    
    
    while(current_que.updating_flag);
    //start timer
    Setup_Timer_CONT();
    MXC_TMR_Start(MXC_TMR5);
    run_routines(HIGH_PRIORITY_ROUTINE);
    //update
    //MXC_TMR_Stop(MXC_TMR5);
    scheduler_update(MXC_TMR_GetCount(MXC_TMR5));
    MXC_TMR5->cnt = 0;

    //start timer
    run_routines(HIGH_PRIORITY_ROUTINE);
    run_routines(MEDIUM_PRIORITY_ROUTINE);

    //update
    //MXC_TMR_Stop(MXC_TMR5);
    scheduler_update(MXC_TMR_GetCount(MXC_TMR5));
    MXC_TMR5->cnt = 0;

    //start timer
    MXC_TMR_Start(MXC_TMR5);  

    run_routines(HIGH_PRIORITY_ROUTINE);
    run_routines(MEDIUM_PRIORITY_ROUTINE);
    run_routines(LOW_PRIORITY_ROUTINE);

    //update
    //MXC_TMR_Stop(MXC_TMR5);
    scheduler_update(MXC_TMR_GetCount(MXC_TMR5));
    MXC_TMR5->cnt = 0;

    Setup_Timer_ISR();

    return(0);
}


//Print all of the active routines on the scheduler
void print_routines(){
    struct schedule_deadline *current_list;
    struct routine *current_routine;
    current_list = main_schedule.head;
    printf("Process ID\tInterval\n");
    printf("_____________________________\n");
    while(current_list != NULL){
        current_routine = current_list->routines_head;
        while(current_routine!=NULL){
            printf("%d\t\t%d\n",current_routine->routine_id,current_list->routine_deadline);
            current_routine = current_routine->next;
        }
        current_list = current_list->next;
    }
    printf("\n\n");
}

//Run the routines sitting in the ready que
void run_routines(Scheduler_Priority routine_priority){
    struct routine *current_routine;
    current_que.priority_running_flag[routine_priority] = 1;
    while(((current_que.priority_buffers[routine_priority])->count)){
        current_routine = Remove_Item(current_que.priority_buffers[routine_priority]);
        
        if(current_routine->Arguments){
            (*current_routine->function_pointer)(current_routine->Arguments[0],current_routine->Arguments[1],current_routine->Arguments[2],current_routine->Arguments[3],current_routine->Arguments[4]);
        }
        else {
            (*current_routine->function_pointer)();
        }
        current_routine->routine_scheduled_flag = 0;
        //decrement counter
        current_que.routine_count--;
    }
    current_que.priority_running_flag[routine_priority] = 0;
}

//Move routines into the ready que
void stage_routine(struct schedule_deadline *node){
    if(current_que.routine_count + node->num_routines > QUE_MAX_SIZE){
        //handle overflow
    }

    else{
        struct routine *current_routine;
        current_routine = node->routines_head;
        
        //Traverse the linked list
        while(current_routine != NULL){
            if(!current_routine->routine_scheduled_flag){
                Add_Item(current_routine,current_que.priority_buffers[current_routine->routine_priority]);
                current_routine->routine_scheduled_flag = 1;
                current_que.routine_count++;
            }
            current_routine = current_routine->next;
        }
    }
}

void Setup_Timer_CONT(){
    mxc_tmr_cfg_t tmr;

    MXC_TMR_Shutdown(MXC_TMR5);
    
    tmr.pres = TMR_PRES_8;
    tmr.mode = TMR_MODE_CONTINUOUS;
    tmr.bitMode = TMR_BIT_MODE_32;
    tmr.clock = MXC_TMR_8K_CLK;
    //tmr.cmp_cnt = 8;      //SystemCoreClock*(1/interval_time);
    tmr.pol = 0;
    
    if (MXC_TMR_Init(MXC_TMR5, &tmr, true) != E_NO_ERROR) {
        printf("Failed one-shot timer Initialization.\n");
        return;
    }
}

void Setup_Timer_ISR(){
    // Declare variables
    mxc_tmr_cfg_t tmr;

    MXC_TMR_Shutdown(MXC_TMR5);
    
    tmr.pres = TMR_PRES_1;
    tmr.mode = TMR_MODE_ONESHOT;
    tmr.bitMode = TMR_BIT_MODE_32;
    tmr.clock = MXC_TMR_8K_CLK;
    tmr.cmp_cnt = 8;      //1ms
    tmr.pol = 0;
    
    if (MXC_TMR_Init(MXC_TMR5, &tmr, true) != E_NO_ERROR) {
        printf("Failed one-shot timer Initialization.\n");
        return;
    }

    NVIC_SetVector(TMR5_IRQn, OneshotTimerHandler);
    NVIC_EnableIRQ(TMR5_IRQn);
    NVIC_SetPriority(TMR5_IRQn, 0);
    MXC_TMR_EnableInt(MXC_TMR5);
    MXC_TMR_Start(MXC_TMR5);
}

void OneshotTimerHandler(){
    scheduler_update(1);
    scheduler_run_routines();   //Always returns 0 on first call
    NVIC_SetVector(TMR5_IRQn, OneshotTimerHandler);
    NVIC_EnableIRQ(TMR5_IRQn);
}


void SCHEDULER_TEST(){


}