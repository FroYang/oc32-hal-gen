# OC32 HAL Config Generator

OC32 系列 MCU 的 HAL 配置文件生成工具。通过图形界面选择芯片型号、配置系统参数和模块功能，自动生成完整的 `hal/` 目录（含定制化的 `oc32_hal_conf.h` 及其他 HAL 源文件）。

可打包为独立可执行文件（PyInstaller），分发给没有 Python 环境的用户（Windows 生成 `.exe`，Linux 生成无后缀二进制）。打包产物内已自带 Python 解释器与 Tcl/Tk，用户机器无需安装 Python。

## 项目结构

```
oc32-hal-gen/
├── pure_ttk/                  # ★ 主程序（纯 tkinter + ttk，原生控件，零 Canvas 重绘）
│   ├── gen.py                 # 生成逻辑（纯函数，无 GUI 依赖，与 customtk 共用同一份）
│   ├── ui.py                  # 图形界面（原生 tk/ttk 控件）
│   └── README.md              # pure_ttk 版说明
├── build_pure_ttk.bash        # PyInstaller 打包脚本（Linux，可 Wine 交叉生成 Windows）
├── build_pure_ttk.ps1        # PyInstaller 打包脚本（Windows）
├── _experiments/            # 历史对照版本（customtk / native_combo，已归档，非主线）
│   ├── customtk/              # 最早的 CustomTkinter 版
│   ├── native_combo/         # 混合版（下拉换原生，其余仍 CTk）
│   └── build_ctk.bash/.ps1  # customtk 版打包脚本
├── OC32_HAL_Config_Generator.spec  # PyInstaller 打包规范
├── THIRD-PARTY-LICENSES.txt   # 第三方许可声明（商用发布随附）
├── chips/                     # 芯片型号配置（JSON，新增即支持新芯片）
│   ├── CV32S6015.json
│   └── CV32S6015U.json
└── hal/                       # HAL 模板和源码
    ├── oc32_hal_conf_template.h # 配置文件模板（含 __占位符__）
    ├── inc/                   # 头文件（复制到输出目录）
    ├── src/                   # 源文件（复制到输出目录）
    └── chip/                  # 芯片 MSR/SR 头文件（复制到输出目录）
```

> 程序默认在 `chips/` 中自动发现所有 `*.json`，因此新增芯片只需添加 JSON 文件，无需改动代码。


## 依赖

- Python 3.8+（`pure_ttk` 只用标准库 `tkinter`/`ttk`，**无需任何第三方库**）
- `PyInstaller`（仅打包时需要）

> 旧的 `customtk/`、`native_combo/` 版运行/打包时仍需 `customtkinter`（`pip install customtkinter`）。

## 使用方式

### 直接运行（开发调试）

```bash
python3 pure_ttk/ui.py
```

> `gen.py` 为纯生成逻辑（无 GUI），可在无图形环境下单独调用；`ui.py` 为图形界面入口。

### 打包为独立可执行文件

Windows：

```powershell
.\build_pure_ttk.ps1
```

生成的可执行文件位于 `dist_pure_ttk\OC32_HAL_Config_Generator.exe`，脚本会将其复制到 `D:\nsis\oc32\bin`。

Linux（同时可选交叉生成 Windows）：

```bash
./build_pure_ttk.bash
```

生成 `dist_pure_ttk/OC32_HAL_Config_Generator`，脚本会将其复制到 `/home/fro/oc32/linux/bin`；若检测到 Wine 及 Windows Python，还会交叉生成 `dist/OC32_HAL_Config_Generator.exe` 并复制到 `/home/fro/oc32/windows/bin`。

> 打包脚本通过 `--add-data` 把 `chips/`、`hal/`、`pure_ttk/` 一并打入可执行文件，最终产物无需 Python 环境即可运行。pure_ttk 版不含 customtkinter，产物比 ctk 版更小、启动解包更快。

## 界面说明

### 左侧面板

**Chip Selection（芯片选择）**
- 从 `chips/` 目录自动加载所有 JSON 配置文件，下拉列表显示芯片型号名称（文件名去掉扩展名）。

**System Settings（系统配置）**

