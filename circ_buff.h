#ifndef CIRCBUFF_H
#define CIRCBUFF_H

#include <stdio.h>
#include <stdint.h>
#include "scheduler.h"

#define BUFF_SIZE 10

struct circ_buff_t{
    uint16_t count;
    uint16_t remove_index;
    uint16_t add_index;
    struct routine *function_buff[BUFF_SIZE];
};

uint32_t Add_Item(struct routine *function_index, struct circ_buff_t *buff);

struct routine *Remove_Item(struct circ_buff_t *buff);

#endif