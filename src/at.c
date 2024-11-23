/*
 * at.c
 *
 *  Created on: 03 nov. 2024
 *      Author: Ludo
 */

#include "at.h"

#include "error.h"
#include "parser.h"
#include "terminal.h"
#include "types.h"

#if ((!defined EMBEDDED_UTILS_AT_DRIVER_DISABLE) && (EMBEDDED_UTILS_AT_INSTANCES_NUMBER > 0))

/*** AT local macros ***/

#define AT_HEADER       "AT"

#define AT_REPLY_OK     "OK"
#define AT_REPLY_ERROR  "ERROR_"
#define AT_REPLY_TAB    "    "
#define AT_REPLY_END    "\r\n"

/*** AT local structures ***/

/*******************************************************************/
typedef union {
    struct {
        uint8_t irq_enable :1;
        uint8_t process :1;
        uint8_t process_pending :1;
        uint8_t echo :1;
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

/*******************************************************************/
typedef struct {
    uint8_t current_instance;
} AT_common_context_t;

/*** AT local functions declaration ***/

static void _AT_rx_irq_callback_0(uint8_t data);
#if (EMBEDDED_UTILS_AT_INSTANCES_NUMBER > 1)
static void _AT_rx_irq_callback_1(uint8_t data);
#endif
#ifdef EMBEDDED_UTILS_AT_INTERNAL_COMMANDS_ENABLE
static AT_status_t _AT_print_commands_list(void);
static AT_status_t _AT_echo_callback(void);
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
        .syntax = "E",
        .parameters = "<enable[bit]>",
        .description = "Enable or disable echo",
        .callback = &_AT_echo_callback,
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

static const TERMINAL_rx_irq_cb_t AT_RX_IRQ_CALLBACK[EMBEDDED_UTILS_AT_INSTANCES_NUMBER] = {
    &_AT_rx_irq_callback_0,
#if (EMBEDDED_UTILS_AT_INSTANCES_NUMBER > 1)
    &_AT_rx_irq_callback_1,
#endif
};

static AT_context_t at_ctx[EMBEDDED_UTILS_AT_INSTANCES_NUMBER];
static AT_common_context_t at_common_ctx;

/*** AT local functions ***/

/*******************************************************************/
#define _AT_check_instance(instance) { \
    if (instance >= EMBEDDED_UTILS_AT_INSTANCES_NUMBER) { \
        status = AT_ERROR_INSTANCE; \
        goto errors; \
    } \
}

/*******************************************************************/
#define _AT_exit_instance(instance) { \
    if (instance >= EMBEDDED_UTILS_AT_INSTANCES_NUMBER) goto errors; \
}

/*******************************************************************/
static void _AT_rx_irq_callback(uint8_t instance, uint8_t data) {
    // Ignore null data.
    if ((data == 0x00) || (at_ctx[instance].flags.irq_enable == 0)) goto errors;
    // Check end marker.
    if ((data == STRING_CHAR_CR) || (data == STRING_CHAR_LF)) {
        // Set flag.
        at_ctx[instance].flags.process = 1;
        // Ask for processing.
        if ((at_ctx[instance].process_callback != NULL) && (at_ctx[instance].flags.process_pending == 0)) {
            // Ensure callback is called only once.
            at_ctx[instance].flags.process_pending = 1;
            at_ctx[instance].process_callback();
        }
    }
    else {
        // Store new byte in buffer.
        at_ctx[instance].rx_buffer[at_ctx[instance].rx_buffer_size] = (char_t) data;
        at_ctx[instance].rx_buffer_size = (at_ctx[instance].rx_buffer_size + 1) % EMBEDDED_UTILS_AT_BUFFER_SIZE;
    }
errors:
    return;
}

/*******************************************************************/
static void _AT_rx_irq_callback_0(uint8_t data) {
    _AT_rx_irq_callback(0, data);
}

#if (EMBEDDED_UTILS_AT_INSTANCES_NUMBER > 1)
/*******************************************************************/
static void _AT_rx_irq_callback_1(uint8_t data) {
    _AT_rx_irq_callback(1, data);
}
#endif

