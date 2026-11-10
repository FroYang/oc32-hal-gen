# OC32 HAL User Manual

## 1. Introduction

The OC32 HAL (Hardware Abstraction Layer) is the driver library for the
CV32S6015 series of 32-bit RISC architecture (OC32 ISA) MCU. It provides a register-independent,
portable C API over the chip's peripherals, so application code does not
touch device-header macros directly.

Layered design:

- Application layer: user code, operating system or RTOS.
- HAL driver layer (this manual): per-peripheral Init/DeInit/transfer
  functions, interrupt handlers, DMA integration, tick service.
- Device layer: chip register definitions (`CV32S6015SR.h` /
  `CV32S6015MSR.h`), memory map, `__sram__` placement.

Scope of this manual: GPIO, RCM, UART, SPI, IIC, TIMER, CCP, WDT, FLASH,
CRC, LVD drivers, the HIC interrupt service layer, and the generic
HAL type system. Each peripheral chapter follows a fixed template:
features, configuration structures, handle (if any), how to use, and a
five-part API description for every public function. No code blocks are
used; API names are referenced inline in numbered steps.

## 2. Acronyms and definitions

- HAL: Hardware Abstraction Layer.
- RCM: Reset and Clock Manager.
- HIC: Hardware Interrupt Controller.
- CCP: Capture/Compare/PWM unit.
- WDT: Watchdog Timer.
- LVD: Low-Voltage Detector.
- VTS: Voltage (or thermal) sensor block.
- IHOSC/ILOSC: Internal High/Low-speed Oscillator.
- XHOSC/XLOSC: External High/Low-speed Oscillator.
- PLLE: PLL Enable. CSF: Clock Switch Flag.
- EIA: Exception/Interrupt Accumulator register (PC shadow, alias R1).
- HIPRM: Higher-priority interrupt request mask (preemption enable).
- SRAM attribute `__sram__`: place function in `.stext` (SRAM) for
  low-latency interrupt fetch.
- `__weak__`: weakly-linked symbol, overridable by user code.

## 3. General information

HAL drivers are organized one header and one source file per peripheral,
all pulled in by `oc32_hal.h`. `oc32_hal_conf.h` holds user configuration:
clock frequencies, flash layout, DMA channel map, HIC channel map,
priority levels, and per-module enable macros (`HAL_UART_ENABLE` and so
on). Modules whose enable macro is undefined disappear from compilation.

Design principles:

- Single configuration structure per operation mode; a dedicated Init
  struct is filled once and passed to `HAL_X_Init`.
- No mutex handle at present; `HAL_LockTypeDef` is reserved for the lock
  field pattern. Per-operation status is returned by `HAL_StatusTypeDef`.
- Three transfer modes where the hardware supports them: polling
  (blocking wait loops), interrupt (`_UserIRQ` / `_IRQHandler`), and
  DMA (channel specified inside the Init struct via `HAL_DMA_ChTypedef`).
- `__weak__` handlers may be overridden by the user; `__sram__` marks
  ISR-resident functions.

API classification:

- Initialization / de-initialization: `HAL_X_Init`, `HAL_X_DeInit`.
- Operation: `HAL_X_TX`, `HAL_X_RX`, `HAL_X_Transfer`, `HAL_X_UpdateNow`.
- Waits: `HAL_X_WaitOv`, `HAL_X_WaitEv`, `HAL_X_WaitCf`, `HAL_X_WaitGf`,
  `HAL_X_WaitFf`, `HAL_X_WaitBusy` style busy loops.
- Interrupt service: `HAL_X_IRQHandler`, overridable
  `HAL_X_UserIRQ`.
- Query: `HAL_X_GetFreq`, `HAL_X_CaptureValue`, `HAL_GetHalVer`.

Compliance: strict ANSI C (C90 basis with designated extensions),
`static inline` helpers where the header needs them, MISRA-C 2004 basic
rules observed.

## 4. Overview of HAL drivers

### 4.1 Common types

All status-returning functions use `HAL_StatusTypeDef` (HAL_OK,
HAL_ERROR, HAL_BUSY, HAL_TIMEOUT). `HAL_ResultTypedef` couples a data
word with its status for CRC packet transfers. `HAL_EnableTypeDef` and
`HAL_LockTypeDef` model enable state and locking. `HAL_EnableTypeDef`
(ModuleIntE) selects on/off for interrupt-driven operations inside every
Init struct.

### 4.2 Lifetime

Typical sequence per peripheral: `HAL_X_Init` (writes the control
registers from the Init struct), normal operation via the operation APIs,
then `HAL_X_DeInit` (resets the module to power-on values). The system
service `HAL_SYS_Init` (weakly linked, overridable) orchestrates the
recommended startup order: RCM Init, HIC Init, TICK Init, and any
peripheral inits needed by the application.

### 4.3 Error model

`HAL_ERROR` means an illegal argument (NULL pointer, unsupported value,
period outside the WDT bounds). `HAL_TIMEOUT` is returned when a wait
loop exceeds its iteration limit (for example `RCM_CSWAIT_LOOP_MAX`).
`HAL_BUSY` means the requested operation cannot start while a previous
one is in progress. Functions that transfer data additionally report
via `HAL_ResultTypedef.Error`.

### 4.4 Clock dependencies

Every module requires RCM setup first. `HAL_RCM_Init` selects the system
clock source and starts the PLL, and `HAL_RCM_GetSysClockFreq` returns
the resulting SCLK in Hz. Per-block clock gating macros exist for FLASH,
ADC, DAC, USB, CRC under the `__HAL_RCM_*_CLK_ENABLE` family; other
peripherals are always clocked by SCLK.

## 5. GPIO driver module

### 5.1 GPIO features

General-purpose input/output ports. The driver supports pin-by-pin
direction, output state and slew control, input pull-up/pull-down,
external-interrupt sense (none/level/rising/falling/both), and input
filter level. This driver supports:

- Direction select: `GPIO_INPUT`, `GPIO_OUTPUT`
- Output state: `GPIO_STATE_LO`, `GPIO_STATE_HI`
- Slew rate: `GPIO_SLEW_HI`, `GPIO_SLEW_LO`
- Pull: `GPIO_PULL_NONE`, `GPIO_PULL_UP`, `GPIO_PULL_DOWN`
- External interrupt: `GPIO_EI_NONE`, `GPIO_EI_RISE`, `GPIO_EI_FALL`,
  `GPIO_EI_BOTH`
- Filter: `GPIO_FILTER_NONE`, `GPIO_FILTER_LV0`, `GPIO_FILTER_LV1`,
  `GPIO_FILTER_LV2`

Clock dependency: GPIO registers are always clocked by SCLK; no RCM
gate required.

### 5.2 Configuration structures

`HAL_GPIO_InitTypeDef` fields:

| Field | Type | Description |
|---|---|---|
| Port | `MODULE_PORT_TypeDef*` | GPIO port to be configured |
| Pin | `HAL_GPIO_PinTypedef` | Bitmask of pins (PIN0..PIN31) |
| Direction | `HAL_GPIO_DirectionTypedef` | input or output |
| State | `HAL_GPIO_StateTypedef` | output drive level |
| Slew | `HAL_GPIO_SlewTypedef` | output slew-rate control |
| Pull | `HAL_GPIO_PullTypedef` | input pull configuration |
| ExInt | `HAL_GPIO_ExIntTypedef` | external-interrupt sense |
| Filter | `HAL_GPIO_FilterTypedef` | input filter strength |

### 5.3 Handle

GPIO has no dedicated handle struct; the `Port` pointer inside the Init
struct is the module reference.

### 5.4 How to use this driver

1. Fill a `HAL_GPIO_InitTypeDef` with Port, Pin, Direction and the
   output- or input-specific fields.
2. Call `HAL_GPIO_Init` to program the port.
3. Drive or read pins through the register-level port macros defined in
   the device header.
4. Call `HAL_GPIO_DeInit` to restore power-on defaults.

This section contains the following APIs:
- `HAL_GPIO_Init`
- `HAL_GPIO_DeInit`

#### Example: drive an LED on output

```c
HAL_GPIO_InitTypeDef led;

led.Port      = PORT0;
led.Pin       = PIN5;          /* HAL_GPIO_PinTypedef 位值，PIN0..PIN31 */
led.Direction = GPIO_OUTPUT;
led.State     = GPIO_STATE_HI;
led.Slew      = GPIO_SLEW_LO;

(void)HAL_GPIO_Init(&led);
```

#### Example: read a switch as a filtered input

```c
HAL_GPIO_InitTypeDef sw;

sw.Port      = PORT1;
sw.Pin       = GPIO_PIN_0;
sw.Direction = GPIO_INPUT;
sw.Pull      = GPIO_PULL_UP;
sw.ExInt     = GPIO_EI_NONE;
sw.Filter    = GPIO_FILTER_LV2;

(void)HAL_GPIO_Init(&sw);
```

### 5.5 API description

#### HAL_GPIO_Init
Function name: `HAL_StatusTypeDef HAL_GPIO_Init(HAL_GPIO_InitTypeDef *Init);`
Function description: Programs one GPIO port's pin mask with the mode,
output level, slew, pull, external-interrupt and filter settings supplied
in the Init structure.
Parameters:
- Init: pointer to a filled `HAL_GPIO_InitTypeDef`.
Return values:
- HAL_OK on success.
- HAL_ERROR if Init is NULL or Pin is zero.
Notes: Multiple pins in one call must share the same settings.

#### HAL_GPIO_DeInit
Function name: `HAL_StatusTypeDef HAL_GPIO_DeInit(HAL_GPIO_InitTypeDef *Init);`
Function description: Restores the pins described by the Init structure
to their default input, no-pull, no-interrupt state.
Parameters:
- Init: pointer to a `HAL_GPIO_InitTypeDef` whose Port and Pin fields
  select the affected pins.
Return values:
- HAL_OK on success.
- HAL_ERROR if Init is NULL.

## 6. RCM driver module

### 6.1 RCM features

RCM manages reset sources, oscillator selection, PLL, USB 25 MHz CDR
clock, and clock-gating masks. This driver supports:

- De-initialization to a safe state: `HAL_RCM_DeInit`
- System clock source switch and PLL start: `HAL_RCM_Init`
- PLL-over-frequency interrupt service: `HAL_PLL_IRQHandler`
- USB oscillator trim control: `HAL_RCM_UsbTrim`,
  `HAL_RCM_UsbTrimLock`
