/*
 * C
 *
 * Copyright 2021 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 */

#include <zephyr/kernel.h>

#include "microej_time.h"
#include "microej.h"
/*
 *********************************************************************************************************
 *                                             DEFINES
 *********************************************************************************************************
 */


/*
 *********************************************************************************************************
 * 	                                      INTERNAL FUNCTIONS
 *********************************************************************************************************
 */

/** Offset in ms from system time to application time */
static int64_t microej_app_time_offset_ms = 0;

/*
 *********************************************************************************************************
 * 	                                      PUBLIC FUNCTIONS
 *********************************************************************************************************
 */
 
void microej_time_init(void) {}
 
/*
 * Convert a time in milliseconds in ticks.
 * Result is round up to the next tick (ie. converting back the resulting ticks
 * to milliseconds results in a value greater than or equals to given time).
 */
int64_t microej_time_time_to_tick(int64_t time) {
    int64_t timeus = time * 1000;
    int64_t os_tick = 1000000 / CONFIG_SYS_CLOCK_TICKS_PER_SEC; // tick in microseconds
    
    return (timeus + (os_tick-1)) / os_tick;
}

/*
 * Convert a nb of ticks in milliseconds.
 * Result is round up to the next millisecond (ie. converting back the resulting ms
 * to ticks results in a value greater than or equals to given time).
 */
int64_t microej_time_tick_to_time(int64_t ticks) {
    int64_t timems = (ticks * 1000) / CONFIG_SYS_CLOCK_TICKS_PER_SEC;

    return timems;
}

int64_t microej_time_get_current_time(uint8_t isPlatformTime) {
    /*
     * /!\
     * isPlatformTime == true when ej.bon.Util.platformTimeMillis
     * isPlatformTime == false when java.lang.System.currentTimeMillis
     * /!\
     */
    
    int64_t time_ms = microej_time_get_time_nanos() / 1000000;

    if (isPlatformTime == MICROEJ_FALSE) {
        time_ms += microej_app_time_offset_ms;
    }
    return time_ms;
}

int64_t microej_time_get_time_nanos(void) {
    /* Current system time in nanoseconds */
    static int64_t system_time_ns = 0;
    /* High-precision cycle counter */
    static uint32_t system_hp_cycle = 0;
    
    if ((system_time_ns == 0) ||
       (k_uptime_get() - (system_time_ns / 1000000)) > ((uint64_t)(UINT32_MAX) * 1000) / CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC) {
        /* if the time is not initialized yet or if the 32 bits high-performance counter has overflowed between two successive function calls, 
           then initialize the system time using the OS tick counter. */
        system_time_ns = k_uptime_get() * 1000000;
    } else {
        /* otherwise use high-performance counter so as to achieve higher precision */
        system_time_ns += ((int64_t)(k_cycle_get_32() - system_hp_cycle) * 1000000000) / CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC;
    }

    /* Update high-precision cycle counter */
    system_hp_cycle = k_cycle_get_32();

    return system_time_ns;
}

void microej_time_set_application_time(int64_t t) {
    microej_app_time_offset_ms = t - microej_time_get_current_time(MICROEJ_TRUE);
}
