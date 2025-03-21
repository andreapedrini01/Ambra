#include "hardware.h"
#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"
#include <stdarg.h>
#include <stdio.h>
#include "../libalpaca/alpaca.h"

void cache_configuration() {
	am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
	am_hal_cachectrl_enable();
}

void init_hw() {
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
    
  char buffer[256];  // Un buffer temporaneo per la stringa formattata
  vsnprintf(buffer, sizeof(buffer), string, args);  // Format the string
  am_util_stdio_printf("%s", buffer);  // Stampa la stringa correttamente
    
  va_end(args);
}
