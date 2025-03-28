/*
 * at.c
 *
 *  Created on: 03 nov. 2024
 *      Author: Ludo
 */

#include "at.h"

#ifndef EMBEDDED_UTILS_DISABLE_FLAGS_FILE
#include "embedded_utils_flags.h"
#endif
#include "error.h"
#include "parser.h"
#include "terminal.h"
#include "types.h"

#ifndef EMBEDDED_UTILS_AT_DRIVER_DISABLE

/*** AT local macros ***/

#define AT_HEADER       "AT"

#define AT_REPLY_OK     "OK"
#define AT_REPLY_ERROR  "ERROR:"
#define AT_REPLY_TAB    "    "

/*** AT local structures ***/

/*******************************************************************/
typedef union {
    struct {
        uint8_t irq_enable :1;
        uint8_t process :1;
        uint8_t process_pending :1;
#ifndef EMBEDDED_UTILS_AT_FORCE_OK
        uint8_t reply_sent : 1;
#endif
    };
    uint8_t all;
} AT_flags_t;

/*******************************************************************/
typedef struct {
    volatile AT_flags_t flags;
    AT_process_callback_t process_callback;
    uint8_t terminal_instance;
    const AT_command_t* commands_list[EMBEDDED_UTILS_AT_COMMANDS_LIST_SIZE];
    uint8_t commands_count;
    char_t rx_buffer[EMBEDDED_UTILS_AT_BUFFER_SIZE];
    uint32_t rx_buffer_size;
    PARSER_context_t parser;
} AT_context_t;

/*** AT local functions declaration ***/

#ifdef EMBEDDED_UTILS_AT_INTERNAL_COMMANDS_ENABLE
static AT_status_t _AT_print_commands_list(void);
static AT_status_t _AT_print_error_stack(void);
static AT_status_t _AT_print_software_version(void);
#endif

/*** AT local global variables ***/

#ifdef EMBEDDED_UTILS_AT_INTERNAL_COMMANDS_ENABLE
static const AT_command_t AT_INTERNAL_COMMANDS_LIST[] = {
    {
        .syntax = STRING_NULL,
        .parameters = NULL,
        .description = "Ping device",
        .callback = NULL
    },
    {
        .syntax = "?",
        .parameters = NULL,
        .description = "List all commands",
        .callback = &_AT_print_commands_list
    },
    {
        .syntax = "$ERROR?",
        .parameters = NULL,
        .description = "Read error stack",
        .callback = &_AT_print_error_stack
    },
    {
        .syntax = "$V?",
        .parameters = NULL,
        .description = "Get software version",
        .callback = &_AT_print_software_version
    }
};
#endif

static AT_context_t at_ctx;

/*** AT local functions ***/

/*******************************************************************/
static void _AT_rx_irq_callback(uint8_t data) {
    // Ignore null data.
    if ((data == 0x00) || (at_ctx.flags.irq_enable == 0)) goto errors;
    // Check end marker.
    if ((data == STRING_CHAR_CR) || (data == STRING_CHAR_LF)) {
        // Set flag.
        at_ctx.flags.process = 1;
        // Ask for processing.
        if ((at_ctx.process_callback != NULL) && (at_ctx.flags.process_pending == 0)) {
            // Ensure callback is called only once.
            at_ctx.flags.process_pending = 1;
            at_ctx.process_callback();
        }
    }
    else {
        // Store new byte in buffer.
        at_ctx.rx_buffer[at_ctx.rx_buffer_size] = (char_t) data;
        at_ctx.rx_buffer_size = (at_ctx.rx_buffer_size + 1) % EMBEDDED_UTILS_AT_BUFFER_SIZE;
    }
errors:
    return;
}

#ifdef EMBEDDED_UTILS_AT_INTERNAL_COMMANDS_ENABLE
/*******************************************************************/
static AT_status_t _AT_print_commands_list(void) {
    // Local variables.
    AT_status_t status = AT_SUCCESS;
    uint8_t idx = 0;
    // List all commands registered in the current instance.
    for (idx = 0; idx < at_ctx.commands_count; idx++) {
        // Print syntax.
        AT_reply_add_string(AT_HEADER);
        AT_reply_add_string((char_t*) ((at_ctx.commands_list[idx])->syntax));
        // Print parameters.
        AT_reply_add_string((char_t*) ((at_ctx.commands_list[idx])->parameters));
        AT_send_reply();
        // Print description.
        AT_reply_add_string(AT_REPLY_TAB);
        AT_reply_add_string((char_t*) ((at_ctx.commands_list[idx])->description));
        AT_send_reply();
    }
    return status;
}
#endif

