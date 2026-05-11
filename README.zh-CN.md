# CodexPad Frame Decoder Arduino lib

[English](README.md)

## 概述

本库为 **CodexPad** 系列手柄在 **Arduino** 平台上的**数据帧解析器**。它通过 `Stream` 对象接收原始数据字节流，解析出数据协议帧，并提取出按键状态与摇杆轴值。适用于通过串口或蓝牙透传模块接收手柄数据的场景。

**库本身不涉及蓝牙连接**，只负责对手柄数据协议帧的校验与解析。如何建立蓝牙连接（如发送 AT 指令）请参考具体示例中的 `Connect()` 函数。

关于 CodexPad 产品的详细信息，请查阅以下产品文档。

| CodexPad型号 | 详情 |
| :--- | :--- |
| CodexPad-C10 | [产品详情](../../../codex_pad_c10/blob/main/README_CN.md#codexpad-c10) |
| CodexPad-S10 | [产品详情](../../../codex_pad_s10/blob/main/README_CN.md#codexpad-s10) |

## 支持的平台

本库仅依赖 `Stream` 接口，通过 `Stream` 接收 CodexPad 手柄的数据协议帧进行解析，并不关心蓝牙连接过程，不与特定硬件绑定，适用于通过串口外接蓝牙透传模块（如 BLE-UNO、NL-16等）或任意支持  `Stream`  输入的 Arduino 兼容平台。

## 特性

- **实时按键事件检测**：可实时读取所有按键的输入状态，并区分**按下**、**释放**和**长按**三种事件。

- **高精度摇杆数据**：获取左右摇杆X轴和Y轴的模拟量数值，范围从0至255，提供精准的控制输入。

- **可靠的数据帧解码**：内置 robust_frame 协议解析器，支持帧同步、转义还原和 CRC-8 完整性校验。

## 使用说明

### 准备工作

在开始编程前，请完成以下准备工作，以确保开发过程顺利进行。

#### 熟悉产品文档

- 详细阅读所使用的蓝牙模块（如 NL-16 蓝牙模块）的数据手册，熟悉如何使用、如何进行连接，以及相应的AT指令等基本信息。

- 详细阅读 CodexPad 产品手册，全面了解硬件特性、熟悉手柄按键摇杆布局、功能定义、指示灯状态以及开关机操作等基本信息。

#### 获取并记录手柄**Bluetooth Device Address(BD_ADDR)**

> **⚠️ 重要提示**：本库的示例中是通过 **Bluetooth Device Address(BD_ADDR)** 进行连接。**编程时，必须在代码明确指定您手柄的Bluetooth Device Address(BD_ADDR)。**

请参考产品手册中提供的方法，获取您手柄的**Bluetooth Device Address(BD_ADDR)**。其格式通常为 `"0C:3D:5E:9D:80:99"`（由0-9、A-F的字符组成，冒号为半角）。请妥善记录此信息，后续需要在代码为您自己手柄的实际**Bluetooth Device Address(BD_ADDR)**。

#### 开启手柄并进入待连接状态

- 将手柄开机，手柄开机后会自动处于蓝牙可被发现的**待连接状态**，此时手柄指示灯应呈现**慢闪状态（约每秒闪烁一次）**。

### 安装 CodexPad Frame Decoder 库

1. **下载库文件**

    **下载链接**：[codex_pad_frame_decoder_arduino_lib-x.x.x.zip](../../../codex_pad_frame_decoder_arduino_lib/archive/refs/tags/v1.0.0.zip)

2. 在 Arduino IDE 中，通过**项目** → **加载库** → **添加 .ZIP 库...** 来安装此文件。

## 示例说明

示例按**功能**和**蓝牙模块**两个层级组织：`examples/<功能>/<蓝牙模块>/`.  

每个功能示例下会包含不同蓝牙模块的专属子目录，模块专属示例中的 `Connect()` 函数实现不同，保证蓝牙连接正确；解析部分则统一使用本库的 `CodexPadFrameDecoder` 类。您可根据使用的蓝牙模块，自行选择对应的蓝牙模块示例。

### 基础轮询示例 (`basic_polling`)

- **示例说明**：通过Bluetooth Device Address与CodexPad蓝牙连接，实时查询、打印其所有按钮状态与摇杆数值。

- **BLE-UNO / NL-16 蓝牙模块示例位置**：在 Arduino IDE 中，通过 **文件** → **示例** → **CodexPad_Frame_Decoder** → **basic_polling** → **ble_uno_or_nl_16_module** 找到该示例。

### 输入状态检测示例 (`inputs_detection`)

- **示例说明**：通过Bluetooth Device Address与CodexPad蓝牙连接，检测到按钮状态与摇杆数值变化后打印。

- **BLE-UNO / NL-16 蓝牙模块示例位置**：在 Arduino IDE 中，通过 **文件** → **示例** → **CodexPad_Frame_Decoder** → **inputs_detection** → **ble_uno_or_nl_16_module** 找到该示例。

## API说明

详情链接：<https://codexpad.github.io/codex_pad_frame_decoder_arduino_lib/html/zh-CN/annotated.html>

## 许可证

本项目采用 MIT 许可证 - 详见 [LICENSE](LICENSE) 文件。
