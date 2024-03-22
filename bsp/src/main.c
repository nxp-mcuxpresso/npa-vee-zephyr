/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "LLMJVM.h"
#include "sni.h"
#include "microej_main.h"

#include "tree_version.h"

int main(void)
{
    printf("NXP PLATFORM ACCELERATOR\n");
    printf("NXP VEE Port '%s' '%s'\r\n", VEE_VERSION, GIT_SHA_1);
#if CONFIG_SEGGER_SYSTEMVIEW
    extern void _SEGGER_RTT;
    printf("SEGGER_RTT block address: %p\n", &(_SEGGER_RTT));
#endif
    microej_main(0, NULL);
	return 0;
}
