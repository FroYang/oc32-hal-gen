# OC32 HAL Config Generator

OC32 系列 MCU 的 HAL 配置文件生成工具。通过图形界面选择芯片型号、配置系统参数和模块功能，自动生成完整的 `hal/` 目录（含定制化的 `oc32_hal_conf.h` 及其他 HAL 源文件）。

可打包为独立 `.exe`（PyInstaller），分发给没有 Python 环境的用户。

## 项目结构

```
oc32-hal-gen/
├── hal_conf_generator.py       # 主程序（Tkinter GUI + 生成逻辑）
├── build.ps1                   # PyInstaller 打包脚本
├── OC32_HAL_Config_Generator.spec  # PyInstaller 打包规范
├── chips/                      # 芯片型号配置（JSON，新增即支持新芯片）
│   ├── CV32S6015.json
│   └── CV32S6015U.json
└── hal/                        # HAL 模板和源码
    ├── oc32_hal_conf_template.h # 配置文件模板（含 __占位符__）
    ├── inc/                    # 头文件（复制到输出目录）
    ├── src/                    # 源文件（复制到输出目录）
    └── chip/                   # 芯片 MSR 头文件（复制到输出目录）
```

> 程序默认在 `chips/` 中自动发现所有 `*.json`，因此新增芯片只需添加 JSON 文件，无需改动代码。

## 依赖

- Python 3.8+
- `tkinter`（Python 自带）
- `PyInstaller`（仅打包时需要）

## 使用方式

### 直接运行（开发调试）

```powershell
python hal_conf_generator.py
```

### 打包为独立 exe

```powershell
.\build.ps1
```

生成的可执行文件位于 `dist\OC32_HAL_Config_Generator.exe`，脚本会将其复制到 `D:\nsis\oc32\bin`。

## 界面说明

### 左侧面板

**Chip Selection（芯片选择）**
- 从 `chips/` 目录自动加载所有 JSON 配置文件，下拉列表显示芯片型号名称（文件名去掉扩展名）。

**System Settings（系统配置）**

| 项 | 说明 | 可选值 |
|---|---|---|
| XHOSC Frequency (MHz) | 外部高速振荡器频率，生成 `OC32_XHOSC_FREQ` | 1–24 |
| Tick Frequency | 系统滴答频率，决定 `HAL_TICK_FREQ_DEFAULT` | 10Hz / 100Hz / 1KHz |
| Debug UART | 调试输出串口，决定 `HAL_UART0_DEBUG` / `HAL_UART1_DEBUG` | UART0 / UART1 |
| Baud Rate | 调试串口波特率，生成 `HAL_UART_DEBUG_BR` | 9600 … 1000000 |

**Output Directory（输出目录）**
- 配置生成文件的保存路径，默认为用户桌面（`~/Desktop`）。
- 点击 **Browse...** 可选择目录。

**生成按钮**
- **Generate Config**：根据当前配置生成 HAL 目录。
- **Open Output Dir**：用系统文件管理器打开输出目录。

### 右侧面板

**Feature Configure（功能模块配置）**
- 根据所选芯片的 `features` 字段动态生成复选框（`always: true` 的模块强制启用、不出现在列表中）。
- 勾选模块后，对应 `HAL_XXX_ENABLE` 宏被展开；未勾选则以注释形式保留。
- 支持模块依赖关系：勾选某模块时，其 `dependencies` 中列出的模块会被自动启用（例如勾选 `FLASH_VERIFY` 会一并启用 `FLASH` 与 `CRC`）。依赖项在生成时自动补全，无需手动勾选。

## 芯片配置文件（JSON 格式）

在 `chips/` 目录下新增 JSON 文件即可支持新芯片，无需修改代码。字段说明：

| 字段 | 类型 | 说明 |
|---|---|---|
| `name` | string | 芯片名称，显示在生成的配置头注释中 |
| `ihosc_freq` | int | 内部高速振荡器频率（**Hz**），用于 `OC32_IHOSC_FREQ` |
| `ilosc_freq` | int | 内部低速振荡器频率（Hz），用于 `OC32_ILOSC_FREQ`（缺省 4000） |
| `cdr_freq` | int | USB CDR 输出频率（Hz），用于 `OC32_CDR_FREQ`（缺省 12000000） |
| `sclk_freq` | int | 系统时钟频率（**Hz**），用于 `OC32_SCLK_FREQ` |
| `flash_size` | int | Flash 总容量（KB） |
| `zone0_size` | int | Boot 区大小（KB） |
| `zone1_size` | int | User 区大小（KB） |
| `int_priority` | int | 中断优先级级别数，生成 `OC32_HIC_PRI0..PRI(n-1)` |
| `dma_channels` | int | DMA 通道数，生成 `OC32_DMA_CH0..CH(n-1)` |
| `dma_map` | string[] | 各 DMA 通道映射的外设名，按索引生成 `OC32_DMA_XXX`；`reservedN` 表示未使用 |
| `hic_map` | string[] | 各 HIC（中断）通道映射的外设名，按索引生成 `OC32_HIC_CH_XXX` 与位掩码 `OC32_HIC_XXX` |
| `vdd_value` | int | VDD 电压值（mV） |
| `features` | object[] | 可选功能模块列表，见下 |

