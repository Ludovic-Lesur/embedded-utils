/*
 * terminal_hw.h
 *
 *  Created on: 29 sep. 2024
 *      Author: Ludo
 */

#ifndef __TERMINAL_HW_H__
#define __TERMINAL_HW_H__

#ifndef EMBEDDED_UTILS_DISABLE_FLAGS_FILE
#include "embedded_utils_flags.h"
#endif
#include "terminal.h"
#include "types.h"

/*** TERMINAL HW functions ***/

/*!******************************************************************
 * \fn TERMINAL_status_t TERMINAL_HW_init(uint8_t instance, TERMINAL_HW_rx_irq_cb_t rx_irq_callback)
 * \brief Init a terminal hardware interface.
 * \param[in]   instance: Terminal instance to initialize.
 * \param[in]   rx_irq_callback: Function to be called when a byte is received.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
TERMINAL_status_t TERMINAL_HW_init(uint8_t instance, TERMINAL_HW_rx_irq_cb_t rx_irq_callback);

/*!******************************************************************
 * \fn TERMINAL_status_t TERMINAL_HW_de_init(uint8_t instance)
 * \brief Release a terminal hardware interface.
 * \param[in]   instance: Terminal instance to release.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
TERMINAL_status_t TERMINAL_HW_de_init(uint8_t instance);

/*!******************************************************************
 * \fn TERMINAL_status_t TERMINAL_HW_write(uint8_t instance, uint8_t* data, uint32_t data_size_bytes)
 * \brief Write data over terminal interface.
 * \param[in]   instance: Terminal instance to use.
 * \param[in]   data: Byte array to send.
 * \param[in]   data_size_bytes: Number of bytes to send.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
TERMINAL_status_t TERMINAL_HW_write(uint8_t instance, uint8_t* data, uint32_t data_size_bytes);

#endif /* __TERMINAL_HW_H__ */