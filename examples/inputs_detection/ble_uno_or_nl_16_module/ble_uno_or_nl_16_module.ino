/**
 * @~Chinese
 * @file inputs_detection/ble_uno_or_nl_16_module/ble_uno_or_nl_16_module.ino
 * @example inputs_detection/ble_uno_or_nl_16_module/ble_uno_or_nl_16_module.ino
 * @brief 演示如何检测已连接的 CodexPad 设备的实时按钮状态与摇杆移动。
 * @details 本示例先通过 AT 指令与指定的 CodexPad 设备建立蓝牙连接，然后持续监控所有用户输入。
 *          它展示了三种不同的按钮状态检测： **按下** (瞬间按下)、 **释放** (瞬间释放)和 **持续按住** 。
 *          同时，它监控模拟摇杆轴，当检测到超过设定阈值的显著变化时打印其值，从而过滤微小抖动。
 *          @note 必须在主循环中尽可能频繁地调用 `Update()` 方法，且不得添加延时，以确保实时响应性并防止数据包丢失。
 * @see CodexPadFrameDecoder::Update
 * @see CodexPadFrameDecoder::pressed
 * @see CodexPadFrameDecoder::released
 * @see CodexPadFrameDecoder::holding
 * @see CodexPadFrameDecoder::HasAxisValueChanged
 * @see CodexPadFrameDecoder::axis_value
 */
/**
 * @~English
 * @file inputs_detection/ble_uno_or_nl_16_module/ble_uno_or_nl_16_module.ino
 * @example inputs_detection/ble_uno_or_nl_16_module/ble_uno_or_nl_16_module.ino
 * @brief Demonstrate how to detect real-time button status and joystick movement of connected CodexPad devices.
 * @details This example first sends AT commands to establish a BLE connection to a CodexPad device (by Bluetooth Device Address), then continuously
 * monitors all user inputs. It showcases the detection of three distinct button states: **pressed** (momentary press), **released** (momentary
 * release), and **holding** (sustained press). It also monitors the analog joystick axes and prints their values when a significant change beyond a
 * set threshold is detected, filtering out minor jitter.
 *          @note The `Update()` method must be called as frequently as possible within the main loop without delays to ensure real‑time
 *          responsiveness and prevent data packet loss.
 * @see CodexPadFrameDecoder::Update
 * @see CodexPadFrameDecoder::pressed
 * @see CodexPadFrameDecoder::released
 * @see CodexPadFrameDecoder::holding
 * @see CodexPadFrameDecoder::HasAxisValueChanged
 * @see CodexPadFrameDecoder::axis_value
 */

#include <SoftwareSerial.h>

#include "codex_pad_frame_decoder.h"

