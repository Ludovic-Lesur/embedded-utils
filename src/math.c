/*
 * math.c
 *
 *  Created on: 28 aug. 2020
 *      Author: Ludo
 */

#include "math.h"

#ifndef EMBEDDED_UTILS_DISABLE_FLAGS_FILE
#include "embedded_utils_flags.h"
#endif
#include "types.h"

#ifndef EMBEDDED_UTILS_MATH_DRIVER_DISABLE

/*** MATH local macros ***/

#define MATH_MEDIAN_FILTER_SIZE_MAX     0xFF

#ifdef EMBEDDED_UTILS_MATH_ATAN2
#define MATH_ARCTAN_FIXED_90_START      11500

#define MATH_ARCTAN_FIXED_89_START      4000

#define MATH_ARCTAN_LUT_1_START         1001
#define MATH_ARCTAN_LUT_1_STEP_FACTOR   1

#define MATH_ARCTAN_LUT_2_START         201
#define MATH_ARCTAN_LUT_2_STEP_FACTOR   10

#define MATH_ARCTAN_LUT_3_START         1
#define MATH_ARCTAN_LUT_3_STEP_FACTOR   100

#define MATH_ARCTAN_RATIO_FACTOR        100
#endif

/*** MATH global variables ***/

const uint32_t MATH_POWER_10[MATH_U32_SIZE_DECIMAL_DIGITS] = { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000 };

#ifdef EMBEDDED_UTILS_MATH_COS_TABLE
const int16_t MATH_COS_TABLE[MATH_TRIGONOMETRIC_TABLE_SIZE] = {
    1000, 1000, 999, 999, 998, 996, 995, 993, 990, 988,
    985, 982, 978, 974, 970, 966, 961, 956, 951, 946,
    940, 934, 927, 921, 914, 906, 899, 891, 883, 875,
    866, 857, 848, 839, 829, 819, 809, 799, 788, 777,
    766, 755, 743, 731, 719, 707, 695, 682, 669, 656,
    643, 629, 616, 602, 588, 574, 559, 545, 530, 515,
    500, 485, 469, 454, 438, 423, 407, 391, 375, 358,
    342, 326, 309, 292, 276, 259, 242, 225, 208, 191,
    174, 156, 139, 122, 105, 87, 70, 52, 35, 17,
    0, -17, -35, -52, -70, -87, -105, -122, -139, -156,
    -174, -191, -208, -225, -242, -259, -276, -292, -309, -326,
    -342, -358, -375, -391, -407, -423, -438, -454, -469, -485,
    -500, -515, -530, -545, -559, -574, -588, -602, -616, -629,
    -643, -656, -669, -682, -695, -707, -719, -731, -743, -755,
    -766, -777, -788, -799, -809, -819, -829, -839, -848, -857,
    -866, -875, -883, -891, -899, -906, -914, -921, -927, -934,
    -940, -946, -951, -956, -961, -966, -970, -974, -978, -982,
    -985, -988, -990, -993, -995, -996, -998, -999, -999, -1000,
    -1000, -1000, -999, -999, -998, -996, -995, -993, -990, -988,
    -985, -982, -978, -974, -970, -966, -961, -956, -951, -946,
    -940, -934, -927, -921, -914, -906, -899, -891, -883, -875,
    -866, -857, -848, -839, -829, -819, -809, -799, -788, -777,
    -766, -755, -743, -731, -719, -707, -695, -682, -669, -656,
    -643, -629, -616, -602, -588, -574, -559, -545, -530, -515,
    -500, -485, -469, -454, -438, -423, -407, -391, -375, -358,
    -342, -326, -309, -292, -276, -259, -242, -225, -208, -191,
    -174, -156, -139, -122, -105, -87, -70, -52, -35, -17,
    0, 17, 35, 52, 70, 87, 105, 122, 139, 156,
    174, 191, 208, 225, 242, 259, 276, 292, 309, 326,
    342, 358, 375, 391, 407, 423, 438, 454, 469, 485,
    500, 515, 530, 545, 559, 574, 588, 602, 616, 629,
    643, 656, 669, 682, 695, 707, 719, 731, 743, 755,
    766, 777, 788, 799, 809, 819, 829, 839, 848, 857,
    866, 875, 883, 891, 899, 906, 914, 921, 927, 934,
    940, 946, 951, 956, 961, 966, 970, 974, 978, 982,
    985, 988, 990, 993, 995, 996, 998, 999, 999, 1000
};
#endif

