#pragma once

#ifndef _CODEX_PAD_FRAME_DECODER_H_
#define _CODEX_PAD_FRAME_DECODER_H_

#include <Stream.h>

#include "robust_frame.h"

/**
 * @file codex_pad_frame_decoder.h
 */

/**
 * @~Chinese
 * @class CodexPadFrameDecoder
 * @brief CodexPad 手柄数据帧解码主类。
 * @details 通过外部传入的 Stream 对象获取原始字节流，内部使用 robust_frame 协议解析器完成帧同步、转义还原和 CRC
 *          校验，成功接收完整的有效载荷后，输出按键状态和摇杆轴值。
 */
/**
 * @~English
 * @class CodexPadFrameDecoder
 * @brief CodexPad controller data frame decoding main class.
 * @details It takes a Stream object as input to read the raw byte stream, and internally uses the robust_frame protocol parser for frame
 *          synchronization, escape sequence restoration, and CRC validation. Upon successfully receiving a complete and valid payload, it outputs
 *          button states and joystick axis values.
 */
class CodexPadFrameDecoder {
 public:
  /**
   * @~Chinese
   * @brief 摇杆轴值的数量。
   */
  /**
   * @~English
   * @brief Number of axis values.
   */
  static constexpr size_t kAxisValueNum = 4;

  /**
   * @~Chinese
   * @brief 摇杆轴的中心值（静止时的典型值）。
   */
  /**
   * @~English
   * @brief Center value of joystick axes (typical value at rest).
   */
  static constexpr uint8_t kAxisCenter = 0x80;

  /**
   * @~Chinese
   * @brief 手柄输入报告的类型标识（有效载荷的第一个字节）。
   */
  /**
   * @~English
   * @brief Input report type identifier for the gamepad (the first byte of the payload).
   */
  static constexpr uint8_t kPayloadType = 0x01;

  /**
   * @~Chinese
   * @enum Button
   * @brief 手柄按键掩码枚举。
   */
  /**
   * @~English
   * @enum Button
   * @brief Gamepad button mask enumeration.
   */
  enum class Button : uint32_t {
    /**
     * @~Chinese
     * @brief 上。
     */
    /**
     * @~English
     * @brief Up.
     */
    kUp = uint32_t{1} << 0,

    /**
     * @~Chinese
     * @brief 下。
     */
    /**
     * @~English
     * @brief Down.
     */
    kDown = uint32_t{1} << 1,

    /**
     * @~Chinese
     * @brief 左。
     */
    /**
     * @~English
     * @brief Left.
     */
    kLeft = uint32_t{1} << 2,

    /**
     * @~Chinese
     * @brief 右。
     */
    /**
     * @~English
     * @brief Right.
     */
    kRight = uint32_t{1} << 3,

    /**
     * @~Chinese
     * @brief 方形 或者 X。
     */
    /**
     * @~English
     * @brief Square or X.
     */
    kSquareX = uint32_t{1} << 4,

    /**
     * @~Chinese
     * @brief 三角形 或者 Y。
     */
    /**
     * @~English
     * @brief Triangle or Y.
     */
    kTriangleY = uint32_t{1} << 5,

    /**
     * @~Chinese
     * @brief 叉形 或者 A。
     */
    /**
     * @~English
     * @brief Cross or A.
     */
    kCrossA = uint32_t{1} << 6,

    /**
     * @~Chinese
     * @brief 圆形 或者 B。
     */
    /**
     * @~English
     * @brief Circle or B.
     */
    kCircleB = uint32_t{1} << 7,

    /**
     * @~Chinese
     * @brief L1。
     */
    /**
     * @~English
     * @brief L1.
     */
    kL1 = uint32_t{1} << 8,

    /**
     * @~Chinese
     * @brief L2。
     */
    /**
     * @~English
     * @brief L2.
     */
    kL2 = uint32_t{1} << 9,

    /**
     * @~Chinese
     * @brief L3。
     */
    /**
     * @~English
     * @brief L3.
     */
    kL3 = uint32_t{1} << 10,

    /**
     * @~Chinese
     * @brief R1。
     */
    /**
     * @~English
     * @brief R1.
     */
    kR1 = uint32_t{1} << 11,

    /**
     * @~Chinese
     * @brief R2。
     */
    /**
     * @~English
     * @brief R2.
     */
    kR2 = uint32_t{1} << 12,

    /**
     * @~Chinese
     * @brief R3。
     */
    /**
     * @~English
     * @brief R3.
     */
    kR3 = uint32_t{1} << 13,

