/**
 * @~Chinese
 * @file basic_polling/ble_uno_or_nl_16_module/ble_uno_or_nl_16_module.ino
 * @example basic_polling/ble_uno_or_nl_16_module/ble_uno_or_nl_16_module.ino
 * @brief 演示通过基本轮询方式定期打印 CodexPad 所有按钮状态与摇杆值。
 * @details 本示例先通过 AT 指令连接到指定的 CodexPad 设备（根据蓝牙设备地址），然后进入轮询循环。
 *          每隔 30 毫秒，它会打印所有按钮的当前状态（按下/弹起）以及两个摇杆的原始模拟值（0‑255）。
 *          它展示了 `button_state()` 用于离散按钮查询和 `axis_value()` 用于连续摇杆读取的基本用法。
 *          @note 本示例使用简单的定时机制（`millis()`）以固定间隔打印，适用于状态监控或日志记录。
 *          对于实时控制应用，请确保尽可能频繁地调用 `Update()` 且无阻塞延时。
 * @see CodexPadFrameDecoder::button_state
 * @see CodexPadFrameDecoder::axis_value
 * @see CodexPadFrameDecoder::Update
 */
/**
 * @~English
 * @file basic_polling/ble_uno_or_nl_16_module/ble_uno_or_nl_16_module.ino
 * @example basic_polling/ble_uno_or_nl_16_module/ble_uno_or_nl_16_module.ino
 * @brief The demonstration regularly prints the status of all buttons and joystick values in CodexPad using a basic polling method.
 * @details This example first sends AT commands to connect to a designated CodexPad (by Bluetooth Device Address), then enters a simple polling loop.
 *          Every 30 milliseconds, it prints the current state of all buttons (pressed/released) and the raw analog values (0‑255) of both joysticks.
 *          It showcases the fundamental usage of `button_state()` for discrete button queries and `axis_value()` for continuous joystick readings.
 *          @note This example uses a simple timing mechanism (`millis()`) to print at a fixed interval, which is suitable for monitoring or logging.
 *          For real‑time control, ensure `Update()` is called as frequently as possible without blocking delays.
 * @see CodexPadFrameDecoder::button_state
 * @see CodexPadFrameDecoder::axis_value
 * @see CodexPadFrameDecoder::Update
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

// 替换为你的 CodexPad 的 Bluetooth device address。
// Replace with your CodexPad device's Bluetooth device address.
const String kBluetoothDeviceAddress = "0C:3D:5E:9D:80:99";

// 与蓝牙模块通信的串口波特率（请查阅模块数据手册，替换为模块对应的波特率）。
// The serial port baud rate for communication with the Bluetooth module (please refer to the module data manual, replace with the corresponding baud
// rate for the module).
constexpr uint32_t kBluetoothModuleSerialBaudRate = 115200;

// 此处解码器传入的串口实例，与 Connect() 中传入的为同一个串口实例。
// The serial instance passed in by the decoder here is the same as the one passed in Connect().
CodexPadFrameDecoder g_codex_pad_frame_decoder(Serial);

// 用于调试输出的软串口。
// A soft serial port used for debugging output.
SoftwareSerial g_debug_serial(kDebugSerialRxPin, kDebugSerialTxPin);

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

  static uint32_t s_print_time = 0;
  if (s_print_time == 0 || s_print_time + 30 < millis()) {
    s_print_time = millis();

    g_debug_serial.print("Up:");
    g_debug_serial.print(g_codex_pad_frame_decoder.button_state(CodexPadFrameDecoder::Button::kUp));
    g_debug_serial.print(", ");
    g_debug_serial.print("Down:");
    g_debug_serial.print(g_codex_pad_frame_decoder.button_state(CodexPadFrameDecoder::Button::kDown));
    g_debug_serial.print(",");
    g_debug_serial.print("Left:");
    g_debug_serial.print(g_codex_pad_frame_decoder.button_state(CodexPadFrameDecoder::Button::kLeft));
    g_debug_serial.print(", ");
    g_debug_serial.print("Right:");
    g_debug_serial.print(g_codex_pad_frame_decoder.button_state(CodexPadFrameDecoder::Button::kRight));
    g_debug_serial.print(", ");
    g_debug_serial.print("Square(X):");
    g_debug_serial.print(g_codex_pad_frame_decoder.button_state(CodexPadFrameDecoder::Button::kSquareX));
    g_debug_serial.print(", ");
    g_debug_serial.print("Triangle(Y):");
    g_debug_serial.print(g_codex_pad_frame_decoder.button_state(CodexPadFrameDecoder::Button::kTriangleY));
    g_debug_serial.print(", ");
    g_debug_serial.print("Cross(A):");
    g_debug_serial.print(g_codex_pad_frame_decoder.button_state(CodexPadFrameDecoder::Button::kCrossA));
    g_debug_serial.print(", ");
    g_debug_serial.print("Circle(B):");
    g_debug_serial.print(g_codex_pad_frame_decoder.button_state(CodexPadFrameDecoder::Button::kCircleB));
    g_debug_serial.print(", ");
    g_debug_serial.print("L1:");
    g_debug_serial.print(g_codex_pad_frame_decoder.button_state(CodexPadFrameDecoder::Button::kL1));
    g_debug_serial.print(", ");
    g_debug_serial.print("L2:");
    g_debug_serial.print(g_codex_pad_frame_decoder.button_state(CodexPadFrameDecoder::Button::kL2));
    g_debug_serial.print(", ");
    g_debug_serial.print("L3:");
    g_debug_serial.print(g_codex_pad_frame_decoder.button_state(CodexPadFrameDecoder::Button::kL3));
    g_debug_serial.print(", ");
    g_debug_serial.print("R1:");
    g_debug_serial.print(g_codex_pad_frame_decoder.button_state(CodexPadFrameDecoder::Button::kR1));
    g_debug_serial.print(", ");
    g_debug_serial.print("R2:");
    g_debug_serial.print(g_codex_pad_frame_decoder.button_state(CodexPadFrameDecoder::Button::kR2));
    g_debug_serial.print(", ");
    g_debug_serial.print("R3:");
    g_debug_serial.print(g_codex_pad_frame_decoder.button_state(CodexPadFrameDecoder::Button::kR3));
    g_debug_serial.print(", ");
    g_debug_serial.print("Select:");
    g_debug_serial.print(g_codex_pad_frame_decoder.button_state(CodexPadFrameDecoder::Button::kSelect));
    g_debug_serial.print(", ");
    g_debug_serial.print("Start:");
    g_debug_serial.print(g_codex_pad_frame_decoder.button_state(CodexPadFrameDecoder::Button::kStart));
    g_debug_serial.print(", ");
    g_debug_serial.print("Home:");
    g_debug_serial.print(g_codex_pad_frame_decoder.button_state(CodexPadFrameDecoder::Button::kHome));
    g_debug_serial.print(", ");

    // 获取摇杆轴数据，axis_value()返回0~255的数值。
    // 中间位置约为128，数值范围表示摇杆的偏移程度。
    // Get joystick axis data, axis_value() returns value from 0 to 255.
    // Center position is around 128, values represent stick deflection.
    g_debug_serial.print("L(X:");
    g_debug_serial.print(g_codex_pad_frame_decoder.axis_value(CodexPadFrameDecoder::Axis::kLeftStickX));
    g_debug_serial.print(", Y:");
    g_debug_serial.print(g_codex_pad_frame_decoder.axis_value(CodexPadFrameDecoder::Axis::kLeftStickY));
    g_debug_serial.print("), R(X:");
    g_debug_serial.print(g_codex_pad_frame_decoder.axis_value(CodexPadFrameDecoder::Axis::kRightStickX));
    g_debug_serial.print(", Y:");
    g_debug_serial.print(g_codex_pad_frame_decoder.axis_value(CodexPadFrameDecoder::Axis::kRightStickY));
    g_debug_serial.println(")");
  }
}