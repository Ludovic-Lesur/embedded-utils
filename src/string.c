/*
 * string.c
 *
 *  Created on: 05 dec. 2021
 *      Author: Ludo
 */

#include "string.h"

#ifndef EMBEDDED_UTILS_DISABLE_FLAGS_FILE
#include "embedded_utils_flags.h"
#endif
#include "math.h"
#include "types.h"

#ifndef EMBEDDED_UTILS_STRING_DRIVER_DISABLE

/*** STRING local macros ***/

#define STRING_VALUE_BUFFER_SIZE            256
#define STRING_SIZE_MAX                     1024
#ifdef EMBEDDED_UTILS_STRING_HEXADECIMAL_UPPER_CASE
#define STRING_HEXADECIMAL_LETTER_START     'A'
#else
#define STRING_HEXADECIMAL_LETTER_START     'a'
#endif

/*** STRING local functions ***/

/*******************************************************************/
#define _STRING_check_pointer(ptr) { \
    if (ptr == NULL) { \
        status = STRING_ERROR_NULL_PARAMETER; \
        goto errors; \
    } \
}

/*******************************************************************/
static STRING_status_t _STRING_is_decimal_char(char_t chr) {
    // Local variables.
    STRING_status_t status = ((chr >= '0') && (chr <= '9')) ? STRING_SUCCESS : STRING_ERROR_DECIMAL_INVALID;
    return status;
}

/*******************************************************************/
static STRING_status_t _STRING_decimal_char_to_digit(char_t chr, uint8_t* value) {
    // Local variables.
    STRING_status_t status = STRING_SUCCESS;
    // Check parameters.
    status = _STRING_is_decimal_char(chr);
    if (status != STRING_SUCCESS) goto errors;
    // Perform conversion.
    (*value) = (chr - '0') & 0x0F;
errors:
    return status;
}

/*******************************************************************/
static STRING_status_t _STRING_decimal_digit_to_char(uint8_t value, char_t* chr) {
    // Local variables.
    STRING_status_t status = STRING_SUCCESS;
    // Check parameters.
    if (value > MATH_DECIMAL_DIGIT_VALUE_MAX) {
        status = STRING_ERROR_DECIMAL_OVERFLOW;
        goto errors;
    }
    // Perform conversion.
    (*chr) = (char_t) (value + '0');
errors:
    return status;
}

/*******************************************************************/
static STRING_status_t _STRING_is_hexadecimal_char(char_t chr) {
    // Local variables.
    STRING_status_t status = (((chr >= '0') && (chr <= '9')) || ((chr >= 'A') && (chr <= 'F')) || ((chr >= 'a') && (chr <= 'f'))) ? STRING_SUCCESS : STRING_ERROR_HEXADECIMAL_INVALID;
    return status;
}

/*******************************************************************/
static STRING_status_t _STRING_hexadecimal_char_to_digit(char_t chr, uint8_t* value) {
    // Local variables.
    STRING_status_t status = STRING_ERROR_HEXADECIMAL_INVALID;
    // Check ranges.
    if ((chr >= 'A') && (chr <= 'F')) {
        (*value) = (chr - 'A' + 10) & 0x0F;
        status = STRING_SUCCESS;
    }
    if ((chr >= 'a') && (chr <= 'f')) {
        (*value) = (chr - 'a' + 10) & 0x0F;
        status = STRING_SUCCESS;
    }
    if ((chr >= '0') && (chr <= '9')) {
        (*value) = (chr - '0') & 0x0F;
        status = STRING_SUCCESS;
    }
    return status;
}

/*******************************************************************/
static STRING_status_t _STRING_hexadecimal_digit_to_char(uint8_t value, char_t* chr) {
    // Local variables.
    STRING_status_t status = STRING_SUCCESS;
    // Check parameters.
    if (value > MATH_HEXADECIMAL_DIGIT_VALUE_MAX) {
        status = STRING_ERROR_HEXADECIMAL_OVERFLOW;
        goto errors;
    }
    // Perform conversion.
    (*chr) = (char_t) (value <= 9 ? (value + '0') : (value + (STRING_HEXADECIMAL_LETTER_START - 10)));
errors:
    return status;
}

/*******************************************************************/
static uint8_t _STRING_get_number_of_decimal_digits(uint32_t value) {
    // Local variables.
    uint8_t number_of_digits = 0;
    // Power loop.
    for (number_of_digits = 0; number_of_digits < MATH_S32_SIZE_DECIMAL_DIGITS; number_of_digits++) {
        if (value < MATH_POWER_10[number_of_digits]) break;
    }
    return ((number_of_digits == 0) ? 1 : number_of_digits);
}