    /**
     * @~Chinese
     * @brief 选择。
     */
    /**
     * @~English
     * @brief Select.
     */
    kSelect = uint32_t{1} << 14,

    /**
     * @~Chinese
     * @brief 开始。
     */
    /**
     * @~English
     * @brief Start.
     */
    kStart = uint32_t{1} << 15,

    /**
     * @~Chinese
     * @brief 首页。
     */
    /**
     * @~English
     * @brief Home.
     */
    kHome = uint32_t{1} << 16,
  };

  /**
   * @~Chinese
   * @enum Axis
   * @brief 摇杆轴索引枚举。
   */
  /**
   * @~English
   * @enum Axis
   * @brief Joystick axis index enumeration.
   */
  enum class Axis : size_t {
    /**
     * @~Chinese
     * @brief 左摇杆X轴。
     */
    /**
     * @~English
     * @brief Left stick X axis.
     */
    kLeftStickX,

    /**
     * @~Chinese
     * @brief 左摇杆Y轴。
     */
    /**
     * @~English
     * @brief Left stick Y axis.
     */
    kLeftStickY,

    /**
     * @~Chinese
     * @brief 右摇杆X轴。
     */
    /**
     * @~English
     * @brief Right stick X axis.
     */
    kRightStickX,

    /**
     * @~Chinese
     * @brief 右摇杆Y轴。
     */
    /**
     * @~English
     * @brief Right stick Y axis.
     */
    kRightStickY,
  };

  /**
   * @~Chinese
   * @brief 构造函数。
   * @param[in] stream 用于读取数据字节流的 Stream 对象引用。
   */
  /**
   * @~English
   * @brief Constructor.
   * @param[in] stream Reference to a Stream object for reading data byte stream.
   */
  CodexPadFrameDecoder(Stream &stream);

  /**
   * @~Chinese
   * @brief 更新解码器状态。需要在主循环中不断调用此函数，以喂入并解析字节流。
   */
  /**
   * @~English
   * @brief Update the decoder state. This function must be called continuously in the main loop
   *        to feed and parse the byte stream.
   */
  void Update();

  /**
   * @~Chinese
   * @brief 查询按键是否被按下。
   * @retval true 按键被按下。
   * @retval false 按键没有被按下。
   */
  /**
   * @~English
   * @brief check if a button is pressed.
   * @retval true if the button is pressed.
   * @retval false if the button is not pressed.
   */
  bool pressed(const Button button) const;

  /**
   * @~Chinese
   * @brief 查询按键是否被释放。
   * @retval true 按键被释放。
   * @retval false 按键没有被释放。
   */
  /**
   * @~English
   * @brief check if a button is released.
   * @retval true if the button is released.
   * @retval false if the button is not released.
   */
  bool released(const Button button) const;

  /**
   * @~Chinese
   * @brief 查询按键是否被持续按下。
   * @retval true 按键被持续按下。
   * @retval false 按键没有被持续按下。
   */
  /**
   * @~English
   * @brief check if a button is held.
   * @retval true if the button is held.
   * @retval false if the button is not held.
   */
  bool holding(const Button button) const;

  /**
   * @~Chinese
   * @brief 查询按键是否被按下或持续按下。
   * @retval true 按键被按下或持续按下。
   * @retval false 按键没有被按下或持续按下。
   */
  /**
   * @~English
   * @brief check if a button is pressed or held.
   * @retval true if the button is pressed or held.
   * @retval false if the button is not pressed or held.
   */
  bool button_state(const Button button) const;

  /**
   * @~Chinese
   * @brief 以位掩码形式获取所有按键的当前状态。
   * @return 一个32位无符号整数，每一位（bit）表示 @ref Button 枚举中对应按键的状态（1为按下，0为未按下）。
   *         位的分配从最低有效位（LSB，第0位）开始，依次对应 @ref Button 中的各个值。
   *         可使用位与操作 (&) 配合具体的 @ref Button 枚举值来检查特定按键状态。
   * 示例：
   * @code
   *   const uint32_t button_states = g_codex_pad_frame_decoder.button_states();
   *   if ((button_states & CodexPadFrameDecoder::Button::kUp)) != 0) {
   *     // 方向上按键被按下
   *   }
   * @endcode
   */
  /**
   * @~English
   * @brief Get all button states, return a 32-bit unsigned integer where each bit represents the state of a specific button.
   * @return A 32-bit unsigned integer where each bit represents the state of a specific button.
   *         The bits are from the least significant bit (LSB, bit 0) to the most significant bit (MSB, bit 31).
   *         You can use bit-and (&) operator to check specific button states.
   * Example:
   * @code
   *   const uint32_t button_states = g_codex_pad_frame_decoder.button_states();
   *   if ((button_states & CodexPadFrameDecoder::Button::kUp) != 0) {
   *     // Up button is pressed
   *   }
   * @endcode
   */
  uint32_t button_states() const;

