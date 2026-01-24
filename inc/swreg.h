/*
 * swreg.h
 *
 *  Created on: 30 nov. 2024
 *      Author: Ludo
 */

#ifndef __SWREG_H__
#define __SWREG_H__

#include "parser.h"
#include "maths.h"
#include "types.h"

#ifndef EMBEDDED_UTILS_SWREG_DRIVER_DISABLE

/*** SWREG macros ***/

#define SWREG_REGISTER_SIZE_BITS    MATH_U32_SIZE_BITS
#define SWREG_REGISTER_SIZE_BYTES   MATH_U32_SIZE_BYTES

#define SWREG_REGISTER_MASK_ALL     MATH_U32_MASK
#define SWREG_REGISTER_MASK_NONE    0x00000000

/*** SWREG functions ***/

/*!******************************************************************
 * \fn void SWREG_modify_register(uint32_t* reg_value, uint32_t new_reg_value, uint32_t reg_mask)
 * \brief Modify a part of a register.
 * \param[in]   reg_value: Pointer to the register value.
 * \param[in]   new_reg_value: New register value to write.
 * \param[in]   reg_mask: Writing operation mask.
 * \param[out]  none
 * \retval      none
 *******************************************************************/
void SWREG_modify_register(uint32_t* reg_value, uint32_t new_reg_value, uint32_t reg_mask);

/*!******************************************************************
 * \fn void SWREG_write_field(uint32_t* reg_value, uint32_t* reg_mask, uint32_t field_value, uint32_t field_mask)
 * \brief Write a field in register.
 * \param[in]   reg_value: Pointer to the register value.
 * \param[in]   field_value: Field value to write.
 * \param[in]   field_mask: Field mask.
 * \param[out]  reg_mask: Pointer to the new register mask.
 * \retval      none
 *******************************************************************/
void SWREG_write_field(uint32_t* reg_value, uint32_t* reg_mask, uint32_t field_value, uint32_t field_mask);

/*!******************************************************************
 * \fn uint32_t SWREG_read_field(uint32_t reg_value, uint32_t field_mask)
 * \brief Read a field in register.
 * \param[in]   reg_value: Register value.
 * \param[in]   field_mask: Field mask.
 * \param[out]  none
 * \retval      Field value.
 *******************************************************************/
uint32_t SWREG_read_field(uint32_t reg_value, uint32_t field_mask);

/*!******************************************************************
 * \fn void SWREG_write_byte_array(uint8_t* data, uint8_t data_size_bytes, uint32_t* reg_list)
 * \brief Write byte array in 32-bits registers.
 * \param[in]   data: Bytes to write.
 * \param[in]   data_size_byte: Number of bytes to write.
 * \param[out]  reg_list: Pointer to the destination registers.
 * \retval      none
 *******************************************************************/
void SWREG_write_byte_array(uint8_t* data, uint8_t data_size_bytes, uint32_t* reg_list);

/*!******************************************************************
 * \fn void SWREG_read_byte_array(uint8_t* data, uint8_t data_size_bytes, uint32_t* reg_list)
 * \brief Read multiple 32-bits registers into a byte array.
 * \param[in]   reg_list: Pointer to the source registers.
 * \param[in]   data_size_byte: Number of bytes to read.
 * \param[out]  data: Pointer to the destination byte array.
 * \retval      Function execution status.
 *******************************************************************/
void SWREG_read_byte_array(uint8_t* data, uint8_t data_size_bytes, uint32_t* reg_list);

/*!******************************************************************
 * \fn PARSER_status_t SWREG_parse_register(PARSER_context_t* parser_ctx, char_t separator, uint32_t* reg_value)
 * \brief Parse a register within a character buffer.
 * \param[in]   parser_ctx: Parser context.
 * \param[in]   separator: Field separator.
 * \param[out]  reg_value: Pointer to integer that will contain the extracted register value.
 * \retval      Function execution status.
 *******************************************************************/
PARSER_status_t SWREG_parse_register(PARSER_context_t* parser_ctx, char_t separator, uint32_t* reg_value);

/*******************************************************************/
#define SWREG_secure_field(field_mask, get_function, convert_function, condition_1, condition_2, default_value, action) { \
    /* Read value */ \
    generic_s32 = get_function(SWREG_read_field(new_reg_value, field_mask)); \
    /* Check value */ \
    if ((generic_s32 condition_1) || (generic_s32 condition_2)) { \
        /* Remove field from mask */ \
        (*reg_mask) &= (~field_mask); \
        /* Set field to default value */ \
        SWREG_write_field(reg_value, &generic_u32, convert_function(default_value), field_mask); \
        action; \
    } \
}

#endif /* EMBEDDED_UTILS_SWREG_DRIVER_DISABLE */

#endif /* __SWREG_H__ */