/*******************************************************************/
static STRING_status_t _STRING_extract_decimal_digits(uint32_t value, char_t* digit_array) {
    // Local variables.
    STRING_status_t status = STRING_SUCCESS;
    uint32_t previous_decade = 0;
    uint8_t digit = 0;
    uint8_t digit_array_index = 0;
    uint8_t first_non_zero_found = 0;
    int32_t idx = 0;
    // Digits loop.
    for (idx = (MATH_U32_SIZE_DECIMAL_DIGITS - 1); idx >= 0; idx--) {
        digit = (uint8_t) ((value - previous_decade) / (MATH_POWER_10[idx]));
        previous_decade += (digit * MATH_POWER_10[idx]);
        if (digit != 0) {
            first_non_zero_found = 1;
        }
        if ((first_non_zero_found != 0) || (idx == 0)) {
            status = _STRING_decimal_digit_to_char(digit, &(digit_array[digit_array_index++]));
            if (status != STRING_SUCCESS) goto errors;
        }
    }
errors:
    return status;
}

/*** STRING functions ***/

/*******************************************************************/
STRING_status_t STRING_integer_to_string(int32_t value, STRING_format_t format, uint8_t print_prefix, char_t* str) {
    // Local variables.
    STRING_status_t status = STRING_SUCCESS;
    uint8_t first_non_zero_found = 0;
    uint32_t str_idx = 0;
    uint8_t generic_byte = 0;
    uint32_t previous_decade = 0;
    uint32_t abs_value = 0;
    int32_t idx = 0;
    // Check parameters.
    _STRING_check_pointer(str);
    // Manage negative numbers.
    if (value < 0) {
        str[str_idx++] = STRING_CHAR_MINUS;
    }
    // Get absolute value.
    MATH_abs(value, abs_value, uint32_t);
    // Build string according to format.
    switch (format) {
    case STRING_FORMAT_BOOLEAN:
        if (print_prefix != 0) {
            // Print "0b" prefix.
            str[str_idx++] = '0';
            str[str_idx++] = 'b';
        }
        for (idx = (MATH_S32_SIZE_BITS - 1); idx >= 0; idx--) {
            if (abs_value & (0b1 << idx)) {
                str[str_idx++] = '1';
                first_non_zero_found = 1;
            }
            else {
                if ((first_non_zero_found != 0) || (idx == 0)) {
                    str[str_idx++] = '0';
                }
            }
        }
        break;
    case STRING_FORMAT_HEXADECIMAL:
        if (print_prefix != 0) {
            // Print "0x" prefix.
            str[str_idx++] = '0';
            str[str_idx++] = 'x';
        }
        for (idx = ((MATH_S32_SIZE_HEXADECIMAL_DIGITS >> 1) - 1); idx >= 0; idx--) {
            generic_byte = (abs_value >> (idx << 3)) & MATH_U8_MASK;
            if (generic_byte != 0) {
                first_non_zero_found = 1;
            }
            if ((first_non_zero_found != 0) || (idx == 0)) {
                // Convert to character.
                status = _STRING_hexadecimal_digit_to_char(((generic_byte & 0xF0) >> 4), &(str[str_idx++]));
                if (status != STRING_SUCCESS) goto errors;
                status = _STRING_hexadecimal_digit_to_char(((generic_byte & 0x0F) >> 0), &(str[str_idx++]));
                if (status != STRING_SUCCESS) goto errors;
            }
        }
        break;
    case STRING_FORMAT_DECIMAL:
        if (print_prefix != 0) {
            // Print "0d" prefix.
            str[str_idx++] = '0';
            str[str_idx++] = 'd';
        }
        for (idx = (MATH_S32_SIZE_DECIMAL_DIGITS - 1); idx >= 0; idx--) {
            generic_byte = (uint8_t) ((abs_value - previous_decade) / (MATH_POWER_10[idx]));
            previous_decade += (generic_byte * MATH_POWER_10[idx]);
            if (generic_byte != 0) {
                first_non_zero_found = 1;
            }
            if ((first_non_zero_found != 0) || (idx == 0)) {
                status = _STRING_decimal_digit_to_char(generic_byte, &(str[str_idx++]));
                if (status != STRING_SUCCESS) goto errors;
            }
        }
        break;
    default:
        status = STRING_ERROR_FORMAT;
        goto errors;
    }
errors:
    str[str_idx++] = STRING_CHAR_NULL; // End string.
    return status;
}

