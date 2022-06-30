/*
 * Copyright (c) The Libre Solar Project Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HELPER_H
#define HELPER_H

/** @file
 *
 * @brief
 * General helper functions
 */

#include <stdint.h>
#include <string.h>
#include "mcu.h"

/** @brief Return a value clamped to a given range.
 * 
 * Macro ensures that expressions are evaluated only once. See @ref Z_MAX for
 * macro limitations.
 */
#define Z_CLAMP(val, low, high) ({                                             \
/* random suffix to avoid naming conflict */                   \
__typeof__(val) _value_val_ = (val);                           \
__typeof__(low) _value_low_ = (low);                           \
__typeof__(high) _value_high_ = (high);                        \
(_value_val_ < _value_low_)  ? _value_low_ :                   \
(_value_val_ > _value_high_) ? _value_high_ :                  \
_value_val_;                    \
})


#ifndef MAX
/**
 * @brief Obtain the maximum of two values.
 *
 * @note Arguments are evaluated twice. Use Z_MAX for a GCC-only, single
 * evaluation version
 *
 * @param a First value.
 * @param b Second value.
 *
 * @returns Maximum value of @p a and @p b.
 */
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
/**
 * @brief Obtain the minimum of two values.
 *
 * @note Arguments are evaluated twice. Use Z_MIN for a GCC-only, single
 * evaluation version
 *
 * @param a First value.
 * @param b Second value.
 *
 * @returns Minimum value of @p a and @p b.
 */
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif




#ifndef CLAMP
/**
 * @brief Clamp a value to a given range.
 *
 * @note Arguments are evaluated multiple times. Use Z_CLAMP for a GCC-only,
 * single evaluation version.
 *
 * @param val Value to be clamped.
 * @param low Lowest allowed value (inclusive).
 * @param high Highest allowed value (inclusive).
 *
 * @returns Clamped value.
 */
#define CLAMP(val, low, high) (((val) <= (low)) ? (low) : MIN(val, high))
#endif






// #ifdef __INTELLISENSE__
/*
 * VS Code intellisense can't cope with all the Zephyr macro layers for logging, so provide it
 * with something more simple and make it silent.
 */

#define LOG_DBG(...) printf(__VA_ARGS__)

#define LOG_INF(...) printf_P(PSTR(__VA_ARGS__))

#define LOG_WRN(...) printf_P(PSTR(__VA_ARGS__))

#define LOG_ERR(...) printf_P(PSTR(__VA_ARGS__))

#define LOG_MODULE_REGISTER(...)

// #else

// #include <logging/log.h>

// #endif /* VSCODE_INTELLISENSE_HACK */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Interpolation in a look-up table. Values of a must be monotonically increasing/decreasing
 *
 * @returns interpolated value of array b at position value_a
 */
float interpolate(const float a[], const float b[], size_t size, float value_a);

/**
 * Convert byte to bit-string
 *
 * Attention: Uses static buffer, not thread-safe
 *
 * @returns pointer to bit-string (8 characters + null-byte)
 */
const char *byte2bitstr(uint8_t b);

// uint8_t _crc8_ccitt_update (uint8_t inCrc, uint8_t inData);

// int64_t k_uptime_get();


uint8_t crc8_ccitt(uint8_t val, const void *buf, size_t cnt);

// // // // void k_sleep(uint32_t s);





#ifdef __cplusplus
}
#endif

#endif /* HELPER_H */
