#pragma once

#ifndef _ROBUST_FRAME_H_
#define _ROBUST_FRAME_H_

#include <stddef.h>
#include <stdint.h>

/**
 * @file robust_frame.h
 */

namespace robust_frame {
/**
 * @~Chinese
 * @class Parser
 * @brief robust_frame 协议帧解析器。
 * @details 负责帧同步、转义还原和 CRC 校验，通过回调输出完整的有效载荷。
 *          不依赖任何特定通信方式，仅需通过 Feed() 逐字节喂入数据。
 *          最大载荷长度由构造函数参数指定，内部缓冲区按需动态分配。
 */
/**
 * @~English
 * @class Parser
 * @brief robust_frame protocol frame parser (receiver only).
 * @details Handles frame synchronization, escape reversal, and CRC verification,
 *          outputting complete payloads via callback. Does not depend on any specific
 *          communication method; only requires feeding bytes one at a time via Feed().
 *          The maximum payload size is specified via the constructor parameter, and
 *          the internal buffer is dynamically allocated accordingly.
 */
class Parser {
 public:
  /**
   * @~Chinese
   * @brief 帧解析回调函数类型。
   * @param data       有效载荷数据指针（不含帧头帧尾和 CRC）。
   * @param user_data  用户自定义上下文指针，由构造函数传入，在回调中原样返回。
   */
  /**
   * @~English
   * @brief Frame parse callback function type.
   * @param data       Pointer to the payload data (excluding frame header, footer, and CRC).
   * @param user_data  User-defined context pointer, passed in via the constructor and returned as-is in the callback.
   */
  using FrameCallback = void (*)(const uint8_t *data, void *user_data);

  /**
   * @~Chinese
   * @brief 构造函数，根据最大载荷长度动态分配内部缓冲区。
   * @param[in] max_payload_size 最大有效载荷长度（字节数），不含 CRC。
   *                             内部实际分配的缓冲区大小为 max_payload_size + 1，
   *                             额外 1 字节用于存储 CRC 校验值。
   * @param[in] callback         帧解析成功时的回调函数（若为 nullptr 则不回调）。
   * @param[in] user_data        回调时传入的用户自定义上下文指针（通常传 this 指针）。
   */
  /**
   * @~English
   * @brief Constructor. Dynamically allocates the internal buffer based on the maximum payload size.
   * @param[in] max_payload_size Maximum payload size in bytes (excluding CRC).
   *                             The actual allocated buffer size is max_payload_size + 1,
   *                             where the extra byte is used to store the CRC checksum.
   * @param[in] callback         Callback function invoked when a frame is successfully parsed (no callback if nullptr).
   * @param[in] user_data        User-defined context pointer passed through to the callback (typically a `this` pointer).
   */
  Parser(const size_t max_payload_size, FrameCallback callback, void *user_data);

  /**
   * @~Chinese
   * @brief 析构函数，释放内部动态分配的缓冲区。
   */
  /**
   * @~English
   * @brief Destructor. Frees the internally allocated buffer.
   */
  ~Parser();

  /**
   * @~Chinese
   * @brief 处理单个字节的核心状态机。
   * @param[in] byte 输入的字节。
   *
   * @details 状态转换图：
   *          - kSyncHeader：等待帧头 0xAA，收到后进入 kCollectPayload。
   *          - kCollectPayload：
   *              - 收到 0x55（帧尾）→ 校验 CRC，若通过则回调有效载荷，然后 Reset。
   *              - 收到 0xDB（转义字符）→ 进入 kUnescape。
   *              - 收到 0xAA（意外帧头）或缓冲区满 → Reset 并重新从该字节开始同步。
   *              - 其他 → 存入内部缓冲区。
   *          - kUnescape：将字节与 0x20 异或还原，检查结果是否为合法特殊字符（0xAA/0x55/0xDB），
   *            若不是则丢弃当前帧并重新同步；否则存储还原后的字节并回到 kCollectPayload。
   *
   * @note 缓冲区满或遇到意外的帧头会触发 Reset 并从当前字节重新尝试同步。
   */
  /**
   * @~English
   * @brief Core state machine for processing a single byte.
   * @param[in] byte The input byte.
   *
   * @details State transition diagram:
   *          - kSyncHeader: Wait for frame header 0xAA, enter kCollectPayload upon receiving it.
   *          - kCollectPayload:
   *              - Receive 0x55 (frame footer) → Verify CRC, callback payload if valid, then Reset.
   *              - Receive 0xDB (escape character) → Enter kUnescape.
   *              - Receive 0xAA (unexpected frame header) or buffer full → Reset and resync starting from this byte.
   *              - Otherwise → Store into internal buffer.
   *          - kUnescape: XOR the byte with 0x20 to restore, check if the result is a valid special character
   *            (0xAA/0x55/0xDB). If not, discard the current frame and resync; otherwise store the restored byte
   *            and return to kCollectPayload.
   *
   * @note Buffer full or encountering an unexpected frame header will trigger Reset and attempt resync from the
   *       current byte.
   */
  void Feed(const uint8_t byte);

 private:
  Parser(const Parser &) = delete;
  Parser &operator=(const Parser &) = delete;

  enum class State {
    kSyncHeader,
    kCollectPayload,
    kUnescape,
  };

  void Reset();

  const size_t max_payload_size_ = 0;
  const FrameCallback callback_;
  const void *const user_data_ = nullptr;
  uint8_t *const buffer_ = nullptr;
  uint8_t buffer_len_ = 0;
  State state_ = State::kSyncHeader;
};
}  // namespace robust_frame

#endif  // _ROBUST_FRAME_H_