- SCLK query in Hz: `HAL_RCM_GetSysClockFreq`
- Per-block clock gating macros for FLASH, ADC, DAC, USB, CRC
- Clock-switch wait bounded by `RCM_CSWAIT_LOOP_MAX` iterations

Clock dependency: RCM itself runs on IHOSC before any switch; it is the
first module to initialize in `HAL_SYS_Init`.

### 6.2 Configuration structures

There is no Init struct; `HAL_RCM_Init` takes no arguments and uses the
configuration constants from `oc32_hal_conf.h` (`OC32_SCLK_FREQ`,
`OC32_CLK_PLL_ENABLE`, `HAL_RCM_CDR_ENABLE`). The two enums used by the
trim functions are:

`HAL_RCM_OscTrimTypedef`: `RCM_OSC_TRIM_OFF`, `RCM_OSC_TRIM_ON`
`HAL_RCM_OscTrimLockTypedef`: `RCM_OSC_TRIM_LOCK_OFF`,
`RCM_OSC_TRIM_LOCK_ON`

### 6.3 Handle

RCM has no handle struct.

### 6.4 How to use this driver

1. Call `HAL_RCM_Init` at system start to select the clock source and
   enable PLL when configured.
2. Use `HAL_RCM_GetSysClockFreq` to read back SCLK for baud-rate and
   timer calculations.
3. For USB, call `HAL_RCM_UsbTrim` to enable the 25 MHz trim and
   `HAL_RCM_UsbTrimLock` to lock the trimer.
4. Register the PLL over-frequency handler by leaving
   `HAL_PLL_IRQHandler` in place; override `HAL_PLL_UserIRQ` for
   application reaction.
5. Use `__HAL_RCM_*_CLK_ENABLE` / `__HAL_RCM_*_CLK_DISABLE` macros to
   gate the clock to a specific block.

This section contains the following APIs:
- `HAL_RCM_DeInit`
- `HAL_RCM_Init`
- `HAL_PLL_IRQHandler`
- `HAL_RCM_UsbTrim`
- `HAL_RCM_UsbTrimLock`
- `HAL_RCM_GetSysClockFreq`

#### Example: bring up the clock tree and read SCLK

```c
/* Clock source, PLL factors and peripheral gating are set via the
   configuration macros in oc32_hal_conf.h, not as function arguments. */
(void)HAL_RCM_Init();

uint32_t sys_clk = HAL_RCM_GetSysClockFreq();
/* use sys_clk to derive UART baud and timer prescaler settings */
```

#### Example: lock the USB 25 MHz trimmer

```c
#ifdef HAL_USB_ENABLE
    (void)HAL_RCM_UsbTrim(RCM_OSC_TRIM_ON);
    (void)HAL_RCM_UsbTrimLock(RCM_OSC_TRIM_LOCK_ON);
#endif
```

### 6.5 API description

#### HAL_RCM_Init
Function name: `HAL_StatusTypeDef HAL_RCM_Init(void);`
Function description: Configures the clock source, enables PLL if
configured, waits for the clock switch flag within `RCM_CSWAIT_LOOP_MAX`
iterations, and updates the global `SystemClock` word.
Parameters:
- none.
Return values:
- HAL_OK on success.
- HAL_TIMEOUT if the clock switch flag did not clear within the limit.

#### HAL_RCM_DeInit
Function name: `HAL_StatusTypeDef HAL_RCM_DeInit(void);`
Function description: Returns the RCM to a safe default state, running
the core on IHOSC with the PLL disabled.
Parameters:
- none.
Return values:
- HAL_OK on success.

#### HAL_PLL_IRQHandler
Function name: `HAL_StatusTypeDef HAL_PLL_IRQHandler(void);`
Function description: Services the PLL over-frequency condition flagged
by `PLLCON->PFO`; clears the flag and calls the overridable
`HAL_PLL_UserIRQ`.
Parameters:
- none.
Return values:
- HAL_OK on success.
Notes: Fired from vector `__int0`; the vector is intentionally not
placed in SRAM because PLL over-frequency is a clock-safety event.

#### HAL_RCM_UsbTrim
Function name: `HAL_StatusTypeDef HAL_RCM_UsbTrim(HAL_RCM_OscTrimTypedef Trim);`
Function description: Enables or disables the USB 25 MHz crystal
oscillator trimer.
Parameters:
- Trim: `RCM_OSC_TRIM_ON` or `RCM_OSC_TRIM_OFF`.
Return values:
- HAL_OK on success.

#### HAL_RCM_UsbTrimLock
Function name: `HAL_StatusTypeDef HAL_RCM_UsbTrimLock(HAL_RCM_OscTrimLockTypedef Lock);`
Function description: Locks the trimer in its final value to prevent
runtime drift.
Parameters:
- Lock: `RCM_OSC_TRIM_LOCK_ON` or `RCM_OSC_TRIM_LOCK_OFF`.
Return values:
- HAL_OK on success.

#### HAL_RCM_GetSysClockFreq
Function name: `uint32_t HAL_RCM_GetSysClockFreq(void);`
Function description: Returns the current SCLK frequency in Hz based on
the configured clock source.
Parameters:
- none.
Return values:
- SCLK frequency in Hz.

## 7. UART driver module

### 7.1 UART features

Two UART instances (UART_0, UART_1). This driver supports:

- Baud-rate select from `HAL_UART_BaudTypedef` (9600 to 1 Mbaud)
- Data width 8/9 bits with or without parity
- Parity modes: `UART_PAR_ODD`, `UART_PAR_EVEN`, `UART_PAR_0`,
  `UART_PAR_1`
- Full-duplex or half-duplex (`HAL_UART_DuplexTypedef`)
- Transmit/receive over polling, interrupt, or DMA
- Overridable user IRQ via `HAL_UART_UserIRQ`

Clock dependency: UART is clocked by SCLK; configure `HAL_RCM_Init`
first and compute the baud divisor from `HAL_RCM_GetSysClockFreq`.

### 7.2 Configuration structures

`HAL_UART_InitTypeDef` fields:

| Field | Type | Description |
|---|---|---|
| Uart | `MODULE_UART_TypeDef*` | UART instance |
| Parity | `HAL_UART_ParityTypedef` | parity mode |
| Bits | `HAL_UART_BitsTypedef` | data width (8, 8+parity, 9) |
| Baud | `HAL_UART_BaudTypedef` | baud rate |
| Duplex | `HAL_UART_DuplexTypedef` | full or half duplex |
| Pins | `HAL_UART_PinsTypedef` | pin bank |
| DmaCh | `HAL_DMA_ChTypedef` | DMA channel for transfers |

`HAL_UART_PacketTypedef` fields:

| Field | Type | Description |
|---|---|---|
| Data | `uint32_t*` | byte buffer |
| Length | `uint32_t` | number of bytes |
| IntE | `HAL_EnableTypeDef` | on/off interrupt-driven operation |

### 7.3 Handle

No handle struct; `Uart` pointer in the Init struct is the module
reference.

### 7.4 How to use this driver

1. Fill `HAL_UART_InitTypeDef` for the target instance.
2. Call `HAL_UART_Init`.
3. For polling, fill `HAL_UART_PacketTypedef` with `IntE` off and call
   `HAL_UART_TX` or `HAL_UART_RX`.
4. For interrupt/DMA, set `IntE` on and let the vector dispatch to
   `HAL_UART_IRQHandler`; override `HAL_UART_UserIRQ` for application
   reaction.
5. Call `HAL_UART_DeInit` to stop the module.

This section contains the following APIs:
- `HAL_UART_Init`
- `HAL_UART_DeInit`
- `HAL_UART_TX`
- `HAL_UART_RX`
- `HAL_UART_IRQHandler`
- `HAL_UART_UserIRQ`

#### Example: polling-mode transmission

```c
HAL_UART_InitTypeDef cfg;
cfg.Uart   = UART0;
cfg.Baud   = UART_BAUD_115200;
cfg.Bits   = UART_8_BIT;
cfg.Parity = UART_PAR_0;

(void)HAL_UART_Init(&cfg);

static const uint32_t msg[] = "hi\r\n";
HAL_UART_PacketTypeDef pkt;
pkt.Data = (uint32_t *)msg;
pkt.Length = 4;
pkt.IntE   = OC32_INT_OFF;      /* polling */
(void)HAL_UART_TX(UART0, &pkt);
```

#### Example: interrupt-driven reception

```c
HAL_UART_PacketTypeDef rpkt;
rpkt.Data = rx_buf;             /* user buffer */
rpkt.Length = 64;
rpkt.IntE   = OC32_INT_ON;      /* use IRQ + optional DMA */
(void)HAL_UART_RX(UART0, &rpkt);
```

### 7.5 API description

#### HAL_UART_Init
Function name: `HAL_StatusTypeDef HAL_UART_Init(HAL_UART_InitTypeDef *Init);`
Function description: Programs the UART instance with the baud rate,
data width, parity, duplex mode, pin bank and DMA channel supplied in
the Init structure.
Parameters:
- Init: pointer to a filled `HAL_UART_InitTypeDef`.
Return values:
- HAL_OK on success.
- HAL_ERROR if Init is NULL.

#### HAL_UART_DeInit
Function name: `HAL_StatusTypeDef HAL_UART_DeInit(MODULE_UART_TypeDef *Uart);`
Function description: Restores the UART instance to its power-on default
state.
Parameters:
- Uart: pointer to the UART instance.
Return values:
- HAL_OK on success.

#### HAL_UART_TX
Function name: `__sram__ HAL_StatusTypeDef HAL_UART_TX(MODULE_UART_TypeDef *Uart, HAL_UART_PacketTypedef *Packet);`
Function description: Transmits `Packet->Length` bytes from
`Packet->Data` using the mode selected by `Packet->IntE`
(polling, interrupt, or DMA).
Parameters:
- Uart: pointer to the UART instance.
- Packet: pointer to a filled `HAL_UART_PacketTypedef`.
Return values:
- HAL_OK on success.
- HAL_BUSY if the transmitter is busy.
- HAL_ERROR if Packet is NULL or Length is zero.
Notes: Placed in SRAM for low-latency ISR use.

