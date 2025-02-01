/*
 * strings.h
 *
 *  Created on: 05 dec. 2021
 *      Author: Ludo
 */

#ifndef __STRINGS_H__
#define __STRINGS_H__

#ifndef EMBEDDED_UTILS_DISABLE_FLAGS_FILE
#include "embedded_utils_flags.h"
#endif
#include "types.h"

/*** STRING macros ***/

#define STRING_CHAR_NULL    '\0'
#define STRING_NULL         "\0"
#define STRING_CHAR_CR      '\r'
#define STRING_CHAR_LF      '\n'
#define STRING_CHAR_MINUS   '-'
#define STRING_CHAR_DOT     '.'
#define STRING_CHAR_COMMA   ','
#define STRING_CHAR_SPACE   ' '

/*** STRING structures ***/

/*!******************************************************************
 * \enum STRING_status_t
 * \brief STRING driver error codes.
 *******************************************************************/
typedef enum {
    // Driver errors.
    STRING_SUCCESS = 0,
    STRING_ERROR_NULL_PARAMETER,
    STRING_ERROR_FORMAT,
    STRING_ERROR_BOOLEAN_INVALID,
    STRING_ERROR_BOOLEAN_SIZE,
    STRING_ERROR_HEXADECIMAL_INVALID,
    STRING_ERROR_HEXADECIMAL_ODD_SIZE,
    STRING_ERROR_HEXADECIMAL_OVERFLOW,
    STRING_ERROR_DECIMAL_INVALID,
    STRING_ERROR_DECIMAL_OVERFLOW,
    STRING_ERROR_NUMBER_OF_DIGITS_UNDERFLOW,
    STRING_ERROR_NUMBER_OF_DIGITS_OVERFLOW,
    STRING_ERROR_SIZE_OVERFLOW,
    STRING_ERROR_COPY_OVERFLOW,
    STRING_ERROR_APPEND_OVERFLOW,
    STRING_ERROR_TEXT_JUSTIFICATION,
    // Last base value.
    STRING_ERROR_BASE_LAST = 0x0100
} STRING_status_t;

#ifndef EMBEDDED_UTILS_STRING_DRIVER_DISABLE

/*!******************************************************************
 * \enum STRING_format_t
 * \brief String formats.
 *******************************************************************/
typedef enum {
    STRING_FORMAT_BOOLEAN = 0,
    STRING_FORMAT_HEXADECIMAL,
    STRING_FORMAT_DECIMAL,
    STRING_FORMAT_LAST
} STRING_format_t;

/*!******************************************************************
 * \enum STRING_justification_t
 * \brief String justification formats.
 *******************************************************************/
typedef enum {
    STRING_JUSTIFICATION_LEFT = 0,
    STRING_JUSTIFICATION_CENTER,
    STRING_JUSTIFICATION_RIGHT,
    STRING_JUSTIFICATION_LAST
} STRING_justification_t;

/*!******************************************************************
 * \struct STRING_copy_t
 * \brief String copy operation parameters.
 *******************************************************************/
typedef struct {
    char_t* source;
    char_t* destination;
    uint32_t destination_size;
    STRING_justification_t justification;
    uint8_t flush_flag;
    char_t flush_char;
} STRING_copy_t;

/*** STRING functions ***/

/*!******************************************************************
 * \fn STRING_status_t STRING_integer_to_string(int32_t value, STRING_format_t format, uint8_t print_prefix, char_t* str)
 * \brief Convert an integer to the corresponding string representation.
 * \param[in]   value: Integer to convert.
 * \param[in]   format: Format of the output string.
 * \param[in]   print_prefix: Print the base prefix if non zero.
 * \param[out]  str: Pointer to the destination string.
 * \retval      Function execution status.
 *******************************************************************/
STRING_status_t STRING_integer_to_string(int32_t value, STRING_format_t format, uint8_t print_prefix, char_t* str);

/*!******************************************************************
 * \fn STRING_status_t STRING_integer_to_floating_decimal_string(int32_t value, uint8_t divider_exponent, uint8_t number_of_digits, char_t* str)
 * \brief Convert an integer to the corresponding string representation in floating point format.
 * \param[in]   value: Integer to convert.
 * \param[in]   divider_exponent: Input value will be divider by 10^(divider_exponent) before being represented.
 * \param[in]   number_of_digits: Number of digits available in the output string.
 * \param[out]  str: Pointer to the destination string.
 * \retval      Function execution status.
 *******************************************************************/
STRING_status_t STRING_integer_to_floating_decimal_string(int32_t value, uint8_t divider_exponent, uint8_t number_of_digits, char_t* str);

/*!******************************************************************
 * \fn STRING_status_t STRING_string_to_integer(char_t* str, STRING_format_t format, uint8_t number_of_digits, int32_t* value)
 * \brief Convert a string to the corresponding value.
 * \param[in]   str: String to convert.
 * \param[in]   format: Format of the input string.
 * \param[in]   number_of_digits: Number of digits of the output value.
 * \param[out]  value: Pointer to the destination value.
 * \retval      Function execution status.
 *******************************************************************/
