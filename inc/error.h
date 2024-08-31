/*
 * error.h
 *
 *  Created on: 12 mar. 2022
 *      Author: Ludo
 */

#ifndef __ERROR_H__
#define __ERROR_H__

#ifndef EMBEDDED_UTILS_DISABLE_FLAGS_FILE
#include "embedded_utils_flags.h"
#endif
#include "types.h"

/*!******************************************************************
 * \typedef ERROR_code_t
 * \brief Board error code type.
 *******************************************************************/
typedef uint16_t ERROR_code_t;

/*** ERROR functions ***/

/*!******************************************************************
 * \fn void ERROR_stack_init(void)
 * \brief Init error stack.
 * \param[in]  	none
 * \param[out] 	none
 * \retval		none
 *******************************************************************/
void ERROR_stack_init(void);

/*!******************************************************************
 * \fn void ERROR_stack_add(ERROR_code_t code)
 * \brief Add error to stack.
 * \param[in]  	code: Error to stack.
 * \param[out] 	none
 * \retval		none
 *******************************************************************/
void ERROR_stack_add(ERROR_code_t code);

/*!******************************************************************
 * \fn ERROR_code_t ERROR_stack_read(void)
 * \brief Read error stack.
 * \param[in]  	none
 * \param[out] 	none
 * \retval		Last error code stored.
 *******************************************************************/
ERROR_code_t ERROR_stack_read(void);

/*!******************************************************************
 * \fn uint8_t ERROR_stack_is_empty(void)
 * \brief Check if error stack is empty.
 * \param[in]  	none
 * \param[out] 	none
 * \retval		1 if the error stack is empty, 0 otherwise.
 *******************************************************************/
uint8_t ERROR_stack_is_empty(void);

#ifdef EMBEDDED_UTILS_ERROR_STACK_SIGFOX
/*!******************************************************************
 * \fn void ERROR_import_sigfox_stack(void)
 * \brief Import the Sigfox EP library error stack in the MCU stack.
 * \param[in]  	none
 * \param[out] 	none
 * \retval		none
 *******************************************************************/
void ERROR_import_sigfox_stack(void);
#endif

/*******************************************************************/
#define ERROR_check_exit(ll_status, ll_success, ll_base)  { if (ll_status != ll_success) { status = ((ll_base << 0) + ll_status); goto errors; } }

/*******************************************************************/
#define ERROR_check_stack(ll_status, ll_success, ll_base) { if (ll_status != ll_success) { ERROR_stack_add((ll_base << 0) + ll_status); } }

/*******************************************************************/
#define ERROR_check_stack_exit(ll_status, ll_success, ll_base, code) { if (ll_status != ll_success) { ERROR_stack_add((ll_base << 0) + ll_status); status = code; goto errors; } }

#endif /* __ERROR_H__ */
