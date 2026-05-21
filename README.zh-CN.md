# CodexPadFrameDecoder Arduino lib

[English](README.md)

## 概述

本库为 **CodexPad** 系列手柄在 **Arduino** 平台上的**数据帧解析器**。它通过 `Stream` 对象接收原始数据字节流，解析出数据协议帧，并提取出按键状态与摇杆轴值。适用于**通过BLE转串口模块来使用手柄**的场景。

本库专注于数据帧的校验与解析，不负责蓝牙连接的建立。如何建立蓝牙连接（如发送 AT 指令）请参考具体示例中的 `Connect()` 函数。

> **💡 关于 `Stream`**：`Stream` 是 Arduino 核心库中用于数据流通信的基类。它定义了 Arduino 中的读取和解析功能，为所有的串口通信提供了统一的接口。常见的 `HardwareSerial`（硬件串口类）和 `SoftwareSerial`（软件串口类）都继承自 `Stream`，其中 `Serial` 是 `HardwareSerial` 类的一个预定义实例。关于 `Stream` 的详细信息详见 [Arduino Stream 官方文档](https://docs.arduino.cc/language-reference/en/functions/communication/stream/)。

关于 CodexPad 产品的详细信息，请查阅以下产品文档。

| CodexPad型号 | 详情 |
| :--- | :--- |
| CodexPad-C10 | [产品详情](../../../codex_pad_c10/blob/main/README.zh-CN.md#codexpad-c10) |
| CodexPad-S10 | [产品详情](../../../codex_pad_s10/blob/main/README.zh-CN.md#codexpad-s10) |

## 支持的平台

本库及示例代码适用于**通过BLE转串口模块来使用手柄**的场景，支持以下两种连接方案：

- 开发板板载集成BLE转串口模块

    适用于**板载了BLE转串口模块**的开发板（例如，BLE-UNO）。主控通过串口与蓝牙芯片进行通信，工作原理与 “开发板 + 外接BLE转串口模块” 相同。

    | 适用开发板 |
    | :-------- |
    | BLE-UNO   |

- 开发板外接BLE转串口模块

    适用于**没有板载集成BLE转串口模块**的开发板（例如，Arduino UNO、Nano），通过外接BLE转串口模块（例如，NL-16）来使用手柄。

    **支持的外接BLE转串口模块**

    | BLE转串口模块 |
    | :------------ |
    | NL-16 (V1.2+) |

    **支持的硬件平台**

    | 支持的硬件平台 |
    | :----------- |
    | Arduino UNO  |
    | Arduino Nano |

## 特性

- **实时按键事件检测**：可实时读取所有按键的输入状态，并区分**按下**、**释放**和**长按**三种事件。

- **高精度摇杆数据**：获取左右摇杆X轴和Y轴的模拟量数值，范围从0至255，提供精准的控制输入。

## 使用说明

### 准备工作

在开始编程前，请完成以下准备工作，以确保开发过程顺利进行。

#### 熟悉产品文档

- 详细阅读所使用的BLE转串口模块（例如，NL-16）的数据手册，熟悉如何使用、如何进行连接，以及相应的 AT 指令等基本信息。

- 详细阅读 CodexPad 产品手册，全面了解硬件特性、熟悉手柄按键摇杆布局、功能定义、指示灯状态以及开关机操作等基本信息。

#### 获取并记录手柄**Bluetooth Device Address(BD_ADDR)**

> **⚠️ 重要提示**：本库的示例中是通过 **Bluetooth Device Address(BD_ADDR)** 进行连接。**编程时，必须在代码明确指定您手柄的Bluetooth Device Address(BD_ADDR)。**

请参考产品手册中提供的方法，获取您手柄的**Bluetooth Device Address(BD_ADDR)**。其格式通常为 `"0C:3D:5E:9D:80:99"`（由0-9、A-F的字符组成，冒号为半角）。请妥善记录此信息，后续需要在代码为您自己手柄的实际**Bluetooth Device Address(BD_ADDR)**。

#### 开启手柄并进入待连接状态

- 将手柄开机，手柄开机后会自动处于蓝牙可被发现的**待连接状态**，此时手柄指示灯应呈现**慢闪状态（约每秒闪烁一次）**。

### 安装 CodexPadFrameDecoder 库

1. **打开 Arduino IDE 库管理器**

   - 菜单栏：**工具** → **管理库...**

   - 快捷键：`Ctrl+Shift+I`（Windows/Linux）或 `Cmd+Shift+I`（Mac）

2. **搜索并安装**

   - 在搜索框中输入：`CodexPadFrameDecoder`

   - 找到 CodexPadFrameDecoder 库

   - **确保在下拉菜单中选择最新版本**

   - 点击 **安装** 按钮

    ![在库管理中搜索 CodexPadFrameDecoder](assets/images/zh-CN/install_codexpad_frame_decoder_library.png)

    > **📌 注意：** 截图仅供参考。请务必安装最新可用版本。

## 示例说明

示例按**BLE转串口模块**和**功能**两个层级组织：`examples/<BLE转串口模块>/<功能>/`。

每个蓝牙模块目录下包含该模块支持的功能示例，模块专属的 `Connect()` 函数实现不同，保证蓝牙连接正确；解析部分则统一使用本库的 `CodexPadFrameDecoder` 类。您可根据使用的BLE转串口模块，自行选择对应的功能示例。

### BLE转串口模块

#### BLE-UNO 开发板

BLE-UNO 开发板已板载集成蓝牙芯片，**无需外接BLE转串口模块**。

##### 可用示例

###### 基础轮询示例 (`basic_polling`)

- **示例说明**：通过Bluetooth Device Address与CodexPad蓝牙连接，实时查询、打印其所有按钮状态与摇杆数值。

- **示例位置**：在 Arduino IDE 中，通过 **文件** → **示例** → **CodexPadFrameDecoder** → **ble_uno_or_nl_16_module** → **basic_polling** 找到该示例。

###### 输入状态检测示例 (`inputs_detection`)

- **示例说明**：通过Bluetooth Device Address与CodexPad蓝牙连接，检测到按钮状态与摇杆数值变化后打印。

- **示例位置**：在 Arduino IDE 中，通过 **文件** → **示例** → **CodexPadFrameDecoder** → **ble_uno_or_nl_16_module** → **inputs_detection** 找到该示例。

##### 接线说明

为查看程序输出的调试信息，引出调试串口即可。

| BLE-UNO 引脚 | USB转TTL模块引脚 |
| :----------- | :-------------- |
| 5            | RXD             |
| 6            | TXD             |
| 3.3V         | 3V3             |
| GND          | GND             |

> **📌 提示**：上表中的引脚号 (5, 6) 为示例代码中的默认配置。如果您在代码中修改了调试串口引脚，请以您代码中的实际配置为准（`kDebugSerialRxPin` 和 `kDebugSerialTxPin`）。

![接线图](assets/images/zh-CN/wiring_ble_uno_debug.png)

#### NL-16 BLE转串口模块

NL-16 BLE转串口模块需要配合开发板进行使用。

> NL-16 蓝牙透传模块请使用固件版本 v1.2 的，以确保与 CodexPad 手柄兼容。

##### 可用示例

###### 基础轮询示例 (`basic_polling`)

- **示例说明**：通过Bluetooth Device Address与CodexPad蓝牙连接，实时查询、打印其所有按钮状态与摇杆数值。

- **示例位置**：在 Arduino IDE 中，通过 **文件** → **示例** → **CodexPadFrameDecoder** → **ble_uno_or_nl_16_module** → **basic_polling** 找到该示例。

###### 输入状态检测示例 (`inputs_detection`)

- **示例说明**：通过Bluetooth Device Address与CodexPad蓝牙连接，检测到按钮状态与摇杆数值变化后打印。

- **示例位置**：在 Arduino IDE 中，通过 **文件** → **示例** → **CodexPadFrameDecoder** → **ble_uno_or_nl_16_module** → **inputs_detection** 找到该示例。

##### 接线说明

使用的开发板以 Arduino UNO 为例：

**Arduino UNO 与 NL-16 蓝牙透传模块接线**

| Arduino UNO 引脚 | NL‑16 引脚 |
| :--------------- | :--------- |
| 5V               | +5V        |
| GND              | GND        |
| RX0              | TXD        |
| TX0              | RXD        |
| RESET            | STAT/D-RST |

**Arduino UNO 与 USB转TTL模块接线**

| Arduino UNO 引脚 | USB转TTL模块引脚 |
| :--------------- | :-------------- |
| 5                | RXD             |
| 6                | TXD             |
| 3.3V             | 3V3             |
| GND              | GND             |

> **📌 提示**：上表中的引脚号 (5, 6) 为示例代码中的默认配置。如果您在代码中修改了调试串口引脚，请以您代码中的实际配置为准（`kDebugSerialRxPin` 和 `kDebugSerialTxPin`）。

![蓝牙模块接线图](assets/images/zh-CN/wireless_downloard.png)

![总接线图](assets/images/zh-CN/wiring_uno_nl16.png)

### 如何查看调试信息

示例程序运行后，调试信息（如按键事件、摇杆数值等）会通过**调试串口**输出。由于默认的硬件串口（D0/D1）已被蓝牙模块占用，无法通过此串口进行查看调试信息，所以需要通过调试串口查看输出。请按照以下步骤操作：

1. 按照上方的接线说明，将 **USB转TTL模块** 连接到对应的调试串口引脚上（默认引脚号是 5, 6）。

2. 将示例程序烧录到开发板中，

3. 烧录完成后,开发板上电运行程序。

4. 将 USB转TTL模块插入电脑的 USB 口，此时系统会识别出一个新的串口（COM 口）。

5. 打开任意一款串口调试工具。

6. 选择 USB转TTL模块对应的串口（COM 口），并设置波特率为 **115200**，数据位 **8**，停止位 **1**，校验位 **None**。

7. 点击“打开串口”，即可实时查看调试信息。

![串口调试工具配置](assets/images/zh-CN/view_debug_info.png)

> **📌 提示**：
>
> 1. 以上调试串口的引脚号（5/6）和波特率（115200）为示例代码的默认值。如果您在代码中修改了调试串口的引脚或波特率中的值，请以您代码中的实际配置为准。
> 2. 连接成功后，在调试串口中，您会先看到 `Connected` 的提示。当您按下手柄按键或推动摇杆时，对应的状态变化会立即打印出来，例如 `Button Triangle(Y): pressed`、`L(X:128, Y:0)` 等

## API说明

详情链接：<https://codexpad.github.io/codex_pad_frame_decoder_arduino_lib/html/zh-CN/annotated.html>

## 许可证

本项目采用 MIT 许可证 - 详见 [LICENSE](LICENSE) 文件。