#### HAL_UART_RX
Function name: `__sram__ HAL_StatusTypeDef HAL_UART_RX(MODULE_UART_TypeDef *Uart, HAL_UART_PacketTypedef *Packet);`
Function description: Receives `Packet->Length` bytes into
`Packet->Data` using the mode selected by `Packet->IntE`.
Parameters:
- Uart: pointer to the UART instance.
- Packet: pointer to a filled `HAL_UART_PacketTypedef`.
Return values:
- HAL_OK on success.
- HAL_BUSY if the receiver is busy.
- HAL_ERROR if Packet is NULL or Length is zero.

#### HAL_UART_IRQHandler
Function name: `__sram__ HAL_StatusTypeDef HAL_UART_IRQHandler(MODULE_UART_TypeDef *Uart);`
Function description: Services the UART interrupt: clears the pending
flags and dispatches to the overridable `HAL_UART_UserIRQ`.
Parameters:
- Uart: pointer to the UART instance.
Return values:
- HAL_OK on success.
Notes: Called from vector `__int20` / `__int21`.

#### HAL_UART_UserIRQ
Function name: `__sram__ __weak__ HAL_StatusTypeDef HAL_UART_UserIRQ(MODULE_UART_TypeDef *Uart);`
Function description: Weakly-linked application hook called at the end
of the UART interrupt service; override to react to transfer completion.
Parameters:
- Uart: pointer to the UART instance.
Return values:
- HAL_OK on success.

## 8. SPI driver module

### 8.1 SPI features

Two SPI instances (SPI_0, SPI_1). This driver supports:

- Four clock modes via `HAL_SPI_ModeTypedef` (CIH/CIL x DSS/DSF)
- Master or slave: `HAL_SPI_SlaveTypedef`
- Hardware or software CS: `HAL_SPI_CsOutTypedef`
- Full or half duplex: `HAL_SPI_DuplexTypedef`
- Half-duplex direction: `HAL_SPI_HalfTxRxTypedef`
- Half-duplex pin sharing: `HAL_SPI_HalfPinTypedef`
- MSB/LSB first: `HAL_SPI_FirstBitTypedef`
- Pin bank select: `HAL_SPI_PinsTypedef`
- DMA-driven transfers

Clock dependency: SPI is clocked by SCLK; the master clock divider is
applied to the resulting SCLK frequency.

### 8.2 Configuration structures

`HAL_SPI_InitTypeDef` fields:

| Field | Type | Description |
|---|---|---|
| Spi | `MODULE_SPI_TypeDef*` | SPI instance |
| Mode | `HAL_SPI_ModeTypedef` | SPI mode 0-3 |
| ClkDiv | `uint32_t` | SPI clock divider |
| Slave | `HAL_SPI_SlaveTypedef` | master or slave |
| CsOut | `HAL_SPI_CsOutTypedef` | hardware CS enable |
| Duplex | `HAL_SPI_DuplexTypedef` | full or half duplex |
| HalfPin | `HAL_SPI_HalfPinTypedef` | half-duplex pin mode |
| FirstBit | `HAL_SPI_FirstBitTypedef` | MSB or LSB first |
| Pins | `HAL_SPI_PinsTypedef` | pin bank |
| DmaCh | `HAL_DMA_ChTypedef` | DMA channel |

`HAL_SPI_PacketTypedef` fields:

| Field | Type | Description |
|---|---|---|
| HalfRx | `HAL_SPI_HalfTxRxTypedef` | half-duplex direction |
| TxData | `uint32_t*` | transmit buffer |
| RxData | `uint32_t*` | receive buffer |
| Length | `uint32_t` | number of bytes |
| IntE | `HAL_EnableTypeDef` | on/off interrupt-driven operation |

### 8.3 Handle

No handle struct; `Spi` pointer in the Init struct is the module
reference.

### 8.4 How to use this driver

1. Fill `HAL_SPI_InitTypeDef` for the target instance.
2. Call `HAL_SPI_Init`.
3. For each frame, fill `HAL_SPI_PacketTypedef` and call
   `HAL_SPI_Transfer`.
4. For interrupt/DMA operation, set `IntE` on and let the vector
   dispatch to `HAL_SPI_IRQHandler`.
5. Call `HAL_SPI_DeInit` to stop the module.

This section contains the following APIs:
- `HAL_SPI_Init`
- `HAL_SPI_DeInit`
- `HAL_SPI_Transfer`
- `HAL_SPI_IRQHandler`
- `HAL_SPI_UserIRQ`

#### Example: full-duplex DMA transfer

```c
HAL_SPI_InitTypeDef cfg;
cfg.Spi      = SPI0;
cfg.Mode     = SPI_CIL_DSS;
cfg.ClkDiv   = 4;
cfg.Slave    = SPI_MASTER;
cfg.Duplex   = SPI_FULL_DUPLEX;
cfg.FirstBit = SPI_MSB_FIRST;
cfg.DmaCh    = OC32_DMA_SPI0_TX;

(void)HAL_SPI_Init(&cfg);

HAL_SPI_PacketTypeDef pkt;
pkt.TxData = tx_buf;
pkt.RxData = rx_buf;
pkt.Length = 32;
pkt.IntE   = OC32_INT_OFF;
(void)HAL_SPI_Transfer(SPI0, &pkt);
```

### 8.5 API description

#### HAL_SPI_Init
Function name: `HAL_StatusTypeDef HAL_SPI_Init(HAL_SPI_InitTypeDef *Init);`
Function description: Programs the SPI instance with the mode, clock
divider, master/slave, CS control, duplex, half-duplex pin sharing,
bit order, pin bank, and DMA channel supplied in the Init structure.
Parameters:
- Init: pointer to a filled `HAL_SPI_InitTypeDef`.
Return values:
- HAL_OK on success.
- HAL_ERROR if Init is NULL.

#### HAL_SPI_DeInit
Function name: `HAL_StatusTypeDef HAL_SPI_DeInit(MODULE_SPI_TypeDef *Spi);`
Function description: Restores the SPI instance to its power-on default
state.
Parameters:
- Spi: pointer to the SPI instance.
Return values:
- HAL_OK on success.

#### HAL_SPI_Transfer
Function name: `__sram__ HAL_StatusTypeDef HAL_SPI_Transfer(MODULE_SPI_TypeDef *Spi, HAL_SPI_PacketTypedef *Packet);`
Function description: Performs one SPI frame of `Packet->Length` bytes,
respecting the mode selected by `Packet->IntE`
(polling, interrupt, or DMA). For half-duplex, only one of TxData or
RxData is used depending on `Packet->HalfRx`.
Parameters:
- Spi: pointer to the SPI instance.
- Packet: pointer to a filled `HAL_SPI_PacketTypedef`.
Return values:
- HAL_OK on success.
- HAL_BUSY if a previous transfer is in progress.
- HAL_ERROR if Packet is NULL.
Notes: Placed in SRAM for low-latency ISR use.

#### HAL_SPI_IRQHandler
Function name: `__sram__ HAL_StatusTypeDef HAL_SPI_IRQHandler(MODULE_SPI_TypeDef *Spi);`
Function description: Services the SPI interrupt: clears the pending
flags and dispatches to the overridable `HAL_SPI_UserIRQ`.
Parameters:
- Spi: pointer to the SPI instance.
Return values:
- HAL_OK on success.
Notes: Called from vector `__int22` / `__int23`.

#### HAL_SPI_UserIRQ
Function name: `__sram__ __weak__ HAL_StatusTypeDef HAL_SPI_UserIRQ(MODULE_SPI_TypeDef *Spi);`
Function description: Weakly-linked application hook called at the end
of the SPI interrupt service.
Parameters:
- Spi: pointer to the SPI instance.
Return values:
- HAL_OK on success.

## 9. IIC driver module

### 9.1 IIC features

Single IIC (I2C) peripheral at base 0x3FF08300. This driver supports:

- Master and slave modes: `HAL_IIC_SlaveTypedef`
- Clock stretching in slave mode: `HAL_IIC_StretchTypedef`
- DMA-driven transfers: DMA channel `OC32_DMA_IIC`
- 7-bit and 10-bit addressing (with a header byte):
  `HAL_IIC_AddrModeTypedef`
- Interrupt or polling operation
- Pin bank select: `HAL_IIC_PinsTypedef`

Because the chip has only one IIC instance, the driver exposes the
device-header register macros (`IICCON`, `IICTBUF`, `IICRBUF`, `IICADR`,
`IICDA`, `IICBC`) directly for fine-grained control; the high-level
`MODULE_IIC_TypeDef` block-struct pattern used by multi-instance
peripherals is not introduced here.

Clock dependency: IIC is clocked by SCLK; `ClkDiv` in the Init struct
scales the master SCL.

### 9.2 Configuration structures

`HAL_IIC_InitTypeDef` fields:

| Field | Type | Description |
|---|---|---|
| Slave | `HAL_IIC_SlaveTypedef` | master or slave |
| AddrMode | `HAL_IIC_AddrModeTypedef` | 7-bit or 10-bit + header |
| DevAddr | `uint32_t` | device address |
| ClkDiv | `uint32_t` | master clock divider |
| Stretch | `HAL_IIC_StretchTypedef` | slave clock-stretch enable |
| Pins | `HAL_IIC_PinsTypedef` | pin bank |
| DmaCh | `HAL_DMA_ChTypedef` | DMA channel |

`HAL_IIC_PacketTypedef` fields:

| Field | Type | Description |
|---|---|---|
| DevAddr | `uint32_t` | target device address |
| Data | `uint32_t*` | byte buffer in SRAM |
| Length | `uint32_t` | byte count (polling max 4, DMA max 1023) |
| IntE | `HAL_EnableTypeDef` | on/off interrupt-driven operation |

Supporting enums: `HAL_IIC_TxControlTypedef` (start/addr/data/stop
sequence codes), `HAL_IIC_TxRxTypedef`, `HAL_IIC_SlaveRWTypedef`,
`HAL_IIC_BusStateTypedef`, `HAL_IIC_SlaveMatchTypedef`,
`HAL_IIC_TransferStateTypedef`.

### 9.3 Handle

No handle struct; the control register `IICCON` is the module reference.

### 9.4 How to use this driver