| 项 | 说明 | 可选值 / 默认 |
|---|---|---|
| Clock Source | 时钟源，生成 `OC32_CLK_SRC_XXX`；下拉项随芯片 `clk_sources` 动态变化 | IHOSC / ILOSC / XHOSC / XLOSC |
| PLL | 是否启用 PLL，生成 `OC32_CLK_PLL_XXX` | Enable / Disable |
| PLL Freq Over Monitor | PLL 频率过监测，生成 `OC32_CLK_PLL_OM_XXX`；仅 PLL 启用时可操作 | Enable / Disable |
| USB CDR | 启用 USB CDR，生成 `HAL_RCM_CDR_ENABLE` 与 `OC32_CDR_FREQ`；仅当芯片定义了 `cdr_freq` 时可操作 | Enable / Disable |
| System Clock (MHz) | 系统时钟频率（MHz，用户输入，支持小数），生成 `OC32_SCLK_FREQ`；仅 PLL 启用时可用；**超过芯片 `sclk_max` 时自动按上限处理** | 默认 99 |
| XHOSC Frequency (MHz) | 外部高速振荡器频率（MHz，用户输入，支持小数），生成 `OC32_XHOSC_FREQ`；随芯片 `clk_sources` 是否含 XHOSC 启用/禁用 | 默认 4 |
| XLOSC Frequency (kHz) | 外部低速振荡器频率（kHz，用户输入，支持小数），生成 `OC32_XLOSC_FREQ`；随芯片 `clk_sources` 是否含 XLOSC 启用/禁用 | 默认 32.768 |
| Tick Frequency | 系统滴答频率，决定 `HAL_TICK_FREQ_DEFAULT` | 10Hz / 100Hz / 1KHz |

> 说明：系统时钟 SCLK 不再由芯片 JSON 提供，改由 GUI 输入；`System Clock` 与 `PLL Freq Over Monitor` 只有在 **PLL = Enable** 时才可编辑。`XHOSC` / `XLOSC` 输入框与下拉项根据所选芯片的 `clk_sources` 自动启用或禁用。

**Output Directory（输出目录）**
- 配置生成文件的保存路径，默认为用户桌面（`~/Desktop`）。
- 点击 **Browse...** 可选择目录。

**生成按钮**
- **Generate**：根据当前配置生成 HAL 目录。
- **Default**：将所有配置项重置为默认值。
- **Open Output**：用系统文件管理器打开输出目录。

### 右侧面板

**Feature Configure（功能模块配置）**
- 根据所选芯片的 `features` 字段动态生成复选框（`always: true` 的模块强制启用、不出现在列表中）。
- 勾选模块后，对应 `HAL_XXX_ENABLE` 宏被展开；未勾选则以注释形式保留。
- 支持模块依赖关系：勾选某模块时，其 `dependencies` 中列出的模块会被自动启用（例如勾选 `FLASH_VERIFY` 会一并启用 `FLASH` 与 `CRC`）。依赖项在生成时自动补全，无需手动勾选。

**Debug UART（调试串口选项，位于右下角独立区块）**
- 调试串口选项集中在右下角的独立 **Debug UART** 区块中，包含：

| 项 | 说明 | 可选值 / 默认 |
|---|---|---|
| Debug UART | 调试输出串口，决定 `HAL_UART0_DEBUG` / `HAL_UART1_DEBUG` | UART0 / UART1（默认 UART1） |
| Baud Rate | 调试串口波特率，生成 `HAL_UART_DEBUG_BR` | 9600 … 1000000（默认 115200） |
| Parity | 调试串口校验位，生成 `HAL_UART_DEBUG_PARITY` | None / Odd / Even / Always 0 / Always 1 |
| Duplex | 调试串口全/半双工，生成 `HAL_UART_DEBUG_DUPLEX` | Full / Half (use TX pin only) |
| Pins | 调试串口引脚，生成 `HAL_UART_DEBUG_PINS` | TX0 & RX0 / TX1 & RX1 |

- 该区块的启用状态由 **Feature Configure 中的 `DEBUG`（Debug Printf Via UART）功能项控制**，状态随勾选**双向**变化：
  - `DEBUG` 勾选 → 整个 Debug UART 区块**启用**。
  - `DEBUG` 取消勾选 → 区块**禁用并置灰**。
  - 反复切换 DEBUG 时，区块状态始终与勾选保持一致。
  - 切换芯片时，区块默认回到禁用状态，需重新勾选 `DEBUG` 方可启用。

## 芯片配置文件（JSON 格式）

在 `chips/` 目录下新增 JSON 文件即可支持新芯片，无需修改代码。字段说明：

