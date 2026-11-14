# OC32 HAL User Manual

## 1. Introduction

The OC32 HAL (Hardware Abstraction Layer) is the driver library for the
CV32S6015 series of 32-bit RISC-architecture MCUs (OC32 ISA). It provides a
register-independent, portable C API over the chip's peripherals so that
application code does not touch device-header register macros directly.

Layered design:

- **Application layer**: user code, an operating system or an RTOS.
- **HAL driver layer** (this manual): per-peripheral Init/DeInit/transfer
  functions, interrupt handlers, DMA integration, and the tick service.
- **Device layer**: chip register definitions
  (`CV32S6015SR.h` / `CV32S6015MSR.h`), the memory map, and the
  `__sram__` placement attribute.

Scope of this manual: GPIO, RCM, WDT, UART, SPI, IIC, TIMER, CCP, FLASH,
CRC, LVD, VTS, DAC, CMP, AMP, MPWM, and ADC drivers, the HIC interrupt
service layer, and the generic HAL type system. Each peripheral chapter
follows a fixed template:
features, configuration structure(s), and a per-function API description.
No code blocks are used; API names are referenced inline in numbered steps.

## 2. Acronyms and definitions

- **HAL**: Hardware Abstraction Layer.
- **RCM**: Reset and Clock Manager.
- **HIC**: Hardware Interrupt Controller.
- **CCP**: Capture / Compare / PWM unit.
- **WDT**: Watchdog Timer.
- **LVD**: Low-Voltage Detector.
- **VTS**: Voltage (or thermal) Sensor block.
- **CRC**: Cyclic Redundancy Check unit.
- **DAC**: Digital-to-Analog Converter.
- **CMP**: Voltage Comparator.
- **AMP**: Amplifier (analog front-end).
- **MPWM**: Multi-PWM / serial-LED driver.
- **ADC**: Analog-to-Digital Converter.
- **IIC**: Inter-Integrated Circuit (I2C) bus controller.
- **IHOSC/ILOSC**: Internal High/Low-speed Oscillator.
- **XHOSC/XLOSC**: External High/Low-speed Oscillator.
- **CDR**: Clock Data Recovery (USB) oscillator.
- **EIA**: Exception/Interrupt Accumulator register (PC shadow; R1 is its alias).
- **HIPRM**: Higher-priority interrupt request mask (interrupt preemption enable).
- **`__sram__`**: place function in the `.stext` (SRAM) section for low-latency fetch.
- **`__weak__`**: weakly-linked symbol, overridable by user code.

## 3. General information

HAL drivers are organized as one header and one source file per peripheral,
all pulled in by `oc32_hal.h`. The file
`oc32_hal_def.h` holds the common types and macros shared by every driver.
`oc32_hal_conf.h` holds user configuration: clock frequencies, flash layout,
the DMA channel map, the HIC channel map, interrupt priority levels, and the
per-module enable macros (`HAL_UART_ENABLE`, `HAL_SPI_ENABLE`, and so on).
A module whose enable macro is undefined is excluded from compilation.

The design uses no per-handle lock structure: state is passed through the
configuration structs, and per-operation status is returned as a
`HAL_StatusTypedef`. Where the hardware supports them, three transfer modes
exist: polling (blocking wait loops), interrupt (`_UserIRQ` / `_IRQHandler`
pairs), and DMA (channel chosen inside the Init struct via
`HAL_DMA_ChTypedef`).

The code is written in strict ANSI C, using `static inline` helpers where a
header needs them, and the weak-attribute pattern for user-overridable hooks.

## 4. API classification

API groups (peripheral acronym written `PPP`):

- **Initialization / de-initialization**: `HAL_PPP_Init`, `HAL_PPP_DeInit`.
- **Operation**: `HAL_PPP_TX`, `HAL_PPP_RX`, `HAL_PPP_Transfer`,
  `HAL_PPP_Read`, `HAL_PPP_Program`, `HAL_PPP_Erase`, `HAL_PPP_UpdateNow`.
- **Wait**: busy-loop helpers such as `HAL_PPP_WaitOv`, `HAL_PPP_WaitEv`,
  `HAL_PPP_WaitCf`, `HAL_PPP_WaitGf`, `HAL_PPP_WaitFf`.
- **Interrupt service**: `HAL_PPP_IRQHandler` (dispatches to the user hook)
  and the overridable `__weak__` `HAL_PPP_UserIRQ`.
- **Query**: `HAL_PPP_CaptureValue`, `HAL_RCM_GetSysClockFreq`,
  `HAL_GetHalVer`, `HAL_GetRevID`, `HAL_GetDevID`, `HAL_GetUIDw0`..`w3`.

## 5. Overview of HAL drivers

### 5.1 Common types

- `HAL_StatusTypedef`: `HAL_OK`, `HAL_ERROR`, `HAL_BUSY`, `HAL_TIMEOUT`.
- `HAL_EnableTypedef`: `HAL_DISABLE` / `HAL_ENABLE` — used to select on/off
  and interrupt-driven operation inside every Packet struct.
- `HAL_ResultTypedef`: couples a 32-bit `Data` word with a
  `HAL_StatusTypedef Error` (used by `HAL_CRC_Packet`).
- Two separate 2-value flag enums are used deliberately:
  `HAL_FlagErrorTypedef` (`HAL_FLAG_GOOD`/`HAL_FLAG_ERROR`) for
  error-condition flags, and `HAL_FlagValidTypedef`
  (`HAL_FLAG_INVALID`/`HAL_FLAG_VALID`) for done/busy flags. Do not mix them.
- `HAL_DMA_ChTypedef`: the shared DMA channel (CH0..CH3) chosen by a module.
- `HAL_HIC_ChTypedef` / `HAL_HIC_MapTypedef`: HIC interrupt-channel
  identifiers and their bit-mask forms.

### 5.2 Lifetime and error model

Typical sequence per peripheral: `HAL_PPP_Init` (writes the control
registers from the Init struct), normal operation via the operation APIs,
then `HAL_PPP_DeInit` (resets the module to power-on values).

- `HAL_ERROR` means an illegal argument (NULL pointer, unsupported value,
  or an unexpected state).
- `HAL_TIMEOUT` is returned when a wait loop exceeds its iteration limit
  (for example the RCM clock-switch wait `RCM_CSWAIT_LOOP_MAX`).
- `HAL_BUSY` is returned when a requested operation cannot start while a
  previous one is in progress.

### 5.3 Generic system service functions

`HAL_SYS_Init` is the system-level entry point that orchestrates the
recommended startup order. Its sequence:

1. `HAL_RCM_Init` — establish the system clock.
2. `HAL_FLASH_Init` — configure the flashloader for standard reads
   (guarded by `HAL_FLASH_ENABLE`).
3. `HAL_FLASH_VerifyCode` — verify flash code integrity
   (guarded by `HAL_FLASH_VERIFY_ENABLE`).
4. `HAL_DEBUG_Init` — bring up the debug UART
   (guarded by `HAL_UART_DEBUG_ENABLE`).
5. `HAL_TICK_Init` — set up the system tick on WDT1 in interrupt mode.

Notes:

- `HAL_TICK_Init` only configures the WDT1 tick; the tick does not actually
  run until the user calls `HAL_HIC_Init`, which sets the WDT HIE bit.
  Until then `uwTick` stays 0 and `HAL_Delay` will not advance.
- `HAL_HIC_Init` always enables the WDT channel (vector 17) in HIE so the
  system tick is active, regardless of the user's `ChEnable`.
- `HAL_HIC_DeInit` clears HIE entirely (including the WDT bit); the tick
  stops and must be re-enabled via `HAL_HIC_Init`.

