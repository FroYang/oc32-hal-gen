
## 3. 串口 UART（`oc32_hal_uart.c` / `.h`，`HAL_UART_ENABLE`）

UART0/UART1 多实例，经 `MODULE_UART_TypeDef *` 区分。

- 初始化：`HAL_UART_Init`（含 `Baud==0` 与 `bitcycles<8` 防护），`HAL_UART_DeInit`。
- 收发：`HAL_UART_TX` / `HAL_UART_RX`（包结构，轮询 / 中断双模，`Length>4` 才自动开 DMA；收发各 4B 快路径 + DMA 余量）。`Length` 单位**字节**。
- 中断：`HAL_UART_IRQHandler`（完成清 flag、关 DMA、`DMAChState` 置 IDLE，idle 才关 `UTIE`）；`HAL_UART_UserIRQ`（`__weak__`）。
- 向量：`__int20`→UART0、`__int21`→UART1（已接 IRQHandler）。

## 4. SPI（`oc32_hal_spi.c` / `.h`，`HAL_SPI_ENABLE`）

SPI0/SPI1 多实例。

- 初始化：`HAL_SPI_Init`（Mode/ClkDiv/主从/CS/双工/半工引脚/首位/引脚），`HAL_SPI_DeInit`。
- 收发：`HAL_SPI_Transfer`（单一 API，轮询 / 中断双模，`Length>4` 才开 DMA；`SPITBC` 单一 TBC 计数器——全双工共用、半双工每次单向，不需要两个计数器）。
- 中断：`HAL_SPI_IRQHandler`；`HAL_SPI_UserIRQ`（`__weak__`）。向量 `__int22`/`__int23`。

## 5. IIC（`oc32_hal_iic.c` / `.h`，`HAL_IIC_ENABLE`）

单实例外设，直接裸寄存器操作（`IICCON` + `IICTBUF/IICRBUF/IICADR/IICDA/IICBC`），不引入 `MODULE_IIC_TypeDef`。

- 初始化：`HAL_IIC_Init`（AddrMode 7/10-bit、ClkDiv、Stretch、Pins、DmaCh），`HAL_IIC_DeInit`。
- 主/从传输：`HAL_IIC_Transfer`（状态机 `IicTransStm`：IDLE→START→ADDR→DATA→STOP；轮询 / 中断双模，`Length>4` 开 DMA）。
- 中断：`HAL_IIC_IRQHandler`（读模块级静态 `IicCurPacket` 继续驱动状态机，带 NULL 防御）；`HAL_IIC_UserIRQ`（`__weak__`）。向量 `__int24`。
- 地址打包：`IicSetAddress`（10-bit 把高 2 位+header+R/W 拼进 16-bit `IICADR`；7-bit 左移 1 位）。
- 错误检测：`IicHandshakeCheck`（NACK）、`IicClockTimeOutCheck`（SCL 拉低超时）；`IicSlaveStart` 轮询 `while(1)` 仅靠 clock-timeout 兜底（外设 busy-wait 不加独立超时的库约定）。

## 6. 定时器 / CCP / WDT（`oc32_hal_{timer,ccp,wdt}.c`，`HAL_*_ENABLE`）

- **Timer**（8 路 `TIMER_0~7`）：`HAL_TIMER_Init` + 编码/复位/暂停/事件各 Init；`UpdateNow`/`ResetNow`/`EventTrig`/`WaitOv`/`WaitEv`（`__weak__`）；`HAL_TIMER_IRQHandler`。向量 `__int9~16`。
- **CCP**（6 路 `CCP_0~5`）：`HAL_CCP_Init` + Capture/Compare/Pwm/Gate/Force 各 Init；`WaitCf/Gf/Ff`、`UpdateNow`（`__weak__`）；`HAL_CCP_IRQHandler`。向量 `__int3~8`。
- **WDT**（2 路 `WDT_0/1`）：`HAL_WDT_Init`（自动选最快 8-bit 重载预分频，`WDT_PERIOD_LIMIT_MARGIN` 边界 +1/−1µs 防重载值取 0）；`HAL_WDT_Clear`（喂狗）；`HAL_WDT_IRQHandler`。`WDT_1` 由 `HAL_TICK_Init` 当 1kHz tick 用。向量 `__int17`。

## 7. 比较器 / 运放 / 电压温度（`oc32_hal_{cmp,amp,vts}.c`，`HAL_*_ENABLE`）

- **CMP**（`CMP_0/1`）：`HAL_CMP_Init`、`HAL_CMP_OutState`、`HAL_CMP_IRQHandler`（`__weak__` 含 `UserIRQ`）。向量 `__int1/2`。
- **AMP**：`HAL_AMP_Init` / `HAL_AMP_DeInit`（复用 Init 入参定位外设）。
- **VTS**：`HAL_VTS_IRQHandler`（`__weak__`，`UserIRQ`）。