#ifdef EMBEDDED_UTILS_MATH_SIN_TABLE
const int16_t MATH_SIN_TABLE[MATH_TRIGONOMETRIC_TABLE_SIZE] = {
    0, 17, 35, 52, 70, 87, 105, 122, 139, 156,
    174, 191, 208, 225, 242, 259, 276, 292, 309, 326,
    342, 358, 375, 391, 407, 423, 438, 454, 469, 485,
    500, 515, 530, 545, 559, 574, 588, 602, 616, 629,
    643, 656, 669, 682, 695, 707, 719, 731, 743, 755,
    766, 777, 788, 799, 809, 819, 829, 839, 848, 857,
    866, 875, 883, 891, 899, 906, 914, 921, 927, 934,
    940, 946, 951, 956, 961, 966, 970, 974, 978, 982,
    985, 988, 990, 993, 995, 996, 998, 999, 999, 1000,
    1000, 1000, 999, 999, 998, 996, 995, 993, 990, 988,
    985, 982, 978, 974, 970, 966, 961, 956, 951, 946,
    940, 934, 927, 921, 914, 906, 899, 891, 883, 875,
    866, 857, 848, 839, 829, 819, 809, 799, 788, 777,
    766, 755, 743, 731, 719, 707, 695, 682, 669, 656,
    643, 629, 616, 602, 588, 574, 559, 545, 530, 515,
    500, 485, 469, 454, 438, 423, 407, 391, 375, 358,
    342, 326, 309, 292, 276, 259, 242, 225, 208, 191,
    174, 156, 139, 122, 105, 87, 70, 52, 35, 17,
    0, -17, -35, -52, -70, -87, -105, -122, -139, -156,
    -174, -191, -208, -225, -242, -259, -276, -292, -309, -326,
    -342, -358, -375, -391, -407, -423, -438, -454, -469, -485,
    -500, -515, -530, -545, -559, -574, -588, -602, -616, -629,
    -643, -656, -669, -682, -695, -707, -719, -731, -743, -755,
    -766, -777, -788, -799, -809, -819, -829, -839, -848, -857,
    -866, -875, -883, -891, -899, -906, -914, -921, -927, -934,
    -940, -946, -951, -956, -961, -966, -970, -974, -978, -982,
    -985, -988, -990, -993, -995, -996, -998, -999, -999, -1000,
    -1000, -1000, -999, -999, -998, -996, -995, -993, -990, -988,
    -985, -982, -978, -974, -970, -966, -961, -956, -951, -946,
    -940, -934, -927, -921, -914, -906, -899, -891, -883, -875,
    -866, -857, -848, -839, -829, -819, -809, -799, -788, -777,
    -766, -755, -743, -731, -719, -707, -695, -682, -669, -656,
    -643, -629, -616, -602, -588, -574, -559, -545, -530, -515,
    -500, -485, -469, -454, -438, -423, -407, -391, -375, -358,
    -342, -326, -309, -292, -276, -259, -242, -225, -208, -191,
    -174, -156, -139, -122, -105, -87, -70, -52, -35, -17
};
#endif