### 5.4 Shared DMA channel management

All peripherals that can use DMA declare the channel through their Init
struct's `DmaCh` field. The HAL layer does **not** arbitrate channel
ownership between modules: the user is responsible for assigning
non-conflicting channels and for serializing access to any channel claimed
by two modules. `DMAChState[OC32_DMA_CH_TOTAL]` only records whether a
channel is currently busy so that `DmaUpdate` can reject a second BUSY
claim on the same channel. Flash/CRC internal operations default to CH0, so
user modules that also use CH0 must not run them concurrently.

`DmaUpdate` (SRAM-resident) is the single helper used by every module
to record a channel as busy or idle and to map the channel to a module.

### 5.5 Interrupt service architecture

Interrupts are split across `oc32_hal_int.c` and `oc32_hal_int_asm.S`.
Vector `__int0` is the fast-path handler for HRF (hardware reset flags),
LVD, and PLL over-frequency faults; it is intentionally **not** SRAM-resident
because it fires on power/clock faults where SRAM is less reliable than
Flash. The remaining peripheral vectors (`__int1`..`__int31`) are weak,
SRAM-resident, and dispatch to the corresponding `HAL_PPP_IRQHandler` with
interrupt preemption enabled via the HIPRM register (the EIA return address
is saved and restored around the HIPRM window).
## 6. HAL configuration (oc32_hal_conf.h)

`oc32_hal_conf.h` is generated from the template `oc32_hal_conf_template.h`.
All configuration knobs live there; the template placeholder fields
(`__HAL_CONFIG_*__`) are filled in per part.

### Clock configuration

| Define | Meaning |
|--------|---------|
| `OC32_CLK_SRC_*` | Reference oscillator source selection macros. |
| `OC32_CLK_PLL_ENABLE` | Enable PLL setup in `HAL_RCM_Init`. |
| `OC32_CLK_PLL_OM_ENABLE` | Enable the PLL frequency-over monitor. |
| `HAL_RCM_CDR_ENABLE` | Use the USB CDR oscillator as the reference. |
| `OC32_CDR_FREQ` | USB CDR output frequency in Hz. |
| `OC32_XHOSC_FREQ` | External high-speed oscillator frequency. |
| `OC32_XLOSC_FREQ` | External low-speed oscillator frequency. |
| `OC32_IHOSC_FREQ` | Internal high-speed oscillator frequency. |
| `OC32_ILOSC_FREQ` | Internal low-speed oscillator frequency. |
| `OC32_SCLK_FREQ` | Target system clock frequency. |

### Flash layout

| Define | Meaning |
|--------|---------|
| `OC32_FLASH_SIZE` | Total flash size. |
| `OC32_CODE_ZONE0_SIZE` | Boot zone size. |
| `OC32_CODE_ZONE1_SIZE` | User zone size. |
| `OC32_CODE_BANK_SIZE` | `ZONE0 + ZONE1` (one bank). |

### System configuration

| Define | Meaning |
|--------|---------|
| `HAL_VDD_VALUE` | Supply voltage in mV. |
| `HAL_TICK_FREQ_DEFAULT` | Tick period (default 1 kHz / 1 ms). |

`HAL_TICK_FreqTypedef` maps tick frequency to period-in-ms:
`HAL_TICK_FREQ_10Hz` (100 ms), `HAL_TICK_FREQ_100Hz` (10 ms),
`HAL_TICK_FREQ_1KHz` (1 ms).

### Debug UART

When `HAL_UART_DEBUG_ENABLE` is defined, a debug UART is brought up at boot
to support `puts` / `printf`. Key knobs: `HAL_UART_DEBUG_BR` (baud rate),
`HAL_UART0_DEBUG` / `HAL_UART1_DEBUG` (which instance),
`HAL_UART_DEBUG_PARITY`, `HAL_UART_DEBUG_DUPLEX`, and
`HAL_UART_DEBUG_PINS`.

### Module enable macros

Per-module enable macros control which drivers are compiled in:
`HAL_PORTB_ENABLE`, `HAL_CRC_ENABLE`, `HAL_FLASH_ENABLE`,
`HAL_FLASH_VERIFY_ENABLE`, `HAL_UART_ENABLE`, `HAL_TIMER_ENABLE`,
`HAL_CCP_ENABLE`, `HAL_SPI_ENABLE`, `HAL_IIC_ENABLE`, `HAL_MPWM_ENABLE`,
`HAL_USB_ENABLE`, `HAL_LVD_ENABLE`, `HAL_ADC_ENABLE`, `HAL_DAC_ENABLE`,
`HAL_CMP_ENABLE`, `HAL_AMP_ENABLE`, `HAL_VTS_ENABLE`, and
`HAL_DEBUG_ENABLE`.

### HIC configuration

`HAL_HICInitTypedef` configures the Hardware Interrupt Controller:
`ChEnable` (per-channel enable bitmask), `ChPriorityBit0` and
`ChPriorityBit1` (priority levels per channel).

`HAL_HIC_PriorityTypedef` provides `OC32_HIC_PRI0`..`OC32_HIC_PRI3`.
The macros `HPL_Bit0(pri, ch)` and `HPL_Bit1(pri, ch)` extract the two
priority bits for channel `ch` at priority `pri` and shift them into
position.

## 7. HAL common functions

### 7.1 HAL_TICK_Init

Initializes the system tick timer on WDT1 in interrupt mode.
Parameters: none. Returns `HAL_OK`.

Note: `HAL_TICK_Init` only configures the WDT tick parameters; it does
NOT enable the WDT interrupt in HIE. The system tick only actually runs
after the user calls `HAL_HIC_Init` (which sets the WDT HIE bit). Until
then, `uwTick` stays 0 and `HAL_Delay` will not advance.

### 7.2 HAL_HIC_Init

Initializes the HIC with the user-supplied `HAL_HICInitTypedef`.
Always ORs in the WDT HIE bit so the tick is active. Returns `HAL_OK`
or `HAL_ERROR` if `Init` is NULL.

### 7.3 HAL_HIC_DeInit

Resets HIE, HIPRM, HIPL0, and HIPL1 to 0. No return value.

Note: clearing HIE also disables the WDT tick channel, so `HAL_Delay`
will stop advancing. Call `HAL_HIC_Init` again if the tick is needed
after a DeInit. `HAL_FLASH_Init` invokes `HAL_HIC_DeInit` internally;
re-run `HAL_HIC_Init` afterwards if the tick is required.

### 7.4 HAL_Delay

Blocks for `Delay` milliseconds using the tick counter. No return value.

### 7.5 HAL_GetHalVer

Returns the HAL driver version packed as `MAIN<<24 | SUB1<<16 | SUB2<<8 | RC`.

### 7.6 HAL_GetRevID / HAL_GetDevID

Read the fused Product ID words. No return value beyond the word.
Must not be called from an ISR.

### 7.7 HAL_GetUIDw0..w3

Read the 128-bit unique device ID words (TSN). Must not be called from an ISR.

### 7.8 HAL_GetRegTrim0w0..w3

Read the fused register-trim values. Must not be called from an ISR.

### 7.9 HAL_POR_UserIRQ / HAL_DBGR_UserIRQ

Weak user hooks for Power-On-Reset and Debug-Reset interrupts.
Default implementation returns `HAL_OK`.

### 7.10 HAL_HRF_IRQHandler

Dispatches hardware reset flag interrupts: clears POR, DBGR, LVD, WDT0,
and VTS flags and routes to the corresponding handler. Returns `HAL_OK`
or `HAL_ERROR`.

### 7.11 DmaUpdate