#ifdef EMBEDDED_UTILS_AT_INTERNAL_COMMANDS_ENABLE
/*******************************************************************/
static AT_status_t _AT_print_commands_list(void) {
    // Local variables.
    AT_status_t status = AT_SUCCESS;
    uint8_t idx = 0;
    // List all commands registered in the current instance.
    for (idx = 0; idx < at_ctx[at_common_ctx.current_instance].commands_count; idx++) {
        // Print syntax.
        AT_reply_add_string(at_common_ctx.current_instance, AT_HEADER);
        AT_reply_add_string(at_common_ctx.current_instance, (char_t*) ((at_ctx[at_common_ctx.current_instance].commands_list[idx])->syntax));
        // Print parameters.
        AT_reply_add_string(at_common_ctx.current_instance, (char_t*) ((at_ctx[at_common_ctx.current_instance].commands_list[idx])->parameters));
        AT_send_reply(at_common_ctx.current_instance);
        // Print description.
        AT_reply_add_string(at_common_ctx.current_instance, AT_REPLY_TAB);
        AT_reply_add_string(at_common_ctx.current_instance, (char_t*) ((at_ctx[at_common_ctx.current_instance].commands_list[idx])->description));
        AT_send_reply(at_common_ctx.current_instance);
    }
    return status;
}
#endif

#ifdef EMBEDDED_UTILS_AT_INTERNAL_COMMANDS_ENABLE
/*******************************************************************/
static AT_status_t _AT_echo_callback(void) {
    // Local variables.
    AT_status_t status = AT_SUCCESS;
    PARSER_status_t parser_status = PARSER_SUCCESS;
    int32_t enable = 0;
    // Parse parameter.
    parser_status = PARSER_get_parameter(&(at_ctx[at_common_ctx.current_instance].parser), STRING_FORMAT_BOOLEAN, STRING_CHAR_NULL, &enable);
    PARSER_exit_error(AT_ERROR_BASE_PARSER);
    // Update local flag.
    at_ctx[at_common_ctx.current_instance].flags.echo = enable;
errors:
    return status;
}
#endif

#ifdef EMBEDDED_UTILS_AT_INTERNAL_COMMANDS_ENABLE
/*******************************************************************/
static AT_status_t _AT_print_error_stack(void) {
    // Local variables.
    AT_status_t status = AT_SUCCESS;
    ERROR_code_t error = EMBEDDED_UTILS_ERROR_STACK_SUCCESS_VALUE;
#ifdef EMBEDDED_UTILS_ERROR_STACK_SIGFOX
    // Import Sigfox errors into MCU stack.
    ERROR_import_sigfox_stack();
#endif
    // Unstack all errors.
    AT_reply_add_string(at_common_ctx.current_instance, "[ ");
    do {
        error = ERROR_stack_read();
        if (error != EMBEDDED_UTILS_ERROR_STACK_SUCCESS_VALUE) {
            AT_reply_add_integer(at_common_ctx.current_instance, (int32_t) error, STRING_FORMAT_HEXADECIMAL, 1);
            AT_reply_add_string(at_common_ctx.current_instance, " ");
        }
    }
    while (error != EMBEDDED_UTILS_ERROR_STACK_SUCCESS_VALUE);
    AT_reply_add_string(at_common_ctx.current_instance, "]");
    AT_send_reply(at_common_ctx.current_instance);
    return status;
}
#endif