1. Fill `HAL_IIC_InitTypeDef` for the role (master or slave).
2. Program the IICCON register using the Init fields (the driver leaves
   this to direct register access for a single-instance peripheral).
3. For master transfers, walk the sequence
   `IIC_TX_START` -> `IIC_TX_ADDR` -> `IIC_TX_DATA` -> `IIC_TX_STOP`
   via the register macros, or issue a DMA transfer on channel
   `OC32_DMA_IIC`.
4. For slave operation, enable clock stretching, let the vector
   `__int24` dispatch to `HAL_IIC_IRQHandler`.

This section contains the following APIs:
- `HAL_IIC_IRQHandler`
- `HAL_IIC_UserIRQ`

All other IIC operations are performed through the device-header
register macros; the chapter intentionally lists only the two interrupt
entry points that are C functions.

#### Example: master write a register over IIC (polling)

```c
/* Single-instance peripheral: the control register is programmed
   directly with the device-header macros. */
IIC_TX_START(IIC);
IIC_TX_ADDR(IIC, I2C_DEV_ADDR << 1);          /* write, 7-bit addr */
while (!IIC_BUSY(IIC));
IIC_TX_DATA(IIC, reg_addr);
while (!IIC_BUSY(IIC));
IIC_TX_DATA(IIC, reg_val);
IIC_TX_STOP(IIC);
```

### 9.5 API description

#### HAL_IIC_IRQHandler
Function name: `__sram__ HAL_StatusTypeDef HAL_IIC_IRQHandler(void);`
Function description: Services the IIC interrupt: clears the pending
flags, services any in-progress DMA transfer, and dispatches to the
overridable `HAL_IIC_UserIRQ`.
Parameters:
- none.
Return values:
- HAL_OK on success.
Notes: Called from vector `__int24`.

#### HAL_IIC_UserIRQ
Function name: `__sram__ __weak__ HAL_StatusTypeDef HAL_IIC_UserIRQ(void);`
Function description: Weakly-linked application hook called at the end
of the IIC interrupt service; override to react to transfer completion
or slave address match.
Parameters:
- none.
Return values:
- HAL_OK on success.

#### HAL_IIC register operations
Function name: N/A (register-level, no C wrapper).
Function description: Master and slave transfers are issued by writing
the `IICCON` control register and polling `IICBC` / `IICRBUF` directly;
the supported sequence codes are enumerated in
`HAL_IIC_TxControlTypedef`.
Parameters:
- N/A.
Return values:
- N/A.
Notes: Register access is the intended usage for a single-instance
peripheral; see the IIC header for the full set of macros.

## 10. TIMER driver module

### 10.1 TIMER features

Eight TIMER instances (TIMER_0..TIMER_7). This driver supports:

- Count modes: `HAL_TIMER_CountModeTypedef` (up, down, toggle, encode)
- Count sources: internal SCLK, external pins, compare outputs, other
  timer outputs (`HAL_TIMER_CountSourceTypedef`)
- Encoder interface with pin/mode/polarity select
  (`HAL_TIMER_EncodeInitTypeDef`)
- External reset generation (`HAL_TIMER_ResetInitTypeDef`)
- Pause generation (`HAL_TIMER_PauseInitTypeDef`)
- Event generation with post-divide
  (`HAL_TIMER_EventInitTypeDef`)
- On-the-fly reload/prescale/counter update
  (`HAL_TIMER_UpdateTypeDef`)
- ADC trigger condition select
  (`HAL_TIMER_AdcTrigConTypedef`)

Clock dependency: TIMER is clocked by SCLK; the prescaler field in the
Init struct scales the free-running count.

### 10.2 Configuration structures

`HAL_TIMER_BaseInitTypeDef` fields:

| Field | Type | Description |
|---|---|---|
| Timer | `MODULE_TIMER_TypeDef*` | TIMER instance |
| ReloadVal | `uint32_t` | reload value |
| PreScaler | `uint32_t` | prescaler |
| CountVal | `uint32_t` | initial counter value |
| IntE | `HAL_EnableTypeDef` | on/off interrupt |
| CountMode | `HAL_TIMER_CountModeTypedef` | count direction |
| StopCon | `HAL_TIMER_StopConTypedef` | stop condition |
| OvfMode | `HAL_TIMER_OvflagModeTypedef` | overflow flag mode |
| CountSource | `HAL_TIMER_CountSourceTypedef` | count input |
| CountExCon | `HAL_TIMER_CountExConTypedef` | external sense |
| UpdateMode | `HAL_TIMER_UpdateModeTypedef` | update trigger |
| ReadReg | `HAL_TIMER_ReadRegTypedef` | register buffer select |
| AdcTrig | `HAL_TIMER_AdcTrigConTypedef` | ADC trigger condition |

Secondary Init structs: `HAL_TIMER_EncodeInitTypeDef`
(enc0Pin, enc1Pin, mode, polarity), `HAL_TIMER_ResetInitTypeDef`
(genEvent, exSource, exCon), `HAL_TIMER_PauseInitTypeDef`
(genEvent, exSource, exCon), `HAL_TIMER_EventInitTypeDef` (ovfPost,
intE, source, exCon), and `HAL_TIMER_UpdateTypeDef` (reloadVal,
prescaler, countVal).

### 10.3 Handle

No handle struct; `Timer` pointer in each Init struct is the module
reference.

### 10.4 How to use this driver

1. Fill `HAL_TIMER_BaseInitTypeDef` for the target instance.
2. Call `HAL_TIMER_Init`.
3. Configure the secondary features by filling the specific Init struct
   and calling `HAL_TIMER_ResetInit`, `HAL_TIMER_PauseInit`,
   `HAL_TIMER_EventInit`, or the encoder variant.
4. For on-the-fly updates, call `HAL_TIMER_UpdateNow` with a
   `HAL_TIMER_UpdateTypeDef`.
5. Wait for overflow or event via `HAL_TIMER_WaitOv` /
   `HAL_TIMER_WaitEv`.
6. For interrupt operation, override `HAL_TIMER_UserIRQ`; the vector
   `__int9`..`__int16` dispatches to `HAL_TIMER_IRQHandler`.

This section contains the following APIs:
- `HAL_TIMER_Init`
- `HAL_TIMER_DeInit`
- `HAL_TIMER_ResetInit`
- `HAL_TIMER_PauseInit`
- `HAL_TIMER_EventInit`
- `HAL_TIMER_UpdateNow`
- `HAL_TIMER_ResetNow`
- `HAL_TIMER_WaitOv`
- `HAL_TIMER_WaitEv`
- `HAL_TIMER_IRQHandler`
- `HAL_TIMER_UserIRQ`

#### Example: 1 ms periodic counter in reset mode

```c
HAL_TIMER_BaseInitTypeDef cfg;
cfg.Timer      = TIMER0;
cfg.PreScaler  = HAL_RCM_GetSysClockFreq() / 1000; /* divide to 1 kHz */
cfg.ReloadVal  = 1000;                              /* 1 ms at 1 kHz */
cfg.CountMode  = TIMER_COUNT_UP;
cfg.IntE       = OC32_INT_ON;

(void)HAL_TIMER_Init(&cfg);

HAL_TIMER_ResetInitTypeDef rcfg;
rcfg.Timer = TIMER0;
(void)HAL_TIMER_ResetInit(&rcfg);

(void)HAL_TIMER_WaitOv(TIMER0);   /* block until the 1 ms flag */
```

### 10.5 API description

#### HAL_TIMER_Init
Function name: `HAL_StatusTypeDef HAL_TIMER_Init(HAL_TIMER_BaseInitTypeDef *Init);`
Function description: Programs the TIMER instance with count mode,
reload, prescaler, initial value, count source, and the secondary
conditions in the Init structure.
Parameters:
- Init: pointer to a filled `HAL_TIMER_BaseInitTypeDef`.
Return values:
- HAL_OK on success.
- HAL_ERROR if Init is NULL.

#### HAL_TIMER_DeInit
Function name: `HAL_StatusTypeDef HAL_TIMER_DeInit(MODULE_TIMER_TypeDef *Timer);`
Function description: Restores the TIMER instance to its power-on
default state.
Parameters:
- Timer: pointer to the TIMER instance.
Return values:
- HAL_OK on success.

#### HAL_TIMER_EventInit
Function name: `__weak__ __sram__ HAL_StatusTypeDef HAL_TIMER_EventInit(HAL_TIMER_EventInitTypeDef *Init);`
Function description: Configures the TIMER event-generation block:
source, sense condition, post-overflow divide, and interrupt enable.
Parameters:
- Init: pointer to a filled `HAL_TIMER_EventInitTypeDef`.
Return values:
- HAL_OK on success.

#### HAL_TIMER_UpdateNow
Function name: `__weak__ __sram__ HAL_StatusTypeDef HAL_TIMER_UpdateNow(HAL_TIMER_UpdateTypeDef *Update);`
Function description: Applies a new reload, prescaler and counter value
to the TIMER without stopping it, using the UpdateMode selected at
init.
Parameters:
- Update: pointer to a filled `HAL_TIMER_UpdateTypeDef`.
Return values:
- HAL_OK on success.
- HAL_ERROR if Update is NULL.

#### HAL_TIMER_WaitOv
Function name: `__weak__ __sram__ HAL_StatusTypeDef HAL_TIMER_WaitOv(MODULE_TIMER_TypeDef *Timer);`
Function description: Blocks until the TIMER overflow flag is set.
Parameters:
- Timer: pointer to the TIMER instance.
Return values:
- HAL_OK when the flag is set.
- HAL_TIMEOUT if the iteration limit is exceeded.

#### HAL_TIMER_IRQHandler
Function name: `__weak__ __sram__ HAL_StatusTypeDef HAL_TIMER_IRQHandler(MODULE_TIMER_TypeDef *Timer);`
Function description: Services the TIMER interrupt: clears the overflow
and event flags and dispatches to the overridable
`HAL_TIMER_UserIRQ`.
Parameters:
- Timer: pointer to the TIMER instance.
Return values:
- HAL_OK on success.
Notes: Called from vectors `__int9`..`__int16`.

#### HAL_TIMER_UserIRQ
Function name: `__weak__ __sram__ HAL_StatusTypeDef HAL_TIMER_UserIRQ(MODULE_TIMER_TypeDef *Timer);`
Function description: Weakly-linked application hook called at the end
of the TIMER interrupt service.
Parameters:
- Timer: pointer to the TIMER instance.
Return values:
- HAL_OK on success.

