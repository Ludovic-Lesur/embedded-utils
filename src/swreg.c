/*
 * swreg.c
 *
 *  Created on: 30 nov. 2024
 *      Author: Ludo
 */

#include "swreg.h"

#include "maths.h"
#include "parser.h"
#include "strings.h"
#include "types.h"

#ifndef EMBEDDED_UTILS_SWREG_DRIVER_DISABLE

/*** SWREG local functions ***/

/*******************************************************************/
static uint8_t _SWREG_get_shift(uint32_t field_mask) {
    // Local variables.
    uint8_t offset = 0;
    // Compute shift according to mask.
    for (offset = 0; offset < SWREG_REGISTER_SIZE_BITS; offset++) {
        if (((field_mask >> offset) & 0x01) != 0) break;
    }
    return offset;
}

/*** SWREG functions ***/

/*******************************************************************/
void SWREG_modify_register(uint32_t* reg_value, uint32_t new_reg_value, uint32_t reg_mask) {
    // Local variables.
    uint32_t temp = 0;
    // Read register.
    temp = (*reg_value);
    // Compute new value.
    temp &= ~reg_mask;
    temp |= (new_reg_value & reg_mask);
    // Write register.
    (*reg_value) = temp;
}

/*******************************************************************/
void SWREG_write_field(uint32_t* reg_value, uint32_t* reg_mask, uint32_t field_value, uint32_t field_mask) {
    // Reset bits.
    (*reg_value) &= ~(field_mask);
    // Set field.
    (*reg_value) |= ((field_value << _SWREG_get_shift(field_mask)) & field_mask);
    // Update mask.
    (*reg_mask) |= field_mask;
}

/*******************************************************************/
uint32_t SWREG_read_field(uint32_t reg_value, uint32_t field_mask) {
    // Isolate field.
    return ((reg_value & field_mask) >> _SWREG_get_shift(field_mask));
}

/*******************************************************************/
void SWREG_write_byte_array(uint8_t* data, uint8_t data_size_bytes, uint32_t* reg_list) {
    // Local variables.
    uint8_t idx = 0;
    uint8_t shift = 0;
    uint8_t reg_addr = 0;
    uint32_t reg_value = 0;
    uint32_t reg_mask = 0;
    // Byte loop.
    for (idx = 0; idx < data_size_bytes; idx++) {
        // Compute address, mask and value.
        reg_addr = (idx >> 2);
        shift = ((idx % 4) << 3);
        reg_mask = (0xFF << shift);
        reg_value = (data[idx] << shift);
        // Write register.
        SWREG_modify_register((uint32_t*) &(reg_list[reg_addr]), reg_value, reg_mask);
    }
}

/*******************************************************************/
void SWREG_read_byte_array(uint8_t* data, uint8_t data_size_bytes, uint32_t* reg_list) {
    // Local variables.
    uint8_t idx = 0;
    uint8_t reg_addr = 0;
    uint32_t reg_mask = 0;
    // Byte loop.
    for (idx = 0; idx < data_size_bytes; idx++) {
        // Compute address and mask.
        reg_addr = (idx >> 2);
        reg_mask = (0xFF << ((idx % 4) << 3));
        // Fill data.
        data[idx] = (uint8_t) SWREG_read_field(reg_list[reg_addr], reg_mask);
    }
}

/*******************************************************************/
PARSER_status_t SWREG_parse_register(PARSER_context_t* parser_ctx, char_t separator, uint32_t* reg_value) {
    // Local variables.
    PARSER_status_t status = PARSER_SUCCESS;
    uint8_t reg_bytes[SWREG_REGISTER_SIZE_BYTES];
    uint32_t reg_size_bytes = 0;
    uint8_t idx = 0;
    // Parse register as byte array.
    status = PARSER_get_byte_array(parser_ctx, separator, SWREG_REGISTER_SIZE_BYTES, 0, (uint8_t*) reg_bytes, &reg_size_bytes);
    if (status != PARSER_SUCCESS) goto errors;
    // Convert byte array to 32 bits value.
    (*reg_value) = 0;
    for (idx = 0; idx < reg_size_bytes; idx++) {
        (*reg_value) |= (reg_bytes[idx] << ((reg_size_bytes - 1 - idx) << 3));
    }
errors:
    return status;
}

#endif /* EMBEDDED_UTILS_SWREG_DRIVER_DISABLE */
