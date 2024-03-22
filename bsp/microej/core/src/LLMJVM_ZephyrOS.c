/*
 * C
 *
 * Copyright 2021-2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief LLMJVM implementation over Zephyr OS.
 * @author MicroEJ Developer Team
 * @version 2.0.0
 */

/* Includes ------------------------------------------------------------------*/

#include <assert.h>
#include <zephyr/kernel.h>

#include "LLMJVM_impl.h"
#include "microej.h"

/* Defines -------------------------------------------------------------------*/

/*
 * Useful macros and definitions
 */

#define MILLISECONDS_TO_NANOSECONDS (1000000) // Converts milliseconds to nanoseconds.

/* Globals -------------------------------------------------------------------*/

/*
 * Shared variables
 */
/* Absolute time in ms at which timer will be launched */
static int64_t current_schedule_time = INT64_MAX;

/** Offset in ms from system time to application time */
static int64_t application_time_offset = 0;

/* Timer for scheduling next alarm */
static struct k_timer LLMJVM_zephyr_wake_up_timer;

/* Binary semaphore to wakeup microJVM */
static struct k_sem LLMJVM_zephyr_semaphore;

/* Private functions ---------------------------------------------------------*/

static void wake_up_timer_callback(struct k_timer *timer);
static int64_t microej_time_get_current_time(void);

/**
 * @brief The callback for the schedule request timer.
 * 
 * @param timer Address of the timer.
 */
static void wake_up_timer_callback(struct k_timer *timer) {
    if(&LLMJVM_zephyr_wake_up_timer == timer) {
		int32_t result = LLMJVM_schedule();
        (void)result;
		__ASSERT(LLMJVM_OK == result, "Failed to schedule VEE, error code: %d", result);
    }
}

/**
 * @brief Gets the current time in milliseconds.
 *
 * @return int64_t The current time in milliseconds.
 */
static int64_t microej_time_get_current_time(void) {
	return k_uptime_get();
}

/* Public functions ----------------------------------------------------------*/

/*
 * Implementation of functions from LLMJVM_impl.h
 * and other helping functions.
 */

/**
 * This function is called once during MicroJvm virtual machine Startup. It may be used to initialize specific data.<br>
 * @return {@link LLMJVM_OK} on success, {@link LLMJVM_ERROR} on error.
 */
// cppcheck-suppress [misra-c2012-5.5]: MicroEJ specific
int32_t LLMJVM_IMPL_initialize(void) {
	int32_t result;
	
    k_timer_init(&LLMJVM_zephyr_wake_up_timer, wake_up_timer_callback, NULL);

    int retcode = k_sem_init(&LLMJVM_zephyr_semaphore, 0U, 1U);
	if (0 == retcode) {
		result = LLMJVM_OK;		
	} else {
		result = LLMJVM_ERROR;
	}
    return result;
}

/**
 * This function is called once during the MicroJvm virtual machine initialization by the MicroJvm virtual machine task.
 * It can be useful if the MicroJvm virtual machine support needs to know the MicroJvm virtual machine task.
 * @return {@link LLMJVM_OK} on success, {@link LLMJVM_ERROR} on error.
 */
// cppcheck-suppress [misra-c2012-5.5]: MicroEJ specific
int32_t LLMJVM_IMPL_vmTaskStarted(void) {
    return LLMJVM_OK;
}

/**
 * Schedule an alarm (or timer) that will be triggered at the given absolute (system) time.
 * If an alarm is already scheduled for an earlier time this function must do nothing,
 * otherwise it must configure the alarm. If the given absolute time has already been reached, this
 * function must call {@link LLMJVM_schedule}.
 * The scheduled alarm must call the function {@link LLMJVM_schedule} when it is triggered.
 * The specified time is in milliseconds.
 * @return {@link LLMJVM_OK} on success, {@link LLMJVM_ERROR} on error.
 */
int32_t LLMJVM_IMPL_scheduleRequest(int64_t absolute_time_milliseconds) {
	int32_t result = LLMJVM_OK;
	int64_t relative_time_milliseconds = 0;

	/* Check if the requested time is lower than the current schedule time */
	if (absolute_time_milliseconds < current_schedule_time) {
		/* Save the new alarm schedule time */
		current_schedule_time = absolute_time_milliseconds;

		/* Stop the scheduled timer */
        k_timer_stop(&LLMJVM_zephyr_wake_up_timer);

		relative_time_milliseconds = absolute_time_milliseconds - microej_time_get_current_time();

		if (0 < relative_time_milliseconds) {
			/* Update the timer and launch it*/
            k_timer_start(&LLMJVM_zephyr_wake_up_timer, K_MSEC(relative_time_milliseconds), K_NO_WAIT);
			result = LLMJVM_OK;
		} else {
			/* Absolute time has already been reached */
			result = LLMJVM_schedule();
		}
	}

	return result;
}


