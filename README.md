# CodexPad Frame Decoder Arduino Lib

[中文](README.zh-CN.md)

## Overview

This library is a **data frame parser** for the **CodexPad** series gamepads on the **Arduino** platform. It receives raw data byte streams via a `Stream` object, parses the protocol frames, and extracts button states and joystick axis values. It is suitable for scenarios where gamepad data is received through a serial or Bluetooth transparent transmission module.

> **💡 About `Stream`**: `Stream` is the base class for data stream communication in the Arduino core library. It defines the read and parse capabilities in Arduino and provides a unified interface for all serial communication. Common `HardwareSerial` and `SoftwareSerial` classes inherit from `Stream`, where `Serial` is a predefined instance of `HardwareSerial`. Because this library receives data through the `Stream` interface, it does not depend on a specific hardware communication method; you can pass any object that implements the `Stream` interface (e.g., `Serial`, `SoftwareSerial`) to the library. For more details about `Stream`, refer to the [Arduino Stream official documentation](https://docs.arduino.cc/language-reference/en/functions/communication/stream/).

**The library itself does not handle Bluetooth connections**; it only validates and parses the gamepad data protocol frames. For instructions on how to establish a Bluetooth connection (e.g., by sending AT commands), please refer to the `Connect()` function in the specific examples.

For detailed information about CodexPad products, please check the following product documentation.

| CodexPad Model | Details |
| :--- | :--- |
| CodexPad-C10 | [Product Details](../../../codex_pad_c10/blob/main/README.md#codexpad-c10) |
| CodexPad-S10 | [Product Details](../../../codex_pad_s10/blob/main/README.md#codexpad-s10) |

## Supported Platforms

This library and its examples are intended for scenarios where **CodexPad gamepad data is received via a serial Bluetooth transparent transmission module**. The following two connection approaches are supported:

- **Development board with integrated Bluetooth transparent transmission module (no external module required)**

    Suitable for development boards that come with a **built-in Bluetooth transparent transmission module**. The main controller communicates with the Bluetooth chip through a serial port, working in the same way as a development board paired with an external Bluetooth module (e.g., Arduino + NL-16).

    | Supported Development Board |
    | :-------------------------- |
    | BLE-UNO                     |

- **Development board with an external Bluetooth transparent transmission module**

    Suitable for development boards that **do not have an integrated Bluetooth transparent transmission module** (e.g., Arduino UNO, Nano, Mega 2560), where an external serial Bluetooth transparent transmission module (e.g., NL-16) is used to receive and parse the data frames from the CodexPad gamepad.

    **Supported External Bluetooth Transparent Transmission Modules**

    | Bluetooth Transparent Transmission Module |
    | :---------------------------------------- |
    | NL-16 (V1.2+)                             |

    **Supported Hardware Platforms**

    | Supported Hardware Platform |
    | :-------------------------- |
    | Arduino UNO                 |
    | Arduino Nano                |
    | Arduino Mega 2560           |

## Features

- **Real-time button event detection**: All button input states can be read in real time, distinguishing between **press**, **release**, and **long press** events.
- **High-precision joystick data**: Obtain analog values from both the left and right joystick X and Y axes, ranging from 0 to 255, for precise control input.
- **Reliable data frame decoding**: Built-in robust_frame protocol parser with support for frame synchronization, escape handling, and CRC-8 integrity check.

## Usage

### Prerequisites

Before programming, complete the following preparations to ensure a smooth development process.

#### Familiarize yourself with product documentation

- Thoroughly read the datasheet of the Bluetooth module you are using (e.g., NL-16 Bluetooth module) to understand how to use it, how to connect it, and the corresponding AT commands.
- Thoroughly read the CodexPad product manual to fully understand the hardware features, button and joystick layout, function definitions, LED indicators, power on/off operations, and other basic information.

#### Obtain and record the gamepad's **Bluetooth Device Address (BD_ADDR)**

> **⚠️ Important**: The examples in this library connect via **Bluetooth Device Address (BD_ADDR)**. **You must explicitly specify your gamepad's Bluetooth Device Address (BD_ADDR) in the code during programming.**

Please refer to the method provided in the product manual to obtain your gamepad's **Bluetooth Device Address (BD_ADDR)**. Its format is typically `"0C:3D:5E:9D:80:99"` (consisting of characters 0-9 and A-F, with colons as separators). Keep a record of this address; you will need to enter your own gamepad's actual **Bluetooth Device Address (BD_ADDR)** in the code later.

#### Power on the gamepad and enter standby mode

- Power on the gamepad. After powering on, the gamepad automatically enters the Bluetooth discoverable **standby mode**, indicated by a **slow blinking LED (approximately once per second)**.

### Install the CodexPad Frame Decoder Library

1. **Open the Arduino IDE Library Manager**
   - Menu: **Tools** → **Manage Libraries...**
   - Shortcut: `Ctrl+Shift+I` (Windows/Linux) or `Cmd+Shift+I` (Mac)

2. **Search and Install**
   - Enter `CodexPadFrameDecoder` in the search box.
   - Locate the CodexPadFrameDecoder library.
   - **Make sure to select the latest version from the dropdown menu.**
   - Click the **Install** button.

    ![Search for CodexPadFrameDecoder in Library Manager](assets/images/en/install_codexpad_frame_decoder_library.png)

    > **📌 Note:** The screenshot is for reference only. Please install the latest available version.

## Examples

Examples are organized by **functionality** and **Bluetooth module**: `examples/<functionality>/<bluetooth_module>/`.

Each functional example includes dedicated subdirectories for different Bluetooth modules. The `Connect()` function in these module-specific examples differs to ensure proper Bluetooth connection, while the parsing part uniformly uses the `CodexPadFrameDecoder` class from this library. You can choose the appropriate Bluetooth module example based on the module you are using.

### Hardware Wiring

Before running the examples, select the appropriate wiring method based on your development board type.

#### Development Board with Integrated Bluetooth Transparent Transmission Module (e.g., BLE-UNO)

These development boards are essentially equivalent to "Arduino UNO + an external Bluetooth transparent transmission module" where the main controller still communicates with the Bluetooth chip via serial, requiring **no additional wiring**. Taking BLE-UNO as an example, to view the program's debug output, it is recommended to connect a debug serial interface as follows:

| BLE-UNO Pin | USB-to-TTL Module Pin |
| :---------- | :-------------------- |
| 5           | RXD                   |
| 6           | TXD                   |
| VCC         | 5V                    |
| GND         | GND                   |

> **📌 Tip**: The pin numbers (5, 6) in the table above are the default configuration in the example code. If you have changed the debug serial pins in your code, please use the `kDebugSerialRxPin` and `kDebugSerialTxPin` constants defined in your code as the reference.

![Debug Serial Wiring Diagram](assets/images/en/wiring_ble_uno_debug.png)

#### Development Board with External Bluetooth Transparent Transmission Module (e.g., Arduino UNO + NL-16)

Suitable for standard Arduino boards without an integrated Bluetooth module, requiring an external Bluetooth transparent transmission module. When using NL-16, ensure the firmware is version 1.2 or higher.

The following is a typical wiring example (using Arduino UNO + NL-16):

**Bluetooth Module Wiring**

| NL-16 Pin   | Arduino UNO Pin |
| :---------- | :-------------- |
| +5V         | 5V              |
| GND         | GND             |
| TXD         | RX0             |
| RXD         | TX0             |
| STAT/D-RST  | RESET           |

> Note: Pin definitions may vary slightly for different Bluetooth modules. Please refer to the actual interface of your module.

**Debug Serial Wiring**

| Arduino UNO Pin | USB-to-TTL Module Pin |
| :-------------- | :-------------------- |
| 5               | RXD                   |
| 6               | TXD                   |
| 3.3V            | VCC                   |
| GND             | GND                   |

![Bluetooth Module Wiring Diagram](assets/images/en/wireless_downloard.png)
![Debug Serial Wiring Diagram](assets/images/en/wiring_uno_nl16.png)

### Basic Polling Example (`basic_polling`)

- **Description**: Connects to the CodexPad via Bluetooth Device Address, continuously queries and prints all button states and joystick values.
- **BLE-UNO / NL-16 Module Example Location**: In the Arduino IDE, go to **File** → **Examples** → **CodexPadFrameDecoder** → **basic_polling** → **ble_uno_or_nl_16_module** to find this example.

### Input Detection Example (`inputs_detection`)

- **Description**: Connects to the CodexPad via Bluetooth Device Address, detects button state and joystick value changes, and prints them upon change.
- **BLE-UNO / NL-16 Module Example Location**: In the Arduino IDE, go to **File** → **Examples** → **CodexPadFrameDecoder** → **inputs_detection** → **ble_uno_or_nl_16_module** to find this example.

### How to View Debug Information

After running an example program, all gamepad button states and joystick values are output through the **debug serial port**. Because the default hardware serial port (D0/D1) is occupied by the Bluetooth module in the examples, you cannot view debug information through that port; you must use a debug serial port to see the output. Follow these steps:

1. Prepare a **USB-to-TTL module** and wire it according to the hardware wiring instructions: connect the TXD pin of the USB-to-TTL module to IO 6, and the RXD pin to IO 5.
2. Plug the USB-to-TTL module into a USB port on your computer. The system will recognize a new serial port (COM port).
3. Open any serial debugging tool.
4. Select the serial port (COM port) corresponding to the USB-to-TTL module, and set the baud rate to **115200**, data bits 8, stop bits 1, parity None.
5. Click "Open Serial Port" to view the debug information in real time.

> **📌 Tips**:
>
> 1. The debug serial pin numbers (5/6) and baud rate (115200) above are the default values in the example code. If you have modified the debug serial pins or baud rate in your code, please refer to your actual configuration.
> 2. Upon successful connection, you will first see the message `Connected` in the debug serial port. When you press a button or move a joystick on the gamepad, the corresponding status changes will be printed immediately, such as `Button Cross(A): pressed`, `L(X:128, Y:0)`, etc.

## API Reference

Detailed API documentation: <https://codexpad.github.io/codex_pad_frame_decoder_arduino_lib/html/en/annotated.html>

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file.
