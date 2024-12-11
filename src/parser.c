/*
 * parser.c
 *
 *  Created on: 10 dec. 2021
 *      Author: Ludo
 */

#include "parser.h"

#ifndef EMBEDDED_UTILS_DISABLE_FLAGS_FILE
#include "embedded_utils_flags.h"
#endif
#include "error.h"
#include "string.h"
#include "math.h"
#include "types.h"

#ifndef EMBEDDED_UTILS_PARSER_DRIVER_DISABLE

/*** PARSER local functions ***/

/*******************************************************************/
#define _PARSER_check_pointer(ptr) { \
    if (ptr == NULL) { \
        status = PARSER_ERROR_NULL_PARAMETER; \
        goto errors; \
    } \
}

/*******************************************************************/
#define _PARSER_check_size(void) { \
    if ((parser_ctx -> buffer_size) == 0) { \
        status = PARSER_ERROR_BUFFER_SIZE; \
        goto errors; \
    } \
}

/*******************************************************************/
static PARSER_status_t _PARSER_search_separator(PARSER_context_t* parser_ctx, char_t separator) {
    // Local variables.
    PARSER_status_t status = PARSER_ERROR_SEPARATOR_NOT_FOUND;
    uint32_t idx = 0;
    // Check parameters.
    _PARSER_check_pointer(parser_ctx);
    // Starting from int8_t following the current separator (which is the start of buffer in case of first call).
    for (idx = (parser_ctx->start_index); idx < (parser_ctx->buffer_size); idx++) {
        if ((parser_ctx->buffer)[idx] == separator) {
            (parser_ctx->separator_index) = idx;
            status = PARSER_SUCCESS;
            break;
        }
    }
errors:
    return status;
}

/*** PARSER functions ***/

/*******************************************************************/
PARSER_status_t PARSER_compare(PARSER_context_t* parser_ctx, PARSER_mode_t mode, char_t* reference) {
    // Local variables.
    PARSER_status_t status = PARSER_SUCCESS;
    uint32_t idx = 0;
    // Check parameters.
    _PARSER_check_pointer(parser_ctx);
    _PARSER_check_pointer(reference);
    _PARSER_check_size();
    // Compare all characters.
    while (reference[idx] != STRING_CHAR_NULL) {
        // Compare current character.
        if ((parser_ctx->buffer)[(parser_ctx->start_index) + idx] != reference[idx]) {
            // Difference found or end of command, exit loop.
            status = PARSER_ERROR_REFERENCE_MISMATCH;
            goto errors;
        }
        idx++;
    }
    switch (mode) {
    case PARSER_MODE_STRICT:
        // Check size equality.
        if (((parser_ctx->buffer_size) - (parser_ctx->start_index)) != idx) {
            status = PARSER_ERROR_REFERENCE_MISMATCH;
            goto errors;
        }
        break;
    case PARSER_MODE_HEADER:
        // Nothing to do.
        break;
    default:
        // Unknown mode.
        status = PARSER_ERROR_MODE;
        goto errors;
        break;
    }
    // Update start index.
    (parser_ctx->start_index) += idx;
errors:
    return status;
}

/*******************************************************************/
PARSER_status_t PARSER_get_parameter(PARSER_context_t* parser_ctx, STRING_format_t format, char_t separator, int32_t* parameter) {
    // Local variables.
    PARSER_status_t status = PARSER_SUCCESS;
    STRING_status_t string_status = STRING_SUCCESS;
    uint32_t end_idx = 0;
    uint32_t param_size_char = 0;
    // Check parameters.
    _PARSER_check_pointer(parser_ctx);
    _PARSER_check_pointer(parameter);
    _PARSER_check_size();
    // Compute end index.
    if (separator != STRING_CHAR_NULL) {
        // Search separator.
        status = _PARSER_search_separator(parser_ctx, separator);
        if (status != PARSER_SUCCESS) goto errors;
        end_idx = (parser_ctx->separator_index) - 1;
    }
    else {
        end_idx = (parser_ctx->buffer_size) - 1;
    }
    // Compute parameter size.
    param_size_char = (end_idx - (parser_ctx->start_index) + 1);
    // Check if parameter is not empty.
    if (param_size_char == 0) {
        status = PARSER_ERROR_PARAMETER_NOT_FOUND;
        goto errors;
    }
    // Convert string.
    string_status = STRING_string_to_integer(&((parser_ctx->buffer)[parser_ctx->start_index]), format, (uint8_t) param_size_char, parameter);
    STRING_exit_error(PARSER_ERROR_BASE_STRING);
    // Update start index after decoding parameter.
    if ((parser_ctx->separator_index) > 0) {
        (parser_ctx->start_index) = (parser_ctx->separator_index) + 1;
    }
errors:
    return status;
}

/*******************************************************************/
PARSER_status_t PARSER_get_byte_array(PARSER_context_t* parser_ctx, char_t separator, uint32_t maximum_size, uint8_t exact_size, uint8_t* parameter, uint32_t* extracted_size) {
    // Local variables.
    PARSER_status_t status = PARSER_SUCCESS;
    STRING_status_t string_status = STRING_SUCCESS;
    uint32_t param_size_char = 0;
    uint32_t end_idx = 0;
    // Check parameters.
    _PARSER_check_pointer(parser_ctx);
    _PARSER_check_pointer(parameter);
    _PARSER_check_pointer(extracted_size);
    _PARSER_check_size();
    // Compute end index.
    if (separator != STRING_CHAR_NULL) {
        // Search separator.
        status = _PARSER_search_separator(parser_ctx, separator);
        if (status != PARSER_SUCCESS) goto errors;
        end_idx = (parser_ctx->separator_index) - 1;
    }
    else {
        end_idx = (parser_ctx->buffer_size) - 1;
    }
    // Compute parameter size.
    param_size_char = (end_idx - (parser_ctx->start_index) + 1);
    // Check if parameter is not empty.
    if (param_size_char == 0) {
        // Error in parameter -> none parameter found.
        status = PARSER_ERROR_PARAMETER_NOT_FOUND;
        goto errors;
    }
    // Convert string.
    string_status = STRING_hexadecimal_string_to_byte_array(&((parser_ctx->buffer)[parser_ctx->start_index]), separator, parameter, extracted_size);
    STRING_exit_error(PARSER_ERROR_BASE_STRING);
    // Update start index after decoding parameter.
    if ((parser_ctx->separator_index) > 0) {
        (parser_ctx->start_index) = (parser_ctx->separator_index) + 1;
    }
    // Check size if required.
    if (((exact_size != 0) && ((*extracted_size) != maximum_size)) || ((*extracted_size) > maximum_size)) {
        status = PARSER_ERROR_BYTE_ARRAY_SIZE;
        goto errors;
    }
errors:
    return status;
}

#endif /* EMBEDDED_UTILS_PARSER_DRIVER_DISABLE */