/*******************************************************************/
STRING_status_t STRING_integer_to_floating_decimal_string(int32_t value, uint8_t divider_exponent, uint8_t number_of_digits, char_t* str) {
    // Local variables.
    STRING_status_t status = STRING_SUCCESS;
    char_t digit_array[MATH_U32_SIZE_DECIMAL_DIGITS];
    uint8_t number_of_effective_digits = number_of_digits;
    uint32_t value_abs = 0;
    uint8_t value_size_digits = 0;
    uint32_t integer_part = 0;
    uint8_t integer_part_size_digits = 0;
    uint8_t number_of_left_zero = 0;
    uint8_t left_zero_count = 0;
    uint8_t str_idx = 0;
    uint8_t digit_idx = 0;
    uint8_t char_idx = 0;
    // Init digit array.
    for (char_idx = 0; char_idx < MATH_U32_SIZE_DECIMAL_DIGITS; char_idx++) {
        digit_array[char_idx] = '0';
    }
    char_idx = 0;
    // Check parameters.
    if (number_of_digits == 0) {
        status = STRING_ERROR_NUMBER_OF_DIGITS_UNDERFLOW;
        goto errors;
    }
    if ((divider_exponent >= MATH_U32_SIZE_DECIMAL_DIGITS) || (number_of_digits > MATH_U32_SIZE_DECIMAL_DIGITS)) {
        status = STRING_ERROR_DECIMAL_OVERFLOW;
        goto errors;
    }
    if (str == NULL) {
        status = STRING_ERROR_NULL_PARAMETER;
        goto errors;
    }
    // Check sign.
    if (value < 0) {
        str[str_idx++] = STRING_CHAR_MINUS;
        number_of_effective_digits--;
    }
    if (number_of_effective_digits == 0) {
        status = STRING_ERROR_NUMBER_OF_DIGITS_UNDERFLOW;
        goto errors;
    }
    // Compute integer part.
    MATH_abs(value, value_abs, uint32_t);
    value_size_digits = _STRING_get_number_of_decimal_digits(value_abs);
    integer_part = (value_abs / MATH_POWER_10[divider_exponent]);
    integer_part_size_digits = _STRING_get_number_of_decimal_digits(integer_part);
    // Check range.
    if (integer_part_size_digits > number_of_effective_digits) {
        status = STRING_ERROR_NUMBER_OF_DIGITS_OVERFLOW;
        goto errors;
    }
    // Extract digits.
    status = _STRING_extract_decimal_digits(value_abs, (char_t*) digit_array);
    if (status != STRING_SUCCESS) goto errors;
    // Compute number of left padding zero.
    if (divider_exponent >= value_size_digits) {
        number_of_left_zero = (uint8_t) (divider_exponent - value_size_digits + 1);
    }
    // Digits loop.
    while (str_idx < number_of_digits) {
        // Compute digit.
        if (char_idx == integer_part_size_digits) {
            // Check if there is enough space for a decimal part.
            if ((number_of_effective_digits - char_idx) < 2) break;
            // Insert dot.
            str[str_idx++] = STRING_CHAR_DOT;
        }
        else {
            // Left padding.
            if (left_zero_count < number_of_left_zero) {
                str[str_idx++] = '0';
                left_zero_count++;
            }
            else {
                str[str_idx++] = digit_array[digit_idx++];
            }
        }
        char_idx++;
    }
errors:
    return status;
}

