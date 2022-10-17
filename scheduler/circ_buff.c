#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "circ_buff.h"



//Add pointer to buffer
uint32_t Add_Item(struct routine *function_index, struct circ_buff_t *buff){
    
    //Buffer is empty
    if(buff->count < BUFF_SIZE){
        
        //Incrmenet counter
        buff->count++;
        
        //Add the value to the buffer
        buff->function_buff[buff->add_index] = function_index;
        
        //Incremenet front index and loop back to start if neccessarry
        buff->add_index++;
        if (buff->add_index >= BUFF_SIZE){
            buff->add_index = 0;
        }
        
        //Return buffer count
        return(buff->count);
    }
    //Buffer is full
    else{
        printf("Buffer is full! %d was not added to the circular buffer\n");
        return(-1);
    }
}

//Remove pointer from buffer
struct routine *Remove_Item(struct circ_buff_t *buff){
    if(!buff->count){
        printf("Trying to remove item from empty buffer!\n");
        return(-1);
    }
    else{
        buff->count--;
        struct routine *ret = buff->function_buff[buff->remove_index];
        buff->remove_index++;
        if(buff->remove_index >= BUFF_SIZE){
            buff->remove_index = 0;
        }
        return(ret);
    }
}

