#ifndef CRITICAL_VARIABLES_H
#define CRITICAL_VARIABLES_H

#include "../hardware/libapollo/mem.h"

#define INPUT_SIZE 5
#define KERNEL_SIZE 3
#define OUTPUT_SIZE (INPUT_SIZE - KERNEL_SIZE + 1)

/**
 * @brief Structure containing power-failure-resilient variables.
 *
 * The `CritVar` struct holds all critical variables that must be retained 
 * across power failures. These variables are properly aligned to ensure 
 * atomic access and compatibility with MRAM storage.
 * Example:
 * - `x` and `y`: Aligned variables for critical computations.
 * - `array[3]`: A three-element array, explicitly aligned to 16 bytes.
 *
 * Any new variables required for executing and storing task calculations 
 * should be added to this struct, ensuring they adhere to the same 
 * alignment constraints for power-failure resilience.
 */
typedef struct
{
	//These are for example, change them on what you need
		AlignedVar idx;
		AlignedVar input[INPUT_SIZE];
		AlignedVar kernel[KERNEL_SIZE];
		AlignedVar output[OUTPUT_SIZE];
} CritVar;

#endif // CRITICAL_VARIABLES_H