## 11. CCP driver module

### 11.1 CCP features

Six CCP instances (CCP_0..CCP_5), each bound to one of the eight TIMER
bases. This driver supports:

- Capture mode with input select, sense and prescaler
  (`HAL_CCP_CaptureInitTypeDef`)
- Compare mode with output control, invert and both positive/negative
  outputs (`HAL_CCP_CompareInitTypeDef`)
- PWM in edge- or center-aligned form, with duty and dead-time
  (`HAL_CCP_PwmInitTypeDef`)
- Gate control with AND/OR/XOR/REPLACE and start/end windows
  (`HAL_CCP_GateInitTypeDef`)
- Output forcing by input or LVD event (`HAL_CCP_ForceInitTypeDef`)
- On-the-fly duty/gate update (`HAL_CCP_UpdateTypeDef`)

Clock dependency: CCP runs on the TIMER base it is assigned to; the
base TIMER's prescaler sets the CCP time base.

### 11.2 Configuration structures

`HAL_CCP_BaseInitTypeDef` fields:

| Field | Type | Description |
|---|---|---|
| CCP | `MODULE_CCP_TypeDef*` | CCP instance |
| IntE | `HAL_EnableTypeDef` | on/off interrupt |
| Mode | `HAL_CCP_ModeTypedef` | capture/compare/PWM edge/PWM center |
| Timer | `HAL_CCP_BaseTimerTypedef` | TIMER base |
| UpdateMode | `HAL_CCP_UpdateModeTypedef` | update trigger |
| ReadReg | `HAL_CCP_ReadRegTypedef` | register buffer select |

Secondary Init structs: `HAL_CCP_CaptureInitTypeDef` (CapCon, CapIn,
Prescaler), `HAL_CCP_CompareInitTypeDef` (ComVal, ComCon, ComOut,
PosOutE, PosOutInv, NegOutE, NegOutInv), `HAL_CCP_PwmInitTypeDef`
(Duty, DeadTime, PwmCon, PosOutE/Inv, NegOutE/Inv),
`HAL_CCP_GateInitTypeDef` (GateStart, GateEnd, GateMode, GateUpCountE,
GateDownCountE, GateInv, GateFlag), `HAL_CCP_ForceInitTypeDef`
(ForceChannel, ForceInSource, ForceCon, ForceE, ForceLvdE, ForcePoState,
ForceNoState), and `HAL_CCP_UpdateTypeDef` (Duty, GateStart, GateEnd).

### 11.3 Handle

No handle struct; `CCP` pointer in each Init struct is the module
reference.

### 11.4 How to use this driver

1. Fill `HAL_CCP_BaseInitTypeDef` with the mode and TIMER base.
2. Call `HAL_CCP_Init`.
3. Configure the specific feature by calling the matching sub-Init:
   `HAL_CCP_CaptureInit`, `HAL_CCP_CompareInit`, `HAL_CCP_PwmInit`,
   `HAL_CCP_GateInit`, or `HAL_CCP_ForceInit`.
4. Read a captured sample via `HAL_CCP_CaptureValue`.
5. Update duty or gate windows on the fly via `HAL_CCP_UpdateNow` with
   a `HAL_CCP_UpdateTypeDef`.
6. Wait for the compare/gate/force flag via `HAL_CCP_WaitCf`,
   `HAL_CCP_WaitGf`, or `HAL_CCP_WaitFf`.
7. For interrupt operation, override `HAL_CCP_UserIRQ`; the vector
   `__int3`..`__int8` dispatches to `HAL_CCP_IRQHandler`.

This section contains the following APIs:
- `HAL_CCP_Init`
- `HAL_CCP_CaptureInit`
- `HAL_CCP_CompareInit`
- `HAL_CCP_PwmInit`
- `HAL_CCP_GateInit`
- `HAL_CCP_ForceInit`
- `HAL_CCP_Update`
- `HAL_CCP_UpdateNow`
- `HAL_CCP_WaitCf`
- `HAL_CCP_WaitGf`
- `HAL_CCP_WaitFf`
- `HAL_CCP_CaptureValue`
- `HAL_CCP_DeInit`
- `HAL_CCP_IRQHandler`
- `HAL_CCP_UserIRQ`

#### Example: 50 % PWM on a CCP channel

```c
HAL_CCP_BaseInitTypeDef base;
base.CCP    = CCP0;
base.Mode   = CCP_PWM_EDGE_MODE;
base.Timer  = CCP_BASE_TIMER0;
base.IntE   = OC32_INT_OFF;

(void)HAL_CCP_Init(&base);

HAL_CCP_PwmInitTypeDef pwm;
pwm.CCP = CCP0;
pwm.Duty = 50;                       /* duty percentage */
(void)HAL_CCP_PwmInit(&pwm);
```

#### Example: capture a falling edge

```c
HAL_CCP_CaptureInitTypeDef cap;
cap.CCP       = CCP1;
cap.CapIn     = CCP_CAP_FALL;
cap.Prescaler = 4;

(void)HAL_CCP_Init(&base);
(void)HAL_CCP_CaptureInit(&cap);

while (!HAL_CCP_CaptureValue(CCP1)); /* spin until capture flag */
uint32_t sample = HAL_CCP_CaptureValue(CCP1);
```

### 11.5 API description

#### HAL_CCP_Init
Function name: `HAL_StatusTypeDef HAL_CCP_Init(HAL_CCP_BaseInitTypeDef *Init);`
Function description: Programs the CCP instance with the mode, TIMER
base, update mode, read-buffer select and interrupt enable supplied in
the Init structure.
Parameters:
- Init: pointer to a filled `HAL_CCP_BaseInitTypeDef`.
Return values:
- HAL_OK on success.
- HAL_ERROR if Init is NULL.

#### HAL_CCP_PwmInit
Function name: `HAL_StatusTypeDef HAL_CCP_PwmInit(HAL_CCP_PwmInitTypeDef *Init);`
Function description: Configures the CCP PWM output: duty, dead-time,
flag sense, and the positive/negative output enable and invert bits.
Parameters:
- Init: pointer to a filled `HAL_CCP_PwmInitTypeDef`.
Return values:
- HAL_OK on success.
- HAL_ERROR if Init is NULL.

#### HAL_CCP_CaptureValue
Function name: `static inline uint32_t HAL_CCP_CaptureValue(MODULE_CCP_TypeDef *CCP);`
Function description: Reads the latest capture sample from the CCP
instance using the ReadReg select from the base Init.
Parameters:
- CCP: pointer to the CCP instance.
Return values:
- The 32-bit capture value.

#### HAL_CCP_UpdateNow
Function name: `HAL_StatusTypeDef HAL_CCP_UpdateNow(HAL_CCP_UpdateTypeDef *Update);`
Function description: Applies a new duty or gate window to the CCP
without stopping it, using the UpdateMode selected at init.
Parameters:
- Update: pointer to a filled `HAL_CCP_UpdateTypeDef`.
Return values:
- HAL_OK on success.
- HAL_ERROR if Update is NULL.

#### HAL_CCP_WaitCf
Function name: `HAL_StatusTypeDef HAL_CCP_WaitCf(MODULE_CCP_TypeDef *CCP);`
Function description: Blocks until the CCP compare flag is set.
Parameters:
- CCP: pointer to the CCP instance.
Return values:
- HAL_OK when the flag is set.
- HAL_TIMEOUT if the iteration limit is exceeded.

#### HAL_CCP_IRQHandler
Function name: `__weak__ __sram__ HAL_StatusTypeDef HAL_CCP_IRQHandler(MODULE_CCP_TypeDef *CCP);`
Function description: Services the CCP interrupt: clears the pending
flags and dispatches to the overridable `HAL_CCP_UserIRQ`.
Parameters:
- CCP: pointer to the CCP instance.
Return values:
- HAL_OK on success.
Notes: Called from vectors `__int3`..`__int8`.

#### HAL_CCP_UserIRQ
Function name: `__weak__ __sram__ HAL_StatusTypeDef HAL_CCP_UserIRQ(MODULE_CCP_TypeDef *CCP);`
Function description: Weakly-linked application hook called at the end
of the CCP interrupt service.
Parameters:
- CCP: pointer to the CCP instance.
Return values:
- HAL_OK on success.

## 12. WDT driver module

### 12.1 WDT features

Two WDT instances: WDT_0 for application supervision and WDT_1
reserved for the HAL tick service. This driver supports:

- Five operating modes via `HAL_WDT_ModeTypedef`
  (NONE, RST, WAKE, INT, INT_AND_WAKE)
- Period bounds derived from the clock domain: `WDT_H_PERIOD_MIN/MAX`
  for high clock, `WDT_L_PERIOD_MIN/MAX` for low clock
- Clear key value `WDT_CLR_KEY`
- SRAM placement of `HAL_WDT_Clear` for safe use from any context

Clock dependency: the WDT counter source is selected by the clock
domain; use the matching `WDT_H_*` or `WDT_L_*` bound when setting the
period.

### 12.2 Configuration structures

`HAL_WDT_InitTypeDef` fields:

| Field | Type | Description |
|---|---|---|
| Wdt | `MODULE_WDT_TypeDef*` | WDT instance |
| Mode | `HAL_WDT_ModeTypedef` | operating mode |
| Period | `uint32_t` | overflow period in microseconds |

### 12.3 Handle

No handle struct; `Wdt` pointer in the Init struct is the module
reference.

### 12.4 How to use this driver

1. Fill `HAL_WDT_InitTypeDef` for WDT_0, keeping Period within the
   active clock-domain bounds.
2. Call `HAL_WDT_Init`.
3. Periodically call `HAL_WDT_Clear` with the clear key to reset the
   counter.
4. For interrupt mode, override `HAL_WDT_UserIRQ`; the vector
   `__int17` dispatches to `HAL_WDT_IRQHandler` for WDT_0 and performs
   the HAL tick increment for WDT_1.
5. Call `HAL_WDT_DeInit` to stop the module.

This section contains the following APIs:
- `HAL_WDT_Init`
- `HAL_WDT_DeInit`
- `HAL_WDT_Clear`
- `HAL_WDT_IRQHandler`
- `HAL_WDT_UserIRQ`

