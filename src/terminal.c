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

/*** TERMINAL functions ***/

/*******************************************************************/
TERMINAL_status_t TERMINAL_open(uint8_t instance, TERMINAL_HW_rx_irq_cb_t rx_irq_callback) {
    // Local variables.
    TERMINAL_status_t status = TERMINAL_SUCCESS;
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
	// Release hardware interface.
    status = TERMINAL_HW_de_init(instance);
    if (status != TERMINAL_SUCCESS) goto errors;
errors:
    return status;
}

/*******************************************************************/
TERMINAL_status_t TERMINAL_print_string(uint8_t instance, char_t* str) {
	// Local variables.
    TERMINAL_status_t status = TERMINAL_SUCCESS;
    STRING_status_t string_status = STRING_SUCCESS;
	uint32_t message_size = 0;
	// Get size.
	string_status = STRING_get_size(str, &message_size);
	STRING_exit_error(TERMINAL_ERROR_BASE_STRING);
	// Print message.
	status = TERMINAL_HW_write(instance, (uint8_t*) str, message_size);
	if (status != TERMINAL_SUCCESS) goto errors;
errors:
    return status;
}

/*******************************************************************/
TERMINAL_status_t TERMINAL_print_integer(uint8_t instance, int32_t value, STRING_format_t format, uint8_t print_prefix) {
	// Local variables.
    TERMINAL_status_t status = TERMINAL_SUCCESS;
    STRING_status_t string_status = STRING_SUCCESS;
	char_t str[EMBEDDED_UTILS_TERMINAL_BUFFER_SIZE] = {0x00};
	// Convert to string.
	string_status = STRING_integer_to_string(value, format, print_prefix, str);
	STRING_exit_error(TERMINAL_ERROR_BASE_STRING);
	// Print value.
	status = TERMINAL_print_string(instance, str);
	if (status != TERMINAL_SUCCESS) goto errors;
errors:
    return status;
}

/*******************************************************************/
TERMINAL_status_t TERMINAL_print_byte_array(uint8_t instance, uint8_t* data, uint32_t data_size_bytes, uint8_t print_prefix) {
    // Local variables.
    TERMINAL_status_t status = TERMINAL_SUCCESS;
    STRING_status_t string_status = STRING_SUCCESS;
    char_t str[EMBEDDED_UTILS_TERMINAL_BUFFER_SIZE] = {0x00};
    // Convert to string.
    string_status = STRING_byte_array_to_hexadecimal_string(data, data_size_bytes, print_prefix, str);
    STRING_exit_error(TERMINAL_ERROR_BASE_STRING);
    // Print value.
    status = TERMINAL_print_string(instance, str);
    if (status != TERMINAL_SUCCESS) goto errors;
errors:
    return status;
}
