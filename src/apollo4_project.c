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
#define GVB(type, ...) GVB_(type, ##__VA_ARGS__, 2, 1)
#define GVB_(type, i, n, ...) GVB##n(type, i)
#define GVB1(type, ...) manager.buffer[1-manager.index.value].type.value
#define GVB2(type, i) manager.buffer[1-manager.index.value].type[i].value

#define ALIGN16 __attribute__((aligned(16)))

const uint32_t INIT_SIGNATURE = 0xA90110;

typedef struct
{
	uint32_t value ALIGN16;
} AlignedVar;

typedef struct
{
		//all power failure resiliant variables listed here
		AlignedVar x;
		AlignedVar y;
		AlignedVar array[3];
} CritVar;

typedef struct
{
		//Double buffer
		CritVar buffer[2];
		//Signature to know if variables have to be initialized
		AlignedVar signature;
		//Journaling index
		AlignedVar needCommit;
		//Main index
		AlignedVar index;
	
} __align(16) StateManager;

//global variables
GLOBAL_SB2(StateManager, manager);

void init_state_manager() {
	if(GV(signature) != INIT_SIGNATURE) {
    COPY_VALUE(GV(needCommit), 0);
    COPY_VALUE(GV(index), 0);

		int ind = GV(index);
		CritVar* buffer1 = &manager.buffer[1-ind];
		CritVar* buffer0 = &manager.buffer[ind];
		
		uint32_t *p_buffer1 = (uint32_t *)buffer1;
    uint32_t *p_buffer0 = (uint32_t *)buffer0;
		
		uint32_t n = sizeof(CritVar) / sizeof(uint32_t);
		am_util_stdio_printf("n = %X\n", n);
		for (uint32_t i = 0; i < n; i++) {
				COPY_VALUE(p_buffer1[i], 0);
				COPY_VALUE(p_buffer0[i], 0);
    }
		
		//init done, set signature
		COPY_VALUE(GV(signature), INIT_SIGNATURE);
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
void commit_state() {
	COPY_VALUE(GV(needCommit), 1-GV(needCommit));
	COPY_VALUE(GV(index), GV(needCommit));
}

//rollback to the original values
void rollback_state() {
		int ind = GV(index);
		CritVar* dest = &manager.buffer[1-ind];
		CritVar* src = &manager.buffer[ind];
		// Considering the struct of crit variables as an array
    uint32_t *p_dest = (uint32_t *)dest;
    const uint32_t *p_src = (const uint32_t *)src;
    // Calculate how many words are in CritVar
    uint32_t n = sizeof(CritVar) / sizeof(uint32_t);

    for (uint32_t i = 0; i < n; i++) {
        if (p_dest[i] != p_src[i]) {
            COPY_VALUE(p_dest[i], p_src[i]);
        }
    }
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
	
		COPY_VALUE(GVB(x), 7);
		COPY_VALUE(GVB(array, 2), 4);
		am_util_stdio_printf("work buffer x = %X\n", GVB(x));
		am_util_stdio_printf("original buffer x = %X\n", manager.buffer[manager.index.value].x.value);
		for(int i=0; i<3; i++) {
			am_util_stdio_printf("work buffer array = %X\n", GVB(array, i));
		}
		commit_state();
		rollback_state();
		am_util_stdio_printf("work buffer x = %X\n", GVB(x));
		am_util_stdio_printf("original buffer x = %X\n", manager.buffer[manager.index.value].x.value);
		for(int i=0; i<3; i++) {
			am_util_stdio_printf("work buffer array = %X\n", GVB(array, i));
		}
	
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