#### Example: run WDT_0 with a 5 s window

```c
HAL_WDT_InitTypeDef cfg;
cfg.Wdt     = WDT_0;
cfg.Mode    = WDT_MODE_INT;          /* raise interrupt, not reset */
cfg.Period  = 5000000;               /* 5 s in us */

(void)HAL_WDT_Init(&cfg);

/* main loop */
for (;;) {
    do_app_work();
    (void)HAL_WDT_Clear(WDT_0);      /* kick the watchdog */
}
```

### 12.5 API description

#### HAL_WDT_Init
Function name: `HAL_StatusTypeDef HAL_WDT_Init(HAL_WDT_InitTypeDef *Init);`
Function description: Programs the WDT instance with the mode and
period supplied in the Init structure; validates the period against
the clock-domain bounds.
Parameters:
- Init: pointer to a filled `HAL_WDT_InitTypeDef`.
Return values:
- HAL_OK on success.
- HAL_ERROR if Period is outside `WDT_H_PERIOD_MIN..MAX` or
  `WDT_L_PERIOD_MIN..MAX`.

#### HAL_WDT_Clear
Function name: `__sram__ HAL_StatusTypeDef HAL_WDT_Clear(MODULE_WDT_TypeDef *Wdt);`
Function description: Writes the clear key to stop the WDT counter
from reaching the overflow state.
Parameters:
- Wdt: pointer to the WDT instance.
Return values:
- HAL_OK on success.
Notes: Placed in SRAM so it can be called from any context, including
idle tick code.

#### HAL_WDT_IRQHandler
Function name: `HAL_StatusTypeDef HAL_WDT_IRQHandler(MODULE_WDT_TypeDef *Wdt);`
Function description: Services the WDT interrupt for the specified
instance; dispatches to the overridable `HAL_WDT_UserIRQ`.
Parameters:
- Wdt: pointer to the WDT instance.
Return values:
- HAL_OK on success.
Notes: For WDT_1, the same vector also calls `HAL_WDT_Clear` and
`HAL_TICK_Inc` as part of the HAL tick service.

#### HAL_WDT_UserIRQ
Function name: `__weak__ HAL_StatusTypeDef HAL_WDT_UserIRQ(MODULE_WDT_TypeDef *Wdt);`
Function description: Weakly-linked application hook called at the end
of the WDT interrupt service.
Parameters:
- Wdt: pointer to the WDT instance.
Return values:
- HAL_OK on success.

## 13. FLASH driver module

### 13.1 FLASH features

SPI-flash interface for external 256K serial NOR. This driver
supports:

- Quad-read mode for the core bus: `HAL_FLASH_CPUQuadRead`
- Clock-domain update after RCM switch: `HAL_FLASH_ClkUpdate`
- Program / read / erase operations on a packet
  (`HAL_Flash_PacketTypeDef`)
- Per-zone CRC update: `HAL_FLASH_UpdateCrc` with
  `HAL_Flash_ZoneTypeDef`
- Code-verify and recovery when
  `HAL_FLASH_VERIFY_ENABLE` is set: `HAL_FLASH_VerifyCode`
- Timing and command constants for 1/2/4-line core access and user
  op modes

Clock dependency: FLASH uses the FLASH clock gate in CLKCON
(`__HAL_RCM_FLASH_CLK_ENABLE`); after any SCLK source change, call
`HAL_FLASH_ClkUpdate` to reprogram the timing.

### 13.2 Configuration structures

`HAL_Flash_PacketTypeDef` fields:

| Field | Type | Description |
|---|---|---|
| Data | `uint32_t*` | byte buffer |
| Address | `uint32_t` | flash address |
| Length | `uint32_t` | byte count |
| DmaCh | `HAL_DMA_ChTypedef` | DMA channel |

`HAL_Flash_ZoneTypeDef` fields:

| Field | Type | Description |
|---|---|---|
| BankNum | `uint32_t` | flash bank number |
| ZoneNum | `uint32_t` | zone within the bank |

Supporting constants: `FLASH_PAGE_SIZE` (256), `FLASH_SECTOR_SIZE`
(4096), command codes (`FLASH_PROGRAM`, `FLASH_READ`,
`FLASH_SECTOR_ERASE`, `FLASH_PAGE_ERASE`, ...), and core/user timing
blocks.

### 13.3 Handle

No handle struct; the packet and zone structs are the references.

### 13.4 How to use this driver

1. Call `HAL_FLASH_Init` at system start.
2. After any RCM clock switch, call `HAL_FLASH_ClkUpdate` to reprogram
   the timing.
3. To program, fill a `HAL_Flash_PacketTypeDef` and call
   `HAL_FLASH_Program`.
4. To read, fill a `HAL_Flash_PacketTypeDef` and call `HAL_FLASH_Read`.
5. To erase, set `Length` to a whole sector or page and call
   `HAL_FLASH_Erase`.
6. To maintain per-zone integrity, call `HAL_FLASH_UpdateCrc` after
   each zone write.
7. Optionally call `HAL_FLASH_VerifyCode` at boot when
   `HAL_FLASH_VERIFY_ENABLE` is set.

This section contains the following APIs:
- `HAL_FLASH_Init`
- `HAL_FLASH_Read`
- `HAL_FLASH_Program`
- `HAL_FLASH_Erase`
- `HAL_FLASH_UpdateCrc`
- `HAL_FLASH_ClkUpdate`
- `HAL_FLASH_CPUQuadRead`
- `HAL_FLASH_VerifyCode`

#### Example: program and read back a flash region

```c
(void)HAL_FLASH_Init();

HAL_Flash_PacketTypeDef pkt;
pkt.Address = 0x00020000;
pkt.Data    = user_data;
pkt.Length  = 64;

(void)HAL_FLASH_Program(&pkt);
(void)HAL_FLASH_Read(&pkt);          /* verify contents */
(void)HAL_FLASH_UpdateCrc(&pkt);     /* refresh the zone CRC */
```

### 13.5 API description

#### HAL_FLASH_Init
Function name: `__sram__ HAL_StatusTypeDef HAL_FLASH_Init(void);`
Function description: Initializes the FLASH controller: writes the core
timing, user op mode and clock divider; enables the FLASH clock gate.
Parameters:
- none.
Return values:
- HAL_OK on success.
- HAL_ERROR on an internal configuration fault.

#### HAL_FLASH_Program
Function name: `__sram__ HAL_StatusTypeDef HAL_FLASH_Program(HAL_Flash_PacketTypeDef *Packet);`
Function description: Programs `Packet->Length` bytes from
`Packet->Data` into the flash address `Packet->Address`, using the
specified DMA channel when set.
Parameters:
- Packet: pointer to a filled `HAL_Flash_PacketTypeDef`.
Return values:
- HAL_OK on success.
- HAL_ERROR if the packet is NULL or the address is out of range.
Notes: The driver performs write-enable, program, and verify steps;
up to `HAL_FLASH_VERIFY_RETRY` retries are attempted on verify failure.

#### HAL_FLASH_Read
Function name: `__sram__ HAL_StatusTypeDef HAL_FLASH_Read(HAL_Flash_PacketTypeDef *Packet);`
Function description: Reads `Packet->Length` bytes from the flash
address `Packet->Address` into `Packet->Data`.
Parameters:
- Packet: pointer to a filled `HAL_Flash_PacketTypeDef`.
Return values:
- HAL_OK on success.
- HAL_ERROR if the packet is NULL or the address is out of range.

#### HAL_FLASH_Erase
Function name: `__sram__ HAL_StatusTypeDef HAL_FLASH_Erase(HAL_Flash_PacketTypeDef *Packet);`
Function description: Erases the region described by `Packet`; a
whole-sector or whole-page alignment is required.
Parameters:
- Packet: pointer to a filled `HAL_Flash_PacketTypeDef` with
  sector/page-aligned Address and Length.
Return values:
- HAL_OK on success.
- HAL_ERROR if the alignment is invalid.

#### HAL_FLASH_UpdateCrc
Function name: `HAL_StatusTypeDef HAL_FLASH_UpdateCrc(HAL_Flash_ZoneTypeDef *Zone);`
Function description: Recomputes the CRC of the specified zone and
writes the result into the zone's reserved CRC slot.
Parameters:
- Zone: pointer to a filled `HAL_Flash_ZoneTypeDef`.
Return values:
- HAL_OK on success.
- HAL_ERROR if Zone is NULL.

#### HAL_FLASH_ClkUpdate
Function name: `__sram__ HAL_StatusTypeDef HAL_FLASH_ClkUpdate(void);`
Function description: Reprograms the FLASH timing registers for the
current SCLK source and frequency; call after any `HAL_RCM_Init`
switch.
Parameters:
- none.
Return values:
- HAL_OK on success.

#### HAL_FLASH_VerifyCode
Function name: `__sram__ HAL_StatusTypeDef HAL_FLASH_VerifyCode(void);`
Function description: Verifies the CRC of each configured code zone
against the stored CRC word; on mismatch, attempts recovery by copying
from the backup zone.
Parameters:
- none.
Return values:
- HAL_OK when all zones verify or recover.
- HAL_ERROR when a zone cannot be recovered.
Notes: Compiled only when `HAL_FLASH_VERIFY_ENABLE` is set.

## 14. CRC driver module

### 14.1 CRC features

Single CRC peripheral at a fixed base. This driver supports:

- Poly, initial value, bit width, input/output reflect, output
  complement, and byte-order reverse configuration
- Input data modes: direct write, capture via DMA in/out, DMA read
  (`HAL_CRC_IDMTypedef`)
- One-byte and one-word software steps
- Block transfer via `HAL_CRC_PacketTypedef`
- DMA channel select for block transfers

Clock dependency: the CRC block has a dedicated CLKCON gate
(`__HAL_RCM_CRC_CLK_ENABLE` / `__HAL_RCM_CRC_CLK_DISABLE`); enable the
gate before use.

### 14.2 Configuration structures

`HAL_CRC_InitTypeDef` fields:

