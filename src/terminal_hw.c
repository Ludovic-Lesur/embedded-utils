/*
 * terminal_hw.c
 *
 *  Created on: 29 sep. 2024
 *      Author: Ludo
 */

#include "terminal_hw.h"

#ifndef EMBEDDED_UTILS_DISABLE_FLAGS_FILE
#include "embedded_utils_flags.h"
#endif
#include "types.h"

#ifndef EMBEDDED_UTILS_TERMINAL_DRIVER_DISABLE

/*** TERMINAL HW functions ***/

/*******************************************************************/
TERMINAL_status_t __attribute__((weak)) TERMINAL_HW_init(uint8_t instance, TERMINAL_HW_rx_irq_cb_t rx_irq_callback) {
    // Local variables.
    TERMINAL_status_t status = TERMINAL_SUCCESS;
	/* To be implemented */
    UNUSED(instance);
    UNUSED(rx_irq_callback);
    return status;
}

/*******************************************************************/
TERMINAL_status_t __attribute__((weak)) TERMINAL_HW_de_init(uint8_t instance) {
    // Local variables.
    TERMINAL_status_t status = TERMINAL_SUCCESS;
	/* To be implemented */
    UNUSED(instance);
    return status;
}

/*******************************************************************/
TERMINAL_status_t __attribute__((weak)) TERMINAL_HW_write(uint8_t instance, uint8_t* data, uint32_t data_size_bytes) {
    // Local variables.
    TERMINAL_status_t status = TERMINAL_SUCCESS;
	/* To be implemented */
    UNUSED(instance);
	UNUSED(data);
	UNUSED(data_size_bytes);
	return status;
}

#endif /* EMBEDDED_UTILS_TERMINAL_DRIVER_DISABLE */