#ifdef EMBEDDED_UTILS_MATH_ATAN2
static const uint8_t MATH_ARCTAN_LUT_1[30] = {
    85, 85, 86, 86, 86, 86, 87, 87, 87, 87,
    87, 87, 88, 88, 88, 88, 88, 88, 88, 88,
    88, 88, 88, 88, 88, 88, 88, 88, 89, 89
};
static const uint8_t MATH_ARCTAN_LUT_2[80] = {
    65, 66, 67, 67, 68, 69, 70, 70, 71, 72,
    72, 73, 73, 74, 74, 74, 75, 75, 76, 76,
    76, 77, 77, 77, 77, 78, 78, 78, 78, 79,
    79, 79, 79, 80, 80, 80, 80, 80, 80, 81,
    81, 81, 81, 81, 81, 81, 82, 82, 82, 82,
    82, 82, 82, 82, 82, 83, 83, 83, 83, 83,
    83, 83, 83, 83, 83, 83, 83, 84, 84, 84,
    84, 84, 84, 84, 84, 84, 84, 84, 84, 84
};
static const uint8_t MATH_ARCTAN_LUT_3[200] = {
    1, 1, 2, 2, 3, 3, 4, 5, 5, 6,
    6, 7, 7, 8, 9, 9, 10, 10, 11, 11,
    12, 12, 13, 13, 14, 15, 15, 16, 16, 17,
    17, 18, 18, 19, 19, 20, 20, 21, 21, 22,
    22, 23, 23, 24, 24, 25, 25, 26, 26, 27,
    27, 27, 28, 28, 29, 29, 30, 30, 31, 31,
    31, 32, 32, 33, 33, 33, 34, 34, 35, 35,
    35, 36, 36, 37, 37, 37, 38, 38, 38, 39,
    39, 39, 40, 40, 40, 41, 41, 41, 42, 42,
    42, 43, 43, 43, 44, 44, 44, 44, 45, 45,
    45, 46, 46, 46, 46, 47, 47, 47, 47, 48,
    48, 48, 48, 49, 49, 49, 49, 50, 50, 50,
    50, 51, 51, 51, 51, 52, 52, 52, 52, 52,
    53, 53, 53, 53, 53, 54, 54, 54, 54, 54,
    55, 55, 55, 55, 55, 56, 56, 56, 56, 56,
    56, 57, 57, 57, 57, 57, 58, 58, 58, 58,
    58, 58, 58, 59, 59, 59, 59, 59, 59, 60,
    60, 60, 60, 60, 60, 60, 61, 61, 61, 61,
    61, 61, 61, 61, 62, 62, 62, 62, 62, 62,
    62, 62, 63, 63, 63, 63, 63, 63, 63, 63
};
#endif

/*** MATH local functions ***/

/*******************************************************************/
#define _MATH_min(data, data_size, type, init_value) { \
    /* Local variables */ \
    type min = init_value; \
    uint8_t idx = 0; \
    for (idx = 0; idx < data_size; idx++) { \
        if (data[idx] < min) { \
            min = data[idx]; \
        } \
    } \
    (*result) = min; \
}

/*******************************************************************/
#define _MATH_max(data, data_size, type, init_value) { \
    /* Local variables */ \
    type max = init_value; \
    uint8_t idx = 0; \
    for (idx = 0; idx < data_size; idx++) { \
        if (data[idx] > max) { \
            max = data[idx]; \
        } \
    } \
    (*result) = max; \
}

/*******************************************************************/
#define _MATH_average(data, data_size, average_type, result_type) { \
    /* Local variables */ \
    average_type average = 0; \
    uint8_t idx = 0; \
    /* Compute rolling mean */ \
    for (idx = 0; idx < data_size; idx++) { \
        MATH_rolling_mean(average, idx, data[idx], average_type); \
    } \
    (*result) = (result_type) average; \
}

/*******************************************************************/
#define _MATH_median_filter(data, median_size, average_size) { \
    /* Local variables */ \
    uint8_t buffer_sorted = 0; \
    uint8_t idx1 = 0; \
    uint8_t idx2 = 0; \
    /* Copy input buffer into local buffer */ \
    for (idx1 = 0; idx1 < median_size; idx1++) { \
        local_buf[idx1] = data[idx1]; \
    } \
    /* Sort buffer in ascending order. */ \
    for (idx1 = 0; idx1 < median_size; ++idx1) { \
        buffer_sorted = 1; \
        for (idx2 = 1; idx2 < (median_size - idx1); ++idx2) { \
            if (local_buf[idx2 - 1] > local_buf[idx2]) { \
                temp = local_buf[idx2 - 1]; \
                local_buf[idx2 - 1] = local_buf[idx2]; \
                local_buf[idx2] = temp; \
                buffer_sorted = 0; \
            } \
        } \
        if (buffer_sorted != 0) break; \
    } \
    /* Compute start and end indexes for final averaging */ \
    if (average_size > 0) { \
        /* Clamp value */ \
        if (average_size > median_size) { \
            average_size = median_size; \
        } \
        start_idx = (median_size >> 1) - (average_size >> 1); \
        end_idx =   (median_size >> 1) + (average_size >> 1); \
        if (end_idx >= median_size) { \
            end_idx = (median_size - 1); \
        } \
    } \
}

/*******************************************************************/
#define _MATH_check_pointer(ptr) { \
    if (ptr == NULL) { \
        status = MATH_ERROR_NULL_PARAMETER; \
        goto errors; \
    } \
}

