/*
 * timer.h
 *
 *  Created on: 20 nov 2017
 *      Author: lorenzo
 */

#ifndef MY_TIMER_H_
#define MY_TIMER_H_

#include <stdbool.h>
#include <stdint.h>

extern uint32_t milliseconds_since_boot;

typedef enum
{
    SOFT_TIMER_0,
    SOFT_TIMER_1,
    SOFT_TIMER_2,
    SOFT_TIMER_3,
    SOFT_TIMER_4,
    SOFT_TIMER_5,
    SOFT_TIMER_6,
    SOFT_TIMER_7,
    SOFT_TIMER_8,
    SOFT_TIMER_9,
    SOFT_TIMER_10,
    SOFT_TIMER_NUM
} e_timer_id;

#define TIMER_RESET_VALUE     (0U)
#define TIMER_RESET(x)        ((void)timeout(TIMER_RESET_VALUE, (x)))
#define ON_TIMER_EXPIRED(cycles,identifier,callback)        if (timeout(cycles, identifier) == true) { callback; }
#define ON_TIMER_NOT_EXPIRED(cycles,identifier,callback)    if (timeout(cycles, identifier) == false) { callback; }
#define ON_TIMER(cycles,identifier,callback,callback2)    if (timeout(cycles, identifier) == true) { callback; } else { callback2; }

bool timeout(uint32_t cycles, e_timer_id timer_id);

#endif /* MY_TIMER_H_ */