## 8. Flash / CRC（`oc32_hal_flash.c` / `oc32_hal_crc.c`，`HAL_FLASH_ENABLE` 等）

**Flash**（SPI Flashloader + DMA，全字节单位）：
- 初始化：`HAL_FLASH_Init`（内部 `HAL_HIC_DeInit` 关全中断）、`HAL_FLASH_ClkUpdate`（按 `SystemClock` 调分频）。
- 读：`HAL_FLASH_Read`（16B 倍数 + 4B 对齐 + 2 的幂行走 DMA 快读，否则逐字节；上限 `FLASH_USER_READ_MAX`=1024B）。
- 写：`HAL_FLASH_Program`（同页约束；整 4B word 走快路径 + 余 1~3 字节逐字节，LSB-first；`Length<4` 时快路径循环 0 次自动跳余量段）。
- 擦除：`HAL_FLASH_Erase`（4KB 扇区 / 256B 页）。
- 校验：`HAL_FLASH_VerifyCode`（2 bank×2 zone，坏 bank 从好 bank 复制，需 ~1KB 静态 buffer，调用时独占 DMA/CRC）。

**CRC**（`HAL_CRC_ENABLE`）：
- 计算：`HAL_CRC_OneByte` / `OneWord` / `HAL_CRC_Packet`（5 种 `IDM`：`WRITE_CRCIN`/`CAPTURE_DMAIN`/`CAPTURE_DMAOUT`/`READ_DMA`；`READ_DMA` 由 CRC 主动发起、不用 `CRCPDC`，捕获型才用 `CRCPDC` 指定监听通道）。
- 中断：`HAL_CRC_IRQHandler`；`__int29`。

## 9. LVD（`oc32_hal_lvd.c`，`HAL_LVD_ENABLE`）

`HAL_LVD_Init`（模式 RST/INT/WAKE/INT_AND_WAKE + 触发电压阈值）、`HAL_LVD_DeInit`。触发经 `__int0`（HRF 向量，`LVDCON->LVDIE && LVDEF` 时调 `HAL_LVD_IRQHandler`）。

## 10. 系统 / 中断 / DMA / 调试（`oc32_hal.c` + `oc32_hal_int.c`）

- 系统：`HAL_SYS_Init`（`__weak__`：RCM→Flash→校验→调试UART→tick）、`HAL_TICK_Init`（仅配 WDT tick 参数，**不使能 HIE**，须用户 `HAL_HIC_Init` 后 tick 才真正跑）、`HAL_HIC_Init` / `HAL_HIC_DeInit`（DeInit 清全 HIE 含 WDT 位，tick 随之停）、`HAL_Delay`。
- 裸机：`memcpy`/`memset`/`puts`/`printf`（`__weak__`，`#ifdef NO_STDLIB` 编译；`printf` 支持 `%d %u %x %X %s %c %p %%`，32-bit、不支持 64-bit/长度修饰符）。
- DMA：`DMAChState[]` + `DmaUpdate`（通道占用记账，库不仲裁——用户负责通道分配与串行化；Flash/CRC 排他占 CH0）。
- 中断向量表（`oc32_hal_int.c`）：`__int0`（HRF/POR/LVD/PLL，**故意留 Flash 非 SRAM**，物理环境故障时取指最可靠）；`__int1~31` 均 `__sram__`。已接 handler：0/1/2/3-8/9-16/17/20-21/22-23/24/28/29/30/31。`__int18/19`（ADC）、`__int25~27`（MPWM）为骨架占位（对应 HAL 未实现，body 只 EIA/HIPRM 保存恢复）。

## 11. 生成器 / 配置

- `oc32_hal_conf.h` + `oc32_hal_conf_template.h`：由 `OC32 HAL Config Generator` 依芯片型号生成（时钟源、Flash 分区、模块 `HAL_*_ENABLE` 开关、`OC32_HIC_*` 通道位等）。切芯片重新生成，不手改。
- `hal/doc/OC32_HAL_USER_MANUAL.md`：面向应用开发者手册（快速开始、各外设用法、中断向量表、DMA 独占约定、`Length` 字节单位说明等）。
- `hal/chip/CV32S6015MSR.h` / `CV32S6015SR.h`：寄存器定义（MSR 指针型 + 解引用型宏 `READ_SR`/`WRITE_SR`，SR 位掩码）。

## 待办 / 占位

- `__int18/19`（ADC）、`__int25~27`（MPWM）handler 占位，待对应 HAL 模块实现后按 `__int24`（IIC）/`__int28`（DAC）模式补调用。
- 部分 `REG.X= VAL` 等号前缺空格（风格统一项，建议格式化工具一次性规整）。