Records a DMA channel's busy/idle state and maps it to a module.
Parameters: `DMAE` (current DMA enable from the module), `DmaCh`,
`Module`, `State`. Returns `HAL_OK` or `HAL_ERROR` (if the channel is
already busy and the caller tries to set it busy again).

### 7.12 HAL_DEBUG_Init

Brings up the debug UART (guarded by `HAL_UART_DEBUG_ENABLE`).
Configures the UART instance, parity, 8-bit + parity framing, the debug
baud rate, duplex mode, and pins. Returns `HAL_OK` or `HAL_ERROR`.

### 7.13 HAL_SYS_Init (weak)

Orchestrates the recommended startup order: RCM, FLASH, FLASH_VERIFY,
DEBUG_UART, TICK. Returns `HAL_OK` or `HAL_ERROR`.

### 7.14 puts / printf (weak, guarded by HAL_UART_DEBUG_ENABLE)

Minimal debug-console implementations for bare-metal. `puts` writes a
string + newline. `printf` supports `%d`, `%u`, `%x`, `%X`, `%s`,
`%c`, `%p`, `%%` (32-bit values only; 64-bit types unsupported).

## 8. GPIO driver

### 8.1 Features

- Two 32-bit GPIO ports: `PORT_A` and `PORT_B`.
- Per-pin configuration of direction (input/output), output state, slew
  rate, pull-up/pull-down, noise-filter level, and external-interrupt
  edge detection (rise/fail/both).
- Input pins can be configured with a digital filter (levels 0..3) and
  rising/falling/both-edge external interrupts.

### 8.2 Configuration structure

`HAL_GPIO_InitTypedef` is the single Init structure. Fields:

- `Port`: pointer to a `MODULE_PORT_Typedef` (must be `PORT_A` or
  `PORT_B`).
- `Pin`: one or more `HAL_GPIO_PinTypedef` bits (PIN0..PIN31),
  maskable to configure several pins at once.
- `Direction`: `GPIO_INPUT` or `GPIO_OUTPUT`.
- `State`: `GPIO_STATE_LO` / `GPIO_STATE_HI` (output mode only).
- `Slew`: `GPIO_SLEW_HI` / `GPIO_SLEW_LO` (output mode only).
- `Pull`: `GPIO_PULL_NONE` / `GPIO_PULL_UP` / `GPIO_PULL_DOWN`
  (input mode only).
- `ExInt`: `GPIO_EI_NONE` / `GPIO_EI_RISE` / `GPIO_EI_FALL` /
  `GPIO_EI_BOTH` (input mode only).
- `Filter`: `GPIO_FILTER_NONE` / `GPIO_FILTER_LV0` / `LV1` / `LV2`
  (input mode only).

### 8.3 HAL_GPIO_Init

1. Validate that `Init->Port` is `PORT_A` or `PORT_B`; return
   `HAL_ERROR` otherwise.
2. If output mode: set the output data and slew bits, disable pull and
   input-enable, and enable output-enable for the mask.
3. If input mode: disable output-enable; set the pull direction;
   configure the noise-filter level (LV0/LV2 set FS0; LV1/LV2 set FS1);
   enable input-enable; configure rising/falling interrupt edges
   (RIE/FIE) per `ExInt`; and clear the stale interrupt flags (IFC).
4. Return `HAL_OK`.

### 8.4 HAL_GPIO_DeInit

Disables output, pull, input, filter, and interrupt edges for the
specified pin mask; clears the interrupt flag. Returns `HAL_OK` or
`HAL_ERROR` if the port is invalid.

## 9. RCM driver

### 9.1 Features

- System clock configuration: reference-oscillator selection, PLL setup,
  flash clock divider update.
- Clock-switch wait loop (`RCM_ClockSwitchWait`) with a fixed iteration
  limit (`RCM_CSWAIT_LOOP_MAX`).
- PLL frequency-over interrupt.
- USB IOSC trim control (guarded by `HAL_USB_ENABLE`).
- Per-peripheral clock-gate macros: `__HAL_RCM_FLASH_CLK_ENABLE/DISABLE`,
  `__HAL_RCM_ADC_CLK_ENABLE/DISABLE`, `__HAL_RCM_DAC_CLK_ENABLE/DISABLE`,
  `__HAL_RCM_USB_CLK_ENABLE/DISABLE`, `__HAL_RCM_CRC_CLK_ENABLE/DISABLE`.

### 9.2 HAL_RCM_Init

1. Slow down the flash clock to the maximum divider (`FLASH_CLK_DIV5`).
2. Select the reference oscillator source (XHOSC, XLOSC, ILOSC, or CDR).
3. If PLL is enabled, compute the optimal PLL divider/multiplier pair
   and configure PLLCON; check the PLL frequency-over monitor.
4. Update the flash clock divider based on the new system clock.
5. On any error, call `HAL_RCM_DeInit` to restore defaults and return
   `HAL_ERROR`. Returns `HAL_OK` on success.

### 9.3 HAL_RCM_DeInit

Writes default values to CLKCON, PLLCON, BPCON, IOSCCON, VTSCON, and
LVDCON; resets `SystemClock` to `OC32_IHOSC_FREQ`; updates the flash
clock divider. Returns `HAL_OK`.

### 9.4 HAL_RCM_GetSysClockFreq

Returns the current `SystemClock` value in Hz.

### 9.5 HAL_PLL_IRQHandler

Falls back to the default clock (`HAL_RCM_DeInit`), clears the PFO flag,
and dispatches to `HAL_PLL_UserIRQ`. Returns `HAL_OK`.

### 9.6 HAL_RCM_UsbTrim / HAL_RCM_UsbTrimLock

Enable/disable the USB IOSC trim value and its lock. Guarded by
`HAL_USB_ENABLE`. Always returns `HAL_OK`.

## 10. WDT driver

### 10.1 Features

- Two watchdog timer instances: `WDT_0` and `WDT_1`.
- Modes: `WDT_MODE_NONE`, `WDT_MODE_RST`, `WDT_MODE_WAKE`,
  `WDT_MODE_INT`, `WDT_MODE_INT_AND_WAKE`.
- 8-bit reload counter with a 32-to-262144 prescaler.
- Period range is validated against `WDT_H_PERIOD_MIN`/`MAX` (high
  clock) or `WDT_L_PERIOD_MIN`/`MAX` (low clock) at init time.
- The clear/refresh key is `WDT_CLR_KEY` (0xA5).
- The min/max period limits carry a 1 µs safety margin
  (`WDT_PERIOD_LIMIT_MARGIN`) to avoid the 8-bit reload value rounding
  to 0.

### 10.2 Configuration structure

`HAL_WDT_InitTypedef` fields:

- `Wdt`: pointer to a `MODULE_WDT_Typedef` (`WDT_0` or `WDT_1`).
- `Mode`: `HAL_WDT_ModeTypedef`.
- `Period`: overflow period in microseconds.

### 10.3 HAL_WDT_Init

1. Validate `Init` is not NULL and `Wdt` is `WDT_0` or `WDT_1`.
2. Determine the WDT clock source (IHOSC if `OC32_CLK_ILOSC_FAKE` is
   set and `CLKCON->IOSCH` is high, else ILOSC) and check the period
   against the appropriate min/max range.
3. Select the smallest prescaler that yields a valid 8-bit reload
   value (counter ≤ 256); return `HAL_ERROR` if none fits.
4. Configure the action mode (reset / interrupt / wake) and enable
   the watchdog (`WDTE=1`). Returns `HAL_OK`.

### 10.4 HAL_WDT_Clear

Writes the clear key (`WDT_CLR_KEY`) to `WDTCLR` and waits for the
timeout flag to clear. SRAM-resident. Returns `HAL_OK` or `HAL_ERROR`
if the instance is invalid.