#ifdef EMBEDDED_UTILS_MATH_ATAN2
/*******************************************************************/
static MATH_status_t _MATH_arctan_ratio(int32_t x, int32_t y, int32_t* angle_degrees) {
    // Local variables.
    MATH_status_t status = MATH_SUCCESS;
    int32_t result = 0;
    int64_t ratio = 0;
    int64_t ratio_abs = 0;
    uint8_t lut_index = 0;
    // Compute ratio.
    ratio = (((int64_t) MATH_ARCTAN_RATIO_FACTOR) * ((int64_t) y)) / ((int64_t) x);
    MATH_abs(ratio, ratio_abs, int64_t);
    // Check range.
    if (ratio_abs >= MATH_ARCTAN_FIXED_90_START) {
        // Fixed value.
        result = 90;
    }
    else if (ratio_abs >= MATH_ARCTAN_FIXED_89_START) {
        // Fixed value.
        result = 89;
    }
    else if (ratio_abs >= MATH_ARCTAN_LUT_1_START) {
        // Use LUT 1 with step 1.
        lut_index = (uint8_t) (((ratio_abs - MATH_ARCTAN_LUT_1_START) * MATH_ARCTAN_LUT_1_STEP_FACTOR) / (MATH_ARCTAN_RATIO_FACTOR));
        result = (int32_t) MATH_ARCTAN_LUT_1[lut_index];
    }
    else if (ratio_abs >= MATH_ARCTAN_LUT_2_START) {
        // Use LUT 2 with step 0.1.
        lut_index = (uint8_t) (((ratio_abs - MATH_ARCTAN_LUT_2_START) * MATH_ARCTAN_LUT_2_STEP_FACTOR) / (MATH_ARCTAN_RATIO_FACTOR));
        result = (int32_t) MATH_ARCTAN_LUT_2[lut_index];
    }
    else if (ratio_abs >= MATH_ARCTAN_LUT_3_START) {
        // Use LUT 3 with step 0.01.
        lut_index = (uint8_t) (((ratio_abs - MATH_ARCTAN_LUT_3_START) * MATH_ARCTAN_LUT_3_STEP_FACTOR) / (MATH_ARCTAN_RATIO_FACTOR));
        result = (int32_t) MATH_ARCTAN_LUT_3[lut_index];
    }
    else {
        result = 0;
    }
    // Apply sign.
    if (ratio < 0) {
        result *= (-1);
    }
    // Check range.
    if ((result < (-MATH_PI_2_DEGREES)) || (result > MATH_PI_2_DEGREES)) {
        status = MATH_ERROR_ARCTAN_RANGE;
        goto errors;
    }
    (*angle_degrees) = result;
errors:
    return status;
}
#endif

/*** MATH functions ***/

/*******************************************************************/
MATH_status_t MATH_min(int32_t* data, uint8_t data_size, int32_t* result) {
    // Local variables.
    MATH_status_t status = MATH_SUCCESS;
    // Check parameters.
    _MATH_check_pointer(data);
    _MATH_check_pointer(result);
    // Compute minimum value.
    _MATH_min(data, data_size, int32_t, MATH_S32_MAX);
errors:
    return status;
}

/*******************************************************************/
MATH_status_t MATH_max(int32_t* data, uint8_t data_size, int32_t* result) {
    // Local variables.
    MATH_status_t status = MATH_SUCCESS;
    // Check parameters.
    _MATH_check_pointer(data);
    _MATH_check_pointer(result);
    // Compute minimum value.
    _MATH_max(data, data_size, int32_t, MATH_S32_MIN);
errors:
    return status;
}

/*******************************************************************/
MATH_status_t MATH_average(int32_t* data, uint8_t data_size, int32_t* result) {
// Local variables.
    MATH_status_t status = MATH_SUCCESS;
    // Check parameters.
    _MATH_check_pointer(data);
    _MATH_check_pointer(result);
    // Compute average.
#if (EMBEDDED_UTILS_MATH_PRECISION == 2)
    _MATH_average(data, data_size, float64_t, int32_t);
#elif (EMBEDDED_UTILS_MATH_PRECISION == 1)
    _MATH_average(data, data_size, float32_t, int32_t);
#else
    _MATH_average(data, data_size, int32_t, int32_t);
#endif
errors:
    return status;
}

