/*
 * terminal.h
 *
 *  Created on: 29 sep. 2024
 *      Author: Ludo
 */

#ifndef __TERMINAL_H__
#define __TERMINAL_H__

#ifndef EMBEDDED_UTILS_DISABLE_FLAGS_FILE
#include "embedded_utils_flags.h"
#endif
#include "string.h"
#include "types.h"

/*** TERMINAL structures ***/

typedef enum {
    // Driver errors.
    TERMINAL_SUCCESS = 0,
    TERMINAL_ERROR_NULL_PARAMETER,
    // Low level drivers errors.
    TERMINAL_ERROR_BASE_STRING = 0x0100,
    TERMINAL_ERROR_BASE_HW_INTERFACE = (TERMINAL_ERROR_BASE_STRING + STRING_ERROR_BASE_LAST),
    // Last base value.
    TERMINAL_ERROR_BASE_LAST = (TERMINAL_ERROR_BASE_HW_INTERFACE + EMBEDDED_UTILS_HW_INTERFACE_ERROR_BASE_LAST)
} TERMINAL_status_t;

/*!******************************************************************
 * \fn TERMINAL_HW_rx_irq_cb_t
 * \brief Byte reception interrupt callback.
 *******************************************************************/
typedef void (*TERMINAL_HW_rx_irq_cb_t)(uint8_t data);

/*** TERMINAL functions ***/

/*!******************************************************************
 * \fn TERMINAL_status_t TERMINAL_open(uint8_t instance, TERMINAL_HW_rx_irq_cb_t rx_irq_callback)
 * \brief Open a terminal with a physical interface to print and receive data.
 * \param[in]   instance: Terminal instance to initialize.
 * \param[in]   rx_irq_callback: Function to be called when a byte is received.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
TERMINAL_status_t TERMINAL_open(uint8_t instance, TERMINAL_HW_rx_irq_cb_t rx_irq_callback);

/*!******************************************************************
 * \fn TERMINAL_status_t TERMINAL_close(uint8_t instance)
 * \brief Close a terminal.
 * \param[in]   instance: Terminal instance to close.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
TERMINAL_status_t TERMINAL_close(uint8_t instance);

/*!******************************************************************
 * \fn TERMINAL_status_t TERMINAL_print_string(uint8_t instance, char_t* str)
 * \brief Print a message on terminal.
 * \param[in]   instance: Terminal instance to use.
 * \param[in]   str: null-terminated string to print.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
TERMINAL_status_t TERMINAL_print_string(uint8_t instance, char_t* str);

/*!******************************************************************
 * \fn TERMINAL_status_t TERMINAL_print_integer(uint8_t instance, int32_t value, TERMINAL_format_t format, uint8_t print_prefix)
 * \brief Print a value on terminal.
 * \param[in]   instance: Terminal instance to use.
 * \param[in]   value: Integer to print.
 * \param[in]   format: Format of the output string.
 * \param[in]   print_prefix: Print the base prefix if non zero.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
TERMINAL_status_t TERMINAL_print_integer(uint8_t instance, int32_t value, STRING_format_t format, uint8_t print_prefix);

/*!******************************************************************
 * \fn TERMINAL_status_t TERMINAL_print_byte_array(uint8_t instance, uint8_t* data, uint32_t data_size_bytes, uint8_t print_prefix)
 * \brief Print a byte array on terminal.
 * \param[in]   instance: Terminal instance to use.
 * \param[in]   data: Byte array to convert.
 * \param[in]   data_size_bytes: Size of the input byte array.
 * \param[in]   print_prefix: Print the base prefix if non zero.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
TERMINAL_status_t TERMINAL_print_byte_array(uint8_t instance, uint8_t* data, uint32_t data_size_bytes, uint8_t print_prefix);

/*******************************************************************/
#define TERMINAL_exit_error(base) { ERROR_check_exit(terminal_status, TERMINAL_SUCCESS, base) }

/*******************************************************************/
#define TERMINAL_stack_error(base) { ERROR_check_stack(terminal_status, TERMINAL_SUCCESS, base) }

/*******************************************************************/
#define TERMINAL_stack_exit_error(base, code) { ERROR_check_stack_exit(terminal_status, TERMINAL_SUCCESS, base, code) }

#endif /* __TERMINAL_H__ */