namespace {
// 调试串口引脚定义。
// 将外部串口工具连接到这些引脚以查看调试输出。
// Debug serial port pin definitions.
// Connect external serial port tools to these pins to view debug output.
constexpr uint8_t kDebugSerialRxPin = 6;  // 调试输出接收引脚 | RX pin for debug output
constexpr uint8_t kDebugSerialTxPin = 5;  // 调试输出发送引脚 | TX pin for debug output

// 摇杆轴变化检测的阈值。当前值与上一帧值的绝对差值达到此值（或到达边界0/255）时才被视为有效变化，用于过滤微小抖动。
// Threshold for joystick axis change detection. Only when the absolute difference between the current and previous axis value reaches this value
// (or the value hits boundaries 0/255) will it be reported as a significant change.
constexpr uint8_t kAxisThreshold = 2;

// 替换为你的 CodexPad 的 Bluetooth device address。
// Replace with your CodexPad device's Bluetooth device address.
const String kBluetoothDeviceAddress = "0C:3D:5E:9D:80:99";

// 与蓝牙模块通信的串口波特率（请查阅模块数据手册，替换为模块对应的波特率）。
// The serial port baud rate for communication with the Bluetooth module (please refer to the module data manual, replace with the corresponding baud
// rate for the module).
constexpr uint32_t kBluetoothModuleSerialBaudRate = 115200;

const CodexPadFrameDecoder::Button kAllButtons[] = {CodexPadFrameDecoder::Button::kUp,
                                                    CodexPadFrameDecoder::Button::kDown,
                                                    CodexPadFrameDecoder::Button::kLeft,
                                                    CodexPadFrameDecoder::Button::kRight,
                                                    CodexPadFrameDecoder::Button::kSquareX,
                                                    CodexPadFrameDecoder::Button::kTriangleY,
                                                    CodexPadFrameDecoder::Button::kCrossA,
                                                    CodexPadFrameDecoder::Button::kCircleB,
                                                    CodexPadFrameDecoder::Button::kL1,
                                                    CodexPadFrameDecoder::Button::kL2,
                                                    CodexPadFrameDecoder::Button::kL3,
                                                    CodexPadFrameDecoder::Button::kR1,
                                                    CodexPadFrameDecoder::Button::kR2,
                                                    CodexPadFrameDecoder::Button::kR3,
                                                    CodexPadFrameDecoder::Button::kSelect,
                                                    CodexPadFrameDecoder::Button::kStart,
                                                    CodexPadFrameDecoder::Button::kHome};

// 用于调试输出的软串口。
// A soft serial port used for debugging output.
SoftwareSerial g_debug_serial(kDebugSerialRxPin, kDebugSerialTxPin);

// 此处解码器传入的串口实例，与 Connect() 中传入的为同一个串口实例。
// The serial instance passed in by the decoder here is the same as the one passed in Connect().
CodexPadFrameDecoder g_codex_pad_frame_decoder(Serial);

/**
 * 将按钮枚举值转换为可读的字符串。
 * Convert a Button enumeration value to a human-readable string.
 */
const String ButtonToString(CodexPadFrameDecoder::Button button) {
  switch (button) {
    case CodexPadFrameDecoder::Button::kUp: {
      return "Up";  // 上按钮 | UP button
    }
    case CodexPadFrameDecoder::Button::kDown: {
      return "Down";  // 下按钮 | DOWN button
    }
    case CodexPadFrameDecoder::Button::kLeft: {
      return "Left";  // 左按钮 | LEFT button
    }
    case CodexPadFrameDecoder::Button::kRight: {
      return "Right";  // 右按钮 | RIGHT button
    }
    case CodexPadFrameDecoder::Button::kSquareX: {
      return "Square(X)";  // 方形 或者 X 按钮 | SQUARE or X button
    }
    case CodexPadFrameDecoder::Button::kTriangleY: {
      return "Triangle(Y)";  // 三角 或者 Y 按钮 | TRIANGLE or Y button
    }
    case CodexPadFrameDecoder::Button::kCrossA: {
      return "Cross(A)";  // 叉形 或者 A 按钮 | CROSS or A button
    }
    case CodexPadFrameDecoder::Button::kCircleB: {
      return "Circle(B)";  // 圆形 或者 B 按钮 | CIRCLE or B button
    }
    case CodexPadFrameDecoder::Button::kL1: {
      return "L1";  // L1按钮 | L1 button
    }
    case CodexPadFrameDecoder::Button::kL2: {
      return "L2";  // L2按钮 | L2 button
    }
    case CodexPadFrameDecoder::Button::kL3: {
      return "L3";  // L3按钮 | L3 button
    }
    case CodexPadFrameDecoder::Button::kR1: {
      return "R1";  // R1按钮 | R1 button
    }
    case CodexPadFrameDecoder::Button::kR2: {
      return "R2";  // R2按钮 | R2 button
    }
    case CodexPadFrameDecoder::Button::kR3: {
      return "R3";  // R3按钮 | R3 button
    }
    case CodexPadFrameDecoder::Button::kSelect: {
      return "Select";  // 选择按钮 | SELECT button
    }
    case CodexPadFrameDecoder::Button::kStart: {
      return "Start";  // 开始按钮 | START button
    }
    case CodexPadFrameDecoder::Button::kHome: {
      return "Home";  // 首页按钮 | HOME button
    }
    default: {
      return {};  // 未知按钮返回空字符串 | Unknown button returns empty string
    }
  }
}

/**
 * @~Chinese
 * @brief 向蓝牙模块发送 AT 指令以建立 BLE 连接。
 * @details 依次发送固定的 AT 指令序列（AT+DISCON, AT+RESET, AT+ECHO=1, AT+ROLE=0, AT+CON=<地址>），
 *          配置并连接到目标设备。
 * @param[in] bluetooth_stream 连接蓝牙模块的 Stream 对象。
 *                             注意：此对象必须与传入 CodexPadFrameDecoder 的 Stream 是同一个实例（如 Serial），
 *                             因为解码器正是从这同一个串口读取手柄发来的数据。
 * @param[in] bluetooth_device_address 目标设备的 MAC 地址，格式为 "XX:XX:XX:XX:XX:XX"。
 */
/**
 * @~English
 * @brief Send AT commands to the Bluetooth module to establish a BLE connection.
 * @details Sends a fixed sequence of AT commands (AT+DISCON, AT+RESET, AT+ECHO=1, AT+ROLE=0, AT+CON=<address>)
 *          to configure and connect to the target device.
 * @param[in] bluetooth_stream Stream connected to the Bluetooth module.
 *                             Note: This must be the same Stream instance that is passed to the CodexPadFrameDecoder
 *                             (e.g., Serial), as the decoder reads incoming data from this same serial port.
 * @param[in] bluetooth_device_address MAC address of the target device in format "XX:XX:XX:XX:XX:XX".
 */
void Connect(Stream &bluetooth_stream, const String &bluetooth_device_address) {
  if (bluetooth_device_address.length() != 17 || bluetooth_device_address[2] != ':' || bluetooth_device_address[5] != ':' ||
      bluetooth_device_address[8] != ':' || bluetooth_device_address[11] != ':' || bluetooth_device_address[14] != ':') {
    g_debug_serial.println("Error: Invalid MAC address format. Expected: XX:XX:XX:XX:XX:XX");
    while (true);
  }

  g_debug_serial.print("Start to connect ");
  g_debug_serial.println(kBluetoothDeviceAddress);

  // 断开任何已存在的蓝牙连接，确保进入清洁状态。
  // Disconnect any existing BLE connection to ensure a clean state.
  bluetooth_stream.println("AT+DISCON");
  delay(100);

  // 软件复位蓝牙芯片，清除所有配对和配置数据。
  // Software reset BLE chip, clear all pairing and configuration data.
  bluetooth_stream.println("AT+RESET");
  delay(100);

  // 打开AT信息显示。
  // Open AT information display.
  bluetooth_stream.println("AT+ECHO=1");
  delay(100);

  // 设置模块为主机模式，使其能够主动连接从机蓝牙。
  // Set the module to host mode so that it can actively connect to the BLE of the slave device.
  bluetooth_stream.println("AT+ROLE=0");
  delay(100);

  // 使用指定的MAC地址发起与从机蓝牙连接。
  // Initiate BLE connection with the slave using the specified MAC address.
  bluetooth_stream.print("AT+CON=");
  bluetooth_stream.println(bluetooth_device_address);
  delay(100);

  g_debug_serial.println("Connected");
}
}  // namespace

