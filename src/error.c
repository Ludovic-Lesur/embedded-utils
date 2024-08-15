/*
 * error.c
 *
 *  Created on: 25 sep. 2022
 *      Author: Ludo
 */

#include "error.h"

#ifndef EMBEDDED_UTILS_DISABLE_FLAGS_FILE
#include "embedded_utils_flags.h"
#endif
#include "types.h"
#ifdef EMBEDDED_UTILS_ERROR_STACK_SIGFOX
#include "error_base.h"
#include "sigfox_ep_api.h"
#include "sigfox_error.h"
#endif

/*** ERROR local structures ***/

/*******************************************************************/
typedef struct {
	ERROR_code_t stack[EMBEDDED_UTILS_ERROR_STACK_DEPTH];
	uint8_t stack_idx;
} ERROR_context_t;

/*** ERROR local global variables ***/

static ERROR_context_t error_ctx;

/*** ERROR functions ***/

/*******************************************************************/
void ERROR_stack_init(void) {
	// Reset stack.
	for (error_ctx.stack_idx=0 ; error_ctx.stack_idx<EMBEDDED_UTILS_ERROR_STACK_DEPTH ; error_ctx.stack_idx++) error_ctx.stack[error_ctx.stack_idx] = EMBEDDED_UTILS_ERROR_STACK_SUCCESS_VALUE;
	error_ctx.stack_idx = 0;
}

/*******************************************************************/
void ERROR_stack_add(ERROR_code_t code) {
	// Check index.
	if (error_ctx.stack_idx < EMBEDDED_UTILS_ERROR_STACK_DEPTH) {
		// Add error code.
		error_ctx.stack[error_ctx.stack_idx] = code;
		error_ctx.stack_idx++;
	}
}

/*******************************************************************/
ERROR_code_t ERROR_stack_read(void) {
	// Local variables.
	ERROR_code_t last_error = EMBEDDED_UTILS_ERROR_STACK_SUCCESS_VALUE;
	// Check index.
	if (error_ctx.stack_idx > 0) {
		// Read last error.
		error_ctx.stack_idx--;
		last_error = error_ctx.stack[error_ctx.stack_idx];
		// Remove error.
		error_ctx.stack[error_ctx.stack_idx] = EMBEDDED_UTILS_ERROR_STACK_SUCCESS_VALUE;
	}
	return last_error;
}

/*******************************************************************/
uint8_t ERROR_stack_is_empty(void) {
	// Local variables.
	uint8_t is_empty = (error_ctx.stack_idx == 0) ? 1 : 0;
	// Return flag.
	return is_empty;
}

#ifdef EMBEDDED_UTILS_ERROR_STACK_SIGFOX
/*******************************************************************/
void ERROR_import_sigfox_stack(void) {
	// Local variables.
	SIGFOX_EP_API_status_t sigfox_ep_api_status = SIGFOX_EP_API_SUCCESS;
	ERROR_code_t error_code;
	SIGFOX_ERROR_t sigfox_error;
	uint32_t error_count = 0;
	do {
		// Read error stack.
		sigfox_ep_api_status = SIGFOX_EP_API_unstack_error(&sigfox_error);
		if (sigfox_ep_api_status != SIGFOX_EP_API_SUCCESS) goto errors;
		// Check value.
		if (sigfox_error.code != SIGFOX_EP_API_SUCCESS) {
			// Convert source to base.
			error_code = ((ERROR_BASE_SIGFOX_EP_LIB + (sigfox_error.source * 0x0100)) + sigfox_error.code);
			ERROR_stack_add(error_code);
			// Increment count.
			error_count++;
		}
		// Error detection.
		if (error_count > ERROR_STACK) goto errors;
	}
	while (sigfox_error.code != SIGFOX_EP_API_SUCCESS);
errors:
	return;
}
#endif
