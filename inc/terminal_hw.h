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

#if (!(defined EMBEDDED_UTILS_TERMINAL_DRIVER_DISABLE) && (EMBEDDED_UTILS_TERMINAL_INSTANCES_NUMBER > 0))

/*** TERMINAL HW functions ***/

/*!******************************************************************
 * \fn TERMINAL_status_t TERMINAL_HW_init(uint8_t instance, TERMINAL_rx_irq_cb_t rx_irq_callback)
 * \brief Init a terminal hardware interface.
 * \param[in]   instance: Terminal instance to initialize.
 * \param[in]   baud_rate: Terminal baud rate.
 * \param[in]   rx_irq_callback: Function to be called when a byte is received.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
TERMINAL_status_t TERMINAL_HW_init(uint8_t instance, uint32_t baud_rate, TERMINAL_rx_irq_cb_t rx_irq_callback);

/*!******************************************************************
 * \fn TERMINAL_status_t TERMINAL_HW_de_init(uint8_t instance)
 * \brief Release a terminal hardware interface.
 * \param[in]   instance: Terminal instance to release.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
TERMINAL_status_t TERMINAL_HW_de_init(uint8_t instance);

/*!******************************************************************
 * \fn TERMINAL_status_t TERMINAL_HW_enable_rx(uint8_t instance)
 * \brief Enable terminal reception.
 * \param[in]   instance: Terminal instance to use.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
TERMINAL_status_t TERMINAL_HW_enable_rx(uint8_t instance);

/*!******************************************************************
 * \fn TERMINAL_status_t TERMINAL_HW_disable_rx(uint8_t instance)
 * \brief Disable terminal reception.
 * \param[in]   instance: Terminal instance to use.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
TERMINAL_status_t TERMINAL_HW_disable_rx(uint8_t instance);

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

#ifdef EMBEDDED_UTILS_TERMINAL_MODE_BUS
/*!******************************************************************
 * \fn void TERMINAL_HW_set_destination_address(uint8_t instance, uint8_t destination_address)
 * \brief Set destination address.
 * \param[in]   instance: Terminal instance to use.
 * \param[in]   destination_address: Address to use when printing the buffer.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
TERMINAL_status_t TERMINAL_HW_set_destination_address(uint8_t instance, uint8_t destination_address);
#endif

#endif /* EMBEDDED_UTILS_TERMINAL_DRIVER_DISABLE */

#endif /* __TERMINAL_HW_H__ */
