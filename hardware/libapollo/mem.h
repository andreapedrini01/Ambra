#ifndef _LIBAPOLLO_MEM_H
#define _LIBAPOLLO_MEM_H

#include "am_util.h"
#include "hardware.h"

#define INIT_SIGNATURE  0xA90110
#define ALIGN16 __attribute__((aligned(16)))
#define __nv __align(16) __attribute__((section("_nv")))

typedef struct
{
	uint32_t value ALIGN16;
} AlignedVar;

/**
 * @brief Macro to store a 32-bit value in a destination address.
 *
 * This macro writes the `src` value into the memory location pointed to by `dest`,
 * ensuring that the destination is 16-byte aligned before performing the operation.
 * The value is first placed in a temporary buffer, which is then written to MRAM 
 * using `am_hal_mram_main_program`.
 *
 * @param dest  Pointer to the destination address where the value will be stored.
 * @param src   The 32-bit value to be written to the destination.
 *
 * @note If `dest` is not 16-byte aligned, an error message is printed, and the 
 *       operation is not executed.
 */
#define MEM_WR(dest, src) MEM_WR_WORDS(dest, src, 4)
#define MEM_WR_WORDS(dest, src, words)   		                             	\
    do                                                                    \
    {                                                                     \
        if (((uintptr_t)(dest) & 0xF) == 0)                             	\
        {                                                                 \
						uint32_t buffer[4] = {(uint32_t)(src), 0, 0, 0};   						\
            am_hal_mram_main_program(AM_HAL_MRAM_PROGRAM_KEY,  		    		\
                                                    (buffer),             \
                                                    ((uint32_t*)dest),    \
                                                    (words));          	  \
        }                                                                 \
    } while (0)
		
/**
 * @brief Macro to copy the contents of a MRAM structure from source to destination.
 *
 * This macro copies a structure of type `type` from `src` to `dest`, assuming 
 * both are properly aligned. It treats the structure as an array of `uint32_t` 
 * values and copies them element by element using `MEM_WR`.
 *
 * @param dest  Pointer to the destination structure where data will be copied.
 * @param src   Pointer to the source structure containing the data to copy.
 * @param type  The data type of the structure being copied.
 *
 * @note The `MEM_WR` macro is used to copy individual `uint32_t` values.
 */
#define COPY_STRUCT(dest, src, type)                      \
    do {                                                  \
        uint32_t *p_dest = (uint32_t *)(dest);            \
        const uint32_t *p_src = (const uint32_t *)(src);  \
        uint32_t n = sizeof(type) / sizeof(uint32_t);     \
        for (uint32_t i = 0; i < n; i++) {                \
            MEM_WR(&p_dest[i], p_src[i]);           		  \
        }                                                 \
    } while (0)

		
/**
 * @brief Macro to store a pointer address in MRAM.
 *
 * This macro stores the address contained in the source pointer (`src`) 
 * into the specified destination (`dest`), which must be 16-byte aligned.
 * 
 * The address is first stored in a temporary buffer, which is then written 
 * to MRAM using `am_hal_mram_main_program`.
 *
 * @param dest  The destination pointer where the address of `src` will be stored.
 * @param src   The source pointer whose address will be saved in MRAM.
 *
 * @note If `dest` is not 16-byte aligned or `src` is NULL, an error message is printed 
 *       and the operation is aborted.
 */
#define COPY_PTR(dest, src)                                 														\
    do                                                                         	 				\
    {                                                                           				\
        if (((uintptr_t)(dest) & 0xF) == 0 && src != NULL)             							  	\
        {                                                                       				\
						uint32_t buffer[4] = {(uint32_t)(uintptr_t)(src), 0, 0, 0};   							\
            am_hal_mram_main_program(AM_HAL_MRAM_PROGRAM_KEY,    												\
                                                      (buffer),                  				\
                                                      ((uint32_t *)(&dest)),	  			 	\
                                                      (4));                  						\
        }                                                                       				\
    } while (0)
		
#endif // _LIBAPOLLO_MEM_H