### 10.5 HAL_WDT_DeInit

Disables reset/wake/interrupt actions, refreshes the watchdog, then
disables it (`WDTE=0`). Returns `HAL_OK` or `HAL_ERROR`.

### 10.6 HAL_WDT_IRQHandler / HAL_WDT_UserIRQ

`HAL_WDT_IRQHandler` refreshes the watchdog and calls the weak
`HAL_WDT_UserIRQ`. The user hook can be overridden to add application
logic.

## 11. UART driver

### 11.1 Features

- Two UART instances: `UART_0` and `UART_1`.
- Full-duplex and half-duplex modes.
- 8-bit, 8-bit-with-parity, and 9-bit framing.
- Odd/even/sticky-0/sticky-1 parity.
- DMA transfers (channel from the Init struct).
- Polling and interrupt operation.

### 11.2 Configuration structures

`HAL_UART_InitTypedef` fields:

- `Uart`: pointer to `MODULE_UART_Typedef` (`UART_0` or `UART_1`).
- `Parity`: `HAL_UART_ParityTypedef`.
- `Bits`: `HAL_UART_BitsTypedef`.
- `Baud`: `HAL_UART_BaudTypedef` (9600 .. 1000000).
- `Duplex`: `UART_FULL_DUPLEX` / `UART_HALF_DUPLEX`.
- `Pins`: `UART_PINS0` / `UART_PINS1`.
- `DmaCh`: the shared DMA channel for transfers.

`HAL_UART_PacketTypedef` fields:

- `Data`: pointer to the data buffer.
- `Length`: number of bytes.
- `IntE`: `HAL_ENABLE` for interrupt mode, `HAL_DISABLE` for polling.

### 11.3 HAL_UART_Init

1. Validate `Init` is not NULL and `Uart` is `UART_0`/`UART_1`.
2. Record the DMA channel per instance.
3. Configure parity, framing, duplex, and pin select.
4. Compute the baud-rate divider from `SystemClock`; return `HAL_ERROR`
   if the baud rate is 0 or fewer than 8 cycles per bit.
5. Write the baud-rate register. Returns `HAL_OK`.

### 11.4 HAL_UART_DeInit

Resets the UART control register to 0, clears TX and RX done flags,
and releases the DMA channel. Returns `HAL_OK` or `HAL_ERROR`.

### 11.5 HAL_UART_TX

Transmits a packet. In half-duplex mode, waits for any pending RX DMA
to complete first. Uses DMA for transfers longer than 4 bytes or when
interrupt mode is enabled. In polling mode, blocks until the TX done
flag is set; in interrupt mode, returns immediately and completion is
handled by `HAL_UART_IRQHandler`. Returns `HAL_OK` or `HAL_ERROR`.

### 11.6 HAL_UART_RX

Receives a packet. In half-duplex mode, waits for any pending TX DMA
to complete first. Uses DMA for transfers longer than 4 bytes or when
interrupt mode is enabled. In polling mode, blocks until the RX done
flag is set; in interrupt mode, returns immediately. For short
non-DMA receives, the first word is read back from the RX buffer.
Returns `HAL_OK` or `HAL_ERROR`.

### 11.7 HAL_UART_IRQHandler / HAL_UART_UserIRQ

`HAL_UART_IRQHandler` handles TX/RX completion: clears the done flag,
disables the associated DMA, updates the DMA channel state, and disables
the UART interrupt when the channel is idle. Calls the weak
`HAL_UART_UserIRQ`. Returns `HAL_OK` or `HAL_ERROR`.

## 12. SPI driver

### 12.1 Features

- Two SPI instances: `SPI_0` and `SPI_1`.
- Four clock modes (CIH/CIL x DSS/DSF).
- Master and slave operation.
- Master CS-output control.
- Full-duplex and half-duplex (separate or shared pin) modes.
- MSB/LSB first.
- DMA transfers (channel from the Init struct).
- Polling and interrupt operation.

### 12.2 Configuration structures

`HAL_SPI_InitTypedef` fields:

- `Spi`: pointer to `MODULE_SPI_Typedef` (`SPI_0` or `SPI_1`).
- `Mode`: `HAL_SPI_ModeTypedef` (4 clock modes).
- `ClkDiv`: master clock divider.
- `Slave`: `SPI_MASTER` / `SPI_SLAVE`.
- `CsOut`: `SPI_CS_OUT_OFF` / `SPI_CS_OUT_ON` (master only).
- `Duplex`: `SPI_FULL_DUPLEX` / `SPI_HALF_DUPLEX`.
- `HalfPin`: `SPI_HALF_PIN_SEPARATE` / `SPI_HALF_PIN_SHARE`.
- `FirstBit`: `SPI_MSB_FIRST` / `SPI_LSB_FIRST`.
- `Pins`: `SPI_PINS0` / `SPI_PINS1`.
- `DmaCh`: the shared DMA channel.

`HAL_SPI_PacketTypedef` fields:

- `HalfRx`: `SPI_HALF_TX` / `SPI_HALF_RX` (half-duplex direction).
- `TxData`: pointer to the TX buffer (NULL only when TX is inactive).
- `RxData`: pointer to the RX buffer (NULL only when RX is inactive).
- `Length`: number of bytes.
- `IntE`: `HAL_ENABLE` for interrupt mode, `HAL_DISABLE` for polling.

### 12.3 HAL_SPI_Init

Validates the SPI instance, records the DMA channel, then configures
clock mode, clock divider, master/slave, CS-output, duplex, half-pin,
first-bit, and pins. Returns `HAL_OK` or `HAL_ERROR`.

### 12.4 HAL_SPI_Transfer

Performs a TX/RX transfer. Waits for any previous packet to complete.
Uses DMA for transfers longer than 4 bytes or when interrupt mode is
enabled. In polling mode, blocks until the transfer-done flag is set;
in interrupt mode, returns immediately. For short non-DMA transfers
the first RX word is read back from the RX buffer. Returns `HAL_OK` or
`HAL_ERROR`.

### 12.5 HAL_SPI_DeInit

Resets the SPI control register to 0, clears the TX done flag, and
releases the DMA channel. Returns `HAL_OK` or `HAL_ERROR`.

### 12.6 HAL_SPI_IRQHandler / HAL_SPI_UserIRQ

Handles transfer completion: clears the done flag, disables DMA,
updates the DMA channel state, and disables the SPI interrupt when the
channel is idle. Calls the weak `HAL_SPI_UserIRQ`. Returns `HAL_OK` or
`HAL_ERROR`.

## 13. IIC (I2C) driver

### 13.1 Features

- Single on-chip IIC peripheral, accessed directly via register macros
  (no `MODULE_IIC_Typedef` block struct).
- Master and slave modes.
- Clock stretching (slave).
- DMA transfers (channel from the Init struct; max 1023 bytes).
- 7-bit and 10-bit (with Header) addressing.
- Polling and interrupt operation.
- Non-DMA (register-buffer) path for transfers of 4 bytes or less.

### 13.2 Configuration structures

`HAL_IIC_InitTypedef` fields:

- `AddrMode`: `IIC_ADDR_7BIT` / `IIC_ADDR_10BIT`.
- `ClkDiv`: master clock divider.
- `Stretch`: clock-stretching enable (slave only).
- `Pins`: `IIC_PINS0` / `IIC_PINS1`.
- `DmaCh`: the shared DMA channel.

`HAL_IIC_PacketTypedef` fields:

