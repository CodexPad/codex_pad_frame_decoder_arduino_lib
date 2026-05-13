# CodexPad Frame Decoder Arduino lib

[中文](README.zh-CN.md)

## Overview

This library is a **data frame decoder** for the **CodexPad** series gamepads on the **Arduino** platform. It receives raw byte streams via a `Stream` object, parses the data protocol frames, and extracts button states and joystick axis values. It is suitable for scenarios where gamepad data is received through a serial or Bluetooth transparent transmission module.

> **💡 About `Stream`**: `Stream` is the base class for data stream communication in the Arduino core library. It defines the reading and parsing functionality in Arduino and provides a unified interface for all serial communications. Common classes like `HardwareSerial` (hardware serial class) and `SoftwareSerial` (software serial class) both inherit from `Stream`, and `Serial` is a predefined instance of the `HardwareSerial` class. This library receives data via the `Stream` interface, so it does not depend on any specific hardware communication method. You can pass any object that implements the `Stream` interface (such as `Serial`, `SoftwareSerial`, etc.) into this library. For more information about `Stream`, see the [Arduino Stream Official Documentation](https://docs.arduino.cc/language-reference/en/functions/communication/stream/).

**The library itself does not handle Bluetooth connections**; it only verifies and parses the gamepad data protocol frames. For how to establish a Bluetooth connection (e.g., by sending AT commands), please refer to the `Connect()` function in the specific examples.

For detailed information about CodexPad products, please refer to the following product documentation.

| CodexPad Model | Details |
| :--- | :--- |
| CodexPad-C10 | [Product Details](../../../codex_pad_c10/blob/main/README.md#codexpad-c10) |
| CodexPad-S10 | [Product Details](../../../codex_pad_s10/blob/main/README.md#codexpad-s10) |

## Supported Platforms

This library only relies on the `Stream` interface. It receives and parses CodexPad gamepad data protocol frames via `Stream` and is not concerned with the Bluetooth connection process or bound to any specific hardware. It is suitable for any Arduino-compatible platform that supports `Stream` input, including those using an external Bluetooth transparent transmission module (such as BLE-UNO, NL-16, etc.).

### Supported Bluetooth Modules

| Supported Bluetooth Modules |
| :------------ |
| NL-16 (V1.2+) |
| HC-05         |
| JDY-08        |
| Other Bluetooth transparent transmission modules supporting AT commands |

### Supported Hardware Platforms

| Supported Hardware Platforms |
| :--- |
| Arduino UNO  |
| Arduino Nano |
| Arduino Mega 2560 |
| BLE-UNO |
| Other Arduino-compatible platforms |

> **📌 Note**: This library is not limited to the above platforms. Any Arduino-compatible board that supports the `Stream` interface can be used.

## Features

- **Real-time Button Event Detection**: Reads the input status of all buttons in real-time and distinguishes between three events: **pressed**, **released**, and **holding**.

- **High-Precision Joystick Data**: Retrieves the analog values of the left and right joystick X and Y axes, ranging from 0 to 255, providing precise control input.

- **Reliable Data Frame Decoding**: Built-in robust_frame protocol parser, supporting frame synchronization, escape reversal, and CRC-8 integrity verification.

## Usage Instructions

### Preparation

Before starting programming, please complete the following preparations to ensure a smooth development process.

#### Familiarize Yourself with Product Documentation

- Read the datasheet of the Bluetooth module you are using (e.g., the NL-16 Bluetooth module) carefully to understand how to use it, how to connect, and the corresponding AT commands.

- Read the CodexPad product manual thoroughly to fully understand the hardware features, the gamepad's button and joystick layout, function definitions, indicator light status, and basic operations like powering on/off.

#### Obtain and Record the Gamepad's **Bluetooth Device Address (BD_ADDR)**

> **⚠️ Important Notice**: The examples in this library connect using the **Bluetooth Device Address (BD_ADDR)**. **When programming, you must explicitly specify your gamepad's Bluetooth Device Address (BD_ADDR) in the code.**

Please refer to the method provided in the product manual to obtain your gamepad's **Bluetooth Device Address (BD_ADDR)**. Its format is typically `"0C:3D:5E:9D:80:99"` (consisting of characters 0-9, A-F, with colons as separators). Please keep this information safe, as you will need to replace it with your own gamepad's actual **Bluetooth Device Address (BD_ADDR)** in the code later.

#### Power on the Gamepad and Enter Connection Standby State

- Power on the gamepad. After powering on, the gamepad will automatically enter a **connection standby state** where it is discoverable via Bluetooth. At this time, the gamepad's indicator light should be **slowly flashing (approximately once per second)**.

### Installing the CodexPad Frame Decoder Library

1. **Open Arduino IDE Library Manager**
   - Menu bar: **Tools** → **Manage Libraries...**
   - Shortcut: `Ctrl+Shift+I` (Windows/Linux) or `Cmd+Shift+I` (Mac)

2. **Search and Install**
   - In the search box, type: `CodexPadFrameDecoder`
   - Locate the CodexPadFrameDecoder library
   - **Ensure the latest version is selected** in the dropdown menu
   - Click the **INSTALL** button

    ![Search for CodexPadFrameDecoder in Library Manager](assets/images/en/install_codexpad_frame_decoder_library.png)

    > **📌 Note:** The screenshot is for reference only. Please make sure to install the latest available version.

## Example Descriptions

Examples are organized in two levels: **functionality** and **Bluetooth module**: `examples/<functionality>/<Bluetooth module>/`.

Each functionality example contains exclusive subdirectories for different Bluetooth modules. The `Connect()` function implementation in each module's exclusive example differs to ensure the correct Bluetooth connection; the parsing part uniformly uses this library's `CodexPadFrameDecoder` class. You can choose the corresponding Bluetooth module example based on the module you are using.

### Hardware Wiring

Before running the examples, please complete the hardware wiring according to the Bluetooth module you are using.

#### Using the BLE-UNO Development Board

BLE-UNO is a development board that integrates a low-power Bluetooth chip onto the official standard Arduino UNO V3.0 motherboard, providing wireless Bluetooth functionality. The Bluetooth chip is already integrated, so **no additional wiring for the Bluetooth module is required**. However, to conveniently view the program's debug output, it is recommended to wire the debug serial port as follows:

| BLE-UNO | USB-to-TTL Module Pin |
| :------ | :--- |
| 5       | RXD |
| 6       | TXD |
| VCC     | 5V |
| GND     | GND |

> **📌 Note**: The pin numbers (5, 6) in the table above are the default configuration in the example code. If you have modified the debug serial port pins in your code, please refer to the `kDebugSerialRxPin` and `kDebugSerialTxPin` constants defined in your code.

![BLE-UNO Debug Serial Wiring Diagram](assets/images/zh-CN/wiring_ble_uno_debug.png)

#### Using Standalone Bluetooth Transparent Transmission Modules such as NL-16, HC-05

Modules like NL-16 and HC-05 need to be connected to the development board via a serial port. When using the NL-16 Bluetooth module, version V1.2 or higher is required.

The following is a typical wiring example (using an Arduino UNO development board with the NL-16 Bluetooth module):

**Bluetooth Module Wiring**

| Module Pin | Arduino Pin |
| :---------- | :----------- |
| +5V         | 5V           |
| GND         | GND          |
| TXD         | RX0          |
| RXD         | TX0          |
| STAT/D-RST  | RESET        |

Note: The table above is a wiring example for the NL-16 Bluetooth module on an Arduino UNO development board. Wiring may vary slightly for different Bluetooth modules; please wire according to the actual interface of your Bluetooth module.

**Debug Serial Wiring**

| Arduino Pin | USB-to-TTL Module Pin |
| :----------- | :-------------- |
| 5            | RXD             |
| 6            | TXD             |
| 3.3V         | VCC             |
| GND          | GND             |

> **📌 Note**: The pin numbers (5, 6) in the table above are the default configuration in the example code. If you have modified the debug serial port pins in your code, please refer to the `kDebugSerialRxPin` and `kDebugSerialTxPin` constants defined in your code.

![Bluetooth Module Wiring Diagram](assets/images/zh-CN/wireless_downloard.png)
![Serial Debug Wiring Diagram](assets/images/zh-CN/wiring_uno_nl16.png)

### Basic Polling Example (`basic_polling`)

- **Example Description**: Connects to the CodexPad via Bluetooth Device Address, and queries and prints all its button states and joystick values in real-time.

- **BLE-UNO / NL-16 Bluetooth Module Example Location**: In the Arduino IDE, find this example via **File** → **Examples** → **CodexPadFrameDecoder** → **basic_polling** → **ble_uno_or_nl_16_module**.

### Input State Detection Example (`inputs_detection`)

- **Example Description**: Connects to the CodexPad via Bluetooth Device Address, and prints messages when changes in button states and joystick values are detected.

- **BLE-UNO / NL-16 Bluetooth Module Example Location**: In the Arduino IDE, find this example via **File** → **Examples** → **CodexPadFrameDecoder** → **inputs_detection** → **ble_uno_or_nl_16_module**.

### How to View Debug Information

After the example program runs, all gamepad button states and joystick values will be output through the **debug serial port**. Since the default hardware serial port (D0/D1) is occupied by the Bluetooth module, it is not possible to view debug information via this port. You need to view the output through the debug serial port. Follow these steps:

1. Prepare a **USB-to-TTL module** and wire it according to the hardware wiring instructions. The TXD pin of the **USB-to-TTL module** should be connected to IO 6, and the RXD pin should be connected to IO 5.
2. Plug the USB-to-TTL module into the computer's USB port. The system will recognize a new serial port (COM port).
3. Open any serial port debugging tool.
4. Select the serial port (COM port) corresponding to the USB-to-TTL module, and set the baud rate to **115200**, data bits to 8, stop bits to 1, and parity to None.
5. Click "Open Serial Port" to view the debug information in real-time.

> **📌 Note**:
>
> 1. The debug serial port pin numbers (5/6) and baud rate (115200) above are the default values in the example code. If you have modified the pins or baud rate for the debug serial port in your code, please refer to the actual configuration in your code.
> 2. After a successful connection, you will first see the `Connected` message in the debug serial port. When you press a gamepad button or move a joystick, the corresponding state changes will be printed immediately, e.g., `Button Cross(A): pressed`, `L(X:128, Y:0)`, etc.

## API Reference

Details link: <https://codexpad.github.io/codex_pad_frame_decoder_arduino_lib/html/en/annotated.html>

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