STRING_status_t STRING_string_to_integer(char_t* str, STRING_format_t format, uint8_t number_of_digits, int32_t* value);

/*!******************************************************************
 * \fn STRING_status_t STRING_byte_array_to_hexadecimal_string(uint8_t* data, uint32_t data_size_bytes, uint8_t print_prefix, char_t* str)
 * \brief Convert a byte array to the corresponding string representation.
 * \param[in]   data: Byte array to convert.
 * \param[in]   data_size: Size of the input byte array.
 * \param[in]   print_prefix: Print the base prefix if non zero.
 * \param[out]  str: Pointer to the destination string.
 * \retval      Function execution status.
 *******************************************************************/
STRING_status_t STRING_byte_array_to_hexadecimal_string(uint8_t* data, uint32_t data_size_bytes, uint8_t print_prefix, char_t* str);

/*!******************************************************************
 * \fn STRING_status_t STRING_hexadecimal_string_to_byte_array(char_t* str, char_t end_char, uint8_t* data, uint32_t* extracted_size)
 * \brief Convert a string to the corresponding byte array.
 * \param[in]   str: String to convert.
 * \param[in]   end_character: Character used as string delimiter.
 * \param[out]  data: Pointer to the destination byte array.
 * \param[out]  extracted_size: Pointer to the effective number of bytes converted.
 * \retval      Function execution status.
 *******************************************************************/
STRING_status_t STRING_hexadecimal_string_to_byte_array(char_t* str, char_t end_character, uint8_t* data, uint32_t* extracted_size);

/*!******************************************************************
 * \fn STRING_status_t STRING_get_size(char_t* str, uint32_t* size)
 * \brief Get the size of a NULL terminated string.
 * \param[in]   str: String to read.
 * \param[out]  size: Pointer to the string size.
 * \retval      Function execution status.
 *******************************************************************/
STRING_status_t STRING_get_size(char_t* str, uint32_t* size);

/*!******************************************************************
 * \fn STRING_status_t STRING_copy(STRING_copy_t* copy)
 * \brief Copy a string into another one.
 * \param[in]   copy: Pointer to the copy operation parameters.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
STRING_status_t STRING_copy(STRING_copy_t* copy);

/*!******************************************************************
 * \fn STRING_status_t STRING_append_string(char_t* str, uint32_t str_size_max, char_t* new_str, uint32_t* str_size)
 * \brief Append a string to another one.
 * \param[in]   str: Destination string.
 * \param[in]   str_size_max: Maximum size of the destination string.
 * \param[in]   new_str: String to append.
 * \param[out]  str_size: Pointer to the new size of the destination string.
 * \retval      Function execution status.
 *******************************************************************/
STRING_status_t STRING_append_string(char_t* str, uint32_t str_size_max, char_t* new_str, uint32_t* str_size);

/*!******************************************************************
 * \fn STRING_status_t STRING_append_integer(char_t* str, uint32_t str_size_max, int32_t value, STRING_format_t format, uint8_t print_prefix, uint32_t* str_size)
 * \brief Convert an integer to its string representation and append it to another string.
 * \param[in]   str: Destination string.
 * \param[in]   str_size_max: Maximum size of the destination string.
 * \param[in]   value: Value to convert and append.
 * \param[in]   format: Output format of the value to append.
 * \param[in]   print_prefix: Print the base prefix if non zero.
 * \param[out]  str_size: Pointer to the new size of the destination string.
 * \retval      Function execution status.
 *******************************************************************/
STRING_status_t STRING_append_integer(char_t* str, uint32_t str_size_max, int32_t value, STRING_format_t format, uint8_t print_prefix, uint32_t* str_size);

/*!******************************************************************
 * \fn STRING_status_t STRING_append_byte_array(char_t* str, uint32_t str_size_max, uint8_t* data, uint32_t data_size_bytes, uint8_t print_prefix, uint32_t* str_size)
 * \brief Convert a byte array to its hexadecimal string representation and append it to another string.
 * \param[in]   str: Destination string.
 * \param[in]   str_size_max: Maximum size of the destination string.
 * \param[in]   data: Byte array to append.
 * \param[in]   data_size: Size of the input byte array.
 * \param[in]   print_prefix: Print the base prefix if non zero.
 * \param[out]  str_size: Pointer to the new size of the destination string.
 * \retval      Function execution status.
 *******************************************************************/
STRING_status_t STRING_append_byte_array(char_t* str, uint32_t str_size_max, uint8_t* data, uint32_t data_size_bytes, uint8_t print_prefix, uint32_t* str_size);

/*******************************************************************/
#define STRING_exit_error(base) { ERROR_check_exit(string_status, STRING_SUCCESS, base) }

/*******************************************************************/
#define STRING_stack_error(base) { ERROR_check_stack(string_status, STRING_SUCCESS, base) }

/*******************************************************************/
#define STRING_stack_exit_error(base, code) { ERROR_check_stack_exit(string_status, STRING_SUCCESS, base, code) }

#endif /* EMBEDDED_UTILS_STRING_DRIVER_DISABLE */

#endif /* __STRINGS_H__ */