- `DevAddr`: target device address.
- `RW`: `IIC_READ` / `IIC_WRITE`.
- `Data`: pointer to the data buffer (SRAM).
- `Length`: number of bytes (non-DMA max 4, DMA max 1023).
- `IntE`: `HAL_ENABLE` for interrupt mode, `HAL_DISABLE` for polling.

### 13.3 HAL_IIC_Init

Validates `Init` is not NULL. Records the DMA channel. Sets master
mode, address-mode, clock divider, clock-stretching, and pin select.
Enables the module. Returns `HAL_OK` or `HAL_ERROR`.

### 13.4 HAL_IIC_DeInit

Writes 0 to the IIC control register, resets the internal transfer-state
machine and in-flight packet pointer, and releases the DMA channel.

### 13.5 HAL_IIC_Transfer

Master-oriented transfer using an internal state machine
(IDLE → START → ADDR → DATA → STOP → IDLE). In interrupt mode,
starts the state machine and returns immediately; completion is handled
by `HAL_IIC_IRQHandler`. In polling mode, blocks until the state
machine returns to IDLE. The caller's `Packet` and its `->Data` buffer
must remain valid (module-scope or static, not a stack local) until the
transfer completes. Returns `HAL_OK` or `HAL_ERROR`.

### 13.6 HAL_IIC_IRQHandler / HAL_IIC_UserIRQ

Checks NACK and clock-timeout error flags, steps the transfer state
machine with the in-flight packet, and calls the weak `HAL_IIC_UserIRQ`.
Returns `HAL_OK` or `HAL_ERROR`.

## 14. TIMER driver

### 14.1 Features

- Eight timer instances: `TIMER_0`..`TIMER_7`.
- Count modes: up, down, toggle, encode.
- Stop conditions: overflow, event, both.
- Overflow-flag modes: none, over, under, both.
- Count sources: SCLK, PIN0..PIN2, CMP0/CMP1, T0/T1.
- Encoder input configuration (pin, mode, polarity).
- External reset, pause, and event-trigger features.
- ADC trigger condition.

### 14.2 Configuration structures

`HAL_TIMER_BaseInitTypedef` fields:

- `Timer`: pointer to `MODULE_TIMER_Typedef`.
- `ReloadVal`: 16-bit reload value.
- `PreScaler`: 16-bit prescaler.
- `CountVal`: initial counter value.
- `IntE`: interrupt enable.
- `CountMode`: `HAL_TIMER_CountModeTypedef`.
- `StopCon`: `HAL_TIMER_StopConTypedef`.
- `OvfMode`: `HAL_TIMER_OvflagModeTypedef`.
- `CountSource`: `HAL_TIMER_CountSourceTypedef`.
- `CountExCon`: external source condition.
- `UpdateMode`: `HAL_TIMER_UpdateModeTypedef`.
- `ReadReg`: `TIMER_READ_REG_DIR` / `TIMER_READ_REG_BUF`.
- `AdcTrig`: `HAL_TIMER_AdcTrigConTypedef`.

Additional Init structures: `HAL_TIMER_EncodeInitTypedef` (encoder mode),
`HAL_TIMER_ResetInitTypedef` (external reset), `HAL_TIMER_PauseInitTypedef`
(external pause), `HAL_TIMER_EventInitTypedef` (event trigger), and
`HAL_TIMER_UpdateTypedef` (dynamic update of reload/prescaler/counter).

### 14.3 HAL_TIMER_Init

Validates the timer instance. Writes the reload/prescaler/counter,
configures count mode, stop condition, overflow-flag mode, count source,
external condition, update mode, ADC trigger, read-register select, and
interrupt enable. Enables the timer as the last step. Returns `HAL_OK`
or `HAL_ERROR`.

### 14.4 HAL_TIMER_DeInit

Resets TCON/TECON to 0, clears TARL/TCNT, and clears the overflow and
event flags. Returns `HAL_OK` or `HAL_ERROR`.

### 14.5 HAL_TIMER_UpdateNow

Dynamically updates the reload value, prescaler, and counter. Temporarily
switches to user-trigger update mode, writes the new values, triggers the
update, then restores the original update mode. SRAM-resident, weak.

### 14.6 HAL_TIMER_ResetNow

Triggers an immediate timer reset. SRAM-resident, weak.

### 14.7 HAL_TIMER_WaitOv / HAL_TIMER_WaitEv

Block until the overflow or event flag is set, then clear it.
Caller must not mask the timer interrupt during the call. SRAM-resident,
weak.

### 14.8 HAL_TIMER_EncodeInit / HAL_TIMER_ResetInit /
### HAL_TIMER_PauseInit / HAL_TIMER_EventInit

Configure the encoder, external reset, external pause, and event-trigger
features respectively. Each validates the timer instance and writes the
relevant control registers. Returns `HAL_OK` or `HAL_ERROR`.

### 14.9 HAL_TIMER_IRQHandler / HAL_TIMER_UserIRQ

`HAL_TIMER_IRQHandler` clears the overflow and event flags and calls the
weak `HAL_TIMER_UserIRQ`. Both are SRAM-resident.

## 15. CCP driver

### 15.1 Features

- Six CCP instances: `CCP_0`..`CCP_5`.
- Four base modes: capture, compare, PWM (edge/center), and gate.
- Capture input selection (PIN0..PIN4, CMP0/CMP1, PINs-XOR).
- Compare and PWM duty/dead-time control.
- Gate start/end values and gate logic (AND/OR/XOR/replace).
- Force output control (channel, input source, LVD).
- Dynamic duty/gate update.

### 15.2 Configuration structures

`HAL_CCP_BaseInitTypedef` fields:

- `CCP`: pointer to `MODULE_CCP_Typedef`.
- `IntE`: interrupt enable.
- `Mode`: `HAL_CCP_ModeTypedef`.
- `Timer`: `HAL_CCP_BaseTimerTypedef` (TIMER0..TIMER7).
- `UpdateMode`: `HAL_CCP_UpdateModeTypedef`.
- `ReadReg`: `CCP_READ_REG_DIR` / `CCP_READ_REG_BUF`.

Additional Init structures: `HAL_CCP_CaptureInitTypedef`,
`HAL_CCP_CompareInitTypedef`, `HAL_CCP_PwmInitTypedef`,
`HAL_CCP_GateInitTypedef`, `HAL_CCP_ForceInitTypedef`, and
`HAL_CCP_UpdateTypedef` (dynamic duty/gate update).

### 15.3 HAL_CCP_Init

Validates the CCP instance. Configures interrupt enable, mode, base
timer, update mode, and read-register select. Enables the CCP as the
last step. Returns `HAL_OK` or `HAL_ERROR`.

### 15.4 HAL_CCP_CaptureInit / HAL_CCP_CompareInit /
### HAL_CCP_PwmInit / HAL_CCP_GateInit / HAL_CCP_ForceInit

Configure the capture, compare, PWM, gate, and force features
respectively. Each validates the CCP instance and writes the relevant
control registers. Returns `HAL_OK` or `HAL_ERROR`.

### 15.5 HAL_CCP_UpdateNow

Dynamically updates the duty and gate values. Temporarily switches to
user-trigger update mode, writes the new values, triggers the update,
then restores the original update mode.

### 15.6 HAL_CCP_WaitCf / HAL_CCP_WaitGf / HAL_CCP_WaitFf

Block until the capture/compare flag, gate flag, or force flag is set,
then clear it. On capture overflow, `HAL_CCP_HandleCov` (weak) is called
before clearing. All are SRAM-resident, weak.

### 15.7 HAL_CCP_CaptureValue

Returns the current capture value from `CCPCV`. Static inline.

### 15.8 HAL_CCP_DeInit

Resets all CCP registers to 0 and clears the capture, gate, and force
flags. Returns `HAL_OK` or `HAL_ERROR`.