每个 `features[]` 条目：

| 字段 | 类型 | 说明 |
|---|---|---|
| `key` | string | 模块标识，默认生成 `HAL_{key}_ENABLE` |
| `description` | string | 界面显示的描述文字 |
| `always` | bool | `true` 时强制启用，不显示在 Feature 列表 |
| `macro_name` | string | 自定义宏名后缀；有此项时生成 `HAL_{macro_name}_ENABLE`（而非 `HAL_{key}_ENABLE`） |
| `dependencies` | string[] | 依赖模块 key 列表；勾选本项时自动启用它们 |

完整示例（`CV32S6015U.json`）：

```json
{
  "name": "CV32S6015U",
  "ihosc_freq": 24000000,
  "ilosc_freq": 4000,
  "cdr_freq": 12000000,
  "sclk_freq": 99000000,
  "flash_size": 256,
  "zone0_size": 30,
  "zone1_size": 98,
  "int_priority": 4,
  "dma_channels": 4,
  "dma_map": [
    "SPI0", "SPI1", "IIC", "UART0", "UART1", "ADC0",
    "MPWM0", "MPWM1", "MPWM2", "FLASH", "DAC0", "ADC1",
    "DAC1", "reserved0", "reserved1", "CRC"
  ],
  "hic_map": [
    "SERR", "CMP0", "CMP1", "CCP0", "CCP1", "CCP2", "CCP3", "CCP4",
    "CCP5", "TIMER0", "TIMER1", "TIMER2", "TIMER3", "TIMER4", "TIMER5",
    "TIMER6", "TIMER7", "WDT", "ADC0", "ADC1", "UART0", "UART1",
    "SPI0", "SPI1", "IIC", "MPWM0", "MPWM1", "MPWM2", "DAC", "CRC",
    "PORTA", "PORTB"
  ],
  "vdd_value": 5000,
  "features": [
    { "key": "PORTB",        "description": "GPIO Port B",            "always": true },
    { "key": "CRC",          "description": "CRC Calculator" },
    { "key": "FLASH",        "description": "Read/Write Flash",
      "dependencies": ["CRC"] },
    { "key": "FLASH_VERIFY", "description": "Verify Flash Code",
      "macro_name": "FLASH_VERIFY", "dependencies": ["FLASH", "CRC"] },
    { "key": "LVD",          "description": "Low Voltage Detector" },
    { "key": "UART",         "description": "UART" },
    { "key": "TIMER",        "description": "Timer" },
    { "key": "CCP",          "description": "Capture/Compare/PWM",   "dependencies": ["TIMER"] },
    { "key": "USB",          "description": "Fullspeed USB" },
    { "key": "RCM_CDR",      "description": "RCM Use USB CDR",
      "macro_name": "RCM_CDR", "dependencies": ["USB"] },
    { "key": "DEBUG",        "description": "Debug Printf Via UART", "dependencies": ["UART"] }
  ]
}
```

## 生成产物

点击 **Generate Config** 后，在输出目录下生成如下结构：

```
<out_dir>/
└── hal/
    ├── inc/
    │   ├── oc32_hal_conf.h   ← 根据当前配置生成的头文件
    │   ├── oc32_hal.h
    │   └── ...（其余 inc/ 头文件）
    ├── src/
    │   ├── oc32_hal.c
    │   ├── oc32_hal_flash.c
    │   └── ...（其余 src/ 源文件）
    └── chip/
        └── CV32S6015MSR.h（chip/ 目录内容）
```

生成逻辑要点：
- `oc32_hal_conf.h` 由 `hal/oc32_hal_conf_template.h` 渲染而来，替换所有 `__占位符__`（时钟、Flash、DMA、HIC、中断优先级、模块启用等）。
- 其余 `inc/`、`src/`、`chip/` 目录内容原样复制，跳过已生成的 `oc32_hal_conf.h`。
