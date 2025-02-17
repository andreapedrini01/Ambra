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

//use this to modify your work variable
//to be sure to not modify variable in wrong buffer
#define GVB(type, ...) GV_(type, ##__VA_ARGS__, 2, 1)
#define GVB_(type, i, n, ...) GV##n(type, i)
#define GVB1(type, ...) _global_ ## manager.buffer[1-manager.index].type
#define GVB2(type, i) _global_ ## manager.buffer[1-manager.index].type[i]

const uint32_t INIT_SIGNATURE = 0xA90110;

typedef struct
{
		//all power failure resiliant variables listed here
		uint32_t x;
		uint32_t y;
} CritVar;

typedef struct
{
		//Signature to know if variables have to be initialized
		uint32_t signature;
		//Journaling index
		uint32_t needCommit;
		//Main index
		uint32_t index;
		//Double buffer
		CritVar buffer[2];
	
} StateManager;

//global variables
GLOBAL_SB2(StateManager, manager);
GLOBAL_SB2(uint32_t, var); //ignore for presentation

void init_state_manager() {
	if(GV(manager.signature) != INIT_SIGNATURE) {
    COPY_VALUE(GV(manager.needCommit), 0);
    COPY_VALUE(GV(manager.index), 0);
		//Buffers init
		for(int i=0; i<2; i++) {
			COPY_VALUE(GV(manager.buffer, i).x, 0);
			COPY_VALUE(GV(manager.buffer, i).y, 0);
		}
		
		//init done, set signature
		COPY_VALUE(GV(manager.signature), INIT_SIGNATURE);
	}
}

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


//switch the index to change the buffer with original values
//
void commit_state() {
	COPY_VALUE(GV(manager.needCommit), 1-GV(manager.needCommit));
	COPY_VALUE(GV(manager.index), GV(manager.needCommit));
}

//rollback to the original values
void rollback_state() {
	int i = GV(manager.index);
	memcpy(&GV(manager.buffer, 1-i),
		&GV(manager.buffer, i),
		sizeof(CritVar));
}

//tasks
void my_task();
void another_task();

TASK(1, my_task);
TASK(2, another_task);

void my_task() {
	COPY_VALUE(GV(var), 5);
	am_util_stdio_printf("var = %#x\n", GV(var));
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
