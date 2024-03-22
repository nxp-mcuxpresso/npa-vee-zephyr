/*
 * C
 *
 * Copyright 2018-2023 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* Includes ------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "lcdic_panel_config.h"
#include "LLUI_DISPLAY_impl.h"

#include "sni.h"
#include "lcd.h"

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>

#ifdef CONFIG_ARCH_POSIX
#include "posix_board_if.h"
#endif

#include "tree_version.h"
#include "display_configuration.h"
#include "zephyr/kernel.h"

static int buf_idx = 0;

#ifdef CONFIG_PARTIAL_BUFFER
#define HOW_MANY_BUFFERS 1
static DISPLAY_CONFIGURATION_flush_region flush_region;
#else
#define HOW_MANY_BUFFERS 2
#endif

static uint8_t *buf[HOW_MANY_BUFFERS] = {NULL};
static struct display_buffer_descriptor buf_desc;
static const struct device *display_dev;
static int vheight = 0;

K_SEM_DEFINE(llui_sem, 0, 1);

#ifndef CONFIG_PARTIAL_BUFFER
#define DISPLAY_STACK_SIZE 500

K_SEM_DEFINE(flush_sem, 0, 1);
#define DISPLAY_PRIORITY 5
void display_task(void *, void *, void *);
K_THREAD_STACK_DEFINE(display_stack_area, DISPLAY_STACK_SIZE);
static struct k_thread display_thread_data;
static volatile uint32_t g_xmin = 0;
static volatile uint32_t g_ymin = 0;
#endif

/****************************UI PORT FUNCTION***************************************/

static size_t buf_size = 0;
void LLUI_DISPLAY_IMPL_initialize(LLUI_DISPLAY_SInitData* init_data)
{
    struct display_capabilities capabilities;

    display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
    if (!device_is_ready(display_dev)) {
        printf("Device %s not found. Aborting sample.\n",
                display_dev->name);
#ifdef CONFIG_ARCH_POSIX
        posix_exit_main(1);
#else
        return;
#endif
    }

    printf("LLUI init for %s\n", display_dev->name);
    display_get_capabilities(display_dev, &capabilities);

    printf("Display x_resolution=%d\n", capabilities.x_resolution);
    printf("Display y_resolution=%d\n", capabilities.y_resolution);

    int divisor = 1;
#ifdef CONFIG_PARTIAL_BUFFER
    /* we use a tenth of the size */
    divisor = 10;
#endif
    vheight = capabilities.y_resolution/divisor;

    /* get the size of the buffer portion */
    buf_size = vheight*capabilities.x_resolution;

    // find pixel format hence pixel size
    uint8_t bg_color;
    switch (capabilities.current_pixel_format) {
        case PIXEL_FORMAT_ARGB_8888:
            bg_color = 0xFFu;
            buf_size *= 4;
            break;
        case PIXEL_FORMAT_RGB_888:
            bg_color = 0xFFu;
            buf_size *= 3;
            break;
        case PIXEL_FORMAT_RGB_565:
            bg_color = 0xFFu;
            buf_size *= 2;
            break;
        case PIXEL_FORMAT_BGR_565:
            bg_color = 0xFFu;
            buf_size *= 2;
            break;
        case PIXEL_FORMAT_MONO01:
            bg_color = 0xFFu;
            buf_size = DIV_ROUND_UP(DIV_ROUND_UP(
                        buf_size, NUM_BITS(uint8_t)), sizeof(uint8_t));
            break;
        case PIXEL_FORMAT_MONO10:
            bg_color = 0x00u;
            buf_size = DIV_ROUND_UP(DIV_ROUND_UP(
                        buf_size, NUM_BITS(uint8_t)), sizeof(uint8_t));
            break;
        default:
            printf("Unsupported pixel format. Aborting sample.\n");
#ifdef CONFIG_ARCH_POSIX
            posix_exit_main(1);
#else
            return;
#endif
    }

    /* allocate the fb buffer */
    for (int i = 0; i < HOW_MANY_BUFFERS; i++)
    {
        buf[i] = k_malloc(buf_size);
        if (buf[i] == NULL) {
            printf("Could not allocate memory. Aborting sample.\n");
#ifdef CONFIG_ARCH_POSIX
            posix_exit_main(1);
#else
            return;
#endif
        }

        // make it bgcolor
        (void)memset(buf[i], bg_color, buf_size);
    }

    buf_desc.buf_size = buf_size;
    buf_desc.pitch = capabilities.x_resolution;
    buf_desc.width = capabilities.x_resolution;
    buf_desc.height = vheight;
    for (int idx = 0; idx < capabilities.x_resolution; idx += vheight) {
        (void)memset(buf[buf_idx], 0, buf_size);
        display_write(display_dev, 0, idx, &buf_desc, buf[buf_idx]);
    }
    // Initialize context data.
    init_data->back_buffer_address = buf[buf_idx];
    init_data->lcd_height = capabilities.y_resolution;
    init_data->lcd_width = capabilities.x_resolution;

#ifndef CONFIG_PARTIAL_BUFFER
    k_tid_t my_tid = k_thread_create(&display_thread_data, display_stack_area,
            K_THREAD_STACK_SIZEOF(display_stack_area),
            display_task,
            NULL, NULL, NULL,
            DISPLAY_PRIORITY, 0, K_NO_WAIT);
#endif
}