#ifdef EMBEDDED_UTILS_AT_INTERNAL_COMMANDS_ENABLE
/*******************************************************************/
static AT_status_t _AT_print_software_version(void) {
    // Local variables.
    AT_status_t status = AT_SUCCESS;
    // Print software version.
    AT_reply_add_string(at_common_ctx.current_instance, "sw");
    AT_reply_add_integer(at_common_ctx.current_instance, (int32_t) EMBEDDED_UTILS_AT_SW_VERSION_MAJOR, STRING_FORMAT_DECIMAL, 0);
    AT_reply_add_string(at_common_ctx.current_instance, ".");
    AT_reply_add_integer(at_common_ctx.current_instance, (int32_t) EMBEDDED_UTILS_AT_SW_VERSION_MINOR, STRING_FORMAT_DECIMAL, 0);
    AT_reply_add_string(at_common_ctx.current_instance, ".");
    AT_reply_add_integer(at_common_ctx.current_instance, (int32_t) EMBEDDED_UTILS_AT_SW_VERSION_INDEX, STRING_FORMAT_DECIMAL, 0);
    if (EMBEDDED_UTILS_AT_SW_VERSION_DIRTY_FLAG != 0) {
        AT_reply_add_string(at_common_ctx.current_instance, ".dev");
    }
    AT_reply_add_string(at_common_ctx.current_instance, " (");
    AT_reply_add_integer(at_common_ctx.current_instance, (int32_t) EMBEDDED_UTILS_AT_SW_VERSION_ID, STRING_FORMAT_HEXADECIMAL, 1);
    AT_reply_add_string(at_common_ctx.current_instance, ")");
    AT_send_reply(at_common_ctx.current_instance);
    return status;
}
#endif

/*******************************************************************/
static void _AT_reset_parser(uint8_t instance) {
    // Local variables.
    uint32_t idx = 0;
    // Flush buffers.
    for (idx = 0; idx < EMBEDDED_UTILS_AT_BUFFER_SIZE; idx++) {
        at_ctx[instance].rx_buffer[idx] = STRING_CHAR_NULL;
    }
    at_ctx[instance].rx_buffer_size = 0;
    // Reset parser.
    at_ctx[instance].parser.buffer = (char_t*) (at_ctx[instance].rx_buffer);
    at_ctx[instance].parser.buffer_size = 0;
    at_ctx[instance].parser.separator_index = 0;
    at_ctx[instance].parser.start_index = 0;
}

/*******************************************************************/
static void _AT_print_ok(void) {
    // Reply OK.
    AT_reply_add_string(at_common_ctx.current_instance, AT_REPLY_OK);
    AT_send_reply(at_common_ctx.current_instance);
}

/*******************************************************************/
static void _AT_print_error(ERROR_code_t error_code) {
    // Erase eventual pending reply.
    TERMINAL_flush_buffer(at_ctx[at_common_ctx.current_instance].terminal_instance);
    // Reply error code.
    AT_reply_add_string(at_common_ctx.current_instance, AT_REPLY_ERROR);
    // Pad with zero if needed to have always 16 bits.
    if (error_code < 0x0100) {
        AT_reply_add_integer(at_common_ctx.current_instance, 0, STRING_FORMAT_HEXADECIMAL, 1);
        AT_reply_add_integer(at_common_ctx.current_instance, (int32_t) error_code, STRING_FORMAT_HEXADECIMAL, 0);
    }
    else {
        AT_reply_add_integer(at_common_ctx.current_instance, (int32_t) error_code, STRING_FORMAT_HEXADECIMAL, 1);
    }
    AT_send_reply(at_common_ctx.current_instance);
}

/*** AT functions ***/

