/*
 * C
 *
 * Copyright 2015-2023 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */
/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <zephyr/input/input.h>
#include <zephyr/logging/log.h>
#include <zephyr/shell/shell.h>
#include <zephyr/sys/atomic.h>

#include "touch_helper.h"

LOG_MODULE_DECLARE(input);


static void input_dump_cb(struct input_event *evt)
{
    static int x = 0;
    static int y = 0;
    static bool is_touch_evt = false;

    if ((evt->type == INPUT_EV_KEY) &&
        (evt->code == INPUT_BTN_TOUCH) &&
        (evt->value == 1)) // we have a touch event
    {
        is_touch_evt = true;
    }

    if (evt->type == INPUT_EV_ABS)
    {
        if (evt->code == INPUT_ABS_X)
        {
            x = evt->value;
        }
        if (evt->code == INPUT_ABS_Y)
        {
            y = evt->value;

            if (is_touch_evt) {
                is_touch_evt = false;
                static bool flip = true;
                if (flip)
                    TOUCH_HELPER_pressed(x, y);
                else
                    TOUCH_HELPER_released();

                flip = !flip;
            }
        }
    }
}
INPUT_CALLBACK_DEFINE(NULL, input_dump_cb, NULL);