uint8_t* LLUI_DISPLAY_IMPL_flush(MICROUI_GraphicsContext* gc,
                                 uint8_t* srcAddr, uint32_t xmin,
                                 uint32_t ymin, uint32_t xmax,
                                 uint32_t ymax)
{
#ifdef CONFIG_PARTIAL_BUFFER
#ifdef HARD_FLUSH_DBG
    printf("x_offset: %d\r\n", flush_region.x_offset);
    printf("y_offset: %d\r\n", flush_region.y_offset);
    printf("width: %d\r\n", flush_region.width);
    printf("height: %d\r\n", flush_region.height);
#endif

    xmin = flush_region.x_offset;
    ymin = flush_region.y_offset;
    xmax = xmin + flush_region.width - 1;
    ymax = ymin + flush_region.height - 1;

#ifdef HARD_FLUSH_DBG
    printf("xmin: %d\r\n", xmin);
    printf("ymin: %d\r\n", ymin);
    printf("xmax: %d\r\n", xmax);
    printf("ymax: %d\r\n", ymax);
    printf("\r\n");
#endif

    display_write(display_dev, xmin, ymin, &buf_desc, buf[buf_idx]);
    LLUI_DISPLAY_flushDone(false);
    return srcAddr;
#else
    // swap idx
    buf_idx ^= 1U;
    g_xmin = xmin;
    g_ymin = ymin;

    k_sem_give(&flush_sem);

    return buf[buf_idx];
#endif

}
void LLUI_DISPLAY_IMPL_binarySemaphoreTake(void* sem){
    k_sem_take(&llui_sem, K_FOREVER);
}

void LLUI_DISPLAY_IMPL_binarySemaphoreGive(void* sem, bool under_isr) {
    k_sem_give(&llui_sem);
}

#ifdef CONFIG_PARTIAL_BUFFER
void Java_com_microej_partial_support_PartialBufferNatives_setFlushLimits(jint xOffset, jint yOffset,
                                                                          jint width, jint height)
{
    flush_region.x_offset = xOffset;
    flush_region.y_offset = yOffset;
    flush_region.width = width;
    flush_region.height = height;
}

int32_t Java_com_microej_partial_support_PartialBufferNatives_getBufferHeight()
{
    return vheight;
}
#else
void display_task(void *, void *, void *)
{
    while (1)
    {
        k_sem_take(&flush_sem, K_FOREVER);
        display_write(display_dev, g_xmin, g_ymin, &buf_desc, buf[buf_idx^1]);
        memcpy((void*)buf[buf_idx], (void*)buf[buf_idx^1], buf_size);

        LLUI_DISPLAY_flushDone(false);
    }
}
#endif