#ifdef EMBEDDED_UTILS_AT_INTERNAL_COMMANDS_ENABLE
/*******************************************************************/
static AT_status_t _AT_print_error_stack(void) {
    // Local variables.
    AT_status_t status = AT_SUCCESS;
    ERROR_code_t error = EMBEDDED_UTILS_ERROR_STACK_SUCCESS_VALUE;
    uint32_t count = 0;
#ifdef EMBEDDED_UTILS_ERROR_STACK_SIGFOX
    // Import Sigfox errors into MCU stack.
    ERROR_import_sigfox_stack();
#endif
    // Unstack all errors.
    do {
        error = ERROR_stack_read();
        if (error != EMBEDDED_UTILS_ERROR_STACK_SUCCESS_VALUE) {
            if (count > 0) {
                AT_reply_add_string(":");
            }
            AT_reply_add_integer((int32_t) error, STRING_FORMAT_HEXADECIMAL, 0);
            count++;
        }
    }
    while (error != EMBEDDED_UTILS_ERROR_STACK_SUCCESS_VALUE);
    if (count == 0) {
        AT_reply_add_string("EMPTY");
    }
    AT_send_reply();
    return status;
}
#endif

#ifdef EMBEDDED_UTILS_AT_INTERNAL_COMMANDS_ENABLE
/*******************************************************************/
static AT_status_t _AT_print_software_version(void) {
    // Local variables.
    AT_status_t status = AT_SUCCESS;
    // Print software version.
    AT_reply_add_string("sw");
    AT_reply_add_integer((int32_t) EMBEDDED_UTILS_AT_SW_VERSION_MAJOR, STRING_FORMAT_DECIMAL, 0);
    AT_reply_add_string(".");
    AT_reply_add_integer((int32_t) EMBEDDED_UTILS_AT_SW_VERSION_MINOR, STRING_FORMAT_DECIMAL, 0);
    AT_reply_add_string(".");
    AT_reply_add_integer((int32_t) EMBEDDED_UTILS_AT_SW_VERSION_INDEX, STRING_FORMAT_DECIMAL, 0);
    if (EMBEDDED_UTILS_AT_SW_VERSION_DIRTY_FLAG != 0) {
        AT_reply_add_string(".dev");
    }
    AT_reply_add_string(":");
    AT_reply_add_integer((int32_t) EMBEDDED_UTILS_AT_SW_VERSION_ID, STRING_FORMAT_HEXADECIMAL, 0);
    AT_send_reply();
    return status;
}
#endif

/*******************************************************************/
static void _AT_reset_parser(void) {
    // Local variables.
    uint32_t idx = 0;
    // Flush buffers.
    for (idx = 0; idx < EMBEDDED_UTILS_AT_BUFFER_SIZE; idx++) {
        at_ctx.rx_buffer[idx] = STRING_CHAR_NULL;
    }
    at_ctx.rx_buffer_size = 0;
    // Reset parser.
    at_ctx.parser.buffer = (char_t*) (at_ctx.rx_buffer);
    at_ctx.parser.buffer_size = 0;
    at_ctx.parser.separator_index = 0;
    at_ctx.parser.start_index = 0;
}

/*******************************************************************/
static void _AT_print_ok(void) {
    // Reply OK.
    AT_reply_add_string(AT_REPLY_OK);
    AT_send_reply();
}

/*******************************************************************/
static void _AT_print_error(AT_status_t at_status) {
    // Erase eventual pending reply.
    TERMINAL_flush_tx_buffer(at_ctx.terminal_instance);
    // Reply error code.
    AT_reply_add_string(AT_REPLY_ERROR);
    AT_reply_add_integer((int32_t) at_status, STRING_FORMAT_HEXADECIMAL, 0);
    AT_send_reply();
}

/*** AT functions ***/

