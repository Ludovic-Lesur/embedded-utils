/*
 * terminal.c
 *
 *  Created on: 29 sep. 2024
 *      Author: Ludo
 */

#include "terminal.h"

#ifndef EMBEDDED_UTILS_DISABLE_FLAGS_FILE
#include "embedded_utils_flags.h"
#endif
#include "error.h"
#include "terminal_hw.h"
#include "string.h"
#include "types.h"

#if (!(defined EMBEDDED_UTILS_TERMINAL_DRIVER_DISABLE) && (EMBEDDED_UTILS_TERMINAL_INSTANCES_NUMBER > 0))

/*** TERMINAL local structures ***/

/*******************************************************************/
typedef struct {
    char_t buffer[EMBEDDED_UTILS_TERMINAL_BUFFER_SIZE];
    uint32_t buffer_size;
} TERMINAL_context_t;

/*** TERMINAL local global variables ***/

static TERMINAL_context_t terminal_ctx[EMBEDDED_UTILS_TERMINAL_INSTANCES_NUMBER];

/*** TERMINAL local functions ***/

/*******************************************************************/
#define _TERMINAL_check_instance(instance) { \
    if (instance >= EMBEDDED_UTILS_TERMINAL_INSTANCES_NUMBER) { \
        status = TERMINAL_ERROR_INSTANCE; \
        goto errors; \
    } \
}

/*** TERMINAL functions ***/

/*******************************************************************/
TERMINAL_status_t TERMINAL_open(uint8_t instance, uint32_t baud_rate, TERMINAL_rx_irq_cb_t rx_irq_callback) {
    // Local variables.
    TERMINAL_status_t status = TERMINAL_SUCCESS;
    // Flush buffer.
    status = TERMINAL_flush_tx_buffer(instance);
    if (status != TERMINAL_SUCCESS) goto errors;
    // Init hardware interface.
    status = TERMINAL_HW_init(instance, baud_rate, rx_irq_callback);
    if (status != TERMINAL_SUCCESS) goto errors;
errors:
    return status;
}

/*******************************************************************/
TERMINAL_status_t TERMINAL_close(uint8_t instance) {
    // Local variables.
    TERMINAL_status_t status = TERMINAL_SUCCESS;
    // Check instance.
    _TERMINAL_check_instance(instance);
    // Release hardware interface.
    status = TERMINAL_HW_de_init(instance);
    if (status != TERMINAL_SUCCESS) goto errors;
errors:
    return status;
}

/*******************************************************************/
TERMINAL_status_t TERMINAL_enable_rx(uint8_t instance) {
    // Local variables.
    TERMINAL_status_t status = TERMINAL_SUCCESS;
    // Check instance.
    _TERMINAL_check_instance(instance);
    // Release hardware interface.
    status = TERMINAL_HW_enable_rx(instance);
    if (status != TERMINAL_SUCCESS) goto errors;
errors:
    return status;
}

/*******************************************************************/
TERMINAL_status_t TERMINAL_disable_rx(uint8_t instance) {
    // Local variables.
    TERMINAL_status_t status = TERMINAL_SUCCESS;
    // Check instance.
    _TERMINAL_check_instance(instance);
    // Release hardware interface.
    status = TERMINAL_HW_disable_rx(instance);
    if (status != TERMINAL_SUCCESS) goto errors;
errors:
    return status;
}

/*******************************************************************/
TERMINAL_status_t TERMINAL_flush_tx_buffer(uint8_t instance) {
    // Local variables.
    TERMINAL_status_t status = TERMINAL_SUCCESS;
    uint32_t idx = 0;
    // Check instance.
    _TERMINAL_check_instance(instance);
    // Flush buffer.
    for (idx = 0; idx < EMBEDDED_UTILS_TERMINAL_BUFFER_SIZE; idx++) {
        terminal_ctx[instance].buffer[idx] = 0;
    }
    terminal_ctx[instance].buffer_size = 0;
errors:
    return status;
}

/*******************************************************************/
TERMINAL_status_t TERMINAL_tx_buffer_add_string(uint8_t instance, char_t* str) {
    // Local variables.
    TERMINAL_status_t status = TERMINAL_SUCCESS;
    STRING_status_t string_status = STRING_SUCCESS;
    // Check instance.
    _TERMINAL_check_instance(instance);
    // Get size.
    string_status = STRING_append_string(terminal_ctx[instance].buffer, EMBEDDED_UTILS_TERMINAL_BUFFER_SIZE, str, &(terminal_ctx[instance].buffer_size));
    STRING_exit_error(TERMINAL_ERROR_BASE_STRING);

errors:
    return status;
}

/*******************************************************************/
TERMINAL_status_t TERMINAL_tx_buffer_add_integer(uint8_t instance, int32_t value, STRING_format_t format, uint8_t print_prefix) {
    // Local variables.
    TERMINAL_status_t status = TERMINAL_SUCCESS;
    STRING_status_t string_status = STRING_SUCCESS;
    // Check instance.
    _TERMINAL_check_instance(instance);
    // Convert to string.
    string_status = STRING_append_integer(terminal_ctx[instance].buffer, EMBEDDED_UTILS_TERMINAL_BUFFER_SIZE, value, format, print_prefix, &(terminal_ctx[instance].buffer_size));
    STRING_exit_error(TERMINAL_ERROR_BASE_STRING);
errors:
    return status;
}

/*******************************************************************/
TERMINAL_status_t TERMINAL_tx_buffer_add_byte_array(uint8_t instance, uint8_t* data, uint32_t data_size_bytes, uint8_t print_prefix) {
    // Local variables.
    TERMINAL_status_t status = TERMINAL_SUCCESS;
    STRING_status_t string_status = STRING_SUCCESS;
    // Check instance.
    _TERMINAL_check_instance(instance);
    // Convert to string.
    string_status = STRING_append_byte_array(terminal_ctx[instance].buffer, EMBEDDED_UTILS_TERMINAL_BUFFER_SIZE, data, data_size_bytes, print_prefix, &(terminal_ctx[instance].buffer_size));
    STRING_exit_error(TERMINAL_ERROR_BASE_STRING);
errors:
    return status;
}

/*******************************************************************/
TERMINAL_status_t TERMINAL_send_tx_buffer(uint8_t instance) {
    // Local variables.
    TERMINAL_status_t status = TERMINAL_SUCCESS;
    // Check instance.
    _TERMINAL_check_instance(instance);
    // Print message.
    status = TERMINAL_HW_write(instance, ((uint8_t*) terminal_ctx[instance].buffer), terminal_ctx[instance].buffer_size);
    if (status != TERMINAL_SUCCESS) goto errors;
errors:
    return status;
}

#ifdef EMBEDDED_UTILS_TERMINAL_MODE_BUS
/*******************************************************************/
TERMINAL_status_t TERMINAL_set_destination_address(uint8_t instance, uint8_t destination_address) {
    // Local variables.
    TERMINAL_status_t status = TERMINAL_SUCCESS;
    // Check instance.
    _TERMINAL_check_instance(instance);
    // Print message.
    status = TERMINAL_HW_set_destination_address(instance, destination_address);
    if (status != TERMINAL_SUCCESS) goto errors;
errors:
    return status;
}
#endif

#endif /* EMBEDDED_UTILS_TERMINAL_DRIVER_DISABLE */