/*******************************************************************/
MATH_status_t MATH_median_filter(int32_t* data, uint8_t median_size, uint8_t average_size, int32_t* result) {
    // Local variables.
    MATH_status_t status = MATH_SUCCESS;
    int32_t local_buf[MATH_MEDIAN_FILTER_SIZE_MAX];
    int32_t temp = 0;
    uint8_t start_idx = 0;
    uint8_t end_idx = 0;
    // Check parameters.
    _MATH_check_pointer(data);
    _MATH_check_pointer(result);
    // Compute median filter.
    _MATH_median_filter(data, median_size, average_size);
    // Compute average or median value.
    if (average_size > 0) {
        status = MATH_average(&(data[start_idx]), (uint8_t) (end_idx - start_idx + 1), result);
    }
    else {
        (*result) = local_buf[(median_size >> 1)];
    }
errors:
    return status;
}

#ifdef EMBEDDED_UTILS_MATH_ATAN2
/*******************************************************************/
MATH_status_t MATH_atan2(int32_t x, int32_t y, int32_t* angle_degrees) {
    // Local variables.
    MATH_status_t status = MATH_SUCCESS;
    int32_t arctan_y_x = 0;
    int32_t result = 0;
    // Check parameters.
    if ((x == 0) && (y == 0)) {
        status = MATH_ERROR_UNDEFINED;
        goto errors;
    }
    _MATH_check_pointer(angle_degrees);
    // Compute inverse tangent.
    if (x != 0) {
        status = _MATH_arctan_ratio(x, y, &arctan_y_x);
        if (status != MATH_SUCCESS) goto errors;
    }
    // Check x sign.
    if (x > 0) {
        // Check y sign.
        result = (y >= 0) ? arctan_y_x : (arctan_y_x + MATH_2_PI_DEGREES);
    }
    else if (x < 0) {
        result = arctan_y_x + MATH_PI_DEGREES;
    }
    else {
        // Check y sign.
        result = (y >= 0) ? MATH_PI_2_DEGREES : MATH_3_PI_4_DEGREES;
    }
    // Apply modulo.
    if (result == MATH_2_PI_DEGREES) {
        result = 0;
    }
    // Check range.
    if ((result < 0) || (result >= MATH_2_PI_DEGREES)) {
        status = MATH_ERROR_ATAN2_RANGE;
        goto errors;
    }
    (*angle_degrees) = result;
errors:
    return status;
}
#endif

/*******************************************************************/
MATH_status_t MATH_two_complement_to_integer(uint32_t value, uint8_t sign_bit_position, int32_t* result) {
    // Local variables.
    MATH_status_t status = MATH_SUCCESS;
    uint8_t bit_idx = 0;
    uint32_t not_value = 0;
    uint32_t absolute_value = 0;
    // Check parameters.
    if (sign_bit_position >= MATH_S32_SIZE_BITS) {
        status = MATH_ERROR_SIGN_BIT;
        goto errors;
    }
    _MATH_check_pointer(result);
    // Check sign bit.
    if ((value & (0b1 << sign_bit_position)) == 0) {
        // Value is positive: nothing to do.
        (*result) = (int32_t) value;
    }
    else {
        // Value is negative.
        for (bit_idx = 0; bit_idx <= sign_bit_position; bit_idx++) {
            if ((value & (0b1 << bit_idx)) == 0) {
                not_value |= (0b1 << bit_idx);
            }
        }
        absolute_value = not_value + 1;
        (*result) = (-1) * ((int32_t) absolute_value);
    }
errors:
    return status;
}

/*******************************************************************/
MATH_status_t MATH_integer_to_signed_magnitude(int32_t value, uint8_t sign_bit_position, uint32_t* result) {
    // Local variables.
    MATH_status_t status = MATH_SUCCESS;
    uint32_t absolute_value = 0;
    uint32_t absolute_mask = (uint32_t) ((0b1 << sign_bit_position) - 1);
    // Check parameters.
    if (sign_bit_position >= MATH_S32_SIZE_BITS) {
        status = MATH_ERROR_SIGN_BIT;
        goto errors;
    }
    _MATH_check_pointer(result);
    // Compute absolute value.
    MATH_abs(value, absolute_value, uint32_t);
    // Check size.
    if (absolute_value > absolute_mask) {
        status = MATH_ERROR_MAGNITUDE_OVERFLOW;
        goto errors;
    }
    (*result) = (absolute_value & absolute_mask);
    if (value < 0) {
        (*result) |= (0b1 << sign_bit_position);
    }
errors:
    return status;
}

#endif /* EMBEDDED_UTILS_MATH_DRIVER_DISABLE */