/*******************************************************************/
AT_status_t AT_init(AT_configuration_t* configuration, PARSER_context_t** parser_context_ptr) {
    // Local variables.
    AT_status_t status = AT_SUCCESS;
    TERMINAL_status_t terminal_status = TERMINAL_SUCCESS;
    uint8_t idx = 0;
    // Check parameters.
    if ((configuration == NULL) || (parser_context_ptr == NULL)) {
        status = AT_ERROR_NULL_PARAMETER;
        goto errors;
    }
    if ((configuration->process_callback) == NULL) {
        status = AT_ERROR_NULL_PARAMETER;
        goto errors;
    }
    // Init context.
    at_ctx.flags.all = 0;
    at_ctx.process_callback = (configuration->process_callback);
    at_ctx.terminal_instance = (configuration->terminal_instance);
    for (idx = 0; idx < EMBEDDED_UTILS_AT_COMMANDS_LIST_SIZE; idx++) {
        at_ctx.commands_list[idx] = NULL;
    }
    at_ctx.commands_count = 0;
    _AT_reset_parser();
    // Update parser pointer.
    (*parser_context_ptr) = &(at_ctx.parser);
#ifdef EMBEDDED_UTILS_AT_INTERNAL_COMMANDS_ENABLE
    // Register internal commands.
    for (idx = 0; idx < (sizeof(AT_INTERNAL_COMMANDS_LIST) / (sizeof(AT_command_t))); idx++) {
        AT_register_command(&(AT_INTERNAL_COMMANDS_LIST[idx]));
    }
#endif
    // Open terminal.
#ifdef EMBEDDED_UTILS_AT_BAUD_RATE
    terminal_status = TERMINAL_open(at_ctx.terminal_instance, EMBEDDED_UTILS_AT_BAUD_RATE, &_AT_rx_irq_callback);
#else
    terminal_status = TERMINAL_open(at_ctx.terminal_instance, (configuration->terminal_baud_rate), &_AT_rx_irq_callback);
#endif
    TERMINAL_exit_error(AT_ERROR_BASE_TERMINAL);
    // Enable interrupt.
    at_ctx.flags.irq_enable = 1;
    // Enable receiver.
    terminal_status = TERMINAL_enable_rx(at_ctx.terminal_instance);
    TERMINAL_exit_error(AT_ERROR_BASE_TERMINAL);
errors:
    return status;
}

/*******************************************************************/
AT_status_t AT_de_init(void) {
    // Local variables.
    AT_status_t status = AT_SUCCESS;
    TERMINAL_status_t terminal_status = TERMINAL_SUCCESS;
    // Disable interrupt.
    at_ctx.flags.irq_enable = 0;
    // Disable receiver.
    TERMINAL_disable_rx(at_ctx.terminal_instance);
    // Close terminal.
    terminal_status = TERMINAL_close(at_ctx.terminal_instance);
    TERMINAL_exit_error(AT_ERROR_BASE_TERMINAL);
errors:
    return status;
}

/*******************************************************************/
AT_status_t AT_process(void) {
    // Local variables.
    AT_status_t status = AT_ERROR_UNKOWN_COMMAND;
    PARSER_mode_t parser_mode = PARSER_MODE_STRICT;
    uint8_t command_idx = 0;
    // Check process flag.
    if (at_ctx.flags.process == 0) goto errors;
    // Disable interrupt and clear flag.
    at_ctx.flags.irq_enable = 0;
    at_ctx.flags.process = 0;
    // Disable receiver.
    TERMINAL_disable_rx(at_ctx.terminal_instance);
    // Update parser and reset reply flag.
    at_ctx.parser.buffer_size = at_ctx.rx_buffer_size;
#ifndef EMBEDDED_UTILS_AT_FORCE_OK
    at_ctx.flags.reply_sent = 0;
#endif
    // Check header.
    if (PARSER_compare(&(at_ctx.parser), PARSER_MODE_HEADER, AT_HEADER) == PARSER_SUCCESS) {
        // Loop on available commands.
        for (command_idx = 0; command_idx < (at_ctx.commands_count); command_idx++) {
            // Update parsing mode.
            parser_mode = (((at_ctx.commands_list[command_idx])->parameters) == NULL) ? PARSER_MODE_STRICT : PARSER_MODE_HEADER;
            // Check type.
            if (PARSER_compare(&(at_ctx.parser), parser_mode, (char_t*) ((at_ctx.commands_list[command_idx])->syntax)) == PARSER_SUCCESS) {
                // Execute callback.
                if ((at_ctx.commands_list[command_idx])->callback != NULL) {
                    status = (at_ctx.commands_list[command_idx])->callback();
                }
                else {
                    status = AT_SUCCESS;
                }
                goto end;
            }
        }
    }
end:
    if (status != AT_SUCCESS) {
        _AT_print_error(status);
    }
    else {
#ifndef EMBEDDED_UTILS_AT_FORCE_OK
        if (at_ctx.flags.reply_sent == 0) {
#endif
        _AT_print_ok();
#ifndef EMBEDDED_UTILS_AT_FORCE_OK
        }
#endif
    }
    _AT_reset_parser();
    // Enable interrupt.
    at_ctx.flags.process_pending = 0;
    at_ctx.flags.irq_enable = 1;
errors:
    // Enable receiver.
    TERMINAL_enable_rx(at_ctx.terminal_instance);
    return status;
}

