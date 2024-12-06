/*
 * math.h
 *
 *  Created on: 28 aug. 2020
 *      Author: Ludo
 */

#ifndef __MATH_H__
#define __MATH_H__

#ifndef EMBEDDED_UTILS_DISABLE_FLAGS_FILE
#include "embedded_utils_flags.h"
#endif
#include "types.h"

/*** MATH macros ***/

#define MATH_BOOLEAN_MIN                        0
#define MATH_BOOLEAN_MAX                        1
#define MATH_BOOLEAN_MASK                       0x01
#define MATH_BOOLEAN_SIZE_BITS                  1
#define MATH_BOOLEAN_SIZE_DECIMAL_DIGITS        1
#define MATH_BOOLEAN_SIZE_HEXADECIMAL_DIGITS    1

#define MATH_U8_MIN                             0
#define MATH_U8_MAX                             255
#define MATH_U8_MASK                            0xFF
#define MATH_U8_SIZE_BITS                       8
#define MATH_U8_SIZE_DECIMAL_DIGITS             3
#define MATH_U8_SIZE_HEXADECIMAL_DIGITS         2

#define MATH_U16_MIN                            0
#define MATH_U16_MAX                            65535
#define MATH_U16_MASK                           0xFFFF
#define MATH_U16_SIZE_BITS                      16
#define MATH_U16_SIZE_BYTES                     2
#define MATH_U16_SIZE_DECIMAL_DIGITS            5
#define MATH_U16_SIZE_HEXADECIMAL_DIGITS        4

#define MATH_U32_MIN                            0
#define MATH_U32_MAX                            4294967295
#define MATH_U32_MASK                           0xFFFFFFFF
#define MATH_U32_SIZE_BITS                      32
#define MATH_U32_SIZE_BYTES                     4
#define MATH_U32_SIZE_DECIMAL_DIGITS            10
#define MATH_U32_SIZE_HEXADECIMAL_DIGITS        8

#define MATH_S8_MIN                             (-128)
#define MATH_S8_MAX                             127
#define MATH_S8_MASK                            0xFF
#define MATH_S8_SIZE_BITS                       8
#define MATH_S8_SIZE_DECIMAL_DIGITS             3
#define MATH_S8_SIZE_HEXADECIMAL_DIGITS         2

#define MATH_S16_MIN                            (-32768)
#define MATH_S16_MAX                            32767
#define MATH_S16_MASK                           0xFFFF
#define MATH_S16_SIZE_BITS                      16
#define MATH_S16_SIZE_BYTES                     2
#define MATH_S16_SIZE_DECIMAL_DIGITS            5
#define MATH_S16_SIZE_HEXADECIMAL_DIGITS        4

#define MATH_S32_MIN                            (-2147483648)
#define MATH_S32_MAX                            2147483647
#define MATH_S32_MASK                           0xFFFFFFFF
#define MATH_S32_SIZE_BITS                      32
#define MATH_S32_SIZE_BYTES                     4
#define MATH_S32_SIZE_DECIMAL_DIGITS            10
#define MATH_S32_SIZE_HEXADECIMAL_DIGITS        8

#define MATH_BOOLEAN_DIGIT_VALUE_MAX            1
#define MATH_DECIMAL_DIGIT_VALUE_MAX            9
#define MATH_HEXADECIMAL_DIGIT_VALUE_MAX        15

#define MATH_PERCENT_MIN                        0
#define MATH_PERCENT_MAX                        100

#define MATH_PERMILLE_MIN                       0
#define MATH_PERMILLE_MAX                       1000

#if (EMBEDDED_UTILS_MATH_PRECISION > 0)
#define MATH_PI_RADIAN                          3.141592654
#endif
#define MATH_2_PI_DEGREES                       360
#define MATH_3_PI_4_DEGREES                     270
#define MATH_PI_DEGREES                         180
#define MATH_PI_2_DEGREES                       90
#define MATH_PI_4_DEGREES                       45

#define MATH_TRIGONOMETRIC_TABLE_SIZE           MATH_2_PI_DEGREES

/*** MATH global variables ***/

extern const uint32_t MATH_POWER_10[MATH_U32_SIZE_DECIMAL_DIGITS];
#ifdef EMBEDDED_UTILS_MATH_COS_TABLE
extern const int16_t MATH_COS_TABLE[MATH_TRIGONOMETRIC_TABLE_SIZE];
#endif
#ifdef EMBEDDED_UTILS_MATH_SIN_TABLE
extern const int16_t MATH_SIN_TABLE[MATH_TRIGONOMETRIC_TABLE_SIZE];
#endif

/*** MATH structures ***/

/*!******************************************************************
 * \enum MATH_status_t
 * \brief MATH driver error codes.
 *******************************************************************/
typedef enum {
    // Driver errors.
    MATH_SUCCESS = 0,
    MATH_ERROR_NULL_PARAMETER,
    MATH_ERROR_OVERFLOW,
    MATH_ERROR_UNDEFINED,
    MATH_ERROR_ARCTAN_RANGE,
    MATH_ERROR_ATAN2_RANGE,
    MATH_ERROR_SIGN_BIT,
    MATH_ERROR_MAGNITUDE_OVERFLOW,
    // Last base value.
    MATH_ERROR_BASE_LAST = 0x0100
} MATH_status_t;

#ifndef EMBEDDED_UTILS_MATH_DRIVER_DISABLE

/*** MATH functions ***/

