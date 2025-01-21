#ifndef _LIBAPOLLO_MEM_H
#define _LIBAPOLLO_MEM_H

#define WORD 4

#define _nv __align(16) __attribute__((section("_nv")))

#define WRITE_MRAM(array_ptr, start_address, size)                                			    \
    do                                                                         	 				\
    {                                                                           				\
        if (((uint32_t)(start_address) % 16) != 0)                              				\
        {                                                                       				\
            am_util_stdio_printf("The starting address is not alligned to 16 byte.\n");         \
        }                                                                       				\
        else                                                                    				\
        {                                                                       				\
            returnCode = am_hal_mram_main_program(AM_HAL_MRAM_PROGRAM_KEY,  				    \
                                                      (array_ptr),                  		    \
                                                      (start_address),          				\
                                                      (size));                  				\
            am_util_stdio_printf("MRAM program returnCode = %d\n", returnCode); 				\
        }                                                                       				\
    } while (0)

#endif // _LIBAPOLLO_MEM_H