| Field | Type | Description |
|---|---|---|
| Poly | `uint32_t` | CRC polynomial |
| InitVal | `uint32_t` | initial register value |
| Bits | `HAL_CRC_BitsTypedef` | 5/8/16/32-bit |
| RefIn | `uint32_t` | input byte reflect |
| RefOut | `uint32_t` | output word reflect |
| CplOut | `uint32_t` | output complement |
| Rvb | `uint32_t` | output byte-order reverse |
| DmaCh | `HAL_DMA_ChTypedef` | DMA channel |

`HAL_CRC_PacketTypedef` fields:

| Field | Type | Description |
|---|---|---|
| Data | `uint32_t*` | byte buffer |
| Length | `uint32_t` | byte count |
| IntE | `HAL_EnableTypeDef` | on/off interrupt |
| IDM | `HAL_CRC_IDMTypedef` | input data mode |

Default constants: `CRC_POLY_DEFAULT` (0x04C11DB7) and
`CRC_GOOD_DEFAULT` (0x2144DF1C).

### 14.3 Handle

No handle struct; the control registers of the CRC block are the module
reference.

### 14.4 How to use this driver

1. Call `__HAL_RCM_CRC_CLK_ENABLE()` to ungate the CRC block.
2. Fill `HAL_CRC_InitTypeDef` with the desired polynomial and option
   bits.
3. Call `HAL_CRC_Init`.
4. For one-shot checks, use `HAL_CRC_OneByte` or `HAL_CRC_OneWord`.
5. For block transfers, fill `HAL_CRC_PacketTypedef` and call
   `HAL_CRC_Packet`.
6. Read the final value via the `HAL_CRC_GetResult` macro.
7. For interrupt/DMA-driven operation, override `HAL_CRC_UserIRQ`;
   the vector `__int29` dispatches to `HAL_CRC_IRQHandler`.
8. Call `HAL_CRC_DeInit` to reset the module.

This section contains the following APIs:
- `HAL_CRC_Init`
- `HAL_CRC_DeInit`
- `HAL_CRC_OneByte`
- `HAL_CRC_OneWord`
- `HAL_CRC_Packet`
- `HAL_CRC_IRQHandler`
- `HAL_CRC_UserIRQ`

#### Example: CRC-32 over a buffer

```c
HAL_CRC_InitTypeDef cfg;
cfg.Poly    = 0x04C11DB7;            /* CRC-32 (IEEE) */
cfg.InitVal = 0xFFFFFFFF;
cfg.Bits    = CRC_32_BIT;
cfg.DmaCh   = OC32_DMA_CRC;

__HAL_RCM_CRC_CLK_ENABLE();
(void)HAL_CRC_Init(&cfg);

HAL_CRC_PacketTypeDef pkt;
pkt.Data   = buf;
pkt.Length = 128;
pkt.IntE   = OC32_INT_OFF;
pkt.IDM    = CRC_IDM_WRITE_CRCIN;

HAL_ResultTypedef res = HAL_CRC_Packet(&pkt);
uint32_t crc = HAL_CRC_GetResult();   /* final value */
(void)res;
```

### 14.5 API description

#### HAL_CRC_Init
Function name: `HAL_StatusTypeDef HAL_CRC_Init(HAL_CRC_InitTypeDef *Init);`
Function description: Programs the CRC block with the polynomial,
initial value, bit width, reflect, complement and byte-order options
supplied in the Init structure.
Parameters:
- Init: pointer to a filled `HAL_CRC_InitTypeDef`.
Return values:
- HAL_OK on success.
- HAL_ERROR if Init is NULL.

#### HAL_CRC_Packet
Function name: `__sram__ HAL_ResultTypedef HAL_CRC_Packet(HAL_CRC_PacketTypedef *Packet);`
Function description: Runs the CRC over `Packet->Length` bytes from
`Packet->Data` using the selected input-data mode; returns the
resulting CRC word and the operation status.
Parameters:
- Packet: pointer to a filled `HAL_CRC_PacketTypedef`.
Return values:
- Data: the 32-bit CRC result.
- Error: HAL_OK, HAL_ERROR, HAL_BUSY or HAL_TIMEOUT.
Notes: Placed in SRAM for low-latency use.

#### HAL_CRC_OneByte
Function name: `__sram__ uint32_t HAL_CRC_OneByte(uint32_t Data);`
Function description: Feeds one byte into the CRC engine and returns
the running CRC value.
Parameters:
- Data: the input byte.
Return values:
- The updated 32-bit CRC value.

#### HAL_CRC_OneWord
Function name: `__sram__ uint32_t HAL_CRC_OneWord(uint32_t Data);`
Function description: Feeds one 32-bit word into the CRC engine and
returns the running CRC value.
Parameters:
- Data: the input word.
Return values:
- The updated 32-bit CRC value.

#### HAL_CRC_IRQHandler
Function name: `__sram__ HAL_StatusTypeDef HAL_CRC_IRQHandler(void);`
Function description: Services the CRC interrupt: clears the pending
flags and dispatches to the overridable `HAL_CRC_UserIRQ`.
Parameters:
- none.
Return values:
- HAL_OK on success.
Notes: Called from vector `__int29`.

#### HAL_CRC_UserIRQ
Function name: `__sram__ __weak__ HAL_StatusTypeDef HAL_CRC_UserIRQ(void);`
Function description: Weakly-linked application hook called at the end
of the CRC interrupt service.
Parameters:
- none.
Return values:
- HAL_OK on success.

## 15. LVD driver module

### 15.1 LVD features

Low-Voltage Detector. This driver supports:

- Sixteen trigger-voltage steps via `HAL_LVD_TrigVTypedef`
  (2.4 V to 4.7 V in 0.1/0.2 V steps)
- Five operating modes via `HAL_LVD_ModeTypedef`
  (NONE, RST, WAKE, INT, INT_AND_WAKE)

Clock dependency: LVD is always active on the power rail; it does not
require an RCM gate. The LVD trigger participates in vector `__int0`
alongside HRF and PLL over-frequency.

### 15.2 Configuration structures

`HAL_LVD_InitTypeDef` fields:

| Field | Type | Description |
|---|---|---|
| Mode | `HAL_LVD_ModeTypedef` | operating mode |
| TrigV | `HAL_LVD_TrigVTypedef` | trigger voltage |

### 15.3 Handle

No handle struct; the LVD control register `LVDCON` is the module
reference.

### 15.4 How to use this driver

1. Fill `HAL_LVD_InitTypeDef` with the desired mode and trigger
   voltage.
2. Call `HAL_LVD_Init`.
3. For interrupt mode, override `HAL_LVD_UserIRQ`; the vector
   `__int0` dispatches to `HAL_LVD_IRQHandler` when the LVD flag is
   set.
4. Call `HAL_LVD_DeInit` to disable the module.

This section contains the following APIs:
- `HAL_LVD_Init`
- `HAL_LVD_DeInit`
- `HAL_LVD_IRQHandler`
- `HAL_LVD_UserIRQ`

#### Example: wake on low battery

```c
HAL_LVD_InitTypeDef cfg;
cfg.Mode  = LVD_MODE_INT;       /* interrupt on threshold */
cfg.TrigV = LVD_TRIG_V3100;       /* 3.2 V threshold */

(void)HAL_LVD_Init(&cfg);

/* vector __int0 dispatches to HAL_LVD_IRQHandler; override the
   weakly-linked HAL_LVD_UserIRQ to flag the low-voltage condition. */
HAL_StatusTypeDef HAL_LVD_UserIRQ(void)
{
    low_battery_flag = 1;
    return HAL_OK;
}
```

### 15.5 API description

#### HAL_LVD_Init
Function name: `HAL_StatusTypeDef HAL_LVD_Init(HAL_LVD_InitTypeDef *Init);`
Function description: Programs the LVD with the mode and trigger
voltage supplied in the Init structure.
Parameters:
- Init: pointer to a filled `HAL_LVD_InitTypeDef`.
Return values:
- HAL_OK on success.
- HAL_ERROR if Init is NULL.

#### HAL_LVD_DeInit
Function name: `HAL_StatusTypeDef HAL_LVD_DeInit(void);`
Function description: Restores the LVD to its disabled state.
Parameters:
- none.
Return values:
- HAL_OK on success.

#### HAL_LVD_IRQHandler
Function name: `HAL_StatusTypeDef HAL_LVD_IRQHandler(void);`
Function description: Services the LVD interrupt: clears the flag and
dispatches to the overridable `HAL_LVD_UserIRQ`.
Parameters:
- none.
Return values:
- HAL_OK on success.
Notes: Fired from vector `__int0`.

#### HAL_LVD_UserIRQ
Function name: `__weak__ HAL_StatusTypeDef HAL_LVD_UserIRQ(void);`
Function description: Weakly-linked application hook called at the end
of the LVD interrupt service.
Parameters:
- none.
Return values:
- HAL_OK on success.

## 16. HIC driver module

### 16.1 HIC features

Hardware Interrupt Controller with 32 channels. This driver supports:

- Per-channel enable, priority-0 and priority-1 bit configuration via
  `HAL_HICInitTypeDef`
- Channel map `HAL_HIC_MapTypedef` covering all peripheral and system
  sources
- Four priority levels `HAL_HIC_PriorityTypedef`
- HRF (hardware reset flag) interrupt service for system faults
- Optional higher-priority preemption through the HIPRM mask

The ISR layer is split into assembly stubs
(`oc32_hal_int_asm.S`, `__asm_int0..31`) that save the EIA register and
return, plus C handlers (`oc32_hal_int.c`, `__int0..31`) that dispatch
to the per-module IRQHandlers. Preemption is enabled by setting the
corresponding HIPRM bit at the top of the C handler and cleared before
`OR R1, R0, eia` restores the return address.

### 16.2 Configuration structures

`HAL_HICInitTypeDef` fields:

| Field | Type | Description |
|---|---|---|
| ChEnable | `uint32_t` | bit mask of enabled HIC channels |
| ChPriorityBit0 | `uint32_t` | per-channel priority bit 0 |
| ChPriorityBit1 | `uint32_t` | per-channel priority bit 1 |

Channel map `HAL_HIC_MapTypedef` defines `OC32_HIC_SERR`
(bit 0) through `OC32_HIC_PORTB` (bit 31), and the channel-index enum
`HAL_HIC_ChTypedef` provides `OC32_HIC_CH_SERR` through
`OC32_HIC_CH_PORTB`.

### 16.3 Handle

No handle struct; the HIC registers are the module reference.

