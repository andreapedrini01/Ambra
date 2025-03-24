#ifndef HARDWARE_H
#define HARDWARE_H

#include "hardware.h"
#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_hal_stimer.h"
#include "am_hal_clkgen.h"
#include "am_util.h"
#include <stdarg.h>
#include <stdio.h>

void cache_configuration(void);
uint32_t get_time_us(void);
void init_hw(void);
void printf_apollo(const char *string, ...);

#endif // CRITICAL_VARIABLES_H
