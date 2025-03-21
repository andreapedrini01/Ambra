#include <stdlib.h>
#include "../libapollo/hardware.h"
#include "../libalpaca/alpaca.h"

//*****************************************************************************
//
// Insert compiler version at compile time.
//
//*****************************************************************************
#define STRINGIZE_VAL(n)                    STRINGIZE_VAL2(n)
#define STRINGIZE_VAL2(n)                   #n

#ifdef __GNUC__
#define COMPILER_VERSION                    ("GCC " __VERSION__)
#elif defined(__ARMCC_VERSION)
#define COMPILER_VERSION                    ("ARMCC " STRINGIZE_VAL(__ARMCC_VERSION))
#elif defined(__KEIL__)
#define COMPILER_VERSION                    "KEIL_CARM " STRINGIZE_VAL(__CA__)
#elif defined(__IAR_SYSTEMS_ICC__)
#define COMPILER_VERSION                    __VERSION__
#else
#define COMPILER_VERSION                    "Compiler unknown"
#endif

#define ENABLE_DEBUGGER

//tasks
void compute_convolution_task(void);
void populate(void);

TASK(1, populate);
TASK(2, compute_convolution_task);

void populate() {
	for (size_t i = 0; i < INPUT_SIZE; i++) {
		COPY_VALUE(&GVB(input, i), (uint32_t)rand());
  }
	
	for (size_t i = 0; i < KERNEL_SIZE; i++) {
		COPY_VALUE(&GVB(kernel, i), (uint32_t)rand());
  }
	TRANSITION_TO(compute_convolution_task);
}

void compute_convolution_task() {
	if (GVB(idx) >= OUTPUT_SIZE) {
        printf_apollo("Convoluzione completata!\n");
        return;
    }

    uint32_t sum = 0;
		uint32_t index = GVB(idx);
    for (int j = 0; j < KERNEL_SIZE; j++) {
        sum += GVB(input, index+j) * GVB(kernel, j);
    }

    COPY_VALUE(&GVB(output, index), sum);

		printf_apollo("Output[%d] = %d\n", index, sum);

    COPY_VALUE(&GVB(idx), index + 1);

    if (index < OUTPUT_SIZE) {
        TRANSITION_TO(compute_convolution_task);
    }
}

ENTRY_TASK(populate);
INIT_FUNC(init_hw);

//*****************************************************************************
//
// Main
//
//*****************************************************************************
int main(void)
{
    _init();
	
		while (1)
    {
        task_prologue();
			printf_apollo("Current task code: %d\n", CUR_TASK->idx);
				switch(CUR_TASK->idx) {
					case 0:
						_entry_task();
						break;
					case 1: 
						populate();
						break;
					case 2:
						compute_convolution_task();
				}
    }
	
		//
    // We are done printing.
    // Disable debug printf messages on ITM.
    //
//    am_util_stdio_printf("Done with prints. Entering While loop\n");
//    am_bsp_debug_printf_disable();
		
		
}
