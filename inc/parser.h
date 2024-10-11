/*
 * parser.h
 *
 *  Created on: 10 dec. 2021
 *      Author: Ludo
 */

#ifndef __PARSER_H__
#define __PARSER_H__

#ifndef EMBEDDED_UTILS_DISABLE_FLAGS_FILE
#include "embedded_utils_flags.h"
#endif
#include "string.h"
#include "types.h"

/*** PARSER structures ***/

/*!******************************************************************
 * \enum PARSER_status_t
 * \brief PARSER driver error codes.
 *******************************************************************/
typedef enum {
    // Driver errors.
    PARSER_SUCCESS,
    PARSER_ERROR_NULL_PARAMETER,
    PARSER_ERROR_MODE,
    PARSER_ERROR_UNKNOWN_COMMAND,
    PARSER_ERROR_BUFFER_SIZE,
    PARSER_ERROR_HEADER_NOT_FOUND,
    PARSER_ERROR_SEPARATOR_NOT_FOUND,
    PARSER_ERROR_PARAMETER_NOT_FOUND,
    PARSER_ERROR_BYTE_ARRAY_SIZE,
    // Low level drivers errors.
    PARSER_ERROR_BASE_STRING = 0x0100,
    // Last base value.
    PARSER_ERROR_BASE_LAST = (PARSER_ERROR_BASE_STRING + STRING_ERROR_BASE_LAST)
} PARSER_status_t;

#ifndef EMBEDDED_UTILS_PARSER_DRIVER_DISABLE

/*!******************************************************************
 * \enum PARSER_mode_t
 * \brief PARSER compare modes.
 *******************************************************************/
typedef enum {
    PARSER_MODE_COMMAND,
    PARSER_MODE_HEADER,
    PARSER_MODE_LAST
} PARSER_mode_t;

/*!******************************************************************
 * \struct PARSER_context_t
 * \brief PARSER context structure.
 *******************************************************************/
typedef struct {
    char_t* buffer;
    uint32_t buffer_size;
    uint32_t start_idx;
    uint32_t separator_idx;
} PARSER_context_t;

/*** PARSER functions ***/

/*!******************************************************************
 * \fn PARSER_status_t PARSER_compare(PARSER_context_t* parser_ctx, PARSER_mode_t mode, char_t* reference)
 * \brief Compare a character buffer to a reference.
 * \param[in]   parser_ctx: Parser context.
 * \param[in]   mode: Comparison mode.
 * \param[in]   reference: String reference to compare with.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
PARSER_status_t PARSER_compare(PARSER_context_t* parser_ctx, PARSER_mode_t mode, char_t* reference);

/*!******************************************************************
 * \fn PARSER_status_t PARSER_get_parameter(PARSER_context_t* parser_ctx, STRING_format_t format, char_t separator, int32_t* parameter
 * \brief Parse a parameter within a character buffer.
 * \param[in]   parser_ctx: Parser context.
 * \param[in]   format: Parameter format.
 * \param[in]   separator: Parameter separator.
 * \param[out]  parameter: Pointer to the parameter value.
 * \retval      Function execution status.
 *******************************************************************/
PARSER_status_t PARSER_get_parameter(PARSER_context_t* parser_ctx, STRING_format_t format, char_t separator, int32_t* parameter);

/*!******************************************************************
 * \fn PARSER_status_t PARSER_get_byte_array(PARSER_context_t* parser_ctx, char_t separator, uint32_t maximum_size, uint8_t exact_size, uint8_t* parameter, uint32_t* extracted_size)
 * \brief Parse a byte array within a character buffer.
 * \param[in]   parser_ctx: Parser context.
 * \param[in]   separator: Parameter separator.
 * \param[in]   maximum_size: Maximum size of the byte array to parse.
 * \param[in]   exact_size: If non zero, returns an error is the extracted size does not equal the maximum size.
 * \param[out]  parameter: Pointer to the parameter value.
 * \param[out]  extracted_size: Pointer to the effective size of the parsed parameter.
 * \retval      Function execution status.
 *******************************************************************/
PARSER_status_t PARSER_get_byte_array(PARSER_context_t* parser_ctx, char_t separator, uint32_t maximum_size, uint8_t exact_size, uint8_t* parameter, uint32_t* extracted_size);

/*******************************************************************/
#define PARSER_exit_error(base) { ERROR_check_exit(parser_status, PARSER_SUCCESS, base) }

/*******************************************************************/
#define PARSER_stack_error(base) { ERROR_check_stack(parser_status, PARSER_SUCCESS, base) }

/*******************************************************************/
#define PARSER_stack_exit_error(base, code) { ERROR_check_stack_exit(parser_status, PARSER_SUCCESS, base, code) }

#endif /* EMBEDDED_UTILS_PARSER_DRIVER_DISABLE */

#endif /* __PARSER_H__ */
