# OC32 HAL 用户手册

**OC32 HAL** 是面向 CV32S6015 系列 MCU 的硬件抽象层（HAL）驱动库。
本文档面向**应用开发者**，介绍如何基于 HAL 库开发程序。

---

## 目录

1. [概述](#1-概述)
2. [快速开始](#2-快速开始)
3. [系统初始化](#3-系统初始化)
4. [时钟管理 (RCM)](#4-时钟管理-rcm)
5. [GPIO](#5-gpio)
6. [UART](#6-uart)
7. [SPI](#7-spi)
8. [Timer](#8-timer)
9. [CCP](#9-ccp)
10. [WDT](#10-wdt)
11. [Flash](#11-flash)
12. [CRC](#12-crc)
13. [LVD](#13-lvd)
14. [中断管理 (HIC)](#14-中断管理-hic)
15. [DMA](#15-dma)
16. [调试输出](#16-调试输出)
17. [错误码与返回类型](#17-错误码与返回类型)
18. [注意事项](#18-注意事项)

---

## 1. 概述

HAL 库提供以下外设的驱动函数：

| 模块 | 头文件 | 说明 |
|------|--------|------|
| 系统 | `oc32_hal.h` / `oc32_hal.c` | 系统初始化、DMA、Fuse 数据 |
| 时钟 (RCM) | `oc32_hal_rcm.h` / `oc32_hal_rcm.c` | 时钟源选择、PLL 配置 |
| GPIO | `oc32_hal_gpio.h` / `oc32_hal_gpio.c` | 引脚输入/输出/中断 |
| UART | `oc32_hal_uart.h` / `oc32_hal_uart.c` | 串口收发（轮询/DMA/中断） |
| SPI | `oc32_hal_spi.h` / `oc32_hal_spi.c` | SPI 收发（单次/包/DMA/中断） |
| Timer | `oc32_hal_timer.h` / `oc32_hal_timer.c` | 8 路定时器（计数/编码/复位/暂停/事件） |
| CCP | `oc32_hal_ccp.h` / `oc32_hal_ccp.c` | 6 路捕获/比较/PWM/门控/强制 |
| WDT | `oc32_hal_wdt.h` / `oc32_hal_wdt.c` | 2 路看门狗 |
| Flash | `oc32_hal_flash.h` / `oc32_hal_flash.c` | SPI Flash 读写/擦除/CRC 校验 |
| CRC | `oc32_hal_crc.h` / `oc32_hal_crc.c` | 硬件 CRC 计算 |
| LVD | `oc32_hal_lvd.h` / `oc32_hal_lvd.c` | 低压检测 |
| 中断 (HIC) | `oc32_hal.c` (HIC 部分) | 硬件中断控制器配置 |

**约定**：
- 所有公共 API 返回 `HAL_StatusTypeDef`（`HAL_OK` / `HAL_ERROR` / `HAL_TIMEOUT`）或 `HAL_ResultTypedef`（携带数据 + 状态）。
- 部分"运行时"操作 API 标记 `__sram__`（放入 `.stext` 段，SRAM 执行）。
- 配置头 `oc32_hal_conf.h` 由生成器（`OC32 HAL Config Generator`）根据芯片型号自动产生，用户一般不需手改。

---

## 2. 快速开始

最小可运行程序（使用 UART1 输出，轮询模式）：

```c
#include "oc32_hal.h"

int main(void)
{
    /* 系统初始化（时钟 + 校验 + 调试 UART + tick） */
    if (HAL_SYS_Init() != HAL_OK)
        while (1) ;

    /* 使能中断控制器（WDT tick 位也在此置位，系统 tick 从这里开始运行） */
    HAL_HIC_InitTypeDef hic = {
        .ChEnable = OC32_HIC_UART1,
        .ChPriorityBit0 = 0,
        .ChPriorityBit1 = 0,
    };
    HAL_HIC_Init(&hic);

    /* 初始化 UART1 为 115200 8N1 全双工 */
    HAL_UART_InitTypeDef uart = {
        .Uart = UART_1,
        .Parity = UART_PAR_ODD,
        .Bits = UART_8_BIT,
        .Baud = UART_BAUD_115200,
        .Duplex = UART_FULL_DUPLEX,
        .Pins = UART_PINS_0,
        .DmaCh = OC32_DMA_CH0,
    };
    HAL_UART_Init(&uart);

    const char *msg = "Hello OC32\r\n";
    for (size_t i = 0; msg[i]; i++)
    {
        uint32_t byte = (uint8_t)msg[i];
        HAL_UART_PacketTypedef upkt = { .Data = &byte, .Length = 1U, .IntE = HAL_DISABLE };
        HAL_UART_TX(UART_1, &upkt);
    }

    while (1) ;
}
```

> 注意：`HAL_SYS_Init` 默认会初始化调试 UART（由 `oc32_hal_conf.h` 的 `HAL_UART_DEBUG_*` 决定），若该 UART 与业务使用冲突，可在 conf 中关闭。

---

## 3. 系统初始化

### `HAL_SYS_Init`
初始化整个系统：时钟（`HAL_RCM_Init`）→ Flash 校验（可选）→ 调试 UART（可选）→ tick 定时器。
返回 `HAL_ERROR` 时系统时钟或某一步失败，程序无法正常运行。

### `HAL_TICK_Init`
配置 WDT1 为 1 kHz 系统 tick（`HAL_TICK_FREQ_DEFAULT`），`uwTick` 每毫秒递增。
**注意**：`HAL_TICK_Init` 仅配置 WDT tick 参数，不会使能 WDT 中断位；tick 只有在用户调用 `HAL_HIC_Init` 之后才真正运行，在此之前 `uwTick` 恒为 0、`HAL_Delay` 不推进。

### `HAL_HIC_Init` / `HAL_HIC_DeInit`
配置硬件中断控制器（HIE / HIPL0 / HIPL1 / HIPRM）。
- `ChEnable`：使能各中断通道位（`OC32_HIC_UART0 | OC32_HIC_SPI0 | ...`）。
- `ChPriorityBit0/1`：各通道优先级（`HPL_Bit0/Bits1(OC32_HIC_PRI0..3, ch)`）。
- WDT 通道（vector 17）**始终被使能**作为 tick 定时器，不参与 `ChEnable` 控制。

> **重要**：`HAL_HIC_DeInit` 会清 HIE 全部位，包括 WDT 位——调用后系统 tick 失效，`HAL_Delay` 无法推进。若需在 DeInit 后恢复 tick，须重新调用 `HAL_HIC_Init`。
> `HAL_FLASH_Init` 内部调用 `HAL_HIC_DeInit`（关闭全部中断）；完成 flash 初始化后如需 tick，请重新 `HAL_HIC_Init`。

---

## 4. 时钟管理 (RCM)

### `HAL_RCM_Init`
按 `oc32_hal_conf.h` 中定义的时钟源（`OC32_CLK_SRC_*`）和 PLL 配置系统时钟：
1. 降速 Flash 时钟（DIV5）防止切换时取指失败；
2. 选择参考振荡器（XHOSC / XLOSC / ILOSC / CDR）；
3. 若使能 PLL，计算最优分频系数，等待切换完成；
4. 更新 Flash 时钟分频。

失败时自动回退到 `HAL_RCM_DeInit`（IHOSC 默认时钟）。

### `HAL_RCM_DeInit`
恢复所有时钟寄存器到复位默认值（IHOSC + 默认 Flash 分频）。

### `HAL_RCM_GetSysClockFreq`
返回当前系统时钟频率（Hz），存储于全局 `SystemClock`。

### PLL 中断（`OC32_CLK_PLL_OM_ENABLE` 时）
若使能了 PLL 频率过冲监视（`PFOM`），过冲时触发 `HAL_PLL_IRQHandler`，
该处理函数自动回退到默认时钟并清除 PFO 标志。

---

## 5. GPIO

### 初始化

```c
HAL_GPIO_InitTypeDef gpio = {
    .Port = PORT_A,
    .Pin = PIN3,
    .Direction = GPIO_OUTPUT,
    .State = GPIO_STATE_HI,
    .Slew = GPIO_SLEW_HI,
};
HAL_GPIO_Init(&gpio);
```

- 输出模式：设引脚电平 + 摆率，拉低上拉，使能 OE。
- 输入模式：配置上拉、滤波（LV0~LV2）、中断边沿（RIE/FIE），清除历史标志。

### 引脚电平
```c
PA3 = 1;   /* 单独某pin置高 */
PA3 = 0;  /* 单独某pin置低 */
WRITE_SR(PA, 0); /* 写入整组端口 A */
```

### 输入读取
```c
uint32_t port_val = READ_SR(PB);   /* 读取整组端口 B */
uint32_t pin_val = PB3;   /* 读取引脚 B3 */
```

### 中断
配置 `.ExInt`（`GPIO_EI_RISE` / `GPIO_EI_FALL` / `GPIO_EI_BOTH`）后，
边沿触发中断由 `__int30`（PORTA）/ `__int31`（PORTB）向量处理。


---

## 6. UART

UART0 和 UART1 共用同一套 API，通过 `MODULE_UART_TypeDef *` 区分。

### 初始化
```c
HAL_UART_InitTypeDef uart = {
    .Uart = UART_0,
    .Parity = UART_PAR_EVEN,
    .Bits = UART_8_BIT_PAR,
    .Baud = UART_BAUD_115200,
    .Duplex = UART_FULL_DUPLEX,
    .Pins = UART_PINS_0,
    .DmaCh = OC32_DMA_CH0,
};
HAL_UART_Init(&uart);
```

- `Baud`：从 `HAL_UART_BaudTypedef` 枚举选择（9600~1000000）。
- 半双工模式（`UART_HALF_DUPLEX`）下 TX/RX 共享同一引脚，收发前需等待对方方向完成。

### 单字节收发（阻塞）
```c
HAL_UART_PacketTypedef pkt = {
    .Data = &one_byte,   /* uint32_t*；低 8 位为发送/接收字节 */
    .Length = 1U,        /* 字节数 */
    .IntE = HAL_DISABLE,
};
HAL_UART_TX(UART_0, &pkt);
HAL_UART_RX(UART_0, &pkt);   /* 结果读回 Packet->Data（或经 SPIRBUF/UTRBUF 语义） */
```

- 无 DMA、无中断，调用者同步阻塞直至收发完成。
- `Length` 单位为**字节**；`Length ≤ 4` 走逐字（≤4 B 的寄存器写）快路径，`Length > 4` 自动启用 DMA。

### 数据包收发（DMA / 中断）
```c
HAL_UART_PacketTypedef pkt = {
    .Data = tx_buf,
    .Length = 128,               /* 字节数 */
    .IntE = HAL_DISABLE,      /* 轮询模式 */
    /* 或 IntE = HAL_ENABLE 使用中断模式 */
};
HAL_UART_TX(UART_0, &pkt);
```

- **轮询模式**（`IntE = OFF`）：函数阻塞直至 DMA 完成。
- **中断模式**（`IntE = ON`）：函数立即返回，DMA 完成后触发 `__int20`（UART0）/ `__int21`（UART1）向量，最终调用 `HAL_UART_IRQHandler`，可在 `HAL_UART_UserIRQ` 中处理用户逻辑。
- 数据包长度单位为**字节**。

### `HAL_UART_UserIRQ`
弱符号，用户可在自己的代码中覆盖以处理 UART 中断。

---

## 7. SPI

SPI0 和 SPI1 共用同一套 API。

### 初始化
```c
HAL_SPI_InitTypeDef spi = {
    .Spi = SPI_0,
    .Mode = SPI_CIL_DSS,       /* 时钟空闲低，采样上升沿（Mode 0） */
    .ClkDiv = 4,               /* 分频系数（SPIMCD[3:0]） */
    .Slave = SPI_MASTER,
    .CsOut = SPI_CS_OUT_ON,
    .Duplex = SPI_FULL_DUPLEX,
    .HalfPin = SPI_HALF_PIN_SEPARATE,
    .FirstBit = SPI_MSB_FIRST,
    .Pins = SPI_PINS_0,
    .DmaCh = OC32_DMA_CH0,
};
HAL_SPI_Init(&spi);
```

- `Mode` 枚举对应 SPI 四种时钟/数据相位：
  - `SPI_CIL_DSS`（Mode 0）：CPOL=0, CPHA=0
  - `SPI_CIL_DSF`（Mode 1）：CPOL=0, CPHA=1
  - `SPI_CIH_DSS`（Mode 2）：CPOL=1, CPHA=0
  - `SPI_CIH_DSF`（Mode 3）：CPOL=1, CPHA=1

### 单次收发（阻塞，无 DMA）
```c
HAL_SPI_PacketTypedef pkt = {
    .HalfRx = SPI_HALF_TX,
    .TxData = &tx_word,
    .RxData = NULL,          /* 可 NULL */
    .Length = 4,             /* 字节数 */
    .IntE = HAL_DISABLE,
};
HAL_SPI_Transfer(SPI_0, &pkt);
```

- 全双工：同时设置 TxData 和 RxData。
- 半双工纯 TX（`SPI_HALF_TX`）：RxData 可为 NULL。
- 半双工纯 RX（`SPI_HALF_RX`）：TxData 可为 NULL。
- `Length ≤ 4` 走寄存器快路径（不启用 DMA），`Length > 4` 自动启用 DMA。

### 数据包收发（DMA / 中断）
```c
HAL_SPI_PacketTypedef pkt = {
    .HalfRx = SPI_HALF_TX,
    .TxData = tx_buf,
    .RxData = rx_buf,
    .Length = 128,           /* 字节数 */
    .IntE = HAL_ENABLE,   /* 中断模式 */
};
HAL_SPI_Transfer(SPI_0, &pkt);
```

- 中断完成后由 `__int22`（SPI0）/ `__int23`（SPI1）触发 `HAL_SPI_IRQHandler`。
- 轮询模式阻塞直至 DMA 完成。
- **DMA 管理说明**：`HAL_SPI_Transfer` 内部在 `Length > 4` 时统一通过 `HAL_DMA_Update` 管理 DMA 通道；`Length ≤ 4` 走纯寄存器路径，不涉及 DMA。

### `HAL_SPI_UserIRQ`
弱符号，用户可覆盖以处理 SPI 中断。


---

## 8. Timer

共 8 路定时器（`TIMER_0` ~ `TIMER_7`），支持以下模式：

| 模式 | Init 函数 | 说明 |
|------|-----------|------|
| 计数（基础） | `HAL_TIMER_Init` | 上/下/翻转/编码计数 |
| 编码 | `HAL_TIMER_EncodeInit` | 2 路编码输入 |
| 复位事件 | `HAL_TIMER_ResetInit` | 外部复位源 |
| 暂停 | `HAL_TIMER_PauseInit` | 暂停事件 |
| 事件 | `HAL_TIMER_EventInit` | 触发事件（如 ADC 触发） |

### 基础计数初始化示例
```c
HAL_TIMER_BaseInitTypeDef timer = {
    .Timer = TIMER_0,
    .ReloadVal = 0xFFFF,
    .PreScaler = 0,
    .CountVal = 0,
    .IntE = HAL_ENABLE,
    .CountMode = TIMER_COUNT_UP,
    .StopCon = TIMER_STOP_OVER,
    .OvfMode = TIMER_OVF_OVER,
    .CountSource = TIMER_SOURCE_SCLK,
    .CountExCon = TIMER_EXCON_RISE,
    .UpdateMode = TIMER_UPDATE_DIR,
    .ReadReg = TIMER_READ_REG_BUF,
    .AdcTrig = TIMER_ADC_TRIG_NONE,
};
HAL_TIMER_Init(&timer);
```

### 运行时操作
```c
/* 等待溢出 */
HAL_TIMER_WaitOv(TIMER_0);

/* 更新重载值/计数器, 实际更新时机根据UpdateMode设置而定 */
HAL_TIMER_UpdateTypeDef upd = { .Timer = TIMER_0, .ReloadVal = 0x1000, .PreScaler = 0, .CountVal = 0 };
HAL_TIMER_Update(&upd);

/* 立即更新计数器值 */
HAL_TIMER_UpdateNow(&upd);

/* 立即复位 */
HAL_TIMER_ResetNow(TIMER_0);

/* 触发事件 */
HAL_TIMER_EventTrig(TIMER_0);
```

### 弱符号
`HAL_TIMER_UserIRQ`、`HAL_TIMER_WaitOv`、`HAL_TIMER_WaitEv`、`HAL_TIMER_EventTrig`、`HAL_TIMER_UpdateNow`、`HAL_TIMER_ResetNow` 均为弱符号，用户可覆盖。

---

## 9. CCP

共 6 路 CCP（`CCP_0` ~ `CCP_5`），支持以下模式：

| 模式 | Init 函数 | 说明 |
|------|-----------|------|
| 基础 | `HAL_CCP_Init` | 选择计时器、更新模式 |
| 捕获 | `HAL_CCP_CaptureInit` | 边沿捕获 |
| 比较 | `HAL_CCP_CompareInit` | 比较输出 |
| PWM | `HAL_CCP_PwmInit` | 边沿/中心对齐 PWM |
| 门控 | `HAL_CCP_GateInit` | 外部门控计数 |
| 强制 | `HAL_CCP_ForceInit` | 强制输出状态 |

### PWM 示例
```c
HAL_CCP_PwmInitTypeDef ccp = {
    .CCP = CCP_0,
    .Duty = 0x0800,          /* 16 位占空比 */
    .DeadTime = 0,
    .PwmCon = CCP_PWM_FLAG_RISE,
    .PosOutE = CCP_OUTPUT_ON,
    .PosOutInv = CCP_OUTPUT_INV_OFF,
    .NegOutE = CCP_OUTPUT_ON,
    .NegOutInv = CCP_OUTPUT_INV_OFF,
};
HAL_CCP_Init(&base);   /* 先初始化基础模式 */
HAL_CCP_PwmInit(&ccp);
```

### 等待标志（阻塞）
```c
HAL_CCP_WaitCf(CCP_0);   /* 等待捕获/比较/PWM 标志 */
HAL_CCP_WaitGf(CCP_0);   /* 等待门控标志 */
HAL_CCP_WaitFf(CCP_0);   /* 等待强制标志 */
```

### 运行时更新
```c
HAL_CCP_UpdateTypeDef upd = { .CCP = CCP_0, .Duty = 0x2000, .GateStart = 0, .GateEnd = 0 };
HAL_CCP_UpdateNow(&upd);
```

---

## 10. WDT

共 2 路看门狗：`WDT_0`（业务用）和 `WDT_1`（系统 tick，由 `HAL_TICK_Init` 管理）。

### 初始化
```c
HAL_WDT_InitTypeDef wdt = {
    .Wdt = WDT_0,
    .Mode = WDT_MODE_RST,        /* 溢出时复位 */
    .Period = 1000,              /* 溢出周期（us） */
};
HAL_WDT_Init(&wdt);
```

- 自动选择满足 8-bit 重载值约束的最快预分频器。
- `Period` 必须在对应时钟档位的有效范围内（`WDT_H_PERIOD_MIN`/`MAX` 或 `WDT_L_PERIOD_MIN`/`MAX`，单位为 us）。范围边界各外扩 1 us（`WDT_PERIOD_LIMIT_MARGIN`）以避免 8-bit 重载值取整为 0。

### 喂狗
```c
HAL_WDT_Clear(WDT_0);   /* 必须在 Period 内调用，否则溢出 */
```

### 模式说明
| 模式 | 行为 |
|------|------|
| `WDT_MODE_RST` | 溢出时系统复位 |
| `WDT_MODE_INT` | 溢出时触发 `__int17` 中断 |
| `WDT_MODE_WAKE` | 溢出时唤醒（从低功耗） |
| `WDT_MODE_INT_AND_WAKE` | 中断 + 唤醒 |

---

## 11. Flash

`HAL_FLASH_*` 操作 SPI Flash（通过 flashloader 外设 + DMA）。

### 初始化
`HAL_FLASH_Init` 在 `HAL_SYS_Init` 中自动调用，无需手动。

### 读
```c
HAL_Flash_PacketTypeDef pkt = {
    .Data = buf,
    .Address = 0x001000,
    .Length = 256,   /* 字节数，必须是 16 的倍数才走 DMA 快读 */
    .DmaCh = OC32_DMA_CH0,
};
HAL_FLASH_Read(&pkt);
```

- 满足条件时（Length 是 16 的倍数、buf 4 字节对齐、lines 为 2 的幂次）自动走 DMA 快读，否则逐字读。

### 写（编程）
```c
HAL_Flash_PacketTypeDef pkt = {
    .Data = &word,
    .Address = 0x002000,
    .Length = 1,
    .DmaCh = OC32_DMA_CH0,
};
HAL_FLASH_Program(&pkt);
```
- 所有数据必须在同一 256 字节页内，跨页返回 `HAL_ERROR`。
- `Data` 为 `uint32_t*`（word 数组）；数据流按 **LSB-first 字节序**传输，完整的 4B word 用快路径，余下的 1..3 字节逐字节补发。

### 擦除
```c
HAL_Flash_PacketTypeDef pkt = {
    .Data = NULL,
    .Address = 0x002000,
    .Length = FLASH_PAGE_SIZE,   /* 256 字节，页对齐 */
    .DmaCh = OC32_DMA_CH0,
};
HAL_FLASH_Erase(&pkt);
```
- 地址和长度必须页对齐（256 B），自动选择 4 KB 扇区擦除或 256 B 页擦除。

### CRC 校验与自动恢复
```c
/* 生成器使能 HAL_FLASH_VERIFY_ENABLE 时可用 */
HAL_FLASH_VerifyCode();   /* 校验 2 bank × 2 zone（Boot/User），出错时自动从好 bank 复制 */
```
- 需要 ~1 KB 静态 buffer，**调用时不得运行其他 DMA/CRC 操作**。

---

## 12. CRC

硬件 CRC 支持 5/8/16/32 位多项式，输入/输出 bit 反转，补码。

### 初始化
```c
HAL_CRC_InitTypeDef crc = {
    .Poly = 0x04C11DB7U,
    .InitVal = 0xFFFFFFFFU,
    .Bits = CRC_32_BIT,
    .RefIn = 1U,
    .RefOut = 1U,
    .CplOut = 1U,
    .Rvb = 0,
    .DmaCh = OC32_DMA_CH0,
};
HAL_CRC_Init(&crc);
```

### 单值计算（阻塞）
```c
uint32_t crc_byte  = HAL_CRC_OneByte(0x12);
uint32_t crc_word  = HAL_CRC_OneWord(0x1234);
```

### 包计算
```c
HAL_CRC_PacketTypedef pkt = {
    .Data = buf,
    .Length = 1024,
    .IntE = HAL_DISABLE,
    .IDM = CRC_IDM_READ_DMA,   /* DMA 读取 */
};
HAL_ResultTypedef r = HAL_CRC_Packet(&pkt);
uint32_t crc32 = r.Data;
```

- `CRC_IDM_READ_DMA`：CRC 主动发起 DMA 读，通道由 DMA control 指定（`OC32_DMA_CRC` 角色），`CRCDA` 给源地址，结果经 `r.Data` 返回。**不用 `CRCPDC`**。
- `CRC_IDM_WRITE_CRCIN`：写入模式（供 Flashloader 等内部使用）。
- `CRC_IDM_CAPTURE_DMAIN/OUT`：CRC 被动监听 DMA 通道，通过 `CRCPDC` 指定监听目标通道。

---

## 13. LVD

低压检测，电压低于阈值时触发告警。

### 初始化
```c
HAL_LVD_InitTypeDef lvd = {
    .Mode = LVD_MODE_INT,
    .TrigV = LVD_TRIG_V2800,   /* 2.8V 触发 */
};
HAL_LVD_Init(&lvd);
```

- 触发后由 `__int0` 向量（HRF）处理，自动调用 `HAL_LVD_IRQHandler`。


---

## 14. 中断管理 (HIC)

OC32 使用**硬件中断控制器（HIC）**，而非传统 NVIC。中断向量 0~31 固定映射到各外设（见 `oc32_hal_int.c`）：

| 向量 | 外设 |
|------|------|
| 0 | HRF（POR/LVD/PLL 错误） |
| 1~8 | CMP0/CMP1、CCP0~5 |
| 9~16 | TIMER0~7 |
| 17 | WDT0/WDT1 |
| 18~19 | ADC0/ADC1 |
| 20~21 | UART0/UART1 |
| 22~23 | SPI0/SPI1 |
| 24 | IIC |
| 25~27 | MPWM0~2 |
| 28 | DAC |
| 29 | Flash（恒关）/ CRC |
| 30~31 | PORTA / PORTB |

### 配置步骤
1. `HAL_HIC_Init` 中通过 `ChEnable` 使能需要的中断通道位（`OC32_HIC_*`）；
2. 各外设自身的 `IntE` 标志（如 `Packet->IntE`、`Init->IntE`）决定是否让外设产生中断；
3. 外设 IRQ 处理函数（如 `HAL_SPI_IRQHandler`）由对应向量自动调用，用户逻辑写在 `HAL_XXX_UserIRQ`（弱符号）中。

### 抢占
`HIPRM` 寄存器允许高优先级中断抢占当前中断。`oc32_hal_int.c` 中各 handler 已保存/恢复 `EIA`（返回地址）以支持抢占。

> **注意**：向量 1/2（CMP0/1）、18/19（ADC0/1）、25~27（MPWM0~2）、28（DAC）的 `__int` 体目前为骨架占位（`HAL_*_IRQHandler` 调用槽保留但 handler 尚未实现），这些模块的硬件中断通路尚未在 HAL 中完整落地。

---

## 15. DMA

共 4 路 DMA 通道（`OC32_DMA_CH0~3`），通过 `DMAChState[]` 全局数组跟踪各通道占用状态。

- 各模块（UART/SPI/CRC/Flash）的 `DmaCh` 字段指定使用哪个 DMA 通道。
- `HAL_DMA_Update` 在传输前将通道映射到目标模块（`OC32_DMA_*`），传输结束后置为 `OC32_DMA_IDLE`。
- 避免让多个模块共享同一 DMA 通道同时传输，否则会导致数据错乱。

> 注意：`FLASH`、`CRC` 等内部操作默认占用 `OC32_DMA_CH0`，业务模块使用时需避开冲突。

---

## 16. 调试输出

`oc32_hal_conf.h` 中 `HAL_UART_DEBUG_ENABLE` 使能后，提供轻量级 `puts` / `printf`（weak）：

```c
puts("boot ok\r\n");
printf("sclk=%u crc=%x\r\n", SystemClock, crc32);
```

- 支持的格式符：`%d %u %x %X %s %c %p %%`。
- 输出到 `oc32_hal_conf.h` 指定的调试 UART（`HAL_UART0_DEBUG` / `HAL_UART1_DEBUG`）。
- 仅支持**非负整数与 32 位值**，无浮点、无字符串长度检查（`%s` 假定 `\0` 结尾）。

---

## 17. 错误码与返回类型

```c
typedef enum {
    HAL_OK      = 0x00U,
    HAL_ERROR   = 0x01U,
    HAL_BUSY    = 0x02U,
    HAL_TIMEOUT = 0x03U
} HAL_StatusTypeDef;

typedef struct {
    uint32_t Data;           /* 数据 */
    HAL_StatusTypeDef Error; /* 状态 */
} HAL_ResultTypedef;
```

| 返回类型 | 使用场景 |
|----------|----------|
| `HAL_StatusTypeDef` | 纯操作类 API（Init/DeInit/Transfer/Clear...） |
| `HAL_ResultTypedef` | 需返回数据的 API（`CRC_Packet` 等） |

---

## 18. 注意事项

1. **不要改动 `hal/inc/` 与 `hal/src/` 的核心逻辑**，业务逻辑放在应用层；需扩展时用 `HAL_XXX_UserIRQ` 等弱符号覆盖。
2. **阻塞类 API**（`WaitOv`/`WaitEv`/轮询 `HAL_UART_TX`/`HAL_UART_RX`/`HAL_SPI_Transfer`）在调用期间**必须保证对应外设中断不被屏蔽**，否则 busy-wait 无法结束。
3. **中断模式**的 `HAL_UART_TX`/`HAL_UART_RX`/`HAL_SPI_Transfer`（`IntE = ON`）立即返回，需通过 `HAL_XXX_UserIRQ` 获取完成通知；阻塞类单字节 API 无中断模式。
4. **Length 单位**：整个 MCU 环境默认按**字节**计数。`UART`/`SPI`/`Flash`/`CRC` 的 `Packet.Length` 均为**字节**。
5. **DMA 通道独占**：同一通道不要并发服务两个模块；`FLASH`/`CRC` 内部默认占 CH0。
6. **`HAL_FLASH_VerifyCode`** 会覆盖 DMA/CRC 配置且需 1KB buffer，调用时不得运行其他 DMA/CRC 操作。
7. **`SystemClock`** 全局变量在 `HAL_RCM_Init` 后有效，`HAL_FLASH_ClkUpdate` 依赖它调整 Flash 分频。
8. **时钟源/Flash 分区/模块使能**由 `oc32_hal_conf.h`（生成器产出）决定，切换芯片时重新生成而非手改。

---

*文档版本与 HAL 源码同步维护。若 API 有变更，请同步更新本手册。*