/*******************************************************************/
STRING_status_t STRING_string_to_integer(char_t* str, STRING_format_t format, uint8_t number_of_digits, int32_t* value) {
    // Local variables.
    STRING_status_t status = STRING_SUCCESS;
    uint32_t char_idx = 0;
    uint32_t start_idx = 0;
    uint8_t negative_flag = 0;
    uint8_t digit_value = 0;
    // Check parameters.
    _STRING_check_pointer(str);
    _STRING_check_pointer(value);
    // Reset result.
    (*value) = 0;
    // Manage negative numbers.
    if (str[0] == STRING_CHAR_MINUS) {
        // Set flag and increment start index to skip minus symbol.
        negative_flag = 1;
        start_idx++;
    }
    // Decode string according to format.
    switch (format) {
    case STRING_FORMAT_BOOLEAN:
        // Check if there is only 1 digit (start and end index are equal).
        if (number_of_digits != MATH_BOOLEAN_SIZE_BITS) {
            status = STRING_ERROR_BOOLEAN_SIZE;
            goto errors;
        }
        // Get digit and check if it is a bit.
        switch (str[start_idx]) {
        case '0':
            (*value) = 0;
            break;
        case '1':
            (*value) = 1;
            break;
        default:
            status = STRING_ERROR_BOOLEAN_INVALID;
            goto errors;
        }
        break;
    case STRING_FORMAT_HEXADECIMAL:
        // Check if parameter has an even number of digits (two hexadecimal characters are required to code a byte).
        if ((number_of_digits % 2) != 0) {
            status = STRING_ERROR_HEXADECIMAL_ODD_SIZE;
            goto errors;
        }
        // Check if parameter can be binary coded on 32 bits.
        if (number_of_digits > MATH_S32_SIZE_HEXADECIMAL_DIGITS) {
            status = STRING_ERROR_HEXADECIMAL_OVERFLOW;
            goto errors;
        }
        // Hexadecimal digits loop.
        for (char_idx = 0; char_idx < number_of_digits; char_idx++) {
            // Convert digit to value.
            status = _STRING_hexadecimal_char_to_digit((str[start_idx + char_idx]), &digit_value);
            if (status != STRING_SUCCESS) goto errors;
            // Add digit to result.
            (*value) |= (digit_value << ((number_of_digits - char_idx - 1) << 2));
        }
        break;
    case STRING_FORMAT_DECIMAL:
        // Check if parameter can be binary coded on 32 bits.
        if (number_of_digits > MATH_S32_SIZE_DECIMAL_DIGITS) {
            // Error in parameter -> value is too large.
            status = STRING_ERROR_DECIMAL_OVERFLOW;
            goto errors;
        }
        // Decimal digits loop.
        for (char_idx = 0; char_idx < number_of_digits; char_idx++) {
            // Convert digit to value.
            status = _STRING_decimal_char_to_digit(str[start_idx + char_idx], &digit_value);
            if (status != STRING_SUCCESS) goto errors;
            // Add digit to result.
            (*value) += (int32_t) (digit_value * MATH_POWER_10[number_of_digits - char_idx - 1]);
        }
        break;
    default:
        status = STRING_ERROR_FORMAT;
        goto errors;
    }
    // Add sign.
    if (negative_flag != 0) {
        (*value) = (-1) * (*value);
    }
errors:
    return status;
}

/*******************************************************************/
STRING_status_t STRING_byte_array_to_hexadecimal_string(uint8_t* data, uint32_t data_size_bytes, uint8_t print_prefix, char_t* str) {
    // Local variables.
    STRING_status_t status = STRING_SUCCESS;
    uint32_t idx = 0;
    // Check parameters.
    _STRING_check_pointer(data);
    _STRING_check_pointer(str);
    // Build string.
    for (idx = 0; idx < data_size_bytes; idx++) {
        status = STRING_integer_to_string((int32_t) data[idx], STRING_FORMAT_HEXADECIMAL, print_prefix, &(str[idx << 1]));
        if (status != STRING_SUCCESS) goto errors;
    }
errors:
    str[idx << 1] = STRING_CHAR_NULL; // End string.
    return status;
}

/*******************************************************************/
STRING_status_t STRING_hexadecimal_string_to_byte_array(char_t* str, char_t end_character, uint8_t* data, uint32_t* extracted_size) {
    // Local variables.
    STRING_status_t status = STRING_SUCCESS;
    uint32_t char_idx = 0;
    int32_t value = 0;
    // Check parameters.
    _STRING_check_pointer(str);
    _STRING_check_pointer(data);
    _STRING_check_pointer(extracted_size);
    // Reset extracted size.
    (*extracted_size) = 0;
    // Char loop.
    while ((str[char_idx] != end_character) && (str[char_idx] != STRING_CHAR_NULL)) {
        // Check character.
        if ((char_idx % 2) == 0) {
            status = _STRING_is_hexadecimal_char(str[char_idx]);
            if (status != STRING_SUCCESS) goto errors;
        }
        else {
            // Check character.
            if (_STRING_is_hexadecimal_char(str[char_idx]) != STRING_SUCCESS) {
                status = STRING_ERROR_HEXADECIMAL_ODD_SIZE;
                goto errors;
            }
            // Convert byte.
            status = STRING_string_to_integer(&(str[char_idx - 1]), STRING_FORMAT_HEXADECIMAL, 2, &value);
            if (status != STRING_SUCCESS) goto errors;
            // Append byte.
            data[char_idx >> 1] = (uint8_t) value;
            (*extracted_size)++;
        }
        char_idx++;
    }
    // Check that the number of analyzed characters is even.
    if ((char_idx % 2) != 0) {
        status = STRING_ERROR_HEXADECIMAL_ODD_SIZE;
        goto errors;
    }
errors:
    return status;
}