/*!******************************************************************
 * \fn MATH_status_t MATH_min(int32_t* data, uint8_t data_size, int32_t* result)
 * \brief Returns the minimum value of a 32-bits data array.
 * \param[in]   data: Input array.
 * \param[in]   data_size: Number of elements in the array.
 * \param[out]  result: Pointer to the result.
 * \retval      Function execution status.
 *******************************************************************/
MATH_status_t MATH_min(int32_t* data, uint8_t data_size, int32_t* result);

/*!******************************************************************
 * \fn MATH_status_t MATH_max(int32_t* data, uint8_t data_size, int32_t* result)
 * \brief Returns the maximum value of a 32-bits data array.
 * \param[in]   data: Input array.
 * \param[in]   data_size: Number of elements in the array.
 * \param[out]  result: Pointer to the result.
 * \retval      Function execution status.
 *******************************************************************/
MATH_status_t MATH_max(int32_t* data, uint8_t data_size, int32_t* result);

/*!******************************************************************
 * \fn MATH_status_t MATH_average(int32_t* data, uint8_t data_size, int32_t* result)
 * \brief Compute the average value of a 32-bits data array.
 * \param[in]   data: Input array.
 * \param[in]   data_size: Number of elements in the array.
 * \param[out]  result: Pointer to the result.
 * \retval      Function execution status.
 *******************************************************************/
MATH_status_t MATH_average(int32_t* data, uint8_t data_size, int32_t* result);

/*!******************************************************************
 * \fn MATH_status_t MATH_median_filter(int32_t* data, uint8_t median_size, uint8_t average_size, int32_t* result)
 * \brief Compute an averaged median value of a 32-bits data array.
 * \param[in]   data: Input array.
 * \param[in]   median_size: Number of elements used to compute the median filter.
 * \param[in]   average_size: Number of elements used to compute average of the median values.
 * \param[out]  result: Pointer to the result.
 * \retval      Function execution status.
 *******************************************************************/
MATH_status_t MATH_median_filter(int32_t* data, uint8_t median_size, uint8_t average_size, int32_t* result);

#ifdef EMBEDDED_UTILS_MATH_ATAN2
/*!******************************************************************
 * \fn MATH_status_t MATH_atan2(int32_t x, int32_t y, int32_t* angle_degrees)
 * \brief Compute the atan2 approximated value.
 * \param[in]   x: Abscissa coordinate of the vector.
 * \param[in]   y: Ordinate coordinate of the vector.
 * \param[out]  angle_degrees: Pointer to integer that will contain the angle of the input vector (in degrees between 0 and 359).
 * \retval      Function execution status.
 *******************************************************************/
MATH_status_t MATH_atan2(int32_t x, int32_t y, int32_t* angle_degrees);
#endif

/*!******************************************************************
 * \fn MATH_status_t MATH_two_complement_to_integer(uint32_t value, uint8_t sign_bit_position, int32_t* result)
 * \brief Convert a two complement value with configurable sign bit position to the standard 32-bits two complement value.
 * \param[in]   value: Two complement value to convert.
 * \param[in]   sign_bit_position: Sign bit position expressed as a bit index from 0.
 * \param[out]  result: Pointer to the result.
 * \retval      Function execution status.
 *******************************************************************/
MATH_status_t MATH_two_complement_to_integer(uint32_t value, uint8_t sign_bit_position, int32_t* result);

/*!******************************************************************
 * \fn MATH_status_t MATH_integer_to_signed_magnitude(int32_t value, uint8_t sign_bit_position, uint32_t* result)
 * \brief Convert a standard 32-bits two complement value to the corresponding signed magnitude representation.
 * \param[in]   value: Two complement value to convert.
 * \param[in]   sign_bit_position: Sign bit position in the result, expressed as a bit index from 0.
 * \param[out]  result: Pointer to the result.
 * \retval      Function execution status.
 *******************************************************************/
MATH_status_t MATH_integer_to_signed_magnitude(int32_t value, uint8_t sign_bit_position, uint32_t* result);

/*!******************************************************************
 * \fn MATH_abs(x, y)
 * \brief Compute the absolute value.
 * \param[in]   x: Input argument.
 * \param[in]   y: Output result.
 * \param[out]  none
 * \retval      none
 *******************************************************************/
#define MATH_abs(x, y) { \
    if (x >= 0) { y = x; } \
    else { y = ((-1) * x); } \
}

/*!******************************************************************
 * \fn MATH_rolling_mean(value, number_of_samples, new_sample, value_type)
 * \brief Compute rolling mean.
 * \param[in]   value: Current value to update.
 * \param[in]   number_of_samples: Current number of samples.
 * \param[in]   new_sample: New sample to add.
 * \param[in]   value_type: Type of the value.
 * \param[out]  none
 * \retval      none
 *******************************************************************/
#define MATH_rolling_mean(value, number_of_samples, new_sample, value_type) { \
    /* Compute rolling mean */ \
    value = (((value * ((value_type) number_of_samples)) + ((value_type) new_sample)) / ((value_type) (number_of_samples + 1))); \
    number_of_samples++; \
}

/*******************************************************************/
#define MATH_exit_error(base) { ERROR_check_exit(math_status, MATH_SUCCESS, base) }

/*******************************************************************/
#define MATH_stack_error(base) { ERROR_check_stack(math_status, MATH_SUCCESS, base) }

/*******************************************************************/
#define MATH_stack_exit_error(base, code) { ERROR_check_stack_exit(math_status, MATH_SUCCESS, base, code) }

#endif /* EMBEDDED_UTILS_MATH_DRIVER_DISABLE */

#endif /* __MATH_H__ */