/**
 * Causes the MicroJvm virtual machine RTOS task to sleep until it is woken up by the {@link LLMJVM_wakeupVM} function.
 * This function is called by the MicroJvm virtual machine task.
 * @return {@link LLMJVM_OK} if wakeup occurred, {@link LLMJVM_INTERRUPTED} if the MicroJvm virtual machine task
 * has been interrupted, or {@link LLMJVM_ERROR} on error.
 */
// cppcheck-suppress [misra-c2012-5.5]: MicroEJ specific
int32_t LLMJVM_IMPL_idleVM(void) {
	int32_t result;
    if (k_sem_take(&LLMJVM_zephyr_semaphore, K_FOREVER) != 0) {
        result = LLMJVM_ERROR;
    } else {
		result = LLMJVM_OK;
	}
	return result;
}

/**
 * Wake up the MicroJvm virtual machine RTOS task. If the MicroJvm virtual machine task is not sleeping,
 * the wakeup stays pending and the MicroJvm virtual machine will not sleep on the next {@link LLMJVM_idleVM} call unless
 * there is a call to {@link LLMJVM_ackWakeup} between this call and the next {@link LLMJVM_idleVM} call.
 * This function must be called only by the MicroJvm virtual machine code. If a task wants to wake up the MicroJvm
 * virtual machine, it must use the {@link LLMJVM_schedule} function (which may in turn call
 * this function).
 * @return {@link LLMJVM_OK} on success, {@link LLMJVM_ERROR} on error.
 */
// cppcheck-suppress [misra-c2012-5.5]: MicroEJ specific
int32_t LLMJVM_IMPL_wakeupVM(void) {
    /* Reset the schedule time */
	current_schedule_time = INT64_MAX;

    k_sem_give(&LLMJVM_zephyr_semaphore);
	
    return LLMJVM_OK;
}

/**
 * Clears any outstanding {@link LLMJVM_wakeupVM} request. After calling this function, a call to {@link LLMJVM_idleVM} will
 * result in a wait even if {@link LLMJVM_wakeupVM} has been called previously (provided no other {@link LLMJVM_wakeupVM} call
 * has occurred since the call to this function.
 * This function is called by the MicroJvm virtual machine task.
 * @return {@link LLMJVM_OK} on success, {@link LLMJVM_ERROR} on error.
 */
// cppcheck-suppress [misra-c2012-5.5]: MicroEJ specific
int32_t LLMJVM_IMPL_ackWakeup(void) {
    return LLMJVM_OK;
}

/**
 * Returns the ID of the current task.
 */
// cppcheck-suppress [misra-c2012-5.5]: MicroEJ specific
int32_t LLMJVM_IMPL_getCurrentTaskID(void) {
	// cppcheck-suppress [misra-c2012-11.4]
    return (int32_t) k_current_get();
}

/**
 * Sets the application time. The application time is the difference, measured in milliseconds,
 * between the current time and midnight, January 1, 1970 UTC.
 * This time does not change the system time.
 * @param t the application time to set in milliseconds
 */
void LLMJVM_IMPL_setApplicationTime(int64_t t) {
	int64_t current_time = microej_time_get_current_time();
	application_time_offset = t - current_time;
}


/**
 * Gets the system or the application time in milliseconds.
 * The system time is the running time since the start of the device. This time is independent from any user considerations
 * and cannot be changed.
 * The application time is the difference, measured in milliseconds, between the current time and midnight, January 1, 1970 UTC.
 * @param system if 1, get the system time, otherwise get the application time.
 */
int64_t LLMJVM_IMPL_getCurrentTime(uint8_t system) {
	int64_t system_time = microej_time_get_current_time();
	return system ? system_time : system_time + application_time_offset;
}


/**
 * Gets the current timestamp in nanoseconds.
 * Only elapsed time between two calls is meaningful.
 * @return the current timestamp in nanoseconds
 */
// cppcheck-suppress [misra-c2012-5.5]: MicroEJ specific
int64_t LLMJVM_IMPL_getTimeNanos(void) {
	return microej_time_get_current_time()*MILLISECONDS_TO_NANOSECONDS;
}

/**
 * This function is called during MicroJvm virtual machine end. It may be used to freed specific data.<br>
 * @return {@link LLMJVM_OK} on success, {@link LLMJVM_ERROR} on error.
 */
// cppcheck-suppress [misra-c2012-5.5]: MicroEJ specific
int32_t LLMJVM_IMPL_shutdown(void) {
    return LLMJVM_OK;
}

