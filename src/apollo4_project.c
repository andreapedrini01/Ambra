#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include <string.h>
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

void init_hw() {

		//
    // Set the default cache configuration
    //
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();

    //
    // Configure the board for low power operation.
    //
    am_bsp_low_power_init();

    //
    // Initialize the printf interface for ITM output
    //
    if (am_bsp_debug_printf_enable())
    {
        // Cannot print - so no point proceeding
        while(1);
    }
		
		am_util_stdio_terminal_clear();
		
		__enable_irq();
		init_state_manager();
}

//tasks
void my_task();
void another_task();

TASK(1, my_task);
TASK(2, another_task);

void my_task() {
	
	am_util_stdio_printf("I am in my_task\n");
  TRANSITION_TO(another_task);
}

void another_task() {
	am_util_stdio_printf("I am in another_task\n");
}

ENTRY_TASK(my_task);
INIT_FUNC(init_hw);

//*****************************************************************************
//
// Main
//
//*****************************************************************************
int
main(void)
{
    _init();
	
//		task_prologue();
//		curctx->task->func();
	
		//
    // We are done printing.
    // Disable debug printf messages on ITM.
    //
    am_util_stdio_printf("Done with prints. Entering While loop\n");
    am_bsp_debug_printf_disable();
		
		while (1)
    {
        // sleep
        am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);
    }
}
