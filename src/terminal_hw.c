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
#include "terminal.h"
#include "types.h"

#if (!(defined EMBEDDED_UTILS_TERMINAL_DRIVER_DISABLE) && (EMBEDDED_UTILS_TERMINAL_INSTANCES_NUMBER > 0))

/*** TERMINAL HW functions ***/

/*******************************************************************/
TERMINAL_status_t __attribute__((weak)) TERMINAL_HW_init(uint8_t instance, uint32_t baud_rate, TERMINAL_rx_irq_cb_t rx_irq_callback) {
    // Local variables.
    TERMINAL_status_t status = TERMINAL_SUCCESS;
    /* To be implemented */
    UNUSED(instance);
    UNUSED(baud_rate);
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
TERMINAL_status_t __attribute__((weak)) TERMINAL_HW_enable_rx(uint8_t instance) {
    // Local variables.
    TERMINAL_status_t status = TERMINAL_SUCCESS;
    /* To be implemented */
    UNUSED(instance);
    return status;
}

/*******************************************************************/
TERMINAL_status_t __attribute__((weak)) TERMINAL_HW_disable_rx(uint8_t instance) {
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

#ifdef EMBEDDED_UTILS_TERMINAL_MODE_BUS
/*******************************************************************/
TERMINAL_status_t __attribute__((weak)) TERMINAL_HW_set_destination_address(uint8_t instance, uint8_t destination_address) {
    // Local variables.
    TERMINAL_status_t status = TERMINAL_SUCCESS;
    /* To be implemented */
    UNUSED(instance);
    UNUSED(destination_address);
    return status;
}
#endif

#endif /* EMBEDDED_UTILS_TERMINAL_DRIVER_DISABLE */