### 15.9 HAL_CCP_IRQHandler / HAL_CCP_UserIRQ

`HAL_CCP_IRQHandler` clears the capture/overflow, gate, and force flags
if set, and calls the weak `HAL_CCP_UserIRQ`. Both are SRAM-resident.

## 16. FLASH driver

### 16.1 Features

- 256 KB external SPI-flash memory, two banks.
- Two code zones: Zone 0 (boot, 30 KB) and Zone 1 (user, 98 KB).
- Read, program, and erase operations via the on-chip flashloader.
- Quad-read mode for CPU instruction fetch (`HAL_FLASH_CPUQuadRead`).
- Dynamic clock-divider update based on `SystemClock`
  (`HAL_FLASH_ClkUpdate`).
- DMA line-read for aligned multi-line transfers.
- CRC32 verification of code zones with automatic bank-recovery
  (`HAL_FLASH_VerifyCode`, guarded by `HAL_FLASH_VERIFY_ENABLE`).

### 16.2 Configuration structures

`HAL_Flash_PacketTypedef` fields:

- `Data`: pointer to the data buffer.
- `Address`: flash address.
- `Length`: number of bytes.
- `DmaCh`: the shared DMA channel for DMA reads.

`HAL_Flash_ZoneTypedef` fields: `BankNum` and `ZoneNum`.

Flash constants: `FLASH_PAGE_SIZE` (256 B), `FLASH_SECTOR_SIZE` (4 KB),
`FLASH_SCLK_HIGH` (50 MHz threshold), `FLASH_USER_READ_MAX` (1024 B),
`FLASH_TIMEOUT_VALUE`, `HAL_FLASH_VERIFY_RETRY` (3).

### 16.3 HAL_FLASH_ClkUpdate

Selects the flash clock divider based on `SystemClock`: sclk/4 with
high TSHSL when `SystemClock > FLASH_SCLK_HIGH`, else sclk/2 with low
TSHSL. SRAM-resident. Returns `HAL_OK`.

### 16.4 HAL_FLASH_Init

1. Disable all interrupts (`HAL_HIC_DeInit`).
2. Configure the flashloader for standard (1-line) reads.
3. Update the flash clock divider.
4. Set default core-read latency, address cycles, data length, and
   operation type. Returns `HAL_OK`.

### 16.5 HAL_FLASH_Read

Reads data from flash. Uses the DMA line-read path when the length is a
multiple of 16 bytes, the buffer is word-aligned, and the line count is a
power of two between 2 and 256. Otherwise falls back to byte-by-byte
user-accessing reads (max `FLASH_USER_READ_MAX` bytes). Returns `HAL_OK`
or `HAL_ERROR`.

### 16.6 HAL_FLASH_Program

Programs data at the specified address. All data must reside within a
single 256-byte page. Sends full 32-bit words then any trailing 1..3
bytes. Returns `HAL_OK` or `HAL_ERROR`.

### 16.7 HAL_FLASH_Erase

Erases the specified address range. Address and length must be
page-aligned. Uses sector erase (4 KB) where possible, falling back to
page erase (256 B) for partial sectors. Returns `HAL_OK` or `HAL_ERROR`.

### 16.8 HAL_FLASH_UpdateCrc

Reads a flash zone in 1 KB blocks via DMA, computes a running CRC32,
and programs the result into the last 4 bytes of the zone. Uses a 1 KB
static DMA buffer; the caller must ensure sufficient memory. Returns
`HAL_OK` or `HAL_ERROR`.

### 16.9 HAL_FLASH_VerifyCode

Verifies the CRC32 of all four code zones (bank 0/1 x zone 0/1). On a
mismatch in one bank, copies the good zone to the bad bank
(`FlashCopyZone`) and retries. After `HAL_FLASH_VERIFY_RETRY` (3)
attempts, returns `HAL_ERROR` if still failing. This function overrides
the DMA and CRC configuration without recovery and requires a 1 KB data
buffer; run it without any other hardware module running.

### 16.10 HAL_FLASH_CPUQuadRead

Enables quad-read mode for CPU instruction fetch: sets the QE bit in
Status Register 2, then configures the core-accessing flash controller
for 4-line reads with the appropriate latency and address cycles.
Returns `HAL_OK` or `HAL_ERROR`.

## 17. CRC driver

### 17.1 Features

- Hardware CRC unit supporting 5/8/16/32-bit polynomials.
- Configurable polynomial, initial value, bit-reflection (in/out),
  output complement, and byte-reversal.
- Four input-data modes: write-CRC-in (software), capture-DMA-in,
  capture-DMA-out, and read-DMA.
- Single-byte, single-word, and packet operations.
- Default polynomial `CRC_POLY_DEFAULT` (0x04C11DB7) and expected result
  `CRC_GOOD_DEFAULT` (0x2144DF1C).

### 17.2 Configuration structures

`HAL_CRC_InitTypedef` fields: `Poly`, `InitVal`, `Bits`
(`HAL_CRC_BitsTypedef`), `RefIn`, `RefOut`, `CplOut`, `Rvb`, and
`DmaCh`.

`HAL_CRC_PacketTypedef` fields: `Data`, `Length` (bytes), `IntE`, and
`IDM` (`HAL_CRC_IDMTypedef`).

### 17.3 HAL_CRC_Init

Writes the polynomial, initial value, bit-length, reflection, complement,
and byte-reversal settings, and records the DMA channel. Returns `HAL_OK`
or `HAL_ERROR` if `Init` is NULL.

### 17.4 HAL_CRC_DeInit

Resets `CRCPOLY`, `CRCREG`, and `CRCCON` to 0 and clears the recorded DMA
channel. Returns `HAL_OK`.

### 17.5 HAL_CRC_OneByte / HAL_CRC_OneWord

Compute the CRC of a single byte or word (LSB first). Blocks until the
previous operation completes, then waits for the new result. SRAM-resident.

### 17.6 HAL_CRC_Packet

Computes the CRC of a data packet. In `READ_DMA` mode the CRC peripheral
acts as the initiating DMA source — the channel is bound in the DMA
controller and the source address in `CRCDA`; `CRCPDC` is NOT used.
In `CAPTURE_*` modes the CRC passively monitors a DMA channel, with
`CRCPDC` selecting which channel's data it listens to.
In polling mode, blocks and returns the CRC in `Result.Data`; in
interrupt mode, `Result.Data` is not valid and the result must be read
in the IRQ. Returns `HAL_ResultTypedef`.

### 17.7 HAL_CRC_IRQHandler / HAL_CRC_UserIRQ

`HAL_CRC_IRQHandler` calls the weak `HAL_CRC_UserIRQ`, clears the done
flag, disables the CRC interrupt, and updates the DMA channel state to
IDLE.

## 18. LVD driver

### 18.1 Features

- Low-voltage detection with 16 selectable trigger voltages
  (2.4 V to 4.7 V in 0.1 V steps).
- Modes: none, reset, wake, interrupt, and interrupt + wake.

### 18.2 Configuration structure

`HAL_LVD_InitTypedef` fields: `Mode` (`HAL_LVD_ModeTypedef`) and
`TrigV` (`HAL_LVD_TrigVTypedef`).

### 18.3 HAL_LVD_Init

Disables all LVD actions, sets the trigger voltage, enables LVD, clears
the stale flag, then configures the action mode. Returns `HAL_OK`.

### 18.4 HAL_LVD_DeInit

Disables all LVD actions and clears the flag. Returns `HAL_OK`.

### 18.5 HAL_LVD_IRQHandler / HAL_LVD_UserIRQ