/*******************************************************************/
STRING_status_t STRING_get_size(char_t* str, uint32_t* size) {
    // Local variables.
    STRING_status_t status = STRING_SUCCESS;
    // Check parameters.
    _STRING_check_pointer(str);
    _STRING_check_pointer(size);
    // Reset result.
    (*size) = 0;
    // Compute source buffer size.
    while (str[(*size)] != STRING_CHAR_NULL) {
        (*size)++;
        // Check overflow.
        if ((*size) > STRING_SIZE_MAX) {
            status = STRING_ERROR_SIZE_OVERFLOW;
            goto errors;
        }
    }
errors:
    return status;
}

/*******************************************************************/
STRING_status_t STRING_copy(STRING_copy_t* copy) {
    // Local variables.
    STRING_status_t status = STRING_SUCCESS;
    uint32_t idx = 0;
    uint32_t source_size = 0;
    uint32_t start_idx = 0;
    uint32_t destination_idx = 0;
    // Reset destination buffer if required.
    if ((copy->flush_flag) != 0) {
        for (idx = 0; idx < (copy->destination_size); idx++)
            (copy->destination)[idx] = (copy->flush_char);
    }
    // Compute source buffer size.
    status = STRING_get_size((copy->source), &source_size);
    if (status != STRING_SUCCESS) goto errors;
    // Check size.
    if (source_size > (copy->destination_size)) {
        status = STRING_ERROR_COPY_OVERFLOW;
        goto errors;
    }
    // Compute column according to justification.
    switch (copy->justification) {
    case STRING_JUSTIFICATION_LEFT:
        start_idx = 0;
        break;
    case STRING_JUSTIFICATION_CENTER:
        start_idx = ((copy->destination_size) - source_size) / (2);
        break;
    case STRING_JUSTIFICATION_RIGHT:
        start_idx = ((copy->destination_size) - source_size);
        break;
    default:
        status = STRING_ERROR_TEXT_JUSTIFICATION;
        goto errors;
    }
    // Char loop.
    idx = 0;
    while ((copy->source)[idx] != STRING_CHAR_NULL) {
        // Check index.
        if (destination_idx >= ((copy->destination_size) - 1)) {
            status = STRING_ERROR_COPY_OVERFLOW;
            goto errors;
        }
        (copy->destination)[start_idx + idx] = (copy->source)[idx];
        idx++;
    }
errors:
    return status;
}

/*******************************************************************/
STRING_status_t STRING_append_string(char_t* str, uint32_t str_size_max, char_t* new_str, uint32_t* str_size) {
    // Local variables.
    STRING_status_t status = STRING_SUCCESS;
    uint32_t idx = 0;
    // Check parameters.
    _STRING_check_pointer(str);
    _STRING_check_pointer(new_str);
    _STRING_check_pointer(str_size);
    // Fill buffer.
    while (new_str[idx] != STRING_CHAR_NULL) {
        // Check index.
        if ((*str_size) >= str_size_max) {
            status = STRING_ERROR_APPEND_OVERFLOW;
            goto errors;
        }
        str[(*str_size)] = new_str[idx];
        // Increment size and index..
        (*str_size)++;
        idx++;
    }
errors:
    return status;
}

/*******************************************************************/
STRING_status_t STRING_append_integer(char_t* str, uint32_t str_size_max, int32_t value, STRING_format_t format, uint8_t print_prefix, uint32_t* str_size) {
    // Local variables.
    STRING_status_t status = STRING_SUCCESS;
    char_t str_value[STRING_VALUE_BUFFER_SIZE] = { STRING_CHAR_NULL };
    // Convert value to string.
    status = STRING_integer_to_string(value, format, print_prefix, str_value);
    if (status != STRING_SUCCESS) goto errors;
    // Add string.
    status = STRING_append_string(str, str_size_max, str_value, str_size);
errors:
    return status;
}

/*******************************************************************/
STRING_status_t STRING_append_byte_array(char_t* str, uint32_t str_size_max, uint8_t* data, uint32_t data_size_bytes, uint8_t print_prefix, uint32_t* str_size) {
    // Local variables.
    STRING_status_t status = STRING_SUCCESS;
    char_t str_byte_array[STRING_VALUE_BUFFER_SIZE] = { STRING_CHAR_NULL };
    // Convert byte array to hexadecimal string.
    status = STRING_byte_array_to_hexadecimal_string(data, data_size_bytes, print_prefix, str_byte_array);
    if (status != STRING_SUCCESS) goto errors;
    // Add string.
    status = STRING_append_string(str, str_size_max, str_byte_array, str_size);
errors:
    return status;
}

#endif /* EMBEDDED_UTILS_STRING_DRIVER_DISABLE */
