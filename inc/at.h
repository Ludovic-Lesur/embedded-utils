/*
 * at.h
 *
 *  Created on: 03 nov. 2024
 *      Author: Ludo
 */

#ifndef __AT_H__
#define __AT_H__

#ifndef EMBEDDED_UTILS_DISABLE_FLAGS_FILE
#include "embedded_utils_flags.h"
#endif
#include "error.h"
#include "parser.h"
#include "terminal.h"
#include "types.h"

/*** AT structures ***/

/*!******************************************************************
 * \enum AT_status_t
 * \brief AT driver error codes.
 *******************************************************************/
typedef enum {
    // Driver errors.
    AT_SUCCESS = 0,
    AT_ERROR_NULL_PARAMETER,
    AT_ERROR_COMMANDS_LIST_FULL,
    AT_ERROR_COMMANDS_LIST_NOT_FOUND,
    AT_ERROR_UNKOWN_COMMAND,
    AT_ERROR_COMMAND_EXECUTION,
    // Low level drivers errors.
    AT_ERROR_BASE_TERMINAL = 0x0100,
    AT_ERROR_BASE_PARSER = (AT_ERROR_BASE_TERMINAL + TERMINAL_ERROR_BASE_LAST),
    // Last base value.
    AT_ERROR_BASE_LAST = (AT_ERROR_BASE_PARSER + PARSER_ERROR_BASE_LAST)
} AT_status_t;

#ifndef EMBEDDED_UTILS_AT_DRIVER_DISABLE

/*!******************************************************************
 * \fn AT_process_callback_t
 * \brief AT command process callback.
 *******************************************************************/
typedef void (*AT_process_callback_t)(void);

/*!******************************************************************
 * \fn AT_command_callback_t
 * \brief AT command execution callback.
 *******************************************************************/
typedef AT_status_t (*AT_command_callback_t)(void);

/*!******************************************************************
 * \enum AT_command_t
 * \brief AT command structure.
 *******************************************************************/
typedef struct {
    const char_t* syntax;
    const char_t* parameters;
    const char_t* description;
    AT_command_callback_t callback;
} AT_command_t;

/*** AT functions ***/

/*!******************************************************************
 * \fn AT_status_t AT_init(uint8_t terminal_instance, AT_process_callback_t process_callback, PARSER_context_t** parser_context_ptr)
 * \brief Initialize AT command manager.
 * \param[in]   terminal_instance: Terminal instance associated to the AT interface.
 * \param[in]   process_callback: Function to be called when the AT parser has to be processed.
 * \param[out]  parser_context_ptr: Pointer which will be set to the internal AT parser.
 * \retval      Function execution status.
 *******************************************************************/
AT_status_t AT_init(uint8_t terminal_instance, AT_process_callback_t process_callback, PARSER_context_t** parser_context_ptr);

/*!******************************************************************
 * \fn AT_status_t AT_de_init(void)
 * \brief Release AT command manager.
 * \param[in]   none
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
AT_status_t AT_de_init(void);

/*!******************************************************************
 * \fn AT_status_t AT_process(uint8_t instance)
 * \brief Process AT command driver.
 * \param[in]   none
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
AT_status_t AT_process(void);

/*!******************************************************************
 * \fn AT_status_t AT_register_command(const AT_command_t* command)
 * \brief Register an AT command.
 * \param[in]   command: Pointer to the command to register.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
AT_status_t AT_register_command(const AT_command_t* command);

/*!******************************************************************
 * \fn AT_status_t AT_unregister_command(const AT_command_t* command)
 * \brief Unregister an AT command.
 * \param[in]   command: Pointer to the command to unregister.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
AT_status_t AT_unregister_command(const AT_command_t* command);

/*!******************************************************************
 * \fn void AT_reply_add_string(char_t* str)
 * \brief Add a message to the reply buffer.
 * \param[in]   str: null-terminated string to print.
 * \param[out]  none
 * \retval      none
 *******************************************************************/
void AT_reply_add_string(char_t* str);

/*!******************************************************************
 * \fn void AT_reply_add_integer(int32_t value, STRING_format_t format, uint8_t print_prefix)
 * \brief Add an integer to the reply buffer.
 * \param[in]   value: Integer to print.
 * \param[in]   format: Format of the output string.
 * \param[in]   print_prefix: Print the base prefix if non zero.
 * \param[out]  none
 * \retval      none
 *******************************************************************/
void AT_reply_add_integer(int32_t value, STRING_format_t format, uint8_t print_prefix);

/*!******************************************************************
 * \fn void AT_reply_add_byte_array(uint8_t instance, uint8_t* data, uint32_t data_size_bytes, uint8_t print_prefix)
 * \brief Add a byte array to the reply buffer.
 * \param[in]   data: Byte array to print.
 * \param[in]   data_size_bytes: Size of the input byte array.
 * \param[in]   print_prefix: Print the base prefix if non zero.
 * \param[out]  none
 * \retval      none
 *******************************************************************/
void AT_reply_add_byte_array(uint8_t* data, uint32_t data_size_bytes, uint8_t print_prefix);

/*!******************************************************************
 * \fn void AT_send_reply(void)
 * \brief Send the reply buffer.
 * \param[in]   none
 * \param[out]  none
 * \retval      none
 *******************************************************************/
void AT_send_reply(void);

/*******************************************************************/
#define AT_exit_error(base) { ERROR_check_exit(at_status, AT_SUCCESS, base) }

/*******************************************************************/
#define AT_stack_error(base) { ERROR_check_stack(at_status, AT_SUCCESS, base) }

/*******************************************************************/
#define AT_stack_exit_error(base, code) { ERROR_check_stack_exit(at_status, AT_SUCCESS, base, code) }

#endif /* EMBEDDED_UTILS_AT_DRIVER_DISABLE */

#endif /* __AT_H__ */