/*******************************************************************/
AT_status_t AT_register_command(const AT_command_t* command) {
    // Local variables.
    AT_status_t status = AT_SUCCESS;
    uint8_t idx = 0;
    uint8_t register_success = 0;
    // Check parameter.
    if (command == NULL) {
        status = AT_ERROR_NULL_PARAMETER;
        goto errors;
    }
    // Search free index in commands list.
    for (idx = 0; idx < EMBEDDED_UTILS_AT_COMMANDS_LIST_SIZE; idx++) {
        // Check if index is free.
        if (at_ctx.commands_list[idx] == NULL) {
            // Register pointer.
            at_ctx.commands_list[idx] = command;
            at_ctx.commands_count++;
            // Exit loop.
            register_success = 1;
            break;
        }
    }
    if (register_success == 0) {
        status = AT_ERROR_COMMANDS_LIST_FULL;
        goto errors;
    }
errors:
    return status;
}

/*******************************************************************/
AT_status_t AT_unregister_command(const AT_command_t* command) {
    // Local variables.
    AT_status_t status = AT_SUCCESS;
    uint8_t idx = 0;
    uint8_t unregister_success = 0;
    // Check parameter.
    if (command == NULL) {
        status = AT_ERROR_NULL_PARAMETER;
        goto errors;
    }
    // Search pointer in commands list.
    for (idx = 0; idx < EMBEDDED_UTILS_AT_COMMANDS_LIST_SIZE; idx++) {
        // Check pointer
        if (at_ctx.commands_list[idx] == command) {
            // Free index.
            at_ctx.commands_list[idx] = NULL;
            if (at_ctx.commands_count > 0) {
                at_ctx.commands_count--;
            }
            // Exit loop.
            unregister_success = 1;
            break;
        }
    }
    if (unregister_success == 0) {
        status = AT_ERROR_COMMANDS_LIST_NOT_FOUND;
        goto errors;
    }
errors:
    return status;
}

/*******************************************************************/
void AT_reply_add_string(char_t* str) {
    // Add string.
    TERMINAL_tx_buffer_add_string(at_ctx.terminal_instance, str);
}

/*******************************************************************/
void AT_reply_add_integer(int32_t value, STRING_format_t format, uint8_t print_prefix) {
    // Add integer.
    TERMINAL_tx_buffer_add_integer(at_ctx.terminal_instance, value, format, print_prefix);
}

/*******************************************************************/
void AT_reply_add_byte_array(uint8_t* data, uint32_t data_size_bytes, uint8_t print_prefix) {
    // Add byte array.
    TERMINAL_tx_buffer_add_byte_array(at_ctx.terminal_instance, data, data_size_bytes, print_prefix);
}

/*******************************************************************/
void AT_send_reply(void) {
    // Add the ending marker.
    TERMINAL_tx_buffer_add_string(at_ctx.terminal_instance, EMBEDDED_UTILS_AT_REPLY_END);
    TERMINAL_send_tx_buffer(at_ctx.terminal_instance);
    TERMINAL_flush_tx_buffer(at_ctx.terminal_instance);
#ifndef EMBEDDED_UTILS_AT_FORCE_OK
    // Update flag.
    at_ctx.flags.reply_sent = 1;
#endif
}

/*** AT compilation flags check ***/

#if (EMBEDDED_UTILS_TERMINAL_INSTANCES_NUMBER == 0)
#error "embedded-utils: Terminal instance missing for AT driver"
#endif

#endif /* EMBEDDED_UTILS_AT_DRIVER_DISABLE */