| 字段 | 类型 | 说明 |
|---|---|---|
| `name` | string | 芯片名称，显示在生成的配置头注释中（缺省 `OC32`） |
| `clk_sources` | string[] | 该芯片支持的时钟源子集，取值 `IHOSC`/`ILOSC`/`XHOSC`/`XLOSC`；GUI 据此过滤 Clock Source 下拉项并启用/禁用 XHOSC、XLOSC 输入框（缺省视为全部四种） |
| `ilosc_real` | bool | ILOSC 为真实晶振时 `true`，生成 `OC32_CLK_ILOSC_REAL`；否则生成 `OC32_CLK_ILOSC_FAKE`（缺省 `false`）；仅当 `clk_sources` 含 ILOSC 时输出该宏 |
| `ihosc_freq` | int | 内部高速振荡器频率（**Hz**），用于 `OC32_IHOSC_FREQ`（缺省 24000000） |
| `ilosc_freq` | int | 内部低速振荡器频率（Hz），用于 `OC32_ILOSC_FREQ`（缺省 4000） |
| `cdr_freq` | int | USB CDR 输出频率（Hz）。**可选字段**：仅当存在且 GUI 中 **USB CDR = Enable** 时，生成 `HAL_RCM_CDR_ENABLE` 与 `OC32_CDR_FREQ`；缺省不输出 CDR 相关宏 |
| `flash_size` | int | Flash 总容量（KB） |
| `zone0_size` | int | Boot 区大小（KB） |
| `zone1_size` | int | User 区大小（KB） |
| `int_priority` | int | 中断优先级级别数，生成 `OC32_HIC_PRI0..PRI(n-1)` |
| `dma_channels` | int | DMA 通道数，生成 `OC32_DMA_CH0..CH(n-1)` |
| `dma_map` | string[] | 各 DMA 通道映射的外设名，按索引生成 `OC32_DMA_XXX`；`reservedN` 表示未使用 |
| `hic_map` | string[] | 各 HIC（中断）通道映射的外设名，按索引生成 `OC32_HIC_CH_XXX` 与位掩码 `OC32_HIC_XXX` |
| `vdd_value` | int | VDD 电压值（mV） |
| `features` | object[] | 可选功能模块列表，见下 |

> 注意：系统时钟 SCLK 由 GUI 输入（`System Clock (MHz)`），不再从芯片 JSON 读取，因此 JSON 中**不需要** `sclk_freq` 字段。

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
  "clk_sources": ["IHOSC", "ILOSC", "XHOSC"],
  "ilosc_real": false,
  "ihosc_freq": 24000000,
  "ilosc_freq": 4000,
  "cdr_freq": 12000000,
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
    │   ├── oc32_hal_rcm.h / oc32_hal_ccp.h / oc32_hal_uart.h ...
    ├── src/
    │   ├── oc32_hal.c
    │   ├── oc32_hal_flash.c
    │   └── ...（其余 src/ 源文件，含 oc32_hal_int.c / .S）
    └── chip/
        ├── CV32S6015MSR.h
        └── CV32S6015SR.h
```

生成逻辑要点：
- `oc32_hal_conf.h` 由 `hal/oc32_hal_conf_template.h` 渲染而来，替换所有 `__占位符__`（时钟源、PLL、ILOSC 类型、SCLK/XHOSC/XLOSC、CDR、Flash、DMA、HIC、中断优先级、Tick、VDD、UART 调试、模块启用等）。
- UART 调试相关宏（`HAL_UART_DEBUG_ENABLE`、`HAL_UART_DEBUG_BR`、`HAL_UART0/1_DEBUG`、`HAL_UART_DEBUG_PARITY/DUPLEX/PINS`）作为一个整体块由模板中的 `__HAL_CONFIG_UART_DEBUG_BLOCK__` 占位符渲染：勾选 **UART** 功能时整块生成并保留在 `#ifdef HAL_UART_DEBUG_ENABLE ... #endif` 内；未勾选时整块被替换为空，不会留下空的 `#ifdef` 残块或未展开的占位符。
- 其余 `inc/`、`src/`、`chip/` 目录内容原样复制，跳过已生成的 `oc32_hal_conf.h`。
- 未勾选的可选模块与未启用的 CDR/UART 调试开关，以注释形式保留（如 `/* #define HAL_XXX_ENABLE */`），便于后续手动启用。
