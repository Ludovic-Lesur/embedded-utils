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

#ifndef EMBEDDED_UTILS_TERMINAL_DRIVER_DISABLE

/*** TERMINAL local structures ***/

/*******************************************************************/
typedef struct {
    char_t buffer[EMBEDDED_UTILS_TERMINAL_BUFFER_SIZE];
    uint32_t buffer_size;
} TERMINAL_context_t;

/*** TERMINAL local global variables ***/

static TERMINAL_context_t terminal_ctx[EMBEDDED_UTILS_TERMINAL_NUMBER];

/*** TERMINAL local functions ***/

/*******************************************************************/
#define _TERMINAL_check_instance(instance) { \
    if (instance >= EMBEDDED_UTILS_TERMINAL_NUMBER) { \
        status = TERMINAL_ERROR_INSTANCE; \
        goto errors; \
    } \
}

/*** TERMINAL functions ***/

/*******************************************************************/
TERMINAL_status_t TERMINAL_open(uint8_t instance, TERMINAL_HW_rx_irq_cb_t rx_irq_callback) {
    // Local variables.
    TERMINAL_status_t status = TERMINAL_SUCCESS;
    // Flush buffer.
    status = TERMINAL_flush_buffer(instance);
    if (status != TERMINAL_SUCCESS) goto errors;
    // Init hardware interface.
    status = TERMINAL_HW_init(instance, rx_irq_callback);
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
TERMINAL_status_t TERMINAL_buffer_add_string(uint8_t instance, char_t* str) {
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
TERMINAL_status_t TERMINAL_buffer_add_integer(uint8_t instance, int32_t value, STRING_format_t format, uint8_t print_prefix) {
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
TERMINAL_status_t TERMINAL_buffer_add_byte_array(uint8_t instance, uint8_t* data, uint32_t data_size_bytes, uint8_t print_prefix) {
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
TERMINAL_status_t TERMINAL_print_buffer(uint8_t instance) {
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

/*******************************************************************/
TERMINAL_status_t TERMINAL_flush_buffer(uint8_t instance) {
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

#endif /* EMBEDDED_UTILS_TERMINAL_DRIVER_DISABLE */