/*******************************************************************/
AT_status_t AT_init(uint8_t instance, uint8_t terminal_instance, AT_process_callback_t process_callback, PARSER_context_t** parser_context_ptr) {
    // Local variables.
    AT_status_t status = AT_SUCCESS;
    TERMINAL_status_t terminal_status = TERMINAL_SUCCESS;
    uint8_t idx = 0;
    // Check instance.
    _AT_check_instance(instance);
    // Check parameters.
    if (process_callback == NULL) {
        status = AT_ERROR_NULL_PARAMETER;
        goto errors;
    }
    // Init context.
    at_ctx[instance].process_callback = process_callback;
    at_ctx[instance].terminal_instance = terminal_instance;
    for (idx = 0; idx < EMBEDDED_UTILS_AT_COMMANDS_LIST_SIZE; idx++) {
        at_ctx[instance].commands_list[idx] = NULL;
    }
    at_ctx[instance].commands_count = 0;
    _AT_reset_parser(instance);
    // Update parser pointer.
    (*parser_context_ptr) = &(at_ctx[instance].parser);
    // Open terminal.
    terminal_status = TERMINAL_open(at_ctx[instance].terminal_instance, AT_RX_IRQ_CALLBACK[instance]);
    TERMINAL_exit_error(AT_ERROR_BASE_TERMINAL);
#ifdef EMBEDDED_UTILS_AT_INTERNAL_COMMANDS_ENABLE
    // Register internal commands.
    for (idx = 0; idx < (sizeof(AT_INTERNAL_COMMANDS_LIST) / (sizeof(AT_command_t))); idx++) {
        AT_register_command(instance, &(AT_INTERNAL_COMMANDS_LIST[idx]));
    }
#endif
    // Enable interrupt.
    at_ctx[instance].flags.irq_enable = 1;
errors:
    return status;
}

/*******************************************************************/
AT_status_t AT_de_init(uint8_t instance) {
    // Local variables.
    AT_status_t status = AT_SUCCESS;
    TERMINAL_status_t terminal_status = TERMINAL_SUCCESS;
    // Check instance.
    _AT_check_instance(instance);
    // Close terminal.
    terminal_status = TERMINAL_close(at_ctx[instance].terminal_instance);
    TERMINAL_exit_error(AT_ERROR_BASE_TERMINAL);
    // Disable interrupt.
    at_ctx[instance].flags.irq_enable = 0;
errors:
    return status;
}

/*******************************************************************/
AT_status_t AT_process(uint8_t instance) {
    // Local variables.
    AT_status_t status = AT_ERROR_UNKOWN_COMMAND;
    PARSER_mode_t parser_mode = PARSER_MODE_STRICT;
    uint8_t instance_idx = 0;
    uint8_t command_idx = 0;
    // Check instance.
    _AT_check_instance(instance);
    // Instances loop.
    for (instance_idx = 0; instance_idx < EMBEDDED_UTILS_AT_INSTANCES_NUMBER; instance_idx++) {
        // Update current instance.
        at_common_ctx.current_instance = instance_idx;
        // Check process flag.
        if (at_ctx[instance_idx].flags.process == 0) continue;
        // Disable interrupt.
        at_ctx[instance].flags.irq_enable = 0;
        // Clear flag.
        at_ctx[instance_idx].flags.process = 0;
        // Update parser.
        at_ctx[instance].parser.buffer_size = at_ctx[instance].rx_buffer_size;
        // Perform echo if enabled.
        if (at_ctx[at_common_ctx.current_instance].flags.echo != 0) {
            AT_reply_add_string(at_common_ctx.current_instance, at_ctx[instance].rx_buffer);
            AT_send_reply(at_common_ctx.current_instance);
        }
        // Check header.
        if (PARSER_compare(&(at_ctx[instance_idx].parser), PARSER_MODE_HEADER, AT_HEADER) == PARSER_SUCCESS) {
            // Loop on available commands.
            for (command_idx = 0; command_idx < (at_ctx[instance_idx].commands_count); command_idx++) {
                // Update parsing mode.
                parser_mode = (((at_ctx[instance_idx].commands_list[command_idx])->parameters) == NULL) ? PARSER_MODE_STRICT : PARSER_MODE_HEADER;
                // Check type.
                if (PARSER_compare(&(at_ctx[instance_idx].parser), parser_mode, (char_t*) ((at_ctx[instance_idx].commands_list[command_idx])->syntax)) == PARSER_SUCCESS) {
                    // Execute callback.
                    if ((at_ctx[instance_idx].commands_list[command_idx])->callback != NULL) {
                        status = (at_ctx[instance_idx].commands_list[command_idx])->callback();
                    }
                    else {
                        status = AT_SUCCESS;
                    }
                    goto end;
                }
            }
        }
    }
end:
    if (status != AT_SUCCESS) {
        _AT_print_error((ERROR_code_t) status);
    }
    else {
        _AT_print_ok();
    }
errors:
    _AT_reset_parser(instance);
    // Enable interrupt.
    at_ctx[instance].flags.process_pending = 0;
    at_ctx[instance].flags.irq_enable = 1;
    return status;
}