void setup() {
  g_debug_serial.begin(115200);

  Serial.begin(kBluetoothModuleSerialBaudRate);

  Connect(Serial, kBluetoothDeviceAddress);
}

void loop() {
  // 重要：Update()方法必须在循环中尽可能频繁地调用，不能添加延时。
  // 该方法负责处理所有接收到的蓝牙数据包，延时会导致数据丢失和响应延迟。
  // 对于实时控制应用，必须保持高频率调用以确保及时响应手柄输入。
  // Important: Update() method must be called as frequently as possible in the loop, no delays should be added.
  // This method processes all received Bluetooth packets, delays will cause data loss and response lag.
  // For real-time control applications, high-frequency calls are essential to ensure prompt response to gamepad input.
  g_codex_pad_frame_decoder.Update();

  for (auto button : kAllButtons) {
    // 检测按钮是否刚刚按下（从弹起变为按下）。
    // Check if button was just pressed (transition from released to pressed).
    if (g_codex_pad_frame_decoder.pressed(button)) {
      g_debug_serial.print("Button ");
      g_debug_serial.print(ButtonToString(button));
      g_debug_serial.println(": pressed");
    }

    // 检测按钮是否刚刚释放（从按下变为弹起）。
    // Check if button was just released (transition from pressed to released).
    else if (g_codex_pad_frame_decoder.released(button)) {
      g_debug_serial.print("Button ");
      g_debug_serial.print(ButtonToString(button));
      g_debug_serial.println(": released");
    }

    // 检测按钮是否持续按下状态。
    // Check if button is holding.
    else if (g_codex_pad_frame_decoder.holding(button)) {
      g_debug_serial.print("Button ");
      g_debug_serial.print(ButtonToString(button));
      g_debug_serial.println(": holding");
    }
  }

  // 检测左右两个摇杆的X轴或Y轴是否有显著变化。
  // Check if left and right sticks X or Y axes have significant changes.
  if (g_codex_pad_frame_decoder.HasAxisValueChanged(CodexPadFrameDecoder::Axis::kLeftStickX, kAxisThreshold) ||
      g_codex_pad_frame_decoder.HasAxisValueChanged(CodexPadFrameDecoder::Axis::kLeftStickY, kAxisThreshold) ||
      g_codex_pad_frame_decoder.HasAxisValueChanged(CodexPadFrameDecoder::Axis::kRightStickX, kAxisThreshold) ||
      g_codex_pad_frame_decoder.HasAxisValueChanged(CodexPadFrameDecoder::Axis::kRightStickY, kAxisThreshold)) {
    // 打印摇杆轴的当前值（0-255）。
    // Print current joystick axis values (0-255).
    g_debug_serial.print("L(X:");
    g_debug_serial.print(
        g_codex_pad_frame_decoder.axis_value(CodexPadFrameDecoder::Axis::kLeftStickX));  // 左摇杆X轴当前值 | Left stick X axis current value
    g_debug_serial.print(", Y:");
    g_debug_serial.print(
        g_codex_pad_frame_decoder.axis_value(CodexPadFrameDecoder::Axis::kLeftStickY));  // 左摇杆Y轴当前值 | Left stick Y axis current value
    g_debug_serial.print("), R(X:");
    g_debug_serial.print(
        g_codex_pad_frame_decoder.axis_value(CodexPadFrameDecoder::Axis::kRightStickX));  // 右摇杆X轴当前值 | Right stick X axis current value
    g_debug_serial.print(", Y:");
    g_debug_serial.print(
        g_codex_pad_frame_decoder.axis_value(CodexPadFrameDecoder::Axis::kRightStickY));  // 右摇杆Y轴当前值 | Right stick Y axis current value
    g_debug_serial.println(")");
  }
}