`HAL_LVD_IRQHandler` de-initializes the LVD and calls the weak
`HAL_LVD_UserIRQ`.

## 19. VTS driver

### 19.1 Features

- Voltage/temperature sensor with alarm-reset control.
- Single-instance peripheral accessed directly via `VTSCON` / `ALMCON`.

### 19.2 HAL_VTS_Init

Enables the VTS alarm reset (`ALMCON->VTSARE`) and the VTS module
(`VTSCON->VTSE`). Returns `HAL_OK`.

### 19.3 HAL_VTS_DeInit

Disables the VTS alarm reset and the VTS module. Returns `HAL_OK`.

### 19.4 HAL_VTS_IRQHandler / HAL_VTS_UserIRQ

`HAL_VTS_IRQHandler` de-initializes the VTS and calls the weak
`HAL_VTS_UserIRQ`.

## 20. DAC driver

### 20.1 Features

- Two digital-to-analog converter instances (`DAC_0`, `DAC_1`).
- Configurable clock divider and input mode (latch or immediate).
- Single or repeated conversion: `Count` conversions in one call; non-DMA
  max 2, DMA max 1023. `Data` is a `uint16_t` buffer (32-bit aligned).
- DMA is engaged automatically when `Count > 2` or `IntE` is enabled;
  DMA modes are `DAC_DMA_ONCE` and `DAC_DMA_LOOP`.
- Interrupt on conversion completion (`__int28` serves both instances,
  dispatching on the `DACCF` flag).

### 20.2 Configuration structures

`HAL_DAC_InitTypedef` fields: `Dac` (pointer to a `MODULE_DAC_Typedef`),
`ClkDiv`, and `Mode` (`HAL_DAC_InputTypedef`).

`HAL_DAC_PacketTypedef` fields: `Dac`, `Data` (`uint16_t*`), `Count`,
`IntE`, and `Mode` (`HAL_DAC_DmaTypedef`).

### 20.3 HAL_DAC_Init

Sets the clock divider and input mode, clears the output buffer, then
enables the DAC. Returns `HAL_OK` or `HAL_ERROR` if the instance is
invalid.

### 20.4 HAL_DAC_DeInit

Clears the output buffer and disables the DAC. Returns `HAL_OK` or
`HAL_ERROR` if the instance is invalid.

### 20.5 HAL_DAC_Convert

Performs `Count` conversions. In interrupt mode (`IntE`) or DMA loop
mode it returns immediately; completion is handled by `HAL_DAC_IRQHandler`.
In polling non-loop mode it blocks until the conversion completes, clears
the done flag, releases the DMA channel, and returns `HAL_OK`.
SRAM-resident.

### 20.6 HAL_DAC_IRQHandler / HAL_DAC_UserIRQ

On conversion completion (`DACCF` set), `HAL_DAC_IRQHandler` calls the weak
`HAL_DAC_UserIRQ`, clears the done flag, disables the DMA data path,
releases the DMA channel, and disables the DAC interrupt once the channel
is idle.

## 21. CMP driver

### 21.1 Features

- Two comparator instances (`CMP_0`, `CMP_1`).
- Configurable hysteresis (none/x1/x2/x4), input filter (none/x1/x2/x3),
  input and reference sources, offset trim (+/-0..7), and a window timer
  (off..T7) with start/end values.
- Output enable, rise/fall flag enables, and an optional interrupt on the
  edge (`__int1` for `CMP_0`, `__int2` for `CMP_1`).

### 21.2 Configuration structure

`HAL_CMP_InitTypedef` fields: `Cmp` (`MODULE_CMP_Typedef`), `Hysteresis`,
`Filter`, `Input`, `Ref`, `RiseFlag`, `FallFlag`, `OutE`, `IntE`,
`Offset`, `WinTimer`, `WinStart`, and `WinEnd`.

### 21.3 HAL_CMP_Init

Packs the configured control and window values into the instance's `CMPxCON`
and `CMPxTW` registers. Returns `HAL_OK` or `HAL_ERROR` if the instance is
invalid.

### 21.4 HAL_CMP_DeInit

Clears the instance's window timer and control registers to 0. Returns
`HAL_OK` or `HAL_ERROR` if the instance is invalid.

### 21.5 HAL_CMP_OutState

Returns the comparator output state in `Result.Data` (via the instance's
`CMPS` flag) and `Result.Status = HAL_OK` on success. SRAM-resident.

### 21.6 HAL_CMP_IRQHandler / HAL_CMP_UserIRQ

On the edge interrupt, `HAL_CMP_IRQHandler` clears the instance's error
flag and calls the weak `HAL_CMP_UserIRQ`.

## 22. AMP driver

### 22.1 Features

- Three amplifier instances (`AMP_0`, `AMP_1`, `AMP_2`) sharing the
  `AMPCON` register.
- Configurable gain (open-loop, x1..x64) and offset trim (none..7).
- No interrupts; instances are enabled via `AMPE` in `AMPCON`.

### 22.2 Configuration structure

`HAL_AMP_InitTypedef` fields: `Amp` (`MODULE_AMP_Typedef`), `Gain`
(`HAL_AMP_GainTypedef`), and `Offset` (`HAL_AMP_OffsetTypedef`).

### 22.3 HAL_AMP_Init

Writes the gain, offset, and enable bits for the selected instance into its
`AMPCON` field (8 bits per instance), preserving the other instances.
Returns `HAL_OK` or `HAL_ERROR` if `Init` is NULL.

### 22.4 HAL_AMP_DeInit

Clears the selected instance's `AMPCON` field, preserving the other
instances. Returns `HAL_OK` or `HAL_ERROR` if `Init` is NULL.

## 23. MPWM driver

### 23.1 Features

- Three multi-PWM instances (`MPWM_0`, `MPWM_1`, `MPWM_2`), each with 8
  PWM "ways".
- PWM mode: set a per-way duty cycle (`HAL_MPWM_UpdateDuty`); the way is
  enabled by writing its `MPWxE` bit.
- LED serial mode: stream a 16-bit sample buffer over a way pair
  (`MPW0E`/`MPW1E`) using DMA (`HAL_MPWM_SerialLed`). The user must set the
  base-timer period so the LED pulse width matches the serial-LED spec.
- Base-timer select (`MPWM_BASE_TIMER0..7`), inverted-output enable, and
  per-instance interrupt enable (`__int25/26/27`).

### 23.2 Configuration structures

`HAL_MPWM_InitTypedef` fields: `Mpwm` (pointer to `MODULE_MPWM_Typedef`),
`IntE`, `Timer` (`HAL_MPWM_BaseTimerTypedef`), and `InvOutE`.

`HAL_MPWM_WayDutyTypedef` fields: `Mpwm`, `Way` (`HAL_MPWM_WayTypedef`),
and `Duty`.

`HAL_MPWM_PacketTypedef` fields: `Mpwm`, `Data` (`uint16_t*`, 32-bit
aligned), and `Length` (bytes, max 1023).

### 23.3 HAL_MPWM_Init

Writes `MPIE`, `MPT`, and `MPIO` from the init struct into the instance's
`MPCON`. Returns `HAL_OK` or `HAL_ERROR` if the instance is invalid.

### 23.4 HAL_MPWM_DeInit

Resets the instance's `MPCON` to 0. Returns `HAL_OK` or `HAL_ERROR` if the
instance is invalid.

### 23.5 HAL_MPWM_UpdateDuty

Sets the duty cycle for the selected way and enables that way (`MPWxE=1`).
Returns `HAL_OK` or `HAL_ERROR`. SRAM-resident.

### 23.6 HAL_MPWM_SerialLed

