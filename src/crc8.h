#pragma once

#ifndef _CRC8_H_
#define _CRC8_H_

#include <stddef.h>
#include <stdint.h>

/**
 * @file crc8.h
 */

namespace crc8 {
/**
 * @~Chinese
 * @brief 计算数据的 CRC-8 校验值（SAE J1850 标准）。
 * @param[in] data 待计算的数据指针。
 * @param[in] size 数据长度（字节数）。
 * @return uint8_t 计算得到的 CRC 值。
 *
 * @details 使用标准 CRC-8/SAE J1850 算法：
 *          - 多项式：0x1D (x^8 + x^4 + x^3 + x^2 + 1)
 *          - 初始值：0xFF
 *          - 最终异或：0xFF
 *          - 输入/输出不反转
 *          内部使用 256 字节查表法运算，提高效率。
 */
/**
 * @~English
 * @brief Calculate the CRC-8 checksum of data (SAE J1850 standard).
 * @param[in] data Pointer to the data to be calculated.
 * @param[in] size Length of the data in bytes.
 * @return uint8_t The calculated CRC value.
 *
 * @details Uses the standard CRC-8/SAE J1850 algorithm:
 *          - Polynomial: 0x1D (x^8 + x^4 + x^3 + x^2 + 1)
 *          - Initial value: 0xFF
 *          - Final XOR: 0xFF
 *          - No input/output reversal
 *          Internally uses a 256-byte lookup table for efficient computation.
 */
uint8_t Calculate(const uint8_t *data, const size_t size);
}  // namespace crc8

#endif  // _CRC8_H_