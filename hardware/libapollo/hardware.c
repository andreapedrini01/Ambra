#include "../libambra/ambra.h"

void cache_configuration() {
	am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
	am_hal_cachectrl_enable();
}

uint32_t get_time_us(void) {
	//at 6MHz 1 tick is 0.166167 ns, tick_num * time_one_tick in ns
  return (am_hal_stimer_counter_get() * 166667/ 1000);
}

void init_hw() {
	am_hal_stimer_reset_config();
	am_hal_stimer_config(AM_HAL_STIMER_HFRC_6MHZ);
	am_hal_stimer_counter_clear();
	
	//
    // Set the default cache configuration
    //
    cache_configuration();

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

void printf_apollo(const char *string, ...) {
	va_list args;
  va_start(args, string);
    
  char buffer[256];
  vsnprintf(buffer, sizeof(buffer), string, args);  // Format the string
  am_util_stdio_printf("%s", buffer);
    
  va_end(args);
}