Transmits a serial-LED packet over a way pair via DMA. In interrupt mode
(`MPCON.MPIE` set) it returns immediately; completion is handled by
`HAL_MPWM_IRQHandler`. In polling mode it blocks on the way0 completion
flag `MPW0F`, clears it, and releases the DMA channel. SRAM-resident.

### 23.7 HAL_MPWM_IRQHandler / HAL_MPWM_UserIRQ

`HAL_MPWM_IRQHandler` clears each way's completion flag when set. It
treats `MPW0F` as the **authoritative** completion flag for the instance's
shared DMA channel: the channel is released (`DmaUpdate(..., OC32_DMA_IDLE)`)
only inside the `MPW0F` block. The LED protocol drives ways 0 and 1, so
way0 reaching `MPW0F` indicates the shared DMA transfer is finished; the
remaining way flags are cleared individually without touching the channel.
It then calls the weak `HAL_MPWM_UserIRQ`.

## 24. ADC driver

### 24.1 Features

- Two ADC instances (`ADC_0`, `ADC_1`), 8 event channels per instance
  (`ADC_EVENT0..7`), with per-channel input select, trigger source,
  priority, start-delay, sample cycles, watch windows, and interrupt enable.
- Configurable resolution (12/10/9/8-bit), data alignment (LSB/MSB),
  clock divider (div1..div8), reference source, and DMA channel.
- Four buffer modes: 256x2, 128x2, 64x2 (dual-buffered; first buffer is
  Event0, second is the active of Event1-7), and a combined 32-bit-per-event
  mode.
- Per-event watch windows (Watch0/Watch1, top/bottom limits) that can
  raise a `HAL_SPECIAL` result and generate interrupts.
- DMA engaged automatically when `Count > 1` or `IntE` is enabled
  (non-DMA max 1 conversion; DMA max 128).

### 24.2 Configuration structures

`HAL_ADC_InitTypedef` fields: `Adc` (pointer to `MODULE_ADC_Typedef`),
`ClkDiv`, `Res`, `Mode`, `Align`, `Ref`, `Pri` (start-delay priority),
`Unit` (start-delay unit), `Buf` (`uint32_t*` DMA buffer, 32-bit aligned,
may be NULL in non-DMA mode), `Watch0Top`/`Watch0Bottom`,
`Watch1Top`/`Watch1Bottom`, and `DmaCh`.

`HAL_ADC_EventTypedef` fields: `Adc`, `Channel`, `Input`, `Source`, `Pri`
(event priority), `Delay` (start-delay count), `IntE`, `Cycle` (sample
cycles), `Count`, `Watch0E`, and `Watch1E`.

### 24.3 HAL_ADC_Init

Records the per-instance DMA channel, then writes the control, watch-window,
and data-address registers. Returns `HAL_OK` or `HAL_ERROR` if `Init` is
NULL or the instance is invalid.

### 24.4 HAL_ADC_DeInit

Resets the instance's `ADCCON` to 0 and releases the recorded DMA channel
(`DmaUpdate(..., OC32_DMA_IDLE)`), which returns the channel status.

### 24.5 HAL_ADC_EventConvert

Configures and starts a per-event conversion. In DMA mode (`Count > 1`) or
interrupt mode (`IntE`) it sets up the DMA and, in interrupt mode, returns
immediately. In polling mode it blocks on the event's completion flag
(`ADCECF`), checks the overflow bit, and sets `Result.Status` to
`HAL_SPECIAL` if a watch window was hit; `Result.Data` carries the 12-bit
result for non-DMA mode (it is not meaningful in DMA mode).
SRAM-resident. Returns `HAL_ResultTypedef`.

### 24.6 HAL_ADC_IRQHandler / HAL_ADC_UserIRQ

`HAL_ADC_IRQHandler` scans all 8 events; for each event whose overflow flag
is set, conversion is complete, or a watch window was hit, it calls the weak
`HAL_ADC_UserIRQ`, clears the overflow/done flags and the watch clear bits
(the `ADCWE0*` position macros are shared by `ADCWC0` and `ADCWC1` because
the two registers use identical bit layouts), clears the DMA-enable bits,
and releases the channel. The return value is a **packed per-channel status
word**: bit *Event* holds the result of `HAL_ADC_UserIRQ(Adc) << Event`, so
a single word reports the outcome of all 8 channels at once.

## 25. How to use the HAL drivers

The typical application sequence is:

1. Call `HAL_SYS_Init` to bring up the clock, flash, debug UART, and tick.
2. Call `HAL_HIC_Init` with a `HAL_HICInitTypedef` to enable the interrupt
   channels and set their priorities (this activates the system tick).
3. For each peripheral to use:
   a. Fill the peripheral's Init structure.
   b. Call `HAL_PPP_Init` to configure the hardware.
   c. Use the operation APIs (`HAL_PPP_TX`, `HAL_PPP_Transfer`,
      `HAL_PPP_Read`, etc.) in polling or interrupt mode.
   d. For interrupt mode, ensure the `HAL_PPP_UserIRQ` hook (or a
      user-defined override) is in place before enabling the interrupt.
4. When a peripheral is no longer needed, call `HAL_PPP_DeInit`.

### DMA mode notes

- Assign a non-conflicting `DmaCh` value in each Init struct; the HAL does
  not arbitrate between modules.
- For UART and SPI, DMA is engaged automatically when the transfer length
  exceeds 4 bytes or when `IntE` is `HAL_ENABLE`.
- For IIC, DMA is engaged when the packet length exceeds 4 bytes.
- Flash and CRC internal operations use `OC32_DMA_CH0` by default; do not
  run user modules that also claim CH0 concurrently with
  `HAL_FLASH_VerifyCode`.

### Interrupt-mode notes

- In polling mode, the caller must not mask the peripheral interrupt
  during the blocking wait, or the busy-loop will not terminate.
- In interrupt mode, the operation API returns immediately; completion is
  signalled by the `HAL_PPP_IRQHandler`, which in turn calls the
  `__weak__` `HAL_PPP_UserIRQ` hook.

## 26. Source file reference

| File | Description |
|------|-------------|
| `oc32_hal.c` | Common HAL: tick, HIC, delay, UID, SYS_Init, DMA_Update. |
| `oc32_hal_gpio.c` | GPIO driver. |
| `oc32_hal_rcm.c` | RCM / clock manager driver. |
| `oc32_hal_wdt.c` | WDT driver. |
| `oc32_hal_uart.c` | UART driver. |
| `oc32_hal_spi.c` | SPI driver. |
| `oc32_hal_iic.c` | IIC driver. |
| `oc32_hal_timer.c` | TIMER driver. |
| `oc32_hal_ccp.c` | CCP driver. |
| `oc32_hal_flash.c` | FLASH driver. |
| `oc32_hal_crc.c` | CRC driver. |
| `oc32_hal_lvd.c` | LVD driver. |
| `oc32_hal_vts.c` | VTS driver. |
| `oc32_hal_dac.c` | DAC driver. |
| `oc32_hal_cmp.c` | CMP driver. |
| `oc32_hal_amp.c` | AMP driver. |
| `oc32_hal_mpwm.c` | MPWM (multi-PWM / serial-LED) driver. |
| `oc32_hal_adc.c` | ADC driver. |
| `oc32_hal_int.c` | C interrupt vectors (int0..int31). |
| `oc32_hal_int_asm.S` | Assembly interrupt vectors (`__asm_int0`..`__asm_int31`). |

Headers: `oc32_hal.h`, `oc32_hal_def.h`, `oc32_hal_conf.h` (and the
peripheral headers `oc32_hal_<periph>.h`). Device headers:
`CV32S6015SR.h` and `CV32S6015MSR.h`.