  /**
   * @~Chinese
   * @brief 获取指定摇杆轴的原始值（0~255）。
   * @param[in] axis 要读取的轴。
   * @return 该轴的当前值，中心为 0x80。
   */
  /**
   * @~English
   * @brief Get the raw value (0~255) of a specified joystick axis.
   * @param[in] axis The axis to read.
   * @return The current value of the axis, with center at 0x80.
   */
  uint8_t axis_value(const Axis axis) const;

  /**
   * @~Chinese
   * @brief 获取所有模拟轴的当前值（返回只读数组指针）。
   * @details 返回指向内部 4 字节数组的 const 指针，索引与 Axis 枚举对应关系：
   *          - 索引 0：左摇杆 X 轴 (Axis::kLeftStickX)
   *          - 索引 1：左摇杆 Y 轴 (Axis::kLeftStickY)
   *          - 索引 2：右摇杆 X 轴 (Axis::kRightStickX)
   *          - 索引 3：右摇杆 Y 轴 (Axis::kRightStickY)
   *          每个值的范围 0~255，中心位置为 128 (0x80)。
   * @return const uint8_t* 指向 4 字节轴值数组的只读指针。
   * @note 返回的指针在 CodexPadFrameDecoder 对象生命周期内有效，下次调用 Update() 后内容会更新。
   *
   * 示例：
   * @code
   *   const uint8_t *axes = g_codex_pad_frame_decoder.axis_values();
   *   uint8_t left_stick_x = axes[static_cast<size_t>(CodexPadFrameDecoder::Axis::kLeftStickX)];
   *   uint8_t left_stick_y = axes[static_cast<size_t>(CodexPadFrameDecoder::Axis::kLeftStickY)];
   * @endcode
   */
  /**
   * @~English
   * @brief Get current values of all analog axes (returns a read-only array pointer).
   * @details Returns a const pointer to the internal 4-byte array. Indices correspond to the Axis enumeration:
   *          - Index 0: Left stick X axis (Axis::kLeftStickX)
   *          - Index 1: Left stick Y axis (Axis::kLeftStickY)
   *          - Index 2: Right stick X axis (Axis::kRightStickX)
   *          - Index 3: Right stick Y axis (Axis::kRightStickY)
   *          Each value ranges from 0 to 255, with the neutral center at 128 (0x80).
   * @return const uint8_t* Read-only pointer to the 4-byte axis value array.
   * @note The returned pointer is valid for the lifetime of the CodexPadFrameDecoder object.
   *       Content will be updated after the next call to Update().
   *
   * Example:
   * @code
   *   const uint8_t *axes = g_codex_pad_frame_decoder.axis_values();
   *   uint8_t left_stick_x = axes[static_cast<size_t>(CodexPadFrameDecoder::Axis::kLeftStickX)];
   *   uint8_t left_stick_y = axes[static_cast<size_t>(CodexPadFrameDecoder::Axis::kLeftStickY)];
   * @endcode
   */
  const uint8_t *axis_values() const;

  /**
   * @~Chinese
   * @brief 查询轴值是否发生变化。
   * @param[in] axis 轴。
   * @param[in] threshold 阈值。
   * @retval true 轴值发生变化。
   * @retval false 轴值未发生变化。
   */
  /**
   * @~English
   * @brief check if an axis value has changed.
   * @param[in] axis Axis.
   * @param[in] threshold Threshold.
   * @retval true if the axis value has changed.
   * @retval false if the axis value has not changed.
   */
  bool HasAxisValueChanged(const Axis axis, const uint8_t threshold) const;

 private:
  CodexPadFrameDecoder(const CodexPadFrameDecoder &) = delete;
  CodexPadFrameDecoder &operator=(const CodexPadFrameDecoder &) = delete;

  struct Inputs {
    uint32_t button_states = 0;
    uint8_t axis_values[kAxisValueNum] = {kAxisCenter, kAxisCenter, kAxisCenter, kAxisCenter};
  };

  void OnFrame(const uint8_t *data);

  static void OnFrameReceived(const uint8_t *data, void *user_data);

  Inputs prev_inputs_;
  Inputs current_inputs_;

  robust_frame::Parser parser_;

  Stream &stream_;
};

#endif  // _CODEX_PAD_FRAME_DECODER_H_