### 16.4 How to use this driver

1. Fill `HAL_HICInitTypeDef` with the channel-enable mask and the two
   priority bit masks.
2. Call `HAL_HIC_Init`.
3. Call `HAL_HIC_DeInit` to clear all HIC channels on shutdown.
4. For system-fault sources (HRF, LVD, PLL over-frequency), leave the
   default `HAL_HRF_IRQHandler`, `HAL_LVD_IRQHandler` and
   `HAL_PLL_IRQHandler` in place; they run from Flash for reliability.
5. For peripheral sources, override the per-module `HAL_X_UserIRQ`
   weakly-linked hook to add application logic.

This section contains the following APIs:
- `HAL_HIC_Init`
- `HAL_HIC_DeInit`
- `HAL_HRF_IRQHandler`
- `HAL_POR_UserIRQ`
- `HAL_DBGR_UserIRQ`

#### Example: enable and prioritize interrupt channels

```c
HAL_HICInitTypeDef hic;
hic.ChEnable      = (1U << HAL_HIC_UART0_CH) | (1U << HAL_HIC_WDT0_CH);
hic.ChPriorityBit0 = 0;          /* lower preemption group */
hic.ChPriorityBit1 = 0;

(void)HAL_HIC_Init(&hic);
/* ... run peripheral drivers ... */
HAL_HIC_DeInit();               /* shutdown: clear all channels */
```

### 16.5 API description

#### HAL_HIC_Init
Function name: `HAL_StatusTypeDef HAL_HIC_Init(HAL_HICInitTypeDef *Init);`
Function description: Programs the HIC with the per-channel enable and
priority bits supplied in the Init structure.
Parameters:
- Init: pointer to a filled `HAL_HICInitTypeDef`.
Return values:
- HAL_OK on success.
- HAL_ERROR if Init is NULL.

#### HAL_HIC_DeInit
Function name: `void HAL_HIC_DeInit(void);`
Function description: Clears all HIC channel enables and priority bits
to their power-on defaults.
Parameters:
- none.
Return values:
- none.

#### HAL_HRF_IRQHandler
Function name: `HAL_StatusTypeDef HAL_HRF_IRQHandler(void);`
Function description: Services the hardware reset flag interrupt:
clears the HRF flag and dispatches to the overridable `HAL_POR_UserIRQ`
or `HAL_DBGR_UserIRQ` depending on the source.
Parameters:
- none.
Return values:
- HAL_OK on success.
Notes: Fired from vector `__int0`.

#### HAL_POR_UserIRQ
Function name: `__weak__ HAL_StatusTypeDef HAL_POR_UserIRQ(void);`
Function description: Weakly-linked application hook for power-on-reset
recovery.
Parameters:
- none.
Return values:
- HAL_OK on success.

#### HAL_DBGR_UserIRQ
Function name: `__weak__ HAL_StatusTypeDef HAL_DBGR_UserIRQ(void);`
Function description: Weakly-linked application hook for debugger-originated
reset.
Parameters:
- none.
Return values:
- HAL_OK on success.

## 17. System driver module

### 17.1 System features

The system layer (`oc32_hal.c`) provides:

- System initialization orchestrator `HAL_SYS_Init`
- Tick service `HAL_TICK_Init` with `HAL_TICK_Inc` / `HAL_TICK_GetVal`
  and `HAL_Delay`
- DMA channel management `HAL_DMA_Update`
- Fuse-data access: `HAL_GetRevID`, `HAL_GetDevID`, `HAL_GetUIDw0..3`,
  `HAL_GetRegTrim0w0..3`
- Debug console (when `HAL_UART_DEBUG_ENABLE`): `HAL_DEBUG_Init`,
  `puts`, `printf`
- HAL version query `HAL_GetHalVer`
- Syscall and systrap trampolines `__syscall_handler`,
  `__systrap_handler`

Clock dependency: the system layer assumes RCM has been initialized;
`HAL_SYS_Init` (weakly linked, overridable) is the recommended entry
point that calls RCM Init, HIC Init, TICK Init and any user peripheral
inits.

### 17.2 Configuration structures

No dedicated Init struct; `HAL_SYS_Init` takes no arguments. The tick
frequency is set via `HAL_TICK_FreqTypeDef` (`HAL_TICK_FREQ_10Hz`,
`HAL_TICK_FREQ_100Hz`, `HAL_TICK_FREQ_1KHz`, `HAL_TICK_FREQ_DEFAULT`).

### 17.3 Handle

No handle struct; `uwTick` is the global tick counter and
`DMAChState[OC32_DMA_CH_TOTAL]` tracks DMA channel busy state.

### 17.4 How to use this driver

1. Call `HAL_SYS_Init` at application start.
2. Use `HAL_Delay` for coarse software delays based on `uwTick`.
3. Use `HAL_DMA_Update` before a DMA transfer to start the channel and
   mark it busy.
4. Read the device identity words via `HAL_GetRevID`, `HAL_GetDevID`,
   `HAL_GetUIDw0..3`.
5. Query the HAL library version via `HAL_GetHalVer`.

This section contains the following APIs:
- `HAL_SYS_Init`
- `HAL_TICK_Init`
- `HAL_HIC_Init`
- `HAL_HIC_DeInit`
- `HAL_DMA_Update`
- `HAL_Delay`
- `HAL_GetHalVer`
- `HAL_GetRevID`
- `HAL_GetDevID`
- `HAL_GetUIDw0`
- `HAL_GetUIDw1`
- `HAL_GetUIDw2`
- `HAL_GetUIDw3`
- `HAL_GetRegTrim0w0`
- `HAL_GetRegTrim0w1`
- `HAL_GetRegTrim0w2`
- `HAL_GetRegTrim0w3`

### 17.5 API description

#### HAL_SYS_Init
Function name: `__weak__ HAL_StatusTypeDef HAL_SYS_Init(void);`
Function description: Weakly-linked system orchestrator; the default
implementation calls `HAL_RCM_Init`, `HAL_HIC_Init` with default
priorities, and `HAL_TICK_Init`. Override to add user-specific
initialization.
Parameters:
- none.
Return values:
- HAL_OK on success.
- HAL_ERROR if any step fails.

#### HAL_TICK_Init
Function name: `HAL_StatusTypeDef HAL_TICK_Init(void);`
Function description: Starts the HAL tick service using WDT_1 as the
tick source at the configured `HAL_TICK_FreqTypeDef` rate.
Parameters:
- none.
Return values:
- HAL_OK on success.
- HAL_ERROR if the WDT tick cannot be started.

#### HAL_Delay
Function name: `void HAL_Delay(uint32_t Delay);`
Function description: Blocks the calling context for `Delay`
milliseconds, based on `uwTick`.
Parameters:
- Delay: number of milliseconds.
Return values:
- none.
Notes: Requires `HAL_TICK_Init` to have been called; the tick must be
running on a context that can be preempted.

#### HAL_DMA_Update
Function name: `__sram__ HAL_StatusTypeDef HAL_DMA_Update(HAL_EnableTypeDef DMAE, HAL_DMA_ChTypedef DmaCh, HAL_DMA_MapTypedef Module, HAL_DMA_StateTypedef State);`
Function description: Updates the DMA channel state in
`DMAChState[]` and enables or disables the channel; called at the
start and end of every DMA-driven transfer.
Parameters:
- DMAE: `HAL_ENABLE` or `HAL_DISABLE`.
- DmaCh: the DMA channel index.
- Module: the DMA map (peripheral) the channel is bound to.
- State: `OC32_DMA_IDLE` or `OC32_DMA_BUSY`.
Return values:
- HAL_OK on success.
- HAL_ERROR on an illegal channel or module combination.
Notes: Placed in SRAM for low-latency ISR use.

#### HAL_GetHalVer
Function name: `#define HAL_GetHalVer() (__OC32_HAL_VERSION)`
Function description: Returns the HAL library version word, laid out as
`[31:24] MAIN | [23:16] SUB1 | [15:8] SUB2 | [7:0] RC`.
Parameters:
- none.
Return values:
- 32-bit version word.

#### HAL_GetRevID
Function name: `uint32_t HAL_GetRevID(void);`
Function description: Reads the silicon revision ID from the fuse
block via the `SYSC` syscall.
Parameters:
- none.
Return values:
- 32-bit revision ID.

## 18. Conventions and compliance

This manual documents the public API only; private helpers are listed
inline where the header needs them. All C functions follow the strict
ANSI C (C90 basis) conventions, with `static inline` used for
single-call-site helpers and `__attribute__((weak))` /
`__attribute__((section(".stext")))` for the `__weak__` and `__sram__`
placement markers. All status-returning functions use
`HAL_StatusTypeDef`; data-carrying functions use
`HAL_ResultTypedef`. No C++-specific constructs appear in the headers;
`extern "C"` guards are provided for C++ interop.

The module-enable macros in `oc32_hal_conf.h` are the single point of
control: any module whose macro is undefined is excluded from
compilation. The default configuration enables GPIO, RCM, WDT, FLASH
(with verify), CRC, LVD, UART, TIMER, CCP, SPI, IIC, MPWM, USB, ADC,
DAC, CMP, AMP, VTS, and DEBUG.

## 19. Revision history

- 1.0.0: initial HAL release covering all on-chip peripherals with
  HIC, DMA and tick services.
- 1.0.0-rc0: pre-release baseline; same API surface as the release.

### 手册修订记录

- 2026-11-09 修订：对齐 source 真实 API/枚举命名。
  - 各外设示例代码中的枚举/常量由占位符替换为头文件真实命名
    （如 `UART_BAUD_115200`、`SPI_CIL_DSS`、`TIMER_COUNT_UP`、
    `WDT_MODE_INT`、`LVD_TRIG_V3100`、`CRC_32_BIT`、`CCP_CAP_FALL`、
    `CCP_PWM_EDGE_MODE`、`RCM_OSC_TRIM_ON` 等）。
  - `GPIO_PIN_5` → `PIN5`（HAL_GPIO_PinTypedef 真实位值命名）。
  - 修复 `USER_GUIDE.md` 遗留的 `HAL_TIMER_EventTrig` 幻觉，
    统一为 source 真实名 `HAL_TIMER_EventInit`。
  - 本次修订仅改动示例代码段与 Revision history，未触碰正文 API 五段式描述。