/*******************************************************************/
AT_status_t AT_register_command(uint8_t instance, const AT_command_t* command) {
    // Local variables.
    AT_status_t status = AT_SUCCESS;
    uint8_t idx = 0;
    uint8_t register_success = 0;
    // Check instance.
    _AT_check_instance(instance);
    // Check parameter.
    if (command == NULL) {
        status = AT_ERROR_NULL_PARAMETER;
        goto errors;
    }
    // Search free index in commands list.
    for (idx = 0; idx < EMBEDDED_UTILS_AT_COMMANDS_LIST_SIZE; idx++) {
        // Check if index is free.
        if (at_ctx[instance].commands_list[idx] == NULL) {
            // Register pointer.
            at_ctx[instance].commands_list[idx] = command;
            at_ctx[instance].commands_count++;
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
AT_status_t AT_unregister_command(uint8_t instance, const AT_command_t* command) {
    // Local variables.
    AT_status_t status = AT_SUCCESS;
    uint8_t idx = 0;
    uint8_t unregister_success = 0;
    // Check instance.
    _AT_check_instance(instance);
    // Check parameter.
    if (command == NULL) {
        status = AT_ERROR_NULL_PARAMETER;
        goto errors;
    }
    // Search pointer in commands list.
    for (idx = 0; idx < EMBEDDED_UTILS_AT_COMMANDS_LIST_SIZE; idx++) {
        // Check pointer
        if (at_ctx[instance].commands_list[idx] == command) {
            // Free index.
            at_ctx[instance].commands_list[idx] = NULL;
            if (at_ctx[instance].commands_count > 0) {
                at_ctx[instance].commands_count--;
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
void AT_reply_add_string(uint8_t instance, char_t* str) {
    // Check instance.
    _AT_exit_instance(instance);
    // Add string.
    TERMINAL_buffer_add_string(at_ctx[instance].terminal_instance, str);
errors:
    return;
}

/*******************************************************************/
void AT_reply_add_integer(uint8_t instance, int32_t value, STRING_format_t format, uint8_t print_prefix) {
    // Check instance.
    _AT_exit_instance(instance);
    // Add integer.
    TERMINAL_buffer_add_integer(at_ctx[instance].terminal_instance, value, format, print_prefix);
errors:
    return;
}

/*******************************************************************/
void AT_reply_add_byte_array(uint8_t instance, uint8_t* data, uint32_t data_size_bytes, uint8_t print_prefix) {
    // Check instance.
    _AT_exit_instance(instance);
    // Add byte array.
    TERMINAL_buffer_add_byte_array(at_ctx[instance].terminal_instance, data, data_size_bytes, print_prefix);
errors:
    return;
}

/*******************************************************************/
void AT_send_reply(uint8_t instance) {
    // Check instance.
    _AT_exit_instance(instance);
    // Add the ending marker.
    TERMINAL_buffer_add_string(at_ctx[instance].terminal_instance, AT_REPLY_END);
    TERMINAL_print_buffer(at_ctx[instance].terminal_instance);
    TERMINAL_flush_buffer(at_ctx[instance].terminal_instance);
errors:
    return;
}

/*** AT compilation flags check ***/

#if (EMBEDDED_UTILS_AT_INSTANCES_NUMBER > 2)
#error "Error: AT driver only supports 0, 1 or 2 instances"
#endif
#if (EMBEDDED_UTILS_AT_INSTANCES_NUMBER > EMBEDDED_UTILS_TERMINAL_INSTANCES_NUMBER)
#error "Error: Terminal instance(s) missing for AT driver"
#endif

#endif /* EMBEDDED_UTILS_AT_DRIVER_DISABLE */
