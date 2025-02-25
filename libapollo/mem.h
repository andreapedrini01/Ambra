#ifndef _LIBAPOLLO_MEM_H
#define _LIBAPOLLO_MEM_H

#include "am_util.h"

#define WORDS 4

#define ALIGN16 __attribute__((aligned(16)))

#define __nv __align(16) __attribute__((section("_nv")))

// SRAM array to store values that will be copied in MRAM
static uint32_t buffer[1];
static int returnCode = 0;

// y is the source value you want to copy in x, that is the destination in MRAM
#define COPY_VALUE(dest, src)                                																\
    do                                                                         	 				\
    {                                                                           				\
        if (((uintptr_t)(&dest) % 16) != 0)                             										\
        {                                                                       				\
            am_util_stdio_printf("The starting address is not alligned to 16 byte.\n"); \
        }                                                                       				\
        else                                                                    				\
        {                                                                       				\
						buffer[0] = (uint32_t)(src);																									\
            returnCode = am_hal_mram_main_program(AM_HAL_MRAM_PROGRAM_KEY,  				    \
                                                      (buffer),                  				\
                                                      (&dest),          										\
                                                      (WORDS));                  				\
            am_util_stdio_printf("MRAM program returnCode = %d\n", returnCode); 				\
        }                                                                       				\
    } while (0)
		
#endif // _LIBAPOLLO_MEM_H
