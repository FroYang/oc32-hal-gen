# OC32 HAL Config Generator

OC32 系列 MCU 的 HAL 配置文件生成工具。通过图形界面选择芯片型号、配置系统参数和模块功能，自动生成完整的 `oc32_hal_conf.h` 及 HAL 源文件目录。

## 项目结构

```
oc32-hal-gen/
├── hal_conf_generator.py    # 主程序（Tkinter GUI + 生成逻辑）
├── build.ps1                # PyInstaller 打包脚本
├── chips/                   # 芯片型号配置（JSON）
│   ├── CV32S6015.json
│   └── CV32S6015U.json
└── hal/                     # HAL 模板和源码
    ├── oc32_hal_conf_template.h  # 配置文件模板
    ├── inc/                   # 头文件（复制到输出目录）
    └── src/                   # 源文件（复制到输出目录）
```

## 依赖

- Python 3.8+
- tkinter（Python 自带）
- PyInstaller（仅打包时使用）

## 使用方式

### 直接运行（开发调试）

```powershell
python hal_conf_generator.py
```

### 打包为独立 exe

```powershell
.\build.ps1
```

生成的可执行文件位于 `dist\OC32_HAL_Config_Generator.exe`，可直接分发给无 Python 环境的用户。

## 功能说明

### 左侧面板

**Chip Selection（芯片选择）**
- 从 `chips/` 目录自动加载所有 JSON 配置文件，下拉列表显示芯片型号名称

**System Settings（系统配置）**
- **XHOSC Frequency**：外部高速振荡器频率（MHz），可选 1–24
- **Tick Interrupt Priority**：系统滴答定时器中断优先级（0–3，0 最低）
- **Tick Frequency**：系统滴答频率，可选 `10HZ` / `100HZ` / `1KHZ`
- **Debug UART**：调试打印输出串口，可选 `UART0` / `UART1`
- **Baud Rate**：调试串口波特率，可选 9600 ~ 921600

**Output Directory（输出目录）**
- 配置生成文件的保存路径，默认 `c:\oc32\include`
- 点击 "Browse..." 可选择目录

**生成按钮**
- **Generate Config**：根据当前配置生成 HAL 目录
- **Open Output Dir**：打开输出目录

### 右侧面板

**Feature Configure（功能模块配置）**
- 根据所选芯片的 `features` 字段动态生成复选框
- 勾选模块后，对应 HAL 宏将被展开；未勾选则注释掉
- 支持模块依赖关系：勾选依赖项时，其依赖的模块自动勾选（如 `FLASH_VERIFY` 依赖 `FLASH`）

## 芯片配置文件（JSON 格式）

在 `chips/` 目录下新增 JSON 文件即可支持新芯片，无需修改代码。

```json
{
  "name": "CV32S6015",
  "ihosc_freq": 24,        // 内部高速振荡器频率（MHz）
  "sclk_freq": 99,         // 系统时钟频率（MHz）
  "flash_size": 256,       // Flash 总容量（KB）
  "zone0_size": 30,        // Zone0 大小（KB，Boot 区）
  "zone1_size": 98,        // Zone1 大小（KB，User 区）
  "dma_channels": 4,       // DMA 通道数量
  "dma_map0": "SPI0",      // DMA 通道 0 映射外设（0~15）
  "dma_map1": "SPI1",
  ...
  "vdd_value": 5000,       // VDD 电压（mV）

  "features": [
    {
      "key": "FLASH",                    // 模块键名（对应宏名）
      "description": "Read/Write Flash", // 界面显示的描述文字
      "always": false                    // true: 强制启用，不出现在 Feature 列表中
    },
    {
      "key": "FLASH_VERIFY",
      "description": "Verify Flash Code",
      "always": false,
      "macro_name": "FLASH_VERIFY",      // 自定义宏名后缀（跳过 HAL_XXX_ENABLE 生成）
      "dependencies": ["FLASH"]          // 依赖模块列表（勾选时自动启用）
    }
  ]
}
```

### JSON 字段说明

| 字段 | 类型 | 说明 |
|---|---|---|
| `name` | string | 芯片名称，显示在下拉列表中 |
| `ihosc_freq` | int | 内部 OSC 频率（MHz），用于 `OC32_IHOSC_FREQ` |
| `sclk_freq` | int | 系统时钟频率（MHz） |
| `flash_size` | int | Flash 总容量（KB） |
| `zone0_size` | int | Boot 区大小（KB） |
| `zone1_size` | int | User 区大小（KB） |
| `dma_channels` | int | DMA 通道数（0~4） |
| `dma_map0~dma_map15` | string | 各通道映射的外设名称，`reserved0/1` 表示未使用 |
| `vdd_value` | int | VDD 电压值（mV） |
| `features[].key` | string | 模块标识符，用于生成 `HAL_{key}_ENABLE` |
| `features[].always` | bool | `true` 时模块强制启用，不显示在 Feature 列表 |
| `features[].macro_name` | string | 自定义宏名后缀，有此项时不生成 `HAL_{key}_ENABLE` |
| `features[].dependencies` | string[] | 依赖模块列表，勾选此项时自动勾选依赖 |

## 生成产物

点击 Generate 后，在输出目录生成以下结构：

```
c:\oc32\
└── hal\
    ├── inc\
    │   └── oc32_hal_conf.h   ← 生成的配置文件
    └── src\
        ├── oc32_hal.c
        ├── oc32_hal_flash.c
        ├── oc32_hal_gpio.c
        └── ...
```
